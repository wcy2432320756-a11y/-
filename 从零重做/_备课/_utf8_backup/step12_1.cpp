// ============================================================
// 第 12.1 课 · 章末练习：多态——把"报纸"加进基类指针数组
// ------------------------------------------------------------
// 基类 Resource 和三个子类在下面（完整，别动）。
// 请补全 Newspaper 类 和 main 里的循环（标【空x】）。
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
    Resource(string t, int i) { title = t; id = i; }

    virtual void showInfo()
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

    void showInfo() override
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

// 【空1】写 Newspaper 类：: public Resource，private 成员 string date;
// 【空2】构造函数：Newspaper(string t, int i, string d)，用初始化列表把 t、i 交给 Resource，date = d
// 【空3】showInfo() override：直接打印 "【报纸】《" << title << "》 " << date << endl
//        （这里没有 showBase，直接用 title 即可）
// ↓ 写在这下面 ↓



int main()
{
    SetConsoleOutputCP(936);

    PaperBook b("C++程序设计", 1, 59.0, 380);
    Magazine m("读者", 2, 15);
    EResource e("中国知网", 3, "https://cnki.net");
    Newspaper n("人民日报", 4, "2026-10-25");

    Resource* list[4] = { &b, &m, &e, &n };

    // 【空4】写 for 循环：i 从 0 到 3，逐个调用 list[i]->showInfo()
    // ↓ 写在这下面 ↓



    return 0;
}
