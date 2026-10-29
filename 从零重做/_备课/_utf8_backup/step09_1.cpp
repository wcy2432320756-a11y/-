// ============================================================
// 第 9.1 课 · 章末练习（main 部分，完整，不要动）
// ============================================================
#include <iostream>
#include <windows.h>
#include "Library2.h"
using namespace std;

int main()
{
    SetConsoleOutputCP(936);

    Library2 lib("校图书馆");
    lib.addBook(Book("三体", "9787536692930", 39.9, 302));
    lib.addBook(Book("活着", "9787506365437", 45.0, 191));
    lib.addBook(Book("解忧杂货店", "9787544270878", 39.5, 320));

    lib.showAll();
    lib.findBook("9787544270878");
    lib.findBook("000");

    return 0;
}
