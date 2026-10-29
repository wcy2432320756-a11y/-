// ============================================================
// 第 10 课 · 依赖：学生借书
// ------------------------------------------------------------
// 依赖 = 一个类的成员函数把另一个类的对象当参数用它一下
// （"我用它"，不是"我拥有它"）。
// 学生借书就是典型：User::borrowBook(Book& b) —— 书还是图书馆的。
//
// 本课重点：值传递 vs 引用传递（实验二的原题）。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
#include "Book.h"
using namespace std;

// ① User 类：名字、已借几本、最多借几本
class User {
private:
    string name;
    int borrowed;
    int maxBorrow;

public:
    User(string n, int maxB)
    {
        name = n;
        borrowed = 0;
        maxBorrow = maxB;
    }

    // ② 依赖：借书 —— 参数是 Book 的"引用"（不拥有，只是用一下）
    bool borrowBook(Book& b)
    {
        if (borrowed >= maxBorrow)
        {
            cout << "【借书失败】" << name << " 已借满 " << maxBorrow << " 本" << endl;
            return false;
        }
        if (!b.isAvailable())
        {
            cout << "【借书失败】《" << b.getTitle() << "》已经借出" << endl;
            return false;
        }

        b.borrowOne();           // ③ 改书的状态 —— 因为传的是引用，改的是原书
        borrowed++;
        cout << "借到：《" << b.getTitle() << "》，" << name
             << " 已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
        return true;
    }

    void showMe()
    {
        cout << "学生：" << name << "，已借 " << borrowed << "/" << maxBorrow << endl;
    }
};

// ④ 对照实验：值传递拿副本，引用传递拿本体
void borrowByValue(Book b)       // 值传递：复制了一份
{
    b.borrowOne();
    cout << "（值传递函数内部）副本可借吗？" << b.isAvailable() << endl;
}

void borrowByRef(Book& b)        // 引用传递：拿到原书
{
    b.borrowOne();
    cout << "（引用函数内部）原书可借吗？" << b.isAvailable() << endl;
}

int main()
{
    SetConsoleOutputCP(936);

    Book b1("三体", "9787536692930", 39.9, 302);
    Book b2("活着", "9787506365437", 45.0, 191);

    User u("张三", 2);           // 最多借 2 本
    u.showMe();

    cout << endl << "== 借书 ==" << endl;
    u.borrowBook(b1);            // 借到
    u.borrowBook(b1);            // 这本已经借出 → 失败
    u.borrowBook(b2);            // 借到（第 2 本）
    u.borrowBook(b1);            // 已到上限 → 失败

    cout << endl << "== 值传递 vs 引用传递（用第三本书）==" << endl;
    Book b3("解忧杂货店", "9787544270878", 39.5, 320);

    borrowByValue(b3);
    cout << "函数返回后，b3 可借吗？" << b3.isAvailable() << endl;

    borrowByRef(b3);
    cout << "函数返回后，b3 可借吗？" << b3.isAvailable() << endl;

    return 0;
}
