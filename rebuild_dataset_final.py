"""
重建数据集脚本 - 最终版本

功能：
1. 清空train文件夹下的所有内容
2. 从以下路径复制图片：
   - resized_128x128_rotated_inverted (非blur)
   - resized_128x128_with_blur_inverted_rotated (blur)

   每个方向文件夹(down/left/right/up)下的0_X到9_X子文件夹
   各复制500张图片

使用方法：
    python rebuild_dataset_final.py --yes

警告：此脚本会删除train文件夹下的所有内容！
"""

import os
import random
import sys
import shutil
from pathlib import Path


def clear_train_folder(train_dir):
    """清空train文件夹下的所有内容"""
    print(f"\n[1/2] 清空train文件夹: {train_dir}")

    if not train_dir.exists():
        print(f"  创建train文件夹...")
        train_dir.mkdir(parents=True)
        return

    # 删除所有子文件夹和文件
    deleted_folders = 0
    deleted_files = 0

    for item in train_dir.iterdir():
        try:
            if item.is_dir():
                shutil.rmtree(item)
                deleted_folders += 1
            else:
                item.unlink()
                deleted_files += 1
        except Exception as e:
            print(f"  [!] 无法删除 {item.name}: {e}")

    print(f"  [OK] 删除了 {deleted_folders} 个文件夹, {deleted_files} 个文件")


def copy_images_from_source(base_dir, train_dir, target_count=500, is_blur=False):
    """
    从源文件夹复制图片到train文件夹

    遍历base_dir下的方向文件夹(down/left/right/up)，
    从每个方向的数字文件夹(0_X到9_X)中复制图片
    """
    base_path = Path(base_dir)

    if not base_path.exists():
        print(f"  [X] 错误：源路径不存在 - {base_path}")
        return 0, 0, 0

    # 方向列表
    directions = ['down', 'left', 'right', 'up']

    total_copied = 0
    processed_folders = 0
    skipped_folders = 0

    image_type = "blur图片" if is_blur else "非blur图片"
    print(f"\n从 {base_path.name} 复制{image_type}")

    for direction in directions:
        direction_path = base_path / direction

        if not direction_path.exists():
            print(f"  [!] 方向文件夹不存在: {direction}")
            continue

        # 获取该方向下的所有数字文件夹 (0_X 到 9_X)
        digit_folders = [f for f in direction_path.iterdir()
                        if f.is_dir() and f.name[0].isdigit() and '_' in f.name]

        for digit_folder in sorted(digit_folders):
            # 只处理0-9的数字
            digit = digit_folder.name.split('_')[0]
            if not (digit.isdigit() and 0 <= int(digit) <= 9):
                continue

            # 获取该文件夹的所有jpg图片
            images = list(digit_folder.glob("*.jpg")) + list(digit_folder.glob("*.JPG"))

            if len(images) == 0:
                print(f"  [!] {digit_folder.name}: 没有图片")
                skipped_folders += 1
                continue

            if len(images) < target_count:
                print(f"  [!] {digit_folder.name}: 图片不足 (只有{len(images)}张，需要{target_count}张)")
                images_to_copy = images
            else:
                # 随机选择target_count张图片
                random.shuffle(images)
                images_to_copy = images[:target_count]

            # 创建目标文件夹
            target_folder = train_dir / digit_folder.name
            target_folder.mkdir(parents=True, exist_ok=True)

            # 复制图片
            copied_count = 0
            for img in images_to_copy:
                try:
                    target_path = target_folder / img.name
                    # 如果文件已存在（从非blur源复制过），跳过
                    if not target_path.exists():
                        shutil.copy2(img, target_path)
                        copied_count += 1
                    else:
                        # 文件已存在，重命名后复制
                        suffix = "_blur" if is_blur else "_plain"
                        new_name = img.stem + suffix + img.suffix
                        target_path = target_folder / new_name
                        shutil.copy2(img, target_path)
                        copied_count += 1
                except Exception as e:
                    print(f"    [!] 无法复制 {img.name}: {e}")

            total_copied += copied_count
            processed_folders += 1

            if copied_count > 0:
                print(f"  [OK] {digit_folder.name}: 复制了 {copied_count}/{len(images)} 张")

    return processed_folders, skipped_folders, total_copied


def main():
    """主函数"""
    # 检查是否有 --yes 参数
    auto_confirm = '--yes' in sys.argv or '-y' in sys.argv

    # 设置路径
    train_dir = Path("./train")
    source_no_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\resized_128x128_rotated_inverted")
    source_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\resized_128x128_with_blur_inverted_rotated")

    print("="*60)
    print("数据集重建脚本 - 最终版本")
    print("="*60)
    print(f"\n目标路径: {train_dir.absolute()}")
    print(f"非blur源路径: {source_no_blur}")
    print(f"blur源路径: {source_blur}")
    print(f"\n每个类别文件夹将复制:")
    print(f"  - 500张非blur图片")
    print(f"  - 500张blur图片")
    print(f"  - 总计: 1000张/类别")

    print("\n" + "="*60)
    print("警告：此操作将删除train文件夹下的所有内容！")
    print("="*60)

    # 确认操作
    if not auto_confirm:
        user_input = input("\n是否继续？(输入 'yes' 确认): ")
        if user_input.lower() != 'yes':
            print("操作已取消")
            return
    else:
        print("\n[自动确认模式]")

    # 步骤1: 清空train文件夹
    clear_train_folder(train_dir)

    # 步骤2: 从非blur源复制图片
    print(f"\n[2/2] 复制图片")
    folders_no_blur, skipped_no_blur, images_no_blur = copy_images_from_source(
        source_no_blur, train_dir, target_count=500, is_blur=False
    )

    # 步骤3: 从blur源复制图片
    folders_blur, skipped_blur, images_blur = copy_images_from_source(
        source_blur, train_dir, target_count=500, is_blur=True
    )

    # 打印总结
    print("\n" + "="*60)
    print("重建完成！")
    print("="*60)
    print(f"成功处理的文件夹: {folders_no_blur} (非blur) + {folders_blur} (blur)")
    print(f"跳过的文件夹: {skipped_no_blur + skipped_blur}")
    print(f"复制的非blur图片: {images_no_blur} 张")
    print(f"复制的blur图片: {images_blur} 张")
    print(f"总计复制图片: {images_no_blur + images_blur} 张")

    # 验证每个文件夹的图片数量
    print("\n验证结果 (前10个文件夹):")
    category_folders = sorted([f for f in train_dir.iterdir() if f.is_dir()])
    for i, folder in enumerate(category_folders):
        if i >= 10:
            print(f"  ... (还有 {len(category_folders) - 10} 个文件夹)")
            break
        count = len(list(folder.glob("*.jpg")))
        print(f"  {folder.name}: {count} 张图片")

    print("="*60)


if __name__ == "__main__":
    # 设置随机种子以便结果可复现
    random.seed(42)

    main()
