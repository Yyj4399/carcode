from pathlib import Path

# 检查原始图片文件夹中是否有类别信息
original_path = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\original_images")

if original_path.exists():
    subdirs = [d.name for d in original_path.iterdir() if d.is_dir()]
    print(f"original_images subdirs: {subdirs[:20]}")

# 检查一个图片的文件名看看是否包含方向信息
merged_path = Path(r"D:\BaiduNetdiskDownload\mnist_jpg\mnist_jpg\mnist_tools\merged_inverted\10")
if merged_path.exists():
    files = list(merged_path.glob("*.jpg"))
    print(f"\nSample files from folder 10:")
    for f in files[:20]:
        print(f"  {f.name}")
