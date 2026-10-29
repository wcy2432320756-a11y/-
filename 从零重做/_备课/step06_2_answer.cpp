// ============================================================
// 第 6.2 课 · 章末练习（R 的答案版，仅供备课验证）
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

    bool checkISBN(string s) {
        int n = (int)s.size();
        if (n != 13) return false;
        int sum = 0;
        for (int i = 0; i < 13; i++) {
            int d = s[i] - '0';
            if (d < 0 || d > 9) return false;
            if (i % 2 == 0) sum = sum + d * 1;
            else            sum = sum + d * 3;
        }
        return sum % 10 == 0;
    }

    void setPages(int p) {
        if (p > 0) { pages = p; }
        else { cout << "【拒收】页数必须大于 0：" << p << endl; }
    }

    string getIsbn() { return isbn; }

    void showInfo() {
        cout << "《" << title << "》 ISBN：" << isbn
             << "  " << price << " 元, " << pages << " 页" << endl;
    }
};

int main() {
    SetConsoleOutputCP(936);

    Book b("解忧杂货店", "9787544270878", 39.5, 320);

    cout << b.checkISBN("9787544270878") << endl;
    cout << b.checkISBN("9787544270870") << endl;

    cout << "书名里的 ISBN 字段：" << b.getIsbn() << endl;

    b.setPages(-5);
    b.showInfo();

    return 0;
}