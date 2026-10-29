// ============================================================
// 第 13 课 · 学生与教师：两类用户，各自能借几本
// ------------------------------------------------------------
// 设计报告2 的地基：User 基类 → Student / Teacher 子类，
// 借书用 Book（第 7 课多文件）+ 依赖（第 10 课）+ 多态（第 12 课）。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
#include "Book.h"
using namespace std;

class User {
protected:
    string name;
    int id;
    int borrowed;      // 已借几本
    int maxBorrow;     // 最多能借几本

public:
    User(string n, int i, int max)
    {
        name = n;
        id = i;
        borrowed = 0;
        maxBorrow = max;
    }

    virtual void show()
    {
        cout << name << "（" << id << "）已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
    }

    // 借书（依赖 Book，和第 10 课一样用引用）
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
        cout << name << " 借了《" << b.getTitle() << "》" << endl;
        return true;
    }

    // 还书
    void returnBook(Book& b)
    {
        b.returnOne();
        if (borrowed > 0)
        {
            borrowed--;
        }
        cout << name << " 归还《" << b.getTitle() << "》" << endl;
    }
};

class Student : public User {
private:
    string major;      // 专业

public:
    Student(string n, int i, string m) : User(n, i, 3)   // 学生最多借 3 本
    {
        major = m;
    }

    void show() override
    {
        cout << "学生 " << name << "（" << id << "，" << major << "）已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
    }
};

class Teacher : public User {
private:
    string title;      // 职称

public:
    Teacher(string n, int i, string t) : User(n, i, 5)   // 教师最多借 5 本
    {
        title = t;
    }

    void show() override
    {
        cout << "教师 " << name << "（" << id << "，" << title << "）已借 " << borrowed << "/" << maxBorrow << " 本" << endl;
    }
};

int main()
{
    SetConsoleOutputCP(936);

    Book b1("三体", "9787536692930", 23.0, 302);
    Book b2("C++ Primer", "9787115408105", 128.0, 748);

    Student s("张三", 2023001, "计算机");
    Teacher t("李老师", 1001, "副教授");

    s.borrowBook(b1);      // 张三借《三体》→ 成功
    s.borrowBook(b1);      // 再借同一本 → 已被借出

    t.borrowBook(b2);      // 李老师借《C++ Primer》→ 成功
    s.borrowBook(b2);      // 被教师借了 → 失败

    s.show();
    t.show();

    s.returnBook(b1);      // 张三还《三体》
    t.borrowBook(b1);      // 李老师现在能借了

    s.show();
    t.show();

    return 0;
}
