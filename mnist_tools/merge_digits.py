"""
MNIST 数字融合脚本
将两个单独的数字图像融合成一个包含两位数的 128x128 图像
"""

import os
import random
import numpy as np
from PIL import Image
from pathlib import Path
from tqdm import tqdm


def detect_image_type(img, threshold=128):
    """
    检测图像是黑底白字还是白底黑字

    Args:
        img: 输入图像
        threshold: 像素值阈值

    Returns:
        'black_bg' 表示黑底白字，'white_bg' 表示白底黑字
    """
    # 转换为灰度并计算平均像素值
    img_array = np.array(img.convert('L'))
    mean_value = np.mean(img_array)

    # 如果平均值小于128，说明整体偏黑，是黑底白字
    if mean_value < threshold:
        return 'black_bg'
    else:
        return 'white_bg'


def crop_blackspace(img, threshold=15):
    """
    裁剪图像周围的黑边,保留数字主体(适用于黑底白字图像)

    Args:
        img: 输入图像(黑底白字)
        threshold: 像素值阈值,低于此值视为黑色

    Returns:
        裁剪后的图像
    """
    # 转换为灰度数组
    img_array = np.array(img.convert('L'))

    # 找到非黑色区域(即白色数字区域)
    non_black = img_array > threshold

    # 获取非黑色区域的边界
    rows = np.any(non_black, axis=1)
    cols = np.any(non_black, axis=0)

    if not np.any(rows) or not np.any(cols):
        # 如果全是黑色,返回原图
        return img

    row_min, row_max = np.where(rows)[0][[0, -1]]
    col_min, col_max = np.where(cols)[0][[0, -1]]

    # 添加更大的边距(5像素),确保不裁剪到数字本体
    margin = 5
    row_min = max(0, row_min - margin)
    row_max = min(img_array.shape[0] - 1, row_max + margin)
    col_min = max(0, col_min - margin)
    col_max = min(img_array.shape[1] - 1, col_max + margin)

    # 裁剪图像
    return img.crop((col_min, row_min, col_max + 1, row_max + 1))


def crop_whitespace(img, threshold=240):
    """
    裁剪图像周围的白边,保留数字主体(适用于白底黑字图像)

    Args:
        img: 输入图像(白底黑字)
        threshold: 像素值阈值,高于此值视为白色

    Returns:
        裁剪后的图像
    """
    # 转换为灰度数组
    img_array = np.array(img.convert('L'))

    # 找到非白色区域(即黑色数字区域)
    non_white = img_array < threshold

    # 获取非白色区域的边界
    rows = np.any(non_white, axis=1)
    cols = np.any(non_white, axis=0)

    if not np.any(rows) or not np.any(cols):
        # 如果全是白色,返回原图
        return img

    row_min, row_max = np.where(rows)[0][[0, -1]]
    col_min, col_max = np.where(cols)[0][[0, -1]]

    # 添加更大的边距(5像素),确保不裁剪到数字本体
    margin = 5
    row_min = max(0, row_min - margin)
    row_max = min(img_array.shape[0] - 1, row_max + margin)
    col_min = max(0, col_min - margin)
    col_max = min(img_array.shape[1] - 1, col_max + margin)

    # 裁剪图像
    return img.crop((col_min, row_min, col_max + 1, row_max + 1))


def merge_two_digits(img1, img2, output_size=(128, 128)):
    """
    将两张数字图像水平拼接并调整到指定大小,自动适配黑底白字或白底黑字

    改进点:
    - 自动检测图像类型(黑底白字 or 白底黑字)
    - 自动裁剪边缘,保留8像素安全边距避免裁剪数字
    - 间距限制在图像宽度的1/2以内
    - 使用LANCZOS高质量重采样避免白线
    - 根据输入图像类型自动设置背景颜色
    - 确保数字完整显示不被裁剪

    Args:
        img1: 第一张图像 (十位数字)
        img2: 第二张图像 (个位数字)
        output_size: 输出图像大小,默认为 (128, 128)

    Returns:
        融合后的图像
    """
    # 确保图像为RGB模式
    if img1.mode != 'RGB':
        img1 = img1.convert('RGB')
    if img2.mode != 'RGB':
        img2 = img2.convert('RGB')

    # 检测图像类型
    img_type = detect_image_type(img1)

    # 根据图像类型裁剪边缘
    if img_type == 'black_bg':
        img1 = crop_blackspace(img1)
        img2 = crop_blackspace(img2)
        bg_color = (0, 0, 0)  # 黑色背景
    else:  # white_bg
        img1 = crop_whitespace(img1)
        img2 = crop_whitespace(img2)
        bg_color = (255, 255, 255)  # 白色背景

    # 获取裁剪后的尺寸
    width1, height1 = img1.size
    width2, height2 = img2.size

    # 计算合理的间距: 较小数字宽度的3-6%之间随机选择
    min_width = min(width1, width2)
    spacing = random.randint(int(min_width * 0.03), int(min_width * 0.06))

    # 限制最大间距不超过图像宽度的1/2
    max_spacing = output_size[0] // 2
    spacing = min(spacing, max_spacing)

    # 计算拼接后的总尺寸
    total_width = width1 + spacing + width2
    total_height = max(height1, height2)

    # 计算缩放比例,确保拼接后的图像能完整放入画布
    # 留出足够边距(上下左右各10像素)
    margin = 10
    scale_w = (output_size[0] - 2 * margin) / total_width
    scale_h = (output_size[1] - 2 * margin) / total_height
    scale = min(scale_w, scale_h)  # 使用较小的缩放比例,确保完整显示

    # 缩放两个数字
    new_width1 = int(width1 * scale)
    new_height1 = int(height1 * scale)
    new_width2 = int(width2 * scale)
    new_height2 = int(height2 * scale)
    new_spacing = int(spacing * scale)

    img1 = img1.resize((new_width1, new_height1), Image.Resampling.LANCZOS)
    img2 = img2.resize((new_width2, new_height2), Image.Resampling.LANCZOS)

    # 创建合并画布 - 根据输入图像类型设置背景颜色
    merged_image = Image.new('RGB', output_size, bg_color)

    # 计算水平和垂直居中位置
    total_content_width = new_width1 + new_spacing + new_width2
    total_content_height = max(new_height1, new_height2)

    x_offset = (output_size[0] - total_content_width) // 2
    y_offset1 = (output_size[1] - new_height1) // 2
    y_offset2 = (output_size[1] - new_height2) // 2

    # 粘贴两张图像,中间有适当间距
    merged_image.paste(img1, (x_offset, y_offset1))
    merged_image.paste(img2, (x_offset + new_width1 + new_spacing, y_offset2))

    return merged_image


def get_images_by_digit(input_dir, max_per_digit=None):
    """
    按数字标签(0-9)分类获取所有图像路径

    Args:
        input_dir: 输入目录路径
        max_per_digit: 每个数字最多获取的图像数量,None表示全部获取

    Returns:
        字典,键为数字标签(0-9),值为该数字的所有图像路径列表
    """
    digit_images = {i: [] for i in range(10)}
    input_path = Path(input_dir)

    # 检查是否按数字分类组织(子文件夹 0-9)
    has_digit_folders = all((input_path / str(i)).exists() for i in range(10))

    if has_digit_folders:
        # 按子文件夹读取
        for digit in range(10):
            digit_dir = input_path / str(digit)
            images = list(digit_dir.glob("*.jpg"))
            if max_per_digit:
                images = images[:max_per_digit]
            digit_images[digit] = images
    else:
        # 从文件名提取标签
        for img_path in input_path.glob("*.jpg"):
            try:
                filename = img_path.name
                parts = filename.replace('.jpg', '').split('_')
                label = int(parts[-1])
                digit_images[label].append(img_path)
            except (IndexError, ValueError):
                continue

        # 限制数量
        if max_per_digit:
            for digit in range(10):
                digit_images[digit] = digit_images[digit][:max_per_digit]

    return digit_images


def generate_merged_images(input_dir, output_dir, num_images=1000, output_size=(128, 128)):
    """
    生成指定数量的融合图像

    Args:
        input_dir: 输入图像目录
        output_dir: 输出目录
        num_images: 要生成的融合图像数量
        output_size: 输出图像尺寸
    """
    # 获取所有数字图像
    print("正在加载图像...")
    digit_images = get_images_by_digit(input_dir)

    # 检查每个数字是否有足够的图像
    for digit in range(10):
        count = len(digit_images[digit])
        print(f"数字 {digit}: {count} 张图像")
        if count == 0:
            print(f"警告: 数字 {digit} 没有可用图像!")
            return

    # 创建输出目录
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    print(f"\n开始生成 {num_images} 张融合图像...")
    print(f"输出尺寸: {output_size[0]}x{output_size[1]}")
    print(f"输出目录: {output_path.absolute()}\n")

    # 统计信息
    stats = {
        'total': 0,
        'by_number': {}  # 存储每个两位数(00-99)的生成次数
    }

    # 生成融合图像
    for i in tqdm(range(num_images), desc="生成进度"):
        # 随机选择两个数字
        # 限制: 0不能为个位 (避免10, 20, 30...等)
        # 这同时也避免了00的情况
        while True:
            digit1 = random.randint(0, 9)  # 十位
            digit2 = random.randint(1, 9)  # 个位，直接从1开始，避免0
            break  # 由于digit2从1开始，无需额外检查

        # 随机选择对应数字的图像
        img1_path = random.choice(digit_images[digit1])
        img2_path = random.choice(digit_images[digit2])

        # 加载图像
        img1 = Image.open(img1_path)
        img2 = Image.open(img2_path)

        # 融合图像
        merged_img = merge_two_digits(img1, img2, output_size)

        # 生成输出文件名
        two_digit_number = digit1 * 10 + digit2
        output_filename = f"merged_{i:06d}_{two_digit_number:02d}.jpg"
        output_filepath = output_path / output_filename

        # 保存图像
        merged_img.save(output_filepath, quality=95)

        # 更新统计
        stats['total'] += 1
        if two_digit_number not in stats['by_number']:
            stats['by_number'][two_digit_number] = 0
        stats['by_number'][two_digit_number] += 1

    # 打印统计信息
    print("\n" + "="*50)
    print("生成完成!")
    print(f"总共生成: {stats['total']} 张融合图像")
    print(f"输出目录: {output_path.absolute()}")

    # 打印数字分布
    print("\n两位数分布 (前20个最常见的):")
    sorted_numbers = sorted(stats['by_number'].items(), key=lambda x: x[1], reverse=True)
    for number, count in sorted_numbers[:20]:
        print(f"  {number:02d}: {count} 张")

    print("="*50)


def generate_sequential_merged_images(input_dir, output_dir, images_per_number=10, output_size=(128, 128)):
    """
    按顺序生成所有两位数的融合图像(00-99)

    Args:
        input_dir: 输入图像目录
        output_dir: 输出目录
        images_per_number: 每个两位数生成的图像数量
        output_size: 输出图像尺寸
    """
    # 获取所有数字图像
    print("正在加载图像...")
    digit_images = get_images_by_digit(input_dir)

    # 检查每个数字是否有足够的图像
    for digit in range(10):
        count = len(digit_images[digit])
        print(f"数字 {digit}: {count} 张图像")
        if count == 0:
            print(f"警告: 数字 {digit} 没有可用图像!")
            return
        if count < images_per_number:
            print(f"警告: 数字 {digit} 只有 {count} 张图像,少于请求的 {images_per_number} 张")

    # 创建输出目录
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    # 为每个有效的两位数创建子文件夹（01-09, 11-19, ..., 91-99）
    for digit1 in range(10):
        for digit2 in range(1, 10):
            number = digit1 * 10 + digit2
            number_dir = output_path / f"{number:02d}"
            number_dir.mkdir(exist_ok=True)

    total_images = 90 * images_per_number  # 90个有效两位数（排除个位为0的情况）
    print(f"\n开始生成 {total_images} 张融合图像 (每个两位数 {images_per_number} 张)...")
    print(f"生成范围: 01-09, 11-19, 21-29, ..., 91-99 (共90个两位数)")
    print(f"输出尺寸: {output_size[0]}x{output_size[1]}")
    print(f"输出目录: {output_path.absolute()}\n")

    # 统计信息
    stats = {
        'total': 0,
        'by_number': {i: 0 for i in range(100)}
    }

    # 生成融合图像
    with tqdm(total=total_images, desc="生成进度") as pbar:
        for digit1 in range(1, 10):  # 十位: 1-9
            for digit2 in range(10):  # 个位: 0-9

                two_digit_number = digit1 * 10 + digit2
                number_dir = output_path / f"{two_digit_number:02d}"

                # 为这个两位数生成多张图像
                for idx in range(images_per_number):
                    # 随机选择对应数字的图像
                    img1_path = random.choice(digit_images[digit1])
                    img2_path = random.choice(digit_images[digit2])

                    # 加载图像
                    img1 = Image.open(img1_path)
                    img2 = Image.open(img2_path)

                    # 融合图像
                    merged_img = merge_two_digits(img1, img2, output_size)

                    # 生成输出文件名
                    output_filename = f"merged_{two_digit_number:02d}_{idx:04d}.jpg"
                    output_filepath = number_dir / output_filename

                    # 保存图像
                    merged_img.save(output_filepath, quality=95)

                    # 更新统计
                    stats['total'] += 1
                    stats['by_number'][two_digit_number] += 1

                    pbar.update(1)

    # 打印统计信息
    print("\n" + "="*50)
    print("生成完成!")
    print(f"总共生成: {stats['total']} 张融合图像")
    print(f"涵盖两位数: 10-19, 20-29, ..., 90-99 (共90个数字)")
    print(f"每个数字: {images_per_number} 张图像")
    print(f"输出目录: {output_path.absolute()}")
    print("="*50)


def generate_from_single_source(input_dir, output_dir, output_size=(128, 128)):
    """
    从单个图像源生成所有两位数的融合图像(10-99)
    每个两位数的数量 = min(十位数字数量, 个位数字数量)
    最终所有两位数的数量统一为全局最小值

    Args:
        input_dir: 输入图像目录
        output_dir: 输出目录
        output_size: 输出图像尺寸
    """
    # 获取所有数字图像
    print(f"\n{'='*50}")
    print(f"处理源: {input_dir.name}")
    print(f"{'='*50}")
    print("\n正在加载图像...")
    digit_images = get_images_by_digit(input_dir)

    # 确定源类型标识
    source_tag = ""
    if "blur" in input_dir.name.lower():
        source_tag = "_blur"  # 有残影
    else:
        source_tag = "_plain"  # 无残影

    print(f"\n目录: {input_dir.name}")
    print(f"文件名标识: {source_tag}")
    for digit in range(10):
        count = len(digit_images[digit])
        print(f"  数字 {digit}: {count} 张图像")
        if count == 0:
            print(f"  警告: 数字 {digit} 没有可用图像!")
            return

    # 计算每个两位数(10-99)的最大可生成数量
    print("\n计算每个两位数的最大可生成数量...")
    max_counts = {}

    for digit1 in range(1, 10):  # 十位: 1-9
        for digit2 in range(10):  # 个位: 0-9
            two_digit = digit1 * 10 + digit2
            # 该两位数的最大数量 = min(十位数字数量, 个位数字数量)
            max_count = min(len(digit_images[digit1]), len(digit_images[digit2]))
            max_counts[two_digit] = max_count

    # 显示最小的几个
    min_pairs = sorted(max_counts.items(), key=lambda x: x[1])[:10]
    print(f"最小的10个两位数:")
    for number, count in min_pairs:
        print(f"  {number:02d}: {count} 张")

    # 计算全局最小值
    global_min = min(max_counts.values())
    print(f"\n全局最小值: {global_min}")
    print(f"所有两位数(10-99)将生成 {global_min} 张图像")

    # 创建输出目录
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True, parents=True)

    # 为每个有效的两位数创建子文件夹（10-99，共90个）
    for digit1 in range(1, 10):  # 十位: 1-9
        for digit2 in range(10):  # 个位: 0-9
            number = digit1 * 10 + digit2
            number_dir = output_path / f"{number:02d}"
            number_dir.mkdir(exist_ok=True)

    total_images = 90 * global_min  # 90个两位数 × 全局最小值
    print(f"\n开始生成 {total_images} 张融合图像...")
    print(f"生成范围: 10-19, 20-29, ..., 90-99 (共90个两位数)")
    print(f"每个两位数: {global_min} 张")
    print(f"输出尺寸: {output_size[0]}x{output_size[1]}")
    print(f"输出目录: {output_path.absolute()}\n")

    # 统计信息
    stats = {
        'total': 0,
        'by_number': {i: 0 for i in range(100)}
    }

    # 生成融合图像
    with tqdm(total=total_images, desc=f"生成进度 ({input_dir.name})") as pbar:
        for digit1 in range(1, 10):  # 十位: 1-9
            for digit2 in range(10):  # 个位: 0-9

                two_digit_number = digit1 * 10 + digit2
                number_dir = output_path / f"{two_digit_number:02d}"

                # 为这个两位数生成 global_min 张图像
                for idx in range(global_min):
                    # 从同一个源中选择两个数字的图像
                    img1_path = random.choice(digit_images[digit1])
                    img2_path = random.choice(digit_images[digit2])

                    # 加载图像
                    img1 = Image.open(img1_path)
                    img2 = Image.open(img2_path)

                    # 融合图像
                    merged_img = merge_two_digits(img1, img2, output_size)

                    # 生成输出文件名 - 添加源类型标识
                    output_filename = f"merged_{two_digit_number:02d}_{idx:06d}{source_tag}.jpg"
                    output_filepath = number_dir / output_filename

                    # 保存图像
                    merged_img.save(output_filepath, quality=95)

                    # 更新统计
                    stats['total'] += 1
                    stats['by_number'][two_digit_number] += 1

                    pbar.update(1)

    # 打印统计信息
    print("\n" + "="*50)
    print("生成完成!")
    print(f"总共生成: {stats['total']} 张融合图像")
    print(f"涵盖两位数: 10-19, 20-29, ..., 90-99 (共90个数字)")
    print(f"每个数字: {global_min} 张图像")
    print(f"输出目录: {output_path.absolute()}")
    print("="*50)


def generate_sequential_merged_images_multi_source(input_dirs, output_dir, images_per_number=None, output_size=(128, 128)):
    """
    从多个图像源按顺序生成所有两位数的融合图像(10-99)
    每个两位数的两个数字来自同一个源文件夹
    所有两位数生成相同数量的图像(取决于最小的可生成数量)

    Args:
        input_dirs: 输入图像目录列表
        output_dir: 输出目录
        images_per_number: 忽略此参数,自动计算
        output_size: 输出图像尺寸
    """
    # 从每个目录获取所有数字图像
    print("正在加载图像...")
    all_digit_images = []

    for input_dir in input_dirs:
        digit_images = get_images_by_digit(input_dir)
        all_digit_images.append(digit_images)
        print(f"\n目录: {input_dir.name}")
        for digit in range(10):
            count = len(digit_images[digit])
            print(f"  数字 {digit}: {count} 张图像")
            if count == 0:
                print(f"  警告: 数字 {digit} 没有可用图像!")
                return

    # 计算每个两位数(10-99)在每个源中的最大可生成数量
    print("\n计算每个两位数的最大可生成数量...")
    max_counts_per_source = []

    for source_idx, digit_images in enumerate(all_digit_images):
        source_name = input_dirs[source_idx].name
        print(f"\n源: {source_name}")
        source_counts = {}

        for digit1 in range(1, 10):  # 十位: 1-9
            for digit2 in range(10):  # 个位: 0-9
                two_digit = digit1 * 10 + digit2
                # 该两位数的最大数量 = min(十位数字数量, 个位数字数量)
                max_count = min(len(digit_images[digit1]), len(digit_images[digit2]))
                source_counts[two_digit] = max_count

        max_counts_per_source.append(source_counts)
        # 显示该源中最小的几个
        min_pairs = sorted(source_counts.items(), key=lambda x: x[1])[:5]
        print(f"  最小的5个: {min_pairs}")

    # 计算全局最小值(所有源、所有两位数中最小的)
    global_min = float('inf')
    for source_counts in max_counts_per_source:
        for count in source_counts.values():
            global_min = min(global_min, count)

    print(f"\n全局最小值: {global_min}")
    print(f"所有两位数(10-99)将生成 {global_min} 张图像")

    # 创建输出目录
    output_path = Path(output_dir)
    output_path.mkdir(exist_ok=True)

    # 为每个有效的两位数创建子文件夹（10-99，共90个）
    for digit1 in range(1, 10):  # 十位: 1-9
        for digit2 in range(10):  # 个位: 0-9
            number = digit1 * 10 + digit2
            number_dir = output_path / f"{number:02d}"
            number_dir.mkdir(exist_ok=True)

    total_images = 90 * global_min  # 90个两位数 × 全局最小值
    print(f"\n开始生成 {total_images} 张融合图像...")
    print(f"图像源数量: {len(input_dirs)}")
    print(f"生成范围: 10-19, 20-29, ..., 90-99 (共90个两位数)")
    print(f"每个两位数: {global_min} 张")
    print(f"输出尺寸: {output_size[0]}x{output_size[1]}")
    print(f"输出目录: {output_path.absolute()}\n")

    # 统计信息
    stats = {
        'total': 0,
        'by_number': {i: 0 for i in range(100)},
        'by_source': {str(input_dir.name): 0 for input_dir in input_dirs}
    }

    # 生成融合图像
    with tqdm(total=total_images, desc="生成进度") as pbar:
        for digit1 in range(1, 10):  # 十位: 1-9
            for digit2 in range(10):  # 个位: 0-9

                two_digit_number = digit1 * 10 + digit2
                number_dir = output_path / f"{two_digit_number:02d}"

                # 为这个两位数生成 global_min 张图像
                for idx in range(global_min):
                    # 从多个图像源中随机选择一个源
                    source_idx = random.randint(0, len(all_digit_images) - 1)
                    digit_images = all_digit_images[source_idx]
                    source_name = input_dirs[source_idx].name

                    # 从同一个源中选择两个数字的图像
                    img1_path = random.choice(digit_images[digit1])
                    img2_path = random.choice(digit_images[digit2])

                    # 加载图像
                    img1 = Image.open(img1_path)
                    img2 = Image.open(img2_path)

                    # 融合图像
                    merged_img = merge_two_digits(img1, img2, output_size)

                    # 生成输出文件名
                    output_filename = f"merged_{two_digit_number:02d}_{idx:06d}.jpg"
                    output_filepath = number_dir / output_filename

                    # 保存图像
                    merged_img.save(output_filepath, quality=95)

                    # 更新统计
                    stats['total'] += 1
                    stats['by_number'][two_digit_number] += 1
                    stats['by_source'][source_name] += 1

                    pbar.update(1)

    # 打印统计信息
    print("\n" + "="*50)
    print("生成完成!")
    print(f"总共生成: {stats['total']} 张融合图像")
    print(f"涵盖两位数: 10-19, 20-29, ..., 90-99 (共90个数字)")
    print(f"每个数字: {global_min} 张图像")
    print(f"输出目录: {output_path.absolute()}")

    print("\n图像源使用统计:")
    for source_name, count in stats['by_source'].items():
        percentage = (count / stats['total']) * 100
        print(f"  {source_name}: {count} 张 ({percentage:.1f}%)")

    print("="*50)


def main():
    """主函数"""

    # ===========================================
    # 配置区域 - 根据需要修改以下参数
    # ===========================================

    # 输出图像尺寸
    output_size = (128, 128)

    # ===========================================
    # 生成配置 - 两个独立的源
    # ===========================================

    # 源1: 只有像素反转
    source1_input = Path("./resized_128x128_inverted")
    source1_output = Path("./merged_inverted")

    # 源2: 残影+像素反转
    source2_input = Path("./resized_128x128_with_blur_inverted")
    source2_output = Path("./merged_with_blur_inverted")

    # ===========================================
    # 执行生成
    # ===========================================

    # 检查输入目录是否存在
    if not source1_input.exists():
        print(f"错误: 输入目录不存在: {source1_input.absolute()}")
        return

    if not source2_input.exists():
        print(f"错误: 输入目录不存在: {source2_input.absolute()}")
        return

    # 生成源1的融合图像
    print("\n" + "="*60)
    print("任务 1/2: 生成只有像素反转的融合图像")
    print("="*60)
    generate_from_single_source(
        input_dir=source1_input,
        output_dir=source1_output,
        output_size=output_size
    )

    # 生成源2的融合图像
    print("\n" + "="*60)
    print("任务 2/2: 生成残影+像素反转的融合图像")
    print("="*60)
    generate_from_single_source(
        input_dir=source2_input,
        output_dir=source2_output,
        output_size=output_size
    )

    print("\n" + "="*60)
    print("全部完成！")
    print("="*60)
    print(f"输出目录1: {source1_output.absolute()}")
    print(f"输出目录2: {source2_output.absolute()}")
    print("="*60)


if __name__ == "__main__":
    main()
