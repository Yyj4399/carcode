"""
合并带残影和不带残影的二位数图像到一个新文件夹
从 merged_inverted 和 merged_with_blur_inverted 复制所有图像到 merged_combined
保持原有文件夹结构不变
"""

import os
import shutil
from pathlib import Path
from tqdm import tqdm


def combine_merged_folders(source_dirs, output_dir):
    """
    将多个源文件夹的图像合并到一个新文件夹

    Args:
        source_dirs: 源文件夹路径列表 (每个源包含10-99的子文件夹)
        output_dir: 输出文件夹路径
    """
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    # 统计信息
    stats = {
        'total_files': 0,
        'by_source': {},
        'by_number': {}
    }

    # 初始化统计
    for source_dir in source_dirs:
        stats['by_source'][source_dir.name] = 0

    # 为每个两位数创建输出子文件夹 (10-99)
    print("\n正在创建输出目录结构...")
    for digit1 in range(1, 10):  # 十位: 1-9
        for digit2 in range(10):  # 个位: 0-9
            number = digit1 * 10 + digit2
            number_dir = output_path / f"{number:02d}"
            number_dir.mkdir(exist_ok=True)
            stats['by_number'][number] = 0

    # 从每个源文件夹复制文件
    for source_dir in source_dirs:
        source_path = Path(source_dir)
        if not source_path.exists():
            print(f"警告: 源文件夹不存在: {source_path}")
            continue

        print(f"\n正在处理源文件夹: {source_path.name}")

        # 遍历该源中的每个两位数文件夹
        for digit1 in range(1, 10):  # 十位: 1-9
            for digit2 in range(10):  # 个位: 0-9
                number = digit1 * 10 + digit2
                number_str = f"{number:02d}"

                source_number_dir = source_path / number_str
                output_number_dir = output_path / number_str

                # 如果该源文件夹存在
                if not source_number_dir.exists():
                    continue

                # 获取该文件夹中的所有 .jpg 文件
                jpg_files = list(source_number_dir.glob("*.jpg"))

                # 复制文件
                for jpg_file in tqdm(jpg_files, desc=f"  复制 {number_str}", leave=False):
                    # 生成目标文件名 (保持原文件名)
                    output_file = output_number_dir / jpg_file.name

                    # 如果文件已存在,添加源标识避免覆盖
                    if output_file.exists():
                        # 获取源标识
                        source_tag = "_plain" if "blur" not in source_path.name else "_blur"
                        # 生成新文件名
                        stem = output_file.stem
                        new_name = f"{stem}{source_tag}.jpg"
                        output_file = output_number_dir / new_name

                    # 复制文件
                    shutil.copy2(jpg_file, output_file)

                    # 更新统计
                    stats['total_files'] += 1
                    stats['by_source'][source_path.name] += 1
                    stats['by_number'][number] += 1

    # 打印统计信息
    print("\n" + "="*60)
    print("合并完成!")
    print(f"总共复制: {stats['total_files']} 张图像")
    print(f"输出目录: {output_path.absolute()}")

    print("\n按源文件夹统计:")
    for source_name, count in stats['by_source'].items():
        percentage = (count / stats['total_files'] * 100) if stats['total_files'] > 0 else 0
        print(f"  {source_name}: {count} 张 ({percentage:.1f}%)")

    print("\n每个两位数的总图像数 (前10个):")
    sorted_numbers = sorted(stats['by_number'].items(), key=lambda x: x[1], reverse=True)[:10]
    for number, count in sorted_numbers:
        print(f"  {number:02d}: {count} 张")

    print("="*60)


def main():
    """主函数"""

    # ===========================================
    # 配置区域 - 根据需要修改以下参数
    # ===========================================

    # 源文件夹列表 (包含不带残影和带残影的二位数图像)
    source_dirs = [
        Path("./merged_inverted"),              # 不带残影的二位数
        Path("./merged_with_blur_inverted")     # 带残影的二位数
    ]

    # 输出文件夹
    output_dir = Path("./merged_combined")

    # ===========================================
    # 执行合并
    # ===========================================

    # 检查源文件夹是否存在
    for source_dir in source_dirs:
        if not source_dir.exists():
            print(f"错误: 源文件夹不存在: {source_dir.absolute()}")
            return

    print("\n" + "="*60)
    print("开始合并二位数图像文件夹")
    print("="*60)
    print(f"\n源文件夹:")
    for source_dir in source_dirs:
        print(f"  - {source_dir.absolute()}")
    print(f"\n输出文件夹: {output_dir.absolute()}")

    # 执行合并
    combine_merged_folders(source_dirs, output_dir)


if __name__ == "__main__":
    main()
