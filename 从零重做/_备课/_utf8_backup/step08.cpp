// ============================================================
// 第 08 课 · 一堆对象：数组与 vector
// ------------------------------------------------------------
// 之前每课只有一两个对象；图书馆有一堆书，得装起来。
// 两种装法：数组（固定大小）和 vector（想加多少加多少）。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <vector>          // ① vector 住在这里
#include <windows.h>
#include "Book.h"
using namespace std;

int main()
{
    SetConsoleOutputCP(936);

    // ② 数组：大小写死 3。元素靠默认构造函数出生（Book.h 里新增的 Book()）
    Book shelf[3];
    shelf[0] = Book("三体", "9787536692930", 39.9, 302);      // ③ 对象可以整体赋值
    shelf[1] = Book("活着", "9787506365437", 45.0, 191);
    shelf[2] = Book("解忧杂货店", "9787544270878", 39.5, 320);

    cout << "== 书架（数组）==" << endl;
    for (int i = 0; i < 3; i++)
    {
        shelf[i].showInfo();
    }

    // ④ vector：想加就加，数量不用提前说
    vector<Book> books;
    books.push_back(Book("三体", "9787536692930", 39.9, 302));
    books.push_back(Book("活着", "9787506365437", 45.0, 191));
    books.push_back(Book("解忧杂货店", "9787544270878", 39.5, 320));

    cout << endl << "== 书库（vector，共 " << books.size() << " 本）==" << endl;
    for (int i = 0; i < (int)books.size(); i++)
    {
        books[i].showInfo();
    }

    // ⑤ 再塞一本，看看数量变化
    books.push_back(Book("C++ Primer", "9787115408105", 128.0, 976));
    cout << endl << "加了一本，现在 " << books.size() << " 本" << endl;

    return 0;
}
