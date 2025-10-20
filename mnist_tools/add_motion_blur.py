"""
为图像添加残影效果（运动模糊）
在已处理的 128x128 图像上添加残影效果，并保存到新文件夹
"""

import os
from PIL import Image, ImageFilter
import numpy as np
from pathlib import Path
from tqdm import tqdm


def add_motion_blur(image, direction='bidirectional', intensity=40):
    """
    为图像添加残影效果（运动模糊）- 增强版

    Args:
        image: PIL Image 对象
        direction: 残影方向 ('horizontal', 'vertical', 'diagonal', 'bidirectional')
        intensity: 残影强度 (像素偏移量，建议30-60)

    Returns:
        添加残影效果后的图像
    """
    # 转换为 numpy 数组
    img_array = np.array(image).astype(np.float32)

    # 创建多层图像用于叠加 - 增加层数
    num_layers = 12  # 从5增加到12层
    result = np.zeros_like(img_array, dtype=np.float32)

    for i in range(num_layers):
        # 计算偏移量 - 使用更大的偏移
        offset = int(intensity * i / num_layers)
        # 调整透明度衰减 - 让残影更明显
        alpha = 0.8 / (i * 0.5 + 1)  # 更慢的衰减

        # 根据方向创建偏移图像
        shifted = np.zeros_like(img_array)

        if direction == 'horizontal':
            if offset < img_array.shape[1]:
                shifted[:, offset:] = img_array[:, :-offset] if offset > 0 else img_array
        elif direction == 'vertical':
            if offset < img_array.shape[0]:
                shifted[offset:, :] = img_array[:-offset, :] if offset > 0 else img_array
        elif direction == 'diagonal':
            if offset < min(img_array.shape[0], img_array.shape[1]):
                if offset > 0:
                    shifted[offset:, offset:] = img_array[:-offset, :-offset]
                else:
                    shifted = img_array
        elif direction == 'bidirectional':
            # 双向模糊 - 同时向左右两个方向
            if offset < img_array.shape[1]:
                if offset > 0:
                    # 向右的残影
                    shifted_right = np.zeros_like(img_array)
                    shifted_right[:, offset:] = img_array[:, :-offset]
                    # 向左的残影
                    shifted_left = np.zeros_like(img_array)
                    shifted_left[:, :-offset] = img_array[:, offset:]
                    # 合并左右残影
                    shifted = (shifted_right + shifted_left) / 2
                else:
                    shifted = img_array

        # 叠加到结果上
        result += shifted * alpha

    # 归一化
    result = np.clip(result, 0, 255).astype(np.uint8)

    return Image.fromarray(result)


def add_ghosting_effect(image, direction='bidirectional', intensity=0.7):
    """
    添加更自然的残影效果 - 增强版

    Args:
        image: PIL Image 对象
        direction: 残影方向 ('right', 'left', 'down', 'up', 'bidirectional')
        intensity: 残影强度 (0.3-1.0，越大越明显)

    Returns:
        添加残影效果后的图像
    """
    img_array = np.array(image).astype(np.float32)

    # 创建结果图像，从原图开始
    result = img_array.copy()

    # 添加多层衰减的残影 - 增加层数和偏移距离
    num_ghosts = 8  # 从3增加到8层
    for i in range(1, num_ghosts + 1):
        offset = i * 5  # 从3增加到5像素
        alpha = intensity / (i * 1.2)  # 更慢的衰减

        # 根据方向偏移
        ghost = np.zeros_like(img_array)

        if direction == 'right' and offset < img_array.shape[1]:
            ghost[:, offset:] = img_array[:, :-offset]
        elif direction == 'left' and offset < img_array.shape[1]:
            ghost[:, :-offset] = img_array[:, offset:]
        elif direction == 'down' and offset < img_array.shape[0]:
            ghost[offset:, :] = img_array[:-offset, :]
        elif direction == 'up' and offset < img_array.shape[0]:
            ghost[:-offset, :] = img_array[offset:, :]
        elif direction == 'bidirectional' and offset < img_array.shape[1]:
            # 双向残影 - 向左右两个方向
            ghost_right = np.zeros_like(img_array)
            ghost_right[:, offset:] = img_array[:, :-offset]
            ghost_left = np.zeros_like(img_array)
            ghost_left[:, :-offset] = img_array[:, offset:]
            # 合并左右残影
            ghost = (ghost_right + ghost_left) / 2

        # 叠加残影
        result = result + ghost * alpha

    # 归一化到 0-255
    result = np.clip(result, 0, 255).astype(np.uint8)

    return Image.fromarray(result)


def process_image_with_blur(input_path, output_path, blur_type='ghosting'):
    """
    处理单张图像，添加残影效果

    Args:
        input_path: 输入图像路径
        output_path: 输出图像路径
        blur_type: 残影类型 ('ghosting' 或 'motion')
    """
    # 读取图像
    img = Image.open(input_path)

    if blur_type == 'ghosting':
        # 添加双向残影效果
        img_blurred = add_ghosting_effect(img, direction='bidirectional', intensity=0.8)
    else:
        # 添加双向运动模糊
        img_blurred = add_motion_blur(img, direction='bidirectional', intensity=25)

    # 保存处理后的图像
    img_blurred.save(output_path, quality=95)


def main():
    """主函数"""
    # 设置输入和输出目录
    # 可以根据需要修改这些路径

    # 选项1: 处理缩放后的 128x128 图像 (推荐,默认)
    input_base_dir = Path("./resized_128x128")
    output_base_dir = Path("./resized_128x128_with_blur")

    # 选项2: 处理缩放+反转后的图像
    # input_base_dir = Path("./resized_128x128_inverted")
    # output_base_dir = Path("./resized_128x128_inverted_with_blur")

    # 选项3: 处理旋转后的图像 (需要指定朝向)
    # input_base_dir = Path("./resized_128x128_rotated/up")
    # output_base_dir = Path("./resized_128x128_rotated_with_blur/up")

    # 选项4: 处理原始图像文件夹 (不推荐,需要先缩放)
    # input_base_dir = Path("./original_images")
    # output_base_dir = Path("./original_images_with_blur")

    # 检查输入目录是否存在
    if not input_base_dir.exists():
        print(f"错误: 输入目录不存在: {input_base_dir}")
        print("请先运行 resize_and_mark.py 生成基础图像")
        return

    # 创建输出主目录
    output_base_dir.mkdir(exist_ok=True)

    # 为每个数字标签(0-9)创建子文件夹
    for digit in range(10):
        digit_dir = output_base_dir / str(digit)
        digit_dir.mkdir(exist_ok=True)

    print(f"输入目录: {input_base_dir.absolute()}")
    print(f"输出目录: {output_base_dir.absolute()}")
    print("开始添加残影效果...\n")

    # 选择残影效果类型
    blur_type = 'motion'  # 可选: 'ghosting' 或 'motion'

    # 统计信息
    stats = {
        'total': 0,
        'by_digit': {i: 0 for i in range(10)}
    }

    # 收集所有需要处理的图像
    all_images = []
    for digit in range(10):
        digit_dir = input_base_dir / str(digit)
        if digit_dir.exists():
            images = list(digit_dir.glob("*.jpg"))
            all_images.extend([(img, digit) for img in images])

    if not all_images:
        print("未找到任何图像文件！")
        return

    print(f"找到 {len(all_images)} 张图像")

    # 处理所有图像
    for img_path, digit in tqdm(all_images, desc="添加残影效果"):
        filename = img_path.name

        # 设置输出路径
        output_path = output_base_dir / str(digit) / filename

        # 处理图像
        process_image_with_blur(img_path, output_path, blur_type)

        # 更新统计
        stats['total'] += 1
        stats['by_digit'][digit] += 1

    # 打印统计信息
    print("\n" + "="*50)
    print("处理完成！")
    print(f"总共处理: {stats['total']} 张图像")
    print(f"残影效果类型: {blur_type}")
    print("\n各数字类别统计:")
    for digit in range(10):
        count = stats['by_digit'][digit]
        print(f"  数字 {digit}: {count} 张图像 -> {output_base_dir / str(digit)}")
    print(f"\n输出目录: {output_base_dir.absolute()}")
    print("="*50)


if __name__ == "__main__":
    main()
