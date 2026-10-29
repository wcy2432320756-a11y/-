// ============================================================
// 第 14.1 课 · 章末练习：给菜单加一个"按书名查找"
// ------------------------------------------------------------
// 菜单已升级为 5 个功能，5 号"按书名查找"留给你填（标【空x】）。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "Book.h"
using namespace std;

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
        if (!b.isAvailable()) { cout << "《" << b.getTitle() << "》已被借出" << endl; return false; }
        if (borrowed >= maxBorrow) { cout << name << " 借阅已满" << endl; return false; }
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
};

int main()
{
    SetConsoleOutputCP(936);

    vector<Book> books;
    books.push_back(Book("三体", "9787536692930", 23.0, 302));
    books.push_back(Book("C++ Primer", "9787115408105", 128.0, 748));

    User me("张三", 3);

    int choice = -1;
    while (choice != 0)
    {
        cout << "======== 图书馆借阅系统 ========" << endl;
        cout << "1.显示全部  2.借书  3.还书  4.添加图书  5.按书名查找  0.退出" << endl;
        cout << "请选择：";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "---- 全部图书 ----" << endl;
            for (int i = 0; i < (int)books.size(); i++)
            {
                books[i].showInfo();
            }
            break;

        case 2:
        {
            string name;
            cout << "请输入要借的书名：";
            cin >> name;
            bool found = false;
            for (int i = 0; i < (int)books.size(); i++)
            {
                if (books[i].getTitle() == name) { found = true; me.borrowBook(books[i]); }
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
                if (books[i].getTitle() == name) { found = true; me.returnBook(books[i]); }
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

        case 5:
        {
            string name;
            cout << "请输入要查找的书名：";
            cin >> name;

            // 【空1】声明一个标记变量 found，初始值 false（参考 case 2 的写法）
            // 【空2】用 for 循环遍历 books：
            //         如果 books[i] 的标题 == name，就 found = true; 并调用 books[i].showInfo();
            // 【空3】循环结束之后：如果 !found，打印 "没有这本书"
            bool found = false;
            for (int i = 0;i < (int)books.size();i++)
            {
                if (books[i].getTitle() == name)
                {
                    found = true;
                    books[i].showInfo();
                    break;
                }
            }
            if(!found) cout << "没有这本书" << endl;
        }
        break;

        case 0:
            cout << "再见！" << endl;
            break;

        default:
            cout << "无效选择，请重新输入" << endl;
            break;
        }
    }

    return 0;
}
