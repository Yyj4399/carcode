"""
测试 invert_colors.py 的旋转方向后缀功能
"""

from pathlib import Path
from invert_colors import process_images_with_inversion

# 测试1: 处理带 down 方向的图像
print("="*60)
print("测试 1: 处理 down 方向的图像")
print("="*60)

input_dir = Path("./resized_128x128_with_blur_rotated/down")
output_dir = Path("./test_output_inverted/down")

if input_dir.exists():
    process_images_with_inversion(input_dir, output_dir)

    # 验证输出文件夹
    print("\n验证输出:")
    for digit in range(3):  # 只检查前3个数字
        folder_name = f"{digit}_down"
        folder_path = output_dir / folder_name
        if folder_path.exists():
            count = len(list(folder_path.glob("*.jpg")))
            print(f"  ✓ 文件夹 {folder_name} 存在，包含 {count} 张图像")
        else:
            print(f"  ✗ 文件夹 {folder_name} 不存在")
else:
    print(f"输入目录不存在: {input_dir}")

print("\n" + "="*60)
print("测试完成！")
print("="*60)
