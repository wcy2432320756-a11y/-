// ============================================================
// 第 11 课 · 继承：图书资源家族
// ------------------------------------------------------------
// 实验三：从"图书资源"基类派生出 一般图书 / 杂志 / 电子资源。
// 派生类 = "is-a"：杂志**是**一种图书资源。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

// ① 基类：所有资源共有的东西
class Resource {
protected:                       // ② protected：自己和子类能用，外界不能碰
    string title;
    int id;

public:
    Resource(string t, int i)    // 基类构造函数
    {
        title = t;
        id = i;
    }

    void showBase()
    {
        cout << "编号 " << id << "：《" << title << "》";
    }
};

// ③ 派生类：一般图书（纸质书）
//    注意：这里没有用 Book 这个名字——项目里已经有第 7 课的 Book 类了，重名会撞车
class PaperBook : public Resource {
private:
    double price;
    int pages;

public:
    PaperBook(string t, int i, double p, int pg)
        : Resource(t, i)         // ④ 初始化列表：先造基类那部分
    {
        price = p;
        pages = pg;
    }

    void showInfo()
    {
        showBase();              // ⑤ 基类的函数直接用
        cout << "  价格 " << price << " 元，" << pages << " 页" << endl;
    }
};

// ⑥ 派生类：杂志
class Magazine : public Resource {
private:
    int issue;

public:
    Magazine(string t, int i, int iss) : Resource(t, i)
    {
        issue = iss;
    }

    void showInfo()
    {
        showBase();
        cout << "  第 " << issue << " 期杂志" << endl;
    }
};

// ⑦ 派生类：电子资源
class EResource : public Resource {
private:
    string url;

public:
    EResource(string t, int i, string u) : Resource(t, i)
    {
        url = u;
    }

    void showInfo()
    {
        showBase();
        cout << "  链接 " << url << endl;
    }
};

int main()
{
    SetConsoleOutputCP(936);

    PaperBook b("C++程序设计", 1, 59.0, 380);
    Magazine m("读者", 2, 15);
    EResource e("中国知网", 3, "https://cnki.net");

    b.showInfo();
    m.showInfo();
    e.showInfo();

    return 0;
}
