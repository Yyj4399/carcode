"""
TFLite 模型结构分析脚本
解析 tflite 文件，提取模型的详细结构信息
"""

import tensorflow as tf
import numpy as np

def analyze_tflite_model(model_path):
    """分析 TFLite 模型的结构"""

    # 加载 TFLite 模型
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()

    # 获取输入和输出详情
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print("=" * 80)
    print(f"TFLite 模型分析: {model_path}")
    print("=" * 80)

    # 输入信息
    print("\n【输入层信息】")
    for i, input_detail in enumerate(input_details):
        print(f"  输入 {i}:")
        print(f"    - 名称: {input_detail['name']}")
        print(f"    - 形状: {input_detail['shape']}")
        print(f"    - 数据类型: {input_detail['dtype']}")
        print(f"    - 量化参数: scale={input_detail['quantization_parameters']['scales']}, "
              f"zero_point={input_detail['quantization_parameters']['zero_points']}")

    # 输出信息
    print("\n【输出层信息】")
    for i, output_detail in enumerate(output_details):
        print(f"  输出 {i}:")
        print(f"    - 名称: {output_detail['name']}")
        print(f"    - 形状: {output_detail['shape']}")
        print(f"    - 数据类型: {output_detail['dtype']}")
        print(f"    - 量化参数: scale={output_detail['quantization_parameters']['scales']}, "
              f"zero_point={output_detail['quantization_parameters']['zero_points']}")

    # 获取所有张量信息
    tensor_details = interpreter.get_tensor_details()

    print(f"\n【模型结构概览】")
    print(f"  - 总层数（张量数）: {len(tensor_details)}")

    # 分析模型层
    print(f"\n【详细层结构】（显示前20层和后20层）")
    print("-" * 80)

    # 显示前20层
    for i, tensor in enumerate(tensor_details[:20]):
        print(f"  层 {i}: {tensor['name']}")
        print(f"       形状: {tensor['shape']}, 类型: {tensor['dtype']}")

    if len(tensor_details) > 40:
        print(f"\n  ... 省略中间 {len(tensor_details) - 40} 层 ...\n")

    # 显示后20层
    for i, tensor in enumerate(tensor_details[-20:], start=len(tensor_details)-20):
        print(f"  层 {i}: {tensor['name']}")
        print(f"       形状: {tensor['shape']}, 类型: {tensor['dtype']}")

    # 分析关键层
    print("\n【关键层识别】")

    # 查找卷积层
    conv_layers = [t for t in tensor_details if 'Conv' in t['name'] or 'conv' in t['name']]
    print(f"  - 卷积层数量: {len(conv_layers)}")

    # 查找全连接层
    dense_layers = [t for t in tensor_details if 'dense' in t['name'].lower() or 'fully_connected' in t['name'].lower()]
    print(f"  - 全连接层数量: {len(dense_layers)}")
    if dense_layers:
        print(f"    最后一个全连接层: {dense_layers[-1]['name']}, 形状: {dense_layers[-1]['shape']}")

    # 查找池化层
    pool_layers = [t for t in tensor_details if 'pool' in t['name'].lower()]
    print(f"  - 池化层数量: {len(pool_layers)}")

    # 查找批归一化层
    bn_layers = [t for t in tensor_details if 'batch_norm' in t['name'].lower() or 'bn' in t['name'].lower()]
    print(f"  - 批归一化层数量: {len(bn_layers)}")

    # 估算模型大小
    import os
    model_size = os.path.getsize(model_path)
    print(f"\n【模型文件大小】")
    print(f"  - 文件大小: {model_size / 1024:.2f} KB ({model_size} bytes)")

    # 从输出形状推断类别数
    if output_details:
        output_shape = output_details[0]['shape']
        if len(output_shape) > 1:
            num_classes = output_shape[-1]
            print(f"\n【推断的模型参数】")
            print(f"  - 类别数: {num_classes}")
            print(f"  - 输入尺寸: {input_details[0]['shape'][1:3]} (高x宽)")
            print(f"  - 输入通道数: {input_details[0]['shape'][3]}")

    print("\n" + "=" * 80)

    return {
        'input_details': input_details,
        'output_details': output_details,
        'tensor_details': tensor_details,
        'model_size': model_size
    }


if __name__ == '__main__':
    # 分析模型
    model_path = 'class_5_mv2_1.3.0.tflite'
    model_info = analyze_tflite_model(model_path)
