// ============================================================
// 第 6.1 课：门卫实战 —— ISBN 合法性检查
// ------------------------------------------------------------
// 上一课的门卫只会说"价格必须大于 0"。
// 这一课给门卫装上真本事：ISBN 合法吗？
//
// 新语法 3 个：
//   s[i]      取字符串里第 i 个字符（下标从 0 开始！第一个字符是 s[0]）
//   s.size()  字符串的长度（13 位才算数）
//   %         取余数（7 % 2 得 1）
// 外加两个小知识：
//   s[i] - '0'  把一个数字字符变成整数（'7' - '0' 就是 7）
//   bool        一种只装 true / false 的返回类型
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
        title = t;
        isbn = i;
        price = p;
        pages = pg;
    }

    // ① 门卫：这个 ISBN 合法吗？
    bool checkISBN(string s) {
        int n = (int)s.size();                // ①-a 字符串长度
        if (n != 13) return false;            // 不是 13 位 → 直接不合法

        int sum = 0;
        for (int i = 0; i < 13; i++) {        // ①-b 遍历每一位
            int d = s[i] - '0';               // ①-c 字符 → 数字
            if (d < 0 || d > 9) return false; // 混了非数字 → 不合法
            if (i % 2 == 0) sum = sum + d * 1; // ①-d 偶数位权重 1
            else            sum = sum + d * 3; //      奇数位权重 3
        }
        return sum % 10 == 0;                 // ①-e 能被 10 整除 → 合法
    }

    // ② 带门卫的写门
    void setISBN(string i) {
        if (checkISBN(i)) {
            isbn = i;
        }
        else {
            cout << "【拒收】ISBN 校验失败：" << i << endl;
        }
    }

    void showInfo() {
        cout << "《" << title << "》 ISBN：" << isbn
             << "  " << price << " 元, " << pages << " 页" << endl;
    }
};

int main() {
    SetConsoleOutputCP(936);   // 让控制台按 GBK 显示（项目约定）

    Book b("解忧杂货店", "9787544270878", 39.5, 320);

    // ③ 合法：9787544270878
    b.showInfo();

    // ④ 非法：最后一位被改坏
    b.setISBN("9787544270870");
    b.showInfo();

    // ⑤ 非法：只有 12 位
    b.setISBN("978754427087");
    b.showInfo();

    // ⑥ 非法：改掉中间一位
    b.setISBN("9787544270888");
    b.showInfo();

    // ⑦ 坑（看完注释改回去）：
    //    把 ①-d 的 `i % 2 == 0` 改成 `i % 2 == 1`，再跑
    //    → 9787544270878 也会被拒收：权重反了，加权和就不对了

    return 0;
}
