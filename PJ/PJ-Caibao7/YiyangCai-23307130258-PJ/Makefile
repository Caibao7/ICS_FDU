# 指定编译器
CC     := g++

# 编译选项
CFLAGS := -O2

# 源文件
SRCS   := y86cpu.cpp

# 目标可执行文件名称
TARGET := y86cpu

.PHONY: all clean

# 默认目标：生成可执行文件
all: $(TARGET)

# 生成可执行文件的规则
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

# 清理目标：删除可执行文件等
clean:
	rm -f $(TARGET)
