"""
MNIST 图像旋转脚本
将图像旋转为4个不同的朝向(上、下、左、右)并保存到不同文件夹
"""

import os
from PIL import Image
from pathlib import Path
from tqdm import tqdm


def rotate_image(image, angle):
    """
    旋转图像

    Args:
        image: PIL Image 对象
        angle: 旋转角度 (0, 90, 180, 270)

    Returns:
        旋转后的图像
    """
    return image.rotate(angle, expand=False, fillcolor=0)


def process_images_with_rotation(input_dir, output_base_dir):
    """
    处理所有图像,生成4个旋转方向的版本

    Args:
        input_dir: 输入目录路径
        output_base_dir: 输出基础目录路径
    """
    input_path = Path(input_dir)
    output_path = Path(output_base_dir)

    # 检查输入目录是否存在
    if not input_path.exists():
        print(f"错误: 输入目录不存在: {input_path}")
        return

    # 根据输入目录名称提取文件名后缀
    # 检查输入路径中包含的特征关键词
    # 注意: 统一处理的都是反转后的图像,所以不添加 _inverted 后缀
    input_dir_lower = str(input_path).lower()
    filename_suffix = ""

    if "blur" in input_dir_lower:
        filename_suffix = "_blur"

    # 定义4个旋转角度和对应的文件夹名称(上下左右四个朝向)
    rotations = {
        0: "up",        # 上(原始方向)
        180: "down",    # 下(旋转180度)
        90: "right",    # 右(顺时针旋转90度)
        270: "left"     # 左(顺时针旋转270度)
    }

    # 创建输出目录结构
    # 为每个旋转角度创建主文件夹,每个主文件夹下再创建0-9的子文件夹
    for angle, folder_name in rotations.items():
        rotation_dir = output_path / folder_name
        rotation_dir.mkdir(parents=True, exist_ok=True)

        # 为每个数字标签(0-9)创建子文件夹,格式为 {数字}_{朝向}
        for digit in range(10):
            digit_dir = rotation_dir / f"{digit}_{folder_name}"
            digit_dir.mkdir(exist_ok=True)

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
        for digit in range(10):
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
                print(f"警告: 无法解析文件名 {filename},跳过")
                continue

    if not all_images:
        print("未找到任何图像文件!")
        return

    print(f"\n找到 {len(all_images)} 张图像")
    print(f"输入目录: {input_path.absolute()}")
    print(f"输出目录: {output_path.absolute()}")
    print(f"文件名后缀: {filename_suffix if filename_suffix else '(无)'}")
    print(f"将生成 {len(all_images) * 4} 张旋转图像\n")

    # 统计信息
    stats = {
        'total_input': len(all_images),
        'total_output': 0,
        'by_rotation': {angle: 0 for angle in rotations.keys()},
        'by_digit': {i: 0 for i in range(10)}
    }

    # 处理每张图像
    for img_path, label in tqdm(all_images, desc="处理进度"):
        filename = img_path.name

        # 读取原始图像
        try:
            img = Image.open(img_path)
        except Exception as e:
            print(f"\n警告: 无法读取图像 {filename}: {e}")
            continue

        # 生成4个旋转版本
        for angle, folder_name in rotations.items():
            # 旋转图像
            if angle == 0:
                rotated_img = img
            else:
                rotated_img = rotate_image(img, angle)

            # 生成新的文件名
            # 从原文件名中提取基础部分(移除可能已有的后缀)
            name_without_ext = filename.replace('.jpg', '')

            # 构建新文件名: 基础名 + 特征后缀 + 朝向后缀
            # 例如: training_123_5_blur_inverted_up.jpg
            new_filename = f"{name_without_ext}{filename_suffix}_{folder_name}.jpg"

            # 设置输出路径,子文件夹格式为 {数字}_{朝向}
            output_file = output_path / folder_name / f"{label}_{folder_name}" / new_filename

            # 保存图像
            try:
                rotated_img.save(output_file, quality=95)
                stats['total_output'] += 1
                stats['by_rotation'][angle] += 1
            except Exception as e:
                print(f"\n警告: 无法保存图像 {new_filename}: {e}")
                continue

        # 更新数字统计
        stats['by_digit'][label] += 4  # 每张图像生成4个版本

    # 打印统计信息
    print("\n" + "="*60)
    print("处理完成!")
    print(f"输入图像: {stats['total_input']} 张")
    print(f"输出图像: {stats['total_output']} 张")
    print("\n按朝向统计:")
    for angle, folder_name in rotations.items():
        count = stats['by_rotation'][angle]
        print(f"  {folder_name:6s} ({angle:3d}度): {count:6d} 张 -> {output_path / folder_name}")

    print("\n按数字类别统计 (包含所有旋转方向):")
    for digit in range(10):
        count = stats['by_digit'][digit]
        print(f"  数字 {digit}: {count:6d} 张图像")

    print(f"\n输出目录: {output_path.absolute()}")
    print("="*60)


def main():
    """主函数"""
    # 设置输入和输出目录
    # 可以根据需要修改这些路径

    # 选项1: 处理原始图像文件夹
    # input_dir = "./original_images"
    # output_dir = "./original_images_rotated"

    # 选项2: 处理缩放后的 128x128 图像
    # input_dir = "./resized_128x128"
    # output_dir = "./resized_128x128_rotated"

    # 选项3: 处理缩放+反转后的图像
    # input_dir = "./resized_128x128_inverted"
    # output_dir = "./resized_128x128_inverted_rotated"

    # 选项4: 处理带残影效果的图像
    # input_dir = "./resized_128x128_with_blur"
    # output_dir = "./resized_128x128_with_blur_rotated"

    # 选项5: 处理带残影效果+反转的图像 (推荐,默认)
    input_dir = "./resized_128x128_with_blur_inverted"
    output_dir = "./resized_128x128_with_blur_inverted_rotated"

    # 选项6: 处理缩放+反转+残影的图像 (另一种处理顺序)
    # input_dir = "./resized_128x128_inverted_with_blur"
    # output_dir = "./resized_128x128_inverted_with_blur_rotated"

    print("="*60)
    print("MNIST 图像旋转脚本 - 上下左右四个朝向")
    print("="*60)
    print(f"\n配置:")
    print(f"  输入目录: {input_dir}")
    print(f"  输出目录: {output_dir}")
    print(f"  朝向: 上(0°), 下(180°), 左(270°), 右(90°)")
    print()

    # 开始处理
    process_images_with_rotation(input_dir, output_dir)


if __name__ == "__main__":
    main()
