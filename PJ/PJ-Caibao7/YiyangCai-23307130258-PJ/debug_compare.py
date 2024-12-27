import sys
import os
from ruamel.yaml import YAML
from ruamel.yaml.comments import CommentedMap, CommentedSeq

# 文件路径变量
######################################
FILE1_PATH = "../answer/abs-asum-cmov.yml"
FILE2_PATH = "../output/abs-asum-cmov.yml"
######################################

def load_yaml(file_path):
    yaml = YAML()
    yaml.preserve_quotes = True
    try:
        print(f"尝试加载文件: {file_path}")
        if not os.path.isfile(file_path):
            print(f"错误: 文件 {file_path} 不存在。")
            sys.exit(1)
        with open(file_path, 'r', encoding='utf-8') as f:
            data = yaml.load(f)
            return data
    except FileNotFoundError:
        print(f"错误: 文件 {file_path} 未找到。")
        sys.exit(1)
    except Exception as e:
        print(f"错误: 解析 YAML 文件 {file_path} 时出错。\n{e}")
        sys.exit(1)


# 获取行号
def get_line_map(data, key):
    if isinstance(data, CommentedMap):
        line = data.lc.key(key)
        if line is not None:
            return line[0] + 1  # 行号从0开始，故加1
    return -1

def get_line_seq(seq, index):
    if isinstance(seq, CommentedSeq):
        line = seq.lc.data.get(index, None)
        if line is not None:
            return line[0] + 1  # 行号从0开始，故加1
    return -1

def get_line_value(data):
    if hasattr(data, 'lc'):
        return data.lc.line + 1
    return -1

def compare(data1, data2, path="", parent1=None, key1=None, parent2=None, key2=None):
    if type(data1) != type(data2):
        line1 = get_line_map(parent1, key1) if isinstance(parent1, CommentedMap) and key1 is not None else get_line_value(data1)
        line2 = get_line_map(parent2, key2) if isinstance(parent2, CommentedMap) and key2 is not None else get_line_value(data2)
        print(f"不同类型在路径 '{path}'。")
        print(f"文件1类型: {type(data1).__name__} (行 {line1})")
        print(f"文件2类型: {type(data2).__name__} (行 {line2})")
        sys.exit(0)

    if isinstance(data1, CommentedMap):
        keys1 = set(data1.keys())
        keys2 = set(data2.keys())
        for key in keys1:
            current_path = f"{path}.{key}" if path else key
            if key not in data2:
                line1 = get_line_map(data1, key)
                print(f"键 '{current_path}' 在文件1中存在但在文件2中缺失。")
                print(f"文件1: 行 {line1}")
                sys.exit(0)
            # Compare with parent info
            compare(data1[key], data2[key], current_path, parent1=data1, key1=key, parent2=data2, key2=key)
        for key in keys2 - keys1:
            current_path = f"{path}.{key}" if path else key
            line2 = get_line_map(data2, key)
            print(f"键 '{current_path}' 在文件2中存在但在文件1中缺失。")
            print(f"文件2: 行 {line2}")
            sys.exit(0)
    elif isinstance(data1, CommentedSeq):
        len1 = len(data1)
        len2 = len(data2)
        min_len = min(len1, len2)
        for index in range(min_len):
            current_path = f"{path}[{index}]"
            compare(data1[index], data2[index], current_path, parent1=data1, key1=index, parent2=data2, key2=index)
        if len1 != len2:
            if len1 > len2:
                line1 = get_line_seq(data1, len2) if len2 < len1 else -1
                print(f"列表 '{path}' 在文件1中比文件2长。")
                print(f"文件1: 行 {line1}")
            else:
                line2 = get_line_seq(data2, len1) if len1 < len2 else -1
                print(f"列表 '{path}' 在文件2中比文件1长。")
                print(f"文件2: 行 {line2}")
            sys.exit(0)
    else:
        if data1 != data2:
            # For scalar values, get the line from parent and key
            line1 = get_line_map(parent1, key1) if isinstance(parent1, CommentedMap) and key1 is not None else get_line_value(data1)
            line2 = get_line_map(parent2, key2) if isinstance(parent2, CommentedMap) and key2 is not None else get_line_value(data2)
            print(f"值不同在路径 '{path}'。")
            print(f"文件1: {data1} (行 {line1})")
            print(f"文件2: {data2} (行 {line2})")
            sys.exit(0)

def main():
    # 打印当前工作目录
    cwd = os.getcwd()
    print(f"当前工作目录: {cwd}")

    # 检查文件路径是否存在
    print(f"检查文件 {FILE1_PATH} 是否存在: {os.path.isfile(FILE1_PATH)}")
    print(f"检查文件 {FILE2_PATH} 是否存在: {os.path.isfile(FILE2_PATH)}")

    # 加载 YAML 文件
    data1 = load_yaml(FILE1_PATH)
    data2 = load_yaml(FILE2_PATH)

    # 比较 YAML 数据
    compare(data1, data2)

    print("两个文件内容完全相同。")

if __name__ == "__main__":
    main()
