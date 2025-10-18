"""
批量处理 resized_128x128_rotated 文件夹中所有方向的图像
对每个方向的图像进行像素反转，保存到 resized_128x128_rotated_inverted
"""

from pathlib import Path
import sys

# 导入 invert_colors 模块
sys.path.append(str(Path(__file__).parent))
from invert_colors import process_images_with_inversion


def main():
    """主函数"""

    # 定义所有旋转方向
    directions = ['up', 'down', 'left', 'right']

    # 源文件夹和输出文件夹
    source_base = Path("./resized_128x128_rotated")
    output_base = Path("./resized_128x128_rotated_inverted")

    # 检查源文件夹是否存在
    if not source_base.exists():
        print(f"错误: 源文件夹不存在: {source_base.absolute()}")
        return

    print("="*60)
    print("批量处理旋转图像的像素反转")
    print("="*60)
    print(f"\n源文件夹: {source_base.absolute()}")
    print(f"输出文件夹: {output_base.absolute()}")
    print(f"\n将处理以下方向: {', '.join(directions)}")
    print("="*60)

    # 处理每个方向
    for direction in directions:
        input_dir = source_base / direction
        output_dir = output_base / direction

        if not input_dir.exists():
            print(f"\n跳过方向 {direction}: 文件夹不存在")
            continue

        print(f"\n{'='*60}")
        print(f"处理方向: {direction.upper()}")
        print(f"{'='*60}")

        # 调用像素反转函数
        process_images_with_inversion(input_dir, output_dir)

    print("\n" + "="*60)
    print("全部完成！")
    print("="*60)
    print(f"\n输出文件夹: {output_base.absolute()}")
    print("\n输出结构:")
    for direction in directions:
        output_dir = output_base / direction
        if output_dir.exists():
            # 统计每个方向的子文件夹
            subdirs = list(output_dir.glob("*_*"))
            print(f"  {direction}/")
            if subdirs:
                for subdir in sorted(subdirs)[:3]:  # 只显示前3个
                    count = len(list(subdir.glob("*.jpg")))
                    print(f"    └── {subdir.name}/ ({count} 张图像)")
                if len(subdirs) > 3:
                    print(f"    └── ... (共 {len(subdirs)} 个文件夹)")

    print("="*60)


if __name__ == "__main__":
    main()
