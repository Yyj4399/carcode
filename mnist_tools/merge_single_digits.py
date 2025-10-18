"""
将单个数字(1-9)的图像处理成与两位数相同的格式和数量
从两个源文件夹读取: resized_128x128_inverted 和 resized_128x128_with_blur_inverted
每个单个数字的数量与对应两位数相同，并放入相同的文件夹结构(01-09)
"""

import os
import random
import shutil
from pathlib import Path
from PIL import Image
from tqdm import tqdm


def process_single_digit(img, output_size=(128, 128)):
    """
    处理单个数字图像，保持原样但确保尺寸正确

    Args:
        img: 输入图像
        output_size: 输出图像大小，默认为 (128, 128)

    Returns:
        处理后的图像
    """
    # 确保图像为RGB模式
    if img.mode != 'RGB':
        img = img.convert('RGB')

    # 如果尺寸不对，调整大小
    if img.size != output_size:
        img = img.resize(output_size, Image.Resampling.LANCZOS)

    return img


def get_target_count_for_digit(digit, merged_dirs):
    """
    获取指定数字应该生成的图像数量
    使用对应的两位数文件夹作为参考（1对应10-19, 2对应20-29, 等等）

    Args:
        digit: 数字 (1-9)
        merged_dirs: 两位数图像目录列表

    Returns:
        该数字应该生成的图像数量
    """
    # 单个数字对应的参考两位数文件夹（1对应10，2对应20，等等）
    # 我们取该十位数范围内的一个代表性文件夹
    reference_folder = f"{digit}1"  # 例如：1->11, 2->21, ..., 9->91

    # 统计所有源文件夹中该参考两位数的图像数量
    total_count = 0
    for merged_dir in merged_dirs:
        target_path = merged_dir / reference_folder
        if target_path.exists():
            count = len(list(target_path.glob("*.jpg")))
            total_count += count

    return total_count


def get_images_by_digit_single(input_dir, digits_to_load):
    """
    按数字标签获取指定数字的所有图像路径

    Args:
        input_dir: 输入目录路径
        digits_to_load: 要加载的数字列表 (例如 [1, 2, 3, ..., 9])

    Returns:
        字典，键为数字标签，值为该数字的所有图像路径列表
    """
    digit_images = {i: [] for i in digits_to_load}
    input_path = Path(input_dir)

    # 检查是否按数字分类组织(子文件夹 1-9)
    has_digit_folders = all((input_path / str(i)).exists() for i in digits_to_load)

    if has_digit_folders:
        # 按子文件夹读取
        for digit in digits_to_load:
            digit_dir = input_path / str(digit)
            images = list(digit_dir.glob("*.jpg"))
            digit_images[digit] = images
    else:
        # 从文件名提取标签
        for img_path in input_path.glob("*.jpg"):
            try:
                filename = img_path.name
                parts = filename.replace('.jpg', '').split('_')
                label = int(parts[-1])
                if label in digits_to_load:
                    digit_images[label].append(img_path)
            except (IndexError, ValueError):
                continue

    return digit_images


def generate_single_digit_images(source_dirs, output_dirs, merged_dirs, output_size=(128, 128)):
    """
    为每个单个数字(1-9)生成与对应两位数相同数量的图像

    Args:
        source_dirs: 源图像目录列表 (resized_128x128_inverted, resized_128x128_with_blur_inverted)
        output_dirs: 输出目录列表 (对应每个源)
        merged_dirs: 两位数图像目录列表 (用于确定目标数量)
        output_size: 输出图像尺寸
    """
    digits_to_process = list(range(1, 10))  # 1-9

    print(f"\n{'='*60}")
    print("单个数字图像生成任务")
    print(f"{'='*60}")

    # 处理每个源
    for source_idx, source_dir in enumerate(source_dirs):
        output_dir = output_dirs[source_idx]
        source_path = Path(source_dir)
        output_path = Path(output_dir)

        print(f"\n处理源 {source_idx + 1}/{len(source_dirs)}: {source_path.name}")
        print(f"输出到: {output_path.name}")

        # 加载该源中的所有单个数字图像
        print("\n正在加载图像...")
        digit_images = get_images_by_digit_single(source_path, digits_to_process)

        for digit in digits_to_process:
            count = len(digit_images[digit])
            print(f"  数字 {digit}: {count} 张图像")
            if count == 0:
                print(f"  警告: 数字 {digit} 没有可用图像!")
                return

        # 为每个数字创建输出文件夹并生成图像
        for digit in digits_to_process:
            # 确定目标数量（与对应两位数相同）
            target_count = get_target_count_for_digit(digit, merged_dirs)

            if target_count == 0:
                print(f"\n  警告: 数字 {digit} 对应的两位数文件夹(0{digit})中没有图像，跳过")
                continue

            # 创建输出文件夹 (01, 02, ..., 09)
            output_folder = output_path / f"{digit:02d}"
            output_folder.mkdir(parents=True, exist_ok=True)

            # 确定源文件名标识
            source_tag = ""
            if "blur" in source_path.name.lower():
                source_tag = "_blur"
            else:
                source_tag = "_plain"

            print(f"\n  数字 {digit} -> 文件夹 {digit:02d}")
            print(f"    目标数量: {target_count} 张")
            print(f"    可用源图像: {len(digit_images[digit])} 张")
            print(f"    文件名标识: {source_tag}")

            # 生成图像
            available_images = digit_images[digit]

            for idx in tqdm(range(target_count), desc=f"    生成进度", leave=False):
                # 随机选择一张源图像
                img_path = random.choice(available_images)

                # 加载并处理图像
                img = Image.open(img_path)
                processed_img = process_single_digit(img, output_size)

                # 生成输出文件名: single_{数字}_{索引}_{源标识}.jpg
                output_filename = f"single_{digit:02d}_{idx:06d}{source_tag}.jpg"
                output_filepath = output_folder / output_filename

                # 保存图像
                processed_img.save(output_filepath, quality=95)

    print(f"\n{'='*60}")
    print("全部完成！")
    print(f"{'='*60}")


def main():
    """主函数"""

    # ===========================================
    # 配置区域 - 根据需要修改以下参数
    # ===========================================

    # 输出图像尺寸
    output_size = (128, 128)

    # 源文件夹列表（包含单个数字1-9的图像）
    source_dirs = [
        Path("./resized_128x128_inverted"),              # 源1: 只有像素反转
        Path("./resized_128x128_with_blur_inverted")     # 源2: 残影+像素反转
    ]

    # 输出文件夹列表（对应每个源）
    output_dirs = [
        Path("./merged_inverted"),                       # 输出到两位数所在的文件夹
        Path("./merged_with_blur_inverted")
    ]

    # 两位数图像目录列表（用于确定单个数字应生成的数量）
    merged_dirs = [
        Path("./merged_inverted"),
        Path("./merged_with_blur_inverted")
    ]

    # ===========================================
    # 执行生成
    # ===========================================

    # 检查源文件夹是否存在
    for source_dir in source_dirs:
        if not source_dir.exists():
            print(f"错误: 源文件夹不存在: {source_dir.absolute()}")
            return

    # 检查两位数文件夹是否存在
    for merged_dir in merged_dirs:
        if not merged_dir.exists():
            print(f"错误: 两位数文件夹不存在: {merged_dir.absolute()}")
            return

    # 生成单个数字图像
    generate_single_digit_images(
        source_dirs=source_dirs,
        output_dirs=output_dirs,
        merged_dirs=merged_dirs,
        output_size=output_size
    )

    # 显示最终统计
    print("\n输出目录:")
    for output_dir in output_dirs:
        print(f"  {output_dir.absolute()}")
        # 统计每个文件夹的图像数量
        for digit in range(1, 10):
            folder_path = output_dir / f"{digit:02d}"
            if folder_path.exists():
                count = len(list(folder_path.glob("*.jpg")))
                print(f"    文件夹 {digit:02d}: {count} 张图像")


if __name__ == "__main__":
    main()
