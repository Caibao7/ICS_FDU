import matplotlib.pyplot as plt
import seaborn as sns
import re

def parse_performance_file(filename):
    performance_data = {}
    instruction_pattern = re.compile(r'(\w+q) Instructions Executed:\s+(\d+)')
    cache_pattern = re.compile(r'(Cache Hits|Cache Misses|Cache Evictions|Cache Hit Rate|Cache Eviction Rate):\s+([\d.]+)')
    total_instructions_pattern = re.compile(r'Total Instructions Executed:\s+(\d+)')
    
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            
            # 匹配总指令数
            total_match = total_instructions_pattern.match(line)
            if total_match:
                performance_data['Total Instructions Executed'] = int(total_match.group(1))
                continue
            
            # 匹配指令执行次数
            instr_match = instruction_pattern.match(line)
            if instr_match:
                instr = instr_match.group(1)
                count = int(instr_match.group(2))
                performance_data[instr] = count
                continue
            
            # 匹配缓存统计
            cache_match = cache_pattern.match(line)
            if cache_match:
                key = cache_match.group(1)
                value = float(cache_match.group(2))
                performance_data[key] = value
                continue
    
    return performance_data

def plot_instruction_counts(performance_data, ins_address):
    # 提取指令执行次数
    instructions = ['addq', 'subq', 'andq', 'xorq', 'mulq', 'divq', 'remq']
    counts = [performance_data.get(instr, 0) for instr in instructions]
    
    # 设置图表风格
    sns.set(style="whitegrid")
    
    plt.figure(figsize=(10, 6))
    sns.barplot(x=instructions, y=counts, palette="viridis")
    
    plt.title('Instruction Execution Counts', fontsize=16)
    plt.xlabel('Instructions', fontsize=14)
    plt.ylabel('Execution Count', fontsize=14)
    
    # 添加数值标签
    for index, value in enumerate(counts):
        plt.text(index, value + max(counts)*0.01, str(value), ha='center', va='bottom', fontsize=12)
    
    plt.tight_layout()
    plt.savefig(ins_address)  # 保存图表为PNG文件
    plt.show()

def plot_cache_statistics(performance_data, cache_address):
    cache_hits = performance_data.get('Cache Hits', 0)
    cache_misses = performance_data.get('Cache Misses', 0)
    cache_evictions = performance_data.get('Cache Evictions', 0)
    
    # 计算Cache Misses中不涉及驱逐的部分
    cache_misses_without_evictions = cache_misses - cache_evictions
    if cache_misses_without_evictions < 0:
        cache_misses_without_evictions = 0  # 防止负数
    
    # 准备数据
    labels = ['Cache Hits', 'Cache Misses without Evictions', 'Cache Evictions']
    sizes = [cache_hits, cache_misses_without_evictions, cache_evictions]
    
    # 为饼图设置颜色
    colors = ['#66b3ff','#99ff99','#ff9999']
    
    # 设置图表风格
    sns.set(style="white")
    
    plt.figure(figsize=(8, 8))
    plt.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=140, colors=colors, explode=(0, 0.1, 0.1))
    
    plt.title('Cache Statistics', fontsize=16)
    plt.axis('equal')  # 确保饼图是圆形
    
    plt.savefig(cache_address)  # 保存图表为PNG文件
    plt.show()

def main():
    # 文件路径变量
    ##############################
    txt_address = '../performance/ret-hazard.txt'
    ins_address = '../performance/images/ins_counts_ret.png'
    cache_address = '../performance/images/cache_stat_ret.png'
    ##############################
    
    # 解析性能报告
    performance_data = parse_performance_file(txt_address)
    
    # 打印解析结果（可选）
    print("Parsed Performance Data:")
    for key, value in performance_data.items():
        print(f"{key}: {value}")
    
    # 绘制指令执行次数柱状图
    plot_instruction_counts(performance_data, ins_address)
    
    # 绘制缓存统计饼图
    plot_cache_statistics(performance_data, cache_address)

if __name__ == "__main__":
    main()
