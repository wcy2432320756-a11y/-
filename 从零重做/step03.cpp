// ============================================================
// 第 03 课 · 函数：把一段代码打包，起个名字
// ------------------------------------------------------------
// 为什么先学这个：类 = 数据 + 函数。学类之前，先把函数搞明白。
// 编译运行：Dev-C++ 打开后按 F11；或在 VS Code 里按 Ctrl+Shift+B。
// ============================================================

#include <iostream>
#include <string>
using namespace std;

// ① 函数的样子：  返回类型  名字(参数表)  { 要做的事 }

// 没有参数、不返回东西（void）：调用它就打印一条分隔线
void printLine()
{
    cout << "--------------------------------" << endl;
}

// ② 带参数的函数：调用时把外面的值"拷贝"进括号里的变量
void printBook(string title, double price)
{
    cout << title << "：" << price << " 元" << endl;
}

// ③ 带返回值的函数：把算好的结果交还给调用者
double totalPrice(double price, int count)
{
    return price * count;
}

int main()
{
    printLine();
    printBook("三体", 39.9);          // 调用函数 = 名字 + 括号（+ 参数）
    printBook("C++ Primer", 128.0);   // 同一个函数用两次，这就是"打包"的意义
    printLine();

    double total = totalPrice(39.9, 3);   // 结果存进变量，想用就用
    cout << "3 本《三体》总价：" << total << " 元" << endl;

    return 0;
}