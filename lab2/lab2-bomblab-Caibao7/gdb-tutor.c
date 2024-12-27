/*
 *  gdb-tutor, version 1.0
 *  by cameudis.
 * 
 *  This is a tutorial for GDB.
 *  It is designed to be used with the BombLab.
 *  ICS 2024, FDU.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <time.h>

#define breakp __asm__("int3")

int level1();
int level2();
int level3();

__uint64_t some_value = 0x2030405060708090;

int main() {
    puts("大家好啊，我是交互式GDB教程（gdb-tutor）。");
    puts("今天我们将会一起学习如何使用GDB调试程序。");
    putchar('\n');

    // ------------------------------ Chapter 0 ------------------------------

    puts("------ Chapter 0: 装载程序 ------");
    putchar('\n');

    puts("首先，我们学习如何使用GDB装载需要调试的程序。");
    puts("通过在命令行中输入 `gdb ./gdb-tutor`，就可以将我装载到GDB中。");
    putchar('\n');

    puts("在GDB启动后，你将会看到一个提示符 `(gdb)`。");
    puts("在这个提示符后，你可以输入GDB指令。");
    puts("输入 `run` 可以运行程序。");
    puts("输入 `quit` 可以退出GDB。");
    putchar('\n');

    puts("现在你需要做的是使用GDB装载我，然后输入 `run` 运行我。");
    puts("我将会检测是否被GDB装载。如果你没有使用GDB装载我，我将会直接退出。");
    putchar('\n');

    if (ptrace(PTRACE_TRACEME, 0, 1, 0) != -1) {
        puts("检测到你没有使用GDB装载我。正在退出……");
        exit(0);
    }
    puts("检测到你已经使用GDB装载我。");
    putchar('\n');

    // ------------------------------ Chapter 1 ------------------------------

    puts("------ Chapter 1: 白盒程序调试 ------");
    putchar('\n');

    puts("欢迎你来到GDB的世界！");
    puts("从现在开始，你将会学习如何使用GDB调试程序。");
    putchar('\n');

    puts("首先，让我们学习调试你自己编译的程序（也即有源代码的情况，这被称为白盒）时的一些基础操作。（BombLab为黑盒程序，没有源代码，其相关调试操作将会在这一部分之后讲解）");
    puts("注意：在编译时加入 `-g` 参数可以在可执行程序中加入调试信息，比如汇编代码与源代码间的对应关系。调试时请一定加入这个参数！（笔者大一时初学DevC++，无法调试程序就是因为没有在编译选项中加入 `-g`）");
    puts("输入 `help` 可以查看GDB的帮助信息。");
    puts("输入 `help <指令>` 可以查看某条指令的用法，例如：`help run`");
    putchar('\n');

    puts("在使用调试器调试自己写的程序时，最常用的控制流指令是 `break` 和 `continue`：");
    puts("输入 `break <行号>` 可以在某一行设置断点。简写为 `b <行号>`。");
    puts("输入 `continue` 可以让程序继续运行。简写为 `c`。");
    puts("输入 `list` 可以显示下面几条代码，此后直接回车可以继续查看更多。");
    puts("当然，你也可以直接去源代码中看行号。");
    putchar('\n');

    puts("我们将程序停在某个位置，往往是为了检查一些变量的值：");
    puts("输入 `print <变量名>` 可以查看某个变量的值。简写为 `p <变量名>`。");
    puts("输入 `info <变量名>` 可以查看某个变量的类型。简写为 `i <变量名>`。");
    putchar('\n');

    puts("接下来，我们会进入一个函数，你需要在我标注的位置下一个断点，并且查看变量 `num` 的值，并继续运行程序。");
    puts("在函数末尾，我将会询问你num的值是什么，你需要输入正确的值才能继续。");
    puts("我会给你一个下断点的机会，你可以在下号断点后输入 `continue` 继续运行程序，来运行到那个断点的位置。");
    
    level1();

    puts("你已经学会了断点，接下来我会给出一些断点的相关指令，以供参考：");
    puts("输入 `info breakpoints` 可以查看当前所有断点的信息（如断点号）。简写为 `i b`。");
    puts("输入 `delete <断点号>` 可以删除某个断点。简写为 `d <断点号>`。");
    puts("输入 `disable <断点号>` 可以禁用某个断点。简写为 `dis <断点号>`。");
    puts("输入 `enable <断点号>` 可以启用某个断点。简写为 `en <断点号>`。");
    puts("输入 `clear <行号>` 可以删除某一行的断点。简写为 `cl <行号>`。");
    puts("输入 `clear` 可以删除所有断点。");
    putchar('\n');

    puts("除了 `break` 和 `continue`外，GDB还有一些更细粒度的控制流指令。");
    puts("输入 `step` 和 `next` 可以让程序继续运行一行代码，不过他们在执行包括函数调用的语句时有所不同：");
    puts("  `step` 会让程序进入调用的函数。简写为 `s`。");
    puts("  `next` 会让程序瞬间执行完调用的函数。简写为 `n`。");
    putchar('\n');

    puts("这就衍生出一个问题：不小心用 `step` 进入了一个很长的函数，想要跳出来怎么办？（比如不小心进入了printf函数）");
    puts("输入 `finish` 可以让程序执行完当前函数。");
    puts("此外，输入 `backtrace` 可以查看当前函数调用栈。简写为 `bt`。");
    putchar('\n');

    puts("这些功能非常直观，我不会单独给你演示，你可以自己尝试。比如接下来你可以不断使用next来执行程序，一条一条打印我的话。");
    putchar('\n');

    // ------------------------------ Chapter 2 ------------------------------

    puts("------ Chapter 2: 黑盒程序调试 ------");
    putchar('\n');

    puts("在这一章节，我们将会学习如何调试黑盒程序。");
    puts("黑盒程序是指没有源代码的程序。");
    puts("在这种情况下，我们需要通过逆向程序来推测程序的行为。");
    putchar('\n');

    puts("即使是黑盒程序，我们也可以通过objdump等反汇编工具来查看其汇编代码。");
    puts("GDB也有这一功能：输入 `disassemble <函数名>` 可以查看某个函数的汇编代码。简写为 `disas <函数名>`。");
    puts("若程序没有开启PIE，那么其运行时的地址与我们在objdump中看到的地址是一致的。（比如BombLab就没有开启PIE）");
    puts("注：PIE和程序的安全性有关，在AttackLab中扮演了重要的作用，但我们的ICS没有这个Lab，所以这里只是拓展。");
    puts("（欢迎感兴趣的同学寒假参加六星CTF冬令营学习二进制漏洞的攻防技巧！）");
    putchar('\n');

    puts("下断点的方法几乎和白盒差不多：");
    puts("输入 `break *<地址>` 可以在某个地址设置断点。");
    puts("输入 `break <函数名>` 可以在某个函数的入口设置断点。");
    puts("除此以外的一些断点操作也和白盒调试一致，比如 `info b` 查看断点信息、 `d <断点号>` 删除断点等等。");
    putchar('\n');

    puts("如果说哪里有区别的话，那就是单步执行的指令。");
    puts("输入 `stepi` 可以让程序执行一条汇编指令，且会进入被调用的函数。简写为 `si`。");
    puts("输入 `nexti` 可以让程序执行一条汇编指令，且会直接快进到函数调用完成。简写为 `ni`。");
    putchar('\n');

    puts("至于查看变量的值，汇编指令中只有寄存器值和内存值的概念（局部变量的信息往往会被抹去）。");
    puts("输入 `info registers` 可以查看所有寄存器的值。简写为 `i r`。");
    putchar('\n');
    
    puts("有两个非常强大的指令用于检查值，它们是 `x` 与 `p`，分别代表eXamine和Print（后者之前介绍过）。这两个指令类似，区别在于 `x` 将输入当作指针来解引用，而 `p` 直接打印输入。");
    puts("输入 `p/x $<寄存器名>` 可以以十六进制查看某个寄存器的值。");
    puts("输入 `p/x <表达式>` 可以帮你计算一个表达式。这里的表达式使用的是C语言的语法！");
    puts("输入 `x/<格式> <地址>` 可以查看某个地址的值。");
    puts("x的完整格式由两个字母组成，分别标识类型和长度。我们可以使用 `help x` 来查看所有的格式。");
    puts("程序编译完之后，所有变量的类型信息都会被抹去，因此有时你需要自己推测变量的类型，用对应的格式进行打印。");
    puts("笔者最常用的格式是 `x/gx`，可以用十六进制查看一个八字节的值。");
    putchar('\n');

    puts("Example：本程序中有一个全局变量叫做 `some_value`，它的值为 0x2030405060708090。");
    puts("你可以尝试这几条指令，并观察他们的区别（也强烈推荐你瞎试试各种其他格式）：");
    puts("  `p/x some_value`");
    puts("  `x/gx &some_value`");
    puts("  `x/gx &some_value + 8`");
    puts("  `x/wx &some_value`");
    puts("  `x/i &some_value` 这会将some_value地址处的值识别为一条指令。很神奇吧，冯·诺依曼体系结构！");

    breakp;

    puts("接下来，我会调用一个没有提供源代码的函数，你需要使用上面教的指令来让这个函数返回true。");
    puts("不用担心失败，我会在一个循环中调用这个函数，直到你成功为止。");
    puts("提示：按 Ctrl-C 可以让GDB弹出提示符！");
    putchar('\n');

    while (!level2()) {
        ;
    }

    puts("恭喜你成功了！");
    putchar('\n');

    // ------------------------------ Chapter 3 ------------------------------

    puts("------ Chapter 3: 使用GDB进行作弊 ------");
    putchar('\n');

    puts("最后，我们再介绍一下如何使用GDB ‘作弊’。（请注意，BombLab要求的是不作弊情况下通关）");
    puts("输入 `set $<寄存器名> = <值>` 可以修改某个寄存器的值。");
    puts("输入 `set *(<类型>*)<地址> = <值>` 可以修改某个地址处的值。这里使用了C语言的语法，看不懂的可以问问AI。");
    puts("当然，不论是这里的地址，还是等号右边的值，都可以是一个表达式，比如 `$rsp+8`。");
    putchar('\n');

    puts("这有什么用呢？");
    puts("你可以跳过某些条件判断，或者修改某些变量的值，来达到你想要的效果。如果程序是一个游戏，你就可以修改某些东西了！");
    puts("在Windows平台，大家往往使用Cheat Engine（CE）来修改内存中的值，达到想要的效果。我们不做延申，欢迎感兴趣的同学自行探索。");
    puts("例子：前段时间有人发现百度云会员加速的判断逻辑位于客户端，于是就可以通过CE来绕过会员加速限制。");
    putchar('\n');

    puts("最后，再进行一个小测试：");
    puts("你的目标是让这个函数返回true。你需要修改的变量位于栈上，你需要想办法用 $rsp 来表示它的地址。");
    puts("提示：你可能需要用 `disassemble` 指令查看该函数的汇编代码。");

    while (!level3())
        ;

    puts("恭喜你，完成了 gdb-tutor！");
    puts("你已经学会了许多的GDB指令，足以帮助你完成BombLab的调试任务。");
    puts("如果你想要更深入地学习GDB，可以参考GDB的官方文档。");
    putchar('\n');

    puts("除此以外，你还可以安装一些GDB的插件，让GDB变得更加友好与强大，如 `pwndbg` 和 `peda`。去Github上搜索就可以找到，教程网上也有，我们不再赘述。");
    putchar('\n');

    puts("最后，祝你愉快地完成BombLab！");
    puts("Heart 来自助教。");
    putchar('\n');

    return 0;
}

int level1()
{
    breakp;

    int num = 0;
    int guess;

    srand(time(0));
    num = rand() % 100;

    // Breakpoint Me
    printf("请告诉我num的值是多少：");
    // Breakpoint Me

    scanf("%d", &guess);
    if (guess == num) {
        puts("恭喜你答对了！");
        putchar('\n');
    } else {
        puts("很遗憾，你失败了！重开程序再来一次吧！");
        puts("提示：你可以使用 `run` 重新运行程序。");
        exit(0);
    }

    return 0;
}

int level3()
{
    breakp;

    int auth = 0;

    if (auth) {
        return 1;
    } else {
        puts("很遗憾，你失败了，再试试吧！");
        return 0;
    }
}