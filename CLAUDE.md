# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是一个多项目代码仓库，包含用于图像处理和深度学习模型训练的工具。主要包括两个子项目：

1. **mnist_tools**: MNIST 手写数字数据集的图像处理工具集
2. **mobilenetv2_training**: MobileNetV2 深度学习模型训练项目

## 仓库结构

```
num/
├── mnist_tools/              # MNIST 图像处理工具集
│   ├── original_images/      # 原始 28x28 MNIST 图像 (70,000张，不在版本控制中)
│   ├── *.py                  # 各种图像处理脚本
│   ├── CLAUDE.md             # mnist_tools 详细文档
│   ├── README.md             # 用户文档
│   └── requirements.txt      # Python 依赖
├── mobilenetv2_training/     # MobileNetV2 训练项目
│   ├── train_mobilenetv2.py  # 主训练脚本
│   └── MobileNetV2训练说明.md  # 训练文档
└── CLAUDE.md                 # 本文档（项目根目录）
```

## 环境配置

### Python 环境

推荐使用 Miniconda 或 Anaconda：

```bash
# Windows 系统
conda activate myconda

# Linux/Mac 系统
# conda activate your_env_name
# 或使用项目特定的虚拟环境
```

**当前运行环境**: Windows (win32)

### 安装依赖

**mnist_tools 项目**：
```bash
cd mnist_tools
pip install -r requirements.txt
# 依赖: Pillow>=10.0.0, numpy>=1.24.0, tqdm>=4.65.0
```

**mobilenetv2_training 项目**：
```bash
# CPU 版本
pip install tensorflow numpy matplotlib seaborn scikit-learn

# GPU 版本（推荐，训练速度快 5-10 倍）
pip install tensorflow[and-cuda] numpy matplotlib seaborn scikit-learn
```

## mnist_tools 项目

### 核心概念

这是一个 MNIST 数据增强管道，通过一系列脚本对 28x28 手写数字图像进行处理：

1. **缩放和标记** (`resize_and_mark.py`): 28x28 → 128x128，为数字 6 和 9 添加特征横杠
2. **残影效果** (`add_motion_blur.py`): 添加运动模糊或残影效果
3. **颜色反转** (`invert_colors.py`): 黑白像素反转
4. **图像旋转** (`rotate_images.py`): 生成 4 个旋转方向（上/下/左/右）
5. **数字融合** (`merge_digits.py`): 将两个单独数字融合成两位数图像

### 常用命令

```bash
cd mnist_tools

# 完整数据增强管道（推荐）
python resize_and_mark.py        # 步骤1: 缩放到 128x128
python add_motion_blur.py        # 步骤2: 添加残影
python invert_colors.py          # 步骤3: 颜色反转
python rotate_images.py          # 步骤4: 旋转为4个方向

# 生成两位数图像
python merge_digits.py           # 融合单个数字为两位数

# 批量处理旋转图像反转
python batch_invert_rotated.py  # 批量处理所有旋转方向的像素反转
```

### 重要提示

- **原始图像位置**: 所有 70,000 张原始 JPG 图片存放在 `original_images/` 文件夹中
- **配置修改**: 所有脚本的输入输出路径在各自的 `main()` 函数中配置
- **处理顺序**: 确保输入目录与前一步的输出目录匹配
- **详细文档**: 查看 `mnist_tools/CLAUDE.md` 获取每个脚本的详细说明

### 脚本编号系统

mnist_tools 使用编号系统组织脚本：

- **脚本 1-9**: 核心处理脚本（按处理流程编号）
- **脚本 10-14**: 辅助工具脚本（批处理、测试、重命名）

## mobilenetv2_training 项目

### 核心概念

使用 MobileNetV2 架构训练轻量级图像分类模型，支持：
- 自动类别检测
- GPU 加速训练
- Int8 量化（模型大小约 1000KB）
- 交互式多轮训练
- 混淆矩阵可视化
- **智能数据增强**（路径感知增强策略）

### 智能数据增强

脚本支持灵活的数据增强选择，提供两种增强方式：

**增强类型**：
- **完整增强**：旋转（360°）+ 水平翻转 + 垂直翻转 + 随机曝光
- **仅随机曝光**：仅使用随机曝光，避免旋转导致的重复数据

**使用方式**：
1. **命令行模式**：启用增强时默认使用完整增强
   ```bash
   python train_mobilenetv2.py --augmentation
   ```

2. **交互式模式**：训练中断时可以选择增强类型
   ```
   是否开启数据增强？(y/n): y

   增强类型选择:
     1. 完整增强 (旋转+翻转+曝光)
     2. 仅随机曝光
   选择增强类型 (1/2): 1
   ```

3. **强制完整增强**：即使使用 `--augmentation` 参数也可以用环境变量指定
   ```bash
   set FULL_AUGMENTATION=1
   python train_mobilenetv2.py --augmentation
   ```

### 数据集要求

```
/mnt/image/
├── train/              # 训练集
│   ├── class_1/
│   ├── class_2/
│   └── class_n/
└── test/               # 测试集
    ├── class_1/
    ├── class_2/
    └── class_n/
```

### 常用命令

```bash
cd mobilenetv2_training

# 基础训练（交互式模式）
python train_mobilenetv2.py

# 自定义参数训练
python train_mobilenetv2.py --epoch 20 --lr 0.001 --batch_size 16

# 启用数据增强
python train_mobilenetv2.py --epoch 20 --augmentation

# 启用完整增强（即使路径第二级为"num"也使用旋转+翻转+曝光）
python train_mobilenetv2.py --epoch 20 --augmentation --full_augmentation

# 自动继续训练模式（无需交互）
python train_mobilenetv2.py --epoch 20 --continue_epochs 10 --continue_lr 0.0005 --auto_continue

# 查看所有参数
python train_mobilenetv2.py --help
```

### 训练模式

**交互式模式（默认）**：
- 每轮训练后询问是否继续
- 可动态调整学习率、批次大小和数据增强
- 如果选择开启/关闭数据增强，会**询问增强类型**：
  - 完整增强（旋转+翻转+曝光）
  - 仅随机曝光
- 适合实验性训练和参数调整

**模型续训**：
- 训练启动时自动检测是否存在上次的最佳模型（checkpoint）
- 如果存在，询问用户是否从该模型继续训练
  - 选择 `y` → 加载上次最好的模型继续训练
  - 选择 `n` → 创建新模型重新开始训练
- 这样可以充分利用之前的训练成果，加快收敛速度

**自动模式（`--auto_continue`）**：
- 完全不询问，按命令行参数执行
- 适合批量训练和自动化流程

### 训练输出文件

- `mobilenetv2_model.h5`: 完整 Keras 模型（~8MB）
- `mobilenetv2_int8.tflite`: Int8 量化 TFLite 模型（~1MB）
- `confusion_matrix.png`: 混淆矩阵可视化
- `classification_report.txt`: 详细分类报告
- `mobilenetv2_checkpoint.h5`: 训练检查点

### 量化模型选择策略

脚本在进行 Int8 量化时会智能选择最佳模型：

**选择逻辑**：
- 如果训练过程中有使用数据增强的轮次 → 只从这些有增强的轮次中选择最佳模型
- 如果整个训练过程都没有数据增强 → 从所有轮次中选择最佳模型

**工作示例**：
```
第 1 轮: 不使用增强 (val_acc: 0.85)
第 2 轮: 使用增强    (val_acc: 0.88)
第 3 轮: 使用增强    (val_acc: 0.90)  ← 选择此模型用于量化

[10] 选择用于量化的模型...
  检测到训练中有数据增强的轮次
  从有增强的轮次中选择最佳模型
  最佳模型来自: 第 3 轮 (验证精度: 0.9000)
```

### 训练指标解释：为什么 val_loss 高于 loss？

在训练过程中，通常会看到验证损失（val_loss）高于训练损失（loss），这是**正常且预期的现象**。以下是详细原因：

**根本原因**：
1. **数据分布差异**：
   - 训练集（loss）：模型直接学习的数据，用于更新权重
   - 验证集（val_loss）：模型从未见过的新数据，用于评估泛化能力

2. **模型过拟合**：
   - 模型在训练数据上过度学习，记住了训练数据的特有特征
   - 包括学到了数据中的噪声和偏差
   - 在验证集（新数据）上，这些学到的"特殊规律"不适用
   - 因此性能下降，val_loss升高

**数值示例**：
```
Epoch 1: loss=0.80, val_loss=0.85  ← 正常，val_loss略高
Epoch 2: loss=0.60, val_loss=0.70  ← 正常，两者都在下降
Epoch 3: loss=0.40, val_loss=0.65  ← 开始过拟合，gap扩大
Epoch 4: loss=0.30, val_loss=0.72  ← 过拟合加重，val_loss反向增长
```

**什么情况是正常的**：
- ✓ val_loss > loss（10-20%差距）：正常现象，表示合理的泛化
- ✓ 两者都在下降：模型在学习和泛化
- ✓ val_loss稳定或缓慢下降：模型已达到较好状态

**什么情况需要改进**：
- ✗ val_loss远高于loss（差距>50%）：严重过拟合
- ✗ val_loss持续增长：模型没有学到有效特征
- ✗ loss降低但val_loss升高：过拟合加重

**改进方法**：
1. **数据增强**（推荐）：
   - 在训练时对数据进行旋转、翻转、曝光等变换
   - 让模型学到更通用的特征
   - 脚本已支持：`--augmentation --full_augmentation`

2. **正则化**：
   - L1/L2正则化：惩罚过大的权重
   - Dropout：随机丢弃神经元，防止过拟合

3. **早停（Early Stopping）**：
   - 监控val_loss，当不再改进时停止训练
   - 脚本已内置：`EarlyStopping` 回调

4. **获取更多数据**：
   - 更多的训练数据能帮助模型学到更通用的特征

**本项目中的实现**：
- ✓ 数据增强：支持旋转、翻转、随机曝光
- ✓ 正则化：通过dropout降低过拟合
- ✓ 早停：val_loss不改进时自动停止
- ✓ 最佳模型保存：只保存最佳的val_acc对应的模型

## Git 工作流

### 仓库信息

- **远程仓库**: `git@github.com:Yyj4399/carcode.git`
- **主分支**: `main`
- **当前分支**: `main`

### 提交规范

根据修改类型使用以下前缀：
- `添加`: 新增功能或文件
- `更新`: 增强现有功能
- `修复`: Bug 修复
- `重构`: 代码重构
- `文档`: 文档更新
- `合并`: 分支合并
- `优化`: 性能优化或代码改进

### 常用 Git 命令

```bash
# 查看状态
git status

# 添加所有修改的文件
git add .

# 提交更改（使用中文提交信息）
git commit -m "添加MobileNetV2训练项目"

# 推送到远程仓库
git push

# 拉取最新代码
git pull
```

## 代码架构模式

### mnist_tools 脚本模式

所有图像处理脚本遵循统一模式：

1. **核心处理函数**: 处理单张图像（例如 `process_image()`, `invert_image()`）
2. **批量处理函数**: `main()` 函数处理整个目录
3. **自动检测**: 自动识别输入目录结构
4. **统计输出**: 显示详细处理统计
5. **进度显示**: 使用 tqdm 显示实时进度

### 配置修改方法

1. 打开对应的 `.py` 文件
2. 找到 `main()` 函数（通常在文件末尾）
3. 修改 `input_dir` 和 `output_dir` 变量
4. 某些脚本还有额外参数（如 `blur_type`, `intensity`）

## 数据集路径配置

### mnist_tools

- 原始图像: `./mnist_tools/original_images/` (70,000 张 28x28 JPG)
- 处理后图像: 各种 `resized_*` 和 `merged_*` 文件夹

**注意**: mnist_tools 的路径都是相对于 `mnist_tools/` 目录的相对路径

### mobilenetv2_training

在 `train_mobilenetv2.py` 的 `Config` 类中修改 (train_mobilenetv2.py:30)：

```python
class Config:
    # Linux/Mac 路径示例
    TRAIN_DIR = '/mnt/image/train'  # 训练集路径
    TEST_DIR = '/mnt/image/test'    # 测试集路径

    # Windows 路径示例（使用原始字符串）
    # TRAIN_DIR = r'D:\ACS image photos\num\num\train'
    # TEST_DIR = r'D:\ACS image photos\num\num\test'
```

### 数据集目录结构示例

**Linux/Mac 路径示例**:
```
/mnt/image/
├── train/              # 训练集
│   ├── class_1/
│   ├── class_2/
│   └── class_n/
└── test/               # 测试集
    ├── class_1/
    ├── class_2/
    └── class_n/
```

**Windows 路径示例（当前环境）**:
```
D:\ACS image photos\num\num\
├── train\              # 训练集
│   ├── 0_right\        # 数字0向右
│   ├── 0_up\           # 数字0向上
│   └── ...
└── test\               # 测试集
    ├── 0_right\
    └── ...
```

## GPU 配置

MobileNetV2 训练脚本会自动检测和配置 GPU：
- 自动检测可用 GPU 设备
- 启用 GPU 内存增长模式（动态分配显存）
- 无 GPU 时自动回退到 CPU 训练

验证 GPU 可用性：
```bash
python -c "import tensorflow as tf; print('GPU可用:', len(tf.config.list_physical_devices('GPU')) > 0)"
```

## 常见任务

### 处理新的 MNIST 数据集

```bash
cd mnist_tools

# 1. 将原始图像放入 original_images/
# 2. 运行完整管道
python resize_and_mark.py
python add_motion_blur.py
python invert_colors.py
python rotate_images.py
```

### 训练新的分类模型

```bash
cd mobilenetv2_training

# 1. 准备数据集（train/ 和 test/ 文件夹）
# 2. 运行训练（GPU 推荐）
python train_mobilenetv2.py --epoch 20 --augmentation

# 3. 根据提示进行交互式调整或查看结果
```

### 生成两位数数据集

```bash
cd mnist_tools

# 随机模式（生成 1000 张）
python merge_digits.py  # 在 main() 中设置 mode = "random"

# 顺序模式（生成所有有效两位数 01-99）
python merge_digits.py  # 在 main() 中设置 mode = "sequential"
```

## 注意事项

1. **大文件**: 所有图像文件（原始和处理后）、训练模型文件都在 `.gitignore` 中，不提交到版本控制
2. **路径格式**:
   - Windows 路径使用原始字符串（`r"D:\path"`）或双反斜杠（`"D:\\path"`）
   - 路径中包含空格（如 `ACS image photos`）时必须使用正确的引号格式
3. **内存占用**:
   - mnist_tools: 图像处理内存占用较小
   - mobilenetv2_training: 训练时根据 batch_size 调整内存（可用 `--batch_size` 减小）
4. **数据完整性**: mobilenetv2_training 会在训练前自动验证和清理损坏的图像文件
5. **文档更新**: 每次修改代码后，更新对应的 md 文档
6. **Claude Code 配置**: `.claude/settings.local.json` 包含项目特定的自动批准命令配置

## 详细文档

- **mnist_tools 详细架构**: 查看 `mnist_tools/CLAUDE.md`（包含所有脚本的详细说明、参数、输出格式）
- **MobileNetV2 训练指南**: 查看 `mobilenetv2_training/MobileNetV2训练说明.md`（包含完整参数说明、训练策略、故障排除）

## 许可证

MIT License
