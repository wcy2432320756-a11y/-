// ============================================================
// 第 14 课 · 菜单系统：把前面所有课串成一个"能操作"的程序
// ------------------------------------------------------------
// 运行后显示菜单，输入数字选择功能：
//   1 显示全部图书   2 借书   3 还书   4 添加图书   0 退出
// 用到：vector（第 8 课）+ Book/User（第 7、13 课）+ cin 输入（第 2 课）
// 新东西：while (choice != 0) 主循环 + switch 分派
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "Book.h"
using namespace std;

// ---- 用户（第 13 课简化版：只要学生）----
class User {
protected:
    string name;
    int borrowed;
    int maxBorrow;

public:
    User(string n, int max)
    {
        name = n;
        borrowed = 0;
        maxBorrow = max;
    }

    bool borrowBook(Book& b)
    {
        if (!b.isAvailable())
        {
            cout << "《" << b.getTitle() << "》已被借出" << endl;
            return false;
        }
        if (borrowed >= maxBorrow)
        {
            cout << name << " 借阅已满（" << maxBorrow << " 本）" << endl;
            return false;
        }
        b.borrowOne();
        borrowed++;
        cout << "借阅成功：《" << b.getTitle() << "》" << endl;
        return true;
    }

    void returnBook(Book& b)
    {
        b.returnOne();
        if (borrowed > 0) borrowed--;
        cout << "归还成功：《" << b.getTitle() << "》" << endl;
    }

    void show()
    {
        cout << name << " 已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
    }
};

int main()
{
    SetConsoleOutputCP(936);

    vector<Book> books;
    books.push_back(Book("三体", "9787536692930", 23.0, 302));
    books.push_back(Book("C++ Primer", "9787115408105", 128.0, 748));

    User me("张三", 3);

    int choice = -1;
    while (choice != 0)          // 主循环：只要没选 0 就一直问
    {
        cout << "======== 图书馆借阅系统 ========" << endl;
        cout << "1.显示全部  2.借书  3.还书  4.添加图书  0.退出" << endl;
        cout << "请选择：";
        cin >> choice;

        switch (choice)          // 按输入的数字分派
        {
        case 1:
            cout << "---- 全部图书 ----" << endl;
            for (int i = 0; i < (int)books.size(); i++)
            {
                books[i].showInfo();
            }
            break;               // 每个 case 末尾必须 break

        case 2:
        {
            string name;
            cout << "请输入要借的书名：";
            cin >> name;
            bool found = false;
            for (int i = 0; i < (int)books.size(); i++)
            {
                if (books[i].getTitle() == name)
                {
                    found = true;
                    me.borrowBook(books[i]);
                }
            }
            if (!found) cout << "没有这本书" << endl;
        }
        break;

        case 3:
        {
            string name;
            cout << "请输入要还的书名：";
            cin >> name;
            bool found = false;
            for (int i = 0; i < (int)books.size(); i++)
            {
                if (books[i].getTitle() == name)
                {
                    found = true;
                    me.returnBook(books[i]);
                }
            }
            if (!found) cout << "没有这本书" << endl;
        }
        break;

        case 4:
        {
            string name, isbn;
            double price;
            int pages;
            cout << "请输入 书名 ISBN 价格 页数：";
            cin >> name >> isbn >> price >> pages;
            books.push_back(Book(name, isbn, price, pages));
            cout << "已添加：《" << name << "》" << endl;
        }
        break;

        case 0:
            cout << "再见！" << endl;
            break;

        default:                 // 其余情况（相当于最后的 else）
            cout << "无效选择，请重新输入" << endl;
            break;
        }
    }

    return 0;
}
