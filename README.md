# MNIST 图像处理工具集

这是一个用于处理 MNIST 手写数字数据集的 Python 工具集,提供图像缩放、特征标记、残影效果、颜色反转和旋转等数据增强功能。

## 项目简介

MNIST 数据集包含 70,000 张 28x28 像素的灰度手写数字图像(60,000 训练集 + 10,000 测试集)。本项目提供了一系列脚本用于对这些图像进行各种处理和增强。

### 主要功能

- **图像缩放与标记**: 将 28x28 图像缩放到 128x128,并为数字 6 和 9 添加特征横杠
- **残影效果**: 添加双向运动模糊或残影效果
- **颜色反转**: 黑白像素反转
- **图像旋转**: 生成上、下、左、右四个朝向的旋转版本

## 环境要求

- Python 3.7+
- 依赖包:
  - Pillow >= 10.0.0
  - numpy >= 1.24.0
  - tqdm >= 4.65.0

## 安装

```bash
# 克隆仓库
git clone https://github.com/your-username/mnist-image-processing.git
cd mnist-image-processing

# 安装依赖
pip install -r requirements.txt
```

## 使用方法

### 数据集准备

本仓库不包含原始 MNIST 图像文件(约 70,000 张 .jpg 文件)。请将 MNIST JPEG 格式数据集放置在项目根目录。

文件命名格式:
- 训练集: `training_{索引}_{标签}.jpg`
- 测试集: `test_{索引}_{标签}.jpg`

### 处理流程

完整的数据增强管道按以下顺序执行:

#### 1. 基础缩放和标记

```bash
python resize_and_mark.py
```

- 将图像从 28x28 缩放到 128x128
- 为数字 6 添加白色横杠(距底部 16 像素)
- 为数字 9 添加白色横杠(距底部 8 像素)
- 输出: `./resized_128x128/{0-9}/`

#### 2. 添加残影效果(可选)

```bash
python add_motion_blur.py
```

- 支持两种效果: `ghosting` (残影) 或 `motion` (运动模糊)
- 双向模糊效果(向左右两个方向)
- 输出: `./resized_128x128_with_blur/{0-9}/`

#### 3. 颜色反转(可选)

```bash
python invert_colors.py
```

- 将黑色变白色,白色变黑色
- 输出: `./resized_128x128_with_blur_inverted/{0-9}/`

#### 4. 图像旋转(可选)

```bash
python rotate_images.py
```

- 生成 4 个旋转方向: 上(0°)、下(180°)、左(270°)、右(90°)
- 输出: `./resized_128x128_with_blur_inverted_rotated/{up,down,left,right}/{0-9}/`

### 配置修改

所有脚本的输入/输出路径都在各自的 `main()` 函数中配置。修改方法:

1. 打开对应的 .py 文件
2. 找到 `main()` 函数(通常在文件末尾)
3. 修改 `input_dir` 和 `output_dir` 变量
4. 某些脚本还有额外参数可调整(如 `blur_type`, `intensity` 等)

## 项目结构

```
.
├── README.md                    # 项目说明
├── CLAUDE.md                    # Claude Code 开发指南
├── requirements.txt             # Python 依赖
├── .gitignore                   # Git 忽略文件
├── resize_and_mark.py          # 图像缩放和标记
├── add_motion_blur.py          # 添加残影效果
├── invert_colors.py            # 图像颜色反转
└── rotate_images.py            # 图像旋转
```

## 输出目录结构

```
.
├── resized_128x128/                              # 缩放后的图像(带横杠)
│   ├── 0/
│   └── ...
├── resized_128x128_with_blur/                    # 添加残影效果
│   ├── 0/
│   └── ...
├── resized_128x128_with_blur_inverted/           # 反转颜色
│   ├── 0/
│   └── ...
└── resized_128x128_with_blur_inverted_rotated/   # 旋转为4个朝向
    ├── up/
    │   ├── 0/
    │   └── ...
    ├── down/
    ├── left/
    └── right/
```

## 代码架构

所有脚本遵循统一的设计模式:

- **核心处理函数**: 处理单张图像的核心逻辑
- **批量处理函数**: `main()` 函数处理整个目录
- **自动检测**: 自动识别输入目录结构(根目录或数字分类文件夹)
- **统计输出**: 处理完成后显示详细统计信息
- **进度显示**: 使用 tqdm 显示实时进度

## 注意事项

- 原始 MNIST 图像文件和所有生成的处理后图像都已在 `.gitignore` 中忽略
- 修改配置时,请确保输入目录与前一步的输出目录匹配
- 每个脚本都能自动检测输入目录结构
- 文件名会累积添加后缀(如 `_inverted`, `_up` 等)

## 开发

查看 [CLAUDE.md](CLAUDE.md) 了解详细的代码架构和开发指南。

## 许可证

MIT License

## 相关链接

- [MNIST 数据集官方网站](http://yann.lecun.com/exdb/mnist/)
