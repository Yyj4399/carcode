"""
MobileNet V2 分类模型训练脚本
- 自动检测类别数
- Alpha = 0.5（可调节）
- Int8 量化输出
- 混淆矩阵可视化
- 内存占用约 1000kb
- 支持两种训练模式：
  1. 交互式模式（默认）：每轮询问是否继续和参数（epoch、学习率、batch_size、数据增强）
  2. 自动模式（--auto_continue）：完全不询问，使用命令行参数
- 支持动态调整批次大小和开启/关闭数据增强
"""

import os
import sys
import argparse
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import confusion_matrix, classification_report
import pathlib

# ==================== 配置参数 ====================
class Config:
    # 数据集路径
    TRAIN_DIR = '/mnt/image/train'
    TEST_DIR = '/mnt/image/test'

    # 模型参数
    ALPHA = 0.5  # MobileNetV2 的宽度乘数（可调节：0.35, 0.5, 0.75, 1.0）
    INPUT_SHAPE = (128, 128, 3)  # 输入图像大小

    # 训练参数
    BATCH_SIZE = 32
    EPOCHS = 50
    EPOCHS_PER_ROUND = 10  # 每轮训练的epoch数（训练后会询问是否继续）
    LEARNING_RATE = 0.0001

    # 模型保存路径
    MODEL_SAVE_PATH = 'mobilenetv2_model.h5'
    TFLITE_MODEL_PATH = 'mobilenetv2_int8.tflite'
    CONFUSION_MATRIX_PATH = 'confusion_matrix.png'
    CHECKPOINT_PATH = 'mobilenetv2_checkpoint.h5'  # 训练中间检查点


# ==================== GPU配置 ====================
def setup_gpu():
    """
    配置GPU设置
    - 检测可用GPU
    - 设置内存增长以避免显存不足
    - 显示GPU信息
    """
    print("\n[GPU配置]")

    # 获取所有可用的GPU
    gpus = tf.config.list_physical_devices('GPU')

    if not gpus:
        print("  ⚠ 未检测到GPU，将使用CPU训练")
        print("  提示: 如果您已安装GPU版本的TensorFlow，请检查CUDA和cuDNN配置")
        return False

    print(f"  ✓ 检测到 {len(gpus)} 个GPU:")

    try:
        # 设置GPU内存增长
        for i, gpu in enumerate(gpus):
            tf.config.experimental.set_memory_growth(gpu, True)

            # 获取GPU详细信息
            gpu_details = tf.config.experimental.get_device_details(gpu)
            gpu_name = gpu_details.get('device_name', 'Unknown GPU')

            print(f"    GPU {i}: {gpu_name}")
            print(f"    设备: {gpu.name}")

        print(f"\n  GPU内存增长已启用（动态分配显存）")

        # 设置默认使用的GPU（如果有多个）
        if len(gpus) > 1:
            print(f"  使用GPU 0进行训练")
            tf.config.set_visible_devices(gpus[0], 'GPU')

        return True

    except RuntimeError as e:
        print(f"  ✗ GPU配置失败: {e}")
        print(f"  将使用CPU训练")
        return False


# ==================== 自动检测类别数 ====================
def get_num_classes(data_dir):
    """自动检测数据集中的类别数"""
    if not os.path.exists(data_dir):
        raise ValueError(f"数据集路径不存在: {data_dir}")

    classes = [d for d in os.listdir(data_dir)
               if os.path.isdir(os.path.join(data_dir, d))]
    num_classes = len(classes)

    print(f"检测到 {num_classes} 个类别: {classes}")
    return num_classes, classes


# ==================== 验证和清理数据集 ====================
def validate_and_clean_dataset(data_dir, target_size=(128, 128)):
    """
    验证数据集中的所有图像文件，删除损坏的图像
    完全模拟 ImageDataGenerator 的加载流程，包括 resize 和转换

    参数：
        data_dir: 数据集根目录
        target_size: 目标图像尺寸，用于测试 resize 操作

    返回：
        (valid_count, corrupted_count): 有效图像数量和损坏图像数量
    """
    from PIL import Image
    import numpy as np

    print(f"\n[验证数据集] {data_dir}")
    print(f"  目标尺寸: {target_size}")

    valid_count = 0
    corrupted_count = 0
    corrupted_files = []

    # 支持的图像格式
    valid_extensions = {'.jpg', '.jpeg', '.png', '.bmp', '.gif', '.tiff', '.webp'}

    # 遍历所有类别文件夹
    if not os.path.exists(data_dir):
        print(f"  警告: 路径不存在 {data_dir}")
        return 0, 0

    for class_name in os.listdir(data_dir):
        class_path = os.path.join(data_dir, class_name)

        if not os.path.isdir(class_path):
            continue

        print(f"  检查类别: {class_name}")
        class_valid = 0
        class_corrupted = 0

        # 遍历该类别下的所有文件
        for filename in os.listdir(class_path):
            file_path = os.path.join(class_path, filename)

            # 跳过子目录
            if os.path.isdir(file_path):
                continue

            # 检查文件扩展名
            _, ext = os.path.splitext(filename.lower())
            if ext not in valid_extensions:
                continue

            # 尝试完整的图像加载流程（模拟 ImageDataGenerator）
            try:
                # 1. 打开图像文件
                img = Image.open(file_path)

                # 2. 转换为 RGB（ImageDataGenerator 默认行为）
                if img.mode != 'RGB':
                    img = img.convert('RGB')

                # 3. Resize 到目标尺寸（这是最容易出错的步骤）
                img_resized = img.resize(target_size, Image.BILINEAR)

                # 4. 转换为 numpy 数组（模拟实际使用）
                img_array = np.array(img_resized, dtype=np.float32)

                # 5. 检查数组形状是否正确
                if img_array.shape != (target_size[0], target_size[1], 3):
                    raise ValueError(f"Invalid image shape: {img_array.shape}")

                # 6. 检查是否有无效值（NaN 或 Inf）
                if np.isnan(img_array).any() or np.isinf(img_array).any():
                    raise ValueError("Image contains NaN or Inf values")

                # 关闭图像
                img.close()

                valid_count += 1
                class_valid += 1

            except Exception as e:
                corrupted_count += 1
                class_corrupted += 1
                corrupted_files.append(file_path)

                # 删除损坏的文件
                try:
                    os.remove(file_path)
                    print(f"    ✗ 已删除损坏文件: {filename}")
                    print(f"      错误类型: {type(e).__name__}")
                    print(f"      错误信息: {str(e)[:100]}")
                except Exception as delete_error:
                    print(f"    ✗ 无法删除损坏文件: {filename}")
                    print(f"      删除错误: {delete_error}")

        if class_valid > 0 or class_corrupted > 0:
            print(f"    类别 {class_name}: 有效 {class_valid}, 损坏 {class_corrupted}")

    print(f"\n  验证完成:")
    print(f"    有效图像: {valid_count}")
    print(f"    损坏图像: {corrupted_count} (已删除)")

    if corrupted_files:
        print(f"\n  损坏文件列表:")
        for f in corrupted_files[:10]:  # 只显示前10个
            print(f"    - {f}")
        if len(corrupted_files) > 10:
            print(f"    ... 还有 {len(corrupted_files) - 10} 个文件")

    return valid_count, corrupted_count


# ==================== 数据加载 ====================
def load_data(config, use_augmentation=False):
    """
    加载训练和测试数据集

    参数：
        config: 配置对象
        use_augmentation: 是否使用数据增强（默认False）
    """

    if use_augmentation:
        # 检查路径第二级目录名是否为 "num"
        path_parts = pathlib.Path(config.TRAIN_DIR).parts
        second_level_dir = path_parts[1].lower() if len(path_parts) > 1 else ""

        # 如果路径第二级为 "num"，则默认仅使用随机曝光（避免重复旋转）
        # 但用户可以通过设置环境变量 FULL_AUGMENTATION=1 来强制使用完整增强
        use_full_augmentation = os.environ.get('FULL_AUGMENTATION', '0') == '1'

        if second_level_dir == "num" and not use_full_augmentation:
            # 路径第二级为 "num"，只使用随机曝光增强
            print("  使用数据增强（仅随机曝光）")
            print("  提示: 可设置环境变量 FULL_AUGMENTATION=1 来使用完整增强")
            train_datagen = ImageDataGenerator(
                rescale=1./255,
                brightness_range=[0.5, 1.5]  # 仅随机曝光，模拟光线不均匀环境
            )
        else:
            # 使用数据增强（全方位旋转、水平垂直翻转、随机曝光）
            if second_level_dir == "num":
                print("  使用数据增强（完整增强 - 检测到用户强制设置）")
            else:
                print("  使用数据增强（全方位旋转、水平垂直翻转、随机曝光）")
            train_datagen = ImageDataGenerator(
                rescale=1./255,
                rotation_range=360,      # 全方位旋转覆盖
                horizontal_flip=True,    # 水平翻转
                vertical_flip=True,      # 垂直翻转
                brightness_range=[0.5, 1.5]  # 随机曝光，模拟光线不均匀环境
            )
    else:
        # 无数据增强，仅归一化
        print("  不使用数据增强")
        train_datagen = ImageDataGenerator(rescale=1./255)

    # 测试数据始终不使用数据增强
    test_datagen = ImageDataGenerator(rescale=1./255)

    # 加载训练数据
    train_generator = train_datagen.flow_from_directory(
        config.TRAIN_DIR,
        target_size=config.INPUT_SHAPE[:2],
        batch_size=config.BATCH_SIZE,
        class_mode='categorical',
        shuffle=True
    )

    # 加载测试数据
    test_generator = test_datagen.flow_from_directory(
        config.TEST_DIR,
        target_size=config.INPUT_SHAPE[:2],
        batch_size=config.BATCH_SIZE,
        class_mode='categorical',
        shuffle=False
    )

    return train_generator, test_generator


# ==================== 构建模型 ====================
def build_model(num_classes, config):
    """构建 MobileNetV2 分类模型"""

    # 加载预训练的 MobileNetV2（不包含顶层）
    base_model = MobileNetV2(
        input_shape=config.INPUT_SHAPE,
        alpha=config.ALPHA,
        include_top=False,
        weights='imagenet'
    )

    # 冻结基础模型的部分层（可选）
    base_model.trainable = True

    # 构建完整模型
    inputs = keras.Input(shape=config.INPUT_SHAPE)
    x = base_model(inputs, training=False)
    x = layers.GlobalAveragePooling2D()(x)
    x = layers.Dropout(0.2)(x)
    outputs = layers.Dense(num_classes, activation='softmax')(x)

    model = keras.Model(inputs, outputs)

    return model


# ==================== Int8 量化 ====================
def convert_to_int8_tflite(model, train_generator, config):
    """
    将模型转换为 int8 量化的 TFLite 格式

    使用完整的 int8 量化策略，包括：
    - 权重和激活值的 int8 量化
    - 代表性数据集校准
    - 输入输出量化设置
    """

    print("\n[Int8 量化转换]")
    print("  收集代表性数据集用于量化校准...")

    # 收集代表性数据集（用于量化校准）
    representative_data = []
    num_calibration_steps = min(100, len(train_generator))  # 最多100批或全部训练数据

    for i in range(num_calibration_steps):
        data, _ = next(train_generator)
        # 只取每批的前几个样本以节省时间
        for sample in data[:min(5, len(data))]:
            representative_data.append(sample.astype(np.float32))
            if len(representative_data) >= 100:  # 收集100个样本即可
                break
        if len(representative_data) >= 100:
            break

    print(f"  已收集 {len(representative_data)} 个校准样本")

    # 创建转换器
    converter = tf.lite.TFLiteConverter.from_keras_model(model)

    # 设置优化选项为默认量化
    converter.optimizations = [tf.lite.Optimize.DEFAULT]

    # 提供代表性数据集用于量化校准
    def representative_dataset():
        for sample in representative_data:
            yield [np.expand_dims(sample, axis=0)]

    converter.representative_dataset = representative_dataset

    # 设置完整的 int8 量化
    # 1. 支持的操作集合
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

    # 2. 强制执行完整的整数量化（包括输入输出）
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8

    # 3. 实验性选项：启用 MLIR 量化器（减少警告）
    converter.experimental_new_quantizer = True

    print("  开始量化转换...")

    # 执行转换
    try:
        tflite_model = converter.convert()
        print("  ✓ 量化转换成功")
    except Exception as e:
        print(f"  ✗ 量化转换失败: {e}")
        print("  尝试使用备用量化策略...")

        # 备用策略：不强制 int8 输入输出
        converter.inference_input_type = tf.float32
        converter.inference_output_type = tf.float32
        tflite_model = converter.convert()
        print("  ✓ 使用备用策略转换成功（输入输出为 float32）")

    # 保存模型
    with open(config.TFLITE_MODEL_PATH, 'wb') as f:
        f.write(tflite_model)

    # 显示模型大小
    model_size_kb = len(tflite_model) / 1024
    print(f"\n  量化后的模型大小: {model_size_kb:.2f} KB")
    print(f"  模型已保存到: {config.TFLITE_MODEL_PATH}")

    return tflite_model


# ==================== 训练模型 ====================
def train_model(model, train_generator, test_generator, config):
    """训练模型"""

    # 编译模型
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=config.LEARNING_RATE),
        loss='categorical_crossentropy',
        metrics=['accuracy']
    )

    # 回调函数
    callbacks = [
        keras.callbacks.ModelCheckpoint(
            config.CHECKPOINT_PATH,
            save_best_only=True,
            monitor='val_accuracy',
            verbose=1
        ),
        keras.callbacks.EarlyStopping(
            monitor='val_loss',
            patience=10,
            restore_best_weights=True
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss',
            factor=0.5,
            patience=5,
            verbose=1
        )
    ]

    # 训练
    history = model.fit(
        train_generator,
        epochs=config.EPOCHS,
        validation_data=test_generator,
        callbacks=callbacks,
        verbose=1
    )

    return history


# ==================== 分阶段训练模型 ====================
def train_model_incremental(model, train_generator, test_generator, epochs, learning_rate, checkpoint_path):
    """
    分阶段训练模型

    参数：
        model: 要训练的模型
        train_generator: 训练数据生成器
        test_generator: 测试数据生成器
        epochs: 本次训练的轮数
        learning_rate: 当前学习率
        checkpoint_path: 检查点保存路径
    """

    # 更新优化器学习率
    keras.backend.set_value(model.optimizer.learning_rate, learning_rate)

    print(f"\n  当前学习率: {learning_rate}")
    print(f"  本轮训练轮数: {epochs}")

    # 回调函数
    callbacks = [
        keras.callbacks.ModelCheckpoint(
            checkpoint_path,
            save_best_only=True,
            monitor='val_accuracy',
            verbose=1
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss',
            factor=0.5,
            patience=5,
            verbose=1
        )
    ]

    # 训练
    history = model.fit(
        train_generator,
        epochs=epochs,
        validation_data=test_generator,
        callbacks=callbacks,
        verbose=1
    )

    return history


# ==================== 用户交互：询问是否继续训练 ====================
def ask_continue_training():
    """询问用户是否继续训练"""
    print("\n" + "=" * 60)
    print("【训练暂停】")
    print("=" * 60)

    while True:
        choice = input("\n是否继续训练？(y/n): ").strip().lower()
        if choice in ['y', 'yes', '是', 'Y']:
            return True
        elif choice in ['n', 'no', '否', 'N']:
            return False
        else:
            print("  无效输入，请输入 y 或 n")


# ==================== 用户交互：询问继续训练的参数 ====================
def ask_next_training_parameters(current_lr, current_batch_size, current_use_augmentation, train_dir=""):
    """
    询问用户下一轮训练的参数

    参数：
        current_lr: 当前学习率
        current_batch_size: 当前批次大小
        current_use_augmentation: 当前是否使用数据增强
        train_dir: 训练集路径（用于检查第二级目录）

    返回：
        (new_lr, new_epochs, new_batch_size, new_use_augmentation): 新的学习率、轮数、批次大小和是否使用数据增强
    """
    print("\n" + "-" * 60)
    print("【继续训练参数设置】")
    print("-" * 60)

    # 输入训练轮数（必须）
    while True:
        try:
            epochs_input = input(f"\n输入本轮要训练的epoch数 (例如: 10): ").strip()
            if epochs_input == "":
                print("  请输入要训练的epoch数")
                continue
            new_epochs = int(epochs_input)
            if new_epochs > 0:
                break
            else:
                print("  训练轮数必须大于0")
        except ValueError:
            print("  无效输入，请输入整数")

    # 询问是否调整学习率
    print(f"\n  当前学习率: {current_lr}")
    while True:
        choice = input("是否调整学习率？(y/n): ").strip().lower()
        if choice in ['y', 'yes', '是', 'Y']:
            # 调整学习率
            while True:
                try:
                    lr_input = input(f"输入新的学习率 (当前: {current_lr}): ").strip()
                    if lr_input == "":
                        print("  请输入新的学习率")
                        continue
                    new_lr = float(lr_input)
                    if new_lr > 0:
                        break
                    else:
                        print("  学习率必须大于0")
                except ValueError:
                    print("  无效输入，请输入数字")
            break
        elif choice in ['n', 'no', '否', 'N']:
            new_lr = current_lr
            break
        else:
            print("  无效输入，请输入 y 或 n")

    # 询问是否调整batch_size
    print(f"\n  当前批次大小: {current_batch_size}")
    while True:
        choice = input("是否调整批次大小？(y/n): ").strip().lower()
        if choice in ['y', 'yes', '是', 'Y']:
            # 调整batch_size
            while True:
                try:
                    bs_input = input(f"输入新的批次大小 (当前: {current_batch_size}): ").strip()
                    if bs_input == "":
                        print("  请输入新的批次大小")
                        continue
                    new_batch_size = int(bs_input)
                    if new_batch_size > 0:
                        break
                    else:
                        print("  批次大小必须大于0")
                except ValueError:
                    print("  无效输入，请输入整数")
            break
        elif choice in ['n', 'no', '否', 'N']:
            new_batch_size = current_batch_size
            break
        else:
            print("  无效输入，请输入 y 或 n")

    # 询问是否开启/关闭数据增强
    augmentation_status = "已开启" if current_use_augmentation else "未开启"
    print(f"\n  当前数据增强: {augmentation_status}")
    while True:
        if current_use_augmentation:
            choice = input("是否关闭数据增强？(y/n): ").strip().lower()
            if choice in ['y', 'yes', '是', 'Y']:
                new_use_augmentation = False
                break
            elif choice in ['n', 'no', '否', 'N']:
                new_use_augmentation = True
                break
            else:
                print("  无效输入，请输入 y 或 n")
        else:
            choice = input("是否开启数据增强？(y/n): ").strip().lower()
            if choice in ['y', 'yes', '是', 'Y']:
                new_use_augmentation = True
                # 扫描路径第二级目录，提示用户增强类型
                if train_dir:
                    path_parts = pathlib.Path(train_dir).parts
                    second_level_dir = path_parts[1].lower() if len(path_parts) > 1 else ""
                    if second_level_dir == "num":
                        print("\n  ⚠ 检测到路径第二级为 'num'")
                        print("  默认将使用【随机曝光增强】（避免重复旋转）")
                        print("  若想使用完整增强（旋转+翻转+曝光），请在本轮完成后重新启动训练并添加 --full_augmentation 参数")
                    else:
                        print(f"\n  ✓ 检测到路径第二级为 '{second_level_dir}'")
                        print("  将使用【完整增强】（旋转+翻转+曝光）")
                break
            elif choice in ['n', 'no', '否', 'N']:
                new_use_augmentation = False
                break
            else:
                print("  无效输入，请输入 y 或 n")

    print(f"\n  本轮训练设置:")
    print(f"    训练轮数: {new_epochs}")
    print(f"    学习率: {new_lr}")
    print(f"    批次大小: {new_batch_size}")
    print(f"    数据增强: {'开启' if new_use_augmentation else '关闭'}")

    return new_lr, new_epochs, new_batch_size, new_use_augmentation


# ==================== 显示训练历史摘要 ====================
def print_training_summary(all_histories, total_epochs):
    """打印训练历史摘要"""
    print("\n" + "=" * 60)
    print("【训练历史摘要】")
    print("=" * 60)
    print(f"  总训练轮数: {total_epochs}")
    print(f"  训练阶段数: {len(all_histories)}")

    # 获取最佳结果
    best_val_acc = 0
    best_epoch = 0
    current_epoch = 0

    for i, history in enumerate(all_histories):
        for j, val_acc in enumerate(history.history['val_accuracy']):
            current_epoch += 1
            if val_acc > best_val_acc:
                best_val_acc = val_acc
                best_epoch = current_epoch

    print(f"\n  最佳验证准确率: {best_val_acc:.4f} (第 {best_epoch} 轮)")

    # 显示每个阶段的结果
    print("\n  各阶段训练结果:")
    current_epoch = 0
    for i, history in enumerate(all_histories):
        epochs_in_stage = len(history.history['val_accuracy'])
        start_epoch = current_epoch + 1
        end_epoch = current_epoch + epochs_in_stage

        final_train_acc = history.history['accuracy'][-1]
        final_val_acc = history.history['val_accuracy'][-1]
        final_train_loss = history.history['loss'][-1]
        final_val_loss = history.history['val_loss'][-1]

        print(f"    阶段 {i+1} (Epoch {start_epoch}-{end_epoch}):")
        print(f"      训练准确率: {final_train_acc:.4f}, 验证准确率: {final_val_acc:.4f}")
        print(f"      训练损失: {final_train_loss:.4f}, 验证损失: {final_val_loss:.4f}")

        current_epoch = end_epoch

    print("=" * 60)


# ==================== 评估模型 ====================
def evaluate_model(model, test_generator):
    """评估模型性能"""

    test_loss, test_acc = model.evaluate(test_generator)
    print(f"\n测试集准确率: {test_acc:.4f}")
    print(f"测试集损失: {test_loss:.4f}")

    return test_loss, test_acc


# ==================== 生成混淆矩阵 ====================
def generate_confusion_matrix(model, test_generator, class_names, config):
    """
    生成并可视化混淆矩阵
    同时输出详细的分类报告
    """
    print("\n[生成混淆矩阵]")

    # 重置测试生成器
    test_generator.reset()

    # 获取真实标签和预测结果
    print("  正在获取预测结果...")
    y_true = test_generator.classes

    # 预测所有测试样本
    predictions = model.predict(test_generator, verbose=1)
    y_pred = np.argmax(predictions, axis=1)

    # 计算混淆矩阵
    cm = confusion_matrix(y_true, y_pred)

    # 创建图形
    plt.figure(figsize=(10, 8))

    # 绘制混淆矩阵热图
    sns.heatmap(cm, annot=True, fmt='d', cmap='Blues',
                xticklabels=class_names,
                yticklabels=class_names,
                cbar_kws={'label': '样本数量'})

    plt.title('混淆矩阵 (Confusion Matrix)', fontsize=16, pad=20)
    plt.ylabel('真实标签 (True Label)', fontsize=12)
    plt.xlabel('预测标签 (Predicted Label)', fontsize=12)
    plt.xticks(rotation=45, ha='right')
    plt.yticks(rotation=0)
    plt.tight_layout()

    # 保存混淆矩阵图像
    plt.savefig(config.CONFUSION_MATRIX_PATH, dpi=300, bbox_inches='tight')
    print(f"  混淆矩阵已保存到: {config.CONFUSION_MATRIX_PATH}")

    # 显示图像（如果在支持的环境中）
    try:
        plt.show()
    except:
        pass

    plt.close()

    # 打印混淆矩阵数值
    print("\n【混淆矩阵数值】")
    print("行：真实标签，列：预测标签")
    print("-" * 60)

    # 打印表头
    header = "真实\\预测 |" + "|".join([f"{name:^10}" for name in class_names])
    print(header)
    print("-" * 60)

    # 打印每一行
    for i, class_name in enumerate(class_names):
        row = f"{class_name:^10} |" + "|".join([f"{cm[i][j]:^10}" for j in range(len(class_names))])
        print(row)

    # 计算每个类别的准确率
    print("\n【各类别统计】")
    print("-" * 60)
    for i, class_name in enumerate(class_names):
        total = np.sum(cm[i])
        correct = cm[i][i]
        accuracy = correct / total if total > 0 else 0
        print(f"  {class_name}: 准确率 {accuracy:.2%} ({correct}/{total})")

    # 生成详细的分类报告
    print("\n【详细分类报告】")
    print("-" * 60)
    report = classification_report(y_true, y_pred,
                                   target_names=class_names,
                                   digits=4)
    print(report)

    # 保存分类报告到文件
    report_path = 'classification_report.txt'
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write("=" * 60 + "\n")
        f.write("分类模型评估报告\n")
        f.write("=" * 60 + "\n\n")
        f.write("混淆矩阵:\n")
        f.write("行：真实标签，列：预测标签\n\n")

        # 写入混淆矩阵
        header = "真实\\预测 |" + "|".join([f"{name:^10}" for name in class_names])
        f.write(header + "\n")
        f.write("-" * 60 + "\n")
        for i, class_name in enumerate(class_names):
            row = f"{class_name:^10} |" + "|".join([f"{cm[i][j]:^10}" for j in range(len(class_names))])
            f.write(row + "\n")

        f.write("\n各类别统计:\n")
        f.write("-" * 60 + "\n")
        for i, class_name in enumerate(class_names):
            total = np.sum(cm[i])
            correct = cm[i][i]
            accuracy = correct / total if total > 0 else 0
            f.write(f"  {class_name}: 准确率 {accuracy:.2%} ({correct}/{total})\n")

        f.write("\n详细分类报告:\n")
        f.write("-" * 60 + "\n")
        f.write(report)

    print(f"\n  分类报告已保存到: {report_path}")

    return cm, y_true, y_pred


# ==================== 主函数 ====================
def main():
    # 解析命令行参数
    parser = argparse.ArgumentParser(description='MobileNet V2 交互式训练脚本')
    parser.add_argument('--epoch', type=int, default=10,
                        help='首轮训练的epoch数 (默认: 10)')
    parser.add_argument('--lr', type=float, default=None,
                        help='初始学习率 (默认: 使用配置文件中的值)')
    parser.add_argument('--batch_size', type=int, default=None,
                        help='批次大小 (默认: 使用配置文件中的值)')
    parser.add_argument('--alpha', type=float, default=None,
                        help='MobileNetV2的宽度乘数 (可选: 0.35, 0.5, 0.75, 1.0)')
    parser.add_argument('--input_size', type=int, default=None,
                        help='输入图像尺寸 (默认: 使用配置文件中的值)')
    parser.add_argument('--continue_epochs', type=int, default=None,
                        help='继续训练时的epoch数 (仅在--auto_continue模式下生效)')
    parser.add_argument('--continue_lr', type=float, default=None,
                        help='继续训练时的学习率 (仅在--auto_continue模式下生效)')
    parser.add_argument('--augmentation', action='store_true',
                        help='启用数据增强 (默认: 关闭)')
    parser.add_argument('--continue_batch_size', type=int, default=None,
                        help='继续训练时的批次大小 (仅在--auto_continue模式下生效)')
    parser.add_argument('--continue_augmentation', type=int, choices=[0, 1], default=None,
                        help='继续训练时是否启用数据增强 (0=关闭, 1=开启, 仅在--auto_continue模式下生效)')
    parser.add_argument('--auto_continue', action='store_true',
                        help='自动继续训练模式 (不询问，直接使用--continue_epochs和--continue_lr)')
    parser.add_argument('--full_augmentation', action='store_true',
                        help='强制使用完整增强（旋转+翻转+曝光），即使路径第二级为"num"')
    args = parser.parse_args()

    print("=" * 60)
    print("MobileNet V2 分类模型训练 (交互式)")
    print("=" * 60)

    # 配置GPU
    gpu_available = setup_gpu()

    # 创建配置
    config = Config()

    # 使用命令行参数覆盖配置
    if args.lr is not None:
        config.LEARNING_RATE = args.lr
    if args.batch_size is not None:
        config.BATCH_SIZE = args.batch_size
    if args.alpha is not None:
        config.ALPHA = args.alpha
    if args.input_size is not None:
        config.INPUT_SHAPE = (args.input_size, args.input_size, 3)

    # 处理完整增强选项
    if args.full_augmentation:
        os.environ['FULL_AUGMENTATION'] = '1'
        print("\n[提示] 已启用完整增强模式（旋转+翻转+曝光）")

    # 验证和清理数据集
    print("\n[1] 验证和清理数据集...")
    print("  检查训练集...")
    train_valid, train_corrupted = validate_and_clean_dataset(
        config.TRAIN_DIR,
        target_size=config.INPUT_SHAPE[:2]
    )
    print("  检查测试集...")
    test_valid, test_corrupted = validate_and_clean_dataset(
        config.TEST_DIR,
        target_size=config.INPUT_SHAPE[:2]
    )

    total_corrupted = train_corrupted + test_corrupted
    if total_corrupted > 0:
        print(f"\n  ⚠ 发现并删除了 {total_corrupted} 个损坏的图像文件")
        print(f"    训练集: {train_corrupted} 个")
        print(f"    测试集: {test_corrupted} 个")
    else:
        print(f"\n  ✓ 所有图像文件都有效")

    # 自动检测类别数
    print("\n[2] 检测数据集类别...")
    num_classes, class_names = get_num_classes(config.TRAIN_DIR)

    # 加载数据
    print("\n[3] 加载数据集...")
    # 使用命令行参数的数据增强选项
    initial_augmentation = args.augmentation
    train_generator, test_generator = load_data(config, initial_augmentation)

    # 构建模型
    print("\n[4] 构建模型...")
    print(f"    - Alpha: {config.ALPHA}")
    print(f"    - 输入尺寸: {config.INPUT_SHAPE}")
    print(f"    - 类别数: {num_classes}")
    model = build_model(num_classes, config)
    model.summary()

    # 编译模型（初始化优化器）
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=config.LEARNING_RATE),
        loss='categorical_crossentropy',
        metrics=['accuracy']
    )

    # 交互式训练
    print("\n[5] 开始交互式训练...")
    print(f"    - 初始学习率: {config.LEARNING_RATE}")
    print(f"    - 批次大小: {config.BATCH_SIZE}")
    print(f"    - 数据增强: {'开启' if initial_augmentation else '关闭'}")
    print(f"    - 首轮训练epoch数: {args.epoch}")

    all_histories = []
    total_epochs = 0
    current_lr = config.LEARNING_RATE
    current_batch_size = config.BATCH_SIZE
    use_augmentation = initial_augmentation  # 使用命令行参数的初始值
    epochs_this_round = args.epoch  # 使用命令行参数
    round_num = 1

    while True:
        print("\n" + "=" * 60)
        print(f"【第 {round_num} 轮训练】")
        print("=" * 60)

        # 训练当前轮
        history = train_model_incremental(
            model,
            train_generator,
            test_generator,
            epochs_this_round,
            current_lr,
            config.CHECKPOINT_PATH
        )

        all_histories.append(history)
        total_epochs += epochs_this_round

        # 显示当前轮的结果
        final_train_acc = history.history['accuracy'][-1]
        final_val_acc = history.history['val_accuracy'][-1]
        final_train_loss = history.history['loss'][-1]
        final_val_loss = history.history['val_loss'][-1]

        print(f"\n  本轮训练结果:")
        print(f"    训练准确率: {final_train_acc:.4f}, 验证准确率: {final_val_acc:.4f}")
        print(f"    训练损失: {final_train_loss:.4f}, 验证损失: {final_val_loss:.4f}")

        # 决定是否继续训练
        if args.auto_continue:
            # 自动继续模式：不询问，直接使用命令行参数
            if args.continue_epochs is not None:
                print("\n【自动继续训练模式】")
                print(f"  使用命令行参数继续训练")

                # 使用命令行参数
                epochs_this_round = args.continue_epochs

                # 检查是否需要更新学习率
                if args.continue_lr is not None:
                    current_lr = args.continue_lr
                    print(f"  下一轮学习率: {current_lr}")

                # 检查是否需要更新batch_size
                new_batch_size = current_batch_size
                if args.continue_batch_size is not None:
                    new_batch_size = args.continue_batch_size
                    print(f"  下一轮批次大小: {new_batch_size}")

                # 检查是否需要更新数据增强设置
                new_use_augmentation = use_augmentation
                if args.continue_augmentation is not None:
                    new_use_augmentation = bool(args.continue_augmentation)
                    print(f"  下一轮数据增强: {'开启' if new_use_augmentation else '关闭'}")

                print(f"  下一轮训练轮数: {epochs_this_round}")

                # 检查是否需要重新加载数据
                if new_batch_size != current_batch_size or new_use_augmentation != use_augmentation:
                    print("\n  检测到批次大小或数据增强设置改变，重新加载数据...")

                    # 更新配置
                    config.BATCH_SIZE = new_batch_size

                    # 重新加载数据
                    train_generator, test_generator = load_data(config, new_use_augmentation)

                    # 更新当前设置
                    current_batch_size = new_batch_size
                    use_augmentation = new_use_augmentation

                round_num += 1
                continue  # 继续下一轮训练
            else:
                print("\n【自动继续训练模式】")
                print("  未指定--continue_epochs参数，训练已停止")
                break
        else:
            # 交互式模式：先询问是否继续，再询问参数
            continue_training = ask_continue_training()

            if not continue_training:
                print("\n  训练已停止")
                break

            # 交互式模式下，忽略命令行参数，完全通过交互来设置
            # 询问用户下一轮训练的参数
            new_lr, new_epochs, new_batch_size, new_use_augmentation = ask_next_training_parameters(
                current_lr, current_batch_size, use_augmentation, config.TRAIN_DIR
            )

            # 检查batch_size或数据增强是否改变
            if new_batch_size != current_batch_size or new_use_augmentation != use_augmentation:
                print("\n  检测到批次大小或数据增强设置改变，重新加载数据...")

                # 更新配置
                config.BATCH_SIZE = new_batch_size

                # 重新加载数据
                train_generator, test_generator = load_data(config, new_use_augmentation)

                # 更新当前设置
                current_batch_size = new_batch_size
                use_augmentation = new_use_augmentation

            current_lr = new_lr
            epochs_this_round = new_epochs

            round_num += 1

    # 显示训练历史摘要
    print_training_summary(all_histories, total_epochs)

    # 加载最佳模型
    if os.path.exists(config.CHECKPOINT_PATH):
        print(f"\n[6] 加载最佳模型...")
        model = keras.models.load_model(config.CHECKPOINT_PATH)
        print(f"  已从检查点加载: {config.CHECKPOINT_PATH}")

    # 评估模型
    print("\n[7] 评估模型...")
    evaluate_model(model, test_generator)

    # 生成混淆矩阵
    print("\n[8] 生成混淆矩阵...")
    cm, y_true, y_pred = generate_confusion_matrix(model, test_generator, class_names, config)

    # 保存最终模型
    print(f"\n[9] 保存最终模型...")
    model.save(config.MODEL_SAVE_PATH)
    print(f"  模型已保存到: {config.MODEL_SAVE_PATH}")

    # Int8 量化转换
    print("\n[10] 转换为 Int8 量化模型...")
    tflite_model = convert_to_int8_tflite(model, train_generator, config)

    print("\n" + "=" * 60)
    print("训练完成!")
    print(f"总训练轮数: {total_epochs}")
    print(f"训练阶段数: {len(all_histories)}")
    print(f"模型已保存到: {config.MODEL_SAVE_PATH}")
    print(f"量化模型已保存到: {config.TFLITE_MODEL_PATH}")
    print(f"混淆矩阵已保存到: {config.CONFUSION_MATRIX_PATH}")
    print(f"分类报告已保存到: classification_report.txt")
    print("=" * 60)


if __name__ == '__main__':
    main()
