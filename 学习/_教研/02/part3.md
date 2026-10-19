## 五、`const Book&`：一个符号解决两个问题

### 5.1 需求不是凭空来的：一个 `printBook` 的两难

你在作业里迟早要写一个"把书打印出来"的函数。照着第一课的写法，你会先写成这样：

```cpp
void printBook(Book b)          // 传值
{
    b.display();
}
```

能跑。但它有个你看不见的代价：**每调一次，整本书被完整拷贝一份**。`Book` 里现在有 4 个 `string` 和 1 个 `double`，在你这台机器的 MSVC 上：

```text
sizeof(Book) = 112 字节
```

也就是说 `printBook(b1)` 这一行，光对象本体就搬了 112 字节 —— 还不算 4 个 `string` 各自在堆上申请的内存。打印一本书根本不需要副本，你要的是"看"。

那改成引用：

```cpp
void printBook(Book& b)         // 引用，不拷贝了
{
    b.display();
}
```

不拷贝了，可新的问题来了：**这个函数现在能改你的书，而且编译器一句话都不会说**。

```cpp
void printBook(Book& b)
{
    b.setIsBorrowed(true);      // 谁写的？为什么？没人拦
    b.display();
}
```

调用方传进来的是一本自己的书，打印完发现状态被改了。

于是需求变成一句话，两个要求同时成立：

> 我不要拷贝，我也不许你改。

C++ 用一个符号同时办这两件事 —— 在引用前面加 `const`：

```cpp
void printBook(const Book& b)   // 不拷贝 + 不许改
{
    b.display();
}
```

`const Book&` 拆开读：`Book&` 是"就是外面那一本，不是副本"（解决拷贝），`const` 是"只能读，不能写"（解决误改）。一个形状，两个约束。

看程序里跑的地址对比（`lesson02.cpp` 5.1 段的真实输出）：

```text
外面那本书: 地址 000000286A5DE320
传值      : 形参地址 000000286A5DDF78  ← 和外面不是同一个对象
引用      : 形参地址 000000286A5DE320  ← 就是外面那一个
const引用 : 形参地址 000000286A5DE320  ← 也是外面那一个
```

第二行那个不一样地址就是拷贝出来的副本 —— 函数返回，它就被销毁了，你白拷了一次。第三、四行地址和外面完全相同：没有第二本书存在，只有同一个对象的不同叫法。

### 5.2 少写一个 `const` 是什么后果

同一个函数，只差一个 `const`：

```cpp
void printButCanChange(Book& b)     // 少写 const
{
    b.setIsBorrowed(true);          // 编译器：完全合法
    cout << "打印: 《" << b.getTitle() << "》" << endl;
}
void printSafely(const Book& b)     // 写了 const
{
    b.setIsBorrowed(true);          // 编译不过，下面 5.4 详说
}
```

真实运行结果：

```text
调用前状态: 可借
打印: 《算法导论》
调用后状态: 已借出   ← 编译器全程没吭声
```

注意最后那行注释。这里没有任何警告 —— 不是编译器漏了，是**你把权限给出去了**。`Book&` 的语义就是"给你改"，编译器没理由拦。`const` 的作用不是让程序更安全这种空话，它的作用是**在编译期把这类手滑挡下来**：只要你写了 `const`，那行 `setIsBorrowed` 就过不了编译，你当天就会发现，而不是等到答辩演示时数据莫名其妙变了。

### 5.3 `const` 的六种位置：一张表拆开讲

`const` 出现在哪个位置，锁的东西就不一样。这是本章最容易糊的地方，一次列清。

| 写法 | 谁被锁住 | 一句话人话 |
|------|----------|-----------|
| `const Book&` | 被引用的那个对象 | 我看这本书，但我保证不涂改它 |
| `Book const&` | 同上，**完全同一个类型** | 和上一行一模一样，只是把 `const` 挪到类型名右边 |
| `const Book*` | 指针指向的对象 | 我拿着这本书的地址，书不许改，但地址我可以换成别的书 |
| `Book* const` | 指针自己 | 地址焊死了，只能指这本，但这本书的内容我可以改 |
| `const Book* const` | 对象 + 指针 | 地址不许换，书也不许改，两头都锁 |
| `void f() const`（成员函数尾缀） | 这个函数里的 `this` | 这个函数承诺不改自己任何成员 |

判断方法只有一条，别背表：

> `const` 修饰的是**它左边紧挨着的那个东西**；左边没东西，才修饰右边。

- `const Book*`：`const` 左边没东西（在行首），所以修饰右边 `Book` → 书不许改。
- `Book const*`：`const` 左边是 `Book` → 修饰 `Book` → 和上一行**一样**。
- `Book* const`：`const` 左边是 `*` → 修饰指针自己 → 地址不许改。

六种写法在程序里都跑了一遍，真实输出：

```text
① const Book&        —— 不许改对象
    读到: 位置演示书
② Book const&        —— 和 ① 是同一个类型
    读到: 位置演示书（写法不同，类型完全相同）
③ const Book*        —— 不许改对象，可以换指向
    读到: 位置演示书
    指针已改成空: 是
④ Book* const        —— 可以改对象，不许换指向
    读到: 位置演示书
    价格已改成: 55
⑤ const Book* const  —— 两头都锁死
    读到: 位置演示书（数据和指向都不许动）
⑥ 成员函数尾缀 const —— this 变成 const Book*
    我是《位置演示书》，状态 可借（这个函数承诺不改任何成员）
```

三条要你自己验证过才算真懂：

**第一，`const Book&` 和 `Book const&` 是同一个类型，不是两个。** 别信"只是写法不同"这种口头说法，用编译器验：

```cpp
class Printer
{
public:
    void f(const Book& b);
    void f(Book const& b);      // 我以为这是重载
};
```

```text
error C2535: "void Printer::f(const Book &)": 已经定义或声明成员函数
note: 参见"Printer::f"的声明
```

编译器说"你已经声明过它了" —— 它俩在编译器眼里就是同一个函数，不是重载。名字不同（一个是 `const Book&` 一个是 `Book const&`），类型只有一个。

**第二，`const Book*` 想改书里的内容会报什么。** 试 `p->m_title = "改一下";`：

```text
error C2678: 二进制"=": 没有找到接受"const std::string"类型的左操作数的运算符(或没有可接受的转换)
note: 转换丢失限定符
```

后面还跟十几条 `note`，全是 `string` 的 `operator=` 候选，一条条说"不能把 `this` 从 `const std::string` 转成 `std::string&`"。**这些 note 不用看**，第一行就够了：`p->m_title` 因为 `p` 是 `const Book*`，取出来的 `m_title` 是个 `const std::string`，常量不能当赋值左边。

**第三，`Book* const` 想换指向会报什么。** 试 `p = &b;`：

```text
error C3892: "p": 赋值给类型为 'Book *const ' 的常量限定 变量
```

报错里那个 `Book *const ` 就是编译器把你写的东西还原成了完整类型 —— 它认的是"指针是常量"，不是"书是常量"。这行报错反过来能教你读类型。

### 5.4 零基础必踩的坑：getter 不加 `const` 尾缀

这是第五节唯一一个你**一定**会撞上的坑，撞上时你还看不懂报错。

假设 `Book` 的 getter 是这么写的（第一课就是这么写的，很自然）：

```cpp
class Book
{
public:
    const string& getTitle() { return m_title; }     // 没有 const 尾缀
};

void printBook(const Book& b)
{
    cout << b.getTitle() << endl;
}
```

编译，报错原文（MSVC 18，逐字未改）：

```text
error C2662: "const std::string &Book::getTitle(void)": 不能将"this"指针从"const Book"转换为"Book &"
note: 转换丢失限定符
note: 参见"Book::getTitle"的声明
note: 尝试匹配参数列表"()"时
```

四条信息，逐条翻译成人话：

1. **第一行**：我（编译器）在调 `Book::getTitle`，但这个函数要求 `this` 是 `Book*`（能改的），而你现在给我的是 `const Book`（不许改的）。两个类型对不上。
2. **`note: 转换丢失限定符`** —— 这句是全篇最关键的一句。"限定符"就是 `const`。"丢失"的意思是：把 `const Book` 当 `Book` 用，等于把 `const` 扔掉；编译器不许你偷偷扔，所以拒绝。
3. **`note: 参见"Book::getTitle"的声明`** —— 报错不在调用处，在声明处。往前翻到 `Book` 的类定义去看那个函数。
4. **`note: 尝试匹配参数列表"()"时`** —— `getTitle()` 一个参数都没传，所以不是"你传错参数了"，问题在函数本身。

**这个错不是"权限不足"，也不是"private 访问不了"**，很多人第一反应往访问控制上想，方向就错了。`getTitle` 是 public 的，能调；问题是它在 `const` 对象上**没资格被调**。

原因就一句话：**成员函数默认隐含一个非 const 的 `this`**。`const Book& b` 传进来的是 `const Book`，你调 `b.getTitle()` 时，编译器要拿 `&b`（类型 `const Book*`）去填 `getTitle` 的 `this`（类型 `Book*`），`const Book*` → `Book*` 是"丢 const"的转换，C++ 禁止。

**改法只有一种是对的：给 getter 加尾缀 `const`。**

```cpp
class Book
{
public:
    const string& getTitle() const { return m_title; }     // ← 尾缀 const
    //                      ↑
    // 加了它，this 的类型从 Book* 变成 const Book*，
    // 于是 const Book 也能传进来，上面的转换就不再"丢 const"了
};
```

尾缀 `const` 的含义不是"这个函数返回 const"，是"**这个函数承诺不改成员**"，编译器真的会检查：

```cpp
int getPrice() const
{
    m_price = 100.0;        // 编译不过：const 函数里改成员
    return m_price;
}
```

一旦承诺，反悔就编译不过。所以这不是装饰，是双向的契约：你调我，我保证不动你的数据；我保证不了，编译器替你保证。

顺带说清一个常见的误会：**加 `const` 不会影响原来的调用**。非 const 对象照样能调 const 成员函数（`Book b; b.getTitle();` 完全合法），因为"不修改"这个承诺对能改的对象也成立。所以给 getter 加 `const` 是纯收益，不会破坏第一课已经写好的代码。

### 5.5 落回你的作业：`Book.h` 里哪几个函数要加

打开你的 `Book.h`，看这两个区段（行号是现在文件里的实际行号）：

```cpp
// Book.h:22-29  获取
string getTitle();           // ← 要加
string getAuthor();          // ← 要加
string getPublisher();       // ← 要加
string getISBN();            // ← 要加
string getId();              // ← 要加
double getPrice();           // ← 要加
bool   getIsBorrowed();      // ← 要加

// Book.h:30-37  修改 设置
void setTitle(string title);              // ← 不能加
void setAuthor(string author);            // ← 不能加
void setPublisher(string publisher);      // ← 不能加
void setISBN(string ISBN);                // ← 不能加
void setId(string id);                    // ← 不能加
void setPrice(double price);              // ← 不能加
void setIsBorrowed(bool isBorrowed);      // ← 不能加

// Book.h:38-39  展示
void display();              // ← 要加
```

判断标准不是"看起来像"，是**函数体里到底改没改成员**：

- `getXxx()` 全部是 `return m_xxx;`，一个字都不写成员 → 加 `const`。
- `setXxx(...)` 里全是 `m_xxx = 参数;` → **加不了**。加了立刻编译不过（这正是 `const` 该干的事）。
- `display()` 里全是 `cout <<`，不写成员 → 加 `const`。

改完之后，你才会遇到真正的问题，也是第六节要解决的：`User` 里怎么存这些 `Book`。

`display() const` 一旦加上，它内部就能调其他 `const` 成员函数，但**不能**调非 const 的。所以顺序要对：先给所有 getter 加 `const`，最后给 `display()` 加 —— 反了的话 `display() const` 里调 `getTitle()` 又会报 C2662。

**顺带定一件事：借还判断"是不是同一本书"，用 `m_ISBN` 还是 `m_id`？**

两个字段都在 `Book` 里，含义不同：

| 字段 | 含义 | 能不能重复 |
|------|------|-----------|
| `m_ISBN` | 国际标准书号，一本书的"型号" | 同一种书买了两本，ISBN 相同 |
| `m_id` | 馆内自己编的编号（贴书脊上那个条码） | 每一本实体书唯一 |

所以：**判断"这本书是不是我手上那本"要用 `m_id`** —— 图书馆里可能有两本一模一样的《C++ Primer》，ISBN 一样，但条码不同，他借的是甲本，甲本被弄丢了，乙本还在架上，这两本不能混。判断"他是不是借过这一**种**书"才用 `m_ISBN`。

你现在这个阶段，`main` 里每本书只有一个实例，两个字段都不会重复，用哪个都能跑。但**写的时候就要想清楚**：`returnBook` 里那道校验如果按 `m_ISBN` 比，将来两本同 ISBN 的书就会互串。建议从一开始就用 `m_id` 判同一个实体，`m_ISBN` 只在"这本书馆里还有几本"这种统计上用。下面第十节任务 C 的参考实现为了简短按 `m_ISBN` 判 —— 你抄的时候改成 `getId()`。

### 5.6 这一节的验收标准

不看讲义，回答这三个，能立刻答上来才算过：

1. `void f(Book b)` 和 `void f(const Book& b)`，调用 `f(myBook)` 之后，`myBook` 的内存有没有被搬动过？怎么用代码证明？
2. `Book const*` 和 `Book* const` 分别锁了什么？给我一个"只有前者才编译不过"的语句。
3. 你的 `getIsBorrowed()` 现在加 `const` 尾缀，`main` 里原来那些 `b.getIsBorrowed()` 会不会因为这个改动而编译不过？

---

## 六、组合：一个类的成员是另一个类的对象

### 6.1 先看它长什么样

组合（composition）就一句话：**一个类的成员变量，类型是另一个类**。

这个你在作业里**已经写了**，而且不知不觉写了两层。打开 `User.h`：

```cpp
// User.h:6-11
struct BorrowRecord
{
    Book m_book;                                  // 第 1 层：一条记录里组合一本书
    void setBook(Book& book) { m_book = book; }
    Book getBook() { return m_book; }
};

// User.h:13-20
class User
{
private:
    string m_name;
    string m_id;
    int m_borrownum;
    int m_maxnum;
    BorrowRecord m_records[60];                   // 第 2 层：User 里组合 60 条记录
};
```

两层都是组合，一层套一层。`User` 对象里真的住着 60 个 `BorrowRecord`，每个 `BorrowRecord` 里真的住着一本完整的 `Book`。画出来是这样：

```text
User 对象（一整块连续内存）
┌──────────────────────────────────────────┐
│ m_name / m_id / m_borrownum / m_maxnum   │
├──────────────────────────────────────────┤
│ m_records[0]                             │
│    └─ m_book: 书名/作者/出版社/ISBN/id/价/状态 │
├──────────────────────────────────────────┤
│ m_records[1]                             │
│    └─ m_book: ……                          │
├──────────────────────────────────────────┤
│ ……（共 60 个格子）                        │
├──────────────────────────────────────────┤
│ m_records[59]                            │
│    └─ m_book: ……                          │
└──────────────────────────────────────────┘
```

一个 `User` 对象占多少字节？`60 × sizeof(BorrowRecord)`，而 `sizeof(BorrowRecord)` 又等于 `sizeof(Book)`。你这台机器上 `Book` 有 4 个 `string` + 1 个 `double` + 1 个 `bool`（`Book.h:8-14`），`sizeof` 大概 120 字节左右，因此一个 `User` 对象 **7KB 上下**。这个数字先记住，6.4 会用到。

对比另一种写法（依赖）——把 `Book` 换成指针，书就不住在 `User` 里了：

```cpp
struct BorrowRecord
{
    Book* m_book;                                 // 只存一个地址，书在别处
};
```

一个是"书在抽屉里"，一个是"抽屉里放着一张纸条，写着书在哪"。这两种在内存布局、构造析构顺序、"改一边另一边变不变"上，全是两种不同的东西。

### 6.2 构造顺序：本章考点，必须讲透

这是本章唯一一个必考的知识点，也是作业里 `m_borrownum` 一上来就是垃圾值的根源。

规则三条，按执行顺序：

1. **成员对象先构造**，顺序是**成员在类里的声明顺序**（不是你在初始化列表里写的顺序）。
2. 全部成员构造完，**才执行本类的构造函数体**（那对花括号里的语句）。
3. 析构顺序**完全相反**：先执行本类析构函数体，再按声明的**逆序**析构成员。

`lesson02.cpp` 里用两个会打印的类把这件事摊开：

```cpp
class Part
{
private:
    string m_tag;
public:
    Part(const string& tag) : m_tag(tag)
    {
        cout << "  [构造] Part " << m_tag << endl;
    }
    ~Part()
    {
        cout << "  [析构] Part " << m_tag << endl;
    }
};

class Owner
{
private:
    Part m_a;
    Part m_b;
public:
    Owner() : m_a("m_a"), m_b("m_b")
    {
        cout << "  [构造] Owner 的构造体开始执行（m_a / m_b 早已造好）" << endl;
    }
    ~Owner()
    {
        cout << "  [析构] Owner 的析构体开始执行（m_a / m_b 还活着）" << endl;
    }
};
```

`main` 里只有一句 `{ Owner o; }`，外面套一对花括号。真实输出：

```text
下面一对花括号是一个作用域，出右括号 o 就析构：
  [构造] Part m_a
  [构造] Part m_b
  [构造] Owner 的构造体开始执行（m_a / m_b 早已造好）
  [析构] Owner 的析构体开始执行（m_a / m_b 还活着）
  [析构] Part m_b
  [析构] Part m_a
出了作用域，o 已经不在了。
```

盯住这六行的位置，它把所有顺序都写死了：

- 前两行是 `Part`。这说明**你写 `Owner()` 的花括号时，`m_a` 和 `m_b` 早就构造完了**。等你的花括号开始执行，两个成员已经是活的、有数据的状态。
- 第三行才是 `Owner` 自己的构造体。这是你唯一能写代码的地方。
- 第四行是 `Owner` 的析构体 —— 此时 `m_a` / `m_b` 还活着，你还能用它们。
- 第五、六行是 `m_b` 先死，`m_a` 后死。**逆序**。

为什么必须逆序？因为 `m_b` 的构造过程可能用到了 `m_a`（比如 `m_b` 的构造函数里读了 `m_a` 的值）。先构造的后析构，才能保证"我死的时候，我依赖的人还活着"。

**考点第一问：把初始化列表写反，输出会变吗？**

```cpp
Owner() : m_b("m_b"), m_a("m_a")      // 故意写成 b 在前
```

答案：**不变**，还是 `m_a` 先、`m_b` 后。因为构造顺序只由 `class Owner` 里 `m_a; m_b;` 的**声明顺序**决定，和你初始化列表里怎么排字无关。初始化列表的书写顺序只影响"每个成员用哪个参数初始化"，不影响"谁先被造"。

MSVC 在这个不一致的地方会给你一条提醒（`/W3` 就会出）：

```text
warning C5038: 数据成员"Owner::m_b"将先于数据成员"Owner::m_a"初始化
```

这条 warning 不是唠叨，它在告诉你"你以为的顺序和实际的顺序不一样"。看到它就说明你列表写反了。

**考点第二问：为什么 `m_borrownum` 会是垃圾值？**

按上面规则，`User` 构造时，`m_records`（60 个 `BorrowRecord`）会先被构造。`BorrowRecord` 自己没写构造函数，所以编译器给它一个隐式默认构造，它再用 `Book()` 把里面的 `m_book` 造出来 —— 两层，全是默认构造，60 次。

这也解释了一件事：**`Book.h:18` 那个 `Book();` 声明是必须存在的。** 删掉它，`BorrowRecord` 编不过，`User` 也编不过，报的就是下面 6.3 第三条的 C2512。

而 `m_borrownum` 是 `int`，`int` 的"构造"是什么都不做。**所以它的值是你这块内存上一位住户留下来的**，可能是 0，可能是 2004318071。第一课那张"初始化"表里已经说过，这里只是它在真实作业里的样子：如果你在 `User::User()` 里不写 `m_borrownum = 0;`，`getBorrownum()` 第一次返回的就是垃圾。

### 6.3 初始化列表：不只是"写法好看"

平时你可以偷懒：

```cpp
Book::Book(string t)
{
    m_title = t;         // 构造体里赋值，能跑
}
```

但有三类成员，这么写**根本编不过**。这三类必须用初始化列表，没有第二条路。

**第一类：`const` 成员。**

```cpp
class BookWithConstIsbn
{
private:
    const string m_isbn;
public:
    BookWithConstIsbn(const string& isbn)
    {
        m_isbn = isbn;               // 想在这里赋值
    }
};
```

```text
error C2678: 二进制"=": 没有找到接受"const std::string"类型的左操作数的运算符(或没有可接受的转换)
note: 转换丢失限定符
```

还是那个"转换丢失限定符"。因为 `const` 成员一旦构造完就是常量，而函数体里的 `=` 是**赋值**，赋值给常量不合法。初始化列表不是赋值，是**构造** —— 它在对象诞生的那一刻就把值放进去。

```cpp
    BookWithConstIsbn(const string& isbn) : m_isbn(isbn) {}     // 对
```

有一个细节值得单独记：**如果你不写初始化列表，const 成员会走它自己的默认构造**。下面的代码是**能编译通过**的：

```cpp
class Book
{
private:
    const string m_isbn;
public:
    Book() {}        // 没初始化 m_isbn，也编过了
};
```

因为 `string` 有默认构造函数，`m_isbn` 被默认构造成空串 —— 这也是一种"初始化"。真正编不过的是 `int` 这种没有构造函数的类型：

```cpp
    const int m_id;
public:
    Book() {}        // 这个就编不过：const int 必须被初始化
```

**第二类：引用成员。**

```cpp
class User
{
private:
    string& m_school;
public:
    User(string& s)
    {
        m_school = s;                // 想在这里绑
    }
};
```

```text
error C2530: "User::m_school": 必须初始化引用
```

引用必须在诞生的那一刻就绑上，而且一辈子不能换。函数体开始执行时引用已经"出生"了 —— 它必须绑在什么东西上，你没给，编译器没法造。

```cpp
    User(string& s) : m_school(s) {}        // 对
```

再补一句容易误会的：`m_school = s;` 写在函数体里，即便能编译，那也**不是"绑定"，是"把 s 的内容赋给 m_school 指向的那个 string"**。引用不能重新绑定，这是它和指针最大的区别。

**第三类：成员对象没有默认构造函数。**

假设你的 `Book.h` 只留了带参构造，把 `Book.h:18` 那行 `Book();` 删了：

```cpp
class Book
{
    // ……字段
public:
    // 只剩带参构造，没有 Book() 了
    Book(string title, string author, string publisher, string ISBN,
         string id, double price, bool isBorrowed);
};

class Student
{
private:
    Book m_borrowed;
public:
    Student() {}                             // 想默认造一个 Student
};
```

```text
error C2512: "Book": 没有合适的默认构造函数可用
note: 参见"Book"的声明
```

因为造 `Student` 时必须先造 `m_borrowed`，而 `Book` 唯一的构造函数要求七个参数。`Student()` 一个都没给，编译器找不着能用的构造函数。

```cpp
    Student() : m_borrowed("未借书", "", "", "", "", 0.0, false) {}    // 对：明确指路
```

这条错误你写作业时**天天会遇到**，而且它的现场往往长这样：你给 `Book` 加了新的带参构造，顺手把默认构造删了。于是 `User.h:20` 那行 `BorrowRecord m_records[60];` 立刻 C2512 —— 因为造 60 个格子时，每个 `BorrowRecord` 都要用 `Book()` 造里面的 `m_book`。

解法只有一条：**给 `Book` 留住默认构造。** `Book.h:18` 那行 `Book();` 别删，`Book.cpp` 里给它赋值：

```cpp
Book::Book()
{
    m_title      = "";
    m_author     = "";
    m_publisher  = "";
    m_ISBN       = "";
    m_id         = "";
    m_price      = 0.0;
    m_isBorrowed = false;
}
```

一行默认构造，60 个格子自己就全填好了。

（理论上还有第二条路：在 `User` 的初始化列表里把 60 个 `m_records` 一个个手写出来。这条路不是"麻烦"，是**根本不解决问题** —— 初始化列表只能写常量列表，写不了循环，你换个上限就得改 60 行。所以对数组类型的成员，默认构造是硬需求。）程序里跑出来的效果：

```text
默认构造出来的学生（里面的 m_borrowed 是一本空书）：
学生:   学号: 
借书记录:
《》 作者:   ISBN:   价格: 0  状态: 可借
```

姓名学号空着、书名空着、价格 0、状态可借 —— 这就是"默认构造出来的空壳"。你能看到 `m_borrowed` 已经是个**存在但空**的 `Book`，不是"没有书"。

### 6.4 组合的硬性质：是"抄一份"，不是"绑在一起"

这一条比构造顺序更容易错，因为它在**编译期完全看不出来**，只有运行起来才发现"我改的那本和我想的那本不是同一本"。

用两个类对比，代码在 `lesson02.cpp` 6.3 段：

```cpp
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
        for (int i = 0; i < 3; i++) m_books[i] = nullptr;
    }
};
```

三类证据，一个比一个硬。

**证据一：`sizeof` 就不一样。**

```text
sizeof(Book)      = 112
sizeof(ShelfA)    = 344（3 本书真住在里面，每本 112 字节）
sizeof(ShelfB)    = 32（3 个指针，每个 8 字节，书还在外面）
```

`ShelfA` 有 344 字节：`3 × 112 = 336` 的书 + 4 字节的 `m_count`，对齐到 8 变成 344。书是**住进来**的，所以对象变胖。`ShelfB` 只有 32 字节：`3 × 8 = 24` 的指针 + 4 字节 `m_count`，对齐后 32。书住在**外面**，它只知道地址。

**证据二：成员地址落在容器内部。**

```text
ShelfA 对象自己的地址   : 000000286A5DE880
  sa.m_books[0] 的地址  : 000000286A5DE880   ← 和上面一模一样
  sa.m_books[1] 的地址  : 000000286A5E…       （比 [0] 大 112）
  [1] 和 [0] 差多少字节 : 112
```

`sa.m_books[0]` 的地址和 `sa` 本身**完全相同** —— 因为数组是类的第一个成员，它就长在对象最前面。`[1]` 在它后面 112 字节处，`[2]` 再后面 112。三本书首尾相接，全在 `sa` 这 344 字节里面。

而指针版本：

```text
外面那本书的地址        : 000000286A5DE440
  sb.m_books[0] 存的地址: 000000286A5DE440   ← 就是它
```

`sb.m_books[0]` 里存的值，正好等于外面那本书的地址。它指向的不是自己内部的存储，是另一个地方。

**证据三：改副本，原件不动。**

```cpp
Book outside("外部书", "某人", "ISBN-000", 66.0);
ShelfA sa2;
sa2.m_books[0] = outside;         // 抄一份进去
```

```text
指针那条 sb.m_books[0] 和 &outside 相等吗: 相等（指针存的同一本书的地址）
抄来的 sa2.m_books[0] 地址和 &outside 相等吗: 不相等（是另一个对象，只是内容一样）
```

内存长这样：

```text
组合：两本书，两份数据
┌─ outside ─────────────┐        ┌─ sa2 ──────────────────────┐
│ m_title  "外部书"     │        │ m_books[0]                 │
│ m_isbn   "ISBN-000"   │        │   m_title  "外部书"        │
│ m_price  66.0         │        │   m_isbn   "ISBN-000"      │
│ m_isBorrowed false    │        │   m_price  66.0            │
└───────────────────────┘        │   m_isBorrowed false       │
   ↑ 改这个                        └────────────────────────────┘
   sa2 里那份纹丝不动                  ↑ 改这个，outside 纹丝不动

指针：一本书，两个地址
┌─ outside ─────────────┐  ←────  sb.m_books[0]
│ m_title  "外部书"     │  ←────  other.m_books[0]
│ m_isBorrowed false    │
└───────────────────────┘
  改一次，两个持有者看到的是同一个改动
```

这就是组合的硬性质：**`User` 里的 `Book` 和外面那个 `Book` 是两个对象，内容相同，命运无关。** 组合是"复制一份数据进来"，不是"建立一个指向关系"。

任务 A 把这条性质直接打在作业上：

```text
把外面的 a1 改成已借出，看 stuA 里那本变不变：
  外面 a1 的状态:      已借出
  stuA 里记的那本:     可借   ← 没跟着变，因为是两个独立对象
```

### 6.5 落回你的作业：`BorrowRecord m_records[60]` 这个设计对不对

先说结论：**你现在这个写法是对的，不用改成指针。** 组合是你的作业里唯一能不碰 `new` / `delete` 的方案，第三课之前你都该用它。

但 `User.h` 里有三处，等你写到 `borrowBook` 的时候一定会绊你。逐个说清。

**第一处：`void setBook(Book& book) { m_book = book; }` —— 这是拷贝，不是绑定。**

`User.h:9`。`m_book = book;` 执行的是 `Book` 的**拷贝赋值运算符**，把外面那本书的每个字段抄一份进 `m_book`。执行完，`m_records[k].m_book` 和外面那本书是两个对象。这就是 6.4 讲的那条硬性质，落在你作业里就是：

```cpp
Book b1("C++ Primer", "Lippman", "ISBN-001", 128.0);
user.borrowBook(b1);            // 里面调 setBook(b1)，抄一份
b1.setIsBorrowed(true);         // 改外面这本
user.displayRecords();          // 里面那份还是"可借"
```

参数写 `Book&` 而不是 `const Book&`，是因为 `setBook` 只是读 `book`、不写它 —— 按理说该写 `const Book&`（第五节）。但 `borrowBook` 是要改 `b1` 状态的，所以 `borrowBook(Book& book)` 这个非 const 参数**是对的**，别改。这两层签名不冲突：`borrowBook` 收可改引用因为要改它，`setBook` 收可改引用但其实只读 —— 后者可以收紧成 `const Book&`，收紧之后编译一样过。

**第二处：`Book getBook() { return m_book; }` —— 返回值，不是引用。**

`User.h:10`。这是你这几个函数里最容易踩的一个坑，因为它的错误**不会报错，只是悄悄没用**：

```cpp
user.getBook().setIsBorrowed(true);      // 看起来改了
// 实际上：getBook() 返回一个临时 Book 副本，
// setIsBorrowed 改的是这个临时对象，
// 语句结束，临时对象销毁 —— 什么都没发生
```

编译通过，运行不崩，数据没变。这种 bug 你查半小时都找不出来。

为什么不能改成 `Book& getBook() { return m_book; }`？能改，而且能解决上面这个问题。但那样一来，任何拿到 `getBook()` 的人都能改你 `User` 内部的私有数据 —— 封装就漏了。所以标准做法是：

```cpp
const Book& getBook() const { return m_book; }     // 给看不给改
```

然后如果真的需要改状态，让 `User` 自己提供方法（正是你的 `borrowBook` / `returnBook` 该干的事），而不是把内部的 `Book&` 交出去。

**第三处：`BorrowRecord m_records[60]` 这个 60，和 `m_maxnum` 是两套数。**

`User.h:19-20` 里 `m_maxnum` 是可以设的上限（构造时传进来），`m_records[60]` 是数组的物理容量。这两个必须满足 `m_maxnum <= 60`，否则 `m_records[m_borrownum]` 就越界。

现在的写法里没有任何地方检查这件事 —— `User(string name, string id, int borrownum, int maxnum)` 允许你传 `maxnum = 200`。建议在构造里加一道：

```cpp
User::User(string name, string id, int borrownum, int maxnum)
{
    m_name      = name;
    m_id        = id;
    m_borrownum = borrownum;
    m_maxnum    = (maxnum > 60) ? 60 : maxnum;    // 顶多到数组容量
}
```

顺便说清 `m_borrownum` 和 `m_maxnum` 的分工，这两个很容易混：

| 字段 | 含义 | 谁改它 |
|------|------|--------|
| `m_borrownum` | **现在**借了几本（0 ~ m_maxnum） | `borrowBook` 加一，`returnBook` 减一 |
| `m_maxnum` | **最多**能借几本（策略，一般不动） | 构造时定，之后不该改 |

判断"能不能再借"用的是 `m_borrownum >= m_maxnum`，不是 `m_borrownum >= 60`。60 是墙，`m_maxnum` 是规则，撞规则给提示，撞墙就是内存事故。

**第四处（不算错，但要心里有数）：`User` 对象有多大。**

`sizeof(User) ≈ 4 个标量 + 60 × sizeof(BorrowRecord) ≈ 7KB`。你在 `main` 里写 `User u;` 这一行，就是在栈上开了 7KB。栈默认 1MB，所以你放 100 个 `User` 就爆栈了 —— 表现为程序直接崩，报"栈溢出"。

现在作业里就一两个 `User`，完全没问题。但你要知道这个数字从哪来：**组合的代价就是对象变大**。指针版本 `BorrowRecord* m_records[60]` 只占 480 字节，书在堆上，但那要你自己 `new` / `delete` —— 第三课的内容。

一句话收尾：**这一章你继续用组合，别换指针。** 你现在遇到的"我改了副本，真书没变"不是设计错了，是组合的固有性质，你的作业本来就该靠"手动同步两边状态"来解 —— 第十节任务 C 的 `borrowBook` 里那行 `b->setIsBorrowed(true)` 干的就是这件事。

---

## 十、动手任务（做完就解锁第三课）

三档，从 A 到 C 难度递增。C 档就是你的作业真需求，A、B 是为它拆的台阶。

**做题规矩**：先只看"要你做什么"，自己写。卡住看"提示"，提示只给方向不给代码。写完能跑通了，再点开 `<details>` 对答案。**直接看答案等于没做。**

参考实现的完整代码在 `学习/lesson02.cpp` 第十节，分段打印，和自己写的对照着看。

---

### 任务 A　把 `Book` 塞进 `Student` 里（组合入门）

**要你做什么**

1. 写一个 `Student` 类，三个成员：`string m_name`、`string m_id`、`Book m_borrowed`。
2. 提供两个构造函数：
   - 默认构造 —— 造出一个空学生（姓名学号空、书是空书）。
   - 带参构造 `Student(string name, string id, const Book& b)` —— 用传进来的书**初始化我的成员**。
3. 写 `void display() const`，打印学生信息和那本书的信息。打印书的时候**调用 `m_borrowed.display()`**，不要自己再写一遍 `cout << m_borrowed.getTitle()`。
4. 在 `main` 里：造一本书 `a1`，用它造一个学生 `stuA`，打印 `stuA`；然后**改 `a1` 的状态为已借出**，再打印一次 `stuA` 里那本书的状态。

**提示（不剧透）**

- 第 3 步那句"用传进来的书初始化我的成员"，位置在**初始化列表**里，不在花括号里。想一想 6.3 第三条：`Book` 如果有带参构造而没有默认构造，写进花括号里根本编不过。
- 第 4 步是这道题的**全部意义**。你改 `a1` 之后 `stuA` 里那本变不变，就是"组合"这个词的定义。
- `display()` 标 `const` 之后，里面能调的方法也必须是 `const` 的。如果 `Book::display()` 还没加 `const` 尾缀，这里会报 C2662 —— 回去把 5.5 的清单做完。

<details>
<summary>做完再看：参考实现和真实输出</summary>

```cpp
class StudentA
{
private:
    string m_name;
    string m_id;
    Book   m_borrowed;        // 组合就发生在这一行

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
```

`main` 里：

```cpp
Book a1("C++ Primer", "Lippman", "ISBN-001", 128.0);
StudentA stuA("YG", "2024001", a1);
cout << "    把外面的 a1 改成已借出，看 stuA 里那本变不变：" << endl;
a1.setIsBorrowed(true);
cout << "      外面 a1 的状态:       " << a1.statusText() << endl;
cout << "      stuA 里记的那本:      " << stuA.getBorrowed().statusText()
     << "   ← 没跟着变，因为是两个独立对象" << endl;
```

真实输出：

```text
学生: YG  学号: 2024001
借书记录:
《C++ Primer》 作者: Lippman  ISBN: ISBN-001  价格: 128  状态: 可借
把外面的 a1 改成已借出，看 stuA 里那本变不变：
  外面 a1 的状态:      已借出
  stuA 里记的那本:     可借   ← 没跟着变，因为是两个独立对象
```

三个必须看懂的细节：

1. `m_borrowed(b)` 调的是 `Book` 的**拷贝构造函数**，不是赋值。它在初始化列表里，所以是"生下来就是这个样子"。
2. `stuA.getBorrowed()` 返回 `const Book&` —— 返回引用，不拷贝；加 `const`，外面拿不到能改的入口。
3. 那句 `stuA.getBorrowed().statusText()` 能调，是因为 `statusText()` 有 `const` 尾缀。少一个 `const` 这里就 C2662。

</details>

---

### 任务 B　借书 / 还书，参数传指针（依赖入门）

**要你做什么**

写一个 `Student` 类，里面存**一本**书（还是 `Book m_borrowed`），再写两个成员函数：

```cpp
bool borrowBook(Book* b);       // 借书
bool returnBook(Book* b);       // 还书
```

要求：

1. `borrowBook` 成功时返回 `true`，失败返回 `false`，**失败必须打印一句人能看懂的原因**。
2. 要挡住四种情况：
   - 传进来的是 `nullptr`；
   - 我手上已经借了别的书（还没还）；
   - 这本书已经被别人借走了；
   - （已经借到手的情况下）再借同一本。
3. `returnBook` 要挡住两种情况：传进来是 `nullptr`；要还的书**不是他借的那本**。
4. `main` 里逐个跑这些场景，每个场景前先 `cout` 一句"这一条应该成功/失败"，然后对照输出。
5. 最后做一个对比：写一个 `void borrowByValue(Book b)` 和一个 `void borrowByPointer(Book* b)`，都在里面调 `setIsBorrowed(true)`，各自打印形参地址，让 `main` 里那本书的真实状态说话。

**提示（不剧透）**

- 判"是不是同一本书"，用 `ISBN` 比对，不要用书名（两本书可能同名）。
- 判"这本书是不是我手上那本"，也用 `ISBN`。用 `==` 比对象？`Book` 没定义 `operator==`，比不了。
- `nullptr` 判断必须是**函数体第一句**。少这一句，传空指针进来就是访问违规崩掉，程序直接死，你连"失败"都打印不出来。
- 第 5 步是本题的重点，不是凑数。你要亲眼看到"值传递改不动外面那本"，才会明白为什么借还函数必须收指针或引用。
- 判断顺序会直接影响打印出来的话对不对。想一想：如果先判"书已被借走"再判"我是不是已经借过同一本"，那么我第二次借同一本时，会打出哪一句？那句对吗？

<details>
<summary>做完再看：参考实现和真实输出</summary>

```cpp
class StudentB
{
private:
    string m_name;
    string m_id;
    Book   m_borrowed;
    bool   m_hasBook;

public:
    StudentB(string name, string id)
        : m_name(name), m_id(id), m_borrowed(), m_hasBook(false)
    {
    }

    bool borrowBook(Book* b)
    {
        // 第 0 关：空指针。这一关必须在最前面，否则下面所有 -> 都会崩
        if (b == nullptr)
        {
            cout << "    [借书失败] 传进来的书指针是空的。" << endl;
            return false;
        }

        // 第 1 关：我手上已经有书了
        if (m_hasBook)
        {
            cout << "    [借书失败] " << m_name << " 已经借了《"
                 << m_borrowed.getTitle() << "》，先还了再说。" << endl;
            return false;
        }

        // 第 2 关：这本书是不是已经被别人借走了
        if (b->getIsBorrowed())
        {
            cout << "    [借书失败] 《" << b->getTitle() << "》已经被别人借走了。" << endl;
            return false;
        }

        // 到这里才真的借：抄一份进我的成员，同时把外面那本的状态改掉
        m_borrowed = *b;              // *b 是"b 指向的那个 Book 对象"
        m_borrowed.setIsBorrowed(true);
        b->setIsBorrowed(true);       // 通过指针改外面那本的真身
        m_hasBook = true;
        cout << "    [借书成功] " << m_name << " 借到《" << b->getTitle() << "》。" << endl;
        return true;
    }

    bool returnBook(Book* b)
    {
        if (b == nullptr)
        {
            cout << "    [还书失败] 传进来的书指针是空的。" << endl;
            return false;
        }
        if (!m_hasBook)
        {
            cout << "    [还书失败] " << m_name << " 手上没有书。" << endl;
            return false;
        }
        // 关键校验：他借的是不是这一本
        if (m_borrowed.getIsbn() != b->getIsbn())
        {
            cout << "    [还书失败] 《" << b->getTitle() << "》不是 "
                 << m_name << " 借的那本。" << endl;
            return false;
        }
        b->setIsBorrowed(false);
        m_borrowed = Book();
        m_hasBook = false;
        cout << "    [还书成功] " << m_name << " 归还《" << b->getTitle() << "》。" << endl;
        return true;
    }
};
```

值传递 vs 指针传递的对比函数：

```cpp
void borrowByValue(Book b)          // 拷贝一份，改的是副本
{
    cout << "      （函数里）形参 b 的地址: " << &b << endl;
    b.setIsBorrowed(true);
}

void borrowByPointer(Book* b)       // 拿到真地址，改的是真身
{
    cout << "      （函数里）形参 b 的值:   " << b << "  ← 就是外面那本书的地址" << endl;
    b->setIsBorrowed(true);
}
```

真实输出（四种拒绝场景 + 两种传参对比）：

```text
借第一本：应该成功
[借书成功] YG 借到《C++ Primer》。
已经借了一本，再借第二本：应该失败
[借书失败] YG 已经借了《C++ Primer》，先还了再说。
还一本他根本没借的书：应该失败
[还书失败] 《计算机网络》不是 YG 借的那本。
还对了：应该成功
[还书成功] YG 归还《C++ Primer》。
借一本已被人借走的书：应该失败
[借书失败] 《被借走的书》已经被别人借走了。
传空指针：应该提示失败，而不是崩掉
[借书失败] 传进来的书指针是空的。
值传递为什么不行：对比两个地址
  main 里 bkTest 的地址:  00000067C155E5E0
  调用前状态:              可借
  （函数里）形参 b 的地址: 00000067C155E178
  调用后状态:              可借   ← 没变
改成指针传：
  main 里 bkTest2 的地址: 00000067C155E730
  调用前状态:              可借
  （函数里）形参 b 的值:   00000067C155E730  ← 就是外面那本书的地址
  调用后状态:              已借出   ← 变了
```

四个要点：

1. `m_borrowed = *b;` 里的 `*b` 是"解引用"：`b` 是地址，`*b` 是那个地址上的对象。这一句是把外面那本书**抄一份**进我的成员。
2. 抄完之后我这份要自己 `setIsBorrowed(true)`，因为拷过来的状态假设是"可借"。
3. `b->setIsBorrowed(true);` 是把**外面那本真身**的状态也改掉。`->` 等价于 `(*b).`。
4. 值传递那段：形参地址 `…E178` 和外面 `…E5E0` 不一样，说明改的是副本；函数一返回副本就没了，外面那本纹丝不动。指针那段的形参值 `…E730` 和外面地址一模一样 —— 它不是副本，它就是那本书的地址。

</details>

---

### 任务 C（作业真需求）　对象数组 + 借书还书 + 还书校验

**要你做什么**

这才是要交上去的那个版本。**按你 `User.h` 现有的骨架写**，不要另起炉灶：

```cpp
// User.h 现在就是这样，一个字段都别改
struct BorrowRecord
{
    Book m_book;                                        // 记录里组合一本书
    void setBook(Book& book) { m_book = book; }
    Book getBook() { return m_book; }
};

class User
{
private:
    string m_name;
    string m_id;
    int m_borrownum;                                    // 现在借了几本
    int m_maxnum;                                       // 最多能借几本
    BorrowRecord m_records[60];                         // 60 个格子的对象数组

public:
    void borrowBook(Book& book);                        // 收引用，因为要改它
    void returnBook(Book& book);
    void displayRecords();
};
```

要求：

1. `borrowBook(Book& book)` 要挡住四种情况，每种打印一句具体原因：
   - **借阅记录已满**（`m_borrownum >= m_maxnum`，不是 `>= 60`）；
   - **他已经借过同一本**（按 `m_id` 判 —— 见 5.5 那张 ISBN / id 的分工表）；
   - 这本书已经被别人借走（`book.getIsBorrowed()`）；
   - 都通过 → `m_records[m_borrownum].setBook(book)` 抄一份进去、`m_borrownum++`、**并且**把真书置为已借出。
2. `returnBook(Book& book)` 的核心是**还书校验**：他要还的书，必须真的在他借的清单里。要挡住：
   - 他根本没借过任何书（`m_borrownum == 0`）；
   - **他要还的这本不在他的清单里**（借了 A 却想还 B）。
3. 还书成功时，`m_records` 数组要**保持前面连续**：删掉中间那个，后面的往前挪一位，`m_borrownum--`，末尾格子清零。
4. `displayRecords()` 打印当前借阅清单（带编号 + 书名 + `m_id`），`m_borrownum == 0` 时打印"暂无借阅记录"而不是空白。
5. `main` 里按这个顺序跑，每个场景前先打印"这一条应该成功/失败"：
   1. 借两本，都成功；
   2. 同一本再借一次，失败；
   3. 打印清单（2 本）；
   4. **借了 A 却想还 B，失败**；
   5. 还 `C++ Primer`，成功，清单少一本；
   6. 还完再借 `计算机网络`，成功；
   7. 打印三本书的真实状态；
   8. 把 `m_maxnum` 借满，全成功；
   9. 再多借一本，失败（上限）。

**提示（不剧透）**

- 要求 1 里"他已经借过同一本"和"这本书已经被别人借走"这两关的**先后顺序**要想清楚。你自己借走的那本书，状态已经是"已借出"，如果你先判"是不是被别人借走"，第二次借同一本就会打出"已经被别人借走了"——话说错了，人也会看晕。哪一关该在前？
- 时刻记住 `m_records` 是 `BorrowRecord` 的数组，不是 `Book` 的数组。要碰里面的书，路径是 `m_records[i].m_book`，两步。
- `Book& book` 是引用不是指针，所以用 `book.getTitle()`、`book.setIsBorrowed(true)`，**点号不是箭头**。任务 B 用指针写的那套 `b->` 到这里全部要改回 `book.`。
- 要求 3 的"往前挪"，核心是一个 `for` 循环：`m_records[i] = m_records[i + 1];`。想清楚 `i` 的上界是 `m_borrownum - 1` 还是 `m_borrownum`。
- 要求 4 里 `displayRecords()` 内部要调 `m_records[i].m_book.display()`。如果 `Book::display()` 没加 `const` 尾缀，这里 C2662 等着你。
- 第 8、9 步的关键是"满"这个状态。如果你判满用的是 `m_borrownum >= 60` 而 `m_maxnum` 只有 5，你会借到 60 本才报错 —— 那是把规则和墙搞混了。

<details>
<summary>做完再看：参考实现（可直接贴进你的 User.cpp）和真实输出</summary>

**先看你现在的 `User.cpp` 有两个洞。** 我不动你的文件，你自己补。

第一个洞，`borrowBook` 只有两道关：

```cpp
void User::borrowBook(Book& book)
{
    if (m_borrownum < m_maxnum)              // 只判了"满不满"
    {
        if (book.getIsBorrowed())            // 只判了"书是否已借出"
        {
            cout << "This book is borrowed." << endl;
            return;
        }
        else { /* 抄一份进数组 */ }
    }
    else { cout << "You have borrowed too many books." << endl; }
}
```

缺的是"你已经借过同一本"这一关。缺它的后果：他已经借了《C++ Primer》，再借一次 —— 那本书的 `m_isBorrowed` 已经是 `true`，所以第二道关会拦下来，打印 `This book is borrowed.`。**话是错的**：不是别人借走的，是他自己借的。程序不会崩，但答辩老师问一句"为什么我自己借的书说我借不了"，你答不上来。

第二个洞，`returnBook` 完全没有校验：

```cpp
void User::returnBook(Book& book)
{
    if (m_borrownum <= 0) { /* 打印"没有借书记录" */ return; }
    book.setIsBorrowed(false);               // 谁的书？不知道，反正标成可借
    m_borrownum--;                           // 直接就减了
    cout << "还书成功：" << book.getTitle() << endl;
}
```

`m_borrownum--` 前面没有任何"这本书是不是他借的"检查。所以 `u.returnBook(计算机网络)`（他从没借过这一本）会打印"还书成功：计算机网络"，然后 `m_borrownum` 从 2 变成 1 —— 他清单里的《算法导论》那一行**被悄悄挤掉了**，而《计算机网络》明明在馆里躺着却被标成"可借"。一个 `for` 循环 + 一个 `-1` 判断就能堵死它。

修正版：**下面这三个函数直接替换你 `User.cpp` 里对应的三个**（构造函数顺手改一处：`m_maxnum` 夹到 60 以内，防止有人填 100 把 `m_records[60]` 写穿）。

```cpp
// ---- 借书：四道关，顺序不能换 ----
void User::borrowBook(Book& book)
{
    // 第 1 关：记录满了没有（用 m_maxnum，不是用 60）
    if (m_borrownum >= m_maxnum)
    {
        cout << "  [借书失败] 已借 " << m_borrownum << " 本，已达上限 " << m_maxnum << " 本。" << endl;
        return;
    }

    // 第 2 关：是不是已经借过同一本（按 id 判同一个实体）
    // 这一关必须排在"书已借出"前面
    for (int i = 0; i < m_borrownum; i++)
    {
        if (m_records[i].m_book.getId() == book.getId())
        {
            cout << "  [借书失败] 你已经借过《" << book.getTitle() << "》了。" << endl;
            return;
        }
    }

    // 第 3 关：这本书已经被别人借走
    if (book.getIsBorrowed())
    {
        cout << "  [借书失败] 《" << book.getTitle() << "》已经被别人借走了。" << endl;
        return;
    }

    // 全过：抄一份进记录，同时改真书状态
    m_records[m_borrownum].setBook(book);
    m_borrownum++;
    book.setIsBorrowed(true);
    cout << "  [借书成功] " << m_name << " 借到《" << book.getTitle()
         << "》，当前共 " << m_borrownum << " 本。" << endl;
}

// ---- 还书：核心就是那道校验 ----
void User::returnBook(Book& book)
{
    if (m_borrownum <= 0)
    {
        cout << "  [还书失败] 当前没有借书记录，无法还书。" << endl;
        return;
    }

    // 第 1 步：在记录里找这本书
    int pos = -1;
    for (int i = 0; i < m_borrownum; i++)
    {
        if (m_records[i].m_book.getId() == book.getId())
        {
            pos = i;
            break;
        }
    }

    // 找不到 = 不是他借的，这就是本节要你加的那一道
    if (pos == -1)
    {
        cout << "  [还书失败] 《" << book.getTitle() << "》不是你借的，不能还。" << endl;
        return;
    }

    // 第 2 步：后面的往前挪一格，保持清单前面连续
    for (int i = pos; i < m_borrownum - 1; i++)
    {
        m_records[i] = m_records[i + 1];
    }

    // 第 3 步：数量减一，末尾那格清空
    m_borrownum--;
    m_records[m_borrownum] = BorrowRecord();

    // 第 4 步：馆藏真书恢复可借
    book.setIsBorrowed(false);
    cout << "  [还书成功] " << m_name << " 归还《" << book.getTitle()
         << "》，当前共 " << m_borrownum << " 本。" << endl;
}

void User::displayRecords()
{
    if (m_borrownum == 0)
    {
        cout << "  暂无借阅记录。" << endl;
        return;
    }
    cout << "  " << m_name << " 的借阅清单（共 " << m_borrownum << " 本）:" << endl;
    for (int i = 0; i < m_borrownum; i++)
    {
        cout << "    " << (i + 1) << ". 《" << m_records[i].m_book.getTitle()
             << "》  [id: " << m_records[i].m_book.getId() << "]" << endl;
    }
}
```

注意上面用了 `m_records[i].m_book` 而不是 `m_records[i].getBook()`。`BorrowRecord` 是 `struct`，里面 `m_book` 是公开的，直接拿得到；`getBook()` 返回的是**一份拷贝**（值返回），你改不到真东西。这是组合那一节讲的"是抄一份"在实际代码里的样子。

**真实输出**（编译运行过，逐字未改）：

```text
借两本：都应该成功
  [借书成功] YG 借到《C++ Primer》，当前共 1 本。
  [借书成功] YG 借到《算法导论》，当前共 2 本。
同一本再借一次：应该失败
  [借书失败] 你已经借过《C++ Primer》了。
借了 A 却想还 B：应该失败（本节最重要的一道校验）
  [还书失败] 《计算机网络》不是你借的，不能还。
  YG 的借阅清单（共 2 本）:
    1. 《C++ Primer》  [id: B001]
    2. 《算法导论》  [id: B002]
还 C++ Primer：应该成功，清单少一本
  [还书成功] YG 归还《C++ Primer》，当前共 1 本。
  YG 的借阅清单（共 1 本）:
    1. 《算法导论》  [id: B002]
还完再借计算机网络：应该成功
  [借书成功] YG 借到《计算机网络》，当前共 2 本。
  YG 的借阅清单（共 2 本）:
    1. 《算法导论》  [id: B002]
    2. 《计算机网络》  [id: B003]
三本书的真实状态
  《C++ Primer》状态: 可借
  《算法导论》状态: 已借出
  《计算机网络》状态: 已借出
把上限借满（m_maxnum = 5）：应该全成功
  [借书成功] YG 借到《填充书1》，当前共 3 本。
  [借书成功] YG 借到《填充书2》，当前共 4 本。
  [借书成功] YG 借到《填充书3》，当前共 5 本。
已借 5 本，再借一本：应该失败
  [借书失败] 已借 5 本，已达上限 5 本。
```

对着这段输出，回去看自己的实现，检查四件事：

1. **同一本借两次**，你的程序报的是"你已经借过"还是"被别人借走了"？报后者说明你把两道关的顺序写反了。
2. **借 A 还 B**，你的程序打印的是"还书失败"还是"还书成功"？报成功说明校验没加。
3. **还完一本之后**，清单里还剩几本、剩的是哪一本？如果《算法导论》不见了，说明你只做了 `m_borrownum--`，没做"往前挪"。
4. **三本书的真实状态**，还完之后《C++ Primer》必须是"可借"。如果它一直显示"已借出"，说明你只改了记录里的那一份拷贝，没改外面的馆藏真书 —— 这正是组合那一节"两份是不同的对象"的后果。

**五处最容易翻车的地方**：

- 判满用 `m_maxnum` 而不是 `60`。`60` 是数组的墙，`m_maxnum` 是"这人最多能借几本"的业务规则。两者不是一个概念，题目要的是后者。
- 判"是否已借过同一本"必须排在判"书已借出"**前面**。反过来，你自己借的那本会走"已被别人借走"的分支，输出是错的。
- `returnBook` 里 `pos == -1` 那一段不能省。省了就是"借 A 能还 B"。
- "往前挪"的循环上界是 `m_borrownum - 1`，不是 `m_borrownum`。写成 `m_borrownum` 会读到 `m_records[m_borrownum]`，那是清单外面的一格，里面是上一轮残留的数据 —— 现在不崩，等内存紧张时崩，答辩现场崩。
- 最后 `book.setIsBorrowed(false)` 别漏。少了这一句，书被还回来了但馆藏里永远显示"已借出"，下一节课的 `findBook` 再找它就找不到了。

</details>

---

### 三档任务和作业的关系

- **A** 让你看到组合的真相：`Student` 里的书和外面那本书是两个人。
- **B** 让你看到指针的必要性：要让改动穿透，参数就得收地址。
- **C** 是交上去的版本：对象数组合 + 借还 + 还书校验 + 上限保护。

三档都做完，第三课（`new` / `delete` / 指针进阶）才有意义 —— 因为你会带着一个具体的问题进去：

> 任务 C 里我抄了一份书进数组，馆藏那本书的状态是靠我手动 `setIsBorrowed` 同步的。如果同一个学生借了两次、或者两个学生同时借同一本，我这几处手动同步就散了。有没有一种写法，让"我借的"和"馆藏那本"本来就是同一个东西？

有。那就是下一课。先把 C 档跑通，跑到每一条失败分支都能打印出正确的失败原因，再往下走。
