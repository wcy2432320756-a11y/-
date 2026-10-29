// ============================================================
// 第 5.1 课 · 练习：把构造函数补齐（R 的答案版，仅供备课验证）
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

    double totalValue(int count)
    {
        return price * count;
    }
};

int main()
{
    Book b("解忧杂货店", 39.5, 320);

    b.showInfo();
    cout << "3 本总价：" << b.totalValue(3) << " 元" << endl;

    return 0;
}