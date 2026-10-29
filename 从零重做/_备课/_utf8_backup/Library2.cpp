// ============================================================
// 第 9.1 课 · 章末练习：组合类的三个动作
// ------------------------------------------------------------
// Library2 是"图书馆组合图书"的练习版。
// 三个空，都在下面。编过、跑对（8 行输出）就过关。
// 编译运行：VS 里 Ctrl+F5；或 Dev-C++ 打开本文件按 F11。
// ============================================================
#include "Library2.h"
#include <iostream>

Library2::Library2(string n)
{
    name = n;
}

// 【空1】addBook：把 b 塞进 books，并打印"入库：《书名》"（书名用 b.getTitle()）
// ↓ 写在这下面 ↓



int Library2::count()
{
    return (int)books.size();
}

// 【空2】showAll：先打印 "== <name> 共 <count> 本 =="，再逐本 showInfo
//        提示：for (int i = 0; i < count(); i++) books[i].showInfo();
// ↓ 写在这下面 ↓



// 【空3】findBook：遍历 books，谁的 ISBN 等于参数就打印"找到：《书名》"并 return；
//        全部找完都没找到，打印"没找到 ISBN 为 <参数> 的书"
//        提示：books[i].getIsbn() == isbn
// ↓ 写在这下面 ↓


