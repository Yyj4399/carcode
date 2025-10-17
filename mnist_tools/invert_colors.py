"""
MNIST 图像颜色反转脚本
将图像的黑白像素反转(黑变白,白变黑)
"""

import os
from PIL import Image, ImageOps
import numpy as np
from pathlib import Path
from tqdm import tqdm


def invert_image(image):
    """
    反转图像的黑白像素

    Args:
        image: PIL Image 对象

    Returns:
        反转后的图像
    """
    # 使用 PIL 的 invert 函数反转颜色
    return ImageOps.invert(image.convert('RGB')).convert('L')


def invert_image_numpy(image):
    """
    使用 numpy 反转图像的黑白像素(备用方法)

    Args:
        image: PIL Image 对象

    Returns:
        反转后的图像
    """
    # 转换为 numpy 数组
    img_array = np.array(image)

    # 反转像素值: 255 - 原值
    inverted_array = 255 - img_array

    # 转回 PIL Image
    return Image.fromarray(inverted_array)


def process_images_with_inversion(input_dir, output_dir):
    """
    处理所有图像,反转黑白像素

    Args:
        input_dir: 输入目录路径
        output_dir: 输出目录路径
    """
    input_path = Path(input_dir)
    output_path = Path(output_dir)

    # 检查输入目录是否存在
    if not input_path.exists():
        print(f"错误: 输入目录不存在: {input_path}")
        return

    # 创建输出目录
    output_path.mkdir(parents=True, exist_ok=True)

    # 收集所有需要处理的图像
    # 支持两种输入目录结构:
    # 1. 直接在根目录下的图像 (training_*.jpg, test_*.jpg)
    # 2. 按数字分类的子文件夹结构 (0/*.jpg, 1/*.jpg, ...)

    all_images = []

    # 检查是否有数字子文件夹
    has_digit_folders = any((input_path / str(d)).exists() for d in range(10))

    if has_digit_folders:
        # 从数字子文件夹中收集图像
        print("检测到按数字分类的文件夹结构")

        # 为每个数字标签(0-9)创建输出子文件夹
        for digit in range(10):
            digit_output_dir = output_path / str(digit)
            digit_output_dir.mkdir(exist_ok=True)

            digit_dir = input_path / str(digit)
            if digit_dir.exists():
                images = list(digit_dir.glob("*.jpg"))
                all_images.extend([(img, digit) for img in images])
    else:
        # 从根目录收集图像
        print("检测到根目录图像结构")
        images = list(input_path.glob("*.jpg"))
        for img_path in images:
            filename = img_path.name
            try:
                # 从文件名提取标签 (格式: training_1234_7.jpg 或 test_1234_7.jpg)
                parts = filename.replace('.jpg', '').split('_')
                label = int(parts[-1])
                all_images.append((img_path, label))
            except (IndexError, ValueError):
                # 如果无法提取标签,使用 None
                all_images.append((img_path, None))

    if not all_images:
        print("未找到任何图像文件!")
        return

    print(f"\n找到 {len(all_images)} 张图像")
    print(f"输入目录: {input_path.absolute()}")
    print(f"输出目录: {output_path.absolute()}")
    print("开始反转黑白像素...\n")

    # 统计信息
    stats = {
        'total': 0,
        'success': 0,
        'failed': 0,
        'by_digit': {i: 0 for i in range(10)}
    }

    # 处理每张图像
    for img_path, label in tqdm(all_images, desc="处理进度"):
        filename = img_path.name

        # 生成新的文件名
        name_without_ext = filename.replace('.jpg', '')
        new_filename = f"{name_without_ext}_inverted.jpg"

        # 设置输出路径
        if has_digit_folders and label is not None:
            output_file = output_path / str(label) / new_filename
        else:
            output_file = output_path / new_filename

        # 处理图像
        try:
            # 读取图像
            img = Image.open(img_path)

            # 反转黑白像素
            inverted_img = invert_image_numpy(img)

            # 保存图像
            inverted_img.save(output_file, quality=95)

            # 更新统计
            stats['total'] += 1
            stats['success'] += 1
            if label is not None:
                stats['by_digit'][label] += 1

        except Exception as e:
            print(f"\n警告: 处理图像 {filename} 时出错: {e}")
            stats['failed'] += 1
            continue

    # 打印统计信息
    print("\n" + "="*60)
    print("处理完成!")
    print(f"总共处理: {stats['total']} 张图像")
    print(f"成功: {stats['success']} 张")
    if stats['failed'] > 0:
        print(f"失败: {stats['failed']} 张")

    if has_digit_folders:
        print("\n按数字类别统计:")
        for digit in range(10):
            count = stats['by_digit'][digit]
            if count > 0:
                print(f"  数字 {digit}: {count:6d} 张 -> {output_path / str(digit)}")

    print(f"\n输出目录: {output_path.absolute()}")
    print("="*60)


def main():
    """主函数"""
    # 设置输入和输出目录
    # 可以根据需要修改这些路径

    # 选项1: 处理原始图像文件夹
    # input_dir = "./original_images"
    # output_dir = "./original_images_inverted"

    # 选项2: 处理缩放后的 128x128 图像
    # input_dir = "./resized_128x128"
    # output_dir = "./resized_128x128_inverted"

    # 选项3: 处理带残影效果的图像 (推荐,默认)
    input_dir = "./resized_128x128_with_blur"
    output_dir = "./resized_128x128_with_blur_inverted"

    # 选项4: 处理旋转后的图像 (需要指定朝向)
    # input_dir = "./resized_128x128_rotated/up"
    # output_dir = "./resized_128x128_rotated_inverted/up"

    # 选项5: 处理带残影+旋转的图像 (需要指定朝向)
    # input_dir = "./resized_128x128_with_blur_rotated/up"
    # output_dir = "./resized_128x128_with_blur_rotated_inverted/up"

    print("="*60)
    print("MNIST 图像黑白反转脚本")
    print("="*60)
    print(f"\n配置:")
    print(f"  输入目录: {input_dir}")
    print(f"  输出目录: {output_dir}")
    print(f"  效果: 黑色变白色, 白色变黑色")
    print()

    # 开始处理
    process_images_with_inversion(input_dir, output_dir)


if __name__ == "__main__":
    main()
