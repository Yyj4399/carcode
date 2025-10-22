"""
验证 TFLite 模型是否为 int8 量化
"""

import tensorflow as tf
import numpy as np

def verify_int8_model(model_path):
    """
    验证 TFLite 模型的量化类型

    参数:
        model_path: TFLite 模型路径
    """
    print("=" * 60)
    print("TFLite 模型量化类型验证")
    print("=" * 60)

    # 加载模型
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()

    # 获取输入输出详情
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print(f"\n模型路径: {model_path}")
    print("\n" + "-" * 60)
    print("输入张量信息:")
    print("-" * 60)

    for i, input_detail in enumerate(input_details):
        dtype = input_detail['dtype']
        shape = input_detail['shape']
        quantization = input_detail['quantization']

        print(f"\n输入 {i}:")
        print(f"  形状: {shape}")
        print(f"  数据类型: {dtype}")

        # 验证是否为 int8
        if dtype == np.int8:
            print("  ✓ 确认为 int8 (有符号 8 位整数)")
            print("  ✓ 数值范围: -128 到 127")
        elif dtype == np.uint8:
            print("  ✗ 警告: 这是 uint8 (无符号 8 位整数)")
            print("  ✗ 数值范围: 0 到 255")
        else:
            print(f"  ⚠ 其他类型: {dtype}")

        # 显示量化参数
        if quantization[0] != 0.0 or quantization[1] != 0:
            print(f"  量化参数:")
            print(f"    - Scale (缩放): {quantization[0]}")
            print(f"    - Zero Point (零点): {quantization[1]}")

    print("\n" + "-" * 60)
    print("输出张量信息:")
    print("-" * 60)

    for i, output_detail in enumerate(output_details):
        dtype = output_detail['dtype']
        shape = output_detail['shape']
        quantization = output_detail['quantization']

        print(f"\n输出 {i}:")
        print(f"  形状: {shape}")
        print(f"  数据类型: {dtype}")

        # 验证是否为 int8
        if dtype == np.int8:
            print("  ✓ 确认为 int8 (有符号 8 位整数)")
            print("  ✓ 数值范围: -128 到 127")
        elif dtype == np.uint8:
            print("  ✗ 警告: 这是 uint8 (无符号 8 位整数)")
            print("  ✗ 数值范围: 0 到 255")
        else:
            print(f"  ⚠ 其他类型: {dtype}")

        # 显示量化参数
        if quantization[0] != 0.0 or quantization[1] != 0:
            print(f"  量化参数:")
            print(f"    - Scale (缩放): {quantization[0]}")
            print(f"    - Zero Point (零点): {quantization[1]}")

    # 验证内部操作
    print("\n" + "-" * 60)
    print("内部操作验证:")
    print("-" * 60)

    # 获取模型的所有张量
    tensor_details = interpreter.get_tensor_details()

    int8_count = 0
    uint8_count = 0
    float_count = 0
    other_count = 0

    for tensor in tensor_details:
        dtype = tensor['dtype']
        if dtype == np.int8:
            int8_count += 1
        elif dtype == np.uint8:
            uint8_count += 1
        elif dtype in [np.float32, np.float16]:
            float_count += 1
        else:
            other_count += 1

    total_tensors = len(tensor_details)

    print(f"\n总张量数: {total_tensors}")
    print(f"  - int8 张量: {int8_count} ({int8_count/total_tensors*100:.1f}%)")
    print(f"  - uint8 张量: {uint8_count} ({uint8_count/total_tensors*100:.1f}%)")
    print(f"  - float 张量: {float_count} ({float_count/total_tensors*100:.1f}%)")
    print(f"  - 其他类型: {other_count} ({other_count/total_tensors*100:.1f}%)")

    # 最终结论
    print("\n" + "=" * 60)
    print("验证结果:")
    print("=" * 60)

    input_is_int8 = all(detail['dtype'] == np.int8 for detail in input_details)
    output_is_int8 = all(detail['dtype'] == np.int8 for detail in output_details)

    if input_is_int8 and output_is_int8:
        print("\n✓✓✓ 模型已成功量化为 int8 (有符号)")
        print("✓✓✓ 输入和输出均为 int8 类型")
        print("✓✓✓ 数值范围: -128 到 127")
    else:
        print("\n✗✗✗ 警告: 模型量化类型不符合预期")
        if not input_is_int8:
            print(f"✗ 输入类型: {input_details[0]['dtype']}")
        if not output_is_int8:
            print(f"✗ 输出类型: {output_details[0]['dtype']}")

    print("=" * 60)

    return input_is_int8 and output_is_int8


if __name__ == '__main__':
    # 验证模型
    model_path = 'mobilenetv2_int8.tflite'

    try:
        is_int8 = verify_int8_model(model_path)

        if is_int8:
            print("\n✓ 验证通过: 模型为 int8 量化")
        else:
            print("\n✗ 验证失败: 模型不是 int8 量化")

    except FileNotFoundError:
        print(f"\n✗ 错误: 找不到模型文件 '{model_path}'")
        print("  请确保已运行训练脚本生成模型")
    except Exception as e:
        print(f"\n✗ 错误: {e}")
