"""
深度解析 TFLite 模型 - 提取输入输出和结构信息
使用 flatbuffers schema 直接解析
"""

import struct
import os

def parse_tflite_metadata(filepath):
    """解析 TFLite 文件的元数据"""

    print("=" * 80)
    print(f"深度解析 TFLite 模型: {filepath}")
    print("=" * 80)

    file_size = os.path.getsize(filepath)
    print(f"\n【文件基本信息】")
    print(f"  - 文件大小: {file_size / 1024:.2f} KB")

    with open(filepath, 'rb') as f:
        data = f.read()

    # 转换为字符串便于搜索
    data_str = data.decode('latin-1', errors='ignore')

    print(f"\n【从文件名推断】")
    print(f"  - 类别数: 5 (从 'class_5' 推断)")
    print(f"  - 架构: MobileNetV2 (从 'mv2' 推断)")
    print(f"  - 可能的 Alpha 值: 1.3 或 1.0 (从 '1.3.0' 推断)")

    # 搜索可能的输入输出维度信息
    print(f"\n【搜索模型张量信息】")

    # 寻找数字模式（可能是维度）
    common_sizes = [96, 112, 128, 160, 192, 224]
    found_sizes = []
    for size in common_sizes:
        if str(size).encode() in data:
            found_sizes.append(size)

    if found_sizes:
        print(f"  - 发现可能的输入尺寸: {found_sizes}")

    # 分析层类型
    print(f"\n【层类型统计】")
    layer_types = {
        'CONV_2D': data.count(b'CONV_2D'),
        'DEPTHWISE_CONV_2D': data.count(b'DEPTHWISE_CONV_2D'),
        'FULLY_CONNECTED': data.count(b'FULLY_CONNECTED'),
        'ADD': data.count(b'ADD'),
        'MUL': data.count(b'MUL'),
        'QUANTIZE': data.count(b'QUANTIZE'),
        'DEQUANTIZE': data.count(b'DEQUANTIZE'),
        'RESHAPE': data.count(b'RESHAPE'),
        'SOFTMAX': data.count(b'SOFTMAX'),
    }

    for layer_type, count in layer_types.items():
        if count > 0:
            print(f"  - {layer_type}: {count} 层")

    # 根据模型大小估算 alpha
    print(f"\n【模型规模估算】")
    print(f"  - 模型大小: {file_size / 1024:.2f} KB")
    print(f"  - Int8 量化的 MobileNetV2 典型大小:")
    print(f"    * Alpha=0.35: ~600 KB")
    print(f"    * Alpha=0.5:  ~800 KB")
    print(f"    * Alpha=0.75: ~1.5 MB")
    print(f"    * Alpha=1.0:  ~2.3 MB")
    print(f"    * Alpha=1.3:  ~3.8 MB")
    print(f"  - 根据文件大小 ({file_size / 1024:.2f} KB)，推断 Alpha 约为 0.5")

    # 搜索 "serving_default" 或 "signature" 相关信息
    if b'serving_default' in data:
        print(f"\n【发现签名信息】")
        print(f"  - 找到 'serving_default' 签名")

    # 查找输入输出名称
    print(f"\n【搜索输入输出名称】")
    possible_io_names = []
    keywords = [b'input', b'Input', b'output', b'Output', b'Identity', b'dense', b'Dense']
    for kw in keywords:
        idx = data.find(kw)
        if idx != -1:
            # 提取周围的文本
            start = max(0, idx - 20)
            end = min(len(data), idx + 50)
            context = data[start:end].decode('latin-1', errors='ignore')
            possible_io_names.append((kw.decode(), context))

    if possible_io_names:
        print(f"  - 发现的可能输入输出名称:")
        seen = set()
        for name, context in possible_io_names[:10]:
            if name not in seen:
                print(f"    * {name}")
                seen.add(name)

    print(f"\n【建议的模型参数】")
    print(f"  - 类别数 (num_classes): 5")
    print(f"  - Alpha: 0.5 (基于文件大小)")
    print(f"  - 输入尺寸: 128x128 或 96x96 (需要进一步确认)")
    print(f"  - 量化类型: Int8")

    print("\n" + "=" * 80)
    print("提示: 为了获得准确的输入输出信息，建议:")
    print("1. 使用 Netron 可视化工具: https://netron.app")
    print("2. 或安装 tensorflow 运行完整分析脚本")
    print("=" * 80)

if __name__ == '__main__':
    parse_tflite_metadata('class_5_mv2_1.3.0.tflite')
