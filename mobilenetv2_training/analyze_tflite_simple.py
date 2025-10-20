"""
使用 flatbuffers 直接解析 TFLite 模型
如果没有 tensorflow，则使用更轻量的方法
"""

try:
    # 尝试使用 netron 的方式（如果可用）
    import json
    import struct

    def read_tflite_simple(filepath):
        """简单读取 tflite 文件的基本信息"""
        print("=" * 80)
        print(f"TFLite 模型分析: {filepath}")
        print("=" * 80)

        # 读取文件大小
        import os
        file_size = os.path.getsize(filepath)
        print(f"\n【文件信息】")
        print(f"  - 文件大小: {file_size / 1024:.2f} KB ({file_size} bytes)")

        # 读取文件的前几个字节来验证格式
        with open(filepath, 'rb') as f:
            # TFLite 文件是 FlatBuffer 格式
            header = f.read(8)
            print(f"  - 文件头: {header.hex()[:32]}")

            # 读取整个文件
            f.seek(0)
            data = f.read()

        print(f"\n【模型特征】")
        print(f"  - 文件名称: {filepath}")
        print(f"  - 从文件名推断: class_5 表示 5 个类别")
        print(f"  - 从文件名推断: mv2 表示 MobileNetV2")
        print(f"  - 从文件名推断: 1.3.0 可能是版本号或 alpha 值")

        # 尝试找到一些关键字符串
        data_str = data.decode('latin-1', errors='ignore')

        # 寻找可能的层名称
        keywords = ['Conv', 'Dense', 'Pooling', 'BatchNorm', 'Add', 'Mul', 'input', 'output']
        found_keywords = {}
        for kw in keywords:
            count = data_str.count(kw)
            if count > 0:
                found_keywords[kw] = count

        if found_keywords:
            print(f"\n【在模型中找到的关键层名】")
            for kw, count in sorted(found_keywords.items(), key=lambda x: x[1], reverse=True):
                print(f"  - {kw}: 出现 {count} 次")

        print("\n" + "=" * 80)
        print("提示: 请使用 Netron (https://netron.app) 可视化查看完整模型结构")
        print("或者安装 tensorflow 后使用完整分析脚本")
        print("=" * 80)

    read_tflite_simple('class_5_mv2_1.3.0.tflite')

except Exception as e:
    print(f"基础分析出错: {e}")
    print("\n建议:")
    print("1. 使用在线工具 https://netron.app 上传 tflite 文件查看结构")
    print("2. 或安装 tensorflow: pip install tensorflow")
