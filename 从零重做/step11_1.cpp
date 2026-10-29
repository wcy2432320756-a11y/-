// ============================================================
// 第 11.1 课 · 章末练习：给图书资源家族添"报纸"
// ------------------------------------------------------------
// 基类 Resource 在下面（完整，别动）；请派生 Newspaper（报纸）。
// 三处空：类声明 + showInfo + main 用法。
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

    void showBase()
    {
        cout << "编号 " << id << "：《" << title << "》";
    }
};

// 【空1】派生 Newspaper : public Resource
//   成员：string date;（日期，private）
//   构造函数：Newspaper(string t, int i, string d)，用初始化列表把 t、i 交给 Resource，date = d
// ↓ 写在这下面 ↓
class Newspaper :public Resource
{
private:
    string date;
public:
    Newspaper(string t, int i, string d) : Resource(t, i)
    {
        date = d;
    }
    void showInfo()
    {
        showBase();
        cout << "日期:" << date << endl;
    }



};


// 【空2】在 Newspaper 里写 showInfo()：先 showBase()，再打印 "  日期 " << date << endl
//        （注意：showInfo 要写在类里面——可以和空1 一起写，也可以分开）
// ↓ 写在这下面 ↓



int main()
{ 
    SetConsoleOutputCP(936);

    // 【空3】造一份报纸：编号 4，标题"人民日报"，日期"2026-10-25"，然后调用 showInfo()
    // ↓ 写在这下面 ↓
    Newspaper n("人民日报", 4, "2026-10-25");
    n.showInfo();


    return 0;
}
