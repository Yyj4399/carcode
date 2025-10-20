"""
将 merged_inverted 和 merged_with_blur_inverted 中的单个数字图像(01-09)
合并到 merged_combined，然后删除原文件夹中的这些图像
"""

import os
import shutil
from pathlib import Path
from tqdm import tqdm


def merge_single_digits_to_combined(source_dirs, output_dir, delete_after_merge=False):
    """
    将单个数字图像(01-09)从多个源文件夹合并到一个输出文件夹

    Args:
        source_dirs: 源文件夹路径列表
        output_dir: 输出文件夹路径
        delete_after_merge: 是否在合并后删除源文件
    """
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    # 单个数字范围: 01-09
    single_digit_folders = [f"{i:02d}" for i in range(1, 10)]

    # 统计信息
    stats = {
        'total_copied': 0,
        'total_deleted': 0,
        'by_source': {},
        'by_digit': {}
    }

    # 初始化统计
    for source_dir in source_dirs:
        stats['by_source'][source_dir.name] = {'copied': 0, 'deleted': 0}

    for digit_folder in single_digit_folders:
        stats['by_digit'][digit_folder] = {'copied': 0, 'deleted': 0}

    print(f"\n{'='*60}")
    print("单个数字图像合并任务")
    print(f"{'='*60}")
    print(f"\n目标文件夹: {digit_folder}")
    print(f"输出文件夹: {output_path.absolute()}")

    # 为每个单个数字创建输出文件夹
    print("\n正在创建输出目录结构...")
    for digit_folder in single_digit_folders:
        digit_dir = output_path / digit_folder
        digit_dir.mkdir(exist_ok=True)

    # 从每个源文件夹复制文件
    for source_dir in source_dirs:
        source_path = Path(source_dir)
        if not source_path.exists():
            print(f"警告: 源文件夹不存在: {source_path}")
            continue

        print(f"\n正在处理源文件夹: {source_path.name}")

        # 遍历每个单个数字文件夹(01-09)
        for digit_folder in single_digit_folders:
            source_digit_dir = source_path / digit_folder
            output_digit_dir = output_path / digit_folder

            if not source_digit_dir.exists():
                print(f"  文件夹 {digit_folder} 不存在，跳过")
                continue

            # 获取该文件夹中的所有 .jpg 文件
            jpg_files = list(source_digit_dir.glob("*.jpg"))

            if len(jpg_files) == 0:
                print(f"  文件夹 {digit_folder} 为空，跳过")
                continue

            print(f"  文件夹 {digit_folder}: 找到 {len(jpg_files)} 张图像")

            # 复制文件
            files_to_delete = []
            for jpg_file in tqdm(jpg_files, desc=f"    复制 {digit_folder}", leave=False):
                # 生成目标文件名 (保持原文件名)
                output_file = output_digit_dir / jpg_file.name

                # 如果文件已存在，添加源标识避免覆盖
                if output_file.exists():
                    # 获取源标识
                    source_tag = source_path.name.split('_')[-1]  # 例如: "inverted" 或 "blur_inverted"
                    # 生成新文件名
                    stem = output_file.stem
                    # 如果文件名还没有源标识，添加它
                    if source_tag not in stem:
                        new_name = f"{stem}_{source_tag}.jpg"
                        output_file = output_digit_dir / new_name

                # 复制文件
                shutil.copy2(jpg_file, output_file)

                # 记录需要删除的文件
                if delete_after_merge:
                    files_to_delete.append(jpg_file)

                # 更新统计
                stats['total_copied'] += 1
                stats['by_source'][source_path.name]['copied'] += 1
                stats['by_digit'][digit_folder]['copied'] += 1

            # 删除源文件（如果指定）
            if delete_after_merge and len(files_to_delete) > 0:
                print(f"    正在删除源文件...")
                for file_to_delete in tqdm(files_to_delete, desc=f"    删除 {digit_folder}", leave=False):
                    try:
                        file_to_delete.unlink()
                        stats['total_deleted'] += 1
                        stats['by_source'][source_path.name]['deleted'] += 1
                        stats['by_digit'][digit_folder]['deleted'] += 1
                    except Exception as e:
                        print(f"      删除失败: {file_to_delete.name} - {e}")

    # 打印统计信息
    print(f"\n{'='*60}")
    print("合并完成！")
    print(f"{'='*60}")
    print(f"\n总共复制: {stats['total_copied']} 张图像")
    if delete_after_merge:
        print(f"总共删除: {stats['total_deleted']} 张图像")
    print(f"输出目录: {output_path.absolute()}")

    print("\n按源文件夹统计:")
    for source_name, counts in stats['by_source'].items():
        print(f"  {source_name}:")
        print(f"    复制: {counts['copied']} 张")
        if delete_after_merge:
            print(f"    删除: {counts['deleted']} 张")

    print("\n按数字文件夹统计:")
    for digit_folder, counts in stats['by_digit'].items():
        print(f"  文件夹 {digit_folder}:")
        print(f"    复制: {counts['copied']} 张")
        if delete_after_merge:
            print(f"    删除: {counts['deleted']} 张")

    print(f"\n{'='*60}")

    # 尝试删除空文件夹
    if delete_after_merge:
        print("\n正在检查并删除空文件夹...")
        for source_dir in source_dirs:
            source_path = Path(source_dir)
            for digit_folder in single_digit_folders:
                source_digit_dir = source_path / digit_folder
                if source_digit_dir.exists():
                    try:
                        # 检查文件夹是否为空
                        if not any(source_digit_dir.iterdir()):
                            source_digit_dir.rmdir()
                            print(f"  已删除空文件夹: {source_digit_dir}")
                    except Exception as e:
                        print(f"  删除文件夹失败: {source_digit_dir} - {e}")


def main():
    """主函数"""

    # ===========================================
    # 配置区域 - 根据需要修改以下参数
    # ===========================================

    # 源文件夹列表 (包含单个数字01-09的图像)
    source_dirs = [
        Path("./merged_inverted"),              # 源1: 不带残影
        Path("./merged_with_blur_inverted")     # 源2: 带残影
    ]

    # 输出文件夹
    output_dir = Path("./merged_combined")

    # 是否在合并后删除源文件
    delete_after_merge = True  # 设置为 False 只复制不删除

    # ===========================================
    # 执行合并
    # ===========================================

    # 检查源文件夹是否存在
    for source_dir in source_dirs:
        if not source_dir.exists():
            print(f"错误: 源文件夹不存在: {source_dir.absolute()}")
            return

    # 显示操作信息
    if delete_after_merge:
        print("\n" + "="*60)
        print("警告: 此操作将删除原文件夹中的单个数字图像(01-09)！")
        print("="*60)
        print("\n源文件夹:")
        for source_dir in source_dirs:
            print(f"  - {source_dir.absolute()}")
        print(f"\n输出文件夹: {output_dir.absolute()}")
        print("\n将要执行的操作:")
        print("  1. 复制 01-09 文件夹中的所有图像到 merged_combined")
        print("  2. 删除原文件夹中的这些图像")
        print("  3. 删除空的 01-09 文件夹")
        print("\n正在执行...")

    # 执行合并
    merge_single_digits_to_combined(
        source_dirs=source_dirs,
        output_dir=output_dir,
        delete_after_merge=delete_after_merge
    )


if __name__ == "__main__":
    main()
