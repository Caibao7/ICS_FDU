#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <thread> // 调试
#include <chrono> // 调试

#define NUM_REGS 16
#define MEM_SIZE 4000  // 内存大小

// 内存类
class Memory {
public:
    std::vector<char> mem;  // 内存数据

    Memory(size_t size) : mem(size, 0) {}  // 初始化内存大小

    // 从文件加载数据到内存
    bool load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        // 去除字符串前后空白的函数
        auto trim = [](std::string &s) {
            size_t start = s.find_first_not_of(" \t");
            size_t end = s.find_last_not_of(" \t");
            if (start == std::string::npos) {
                s.clear();
            } else {
                s = s.substr(start, end - start + 1);
            }
        };

        std::string line;
        while (std::getline(file, line)) {
            trim(line);
            if (line.empty()) continue;

            // 查找冒号和管道符
            size_t colon_pos = line.find(':');
            size_t pipe_pos = line.find('|');
            if (colon_pos == std::string::npos || pipe_pos == std::string::npos || pipe_pos <= colon_pos) {
                // 如果没有正确的 ':' 和 '|' 或者 '|'' 在 ':' 之前，跳过
                continue;
            }

            // 截取 ':' 和 '|' 之间的内容
            std::string content = line.substr(colon_pos + 1, pipe_pos - (colon_pos + 1));
            trim(content);

            // 如果截取内容为空，则跳过此行
            if (content.empty()) {
                continue;
            }

            // 确定当前地址（:前面的部分）
            std::string address_str = line.substr(0, colon_pos);
            trim(address_str);
            uint64_t current_address = 0;
            try {
                current_address = std::stoull(address_str, nullptr, 16);
            } catch (const std::exception &e) {
                std::cerr << "Error parsing address: " << address_str << " (" << e.what() << ")\n";
                continue;
            }

            // 判断content是否为纯十六进制数据
            auto is_hex_char = [](char c) {
                return (c >= '0' && c <= '9') ||
                    (c >= 'a' && c <= 'f') ||
                    (c >= 'A' && c <= 'F');
            };
            bool is_hex_data = !content.empty() && std::all_of(content.begin(), content.end(), is_hex_char);
            if (!is_hex_data) {
                // 如果不是十六进制数据，跳过
                continue;
            }

            // 十六进制数据长度必须为偶数（每两个字符表示一个字节）
            if (content.size() % 2 != 0) {
                std::cerr << "Incomplete hex data: " << content << std::endl;
                continue;
            }

            // 写入数据到mem中
            for (size_t i = 0; i < content.size(); i += 2) {
                std::string byte_str = content.substr(i, 2);
                uint8_t byte = 0;
                try {
                    byte = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
                } catch (const std::exception &e) {
                    std::cerr << "Error parsing byte: " << byte_str << " (" << e.what() << ")\n";
                    break;
                }

                if (current_address >= mem.size()) {
                    std::cerr << "Memory address out of range: 0x" << std::hex << current_address << std::dec << std::endl;
                    break;
                }

                mem[current_address++] = byte;
            }
        }

        file.close();
        return true;
    }

    // 读取内存指定位置的数据
    int read(int addr) const {
        return int(static_cast<uint8_t>(mem[addr]));
    }

    // 写入数据到内存指定位置
    void write(int addr, char data) {
        mem[addr] = data;
    }
};

// 寄存器、PC 和条件码类
class CPUState {
public:

    long long regs[NUM_REGS];  // 寄存器
    int PC;  // 程序计数器
    int ZF, SF, OF;// 条件码（零标志，符号标志，溢出标志）  
    int Stat = 1;  // 状态码

    // 性能计数器
    int instruction_count = 0;
    int addq_count = 0;
    int subq_count = 0;
    int andq_count = 0;
    int xorq_count = 0;
    int mulq_count = 0;
    int divq_count = 0;
    int remq_count = 0;
    int cache_hits = 0;
    int cache_misses = 0;
    int pipeline_stalls = 0;

    CPUState() : PC(0), ZF(1), SF(0), OF(0) {
        std::fill(std::begin(regs), std::end(regs), 0);  // 初始化寄存器为0
    }

    // 更新条件码
    void update_condition_codes(int64_t valA, int64_t valB, int64_t valE, uint8_t icode, uint8_t ifun) {
        // 更新零标志（ZF）
        ZF = (valE == 0) ? 1 : 0;

        // 更新符号标志（SF）
        SF = (valE < 0) ? 1 : 0;

        // 初始化溢出标志（OF）
        OF = 0;

        long long result;

        if (icode == 6) { // OPq 指令
            switch (ifun) {
                case 0: // addq
                    // 对于加法，当两个操作数符号相同，但结果符号不同，发生溢出
                    if ((valA < 0 == valB < 0) && (valE < 0 != valA < 0)) {
                        OF = 1;
                    }
                    break;
                case 1: // subq
                    // 对于减法，当操作数符号相异，且结果符号与被减数符号相反时，发生溢出
                    if ((valB < 0 != valA < 0) && (valE < 0 != valB < 0)) {
                        OF = 1;
                    }
                    break;
                case 2: // andq
                case 3: // xorq
                    // 对于逻辑操作，溢出标志总是清零
                    OF = 0;
                    break;
                case 4: // mulq
                    if (valE > INT64_MAX || valE < INT64_MIN) {
                        OF = 1;
                    }
                    break;
                case 5: // divq 溢出在执行阶段已处理，此处无需设置 OF，只需确保 ZF 和 SF 已根据结果设置
                    break;
                case 6: // remq
                    break;
                default:
                    // 无效的功能码，不更新 OF
                    break;
            }
        }
}

    bool compute_cnd(uint8_t ifun) {
    switch (ifun) {
        case 1:  // <= le
            return (SF ^ OF) || ZF;
        case 2:   // < l
            return SF ^ OF;
        case 3:   // == e
            return ZF;
        case 4:  // != ne
            return !ZF;
        case 5:  // >= ge
            return !(SF ^ OF);
        case 6:   // > g
            return !(SF ^ OF) && !ZF;
        default:   // 无效指令
            return false;
    }
}
};

// cache的结构与类 
typedef struct {
    int valid_bits;
    int tag;
    int stamp; // 用于LRU
} cache_line, *cache_asso, **cache;
typedef struct {
    cache_line** lines;       // LRU 时使用
} cache_struct;
class Cache {
public:
    int s = 0; // 组数
    int E = 0; // 每组的行数
    int b = 0; // 块大小(字节数)

    cache_struct _cache_;

    // 性能计数器
    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;

    // 全局时间戳，用于LRU
    int current_stamp = 0;

    // 指向内存的指针
    Memory* memory_ptr;

    // 构造函数
    Cache(int sets = 2, int Es = 4, int block = 16, Memory* mem = nullptr) : s(sets), E(Es), b(block), memory_ptr(mem) {
        // 分配组的内存
        _cache_.lines = new cache_line*[s];
        for(int i = 0; i < s; i++) {
            _cache_.lines[i] = new cache_line[E];
            for(int j = 0; j < E; j++) {
                _cache_.lines[i][j].valid_bits = 0;
                _cache_.lines[i][j].tag = 0;
                _cache_.lines[i][j].stamp = 0;
            }
        }
    }

    // 析构函数
    ~Cache() {
        for(int i = 0; i < s; i++) {
            delete[] _cache_.lines[i];
        }
        delete[] _cache_.lines;
    }

    // 更新cache的函数
    void update(long long address)
    {
        // 计算组索引和标签
        int block_num = address / b;
        int setindex_add = block_num % s; // 组索引
        int tag_add = block_num / s; // 标签
        if(setindex_add > s - 1){
            tag_add += E*(setindex_add - s);
            setindex_add = setindex_add % s; // 如果组索引大于S-1，则标签需要增加E*(setindex_add - S)
        }

        int max_stamp = -2147483648; // INT_MIN
        int max_stamp_index = -1; 

        // 检查是否命中，逐行检查每个组里的行
        for(int i = 0; i < E; i++)
        {
            if(_cache_.lines[setindex_add][i].tag == tag_add && _cache_.lines[setindex_add][i].valid_bits)
            {
                _cache_.lines[setindex_add][i].stamp = 0; // 重置时间戳
                hit_count++;
                return;
            }
        }
        
        // 如果未命中，查找是否有空行
        for(int i = 0; i < E; i++)
        {
            if(_cache_.lines[setindex_add][i].valid_bits == 0)
            {
                _cache_.lines[setindex_add][i].valid_bits = 1;
                _cache_.lines[setindex_add][i].tag = tag_add;
                _cache_.lines[setindex_add][i].stamp = 0;
                miss_count++;
                return ;
            }
        }

        // 没有空行又没有hit就是要替换了
        miss_count++; // 未命中次数加1，前面两步都会return，不用担心重复加
        eviction_count++; // 替换次数加1
        // 找到 LRU 行（最大的 stamp）
        for(int i = 0; i < E; ++i)
        {
            if(_cache_.lines[setindex_add][i].stamp > max_stamp)
            {
                max_stamp = _cache_.lines[setindex_add][i].stamp;
                max_stamp_index = i;
            }
            /*printf("max_stamp_index %d ", max_stamp_index); // 调试
            printf("max_stamp %d \n", max_stamp);*/ // 调试
        }
        _cache_.lines[setindex_add][max_stamp_index].tag = tag_add;
        _cache_.lines[setindex_add][max_stamp_index].stamp = 0;
        return ;
    }

    // 获取缓存命中率
    double get_hit_rate() const {
        if(hit_count + miss_count == 0) return 0.0;
        return (static_cast<double>(hit_count) / (hit_count + miss_count)) * 100.0;
    }

    // 获取缓存替换率
    double get_eviction_rate() const {
        if(hit_count + miss_count == 0) return 0.0;
        return (static_cast<double>(eviction_count) / (hit_count + miss_count)) * 100.0;
    }
};

// 写入output
void write_state_to_yaml(std::ofstream& outfile, const CPUState& cpu, const Memory& mem) {
    outfile << "- PC: " << cpu.PC << "\n";
    outfile << "  REG:\n";
    outfile << "    rax: " << cpu.regs[0] << "\n";
    outfile << "    rcx: " << cpu.regs[1] << "\n";
    outfile << "    rdx: " << cpu.regs[2] << "\n";
    outfile << "    rbx: " << cpu.regs[3] << "\n";
    outfile << "    rsp: " << cpu.regs[4] << "\n";
    outfile << "    rbp: " << cpu.regs[5] << "\n";
    outfile << "    rsi: " << cpu.regs[6] << "\n";
    outfile << "    rdi: " << cpu.regs[7] << "\n";
    outfile << "    r8: " << cpu.regs[8] << "\n";
    outfile << "    r9: " << cpu.regs[9] << "\n";
    outfile << "    r10: " << cpu.regs[10] << "\n";
    outfile << "    r11: " << cpu.regs[11] << "\n";
    outfile << "    r12: " << cpu.regs[12] << "\n";
    outfile << "    r13: " << cpu.regs[13] << "\n";
    outfile << "    r14: " << cpu.regs[14] << "\n";
    
    outfile << "  MEM:\n";
    for (int i = 0; i < MEM_SIZE; i += 8) {
        long long mem_val = 0;
        for (int j = 7; j >= 0; j--) {
            mem_val = (mem_val << 8) + mem.read(i + j);
        }
        if (mem_val != 0) {
            outfile << "    " << i << ": " << mem_val << "\n";
        }
    }

    outfile << "  CC:\n";
    outfile << "    ZF: " << cpu.ZF << "\n";
    outfile << "    SF: " << cpu.SF << "\n";
    outfile << "    OF: " << cpu.OF << "\n";
    outfile << "  STAT: " << cpu.Stat << "\n";
}

// 指令
void fetch(int* instruction_length, Memory& mem, int* icode, int* ifun, int* reg1, int* reg2, long long* valP, long long* valC, CPUState& cpu) {
    *icode = (mem.read(cpu.PC) >> 4);
    *ifun = (mem.read(cpu.PC) & 0xf);

    switch (*icode) {
        case 0: // halt
            cpu.Stat = 2; 
            *instruction_length = 1; 
            *valP = cpu.PC + *instruction_length;
            break;  
        case 1:  // nop
            *instruction_length = 1;
            *valP = cpu.PC + *instruction_length;
            break;  
        case 2:   // rrmovq or cmovXX
            *instruction_length = 2; 
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            *reg2 = (mem.read(cpu.PC + 1) & 0xf);
            *valP = cpu.PC + *instruction_length;
            break;
        case 3: // irmovq
            // reg1: F
            *reg2 = (mem.read(cpu.PC + 1) & 0xf);
            *valC = 0;
            for (int i = 7; i >= 0; i--) {
                *valC = (*valC << 8) + mem.read(cpu.PC + 2 + i);  
            } 
            *instruction_length = 10;
            *valP = cpu.PC + *instruction_length;
            break; 
        case 4: // rmmovq
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            *reg2 = (mem.read(cpu.PC + 1) & 0xf);
            *valC = 0;
            for (int i = 7; i >= 0; i--) {
                *valC = (*valC << 8) + mem.read(cpu.PC + 2 + i);  
            } 
            *instruction_length = 10; 
            *valP = cpu.PC + *instruction_length;
            break; 
        case 5: // mrmovq
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            *reg2 = (mem.read(cpu.PC + 1) & 0xf);
            *valC = 0;
            for (int i = 7; i >= 0; i--) {
                *valC = (*valC << 8) + mem.read(cpu.PC + 2 + i);  
            } 
            *instruction_length = 10; 
            *valP = cpu.PC + *instruction_length;
            break; 
        case 6: // OPq
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            *reg2 = (mem.read(cpu.PC + 1) & 0xf);
            *instruction_length = 2; 
            *valP = cpu.PC + *instruction_length;
            break;  
        case 7: // jXX
            *valC = 0;
            for (int i = 7; i >= 0; i--) {
                *valC = (*valC << 8) + mem.read(cpu.PC + 1 + i);  
            } 
            *instruction_length = 9;
            *valP = cpu.PC + *instruction_length;
            break;  
        case 8: // call
            *valC = 0;
            for (int i = 7; i >= 0; i--) {
                *valC = (*valC << 8) + mem.read(cpu.PC + 1 + i);  
            } 
            *instruction_length = 9; 
            *valP = cpu.PC + *instruction_length;
            break;  
        case 9: // ret
            *instruction_length = 1; 
            *valP = cpu.PC + *instruction_length;
            break;  
        case 10: // pushq
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            // *reg2: F
            *instruction_length = 2;
            *valP = cpu.PC + *instruction_length;
            break; 
        case 11: // popq
            *reg1 = (mem.read(cpu.PC + 1) >> 4);
            // *reg2: F
            *instruction_length = 2; 
            *valP = cpu.PC + *instruction_length;
            break; 
        default: 
            cpu.Stat = 3; // 假设 3 表示 'AINS'（非法指令）
            *instruction_length = 1; // 跳过一个字节以避免无限循环
            break;
    }
}

// 解码阶段
void decode(long long* valA, long long* valB, int icode, int reg1, int reg2, CPUState& cpu) {
    *valA = 0;
    *valB = 0;
    switch (icode) {
        case 0: // halt
            cpu.Stat = 2; // 2 表示 'HLT'（停止）
            break;
        case 1: break;  // nop
        case 2:   // rrmovq or cmovXX
            *valA = cpu.regs[reg1];
            break;
        case 3: // irmovq
            break; 
        case 4: // rmmovq
            *valA = cpu.regs[reg1];
            *valB = cpu.regs[reg2];
            break; 
        case 5: // mrmovq
            *valB = cpu.regs[reg2];
            break; 
        case 6: // OPq
            *valA = cpu.regs[reg1];
            *valB = cpu.regs[reg2];
            break;  
        case 7: // jXX
            break;  
        case 8: // call
            *valB = cpu.regs[4]; // %rsp
            break;  
        case 9: // ret
            *valA = cpu.regs[4]; // %rsp
            *valB = cpu.regs[4]; // %rsp
            if (*valA < 0 || *valA >= MEM_SIZE) cpu.Stat = 3; // ret将基于valA访存
            break;  
        case 10: // pushq
            *valA = cpu.regs[reg1];
            *valB = cpu.regs[4]; // %rsp
            break; 
        case 11: // popq
            *valA = cpu.regs[4]; // %rsp
            *valB = cpu.regs[4]; // %rsp
            if (*valA < 0 || *valA >= MEM_SIZE) cpu.Stat = 3; // popq将基于valA访存
            break; 
        default: 
            cpu.Stat = 3; // 假设 3 表示 'AINS'（非法指令）
            break;
    }
}

// 执行阶段
void execute(long long* valE, long long valA, long long valB, long long valC, int icode, int ifun, CPUState& cpu) {
    switch (icode) {
        case 0: // halt
            cpu.Stat = 2; // 2 表示 'HLT'（停止）
            break;
        case 1: break;  // nop
        case 2:   // rrmovq or cmovXX
            *valE = valA;
            break; 
        case 3: // irmovq
            *valE = valC;
            break; 
        case 4: // rmmovq
            *valE = valB + valC;
            if (*valE < 0 || *valE >= MEM_SIZE) cpu.Stat = 3; // rmmovq和mrmovq都将基于valE访存
            break; 
        case 5: // mrmovq
            *valE = valB + valC;
            if (*valE < 0 || *valE >= MEM_SIZE) cpu.Stat = 3;
            break;
        case 6: // OPq
            switch(ifun){
                case 0: // addq
                    *valE = valA + valB;
                    cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    break;  
                case 1: // subq
                    *valE = valB - valA;
                    cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    break;  
                case 2: // andq
                    *valE = valA & valB;
                    cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    break;
                case 3: // xorq
                    *valE = valA ^ valB;
                    cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    break;
                case 4: // mulq
                    *valE = valA * valB;
                    cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    break;
                case 5: // divq
                    if (valB == 0) { // 除数为零
                        cpu.OF = 1; // 设置溢出标志
                        cpu.Stat = 3; // 设置状态为非法指令或错误状态
                    } else {
                        *valE = valB / valA; // 商
                        cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    }
                    break;
                case 6: // remq
                    if (valB == 0) { // 除数为零
                        cpu.OF = 1; // 设置溢出标志
                        cpu.Stat = 3; // 设置状态为非法指令或错误状态
                    } else {
                        *valE = valB % valA; // 余数
                        cpu.update_condition_codes(valA, valB, *valE, icode, ifun);
                    }
                    break;
                default: break;  // 错误指令
            }
            break;  
        case 7: break; // jXX
        case 8: // call
            *valE = valB - 8;
            break;  
        case 9: // ret
            *valE = valB + 8;
            break;  
        case 10: // pushq
            *valE = valB - 8;
            if (*valE < 0 || *valE >= MEM_SIZE) cpu.Stat = 3;
            break; 
        case 11: // popq
            *valE = valB + 8;
            break; 
        default: 
            cpu.Stat = 3; // 假设 3 表示 'AINS'（非法指令）
            break;
    }
}

// 访存阶段
void memory(long long valA, long long valE, long long valP, long long* valM, int icode, CPUState& cpu, Memory& mem) {
    if(cpu.Stat == 1){ // 只有Stat为1才更新状态
        switch (icode) {
            case 0: break;  // halt
            case 1: break;  // nop
            case 2: break;  // rrmovq or cmovXX
            case 3: break;  // irmovq
            case 4: // rmmovq
                for (int i = 0; i < 8; i++) {
                    // 从低位开始，每次取一个byte，存入内存中valE + i的位置
                    char data = (valA >> (i * 8)) & 0xFF;
                    mem.write(valE + i, data);
                }
                break; 
            case 5: // mrmovq
                for (int i = 7; i >= 0; i--) {
                    *valM = (*valM << 8) + mem.read(valE + i); 
                }
                break;
            case 6: break;  // OPq
            case 7: break;  // jXX
            case 8: // call
                for (int i = 0; i < 8; i++) {
                    char data = (valP >> (i * 8)) & 0xFF;
                    mem.write(valE + i, data);
                }
                break;  
            case 9: // ret
                for (int i = 7; i >= 0; i--) {
                    *valM = (*valM << 8) + mem.read(valA + i); 
                }
                break;  
            case 10: // pushq
                for (int i = 0; i < 8; i++) {
                    char data = (valA >> (i * 8)) & 0xFF;
                    mem.write(valE + i, data);
                }
                break; 
            case 11: // popq
                for (int i = 7; i >= 0; i--) {
                    *valM = (*valM << 8) + mem.read(valA + i); 
                }
                break; 
            default: 
                cpu.Stat = 3; // 假设 3 表示 'AINS'（非法指令）
                break;
        }
    }
}

// 写回阶段
void write_back(long long valE, long long valM, int reg1, int reg2, int icode, int ifun, CPUState& cpu) {
    if(cpu.Stat == 1){ // 只有Stat为1才更新状态
        switch (icode) {
            case 0: break;  // halt
            case 1: break;  // nop
            case 2: // rrmovq or cmovXX
                if(!ifun){ // rrmovq
                    cpu.regs[reg2] = valE;
                }
                else{ // cmovXX
                    if(cpu.compute_cnd(ifun)){
                        cpu.regs[reg2] = valE;
                    }
                }
                break;  
            case 3: // irmovq
                cpu.regs[reg2] = valE;
                break;  
            case 4: break;  // rmmovq
            case 5: // mrmovq
                cpu.regs[reg1] = valM;
                break;
            case 6: // OPq
                cpu.regs[reg2] = valE;
                break;  
            case 7: break;  // jXX
            case 8: // call
                cpu.regs[4] = valE;
                break;  
            case 9: // ret
                cpu.regs[4] = valE;
                break;  
            case 10: // pushq
                cpu.regs[4] = valE;
                break; 
            case 11: // popq
                cpu.regs[4] = valE;
                cpu.regs[reg1] = valM;
                break; 
            default: 
                cpu.Stat = 3; // 假设 3 表示 'AINS'（非法指令）
                break;
        }
    }
}

// 更新PC
void update_PC(long long valP, long long valC, long long valM, int icode, int ifun, CPUState &cpu){
    if(cpu.Stat == 1){ // 只有Stat为1才更新状态
        switch(icode){
            case 7: // jXX
                if(!ifun){
                    cpu.PC = valC;
                }
                else if(ifun && cpu.compute_cnd(ifun)){
                    cpu.PC = valC;
                }
                else{
                    cpu.PC = valP;
                }
                break;
            case 8: // call
                cpu.PC = valC;
                break;
            case 9: // ret
                cpu.PC = valM;
                break;
            default: // 其他情况
                cpu.PC = valP;
        }
    }
}

int main() {
    // 文件路径变量
    // ####################################################
    const std::string machine_code_path = "../test/abs-asum-cmov.yo";
    const std::string output_yaml_path = "../output/abs-asum-cmov.yml";
    const std::string performance_report_path = "../performance/abs-asum-cmov.txt";
    // ####################################################

    // 创建 CPU, Memory, Cache 对象
    CPUState cpu;
    Memory M(MEM_SIZE);
    Cache cache;

    // 从文件加载机器码
    if (!M.load_from_file(machine_code_path)) {
        return -1;
    }
    
    int reg1, reg2; // 用于存储寄存器号
    // 打开输出文件
    std::ofstream outfile(output_yaml_path);
    
    if (!outfile.is_open()) {
        std::cerr << "Failed to open output file: output/abs-asum-cmov.yml" << std::endl;
        return -1;
    }

    // 执行 CPU (fetch-decode-execute 循环)
    while (cpu.PC < MEM_SIZE && cpu.Stat == 1) {
        int instruction_length;
        long long valP = 0;
        long long valA = 0;
        long long valB = 0;
        long long valE = 0;
        long long valC = 0;
        long long valM = 0;
        int icode, ifun;

        fetch(&instruction_length, M, &icode, &ifun, &reg1, &reg2, &valP, &valC, cpu);
        decode(&valA, &valB, icode, reg1, reg2, cpu);
        execute(&valE, valA, valB, valC, icode, ifun, cpu);
        memory(valA, valE, valP, &valM, icode, cpu, M);
        write_back(valE, valM, reg1, reg2, icode, ifun, cpu);
        update_PC(valP, valC, valM, icode, ifun, cpu);

        cache.update(cpu.PC);

        // 根据指令类型增加特定计数
        if (icode == 6) { // OPq
            switch(ifun){
                case 0: cpu.addq_count++; break;
                case 1: cpu.subq_count++; break;
                case 2: cpu.andq_count++; break;
                case 3: cpu.xorq_count++; break;
                case 4: cpu.mulq_count++; break;
                case 5: cpu.divq_count++; break;
                case 6: cpu.remq_count++; break;
                default: break;
            }
        }
        cpu.instruction_count++;

        // 将当前 CPU 状态写入 YAML 文件
        write_state_to_yaml(outfile, cpu, M);

        // 调试输出************************************
        /*std::cout << "- PC: " << cpu.PC << "\n";
        std::cout << "  REG:\n";
        std::cout << "    rax: " << cpu.regs[0] << "\n";
        std::cout << "    rcx: " << cpu.regs[1] << "\n";
        std::cout << "    rdx: " << cpu.regs[2] << "\n";
        std::cout << "    rbx: " << cpu.regs[3] << "\n";
        std::cout << "    rsp: " << cpu.regs[4] << "\n";
        std::cout << "    rbp: " << cpu.regs[5] << "\n";
        std::cout << "    rsi: " << cpu.regs[6] << "\n";
        std::cout << "    rdi: " << cpu.regs[7] << "\n";
        std::cout << "    r8: " << cpu.regs[8] << "\n";
        std::cout << "    r9: " << cpu.regs[9] << "\n";
        std::cout << "    r10: " << cpu.regs[10] << "\n";
        std::cout << "    r11: " << cpu.regs[11] << "\n";
        std::cout << "    r12: " << cpu.regs[12] << "\n";
        std::cout << "    r13: " << cpu.regs[13] << "\n";
        std::cout << "    r14: " << cpu.regs[14] << "\n";
        
        std::cout << "  MEM:\n";
        for (int i = 0; i < MEM_SIZE; i += 8) {
            long long mem_val = 0;
            for (int j = 7; j >= 0; j--) {
                mem_val = (mem_val << 8) + M.read(i + j);
            }
            if (mem_val != 0) {
                std::cout << "    " << i << ": " << mem_val << "\n";
            }
        }

        std::cout << "  CC:\n";
        std::cout << "    ZF: " << cpu.ZF << "\n";
        std::cout << "    SF: " << cpu.SF << "\n";
        std::cout << "    OF: " << cpu.OF << "\n";
        std::cout << "  STAT: " << cpu.Stat << "\n";*/
        // 调试输出************************************

        // 检查是否遇到错误状态
        if (cpu.Stat != 1) {
            std::cerr << "CPU stopped with status: " << cpu.Stat << std::endl;
            break;
        }

        // 调试，每次循环停止一小段时间
        /*std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 暂停 500 毫秒 */
    }

    outfile.close();

    // 输出性能报告
    std::ofstream perf_report(performance_report_path);
    if (perf_report.is_open()) {
        perf_report << "Performance Report\n";
        perf_report << "==================\n";
        perf_report << "Total Instructions Executed: " << cpu.instruction_count << "\n";
        perf_report << "addq Instructions Executed: " << cpu.addq_count << "\n";
        perf_report << "subq Instructions Executed: " << cpu.subq_count << "\n";
        perf_report << "andq Instructions Executed: " << cpu.andq_count << "\n";
        perf_report << "xorq Instructions Executed: " << cpu.xorq_count << "\n";
        perf_report << "mulq Instructions Executed: " << cpu.mulq_count << "\n";
        perf_report << "divq Instructions Executed: " << cpu.divq_count << "\n";
        perf_report << "remq Instructions Executed: " << cpu.remq_count << "\n";
        perf_report << "Cache Hits: " << cache.hit_count << "\n";
        perf_report << "Cache Misses: " << cache.miss_count << "\n";
        perf_report << "Cache Evictions: " << cache.eviction_count << "\n"; // **
        perf_report << "Cache Hit Rate: " << cache.get_hit_rate() << "\n";
        perf_report << "Cache Eviction Rate: " << cache.get_eviction_rate() << "\n";
        // 添加更多性能指标
        perf_report.close();
    } else {
        std::cerr << "Failed to open performance report file." << std::endl;
    }
    return 0;
}