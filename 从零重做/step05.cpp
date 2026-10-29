// ============================================================
// 第 05 课 · 构造函数：对象一出生就带齐数据
// ------------------------------------------------------------
// 上一课造对象要手工填三格（b1.title=...; b1.price=...; b1.pages=...），
// 麻烦，而且漏填一格就是"残缺的书"。构造函数让这一步自动化：
// 造对象时直接把数据递进去，出生即完整。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================

#include <iostream>
#include <string>
using namespace std;

class Book
{
public:
    string title;
    double price;
    int    pages;

    // ① 构造函数：名字和类名一模一样、没有返回类型（连 void 都不写）
    //    只要造对象，它就自动执行，把括号里的值装进成员
    Book();                 // ← 这是"不带参数"的构造函
    Book(string t, double p, int pg)
    {
        title = t;
        price = p;
        pages = pg;
    }

    void showInfo()
    {
        cout << "《" << title << "》 " << price << " 元, " << pages << " 页" << endl;
    }
};

int main()
{
    // ② 造对象 = 调用构造函数，参数当场给齐：一行顶上一课六行
    Book b1("三体", 39.9, 302);
    Book b2("C++ Primer", 128.0, 976);

    b1.showInfo();
    b2.showInfo();

    // ③ 练手坑：把下面这行的 // 去掉（让 Book b3; 生效），看编译器怎么骂
    //Book b3;                // ← 空着造：报错，因为"不带参数"的构造不存在

    return 0;
}