// ============================================================
// 第 10.1 课 · 章末练习（R 的答案版，仅供备课验证）
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
#include "Book.h"
using namespace std;

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

    bool borrowBook(Book& b);
    void returnBook(Book& b);
};

bool User::borrowBook(Book& b)
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
    b.borrowOne();
    borrowed++;
    cout << "借到：《" << b.getTitle() << "》，" << name
         << " 已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
    return true;
}

void User::returnBook(Book& b)
{
    b.returnOne();
    borrowed--;
    cout << "归还：《" << b.getTitle() << "》完成，" << name
         << " 已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
}

void borrowByValue(Book b)
{
    b.borrowOne();
    cout << "（值传递）副本可借吗？" << b.isAvailable() << endl;
}

int main()
{
    SetConsoleOutputCP(936);

    Book b1("三体", "9787536692930", 39.9, 302);
    Book b3("测试书", "9787544270878", 10.0, 100);

    User u("李四", 2);
    cout << "学生：李四，已借 0/2" << endl;

    u.borrowBook(b1);
    borrowByValue(b3);
    u.returnBook(b1);

    cout << "b1 可借吗？" << b1.isAvailable() << endl;
    cout << "b3 可借吗？" << b3.isAvailable() << endl;

    return 0;
}
