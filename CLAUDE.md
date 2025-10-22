# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

这是一个多项目代码仓库，包含用于图像处理和深度学习模型训练的工具。主要包括两个子项目：

1. **mnist_tools**: MNIST 手写数字数据集的图像处理工具集
2. **mobilenetv2_training**: MobileNetV2 深度学习模型训练项目

## 仓库结构

```
num/
├── mnist_tools/              # MNIST 图像处理工具集
│   ├── original_images/      # 原始 28x28 MNIST 图像 (70,000张，不在版本控制中)
│   ├── *.py                  # 各种图像处理脚本
│   ├── CLAUDE.md             # mnist_tools 详细文档
│   ├── README.md             # 用户文档
│   └── requirements.txt      # Python 依赖
├── mobilenetv2_training/     # MobileNetV2 训练项目
│   ├── train_mobilenetv2.py  # 主训练脚本
│   └── MobileNetV2训练说明.md  # 训练文档
└── CLAUDE.md                 # 本文档（项目根目录）
```

## 环境配置

### Python 环境

推荐使用 Miniconda 或 Anaconda：

```bash
# 激活环境（如果使用 conda）
conda activate myconda

# 或使用项目特定的虚拟环境
# 路径: /root/miniconda3/envs/myconda
```

### 安装依赖

**mnist_tools 项目**：
```bash
cd mnist_tools
pip install -r requirements.txt
# 依赖: Pillow>=10.0.0, numpy>=1.24.0, tqdm>=4.65.0
```

**mobilenetv2_training 项目**：
```bash
# CPU 版本
pip install tensorflow numpy matplotlib seaborn scikit-learn

# GPU 版本（推荐，训练速度快 5-10 倍）
pip install tensorflow[and-cuda] numpy matplotlib seaborn scikit-learn
```

## mnist_tools 项目

### 核心概念

这是一个 MNIST 数据增强管道，通过一系列脚本对 28x28 手写数字图像进行处理：

1. **缩放和标记** (`resize_and_mark.py`): 28x28 → 128x128，为数字 6 和 9 添加特征横杠
2. **残影效果** (`add_motion_blur.py`): 添加运动模糊或残影效果
3. **颜色反转** (`invert_colors.py`): 黑白像素反转
4. **图像旋转** (`rotate_images.py`): 生成 4 个旋转方向（上/下/左/右）
5. **数字融合** (`merge_digits.py`): 将两个单独数字融合成两位数图像

### 常用命令

```bash
cd mnist_tools

# 完整数据增强管道（推荐）
python resize_and_mark.py        # 步骤1: 缩放到 128x128
python add_motion_blur.py        # 步骤2: 添加残影
python invert_colors.py          # 步骤3: 颜色反转
python rotate_images.py          # 步骤4: 旋转为4个方向

# 生成两位数图像
python merge_digits.py           # 融合单个数字为两位数

# 批量处理旋转图像反转
python batch_invert_rotated.py  # 批量处理所有旋转方向的像素反转
```

### 重要提示

- **原始图像位置**: 所有 70,000 张原始 JPG 图片存放在 `original_images/` 文件夹中
- **配置修改**: 所有脚本的输入输出路径在各自的 `main()` 函数中配置
- **处理顺序**: 确保输入目录与前一步的输出目录匹配
- **详细文档**: 查看 `mnist_tools/CLAUDE.md` 获取每个脚本的详细说明

### 脚本编号系统

mnist_tools 使用编号系统组织脚本：

- **脚本 1-9**: 核心处理脚本（按处理流程编号）
- **脚本 10-14**: 辅助工具脚本（批处理、测试、重命名）

## mobilenetv2_training 项目

### 核心概念

使用 MobileNetV2 架构训练轻量级图像分类模型，支持：
- 自动类别检测
- GPU 加速训练
- Int8 量化（模型大小约 1000KB）
- 交互式多轮训练
- 混淆矩阵可视化

### 数据集要求

```
/mnt/image/
├── train/              # 训练集
│   ├── class_1/
│   ├── class_2/
│   └── class_n/
└── test/               # 测试集
    ├── class_1/
    ├── class_2/
    └── class_n/
```

### 常用命令

```bash
cd mobilenetv2_training

# 基础训练（交互式模式）
python train_mobilenetv2.py

# 自定义参数训练
python train_mobilenetv2.py --epoch 20 --lr 0.001 --batch_size 16

# 启用数据增强
python train_mobilenetv2.py --epoch 20 --augmentation

# 自动继续训练模式（无需交互）
python train_mobilenetv2.py --epoch 20 --continue_epochs 10 --continue_lr 0.0005 --auto_continue

# 查看所有参数
python train_mobilenetv2.py --help
```

### 训练模式

**交互式模式（默认）**：
- 每轮训练后询问是否继续
- 可动态调整学习率、批次大小和数据增强
- 适合实验性训练

**自动模式（`--auto_continue`）**：
- 完全不询问，按命令行参数执行
- 适合批量训练和自动化流程

### 训练输出文件

- `mobilenetv2_model.h5`: 完整 Keras 模型（~8MB）
- `mobilenetv2_int8.tflite`: Int8 量化 TFLite 模型（~1MB）
- `confusion_matrix.png`: 混淆矩阵可视化
- `classification_report.txt`: 详细分类报告
- `mobilenetv2_checkpoint.h5`: 训练检查点

## Git 工作流

### 当前分支

- 主分支: `main`
- 当前分支: `main`

### 提交规范

根据修改类型使用以下前缀：
- `添加`: 新增功能或文件
- `更新`: 增强现有功能
- `修复`: Bug 修复
- `重构`: 代码重构
- `文档`: 文档更新
- `合并`: 分支合并

示例：
```bash
git add .
git commit -m "添加MobileNetV2训练项目"
git push
```

## 代码架构模式

### mnist_tools 脚本模式

所有图像处理脚本遵循统一模式：

1. **核心处理函数**: 处理单张图像（例如 `process_image()`, `invert_image()`）
2. **批量处理函数**: `main()` 函数处理整个目录
3. **自动检测**: 自动识别输入目录结构
4. **统计输出**: 显示详细处理统计
5. **进度显示**: 使用 tqdm 显示实时进度

### 配置修改方法

1. 打开对应的 `.py` 文件
2. 找到 `main()` 函数（通常在文件末尾）
3. 修改 `input_dir` 和 `output_dir` 变量
4. 某些脚本还有额外参数（如 `blur_type`, `intensity`）

## 数据集路径配置

### mnist_tools

- 原始图像: `./mnist_tools/original_images/` (70,000 张 28x28 JPG)
- 处理后图像: 各种 `resized_*` 和 `merged_*` 文件夹

### mobilenetv2_training

在 `train_mobilenetv2.py` 的 `Config` 类中修改：

```python
class Config:
    TRAIN_DIR = '/mnt/image/train'  # 训练集路径
    TEST_DIR = '/mnt/image/test'    # 测试集路径
```

## GPU 配置

MobileNetV2 训练脚本会自动检测和配置 GPU：
- 自动检测可用 GPU 设备
- 启用 GPU 内存增长模式（动态分配显存）
- 无 GPU 时自动回退到 CPU 训练

验证 GPU 可用性：
```bash
python -c "import tensorflow as tf; print('GPU可用:', len(tf.config.list_physical_devices('GPU')) > 0)"
```

## 常见任务

### 处理新的 MNIST 数据集

```bash
cd mnist_tools

# 1. 将原始图像放入 original_images/
# 2. 运行完整管道
python resize_and_mark.py
python add_motion_blur.py
python invert_colors.py
python rotate_images.py
```

### 训练新的分类模型

```bash
cd mobilenetv2_training

# 1. 准备数据集（train/ 和 test/ 文件夹）
# 2. 运行训练（GPU 推荐）
python train_mobilenetv2.py --epoch 20 --augmentation

# 3. 根据提示进行交互式调整或查看结果
```

### 生成两位数数据集

```bash
cd mnist_tools

# 随机模式（生成 1000 张）
python merge_digits.py  # 在 main() 中设置 mode = "random"

# 顺序模式（生成所有有效两位数 01-99）
python merge_digits.py  # 在 main() 中设置 mode = "sequential"
```

## 注意事项

1. **大文件**: 所有图像文件（原始和处理后）都在 `.gitignore` 中，不提交到版本控制
2. **路径格式**: Windows 路径使用原始字符串（`r"D:\path"`）或双反斜杠（`"D:\\path"`）
3. **内存占用**:
   - mnist_tools: 图像处理内存占用较小
   - mobilenetv2_training: 训练时根据 batch_size 调整内存（可用 `--batch_size` 减小）
4. **数据完整性**: mobilenetv2_training 会在训练前自动验证和清理损坏的图像文件
5. **文档更新**: 每次修改代码后，更新对应的 md 文档

## 详细文档

- **mnist_tools 详细架构**: 查看 `mnist_tools/CLAUDE.md`（包含所有脚本的详细说明、参数、输出格式）
- **MobileNetV2 训练指南**: 查看 `mobilenetv2_training/MobileNetV2训练说明.md`（包含完整参数说明、训练策略、故障排除）

## 许可证

MIT License
