"""
图像数量缩减脚本

功能：将每个类别文件夹中的图像数量精确设置为1000张
- 精确保留500张文件名带'blur'的图像
- 精确保留500张文件名不带'blur'的图像
- 如果图像不足1000张，会跳过该文件夹并给出警告

使用方法：
    python reduce_images.py

警告：此脚本会永久删除图像文件，请谨慎使用！
"""

import os
import random
import sys
from pathlib import Path


def reduce_images_in_folder(folder_path, target_blur=500, target_no_blur=500):
    """
    将文件夹中的图像数量精确设置为目标数量

    Args:
        folder_path: 类别文件夹路径
        target_blur: 目标保留的带blur图像数量（精确）
        target_no_blur: 目标保留的不带blur图像数量（精确）

    Returns:
        tuple: (删除的图像数量, 保留的图像数量, 是否成功)
    """
    folder_path = Path(folder_path)

    if not folder_path.exists():
        print(f"[X] 警告：文件夹不存在 - {folder_path}")
        return 0, 0, False

    # 获取所有jpg图像
    all_images = list(folder_path.glob("*.jpg")) + list(folder_path.glob("*.JPG"))

    if not all_images:
        print(f"[X] 警告：文件夹中没有图像 - {folder_path.name}")
        return 0, 0, False

    # 分类图像：带blur和不带blur
    blur_images = [img for img in all_images if 'blur' in img.name.lower()]
    no_blur_images = [img for img in all_images if 'blur' not in img.name.lower()]

    print(f"\n处理文件夹: {folder_path.name}")
    print(f"  原始图像数量 - 带blur: {len(blur_images)}, 不带blur: {len(no_blur_images)}, 总计: {len(all_images)}")

    # 检查图像数量是否足够
    if len(blur_images) < target_blur:
        print(f"  [X] 错误：带blur图像不足！需要 {target_blur} 张，实际只有 {len(blur_images)} 张")
        print(f"  [SKIP] 跳过此文件夹")
        return 0, len(all_images), False

    if len(no_blur_images) < target_no_blur:
        print(f"  [X] 错误：不带blur图像不足！需要 {target_no_blur} 张，实际只有 {len(no_blur_images)} 张")
        print(f"  [SKIP] 跳过此文件夹")
        return 0, len(all_images), False

    deleted_count = 0

    # 处理带blur的图像 - 精确保留target_blur张
    if len(blur_images) > target_blur:
        # 随机选择要保留的图像
        random.shuffle(blur_images)
        images_to_delete = blur_images[target_blur:]

        # 删除多余的图像
        for img in images_to_delete:
            try:
                img.unlink()
                deleted_count += 1
            except Exception as e:
                print(f"    [!] 错误：无法删除 {img.name} - {e}")

        print(f"  [OK] 带blur图像：保留 {target_blur} 张，删除 {len(images_to_delete)} 张")
    elif len(blur_images) == target_blur:
        print(f"  [OK] 带blur图像：已经是 {target_blur} 张，无需处理")

    # 处理不带blur的图像 - 精确保留target_no_blur张
    if len(no_blur_images) > target_no_blur:
        # 随机选择要保留的图像
        random.shuffle(no_blur_images)
        images_to_delete = no_blur_images[target_no_blur:]

        # 删除多余的图像
        for img in images_to_delete:
            try:
                img.unlink()
                deleted_count += 1
            except Exception as e:
                print(f"    [!] 错误：无法删除 {img.name} - {e}")

        print(f"  [OK] 不带blur图像：保留 {target_no_blur} 张，删除 {len(images_to_delete)} 张")
    elif len(no_blur_images) == target_no_blur:
        print(f"  [OK] 不带blur图像：已经是 {target_no_blur} 张，无需处理")

    # 最终保留的图像数量（精确1000张）
    remaining_count = target_blur + target_no_blur

    print(f"  [SUCCESS] 结果：删除 {deleted_count} 张，保留 {remaining_count} 张（精确）")

    return deleted_count, remaining_count, True


def main():
    """主函数"""
    # 检查是否有 --yes 参数
    auto_confirm = '--yes' in sys.argv or '-y' in sys.argv

    # 设置训练数据目录
    train_dir = Path("./train")

    if not train_dir.exists():
        print(f"[X] 错误：训练数据目录不存在 - {train_dir.absolute()}")
        return

    # 获取所有类别文件夹
    category_folders = [f for f in train_dir.iterdir() if f.is_dir()]

    if not category_folders:
        print("[X] 错误：train目录中没有找到类别文件夹")
        return

    print(f"找到 {len(category_folders)} 个类别文件夹")
    print("\n" + "="*60)
    print("警告：此操作将永久删除图像文件！")
    print("每个文件夹将精确保留1000张图像（500张blur + 500张非blur）")
    print("图像数量不足的文件夹将被跳过")
    print("="*60)

    # 确认操作
    if not auto_confirm:
        user_input = input("\n是否继续？(输入 'yes' 确认): ")
        if user_input.lower() != 'yes':
            print("操作已取消")
            return
    else:
        print("\n自动确认模式：开始处理...")

    print("\n开始处理...")

    # 统计信息
    total_deleted = 0
    total_remaining = 0
    processed_folders = 0
    skipped_folders = 0

    # 处理每个类别文件夹
    for folder in sorted(category_folders):
        deleted, remaining, success = reduce_images_in_folder(folder, target_blur=500, target_no_blur=500)
        total_deleted += deleted
        total_remaining += remaining

        if success:
            processed_folders += 1
        else:
            skipped_folders += 1

    # 打印总结
    print("\n" + "="*60)
    print("处理完成！")
    print("="*60)
    print(f"总文件夹数量: {len(category_folders)}")
    print(f"成功处理的文件夹: {processed_folders}")
    print(f"跳过的文件夹: {skipped_folders}")
    print(f"总共删除的图像: {total_deleted} 张")
    print(f"成功处理文件夹的图像总数: {processed_folders * 1000} 张（每个文件夹精确1000张）")
    print("="*60)


if __name__ == "__main__":
    # 设置随机种子以便结果可复现（可选）
    random.seed(42)

    main()
