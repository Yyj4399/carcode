"""
清理并重新分割数据集
确保：
- Train: 各类1200张（残影600+不含600）
- Test: 各类300张（残影150+不含150）
- Train和Test数据完全不同
"""

import os
import shutil
import random
from pathlib import Path
from collections import defaultdict


def clean_and_split_dataset(train_dir, test_dir, train_target=1200, test_target=300):
    """
    清理并重新分割数据集
    """

    print("="*70)
    print("数据集清理和分割脚本")
    print("="*70)

    train_path = Path(train_dir)
    test_path = Path(test_dir)

    # 清空test目录
    if test_path.exists():
        print("\n清空test目录...")
        shutil.rmtree(test_path)
    test_path.mkdir(parents=True, exist_ok=True)

    # 扫描train目录
    train_classes = sorted([d for d in train_path.iterdir() if d.is_dir()])
    print(f"\n找到{len(train_classes)}个类别\n")

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

    for class_dir in train_classes:
        class_name = class_dir.name
        print(f"处理类别 {class_name}...", end="")

        # 获取所有图片
        img_files = sorted(list(class_dir.glob('*.jpg')) + list(class_dir.glob('*.jpeg')))

        if not img_files:
            print(" [无图片，跳过]")
            continue

        # 分类（根据文件名或简单的启发式方法）
        blur_imgs = []
        plain_imgs = []

        for img_path in img_files:
            filename = img_path.name
            # 检查文件名中是否有blur标记
            if '_blur' in filename.lower() or 'blur' in filename.lower():
                blur_imgs.append(str(img_path))
            else:
                plain_imgs.append(str(img_path))

        # 如果文件名中没有标记，则按50:50分配
        if len(blur_imgs) == 0 or len(plain_imgs) == 0:
            random.shuffle(img_files)
            split_point = len(img_files) // 2
            blur_imgs = [str(f) for f in img_files[:split_point]]
            plain_imgs = [str(f) for f in img_files[split_point:]]

        total = len(blur_imgs) + len(plain_imgs)
        print(f" 总共{total}张 (含残影{len(blur_imgs)}, 不含{len(plain_imgs)})", end="")

        # 打乱
        random.shuffle(blur_imgs)
        random.shuffle(plain_imgs)

        # 分配Train
        train_blur = blur_imgs[:train_blur_need]
        train_plain = plain_imgs[:train_plain_need]

        # 分配Test
        test_blur = blur_imgs[train_blur_need:train_blur_need + test_blur_need]
        test_plain = plain_imgs[train_plain_need:train_plain_need + test_plain_need]

        # 需要删除的
        delete_blur = blur_imgs[train_blur_need + test_blur_need:]
        delete_plain = plain_imgs[train_plain_need + test_plain_need:]

        # 创建test类别文件夹
        test_class_dir = test_path / class_name
        test_class_dir.mkdir(exist_ok=True)

        # 复制test数据
        for idx, src_path in enumerate(test_blur):
            dst_name = f"{class_name}_{idx:06d}_blur.jpg"
            dst_path = test_class_dir / dst_name
            shutil.copy2(src_path, dst_path)
            stats['test_blur'][class_name] += 1

        for idx, src_path in enumerate(test_plain):
            dst_name = f"{class_name}_{idx:06d}_plain.jpg"
            dst_path = test_class_dir / dst_name
            shutil.copy2(src_path, dst_path)
            stats['test_plain'][class_name] += 1

        stats['test'][class_name] = len(test_blur) + len(test_plain)
        stats['train_blur'][class_name] = len(train_blur)
        stats['train_plain'][class_name] = len(train_plain)
        stats['train'][class_name] = len(train_blur) + len(train_plain)

        # 删除多余train文件
        for del_path in delete_blur + delete_plain:
            try:
                os.remove(del_path)
                stats['deleted'][class_name] += 1
            except:
                pass

        print(f" -> Train:{stats['train'][class_name]}, Test:{stats['test'][class_name]}")

    # 打印总结
    print("\n" + "="*70)
    print("【完成!】")
    print("="*70)

    total_train = sum(stats['train'].values())
    total_test = sum(stats['test'].values())

    print(f"\nTrain数据集: {total_train:,d}张 (残影{sum(stats['train_blur'].values()):,d}, 不含{sum(stats['train_plain'].values()):,d})")
    print(f"Test数据集: {total_test:,d}张 (残影{sum(stats['test_blur'].values()):,d}, 不含{sum(stats['test_plain'].values()):,d})")
    print(f"删除文件: {sum(stats['deleted'].values()):,d}张\n")

    # 验证所有类别都符合要求
    all_correct = True
    for class_name in sorted(stats['train'].keys()):
        train_count = stats['train'][class_name]
        test_count = stats['test'][class_name]
        if train_count != train_target or test_count != test_target:
            print(f"  [ERROR] {class_name}: Train={train_count}(需要{train_target}), Test={test_count}(需要{test_target})")
            all_correct = False

    if all_correct:
        print("OK All classes meet requirements!")
    else:
        print("WARNING Some classes do not meet requirements!")

    print("="*70)


def main():
    train_dir = r"D:\ACS image photos\num\num\train"
    test_dir = r"D:\ACS image photos\num\num\test"

    clean_and_split_dataset(train_dir, test_dir, train_target=1200, test_target=300)


if __name__ == "__main__":
    main()
