// ============================================================
// 第 12 课 · 多态：一个接口，多种行为
// ------------------------------------------------------------
// 实验四：基类指针指向不同子类对象，同一个函数调用干不同的事。
// 关键：基类函数加 virtual，子类函数加 override。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class Resource {
protected:
    string title;
    int id;

public:
    Resource(string t, int i)
    {
        title = t;
        id = i;
    }

    virtual void showInfo()          // ① virtual：告诉编译器"子类可以改写它"
    {
        cout << "编号 " << id << "：《" << title << "》" << endl;
    }
};

class PaperBook : public Resource {
private:
    double price;
    int pages;

public:
    PaperBook(string t, int i, double p, int pg) : Resource(t, i)
    {
        price = p;
        pages = pg;
    }

    void showInfo() override         // ② override：明确"我在改写基类的虚函数"
    {
        cout << "【图书】《" << title << "》 " << price << " 元，" << pages << " 页" << endl;
    }
};

class Magazine : public Resource {
private:
    int issue;

public:
    Magazine(string t, int i, int iss) : Resource(t, i)
    {
        issue = iss;
    }

    void showInfo() override
    {
        cout << "【杂志】《" << title << "》 第 " << issue << " 期" << endl;
    }
};

class EResource : public Resource {
private:
    string url;

public:
    EResource(string t, int i, string u) : Resource(t, i)
    {
        url = u;
    }

    void showInfo() override
    {
        cout << "【电子】《" << title << "》 " << url << endl;
    }
};

int main()
{
    SetConsoleOutputCP(936);

    PaperBook b("C++程序设计", 1, 59.0, 380);
    Magazine m("读者", 2, 15);
    EResource e("中国知网", 3, "https://cnki.net");

    // ③ 基类指针数组：& 取地址（指针里存的是"门牌号"）
    Resource* list[3] = { &b, &m, &e };

    // ④ 同一个 showInfo()，各自干各自的事（多态）
    for (int i = 0; i < 3; i++)
    {
        list[i]->showInfo();          // -> 通过指针调用成员函数
    }

    // ⑤ 试试：把基类 showInfo 前面的 virtual 删掉，再跑一遍
    //    → 三行全变成"编号 …《…》"：没有 virtual，编译器只看指针类型（Resource），不看真实对象

    return 0;
}
