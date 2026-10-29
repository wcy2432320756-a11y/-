// ============================================================
// 第 06 课 · 数据上锁：private + setter/getter
// ------------------------------------------------------------
// 上一课的成员全是 public：外面想怎么改就怎么改。
// 这一课给数据上锁（private），只在类里留几扇门（成员函数）：
// 想读写？走门，门卫说了算。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================

#include <iostream>
#include <string>
using namespace std;

class Book
{
private:                    // ① private：下面的成员，只有类自己碰得到
    string title;
    double price;
    int    pages;

public:                     // ② public：对外的几扇门
    Book(string t, double p, int pg)
    {
        title = t;
        price = p;
        pages = pg;
    }

    void setPrice(double p) // ③ 写门（setter）：带检查，非法值拒收
    {
        if (p > 0)
        {
            price = p;
        }
        else
        {
            cout << "你的输入被拒收，价格必须是正数。" << "你的输入："<<p << endl;
        }
    }

    double getPrice() { return price; }   // ④ 读门（getter）
    string getTitle() { return title; }

    void showInfo()
    {
        cout << "《" << title << "》, " << price << " 元, " << pages << " 页" << endl;
    }
};

int main()
{
    Book b("三体", 39.9, 302);
    b.showInfo();

    // ⑤ 走门改价：合法的，进得去
    b.setPrice(35.0);
    b.showInfo();

    // ⑥ 走门改价：非法的，被门卫拒收（数据没变）
    b.setPrice(-10);
    b.showInfo();

    // ⑦ 读数据也要走门
    cout << "书名：" << b.getTitle() << "，现价：" << b.getPrice() << " 元" << endl;

    // ⑧ 硬闯？把下面这行的 // 去掉试试：编译直接报 C2248
    // b.price = 1.0;

    return 0;
}