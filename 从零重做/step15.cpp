// ============================================================
// 第 15 课 · 文件读写：关掉程序，数据还在
// ------------------------------------------------------------
// 新东西：<fstream> 里的 ofstream（写文件）、ifstream（读文件）
// 用法和 cout / cin 一模一样，只是把"屏幕"换成"文件"。
// 关键坑：>> 遇到空格就停，所以书名里有空格（如 C++ Primer）会被切碎；
//         这里改成"每个字段占一行"，用 getline 整行读。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include "Book.h"
using namespace std;

// 把书写进 books.txt：每本 5 行（书名 / ISBN / 价格 / 页数 / 是否在馆）
void saveBooks(vector<Book>& books)
{
    ofstream out("books.txt");        // 打开文件（不存在就新建，已存在就清空重写）
    if (!out)
    {
        cout << "打不开 books.txt，保存失败" << endl;
        return;
    }
    for (int i = 0; i < (int)books.size(); i++)
    {
        out << books[i].getTitle() << endl;
        out << books[i].getIsbn() << endl;
        out << books[i].getPrice() << endl;
        out << books[i].getPages() << endl;
        if (books[i].isAvailable()) out << 1 << endl;   // 1 = 在馆
        else                       out << 0 << endl;   // 0 = 已借出
    }
    out.close();
    cout << "已保存 " << books.size() << " 本到 books.txt" << endl;
}

// 从 books.txt 读回来
void loadBooks(vector<Book>& books)
{
    ifstream in("books.txt");
    if (!in)
    {
        cout << "没有找到 books.txt，先用默认图书" << endl;
        return;
    }
    string title, isbn, sPrice, sPages, sAvail;
    while (getline(in, title))                  // 读一整行当书名
    {
        if (!getline(in, isbn)) break;
        if (!getline(in, sPrice)) break;
        if (!getline(in, sPages)) break;
        if (!getline(in, sAvail)) break;

        Book b(title, isbn, stod(sPrice), stoi(sPages));   // 文本转数字
        if (stoi(sAvail) == 0) b.borrowOne();              // 存档是"已借出"，恢复状态
        books.push_back(b);
    }
    in.close();
    cout << "从 books.txt 载入 " << books.size() << " 本" << endl;
}

int main()
{
    SetConsoleOutputCP(936);

    vector<Book> books;
    loadBooks(books);                     // 先试着读存档

    if (books.empty())                    // 第一次跑：没有存档
    {
        books.push_back(Book("三体", "9787536692930", 23.0, 302));
        books.push_back(Book("C++ Primer", "9787115408105", 128.0, 748));
    }

    cout << "---- 当前图书 ----" << endl;
    for (int i = 0; i < (int)books.size(); i++)
    {
        books[i].showInfo();
    }

    saveBooks(books);                     // 再存一次

    return 0;
}
