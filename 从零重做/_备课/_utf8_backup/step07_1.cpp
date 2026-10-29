// ============================================================
// 第 7.1 课 · 章末练习（main 部分，完整，不要动）
// ============================================================
#include <iostream>
#include <windows.h>
#include "Book2.h"
using namespace std;

int main()
{
    SetConsoleOutputCP(936);

    Book2 b("解忧杂货店", "9787544270878", 39.5, 320);
    b.showInfo();

    b.setPages(-5);
    b.showInfo();

    cout << "ISBN 校验：" << b.checkISBN("9787544270878") << endl;
    cout << "拿到的 ISBN：" << b.getIsbn() << endl;

    return 0;
}
