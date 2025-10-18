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
├── merged_digits_128x128/    # 融合后的两位数图像
├── resize_and_mark.py        # 脚本1: 缩放和标记
├── add_motion_blur.py        # 脚本2: 添加残影
├── invert_colors.py          # 脚本3: 颜色反转
├── rotate_images.py          # 脚本4: 图像旋转
├── merge_digits.py           # 脚本5: 数字融合
├── delete_unwanted_images.py # 脚本6: 删除不需要的图像
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

### 5. merge_digits.py - 数字融合

将两个单独的数字图像水平拼接成一个包含两位数的 128x128 图像。

**功能**:
- 将两张单独的数字图像拼接成两位数
- **自动裁剪白边**: 去除数字周围多余空白,保留主体（5像素安全边距）
- **优化间距**: 根据数字宽度自动计算合理间距(3-6%),更自然紧凑
- **居中对齐**: 两个数字在画布中水平和垂直居中,确保视觉平衡
- **完整显示**: 数字占画布80%高度,保证不被裁剪
- **避免白线**: 直接在目标尺寸画布上粘贴,无需二次缩放
- 支持随机生成模式和顺序生成模式
- 输出尺寸统一为 128x128 像素

**融合规则**:
- **0不能为个位**: 避免生成 10, 20, 30... 等
- **自动排除00**: 由于0不能为个位，00自然被排除
- **有效范围**: 01-09, 11-19, 21-29, ..., 91-99 (共90个两位数)

**核心函数**:
- `crop_whitespace(img, threshold)` (merge_digits.py:14) - 裁剪白边，保留5像素安全边距
- `merge_two_digits(img1, img2, output_size)` (merge_digits.py:53) - 融合两个数字

**运行**:
```bash
python merge_digits.py
```

**配置位置**: `main()` 函数 (merge_digits.py:286)

**可用的输入输出路径配置**:
- ✓ `./resized_128x128` → `./merged_digits_128x128` (推荐,默认)
- `./resized_128x128_with_blur` → `./merged_digits_128x128`
- `./resized_128x128_inverted` → `./merged_digits_128x128`
- `./resized_128x128_rotated/up` → `./merged_digits_128x128` (需要指定朝向)
- `./original_images` → `./merged_digits_128x128` (不推荐,图像太小)

**生成模式**:

**模式1: 随机生成** (默认)
- 随机选择两个数字进行融合
- 可设置生成数量 (默认1000张)
- 输出文件名: `merged_{索引:06d}_{两位数:02d}.jpg`
- 示例: `merged_000000_87.jpg` (索引0,数字87)

**模式2: 顺序生成**
- 生成所有有效两位数 (01-09, 11-19, ..., 91-99)
- 共90个两位数（排除个位为0的情况）
- 每个两位数可生成多张不同组合
- 自动创建子文件夹按两位数分类
- 输出文件名: `merged_{两位数:02d}_{索引:04d}.jpg`

**输出**: `./merged_digits_128x128/`

**技术细节**:
- **白边裁剪**: 使用numpy检测非白色区域边界,保留5像素安全边距
- **高度统一**: 两个数字缩放到相同高度(输出尺寸的80%)
- **优化间距**: 间距为较小数字宽度的3-6%,更紧凑自然
- **居中布局**: 整体内容在128x128画布中水平和垂直居中
- **无二次缩放**: 直接在目标尺寸画布上操作,避免缩放失真和白线
- **LANCZOS重采样**: 使用高质量重采样算法,避免锯齿和白线

**改进说明** (2025-10-18):
- 新增 `crop_whitespace()` 函数自动裁剪白边，增加5像素安全边距
- 优化间距算法，从8-12%缩小到3-6%，更自然紧凑
- 采用居中布局，替代之前的直接拼接+缩放
- 数字占画布80%高度，确保完整显示不被裁剪
- 优化生成规则，0不能为个位，共90个有效两位数
- 简化随机生成逻辑，直接从1-9生成个位数
- 避免中间出现白线问题

**使用示例**:
```python
# 修改 main() 函数中的配置
mode = "random"              # 随机模式
num_random_images = 1000    # 生成1000张

# 或者使用顺序模式
mode = "sequential"          # 顺序模式
images_per_number = 100     # 每个两位数生成100张
```

### 6. delete_unwanted_images.py - 删除不需要的图像文件

删除文件名中包含特定字符的图像文件(如"副本"或"_")。

**功能**:
- 扫描指定文件夹及其子文件夹中的所有图像文件
- 删除文件名包含指定模式的图像(默认: "副本", "_")
- 支持常见图像格式: .jpg, .jpeg, .png, .bmp, .gif
- 提供预览模式,安全确认后再删除
- 显示详细统计信息

**核心函数**:
- `should_delete(filename)` (delete_unwanted_images.py:20) - 判断是否删除
- `delete_unwanted_images(input_dir, dry_run)` (delete_unwanted_images.py:35) - 删除处理

**运行**:
```bash
python delete_unwanted_images.py
```

**配置位置**: `main()` 函数 (delete_unwanted_images.py:95)

**可配置参数**:
- `INPUT_DIR`: 要处理的文件夹路径 (默认: 当前目录)
- `DELETE_PATTERNS`: 要删除的文件名特征列表 (默认: ['副本', '_'])
- `dry_run`: 是否为预览模式 (默认: True)

**安全特性**:
- 默认开启预览模式,不会直接删除文件
- 显示所有将要删除的文件列表
- 需要用户输入 'yes' 确认后才会执行删除
- 显示删除前后的统计对比

**使用场景**:
- 清理重复的副本文件
- 删除包含特殊字符的临时文件
- 批量清理不符合命名规范的图像

**使用示例**:
```python
# 修改配置删除特定模式的文件
INPUT_DIR = r"D:\your\folder\path"
DELETE_PATTERNS = ['副本', '_', 'temp']  # 自定义删除模式

# 实际删除(关闭预览模式)
delete_unwanted_images(INPUT_DIR, dry_run=False)
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
├── merged_digits_128x128/                        # 融合后的两位数图像
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

**merge_digits.py**:
| 输入路径 | 输出路径 | 说明 |
|---------|---------|------|
| `./resized_128x128` ✓ | `./merged_digits_128x128` | 推荐,默认 |
| `./resized_128x128_with_blur` | `./merged_digits_128x128` | 使用带残影的图像 |
| `./resized_128x128_inverted` | `./merged_digits_128x128` | 使用反转图像 |
| `./resized_128x128_rotated/up` | `./merged_digits_128x128` | 使用旋转图像 |

## 重要提示

- **原始图像位置**: 所有 70,000 张原始 JPG 图片现在存放在 `original_images/` 文件夹中
- **修改配置**: 在各脚本的 `main()` 函数中取消注释相应配置选项
- **输入输出匹配**: 确保输入目录与前一步的输出目录匹配
- **目录结构检测**: 每个脚本都能自动检测输入目录结构(根目录或数字分类文件夹)
- **文件名后缀**: 文件名会累积添加后缀 (如 `_inverted`, `_up` 等)
- **独立函数调用**: 所有核心函数都可以单独调用用于自定义处理流程
- **处理顺序灵活**: 可以跳过某些步骤或改变处理顺序,只需调整配置即可
