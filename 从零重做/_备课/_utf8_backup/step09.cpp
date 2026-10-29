// ============================================================
// 第 09 课 · 组合：图书馆里装图书
// ------------------------------------------------------------
// 一个类（Library）把另一个类的对象（vector<Book>）当自己的成员——
// 这就是"整体与部分"的组合关系（has-a）。
// 作业实验二的核心之一：图书馆类组合图书类。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "Book.h"
using namespace std;

// ① Library = 整体，Book = 部分
class Library {
private:
    string name;              // 馆名
    vector<Book> books;       // ② 组合：成员是"一堆 Book"（一开始自动是空的）

public:
    Library(string n)
    {
        name = n;
    }

    void addBook(Book b)      // ③ 收一本进来
    {
        books.push_back(b);
        cout << "入库：《" << b.getTitle() << "》" << endl;
    }

    int count()               // 馆里现在几本
    {
        return (int)books.size();
    }

    void showAll()            // ④ 整体调用部分：让每本书各自自我介绍
    {
        cout << endl << "== " << name << " 共 " << count() << " 本 ==" << endl;
        for (int i = 0; i < count(); i++)
        {
            books[i].showInfo();
        }
    }
};

int main()
{
    SetConsoleOutputCP(936);

    Library lib("校图书馆");       // ⑤ 造一个图书馆（一开始是空架子）

    lib.addBook(Book("三体", "9787536692930", 39.9, 302));
    lib.addBook(Book("活着", "9787506365437", 45.0, 191));
    lib.addBook(Book("解忧杂货店", "9787544270878", 39.5, 320));

    lib.showAll();

    cout << endl << "馆里一共 " << lib.count() << " 本书。" << endl;

    return 0;
}
