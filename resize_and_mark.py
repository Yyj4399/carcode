"""
MNIST 图像处理脚本
将 28x28 图像缩放到 128x128，并在标签为 6 和 9 的图像下方添加白色横杠
"""

import os
from PIL import Image, ImageDraw
from pathlib import Path
from tqdm import tqdm


def process_image(input_path, output_path, label=None):
    """
    处理单张图像

    Args:
        input_path: 输入图像路径
        output_path: 输出图像路径
        label: 图像标签（用于判断横杠位置）
    """
    # 读取原始图像
    img = Image.open(input_path)

    # 缩放到 128x128
    img_resized = img.resize((128, 128), Image.Resampling.LANCZOS)

    # 如果需要添加横杠（针对 6 和 9）
    if label == 6 or label == 9:
        # 转换为 RGB 模式以便绘制白色横杠
        if img_resized.mode != 'RGB':
            img_resized = img_resized.convert('RGB')

        draw = ImageDraw.Draw(img_resized)

        # 根据标签设置不同的横杠位置
        if label == 6:
            # 数字 6: 距离底端 16 像素 (y = 112)
            line_y = int(128 * 0.875)
        else:  # label == 9
            # 数字 9: 距离底端 8 像素 (原来16像素的1/2, y = 120)
            line_y = int(128 * 0.9375)

        line_thickness = 3  # 横杠厚度
        line_start_x = int(128 * 0.2)  # 横杠起始 x 坐标
        line_end_x = int(128 * 0.8)  # 横杠结束 x 坐标

        # 绘制白色横杠
        for i in range(line_thickness):
            draw.line(
                [(line_start_x, line_y + i), (line_end_x, line_y + i)],
                fill=(255, 255, 255),
                width=1
            )

    # 保存处理后的图像
    img_resized.save(output_path, quality=95)


def main():
    """主函数"""
    # 设置输入和输出目录
    input_dir = Path(".")  # 当前目录
    output_dir = Path("./resized_128x128")

    # 创建输出目录
    output_dir.mkdir(exist_ok=True)

    # 为每个数字标签(0-9)创建子文件夹
    for digit in range(10):
        digit_dir = output_dir / str(digit)
        digit_dir.mkdir(exist_ok=True)

    # 获取所有 jpg 文件
    image_files = list(input_dir.glob("*.jpg"))

    if not image_files:
        print("未找到任何 .jpg 图像文件！")
        return

    print(f"找到 {len(image_files)} 张图像")
    print(f"输出目录: {output_dir.absolute()}")
    print("开始处理...\n")

    # 统计信息
    stats = {
        'total': 0,
        'with_line': 0,
        'without_line': 0,
        'by_digit': {i: 0 for i in range(10)}  # 每个数字的统计
    }

    # 处理每张图像
    for img_path in tqdm(image_files, desc="处理进度"):
        filename = img_path.name

        # 从文件名提取标签
        # 格式: training_1234_7.jpg 或 test_1234_7.jpg
        try:
            parts = filename.replace('.jpg', '').split('_')
            label = int(parts[-1])  # 最后一部分是标签
        except (IndexError, ValueError):
            print(f"警告: 无法解析文件名 {filename}，跳过")
            continue

        # 判断是否需要添加横杠（标签为 6 或 9）
        add_line = (label == 6 or label == 9)

        # 设置输出路径 - 保存到对应数字的文件夹中
        output_path = output_dir / str(label) / filename

        # 处理图像,传入标签以便设置不同的横杠位置
        process_image(img_path, output_path, label=label)

        # 更新统计信息
        stats['total'] += 1
        stats['by_digit'][label] += 1
        if add_line:
            stats['with_line'] += 1
        else:
            stats['without_line'] += 1

    # 打印统计信息
    print("\n" + "="*50)
    print("处理完成！")
    print(f"总共处理: {stats['total']} 张图像")
    print(f"添加横杠: {stats['with_line']} 张 (标签 6 和 9)")
    print(f"未添加横杠: {stats['without_line']} 张")
    print("\n各数字类别统计:")
    for digit in range(10):
        count = stats['by_digit'][digit]
        print(f"  数字 {digit}: {count} 张图像 -> {output_dir / str(digit)}")
    print(f"\n输出目录: {output_dir.absolute()}")
    print("="*50)


if __name__ == "__main__":
    main()
