// ============================================================
// 第二课练习：组合与依赖
// 编译运行： .\build.ps1 lesson02
//
// 本文件对应讲义的三节，一段一节，跑一遍等于把三节的演示全看一遍：
//   === 第五节 ===  const Book& ：不拷贝、也不许改
//   === 第六节 ===  组合：Book 当成员，构造顺序 + 抄一份
//   === 第十节 ===  动手任务 A / B / C 的参考实现
// ============================================================

#include <iostream>
#include <string>
#include <windows.h>          // 只为了用 SetConsoleOutputCP，让中文不乱码
using namespace std;

// ============================================================
// 共用的 Book 类
// 接口名照你项目里的 Book.h 抄的，只是少留了几个字段好读
//
// 注意下面每个 getter 后面的 const —— 第五节整节都在讲它为什么必须加。
// 不加的后果不是"风格差一点"，是第五节 5.4 那个报错，一行代码都编译不过
// ============================================================
class Book
{
private:
    string m_title;           // 书名
    string m_author;          // 作者
    string m_isbn;            // ISBN：借还时用它判断"是不是同一本书"
    double m_price;           // 价格
    bool   m_isBorrowed;      // 是否已借出

public:
    // ---- 默认构造 ----
    // 第六节要写 Book m_books[3] 这种对象数组，
    // 造数组时编译器会拿默认构造把每个格子造出来。没有它那一行是编译错误
    Book()
    {
        m_title      = "";
        m_author     = "";
        m_isbn       = "";
        m_price      = 0.0;
        m_isBorrowed = false;
    }

    // ---- 带参构造 ----
    Book(string title, string author, string isbn, double price)
    {
        m_title      = title;
        m_author     = author;
        m_isbn       = isbn;
        m_price      = price;
        m_isBorrowed = false;         // 新书默认在馆
    }

    // ---- getter：只读，所以每个尾巴上都有 const ----
    string getTitle()      const { return m_title; }
    string getAuthor()     const { return m_author; }
    string getIsbn()       const { return m_isbn; }
    double getPrice()      const { return m_price; }
    bool   getIsBorrowed() const { return m_isBorrowed; }

    // ---- setter：要改数据，所以一个都不能带 const ----
    void setTitle(string title) { m_title = title; }
    void setPrice(double price) { m_price = price; }
    void setIsBorrowed(bool b)  { m_isBorrowed = b; }

    // ---- 把状态变成一句话 ----
    string statusText() const
    {
        return m_isBorrowed ? "已借出" : "可借";
    }

    // ---- 打印一本书 ----
    void display() const
    {
        cout << "    《" << m_title << "》 作者: " << m_author
             << "  ISBN: " << m_isbn
             << "  价格: " << m_price
             << "  状态: " << statusText() << endl;
    }

    // ---- 第五节第六种 const 位置就在这：函数尾巴上的 const ----
    void whoAmI() const
    {
        cout << "        我是《" << m_title << "》，状态 " << statusText()
             << "（这个函数承诺不改任何成员）" << endl;
    }
};

// ============================================================
// 第五节用到的几个函数
// ============================================================

// ---- 5.1 三种传参方式，看形参地址就知道有没有拷贝 ----
void showByValue(Book b)                 // 值传递：形参是一份全新的拷贝
{
    cout << "    传值      : 形参地址 " << &b << "  ← 和外面不是同一个对象" << endl;
}

void showByRef(Book& b)                  // 引用传递：形参就是外面那本本人
{
    cout << "    引用      : 形参地址 " << &b << "  ← 就是外面那一个" << endl;
}

void showByConstRef(const Book& b)       // const 引用：本人，但不许改
{
    cout << "    const引用 : 形参地址 " << &b << "  ← 也是外面那一个" << endl;
}

// ---- 5.2 参数少写一个 const，手滑就把数据改了，编译器一声不吭 ----
void printButCanChange(Book& b)
{
    cout << "    打印: 《" << b.getTitle() << "》" << endl;
    b.setIsBorrowed(true);       // 这行是"手滑"。参数要是 const Book&，它根本编不过
}

// ---- 5.2 的对照：同样只想打印，参数写成 const 引用 ----
void printSafely(const Book& b)
{
    cout << "    打印: 《" << b.getTitle() << "》" << endl;
    // b.setIsBorrowed(true);    // 取消注释 → error C2662，编译器在这里拦住你
}

// ---- 5.3 六种 const 位置，一个一个摆出来，全部都编译通过 ----
void pos1_constRef(const Book& b)        // ① 不许通过 b 改对象
{
    cout << "        读到: " << b.getTitle() << endl;
}

void pos2_refConst(Book const& b)        // ② 和 ① 完全等价，只是 const 换了位置
{
    cout << "        读到: " << b.getTitle() << "（写法不同，类型完全相同）" << endl;
}

void pos3_constPtr(const Book* p)        // ③ 不许通过 p 改对象，但 p 可以换指向
{
    cout << "        读到: " << p->getTitle() << endl;
    const Book* q = p;
    q = nullptr;                         // 允许：换指向，不改数据
    cout << "        指针已改成空: " << (q == nullptr ? "是" : "否") << endl;
}

void pos4_ptrConst(Book* const p)        // ④ 可以改对象，但 p 不能换指向
{
    cout << "        读到: " << p->getTitle() << endl;
    p->setPrice(55.0);                   // 允许：改数据
    cout << "        价格已改成: " << p->getPrice() << endl;
}

void pos5_constPtrConst(const Book* const p)   // ⑤ 两头都锁死
{
    cout << "        读到: " << p->getTitle() << "（数据和指向都不许动）" << endl;
}

// ---- 5.4 getter 加了 const，才能在 const Book& 上随便调用 ----
void printCard(const Book& b)
{
    cout << "    在 const Book& 上调用 getter：" << endl;
    cout << "      书名 " << b.getTitle()
         << "   作者 " << b.getAuthor()
         << "   ISBN " << b.getIsbn() << endl;
    cout << "      价格 " << b.getPrice()
         << "   状态 " << b.statusText() << endl;
}

// ============================================================
// 第六节用到的类
// ============================================================

// ---- 6.1 会自己报名的两个类，只为把构造 / 析构顺序打出来 ----
class Part
{
private:
    string m_tag;             // 起个名，好认出析构时是哪一个

public:
    Part(const string& tag) : m_tag(tag)
    {
        cout << "      [构造] Part " << m_tag << endl;
    }

    ~Part()
    {
        cout << "      [析构] Part " << m_tag << endl;
    }
};

class Owner
{
private:
    Part m_a;                 // 先声明
    Part m_b;                 // 后声明

public:
    // 初始化列表里也是 m_a 在前 ——
    // 但真正说了算的是上面的声明顺序，不是这里的书写顺序
    Owner() : m_a("m_a"), m_b("m_b")
    {
        cout << "      [构造] Owner 的构造体开始执行（m_a / m_b 早已造好）" << endl;
    }

    ~Owner()
    {
        cout << "      [析构] Owner 的析构体开始执行（m_a / m_b 还活着）" << endl;
    }
};

// ---- 6.2 场景一：const 成员，只能在初始化列表里构造 ----
class BookWithConstIsbn
{
private:
    const string m_isbn;      // 加锁：一旦定下，终身不许改
    string       m_title;

public:
    BookWithConstIsbn(string isbn, string title)
        : m_isbn(isbn), m_title(title)     // 唯一合法写法：本体还没进，值已经定好
    {
    }

    string getIsbn() const { return m_isbn; }
    string getTitle() const { return m_title; }
};

// ---- 6.2 场景二：引用成员，必须在出生那一刻绑定 ----
class School
{
private:
    string m_name;

public:
    School(string n) : m_name(n) {}
    string getName() const { return m_name; }
};

class StudentWithSchool
{
private:
    School& m_school;         // 引用成员：绑死在某个学校上，之后再也换不了
    string  m_name;

public:
    StudentWithSchool(School& s, string n)
        : m_school(s), m_name(n)           // 唯一合法写法
    {
    }

    string describe() const
    {
        return m_name + " 属于 " + m_school.getName();
    }
};

// ---- 6.2 场景三：成员对象没有默认构造函数 ----
class NoDefault
{
private:
    int m_value;

public:
    NoDefault(int v) : m_value(v) {}       // 只给了带参构造，"自动送的"那个就没了

    int getValue() const { return m_value; }
};

class NoDefaultHolder
{
private:
    NoDefault m_item;

public:
    NoDefaultHolder(int v)
        : m_item(v)                        // 唯一合法写法
    {
    }

    int getValue() const { return m_item.getValue(); }
};

// ---- 6.3 两种"放书"的方式，用来证明组合是抄一份、不是绑一起 ----
class ShelfA                    // 对象数组：三本书真住在里面
{
public:
    Book m_books[3];
    int  m_count;

    ShelfA() : m_count(0) {}
};

class ShelfB                    // 指针数组：只存三个地址，书还在外面
{
public:
    Book* m_books[3];
    int   m_count;

    ShelfB() : m_count(0)
    {
        for (int i = 0; i < 3; i++)
        {
            m_books[i] = nullptr;
        }
    }
};

// ============================================================
// 第十节　任务 A：Book 直接当 Student 的成员（组合入门）
// ============================================================
class StudentA
{
private:
    string m_name;
    string m_id;
    Book   m_borrowed;        // 组合就发生在这一行：成员是一个类对象

public:
    // 默认构造：不写 m_borrowed，它也会用 Book 的默认构造自己造好
    StudentA()
        : m_name(""), m_id(""), m_borrowed()
    {
    }

    // 带参构造：m_borrowed(b) = 用 b 拷贝构造我的成员
    // 参数写 const Book& ：不拷贝、也不许改外面那本
    StudentA(string name, string id, const Book& b)
        : m_name(name), m_id(id), m_borrowed(b)
    {
    }

    void display() const
    {
        cout << "    学生: " << m_name << "  学号: " << m_id << endl;
        cout << "    借书记录:" << endl;
        m_borrowed.display();           // 让成员对象自己打印自己
    }

    const Book& getBorrowed() const { return m_borrowed; }
};

// ============================================================
// 第十节　任务 B：借书 / 还书，参数传指针（依赖入门）
// ============================================================
class StudentB
{
private:
    string m_name;
    string m_id;
    Book   m_borrowed;        // 借的那本书（组合：抄一份进来）
    bool   m_hasBorrowed;     // 有没有借 —— "借了一本空书"和"没借"要分清

public:
    StudentB()
        : m_name(""), m_id(""), m_borrowed(), m_hasBorrowed(false)
    {
    }

    StudentB(string name, string id)
        : m_name(name), m_id(id), m_borrowed(), m_hasBorrowed(false)
    {
    }

    // 参数是 Book*（地址），不是 Book（复印件）。
    // 传值的后果：改的是复印件，函数一结束就没了，外面那本纹丝不动
    bool borrowBook(Book* b)
    {
        if (b == nullptr)
        {
            cout << "    [借书失败] 传进来的书指针是空的。" << endl;
            return false;
        }

        if (m_hasBorrowed)
        {
            cout << "    [借书失败] " << m_name << " 已经借了《"
                 << m_borrowed.getTitle() << "》，先还了再说。" << endl;
            return false;
        }

        if (b->getIsBorrowed())
        {
            cout << "    [借书失败] 《" << b->getTitle() << "》已经被别人借走了。" << endl;
            return false;
        }

        // 三关都过 → 两边状态一起改
        m_borrowed    = *b;              // 记录：把 b 指向的那本抄进我的成员
        m_hasBorrowed = true;
        b->setIsBorrowed(true);          // 真书：改成已借出

        cout << "    [借书成功] " << m_name << " 借到《"
             << m_borrowed.getTitle() << "》。" << endl;
        return true;
    }

    bool returnBook(Book* b)
    {
        if (b == nullptr)
        {
            cout << "    [还书失败] 传进来的书指针是空的。" << endl;
            return false;
        }

        if (!m_hasBorrowed)
        {
            cout << "    [还书失败] " << m_name << " 现在没有借书。" << endl;
            return false;
        }

        // 校验：还的必须是我借的那本（按 ISBN 比，ISBN 就是书的身份证）
        if (b->getIsbn() != m_borrowed.getIsbn())
        {
            cout << "    [还书失败] 《" << b->getTitle() << "》不是 "
                 << m_name << " 借的那本。" << endl;
            return false;
        }

        m_hasBorrowed = false;
        m_borrowed    = Book();          // 记录清空
        b->setIsBorrowed(false);         // 真书：改回可借

        cout << "    [还书成功] " << m_name << " 归还《" << b->getTitle() << "》。" << endl;
        return true;
    }

    void display() const
    {
        cout << "    学生: " << m_name << "  学号: " << m_id << endl;
        if (m_hasBorrowed)
        {
            cout << "    当前借阅: 《" << m_borrowed.getTitle() << "》" << endl;
        }
        else
        {
            cout << "    当前借阅: 无" << endl;
        }
    }
};

// ---- 故意写错的版本：形参是 Book（值传递）----
// 能编译、能运行、不报错，但什么都改不了。这是本课最阴险的错：不报错的错
void borrowByValue(Book b)
{
    cout << "      （函数里）形参 b 的地址: " << &b << endl;
    b.setIsBorrowed(true);               // 改的是复印件
}

// ---- 同样的逻辑，换成指针传 ----
void borrowByPointer(Book* b)
{
    cout << "      （函数里）形参 b 的值:   " << b << "  ← 就是外面那本书的地址" << endl;
    b->setIsBorrowed(true);              // 顺着地址找到真书，改的是它本身
}

// ============================================================
// 第十节　任务 C（作业真需求）：对象数组 + 借还 + 还书校验
// ============================================================
class StudentC
{
private:
    string m_name;
    string m_id;
    Book   m_books[10];       // 对象数组：10 个 Book 住在 StudentC 里面
    int    m_borrownum;       // 借了几本 —— 同时是"下一个空位的下标"

public:
    StudentC()
        : m_name(""), m_id(""), m_borrownum(0)
    {
        // m_books 不用写：进这个花括号之前，10 个格子已经用默认构造造好了
    }

    StudentC(string name, string id)
        : m_name(name), m_id(id), m_borrownum(0)
    {
    }

    bool borrowBook(Book* b)
    {
        if (b == nullptr)
        {
            cout << "    [借书失败] 传进来的书指针是空的。" << endl;
            return false;
        }

        // 第 1 关：记录满了没有（不判就会写出界，程序可能直接崩）
        if (m_borrownum >= 10)
        {
            cout << "    [借书失败] " << m_name << " 的借阅记录已满（上限 10 本）。" << endl;
            return false;
        }

        // 第 2 关：我是不是已经借过同一本了（按 ISBN 判重复）
        // 这一关必须排在"书已借出"前面 —— 否则你借的书自己占着状态，
        // 第二次借同一本会报"已经被借走了"，学生看不懂到底是谁借的
        for (int i = 0; i < m_borrownum; i++)
        {
            if (m_books[i].getIsbn() == b->getIsbn())
            {
                cout << "    [借书失败] 你已经借过《" << b->getTitle() << "》了。" << endl;
                return false;
            }
        }

        // 第 3 关：这本书是不是已经被别人借走了
        if (b->getIsBorrowed())
        {
            cout << "    [借书失败] 《" << b->getTitle() << "》已经被别人借走了。" << endl;
            return false;
        }

        m_books[m_borrownum] = *b;       // 抄一份进第 m_borrownum 个格子
        m_borrownum++;
        b->setIsBorrowed(true);          // 真书：改成已借出

        cout << "    [借书成功] " << m_name << " 借到《" << b->getTitle()
             << "》，当前共 " << m_borrownum << " 本。" << endl;
        return true;
    }

    // 还书的核心：必须校验"还的是不是他借的那本"
    bool returnBook(Book* b)
    {
        if (b == nullptr)
        {
            cout << "    [还书失败] 传进来的书指针是空的。" << endl;
            return false;
        }

        if (m_borrownum == 0)
        {
            cout << "    [还书失败] " << m_name << " 没有借书记录。" << endl;
            return false;
        }

        // 第 1 步：在记录里找这本书，找到就记住位置 pos
        int pos = -1;
        for (int i = 0; i < m_borrownum; i++)
        {
            if (m_books[i].getIsbn() == b->getIsbn())
            {
                pos = i;
                break;
            }
        }

        // 找不到 = 这本书不是他借的。少了这一步，就变成"借了 A 却能还 B"
        if (pos == -1)
        {
            cout << "    [还书失败] 《" << b->getTitle() << "》不是 "
                 << m_name << " 借的，不能还。" << endl;
            return false;
        }

        // 第 2 步：把 pos 后面所有元素往前挪一格，把洞填掉
        for (int i = pos; i < m_borrownum - 1; i++)
        {
            m_books[i] = m_books[i + 1];
        }

        // 第 3 步：数量 -1，顺手把尾巴上多出来的格子清空
        m_borrownum--;
        m_books[m_borrownum] = Book();

        // 第 4 步：真书恢复可借
        b->setIsBorrowed(false);

        cout << "    [还书成功] " << m_name << " 归还《" << b->getTitle()
             << "》，当前共 " << m_borrownum << " 本。" << endl;
        return true;
    }

    // 这个函数带 const 尾缀，说的是"我不改任何成员"。
    // 代价是：它里面只能调同样带 const 的成员函数 ——
    // 这就是 Book 的 getTitle() / getIsbn() 必须加 const 的直接原因
    void displayRecords() const
    {
        if (m_borrownum == 0)
        {
            cout << "    " << m_name << " 暂无借阅记录。" << endl;
            return;
        }

        cout << "    " << m_name << " 的借阅清单（共 " << m_borrownum << " 本）:" << endl;
        for (int i = 0; i < m_borrownum; i++)
        {
            cout << "      " << (i + 1) << ". 《" << m_books[i].getTitle()
                 << "》  [" << m_books[i].getIsbn() << "]" << endl;
        }
    }

    int getBorrownum() const { return m_borrownum; }
};

// ============================================================
// 程序入口
// ============================================================
int main()
{
    SetConsoleOutputCP(65001);        // 让控制台按 UTF-8 显示中文

    // ========================================================
    // 第五节
    // ========================================================
    cout << "=== 第五节　const Book&：一个符号解决两个问题 ===" << endl;
    cout << endl;

    Book e1("C++ Primer", "Lippman", "ISBN-001", 128.0);

    cout << "--- 5.1 三种传参方式，看形参地址 ---" << endl;
    cout << "    外面那本书: 地址 " << &e1 << endl;
    showByValue(e1);                  // 拷贝一份进去
    showByRef(e1);                    // 本人，能改
    showByConstRef(e1);               // 本人，不许改
    cout << endl;

    cout << "--- 5.2 参数少写一个 const，手滑就改了数据 ---" << endl;
    Book e2("算法导论", "Cormen", "ISBN-002", 99.0);
    cout << "    调用前状态: " << e2.statusText() << endl;
    printButCanChange(e2);            // 这个函数只想打印，却把状态改了
    cout << "    调用后状态: " << e2.statusText() << "   ← 编译器全程没吭声" << endl;
    printSafely(e2);                  // 参数写成 const Book&，同样的手滑会当场编不过
    cout << endl;

    cout << "--- 5.3 六种 const 位置 ---" << endl;
    Book e3("位置演示书", "无名", "ISBN-POS", 66.0);
    cout << "    ① const Book&        —— 不许改对象" << endl;
    pos1_constRef(e3);
    cout << "    ② Book const&        —— 和 ① 是同一个类型" << endl;
    pos2_refConst(e3);
    cout << "    ③ const Book*        —— 不许改对象，可以换指向" << endl;
    pos3_constPtr(&e3);
    cout << "    ④ Book* const        —— 可以改对象，不许换指向" << endl;
    pos4_ptrConst(&e3);
    cout << "    ⑤ const Book* const  —— 两头都锁死" << endl;
    pos5_constPtrConst(&e3);
    cout << "    ⑥ 成员函数尾缀 const —— this 变成 const Book*" << endl;
    e3.whoAmI();
    cout << endl;

    cout << "--- 5.4 getter 加了 const，才能在 const 引用上调用 ---" << endl;
    printCard(e1);
    cout << "    sizeof(Book) = " << sizeof(Book)
         << " 字节 —— 值传递光对象本身就要拷这么多，里面的 string 还会各自再要堆内存" << endl;
    cout << endl;

    // ========================================================
    // 第六节
    // ========================================================
    cout << "=== 第六节　组合：一个类的成员是另一个类的对象 ===" << endl;
    cout << endl;

    cout << "--- 6.1 对象成员的构造 / 析构顺序 ---" << endl;
    cout << "    下面一对花括号是一个作用域，出右括号 o 就析构：" << endl;
    {
        Owner o;
    }
    cout << "    出了作用域，o 已经不在了。" << endl;
    cout << endl;

    cout << "--- 6.2 初始化列表：三类必须用它 ---" << endl;

    // 场景一
    BookWithConstIsbn cb("ISBN-CONST", "带 const 成员的书");
    cout << "    场景一 const 成员:      " << cb.getTitle()
         << " 的 ISBN 是 " << cb.getIsbn() << "（只能靠初始化列表定下）" << endl;

    // 场景二
    School sc("某某大学");
    StudentWithSchool sw(sc, "YG");
    cout << "    场景二 引用成员:       " << sw.describe() << "（绑死，换不了）" << endl;

    // 场景三
    NoDefaultHolder nh(42);
    cout << "    场景三 无默认构造成员: 值是 " << nh.getValue()
         << "（不写初始化列表就是 C2512）" << endl;
    cout << endl;

    cout << "--- 6.3 组合是抄一份，不是绑在一起 ---" << endl;
    cout << "    sizeof(Book)      = " << sizeof(Book) << endl;
    cout << "    sizeof(ShelfA)    = " << sizeof(ShelfA)
         << "（3 本书真住在里面，每本 " << sizeof(Book) << " 字节）" << endl;
    cout << "    sizeof(ShelfB)    = " << sizeof(ShelfB)
         << "（3 个指针，每个 8 字节，书还在外面）" << endl;

    ShelfA sa;
    sa.m_books[0] = Book("A0", "某人", "ISBN-A0", 10.0);
    sa.m_books[1] = Book("A1", "某人", "ISBN-A1", 10.0);
    sa.m_books[2] = Book("A2", "某人", "ISBN-A2", 10.0);
    cout << "    ShelfA 对象自己的地址   : " << &sa << endl;
    cout << "      sa.m_books[0] 的地址  : " << &sa.m_books[0] << "   ← 和上面一模一样" << endl;
    cout << "      sa.m_books[1] 的地址  : " << &sa.m_books[1] << endl;
    cout << "      [1] 和 [0] 差多少字节 : "
         << ((char*)&sa.m_books[1] - (char*)&sa.m_books[0]) << endl;

    Book outside("外面的书", "某人", "ISBN-OUT", 10.0);
    ShelfB sb;
    sb.m_books[0] = &outside;         // 只存地址，不抄内容
    cout << "    外面那本书的地址        : " << &outside << endl;
    cout << "      sb.m_books[0] 存的地址: " << sb.m_books[0] << "   ← 就是它" << endl;

    ShelfA sa2;
    sa2.m_books[0] = outside;         // 抄一份进去
    cout << "    改成抄一份进 ShelfA 后，比一比：" << endl;
    cout << "      指针那条 sb.m_books[0] 和 &outside 相等吗: "
         << (sb.m_books[0] == &outside ? "相等（指针存的同一本书的地址）" : "不相等") << endl;
    cout << "      抄来的 sa2.m_books[0] 地址和 &outside 相等吗: "
         << (&sa2.m_books[0] == &outside ? "相等" : "不相等（是另一个对象，只是内容一样）") << endl;
    cout << endl;

    // ========================================================
    // 第十节
    // ========================================================
    cout << "=== 第十节　动手任务 A / B / C 的参考实现 ===" << endl;
    cout << endl;

    // ---------- 任务 A ----------
    cout << "--- 任务 A　把 Book 塞进 Student 里（组合）---" << endl;
    Book a1("C++ Primer", "Lippman", "ISBN-001", 128.0);
    StudentA stuA("YG", "2024001", a1);
    stuA.display();
    cout << "    把外面的 a1 改成已借出，看 stuA 里那本变不变：" << endl;
    a1.setIsBorrowed(true);
    cout << "      外面 a1 的状态:      " << a1.statusText() << endl;
    cout << "      stuA 里记的那本:     " << stuA.getBorrowed().statusText()
         << "   ← 没跟着变，因为是两个独立对象" << endl;
    StudentA emptyStu;
    cout << "    默认构造出来的学生（里面的 m_borrowed 是一本空书）：" << endl;
    emptyStu.display();
    cout << endl;

    // ---------- 任务 B ----------
    cout << "--- 任务 B　借书 / 还书，参数传指针（依赖）---" << endl;
    Book bk1("C++ Primer", "Lippman", "ISBN-001", 128.0);
    Book bk2("算法导论", "Cormen", "ISBN-002", 99.0);
    Book bk3("计算机网络", "谢希仁", "ISBN-003", 79.0);

    StudentB stuB("YG", "2024001");
    stuB.display();
    cout << "    借第一本：应该成功" << endl;
    stuB.borrowBook(&bk1);
    stuB.display();
    cout << "    已经借了一本，再借第二本：应该失败" << endl;
    stuB.borrowBook(&bk2);
    cout << "    还一本他根本没借的书：应该失败" << endl;
    stuB.returnBook(&bk3);
    cout << "    还对了：应该成功" << endl;
    stuB.returnBook(&bk1);
    stuB.display();
    cout << "    借一本已被人借走的书：应该失败" << endl;
    Book bkBusy("被借走的书", "某人", "ISBN-009", 50.0);
    bkBusy.setIsBorrowed(true);
    stuB.borrowBook(&bkBusy);
    cout << "    传空指针：应该提示失败，而不是崩掉" << endl;
    stuB.borrowBook(nullptr);
    cout << "    值传递为什么不行：对比两个地址" << endl;
    Book bkTest("测试书", "无名", "ISBN-TEST", 10.0);
    cout << "      main 里 bkTest 的地址:  " << &bkTest << endl;
    cout << "      调用前状态:              " << bkTest.statusText() << endl;
    borrowByValue(bkTest);
    cout << "      调用后状态:              " << bkTest.statusText() << "   ← 没变" << endl;
    cout << "    改成指针传：" << endl;
    Book bkTest2("测试书2", "无名", "ISBN-TEST2", 10.0);
    cout << "      main 里 bkTest2 的地址: " << &bkTest2 << endl;
    cout << "      调用前状态:              " << bkTest2.statusText() << endl;
    borrowByPointer(&bkTest2);
    cout << "      调用后状态:              " << bkTest2.statusText() << "   ← 变了" << endl;
    cout << endl;

    // ---------- 任务 C ----------
    cout << "--- 任务 C　对象数组 + 借还 + 还书校验（作业真需求）---" << endl;
    Book cpp("C++ Primer", "Lippman", "ISBN-001", 128.0);
    Book algo("算法导论", "Cormen", "ISBN-002", 99.0);
    Book net("计算机网络", "谢希仁", "ISBN-003", 79.0);

    StudentC stuC("YG", "2024001");
    cout << "    借两本：都应该成功" << endl;
    stuC.borrowBook(&cpp);
    stuC.borrowBook(&algo);
    cout << "    同一本再借一次：应该失败" << endl;
    stuC.borrowBook(&cpp);
    stuC.displayRecords();
    cout << "    借了 A 却想还 B：应该失败（本节最重要的一道校验）" << endl;
    stuC.returnBook(&net);
    cout << "    还 C++ Primer：应该成功，清单少一本" << endl;
    stuC.returnBook(&cpp);
    stuC.displayRecords();
    cout << "    还完再借计算机网络：应该成功" << endl;
    stuC.borrowBook(&net);
    stuC.displayRecords();
    cout << "    三本书的真实状态" << endl;
    cpp.display();
    algo.display();
    net.display();
    cout << "    借阅上限：再借 8 本，应该全成功（正好到 10）" << endl;
    Book pool[8] = {
        Book("填充书 1", "某人", "ISBN-F01", 1.0),
        Book("填充书 2", "某人", "ISBN-F02", 1.0),
        Book("填充书 3", "某人", "ISBN-F03", 1.0),
        Book("填充书 4", "某人", "ISBN-F04", 1.0),
        Book("填充书 5", "某人", "ISBN-F05", 1.0),
        Book("填充书 6", "某人", "ISBN-F06", 1.0),
        Book("填充书 7", "某人", "ISBN-F07", 1.0),
        Book("填充书 8", "某人", "ISBN-F08", 1.0)
    };
    for (int i = 0; i < 8; i++)
    {
        stuC.borrowBook(&pool[i]);
    }
    cout << "    现在共借了 " << stuC.getBorrownum() << " 本" << endl;
    cout << "    已经到上限，再借第 11 本：应该失败" << endl;
    Book over("第 11 本", "某人", "ISBN-OVER", 1.0);
    stuC.borrowBook(&over);
    cout << endl;

    // ========================================================
    // 自测
    // ========================================================
    cout << "=== 你现在应该能回答的三个问题 ===" << endl;
    cout << endl;
    cout << "  1. 5.1 里 showByValue 打印的形参地址和外面那本书不一样，原因是什么？" << endl;
    cout << "     把它的参数从 Book 改成 Book& 之后，为什么连函数体都不改就能编译通过？" << endl;
    cout << endl;
    cout << "  2. 6.1 里 Owner 的构造体开始执行之前，m_a / m_b 已经构造好了吗？" << endl;
    cout << "     如果把初始化列表写成 : m_b(\"m_b\"), m_a(\"m_a\")，输出顺序会变吗？" << endl;
    cout << endl;
    cout << "  3. 任务 C 的 returnBook 里，为什么必须先 for 循环找到 pos 才动手删？" << endl;
    cout << "     如果跳过这道校验直接 m_borrownum--，那么 stuC.returnBook(&net)" << endl;
    cout << "     会打印\"还书成功\"，接下来清单里剩下的会是哪几本？" << endl;
    cout << endl;

    return 0;      // 0 = 正常结束
}
