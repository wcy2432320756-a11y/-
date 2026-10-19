// ============================================================
// 第一课动手任务参考答案 —— 完整可运行版
// 编译运行： .\build.ps1 lesson01-answer
// 学生做完自己的 lesson01.cpp 之后再来对
// ============================================================

#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class BookCard
{
private:
    // 任务 A：新加的成员
    int    m_pages;

    string m_title;
    double m_price;
    bool   m_isBorrowed;

    // 任务 C 要用到，先把 ISBN 存进来
    string m_isbn;

public:
    BookCard()
    {
        // 任务 A：默认构造里也要给它初值，否则是垃圾值
        m_pages      = 0;
        m_title      = "未命名";
        m_price      = 0.0;
        m_isBorrowed = false;
        m_isbn       = "";
    }

    // 任务 A：带参构造多了 pages 和 isbn 两个参数
    BookCard(string title, double price, int pages, string isbn)
    {
        m_title      = title;
        m_price      = price;
        m_pages      = pages;
        m_isbn       = isbn;
        m_isBorrowed = false;
    }

    // ---- getter ----
    string getTitle()      { return m_title; }
    double getPrice()      { return m_price; }
    bool   getIsBorrowed() { return m_isBorrowed; }
    int    getPages()      { return m_pages; }     // 任务 A
    string getISBN()       { return m_isbn; }

    // ---- setter ----
    void setTitle(string title)   { m_title = title; }
    void setPrice(double price)   { m_price = price; }
    void setIsBorrowed(bool b)    { m_isBorrowed = b; }
    void setPages(int pages)      { m_pages = pages; }   // 任务 A
    void setISBN(string isbn)     { m_isbn = isbn; }

    // ---- 任务 B：价格合法性 ----
    bool isValidPrice()
    {
        return m_price > 0;       // 比较表达式本身就是 bool，直接返回就行
    }

    // ---- 任务 C：ISBN-13 合法性 ----
    // 规则：13 位数字。前 12 位从左到右按权重 1、3、1、3… 加权求和，
    //       校验位 = 10 - (和 % 10)，若结果是 10 则取 0，必须等于第 13 位。
    bool isValidISBN()
    {
        // 第一步：只挑出数字，顺便把 '-' 和空格当分隔符忽略，
        //         碰到别的字符直接判定非法
        string digits = "";
        for (int i = 0; i < (int)m_isbn.length(); i++)
        {
            char c = m_isbn[i];
            if (c >= '0' && c <= '9')
            {
                digits = digits + c;
            }
            else if (c == '-' || c == ' ')
            {
                continue;                  // 跳过，不算非法
            }
            else
            {
                return false;              // 出现了字母之类的东西
            }
        }

        // 第二步：必须正好 13 位
        if ((int)digits.length() != 13)
        {
            return false;
        }

        // 第三步：前 12 位加权求和
        int sum = 0;
        for (int i = 0; i < 12; i++)
        {
            int d = digits[i] - '0';       // '9' - '0' = 9，字符变数字的标准写法
            if (i % 2 == 0)
            {
                sum = sum + d * 1;         // 第 1、3、5… 位权重 1
            }
            else
            {
                sum = sum + d * 3;         // 第 2、4、6… 位权重 3
            }
        }

        // 第四步：算校验位并比对
        int check = 10 - (sum % 10);
        if (check == 10)
        {
            check = 0;
        }
        int lastDigit = digits[12] - '0';
        return check == lastDigit;
    }

    void display()
    {
        cout << "书名: " << m_title << endl;
        cout << "价格: " << m_price << " 元" << endl;
        cout << "页数: " << m_pages << " 页" << endl;                       // 任务 A
        cout << "ISBN: " << m_isbn << "  ("
             << (isValidISBN() ? "合法" : "不合法") << ")" << endl;          // 任务 C
        cout << "状态: " << (m_isBorrowed ? "已借出" : "可借") << endl;
        cout << "------------------------" << endl;
    }
};

int main()
{
    SetConsoleOutputCP(65001);

    // 这个 ISBN 是算过的：前 12 位加权和 131，131 % 10 = 1，10 - 1 = 9，末位正是 9
    BookCard ok("C++ Primer", 128.0, 848, "978-7-121-35857-9");
    // 末位改成 8，校验不过
    BookCard bad("算法导论", 128.0, 780, "9787121358578");
    // 位数不对
    BookCard shortOne("残本", 30.0, 100, "978712135");

    cout << "=== 三本书 ===" << endl;
    ok.display();
    bad.display();
    shortOne.display();

    // 任务 B 的验证
    BookCard negative("错价书", -5.0, 10, "9787121358579");
    cout << "价格为 -5 的书，isValidPrice() 返回: "
         << (negative.isValidPrice() ? "true（合法）" : "false（不合法）") << endl;
    cout << "正常价格为 128 的书，isValidPrice() 返回: "
         << (ok.isValidPrice() ? "true（合法）" : "false（不合法）") << endl;

    // 任务 A 的验证：单独改页数
    ok.setPages(900);
    cout << "\n改页数后 -> " << ok.getTitle() << " 共 " << ok.getPages() << " 页" << endl;

    return 0;
}
