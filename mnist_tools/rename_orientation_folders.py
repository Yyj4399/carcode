"""
重命名旋转文件夹中的子文件夹后缀
"""
import os
from pathlib import Path

def rename_subfolders(base_dir):
    """
    重命名指定目录下的子文件夹后缀

    映射关系:
    - down 文件夹中: *_left -> *_down
    - left 文件夹中: *_down -> *_left
    - up 文件夹中: *_right -> *_up
    - right 文件夹中: *_up -> *_right
    """
    base_path = Path(base_dir)

    # 定义重命名映射（交换文件夹后，子文件夹的后缀也需要跟着改）
    # 新的映射关系: down→left, left→down, up→right, right→up
    rename_mapping = {
        'down': ('left', 'down'),    # 原来是left文件夹移动过来的，所以后缀是_left，改为_down
        'left': ('down', 'left'),    # 原来是down文件夹移动过来的，所以后缀是_down，改为_left
        'up': ('right', 'up'),       # 原来是right文件夹移动过来的，所以后缀是_right，改为_up
        'right': ('up', 'right')     # 原来是up文件夹移动过来的，所以后缀是_up，改为_right
    }

    print(f"\n正在处理: {base_path}")
    print("="*60)

    for orientation_folder, (old_suffix, new_suffix) in rename_mapping.items():
        orientation_path = base_path / orientation_folder

        if not orientation_path.exists():
            print(f"跳过不存在的文件夹: {orientation_folder}")
            continue

        print(f"\n处理 {orientation_folder} 文件夹:")

        renamed_count = 0
        for item in orientation_path.iterdir():
            if item.is_dir() and item.name.endswith(f'_{old_suffix}'):
                # 生成新名称
                digit = item.name.split('_')[0]
                new_name = f"{digit}_{new_suffix}"
                new_path = orientation_path / new_name

                # 重命名
                item.rename(new_path)
                print(f"  {item.name} -> {new_name}")
                renamed_count += 1

        if renamed_count == 0:
            print(f"  没有找到需要重命名的文件夹 (*_{old_suffix})")
        else:
            print(f"  共重命名 {renamed_count} 个文件夹")

    print("\n" + "="*60)
    print("重命名完成!")

def main():
    """主函数"""
    # 需要处理的基础文件夹列表
    base_folders = [
        "./resized_128x128_rotated_inverted",
        "./resized_128x128_with_blur_rotated",
        "./resized_128x128_with_blur_inverted_rotated"
    ]

    for folder in base_folders:
        folder_path = Path(folder)
        if folder_path.exists():
            rename_subfolders(folder_path)
        else:
            print(f"\n警告: 文件夹不存在，跳过: {folder}")

if __name__ == "__main__":
    main()
