"""
批量重命名旋转文件夹的子文件夹,添加朝向后缀
使子文件夹名称与图像文件名的朝向一致
"""
import os
from pathlib import Path
from tqdm import tqdm


def rename_subfolders_with_orientation(base_dir):
    """
    重命名指定目录下的子文件夹,添加朝向后缀

    例如:
    - down/0/ -> down/0_down/
    - up/5/ -> up/5_up/
    """
    base_path = Path(base_dir)

    # 朝向列表
    orientations = ['down', 'left', 'up', 'right']

    print(f"\n正在处理: {base_path}")
    print("="*60)

    total_renamed = 0

    for orientation in orientations:
        orientation_path = base_path / orientation

        if not orientation_path.exists():
            print(f"跳过不存在的文件夹: {orientation}")
            continue

        print(f"\n处理 {orientation} 文件夹:")

        # 获取所有子文件夹
        subfolders = [f for f in orientation_path.iterdir() if f.is_dir()]

        # 过滤出只有数字名称的文件夹(没有朝向后缀的)
        folders_to_rename = []
        for folder in subfolders:
            # 检查文件夹名是否只是数字(0-9)
            if folder.name.isdigit() and len(folder.name) == 1:
                folders_to_rename.append(folder)

        if len(folders_to_rename) == 0:
            print(f"  没有需要重命名的文件夹")
            continue

        # 重命名子文件夹
        renamed_count = 0
        for folder in tqdm(folders_to_rename, desc=f"  重命名 {orientation}"):
            digit = folder.name
            new_name = f"{digit}_{orientation}"
            new_path = folder.parent / new_name

            # 检查目标文件夹是否已存在
            if new_path.exists():
                print(f"    警告: {new_name} 已存在,跳过 {folder.name}")
                continue

            try:
                folder.rename(new_path)
                renamed_count += 1
            except Exception as e:
                print(f"    错误: 无法重命名 {folder.name}: {e}")

        total_renamed += renamed_count
        print(f"  成功重命名 {renamed_count} 个文件夹")

    print("\n" + "="*60)
    print(f"重命名完成! 共重命名 {total_renamed} 个子文件夹")
    print("="*60)


def main():
    """主函数"""
    # 需要处理的基础文件夹列表
    base_folders = [
        "./resized_128x128_rotated",
        "./resized_128x128_rotated_inverted"
    ]

    print("\n" + "="*60)
    print("批量重命名旋转文件夹的子文件夹,添加朝向后缀")
    print("="*60)
    print("\n警告: 此操作将修改文件夹结构!")
    print("\n将要处理的文件夹:")
    for folder in base_folders:
        folder_path = Path(folder)
        if folder_path.exists():
            print(f"  [存在] {folder}")
        else:
            print(f"  [不存在] {folder}")

    print("\n映射关系:")
    print("  - down/0/ → down/0_down/")
    print("  - left/5/ → left/5_left/")
    print("  - up/9/ → up/9_up/")
    print("  - right/3/ → right/3_right/")

    input("\n按 Enter 键开始处理...")

    for folder in base_folders:
        folder_path = Path(folder)
        if folder_path.exists():
            rename_subfolders_with_orientation(folder_path)
        else:
            print(f"\n警告: 文件夹不存在,跳过: {folder}")

    print("\n所有文件夹处理完成!")


if __name__ == "__main__":
    main()
