// ============================================================
// 第 6.2 课 · 章末练习：上锁的图书卡
// ------------------------------------------------------------
// 第 6 章（数据上锁 + ISBN 检查）的收尾练习。
// 四处【空N】，补齐、编过、跑对，第 6 章就算过关。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class Book {
private:
    string title;
    string isbn;
    double price;
    int pages;

public:
    Book(string t, string i, double p, int pg) {
        title = t; isbn = i; price = p; pages = pg;
    }

    bool CheckISBN(string s)
    {
        int d=s.size(), sum = 0;
        if (d != 13)return false;
        else
        {
            for (int i = 0;i<13;i++)
            {
                int b = s[i] - '0';
                if (b < 0 || b>9) return false;
                else if (i % 2 == 0) sum += b * 1;
                else sum += b * 3;
            }
        }
        
        // 【空1】加权：偶数位 ×1，奇数位 ×3，累加进 sum
        // ↓ 写两行 if/else 在这下面 ↓
        return sum % 10 == 0;
    }

    // 【空2】写门：setPages —— 页数大于 0 才收，否则打印【拒收】
    // ↓ 写在这下面 ↓
    void setpages(int py)
    {
        if (py > 0)
        {
            pages = py;
        }
        else cout << "拒收" << endl;
    }


    // 【空3】读门：getIsbn —— 返回 isbn
    // ↓ 写在这下面 ↓

    void getISBN(string a)
    {
        if (CheckISBN(a))
        {
            isbn = a;
        }
        else cout<< "【拒收】ISBN 校验失败：" << a <<"不合法" << endl;

    }

    void showInfo() {
        cout << "《" << title << "》 ISBN：" << isbn
             << "  " << price << " 元, " << pages << " 页" << endl;
    }
};

int main() {
    SetConsoleOutputCP(936);

    Book b("解忧杂货店", "9787544270878", 39.5, 320);

    // 【空4】打印两次校验结果：
    //        b.checkISBN("9787544270878") 应该打印 1（合法）
    //        b.checkISBN("9787544270870") 应该打印 0（非法）
    // ↓ 写在这下面 ↓
    b.CheckISBN("9787544270878");
    b.CheckISBN("9787544270870");


    // 这一行用到 getIsbn（空3）：空3 不填，这里就编不过
    

    // 这两行替你测 setPages：非法页数应该被拒收
    b.setpages(320);
    b.showInfo();

    return 0;
}
