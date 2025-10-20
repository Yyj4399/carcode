"""
重建数据集脚本 v2

功能：
1. 清空train文件夹下的所有内容
2. 从merged_inverted和merged_with_blur_inverted文件夹中
   复制文件夹01-09的图片（各500张）到train文件夹

使用方法：
    python rebuild_dataset_v2.py --yes

警告：此脚本会删除train文件夹下的所有内容！
"""

import os
import random
import sys
import shutil
from pathlib import Path


def clear_train_folder(train_dir):
    """清空train文件夹下的所有内容"""
    print(f"\n[1/3] 清空train文件夹: {train_dir}")

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


def copy_images_from_folders(source_dir, train_dir, folder_list, target_count=500, is_blur=False):
    """
    从源文件夹的指定子文件夹复制图片到train文件夹

    Args:
        source_dir: 源文件夹路径
        train_dir: 目标train文件夹路径
        folder_list: 要复制的文件夹名称列表（如['01', '02', ...]）
        target_count: 每个文件夹要复制的图片数量
        is_blur: 是否为blur图片
    """
    source_path = Path(source_dir)

    if not source_path.exists():
        print(f"  [X] 错误：源路径不存在 - {source_path}")
        return 0, 0

    total_copied = 0
    processed_folders = 0

    image_type = "blur图片" if is_blur else "非blur图片"
    print(f"\n从 {source_path.name} 复制{image_type}")
    print(f"  处理文件夹: {folder_list}")

    for folder_name in folder_list:
        source_folder = source_path / folder_name

        if not source_folder.exists():
            print(f"  [!] {folder_name}: 文件夹不存在")
            continue

        # 获取该文件夹的所有jpg图片
        images = list(source_folder.glob("*.jpg")) + list(source_folder.glob("*.JPG"))

        if len(images) == 0:
            print(f"  [!] {folder_name}: 没有图片")
            continue

        if len(images) < target_count:
            print(f"  [!] {folder_name}: 图片不足 (只有{len(images)}张，需要{target_count}张)")
            images_to_copy = images
        else:
            # 随机选择target_count张图片
            random.shuffle(images)
            images_to_copy = images[:target_count]

        # 创建目标文件夹（使用原文件夹名）
        target_folder = train_dir / folder_name
        target_folder.mkdir(parents=True, exist_ok=True)

        # 复制图片
        copied_count = 0
        for img in images_to_copy:
            try:
                target_path = target_folder / img.name
                shutil.copy2(img, target_path)
                copied_count += 1
            except Exception as e:
                print(f"    [!] 无法复制 {img.name}: {e}")

        total_copied += copied_count
        processed_folders += 1

        print(f"  [OK] {folder_name}: 复制了 {copied_count}/{len(images)} 张")

    return processed_folders, total_copied


def main():
    """主函数"""
    # 检查是否有 --yes 参数
    auto_confirm = '--yes' in sys.argv or '-y' in sys.argv

    # 设置路径
    train_dir = Path("./train")
    source_no_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\merged_inverted")
    source_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\merged_with_blur_inverted")

    # 要复制的文件夹列表（01-09）
    folders_to_copy = [f"{i:02d}" for i in range(1, 10)]  # ['01', '02', ..., '09']

    print("="*60)
    print("数据集重建脚本 v2")
    print("="*60)
    print(f"\n目标路径: {train_dir.absolute()}")
    print(f"非blur源路径: {source_no_blur}")
    print(f"blur源路径: {source_blur}")
    print(f"\n要复制的文件夹: {folders_to_copy}")
    print(f"\n每个文件夹将复制:")
    print(f"  - 500张非blur图片")
    print(f"  - 500张blur图片")
    print(f"  - 总计: 1000张/文件夹")

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
    print(f"\n[2/3] 从非blur源复制图片")
    folders_no_blur, images_no_blur = copy_images_from_folders(
        source_no_blur, train_dir, folders_to_copy, target_count=500, is_blur=False
    )

    # 步骤3: 从blur源复制图片
    print(f"\n[3/3] 从blur源复制图片")
    folders_blur, images_blur = copy_images_from_folders(
        source_blur, train_dir, folders_to_copy, target_count=500, is_blur=True
    )

    # 打印总结
    print("\n" + "="*60)
    print("重建完成！")
    print("="*60)
    print(f"处理的文件夹数量: {len(folders_to_copy)}")
    print(f"复制的非blur图片: {images_no_blur} 张")
    print(f"复制的blur图片: {images_blur} 张")
    print(f"总计复制图片: {images_no_blur + images_blur} 张")

    # 验证每个文件夹的图片数量
    print("\n验证结果:")
    category_folders = sorted([f for f in train_dir.iterdir() if f.is_dir()])
    for folder in category_folders:
        count = len(list(folder.glob("*.jpg")))
        print(f"  {folder.name}: {count} 张图片")

    print("="*60)


if __name__ == "__main__":
    # 设置随机种子以便结果可复现
    random.seed(42)

    main()
