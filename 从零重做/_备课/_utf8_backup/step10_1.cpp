// ============================================================
// 第 10.1 课 · 章末练习：借书、还书与"复印件"
// ------------------------------------------------------------
// 第 10 章（依赖 + 引用）的收尾练习。
// 三个空：借书、还书、值传递对照。
// 每一处都涉及"值 vs 引用"，做完就能分清。
// ============================================================
#include <iostream>
#include <string>
#include <windows.h>
#include "Book.h"
using namespace std;

class User {
private:
    string name;
    int borrowed;
    int maxBorrow;

public:
    User(string n, int maxB)
    {
        name = n;
        borrowed = 0;
        maxBorrow = maxB;
    }

    bool borrowBook(Book& b);   // 声明
    void returnBook(Book& b);   // 声明
};

// 【空1】borrowBook 的定义（类外，记得写 User::）
//   1) 已借满 → 打印"【借书失败】<name> 已借满 <max> 本"，return false
//   2) 书不可借 → 打印"【借书失败】《书名》已经借出"，return false
//   3) 都过 → b.borrowOne(); borrowed++; 打印"借到：《书名》，<name> 已借 <数>/<max> 本"；return true
// ↓ 写在这下面 ↓



// 【空2】returnBook 的定义
//   b.returnOne(); borrowed--; 打印"归还：《书名》完成，<name> 已借 <数>/<max> 本"
//   顺序：先改书的状态，再减自己的计数
// ↓ 写在这下面 ↓



// 【空3】borrowByValue 的定义（值传递，参数没有 &）
//   b.borrowOne(); 打印"（值传递）副本可借吗？" << b.isAvailable()
//   提醒：改的是副本——main 最后会验证原书 b3 没受影响
// ↓ 写在这下面 ↓



int main()
{
    SetConsoleOutputCP(936);

    Book b1("三体", "9787536692930", 39.9, 302);
    Book b3("测试书", "9787544270878", 10.0, 100);

    User u("李四", 2);
    cout << "学生：李四，已借 0/2" << endl;

    u.borrowBook(b1);
    borrowByValue(b3);
    u.returnBook(b1);

    cout << "b1 可借吗？" << b1.isAvailable() << endl;
    cout << "b3 可借吗？" << b3.isAvailable() << endl;

    return 0;
}
