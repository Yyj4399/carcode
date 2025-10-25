"""
直接删除train文件夹中超过1200张的文件
"""

import os
from pathlib import Path


def delete_excess_files(train_dir, target_per_class=1200):
    """删除train中超过target的文件"""

    train_path = Path(train_dir)
    train_classes = sorted([d for d in train_path.iterdir() if d.is_dir()])

    print(f"处理 {len(train_classes)} 个类别, 每类保留 {target_per_class} 张")
    print()

    total_deleted = 0

    for class_dir in train_classes:
        class_name = class_dir.name
        img_files = sorted(list(class_dir.glob('*.jpg')) + list(class_dir.glob('*.jpeg')))

        current_count = len(img_files)
        excess_count = max(0, current_count - target_per_class)

        if excess_count > 0:
            print(f"{class_name}: {current_count} -> {target_per_class} (删除{excess_count}张)")

            # 删除后面的excess_count张文件
            for i in range(excess_count):
                file_to_delete = img_files[-(i+1)]
                try:
                    os.remove(file_to_delete)
                    total_deleted += 1
                except Exception as e:
                    print(f"  错误: {e}")

    print(f"\n总共删除: {total_deleted}张文件")


def main():
    train_dir = r"D:\ACS image photos\num\num\train"
    delete_excess_files(train_dir, target_per_class=1200)


if __name__ == "__main__":
    main()
