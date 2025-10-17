# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是 MNIST 手写数字数据集的 JPEG 图像格式版本。包含 70,000 张 28x28 像素的灰度手写数字图像。

## 数据集结构

### 文件命名规则
- **训练集**: `training_{索引}_{标签}.jpg` (例如: `training_0_5.jpg` 表示第0张训练图像,标签为数字5)
- **测试集**: `test_{索引}_{标签}.jpg` (例如: `test_1234_9.jpg` 表示第1234张测试图像,标签为数字9)

### 图像规格
- 格式: JPEG (JFIF 1.01)
- 尺寸: 28x28 像素
- 颜色: 灰度图像(单通道)
- 精度: 8位
- 总文件数: 70,000 张图像

### 数据分布
- 训练集: 60,000 张图像 (training_0_*.jpg 到 training_59999_*.jpg)
- 测试集: 10,000 张图像 (test_0_*.jpg 到 test_9999_*.jpg)
- 标签范围: 0-9 (十个数字类别)

## 典型使用场景

这个数据集通常用于:
1. 手写数字识别模型训练
2. 深度学习/机器学习教学示例
3. 图像分类算法测试
4. 卷积神经网络(CNN)实验

## 数据加载示例

使用 Python 加载图像时的常见模式:

```python
# 从文件名提取标签
filename = "training_1234_7.jpg"
parts = filename.replace('.jpg', '').split('_')
index = int(parts[1])
label = int(parts[2])

# 加载图像
from PIL import Image
import numpy as np

img = Image.open(filename)
img_array = np.array(img)  # 形状: (28, 28)
```

## 注意事项

- 所有图像已预处理为标准化的 28x28 尺寸
- 文件名中包含真实标签,方便监督学习
- 图像索引从 0 开始计数
- 这是 MNIST 原始数据集的 JPEG 转换版本

## 环境设置

安装 Python 依赖:

```bash
pip install -r requirements.txt
```

依赖包包括:
- Pillow (图像处理)
- numpy (数值计算)
- tqdm (进度条显示)

## 项目目录结构

```
mnist_tools/
├── original_images/          # 70,000 张原始 28x28 JPG 图片
│   ├── training_0_5.jpg
│   ├── training_1_3.jpg
│   ├── test_0_7.jpg
│   └── ...
├── resized_128x128/          # 缩放后的 128x128 图像(带标记横杠)
│   ├── 0/
│   ├── 1/
│   └── ...
├── resized_128x128_with_blur/           # 添加残影效果的图像
├── resized_128x128_with_blur_inverted/  # 反转颜色的图像
├── resized_128x128_with_blur_inverted_rotated/  # 旋转的图像
│   ├── up/
│   ├── down/
│   ├── left/
│   └── right/
├── resize_and_mark.py        # 脚本1: 缩放和标记
├── add_motion_blur.py        # 脚本2: 添加残影
├── invert_colors.py          # 脚本3: 颜色反转
├── rotate_images.py          # 脚本4: 图像旋转
├── requirements.txt          # Python 依赖
├── CLAUDE.md                 # 本文档
└── README.md                 # 项目说明
```

## 代码架构

所有图像处理脚本遵循统一的设计模式:

### 共同结构
- **主处理函数**: 每个脚本包含一个核心图像处理函数(如 `process_image()`, `add_motion_blur()`, `invert_image()`, `rotate_image()`)
- **批量处理函数**: 使用 `main()` 函数处理整个目录的图像
- **目录结构检测**: 自动识别输入目录结构(根目录或数字分类文件夹)
- **统计输出**: 处理完成后显示详细统计信息
- **进度显示**: 使用 tqdm 库显示处理进度

### 配置修改
所有脚本的输入/输出路径配置都位于各自的 `main()` 函数中。修改配置时:
1. 打开对应的 .py 文件
2. 找到 `main()` 函数(通常在文件末尾)
3. 取消注释想要使用的配置选项,注释掉当前选项
4. 每个脚本都提供了多个预设的路径配置选项
5. 某些脚本还有额外参数(如 `blur_type`, `intensity` 等)

## 图像处理脚本

### 1. resize_and_mark.py - 图像缩放和标记

将 28x28 图像缩放到 128x128,并在特定数字下方添加白色横杠。

**功能**:
- 将所有图像从 28x28 缩放到 128x128 像素
- 对标签为 6 的图像: 在距离底部 16 像素处添加白色横杠
- 对标签为 9 的图像: 在距离底部 8 像素处添加白色横杠
- 按数字类别(0-9)组织输出到子文件夹

**核心函数**: `process_image(input_path, output_path, label)` (resize_and_mark.py:12)

**运行**:
```bash
python resize_and_mark.py
```

**配置位置**: `main()` 函数 (resize_and_mark.py:59)

**可用的输入输出路径配置**:
- ✓ `./original_images` → `./resized_128x128` (推荐,默认)
- `./` (当前目录) → `./resized_128x128`

**输出**: `./resized_128x128/{0-9}/`

### 2. add_motion_blur.py - 添加残影效果

在已处理的 128x128 图像上添加运动模糊/残影效果。

**功能**:
- 支持两种残影效果: `ghosting` (残影) 或 `motion` (运动模糊)
- 双向模糊效果 (向左右两个方向)
- 可调整强度参数
- 保持原有的数字类别组织结构

**核心函数**:
- `add_ghosting_effect(image, direction, intensity)` (add_motion_blur.py:77)
- `add_motion_blur(image, direction, intensity)` (add_motion_blur.py:13)

**运行**:
```bash
python add_motion_blur.py
```

**配置位置**: `main()` 函数 (add_motion_blur.py:152)

**可用的输入输出路径配置**:
- ✓ `./resized_128x128` → `./resized_128x128_with_blur` (推荐,默认)
- `./resized_128x128_inverted` → `./resized_128x128_inverted_with_blur`
- `./resized_128x128_rotated/up` → `./resized_128x128_rotated_with_blur/up` (需要指定朝向)
- `./original_images` → `./original_images_with_blur` (不推荐)

**输出**: `./resized_128x128_with_blur/{0-9}/`

**注意**: 此脚本通常处理已缩放的图像,建议先运行 resize_and_mark.py

### 3. invert_colors.py - 图像颜色反转

将图像的黑白像素反转(黑变白,白变黑)。

**功能**:
- 支持两种反转方法: PIL ImageOps 或 numpy 数组操作
- 自动检测输入目录结构(根目录或按数字分类)
- 保持原有的文件夹组织结构
- 生成的文件名添加 `_inverted` 后缀

**核心函数**:
- `invert_image(image)` - 使用 PIL (invert_colors.py:13)
- `invert_image_numpy(image)` - 使用 numpy (invert_colors.py:27)

**运行**:
```bash
python invert_colors.py
```

**配置位置**: `main()` 函数 (invert_colors.py:176)

**可用的输入输出路径配置**:
- `./original_images` → `./original_images_inverted`
- `./resized_128x128` → `./resized_128x128_inverted`
- ✓ `./resized_128x128_with_blur` → `./resized_128x128_with_blur_inverted` (推荐,默认)
- `./resized_128x128_rotated/up` → `./resized_128x128_rotated_inverted/up` (需要指定朝向)
- `./resized_128x128_with_blur_rotated/up` → `./resized_128x128_with_blur_rotated_inverted/up` (需要指定朝向)

**输出**: `./resized_128x128_with_blur_inverted/{0-9}/` (默认配置)

### 4. rotate_images.py - 图像旋转

将图像旋转为4个不同的朝向(上、下、左、右)。

**功能**:
- 生成4个旋转方向: 上(0°)、下(180°)、左(270°)、右(90°)
- 输出组织为 `{朝向}/{数字类别}/` 结构
- 自动检测输入目录结构
- 生成的文件名添加朝向后缀 (如 `_up`, `_down`, `_left`, `_right`)

**核心函数**: `rotate_image(image, angle)` (rotate_images.py:12)

**运行**:
```bash
python rotate_images.py
```

**配置位置**: `main()` 函数 (rotate_images.py:182)

**可用的输入输出路径配置**:
- `./original_images` → `./original_images_rotated`
- `./resized_128x128` → `./resized_128x128_rotated`
- `./resized_128x128_inverted` → `./resized_128x128_inverted_rotated`
- `./resized_128x128_with_blur` → `./resized_128x128_with_blur_rotated`
- ✓ `./resized_128x128_with_blur_inverted` → `./resized_128x128_with_blur_inverted_rotated` (推荐,默认)
- `./resized_128x128_inverted_with_blur` → `./resized_128x128_inverted_with_blur_rotated`

**输出**: `./resized_128x128_with_blur_inverted_rotated/{up,down,left,right}/{0-9}/`

**输出目录结构示例**:
```
resized_128x128_rotated/
├── up/
│   ├── 0_up/
│   ├── 1_up/
│   └── ...
├── down/
│   ├── 0_down/
│   ├── 1_down/
│   └── ...
├── left/
│   ├── 0_left/
│   └── ...
└── right/
    ├── 0_right/
    └── ...
```

### 处理流程顺序

完整的数据增强管道按以下顺序执行:

1. **基础缩放**: `resize_and_mark.py`
   - 从 `original_images/` 读取原始 28x28 图像
   - 生成 128x128 图像并添加特征标记(数字6和9的横杠)
   - 输出到 `resized_128x128/`

2. **残影效果** (可选): `add_motion_blur.py`
   - 从 `resized_128x128/` 读取
   - 添加运动模糊或残影效果
   - 输出到 `resized_128x128_with_blur/`

3. **颜色反转** (可选): `invert_colors.py`
   - 从 `resized_128x128_with_blur/` 读取
   - 反转黑白像素
   - 输出到 `resized_128x128_with_blur_inverted/`

4. **旋转变换** (可选): `rotate_images.py`
   - 从 `resized_128x128_with_blur_inverted/` 读取
   - 生成4个朝向的旋转版本
   - 输出到 `resized_128x128_with_blur_inverted_rotated/`

**典型处理链示例**:
```bash
# 完整管道 (推荐的默认流程)
python resize_and_mark.py          # 步骤1: 缩放到 128x128
python add_motion_blur.py          # 步骤2: 添加残影
python invert_colors.py            # 步骤3: 颜色反转
python rotate_images.py            # 步骤4: 旋转为4个方向
```

**其他可能的处理流程**:
```bash
# 简单流程: 只缩放
python resize_and_mark.py

# 缩放 + 反转
python resize_and_mark.py
python invert_colors.py  # 需要修改配置为 resized_128x128 → resized_128x128_inverted

# 缩放 + 残影 + 旋转 (跳过反转)
python resize_and_mark.py
python add_motion_blur.py
python rotate_images.py  # 需要修改配置为 resized_128x128_with_blur → ...
```

**注意**: 修改处理顺序时,需要调整各脚本 `main()` 函数中的输入输出路径配置

### 输出目录结构

```
mnist_tools/
├── original_images/                              # 原始 28x28 图像 (70,000张)
│   ├── training_*.jpg
│   └── test_*.jpg
├── resized_128x128/                              # 缩放后的图像(带横杠)
│   ├── 0/
│   ├── 1/
│   └── ... (0-9)
├── resized_128x128_with_blur/                    # 添加残影效果
│   ├── 0/
│   └── ...
├── resized_128x128_with_blur_inverted/           # 反转颜色
│   ├── 0/
│   └── ...
└── resized_128x128_with_blur_inverted_rotated/   # 旋转为4个朝向
    ├── up/
    │   ├── 0_up/
    │   ├── 1_up/
    │   └── ...
    ├── down/
    │   ├── 0_down/
    │   └── ...
    ├── left/
    │   ├── 0_left/
    │   └── ...
    └── right/
        ├── 0_right/
        └── ...
```

## 路径配置快速参考

### 所有可能的输入输出路径组合

**resize_and_mark.py**:
| 输入路径 | 输出路径 | 说明 |
|---------|---------|------|
| `./original_images` ✓ | `./resized_128x128` | 推荐,默认 |
| `.` | `./resized_128x128` | 图片在当前目录 |

**add_motion_blur.py**:
| 输入路径 | 输出路径 | 说明 |
|---------|---------|------|
| `./resized_128x128` ✓ | `./resized_128x128_with_blur` | 推荐,默认 |
| `./resized_128x128_inverted` | `./resized_128x128_inverted_with_blur` | 先反转后残影 |
| `./resized_128x128_rotated/up` | `./resized_128x128_rotated_with_blur/up` | 对旋转图像添加残影 |
| `./original_images` | `./original_images_with_blur` | 不推荐 |

**invert_colors.py**:
| 输入路径 | 输出路径 | 说明 |
|---------|---------|------|
| `./original_images` | `./original_images_inverted` | 反转原始图像 |
| `./resized_128x128` | `./resized_128x128_inverted` | 反转缩放图像 |
| `./resized_128x128_with_blur` ✓ | `./resized_128x128_with_blur_inverted` | 推荐,默认 |
| `./resized_128x128_rotated/up` | `./resized_128x128_rotated_inverted/up` | 反转旋转图像 |
| `./resized_128x128_with_blur_rotated/up` | `./resized_128x128_with_blur_rotated_inverted/up` | 反转残影+旋转 |

**rotate_images.py**:
| 输入路径 | 输出路径 | 说明 |
|---------|---------|------|
| `./original_images` | `./original_images_rotated` | 旋转原始图像 |
| `./resized_128x128` | `./resized_128x128_rotated` | 旋转缩放图像 |
| `./resized_128x128_inverted` | `./resized_128x128_inverted_rotated` | 旋转反转图像 |
| `./resized_128x128_with_blur` | `./resized_128x128_with_blur_rotated` | 旋转残影图像 |
| `./resized_128x128_with_blur_inverted` ✓ | `./resized_128x128_with_blur_inverted_rotated` | 推荐,默认 |
| `./resized_128x128_inverted_with_blur` | `./resized_128x128_inverted_with_blur_rotated` | 另一种顺序 |

## 重要提示

- **原始图像位置**: 所有 70,000 张原始 JPG 图片现在存放在 `original_images/` 文件夹中
- **修改配置**: 在各脚本的 `main()` 函数中取消注释相应配置选项
- **输入输出匹配**: 确保输入目录与前一步的输出目录匹配
- **目录结构检测**: 每个脚本都能自动检测输入目录结构(根目录或数字分类文件夹)
- **文件名后缀**: 文件名会累积添加后缀 (如 `_inverted`, `_up` 等)
- **独立函数调用**: 所有核心函数都可以单独调用用于自定义处理流程
- **处理顺序灵活**: 可以跳过某些步骤或改变处理顺序,只需调整配置即可
