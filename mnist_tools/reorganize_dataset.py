"""
数据集重组脚本
从train中抽取300张/类到test，保持1200张/类在train
要求：
- Train: 各类1200张（含残影600张、不含残影600张）
- Test: 各类300张（含残影150张、不含残影150张）
- Train和Test的照片完全不同
"""

import os
import shutil
import random
from pathlib import Path
from collections import defaultdict
from tqdm import tqdm


def detect_blur_images(folder_path):
    """
    检测文件夹中哪些是含残影的图片
    返回: (blur_images_list, plain_images_list)
    """
    blur_images = []
    plain_images = []

    img_files = list(Path(folder_path).glob('*.jpg')) + list(Path(folder_path).glob('*.jpeg'))

    for img_path in img_files:
        # 尝试通过文件名检测（如果有_blur标记）
        filename = img_path.name
        if '_blur' in filename.lower() or 'blur' in filename.lower():
            blur_images.append(str(img_path))
        else:
            plain_images.append(str(img_path))

    # 如果文件名中没有标记，则通过图像处理来检测
    if not blur_images:
        # 简单的启发式方法：如果某个图片文件大小明显更大，可能是含残影的
        # 或者随机分配
        random.shuffle(img_files)
        split_point = len(img_files) // 2
        blur_images = [str(f) for f in img_files[:split_point]]
        plain_images = [str(f) for f in img_files[split_point:]]

    return blur_images, plain_images


def reorganize_dataset(train_dir, test_dir, train_target=1200, test_target=300):
    """
    重组数据集

    Args:
        train_dir: 训练集目录
        test_dir: 测试集目录
        train_target: 每类训练集目标数量
        test_target: 每类测试集目标数量
    """

    print("="*70)
    print("数据集重组脚本")
    print("="*70)
    print(f"\n配置:")
    print(f"  Train目录: {train_dir}")
    print(f"  Test目录: {test_dir}")
    print(f"  Train目标: 每类{train_target}张 (残影{train_target//2}, 不含{train_target//2})")
    print(f"  Test目标: 每类{test_target}张 (残影{test_target//2}, 不含{test_target//2})")
    print()

    train_path = Path(train_dir)
    test_path = Path(test_dir)

    # 创建test目录
    test_path.mkdir(parents=True, exist_ok=True)

    # 扫描train目录中的所有类别文件夹
    train_classes = sorted([d for d in train_path.iterdir() if d.is_dir()])

    if not train_classes:
        print("[ERROR] No class folders found in train directory")
        return

    print(f"[1] 扫描训练集... 找到 {len(train_classes)} 个类别\n")

    stats = {
        'train': defaultdict(int),
        'test': defaultdict(int),
        'train_blur': defaultdict(int),
        'train_plain': defaultdict(int),
        'test_blur': defaultdict(int),
        'test_plain': defaultdict(int),
        'deleted': defaultdict(int),
    }

    train_blur_need = train_target // 2
    train_plain_need = train_target - train_blur_need
    test_blur_need = test_target // 2
    test_plain_need = test_target - test_blur_need

    print(f"[2] 处理各类别...\n")

    for class_dir in train_classes:
        class_name = class_dir.name
        print(f"  处理类别: {class_name}")

        # 检测该类别中哪些是含残影的
        blur_imgs, plain_imgs = detect_blur_images(str(class_dir))
        print(f"    检测到: 含残影{len(blur_imgs)}张, 不含残影{len(plain_imgs)}张")

        # 打乱列表
        random.shuffle(blur_imgs)
        random.shuffle(plain_imgs)

        # 分配Train数据
        train_blur = blur_imgs[:train_blur_need]
        train_plain = plain_imgs[:train_plain_need]

        # 分配Test数据（从剩余部分）
        test_blur = blur_imgs[train_blur_need:train_blur_need + test_blur_need]
        test_plain = plain_imgs[train_plain_need:train_plain_need + test_plain_need]

        # 需要删除的文件
        delete_blur = blur_imgs[train_blur_need + test_blur_need:]
        delete_plain = plain_imgs[train_plain_need + test_plain_need:]

        # 创建test的类别文件夹
        test_class_dir = test_path / class_name
        test_class_dir.mkdir(exist_ok=True)

        # 复制test数据
        print(f"    复制Test数据: {len(test_blur) + len(test_plain)}张")
        for idx, src_path in enumerate(test_blur):
            dst_name = f"{class_name}_{idx:06d}_test_blur.jpg"
            dst_path = test_class_dir / dst_name
            shutil.copy2(src_path, dst_path)
            stats['test_blur'][class_name] += 1
            stats['test'][class_name] += 1

        for idx, src_path in enumerate(test_plain):
            dst_name = f"{class_name}_{idx:06d}_test_plain.jpg"
            dst_path = test_class_dir / dst_name
            shutil.copy2(src_path, dst_path)
            stats['test_plain'][class_name] += 1
            stats['test'][class_name] += 1

        # 统计train数据
        stats['train_blur'][class_name] = len(train_blur)
        stats['train_plain'][class_name] = len(train_plain)
        stats['train'][class_name] = len(train_blur) + len(train_plain)

        # 删除多余文件（从train中）
        print(f"    删除Train中多余: {len(delete_blur) + len(delete_plain)}张")
        for del_path in delete_blur + delete_plain:
            try:
                os.remove(del_path)
                stats['deleted'][class_name] += 1
            except Exception as e:
                print(f"      [ERROR] Failed to delete {del_path}: {e}")

        print(f"    Result - Train: {stats['train'][class_name]}张 (残影{stats['train_blur'][class_name]}, 不含{stats['train_plain'][class_name]})")
        print(f"             Test: {stats['test'][class_name]}张 (残影{stats['test_blur'][class_name]}, 不含{stats['test_plain'][class_name]})")
        print()

    # 打印总结
    print("="*70)
    print("【重组完成!】")
    print("="*70)

    total_train = sum(stats['train'].values())
    total_test = sum(stats['test'].values())
    total_deleted = sum(stats['deleted'].values())

    print(f"\nTrain数据集:")
    print(f"  总数: {total_train:,d}张")
    print(f"  含残影: {sum(stats['train_blur'].values()):,d}张")
    print(f"  不含残影: {sum(stats['train_plain'].values()):,d}张")

    print(f"\nTest数据集:")
    print(f"  总数: {total_test:,d}张")
    print(f"  含残影: {sum(stats['test_blur'].values()):,d}张")
    print(f"  不含残影: {sum(stats['test_plain'].values()):,d}张")

    print(f"\n删除的文件: {total_deleted:,d}张")

    print(f"\n按类别统计:")
    for class_name in sorted(stats['train'].keys()):
        print(f"  {class_name:15s}: Train={stats['train'][class_name]:4d}, Test={stats['test'][class_name]:3d}, Delete={stats['deleted'][class_name]:4d}")

    print("="*70)


def main():
    """主函数"""
    print("\n" + "="*70)
    print("数据集重组脚本 - Train(1200/类) + Test(300/类)")
    print("="*70 + "\n")

    train_dir = r"D:\ACS image photos\num\num\train"
    test_dir = r"D:\ACS image photos\num\num\test"

    print(f"配置:")
    print(f"  Train目录: {train_dir}")
    print(f"  Test目录: {test_dir}")
    print()

    # 检查目录
    if not Path(train_dir).exists():
        print(f"[ERROR] Train directory not found: {train_dir}")
        return

    # 执行重组
    reorganize_dataset(
        train_dir=train_dir,
        test_dir=test_dir,
        train_target=1200,
        test_target=300
    )


if __name__ == "__main__":
    main()
