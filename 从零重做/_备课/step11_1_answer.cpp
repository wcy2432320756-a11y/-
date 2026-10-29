// ============================================================
// 第 11.1 课 · 章末练习（R 的答案版，仅供备课验证）
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

class Newspaper : public Resource {
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
        cout << "  日期 " << date << endl;
    }
};

int main()
{
    SetConsoleOutputCP(936);

    Newspaper n("人民日报", 4, "2026-10-25");
    n.showInfo();

    return 0;
}
