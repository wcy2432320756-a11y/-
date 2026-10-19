// ============================================================
// 第一课练习：一个"图书卡"类
// 它就是作业里 Book 类的简化版 —— 只有 3 个数据成员，好读懂
// 编译运行： .\build.ps1 lesson01
// ============================================================

#include <iostream>
#include <string>
#include <windows.h>          // 只为了用 SetConsoleOutputCP，让中文不乱码
using namespace std;

// ---------- 类的定义 ----------
// class 名字 { private: ... public: ... };
// 这是一张"图纸"，还没有占用任何实际数据
class BookCard
{
private:
    // 数据成员（成员变量）：这一类的对象都有这三样东西
    string m_title;           // 书名
    double m_price;           // 价格
    bool   m_isBorrowed;      // 是否已借出

public:
    // ---- 构造函数 1：默认构造 ----
    // 名字和类名一样、没有返回值 —— 创建对象时自动被调用
    BookCard()
    {
        m_title      = "未命名";
        m_price      = 0.0;
        m_isBorrowed = false;
    }

    // ---- 构造函数 2：带参构造 ----
    // 和上面名字一样但参数不同 = 重载。编译器按你传的参数自己挑
    BookCard(string title, double price)
    {
        m_title      = title;
        m_price      = price;
        m_isBorrowed = false;     // 新书默认在馆
    }

    // ---- 获取器（getter）----
    // 读数据成员的唯一入口
    string getTitle()       { return m_title; }
    double getPrice()       { return m_price; }
    bool   getIsBorrowed()  { return m_isBorrowed; }

    // ---- 修改器（setter）----
    // 写数据成员的唯一入口
    void setTitle(string title)      { m_title = title; }
    void setPrice(double price)      { m_price = price; }
    void setIsBorrowed(bool b)       { m_isBorrowed = b; }

    // ---- 展示 ----
    void display()
    {
        cout << "书名: " << m_title << endl;
        cout << "价格: " << m_price << " 元" << endl;
        // (条件 ? 值1 : 值2) 是三元运算符：条件为真取值1，否则值2
        cout << "状态: " << (m_isBorrowed ? "已借出" : "可借") << endl;
        cout << "------------------------" << endl;
    }
};

// ---------- 程序入口 ----------
// 程序从 main 的第一行开始跑，跑到 return 0 结束
int main()
{
    SetConsoleOutputCP(65001);        // 让控制台按 UTF-8 显示中文

    // 创建对象 1、2：走带参构造
    BookCard b1("C++ Primer", 128.0);
    BookCard b2("算法导论", 128.0);

    // 创建对象 3：不传参数，走默认构造
    BookCard b3;

    cout << "=== 初始状态 ===" << endl;
    b1.display();
    b2.display();
    b3.display();

    // 调用成员函数修改对象的状态
    b1.setIsBorrowed(true);        // 第 1 本被借走了
    b2.setPrice(99.0);             // 第 2 本打折
    b3.setTitle("Effective C++");  // 第 3 本改名

    cout << "=== 修改后 ===" << endl;
    b1.display();
    b2.display();
    b3.display();

    // getter 把值取出来参与别的运算
    cout << "第 1 本叫: " << b1.getTitle() << endl;
    cout << "第 2 本现价: " << b2.getPrice() << " 元" << endl;

    return 0;      // 0 = 正常结束
}
