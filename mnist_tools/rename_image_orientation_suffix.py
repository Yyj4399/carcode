"""
批量重命名图片文件中的朝向后缀
使其与实际数字朝向一致
"""
import os
from pathlib import Path
from tqdm import tqdm


def rename_image_files(base_dir):
    """
    重命名指定目录下所有图片文件的朝向后缀

    映射关系 (基于实际朝向):
    - down 文件夹中: *_right*.jpg -> *_down*.jpg
    - left 文件夹中: *_up*.jpg -> *_left*.jpg
    - up 文件夹中: *_left*.jpg -> *_up*.jpg
    - right 文件夹中: *_down*.jpg -> *_right*.jpg

    处理文件名格式如: test_10_0_right_inverted.jpg
    """
    base_path = Path(base_dir)

    # 定义重命名映射
    # 文件夹名 -> (旧后缀, 新后缀)
    # 文件夹已经交换过了，所以需要根据当前文件夹中实际的文件名后缀来映射
    rename_mapping = {
        'down': ('_right', '_down'),   # down文件夹中实际是_right后缀，改为_down
        'left': ('_up', '_left'),      # left文件夹中实际是_up后缀，改为_left
        'up': ('_left', '_up'),        # up文件夹中实际是_left后缀，改为_up
        'right': ('_down', '_right')   # right文件夹中实际是_down后缀，改为_right
    }

    print(f"\n正在处理: {base_path}")
    print("="*60)

    total_renamed = 0

    for orientation_folder, (old_suffix, new_suffix) in rename_mapping.items():
        orientation_path = base_path / orientation_folder

        if not orientation_path.exists():
            print(f"跳过不存在的文件夹: {orientation_folder}")
            continue

        print(f"\n处理 {orientation_folder} 文件夹:")

        # 遍历所有子文件夹（如 0_down, 1_down 等）
        subfolder_count = 0
        for subfolder in sorted(orientation_path.iterdir()):
            if not subfolder.is_dir():
                continue

            # 查找所有包含旧朝向后缀的图片文件
            all_images = list(subfolder.glob("*.jpg"))
            image_files = [img for img in all_images if old_suffix in img.name]

            if len(image_files) == 0:
                continue

            subfolder_count += 1
            renamed_count = 0

            # 重命名图片文件
            for img_file in tqdm(image_files, desc=f"  {subfolder.name}", leave=False):
                # 生成新文件名 (替换朝向后缀)
                old_name = img_file.name
                new_name = old_name.replace(old_suffix, new_suffix)
                new_path = img_file.parent / new_name

                # 重命名
                try:
                    img_file.rename(new_path)
                    renamed_count += 1
                except Exception as e:
                    print(f"    错误: 无法重命名 {old_name}: {e}")

            total_renamed += renamed_count

        if subfolder_count == 0:
            print(f"  没有找到需要重命名的文件 (*{old_suffix}*.jpg)")
        else:
            print(f"  处理了 {subfolder_count} 个子文件夹")

    print("\n" + "="*60)
    print(f"重命名完成! 共重命名 {total_renamed} 个图片文件")
    print("="*60)


def main():
    """主函数"""
    # 需要处理的基础文件夹列表
    base_folders = [
        "./resized_128x128_rotated",
        "./resized_128x128_rotated_inverted",
        "./resized_128x128_with_blur_rotated",
        "./resized_128x128_with_blur_inverted_rotated"
    ]

    print("\n" + "="*60)
    print("批量重命名图片文件的朝向后缀")
    print("="*60)
    print("\n警告: 此操作将修改大量图片文件名!")
    print("\n将要处理的文件夹:")
    for folder in base_folders:
        folder_path = Path(folder)
        if folder_path.exists():
            print(f"  [存在] {folder}")
        else:
            print(f"  [不存在] {folder}")

    print("\n映射关系:")
    print("  - down 文件夹中: *_right*.jpg → *_down*.jpg")
    print("  - left 文件夹中: *_up*.jpg → *_left*.jpg")
    print("  - up 文件夹中: *_left*.jpg → *_up*.jpg")
    print("  - right 文件夹中: *_down*.jpg → *_right*.jpg")

    input("\n按 Enter 键开始处理...")

    for folder in base_folders:
        folder_path = Path(folder)
        if folder_path.exists():
            rename_image_files(folder_path)
        else:
            print(f"\n警告: 文件夹不存在，跳过: {folder}")

    print("\n所有文件夹处理完成!")


if __name__ == "__main__":
    main()
