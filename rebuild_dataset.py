"""
重建数据集脚本

功能：
1. 清空train文件夹下的所有内容
2. 从两个源路径中各选取500张图片复制到train文件夹
   - resized_128x128_rotated_inverted: 500张非blur图片
   - resized_128x128_with_blur_inverted_rotated: 500张blur图片

使用方法：
    python rebuild_dataset.py --yes

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


def copy_images_from_source(source_dir, train_dir, target_count=500, is_blur=False):
    """
    从源文件夹复制图片到train文件夹

    Args:
        source_dir: 源文件夹路径
        train_dir: 目标train文件夹路径
        target_count: 每个类别要复制的图片数量
        is_blur: 是否为blur图片
    """
    source_path = Path(source_dir)

    if not source_path.exists():
        print(f"  [X] 错误：源路径不存在 - {source_path}")
        return 0, 0

    # 获取所有类别文件夹
    category_folders = [f for f in source_path.iterdir() if f.is_dir()]

    if not category_folders:
        print(f"  [X] 错误：源路径中没有找到类别文件夹")
        return 0, 0

    total_copied = 0
    processed_folders = 0

    image_type = "blur图片" if is_blur else "非blur图片"
    print(f"\n从 {source_path.name} 复制{image_type}")
    print(f"  找到 {len(category_folders)} 个类别文件夹")

    for category_folder in sorted(category_folders):
        # 获取该类别的所有jpg图片
        images = list(category_folder.glob("*.jpg")) + list(category_folder.glob("*.JPG"))

        if len(images) < target_count:
            print(f"  [!] {category_folder.name}: 图片不足 (只有{len(images)}张，需要{target_count}张)")
            # 如果图片不足，就复制所有图片
            images_to_copy = images
        else:
            # 随机选择target_count张图片
            random.shuffle(images)
            images_to_copy = images[:target_count]

        # 创建目标文件夹（如果不存在）
        target_folder = train_dir / category_folder.name
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

        if copied_count > 0:
            print(f"  [OK] {category_folder.name}: 复制了 {copied_count} 张")

    return processed_folders, total_copied


def main():
    """主函数"""
    # 检查是否有 --yes 参数
    auto_confirm = '--yes' in sys.argv or '-y' in sys.argv

    # 设置路径
    train_dir = Path("./train")
    source_no_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\resized_128x128_rotated_inverted")
    source_blur = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\resized_128x128_with_blur_inverted_rotated")

    print("="*60)
    print("数据集重建脚本")
    print("="*60)
    print(f"\n目标路径: {train_dir.absolute()}")
    print(f"非blur源路径: {source_no_blur}")
    print(f"blur源路径: {source_blur}")
    print(f"\n每个类别将复制:")
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
    print(f"\n[2/3] 从非blur源复制图片")
    folders_no_blur, images_no_blur = copy_images_from_source(
        source_no_blur, train_dir, target_count=500, is_blur=False
    )

    # 步骤3: 从blur源复制图片
    print(f"\n[3/3] 从blur源复制图片")
    folders_blur, images_blur = copy_images_from_source(
        source_blur, train_dir, target_count=500, is_blur=True
    )

    # 打印总结
    print("\n" + "="*60)
    print("重建完成！")
    print("="*60)
    print(f"处理的类别数量: {max(folders_no_blur, folders_blur)}")
    print(f"复制的非blur图片: {images_no_blur} 张")
    print(f"复制的blur图片: {images_blur} 张")
    print(f"总计复制图片: {images_no_blur + images_blur} 张")

    # 验证每个文件夹的图片数量
    print("\n验证结果:")
    category_folders = [f for f in train_dir.iterdir() if f.is_dir()]
    for folder in sorted(category_folders)[:5]:  # 显示前5个作为示例
        count = len(list(folder.glob("*.jpg")))
        print(f"  {folder.name}: {count} 张图片")

    if len(category_folders) > 5:
        print(f"  ... (还有 {len(category_folders) - 5} 个文件夹)")

    print("="*60)


if __name__ == "__main__":
    # 设置随机种子以便结果可复现
    random.seed(42)

    main()
