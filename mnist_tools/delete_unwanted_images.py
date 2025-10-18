"""
删除文件名中包含"副本"或"_"的图像文件

使用方法:
    python delete_unwanted_images.py

配置说明:
    - INPUT_DIR: 要处理的文件夹路径
    - 支持的图像格式: .jpg, .jpeg, .png, .bmp, .gif
    - 删除包含"副本"或"_"的图像文件
"""

import os
from pathlib import Path

# ============ 配置区域 ============
# 输入文件夹路径 - 请根据需要修改
INPUT_DIR = r"D:\ACS image photos\image - 副本\train\wrench"

# 支持的图像格式
IMAGE_EXTENSIONS = {'.jpg', '.jpeg', '.png', '.bmp', '.gif'}

# 要删除的文件名特征
DELETE_PATTERNS = ['副本', '_']
# =================================


def should_delete(filename: str) -> bool:
    """
    判断文件是否应该被删除

    Args:
        filename: 文件名

    Returns:
        bool: 如果文件名包含指定模式则返回True
    """
    for pattern in DELETE_PATTERNS:
        if pattern in filename:
            return True
    return False


def delete_unwanted_images(input_dir: str, dry_run: bool = True):
    """
    删除符合条件的图像文件

    Args:
        input_dir: 输入文件夹路径
        dry_run: 如果为True，只显示将要删除的文件，不实际删除
    """
    input_path = Path(input_dir)

    if not input_path.exists():
        print(f"❌ 错误: 文件夹不存在: {input_dir}")
        return

    # 统计信息
    total_images = 0
    deleted_count = 0
    files_to_delete = []

    print(f"🔍 正在扫描文件夹: {input_dir}")
    print(f"🎯 删除模式: {DELETE_PATTERNS}")
    print(f"📁 支持的图像格式: {', '.join(IMAGE_EXTENSIONS)}\n")

    # 遍历所有文件（包括子文件夹）
    for root, dirs, files in os.walk(input_path):
        for file in files:
            # 检查是否是图像文件
            file_ext = Path(file).suffix.lower()
            if file_ext in IMAGE_EXTENSIONS:
                total_images += 1

                # 检查是否应该删除
                if should_delete(file):
                    file_path = Path(root) / file
                    files_to_delete.append(file_path)
                    deleted_count += 1

    # 显示将要删除的文件
    if files_to_delete:
        print(f"📋 找到 {deleted_count} 个符合条件的文件:\n")
        for file_path in files_to_delete:
            print(f"  - {file_path}")

        print(f"\n📊 统计:")
        print(f"  总图像文件数: {total_images}")
        print(f"  将要删除: {deleted_count}")
        print(f"  保留: {total_images - deleted_count}")

        if dry_run:
            print("\n⚠️  这是预览模式，没有实际删除文件")
            print("💡 如需实际删除，请修改代码中的 dry_run=False")
        else:
            # 确认删除
            confirm = input("\n⚠️  确定要删除这些文件吗? (输入 'yes' 确认): ")
            if confirm.lower() == 'yes':
                for file_path in files_to_delete:
                    try:
                        os.remove(file_path)
                        print(f"✅ 已删除: {file_path}")
                    except Exception as e:
                        print(f"❌ 删除失败 {file_path}: {e}")
                print(f"\n✅ 删除完成! 共删除 {deleted_count} 个文件")
            else:
                print("❌ 已取消删除操作")
    else:
        print(f"✅ 没有找到符合条件的文件")
        print(f"📊 总图像文件数: {total_images}")


if __name__ == "__main__":
    # 预览模式 - 只显示将要删除的文件，不实际删除
    # 如需实际删除，请将 dry_run 改为 False
    delete_unwanted_images(INPUT_DIR, dry_run=False)
