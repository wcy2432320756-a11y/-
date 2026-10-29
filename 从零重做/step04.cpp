// ============================================================
// 第 04 课 · 第一个类：图书卡片
// ------------------------------------------------------------
// 为什么先学它：接下来的所有内容（继承、多态、容器）都围着"类"转，
// 而且作业实验一要求的就是一个"图书类"。不会类，后面寸步难行。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================

#include <iostream>
#include <string>
using namespace std;

// ① class = 我们自己造的一个"新类型"，用法和 int、string 一样
class Book
{
public:                 // ② public = "对外可见"。为什么要有这行，第 6 课揭晓
    // ③ 成员变量：这张卡片要记住的三样东西
    string title;
    double price;
    int    pages;

    // ④ 成员函数：属于这张卡片的动作（自我介绍）
    void showInfo()
    {
        cout << "《" << title << "》 " << price << " 元, " << pages << " 页" << endl;
    }
};

int main()
{
    Book b1;                 // ⑤ 用 Book 造一个对象：像 int n; 一样自然
    b1.title = "三体";       // ⑥ 用小数点 . 访问对象里的成员
    b1.price = 39.9;
    b1.pages = 302;

    Book b2;                 // 同一个类，可以造无数个对象
    b2.title = "C++ Primer";
    b2.price = 128.0;
    b2.pages = 976;

    b1.showInfo();           // ⑦ 调用成员函数：对象自己的动作
    b2.showInfo();

    return 0;
}