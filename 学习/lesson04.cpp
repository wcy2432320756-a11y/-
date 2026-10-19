// ============================================================
// 第四课练习：多态与虚函数
// 对应作业：实验四 多态与虚函数
// 编译运行： .\build.ps1 lesson04
//
// 这份文件里有五段，按顺序跑完你就把这一章的重点全踩了一遍：
//   第一段 A：基类 display 前面【没有】virtual —— 看它怎么"失灵"
//   第一段 B：同一个例子，只加了 virtual —— 看它怎么"生效"
//   第二段：  虚析构 —— delete 基类指针时子类析构会不会执行
//   第三段：  对象切片 —— BookResource b = magazine; 为什么多态失效
//   第四段：  虚函数表的代价 —— sizeof 一个带虚函数的类多了几字节
//   第五段：  作业正题 —— vector<BookResource*> 混装三种资源 + 菜单
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>          // 只为了用 SetConsoleOutputCP，让中文不乱码
using namespace std;

// 一个小工具：统计"我 new 出来、但还没被 delete 掉"的字节数
// 它的作用只有一个 —— 用数字证明析构函数到底有没有执行
int g_leakedBytes = 0;


// ============================================================
// 第一段 A：基类的 display 前面【没有】virtual
// ============================================================
// 这是我们想当然以为"能行"的写法。三分钟之后你会看到它为什么不行。

class BaseNoVirtual
{
protected:
    // 【新词】protected（保护）：
    //   private  —— 只有本类自己的成员函数能碰
    //   public   —— 谁都能碰
    //   protected —— 本类 + 它的派生类能碰，外面的 main 碰不到
    // 我们需要子类 BookNoVirtual 能读到 m_title，所以这里用 protected
    string m_title;

public:
    // 【新词】初始化列表：构造函数名后面跟冒号，冒号后面才是函数体
    //   BaseNoVirtual(string title) : m_title(title) { }
    //   ↑ 意思是"在函数体执行之前，先用 title 给 m_title 赋初值"
    // 这节课用它，是因为子类调基类的带参构造【只能】这么写，没有第二种写法
    BaseNoVirtual(string title) : m_title(title) { }

    // 注意：这里是普通函数，前面【没有】virtual
    void display()
    {
        cout << "  [BaseNoVirtual::display]    标题: " << m_title << endl;
    }
};

class BookNoVirtual : public BaseNoVirtual
{
private:
    string m_author;

public:
    // 冒号后面写 BaseNoVirtual(title) = 把 title 交给基类的构造函数去初始化
    // 派生类的构造函数，一定要先让基类那半边先造好
    BookNoVirtual(string title, string author) : BaseNoVirtual(title)
    {
        m_author = author;
    }

    void display()
    {
        cout << "  [BookNoVirtual::display]    书名: " << m_title
             << " / 作者: " << m_author << endl;
    }
};

class MagazineNoVirtual : public BaseNoVirtual
{
private:
    int m_year;
    int m_issue;

public:
    MagazineNoVirtual(string title, int year, int issue) : BaseNoVirtual(title)
    {
        m_year = year;
        m_issue = issue;
    }

    void display()
    {
        cout << "  [MagazineNoVirtual::display] 刊名: " << m_title
             << " / " << m_year << " 年第 " << m_issue << " 期" << endl;
    }
};


// ============================================================
// 第一段 B：一模一样的东西，只给基类的 display 加了 virtual
// ============================================================
// 除了 virtual 这个关键字，其余的类名、成员、函数体结构全部照抄上面。
// 这样你才能确定：结果不一样，原因只可能是 virtual。

class BaseVirtual
{
protected:
    string m_title;

public:
    BaseVirtual(string title) : m_title(title) { }

    // ↓↓↓ 就是这一个词 ↓↓↓
    virtual void display()
    {
        cout << "  [BaseVirtual::display]    标题: " << m_title << endl;
    }

    // 【重点】只要一个类里有 virtual 函数，它的析构函数就必须写 virtual
    // 为什么，第二段专门演示
    virtual ~BaseVirtual() { }
};

class BookVirtual : public BaseVirtual
{
private:
    string m_author;

public:
    BookVirtual(string title, string author) : BaseVirtual(title)
    {
        m_author = author;
    }

    // 【新词】override：写在函数参数列表后面，告诉编译器
    //   "我这个函数是故意要重写基类那个虚函数的，你帮我检查有没有写错"
    // 如果基类没有同名同参数同 const 的虚函数，编译器会当场报错 C3668
    // 建议：凡是你想重写的虚函数，后面都加上 override，能挡掉一大半低级错误
    void display() override
    {
        cout << "  [BookVirtual::display]    书名: " << m_title
             << " / 作者: " << m_author << endl;
    }

    ~BookVirtual() override { }
};

class MagazineVirtual : public BaseVirtual
{
private:
    int m_year;
    int m_issue;

public:
    MagazineVirtual(string title, int year, int issue) : BaseVirtual(title)
    {
        m_year = year;
        m_issue = issue;
    }

    void display() override
    {
        cout << "  [MagazineVirtual::display] 刊名: " << m_title
             << " / " << m_year << " 年第 " << m_issue << " 期" << endl;
    }

    ~MagazineVirtual() override { }
};


// ============================================================
// 第二段：虚析构 —— 基类析构函数不写 virtual 会发生什么
// ============================================================
// 先说结论：不会报错，不会崩溃，编译器和运行期都不吭声。
// 但子类的析构函数【根本不会被调用】。子类在析构函数里该做的事全都没做。
// 下面用两个类现场证明：一个基类析构没写 virtual，一个写了。

// ---- 错的版本：基类析构前面没有 virtual ----
class BaseNoVDtor
{
public:
    BaseNoVDtor() { }
    ~BaseNoVDtor()
    {
        cout << "    基类析构执行 BaseNoVDtor::~BaseNoVDtor()" << endl;
    }
};

class DerivedNoVDtor : public BaseNoVDtor
{
private:
    int* m_payload;      // 子类自己 new 出来的一块内存

public:
    DerivedNoVDtor()
    {
        // 256 个 int = 1024 字节
        m_payload = new int[256];
        g_leakedBytes += 1024;
        cout << "    子类构造执行：申请 1024 字节，当前未归还 "
             << g_leakedBytes << " 字节" << endl;
    }

    ~DerivedNoVDtor()
    {
        delete[] m_payload;              // ← 本该在这里归还内存
        g_leakedBytes -= 1024;
        cout << "    子类析构执行：归还 1024 字节，当前未归还 "
             << g_leakedBytes << " 字节" << endl;
    }
};

// ---- 对的版本：基类析构前面加了 virtual ----
class BaseVDtor
{
public:
    BaseVDtor() { }
    virtual ~BaseVDtor()
    {
        cout << "    基类析构执行 BaseVDtor::~BaseVDtor()" << endl;
    }
};

class DerivedVDtor : public BaseVDtor
{
private:
    int* m_payload;

public:
    DerivedVDtor()
    {
        m_payload = new int[256];
        g_leakedBytes += 1024;
        cout << "    子类构造执行：申请 1024 字节，当前未归还 "
             << g_leakedBytes << " 字节" << endl;
    }

    ~DerivedVDtor() override
    {
        delete[] m_payload;
        g_leakedBytes -= 1024;
        cout << "    子类析构执行：归还 1024 字节，当前未归还 "
             << g_leakedBytes << " 字节" << endl;
    }
};


// ============================================================
// 第四段用：两个类，一个没有虚函数，一个有
// 只为了用 sizeof 量一下"多态要花多少字节"
// ============================================================
class NoVTable
{
public:
    int m_a;
};

class HasVTable
{
public:
    virtual void hello() { }     // 有虚函数，对象里就会多藏一个指针
    int m_a;
};


// ============================================================
// 第五段：作业正题
// BookResource 基类 + 三个子类 + Library 组合类
// ============================================================

// ---- 基类：抽象类 ----
// 抽象类的意思：它只负责规定"所有资源都必须有哪些能力"，
// 具体怎么干活交给子类。它自己不能直接造对象。
class BookResource
{
protected:
    string m_id;             // 资源编号，三类资源都有
    string m_title;          // 资源名称
    bool   m_isBorrowed;     // 是否已借出

public:
    BookResource(string id, string title)
    {
        m_id = id;
        m_title = title;
        m_isBorrowed = false;
    }

    // 基类的析构必须是 virtual —— 因为我们一定会 delete 基类指针
    virtual ~BookResource() { }

    // 普通的读写函数。注意后面的 const：
    //   【新词】const 成员函数：写在参数列表后面，表示"我保证不修改本对象的成员"
    //   好处是 const 对象 / const 指针也能调用它。第二课讲过 const 引用，这里是同一件事
    string getId() const { return m_id; }
    string getTitle() const { return m_title; }
    bool getIsBorrowed() const { return m_isBorrowed; }
    void setIsBorrowed(bool b) { m_isBorrowed = b; }

    // 【重点】纯虚函数 = 0
    //   结尾写 = 0 意思是"我这里只留个位置，不写实现，谁继承我谁必须自己写"
    //   一个类只要含有纯虚函数，它就是抽象类，不能 new、不能定义对象
    virtual void display() const = 0;
    virtual string getTypeName() const = 0;
    virtual double getFinePerDay() const = 0;

    // 普通虚函数：有实现，子类可以重写，也可以不管它直接继承这个版本
    // 这里演示"基类写通用逻辑，内部再用纯虚函数去问子类要差异化数据"
    virtual double calcFine(int overdueDays) const
    {
        if (overdueDays <= 0)
        {
            return 0.0;
        }
        return overdueDays * getFinePerDay();
    }
};

// ---- 子类 1：图书 ----
class Book : public BookResource
{
private:
    string m_author;
    string m_isbn;
    int    m_pages;

public:
    Book(string id, string title, string author, string isbn, int pages)
        : BookResource(id, title)
    {
        m_author = author;
        m_isbn = isbn;
        m_pages = pages;
    }

    void display() const override
    {
        cout << "【图书】" << m_id << " | 《" << m_title << "》"
             << " | 作者 " << m_author
             << " | ISBN " << m_isbn
             << " | " << m_pages << " 页"
             << " | " << (m_isBorrowed ? "已借出" : "可借") << endl;
    }

    string getTypeName() const override { return "图书"; }
    double getFinePerDay() const override { return 0.2; }
};

// ---- 子类 2：杂志 ----
class Magazine : public BookResource
{
private:
    int    m_year;
    int    m_issue;
    string m_period;

public:
    Magazine(string id, string title, int year, int issue, string period)
        : BookResource(id, title)
    {
        m_year = year;
        m_issue = issue;
        m_period = period;
    }

    void display() const override
    {
        cout << "【杂志】" << m_id << " | 《" << m_title << "》"
             << " | " << m_year << " 年第 " << m_issue << " 期"
             << " | " << m_period
             << " | " << (m_isBorrowed ? "已借出" : "可借") << endl;
    }

    string getTypeName() const override { return "杂志"; }
    double getFinePerDay() const override { return 0.5; }
};

// ---- 子类 3：电子资源 ----
class EResource : public BookResource
{
private:
    string m_url;
    int    m_maxOnline;

public:
    EResource(string id, string title, string url, int maxOnline)
        : BookResource(id, title)
    {
        m_url = url;
        m_maxOnline = maxOnline;
    }

    void display() const override
    {
        cout << "【电子】" << m_id << " | 《" << m_title << "》"
             << " | " << m_url
             << " | 同时在线上限 " << m_maxOnline
             << " | " << (m_isBorrowed ? "已借出" : "可借") << endl;
    }

    string getTypeName() const override { return "电子资源"; }

    // 电子资源不涉及逾期罚款，日罚金就是 0
    double getFinePerDay() const override { return 0.0; }
};

// ---- 组合类：图书馆 ----
// 【新词】组合：一个类的成员变量里，装着另一类东西。
// 这里 Library "拥有" 一批 BookResource 指针，所以 Library 必须负责它们的生死。
class Library
{
private:
    // 为什么装指针不装对象：
    //   装对象 vector<BookResource> —— 装不下，基类是抽象类，造不出对象
    //   就算基类不抽象，装对象也会切片（第三段演示）
    //   所以只能装指针：vector<BookResource*>
    vector<BookResource*> m_items;

public:
    Library() { }

    // 【重点】析构函数里必须把每个 new 出来的对象 delete 掉
    // 谁 new 谁 delete。这里是 Library 的 add() 接手的对象，
    // 于是由 Library 的析构函数统一收尾。漏了这一步就是内存泄漏
    ~Library()
    {
        int n = (int)m_items.size();     // 先记住数量，因为下面 clear 之后这个数就变 0 了
        for (int i = 0; i < n; i++)
        {
            delete m_items[i];           // 因为有 virtual 析构，三种子类的析构都会执行
        }
        m_items.clear();
        cout << "  （Library 析构：已 delete 全部 " << n << " 项资源，容器清空）" << endl;
    }

    // 【进阶，看不懂可以先跳过】
    // = delete 表示"这个函数禁止被调用"。为什么要禁？因为 Library 一旦被复制，
    // 两个对象里存的是同一批指针，各自的析构函数都会 delete 一遍 —— 同一块内存被删两次，
    // 程序直接崩。第五课讲 vector 时会再回来讲这条。
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;

    // 【重点】add 接收的是 new 出来的指针，所有权转移给 Library
    void add(BookResource* r)
    {
        if (r != nullptr)
        {
            m_items.push_back(r);
        }
    }

    int size() const { return (int)m_items.size(); }

    // 一趟循环，三种资源各自显示各的 —— 这就是多态的价值
    void showAll() const
    {
        cout << "书库共 " << m_items.size() << " 件资源：" << endl;
        for (int i = 0; i < (int)m_items.size(); i++)
        {
            cout << " 第 " << (i + 1) << " 项 ";
            m_items[i]->display();      // ← 全篇最重要的一行
        }
    }

    // 多态不只用来打印。同一趟循环里，三种资源按各自费率算罚款
    void showFineReport(int overdueDays) const
    {
        double total = 0.0;
        cout << "逾期 " << overdueDays << " 天的罚款明细：" << endl;
        for (int i = 0; i < (int)m_items.size(); i++)
        {
            double fine = m_items[i]->calcFine(overdueDays);
            total += fine;
            cout << "  " << m_items[i]->getTypeName()
                 << " 《" << m_items[i]->getTitle() << "》"
                 << " 日罚 " << m_items[i]->getFinePerDay() << " 元"
                 << " → " << fine << " 元" << endl;
        }
        cout << "  合计 " << total << " 元" << endl;
    }

    int countByType(const string& typeName) const
    {
        int n = 0;
        for (int i = 0; i < (int)m_items.size(); i++)
        {
            if (m_items[i]->getTypeName() == typeName)
            {
                n++;
            }
        }
        return n;
    }

    BookResource* findById(const string& id) const
    {
        for (int i = 0; i < (int)m_items.size(); i++)
        {
            if (m_items[i]->getId() == id)
            {
                return m_items[i];
            }
        }
        return nullptr;          // 【新词】nullptr = 空指针，表示"什么都没有"
    }
};


// ============================================================
// 小工具：读一个整数（和项目 LibrarySys.cpp 里的 readInt 一样）
// ============================================================
int readInt(string tip)
{
    int value = 0;
    cout << tip;
    if (!(cin >> value))
    {
        cin.clear();
        cin.ignore(1024, '\n');
        cout << "输入格式不正确，请输入数字" << endl;
        return -1;
    }
    cin.ignore(1024, '\n');
    return value;
}


// ============================================================
// 第一段 A 的演示函数
// ============================================================
void demoNoVirtual()
{
    cout << "\n【第一段 A】基类的 display 前面【没有】virtual" << endl;
    cout << "------------------------------------------------------------" << endl;

    BookNoVirtual     book("C++ Primer", "Stanley Lippman");
    MagazineNoVirtual mag("读者", 2026, 7);

    cout << "(1) 通过对象本身调用：" << endl;
    book.display();
    mag.display();

    cout << "(2) 同样的两个对象，改用基类指针调用：" << endl;
    // 【新词】类型兼容：派生类对象，可以拿基类指针来指。
    // 因为"一本图书"本来就是"一种资源"，反过来不成立
    BaseNoVirtual* p1 = &book;
    BaseNoVirtual* p2 = &mag;
    p1->display();
    p2->display();

    cout << "结论：同一个对象，(1) 打的是子类版本，(2) 打的是基类版本。" << endl;
    cout << "      指针明明指着 BookNoVirtual，却去执行了 BaseNoVirtual::display。" << endl;
}


// ============================================================
// 第一段 B 的演示函数（结构和上面完全一样）
// ============================================================
void demoVirtual()
{
    cout << "\n【第一段 B】一模一样的东西，只给基类的 display 加了 virtual" << endl;
    cout << "------------------------------------------------------------" << endl;

    BookVirtual     book("C++ Primer", "Stanley Lippman");
    MagazineVirtual mag("读者", 2026, 7);

    cout << "(1) 通过对象本身调用：" << endl;
    book.display();
    mag.display();

    cout << "(2) 同样的两个对象，改用基类指针调用：" << endl;
    BaseVirtual* p1 = &book;
    BaseVirtual* p2 = &mag;
    p1->display();
    p2->display();

    cout << "结论：(1) 和 (2) 的输出一模一样，都走了子类版本。" << endl;
    cout << "      唯一的变化就是基类那个 display 前面多了 virtual。" << endl;
}


// ============================================================
// 第二段：虚析构演示
// ============================================================
void demoVirtualDtor()
{
    cout << "\n【第二段】虚析构：delete 基类指针时，子类析构函数会不会执行" << endl;
    cout << "------------------------------------------------------------" << endl;

    // 计分板：比较"这一段开始时"和"这一段结束时"的未归还字节数，
    // 差多少，就是这一段里漏掉多少
    int before = 0;
    int leaked = 0;

    cout << "(1) 基类析构【没有】virtual：" << endl;
    before = g_leakedBytes;
    BaseNoVDtor* bad = new DerivedNoVDtor();
    delete bad;
    leaked = g_leakedBytes - before;
    cout << "    delete 之后：本段泄漏 " << leaked << " 字节（本该是 0）" << endl;

    cout << "\n(2) 基类析构【加了】virtual：" << endl;
    before = g_leakedBytes;
    BaseVDtor* good = new DerivedVDtor();
    delete good;
    leaked = g_leakedBytes - before;
    cout << "    delete 之后：本段泄漏 " << leaked << " 字节" << endl;

    cout << "\n结论：(1) 只打印了基类析构那一行，子类析构【一次都没执行】，" << endl;
    cout << "      子类里 new 出来的 1024 字节永远拿不回来。" << endl;
    cout << "      (2) 子类析构先跑、基类析构后跑，本段泄漏 0 字节。" << endl;
    cout << "      两次 delete 都没有任何报错、没有任何警告 —— 这才是虚析构最坑的地方。" << endl;
}


// ============================================================
// 第三段要用：按值传参的切片
// 形参写成 BaseVirtual 而不是 BaseVirtual&，每调用一次就复制一个纯基类对象出来
// ============================================================
void showByValue(BaseVirtual r)
{
    r.display();
}


// ============================================================
// 第三段：对象切片
// ============================================================
void demoSlicing()
{
    cout << "\n【第三段】对象切片：为什么多态会在这里失效" << endl;
    cout << "------------------------------------------------------------" << endl;

    cout << "sizeof(BaseVirtual) = " << sizeof(BaseVirtual) << " 字节" << endl;
    cout << "sizeof(BookVirtual) = " << sizeof(BookVirtual) << " 字节"
         << "（多出来的部分是 m_author）" << endl;

    BookVirtual real("C++ Primer", "Stanley Lippman");

    cout << "\n把子类对象【赋值】给基类对象：" << endl;
    cout << "  BaseVirtual sliced = real;" << endl;
    // ↑ 这一行做的事：拿 real 的基类那部分，去调用 BaseVirtual 的拷贝构造函数，
    //   造一个全新的、纯粹是 BaseVirtual 的对象 sliced。
    //   子类多出来的 m_author、以及"我是 BookVirtual"这个身份，全部被切掉
    BaseVirtual sliced = real;

    cout << "\n通过原对象 real 调用 display()：" << endl;
    real.display();

    cout << "通过切片对象 sliced 调用 display()：" << endl;
    sliced.display();

    cout << "\n结论：基类的 display 明明是 virtual，sliced 还是走了基类版本。" << endl;
    cout << "      因为 sliced 的真实类型就是 BaseVirtual，它肚子里已经没有 BookVirtual 了。" << endl;
    cout << "      虚函数救不了切片 —— 对象在被切的那一刻，子类部分就已经不存在了。" << endl;

    // 顺带看一个更容易踩的：按值传参也会切片
    cout << "\n顺带看一种更隐蔽的切片 —— 按值传参：" << endl;
    cout << "  void showByValue(BaseVirtual r) { r.display(); }" << endl;
    cout << "  现在把 real 传进去，看看它打什么：" << endl;
    cout << "  ";
    showByValue(real);
    cout << "  ↑ 还是基类版本。因为形参 r 是当场复制出来的纯 BaseVirtual 对象，" << endl;
    cout << "    子类那半截在传参的那一瞬间就被切掉了。" << endl;
    cout << "  改法：形参写成 BaseVirtual& r（引用），传的就是 real 本人，不复制，不切片。" << endl;
}


// ============================================================
// 第四段：多态不是免费的
// ============================================================
void demoVTableCost()
{
    cout << "\n【第四段】多态的代价：每个对象多藏了一个指针" << endl;
    cout << "------------------------------------------------------------" << endl;

    cout << "sizeof(NoVTable)  = " << sizeof(NoVTable)
         << " 字节（只有一个 int）" << endl;
    cout << "sizeof(HasVTable) = " << sizeof(HasVTable)
         << " 字节（int + 一个隐藏的虚函数表指针）" << endl;

    cout << "\n这个隐藏指针叫 vptr，指向本类共用的那张【虚函数表】。" << endl;
    cout << "对象一被造出来，vptr 就被设成'我这个类该用哪张表'。" << endl;
    cout << "所以通过基类指针 p->display() 时，程序不是去看 p 的类型，" << endl;
    cout << "而是顺着 vptr 找到表、按表里的地址跳过去 —— 这才叫'运行时决定'。" << endl;
    cout << "代价就是每个对象多 8 字节（64 位），外加一次间接跳转。" << endl;
}


// ============================================================
// 第五段：作业正题，附带一个和你项目同款的菜单
// ============================================================
void demoProjectSystem()
{
    cout << "\n【第五段】作业正题：vector<BookResource*> 混装三种资源" << endl;
    cout << "------------------------------------------------------------" << endl;

    Library lib;

    // 注意：这里 new 出来的对象，所有权交给 lib
    // 不要在 main 里再 delete 它们 —— 那是 double delete，程序会崩
    lib.add(new Book("B001", "C++ Primer", "Stanley Lippman", "9787121358579", 976));
    lib.add(new Book("B002", "Effective C++", "Scott Meyers", "9787121155887", 320));
    lib.add(new Magazine("M001", "读者", 2026, 7, "月刊"));
    lib.add(new Magazine("M002", "国家地理", 2026, 3, "月刊"));
    lib.add(new EResource("E001", "中国知网论文库", "https://www.cnki.net", 50));

    cout << "往一个 vector 里塞了 5 个 BookResource*，实际指向 2 本书 + 2 本杂志 + 1 个电子资源。" << endl;

    cout << "\n--- 一次循环，各显示各的 ---" << endl;
    lib.showAll();

    cout << "\n--- 按类型统计 ---" << endl;
    cout << "图书 " << lib.countByType("图书") << " 件，"
         << "杂志 " << lib.countByType("杂志") << " 件，"
         << "电子资源 " << lib.countByType("电子资源") << " 件" << endl;

    cout << "\n--- 同一趟循环，按各自费率算罚款 ---" << endl;
    lib.showFineReport(7);

    cout << "\n--- 把其中一本标成已借出，再显示一次 ---" << endl;
    BookResource* found = lib.findById("B002");
    if (found != nullptr)
    {
        found->setIsBorrowed(true);
        found->display();
    }

    cout << "\n--- demoProjectSystem 即将结束，lib 出作用域，析构函数开始跑 ---" << endl;
}


// ============================================================
// 第五段的菜单版：和你项目 LibrarySys.cpp 的菜单是同一个套路
// 想退出输 0
// ============================================================
void runMenu()
{
    cout << "\n【第五段 · 菜单版】输入 0 退出" << endl;
    cout << "------------------------------------------------------------" << endl;

    Library lib;
    lib.add(new Book("B001", "C++ Primer", "Stanley Lippman", "9787121358579", 976));
    lib.add(new Book("B002", "Effective C++", "Scott Meyers", "9787121155887", 320));
    lib.add(new Magazine("M001", "读者", 2026, 7, "月刊"));
    lib.add(new EResource("E001", "中国知网论文库", "https://www.cnki.net", 50));

    bool running = true;
    while (running)
    {
        cout << "\n======== 多态资源管理器 ========" << endl;
        cout << "1. 显示全部资源" << endl;
        cout << "2. 按类型统计数量" << endl;
        cout << "3. 逾期罚款明细" << endl;
        cout << "4. 按编号查找资源" << endl;
        cout << "0. 退出" << endl;

        int choice = readInt("请选择: ");

        // readInt 返回 -1 表示"读到的东西不是数字"或"输入流结束了"
        // 这种情况下直接退出菜单，否则会死循环刷屏
        if (choice < 0)
        {
            cout << "（输入结束或格式错误，菜单自动退出）" << endl;
            break;
        }

        switch (choice)
        {
        case 1:
            lib.showAll();
            break;

        case 2:
            cout << "图书 " << lib.countByType("图书") << " 件，"
                 << "杂志 " << lib.countByType("杂志") << " 件，"
                 << "电子资源 " << lib.countByType("电子资源") << " 件" << endl;
            break;

        case 3:
        {
            int days = readInt("请输入逾期天数: ");
            if (days < 0)
            {
                cout << "天数不合法" << endl;
                break;
            }
            lib.showFineReport(days);
            break;
        }

        case 4:
        {
            cout << "请输入编号: ";
            string id;
            if (!getline(cin, id) || id.empty())
            {
                cout << "编号不能为空" << endl;
                break;
            }
            BookResource* r = lib.findById(id);
            if (r == nullptr)
            {
                cout << "没有找到编号为 " << id << " 的资源" << endl;
            }
            else
            {
                r->display();
            }
            break;
        }

        case 0:
            cout << "退出菜单。lib 即将析构，会把 4 个资源全部 delete。" << endl;
            running = false;
            break;

        default:
            cout << "输入无效，请重新选择。" << endl;
            break;
        }
    }
}


// ============================================================
// 程序入口
// ============================================================
int main()
{
    SetConsoleOutputCP(65001);        // 让控制台按 UTF-8 显示中文

    cout << "============================================================" << endl;
    cout << "第四课　多态与虚函数" << endl;
    cout << "============================================================" << endl;

    demoNoVirtual();        // 第一段 A：没有 virtual 会怎样
    demoVirtual();          // 第一段 B：加了 virtual 会怎样
    demoVirtualDtor();      // 第二段：虚析构
    demoSlicing();          // 第三段：对象切片
    demoVTableCost();       // 第四段：虚函数表的代价
    demoProjectSystem();    // 第五段：作业正题
    runMenu();              // 第五段菜单版，可输 0 退出

    cout << "\n全部演示结束。g_leakedBytes 最终 = " << g_leakedBytes << " 字节。" << endl;
    cout << "这 " << g_leakedBytes << " 字节就是第二段 (1) 漏掉的那一块 —— " << endl;
    cout << "从头到尾没有任何报错、没有崩溃、没有任何提示。这就是虚析构必须写的原因。" << endl;
    return 0;      // 0 = 正常结束
}
