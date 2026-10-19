## 三、引用传递：`Book& book` 到底绑定了什么

上一节看到的是失败的样子：值传递改不动原对象，两个地址不一样。这一节把地址对上，看引用是怎么做到"同一个对象"的。

### 3.1 只多了一个 `&`：两个版本并排

把两版函数贴在一起，你会看到全部差别就是**形参上多了一个字符**：

```cpp
// 值传递版
void passByValue(Book b)          // 形参是 Book 类型 —— 一个新的对象
{
    b.setIsBorrowed(true);
}

// 引用传递版
void passByRef(Book& b)           // 形参是 Book& 类型 —— 不是新对象，是别名
{
    b.setIsBorrowed(true);
}
```

函数体一个字都没变。`b.setIsBorrowed(true)` 这两行在源码里长得完全一样。

调用处的写法也完全一样：

```cpp
Book lib("C++ Primer", "Lippman", "9787121358579", 128.0);

passByValue(lib);      // 不加任何符号
passByRef(lib);        // 一样，也不加任何符号
```

三个地方摆在一起，**唯一不同的地方只有形参声明里的那个 `&`**：

| 位置 | 值传递版 | 引用传递版 | 差别 |
|------|---------|-----------|------|
| 形参声明 | `Book b` | `Book& b` | 多一个 `&` |
| 函数体 | `b.setIsBorrowed(true);` | `b.setIsBorrowed(true);` | 完全相同 |
| 调用处 | `passByValue(lib);` | `passByRef(lib);` | 完全相同 |

那为什么结果不同？因为那个 `&` 改的不是"怎么调用"，是**形参 `b` 这个身份本身**。

把两个版本各跑一次，同时在函数里打印形参的地址：

```cpp
void passByValue(Book b)
{
    cout << "    形参 &b             = " << &b << "      <- 和 lib 不一样" << endl;
    b.setIsBorrowed(true);
}

void passByRef(Book& b)
{
    cout << "    形参 &b             = " << &b << "      <- 和 lib 一模一样" << endl;
    b.setIsBorrowed(true);
}
```

真实输出（地址在你机器上会不一样，看关系就够）：

```
  lib 的地址            = 0000005398DFF640

  [值传递]
    形参 &b             = 0000005398DFF588      <- 和 lib 不一样
    调用后 lib 的状态    = 可借      <- 没改到

  [引用传递]
    形参 &b             = 0000005398DFF640      <- 和 lib 一模一样
    调用后 lib 的状态    = 已借出      <- 改到了
```

`&b` 打印出来的是 `b` 自己的地址。

- 值传递版打印 `...F588`，`lib` 在 `...F640` —— 两个不同的地址，说明 `b` 是另一块内存里的一本书。
- 引用传递版打印 `...F640`，和 `lib` 的地址一模一样 —— 说明 `b` 没有自己的内存，`&b` 问到的是 `lib` 的地址。

> **一句话记住**：值传递的形参有独立地址，引用传递的形参没有独立地址。这不是比喻，是 `&` 运算符当场能验证的事实。

### 3.2 `&` 这个符号在同一页里有三种身份

这是本章第一个必须单独点破的坑。初学者看到 `&` 就以为是"取地址"，看到 `Book&` 就懵了。实情是：**`&` 至少有三个不相干的身份，全靠上下文区分。**

```cpp
int x = 5;

int& a = x;        // 身份一：引用声明符 —— a 是 x 的另一个名字
int* p = &x;       // 身份二：取地址运算符 —— 取得 x 的内存地址
int c = x & 3;     // 身份三：按位与 —— 把 x 和 3 的二进制位逐个做与运算
```

三行代码，三个 `&`，三种完全不同的意思。而第一行和第三行的 `&` 在源码里长得几乎一样。

#### 判断口诀

**先看这一行是不是"声明一个变量"。**

| 这一行在干什么 | `&` 的意思是 | 例子 |
|---------------|-------------|------|
| 声明变量（有类型名 + 变量名） | **引用** | `Book& b = lib;`　`int& a = x;`　`void f(Book& b)` |
| 在一条声明里，跟在逗号后面 | 还是**引用**（声明延续） | `int &ra = x, &rb = x;` |
| 是表达式（在算一个值） | **取地址** | `p = &x;`　`f(&lib);`　`return &shelf[i];` |
| 两个整数值之间 | **按位与** | `x & 3` |

判据就一条：`&` 前面那个东西是**类型名**，还是**一个已经存在的值**。

- `Book &b = lib;` → `&` 前面是 `Book`，是个类型名，这一行在声明变量 → 引用。
- `p = &x;` → `&` 前面是 `=`，这一行在算一个值 → 取地址。

> **注意一个反直觉的地方**：`Book &b` 和 `Book& b` 和 `Book & b` **三种写法完全等价**，空格放在哪都合法，编译器只看 `&` 是不是在声明符里。所以"`&` 在类型后面就是引用、在变量前面就是取地址"这个说法是**错的** —— 因为 `Book &b` 里的 `&` 就紧挨在变量 `b` 前面，它仍然是引用。
>
> 别用空格位置做判断，用"这一行是不是声明"做判断。

#### 为什么这里特别容易混

因为同一个变量 `r`，你可以对它同时用两种身份，**两种写法都合法，但意思差着十万八千里**：

```cpp
Book lib("C++ Primer", "Lippman", "9787121358579", 128.0);

Book& r = lib;              // 引用：r 是 lib 的别名
Book* p = &r;               // 取地址：r 就是 lib，所以 &r 就是 lib 的地址

r.setIsBorrowed(true);      // 通过别名改一次
cout << &lib << "  " << &r << "  " << (&r == &lib) << "  " << lib.statusText() << endl;
```

第二行的 `&r` 拿到的地址，和 `&lib` **完全相等**。真实输出：

```
  &lib          = 0000005398DFF6D0
  &r            = 0000005398DFF6D0
  &r == &lib ?    true
  通过 r 改，lib 状态 = 已借出
```

`&r == &lib` 为 `true`，因为 `r` 根本不是另一个变量 —— 它就是 `lib`，只是多了个名字。

#### 第三条身份为什么也要提

因为 `a & b`（按位与）和 `Book& b`（引用声明）在肉眼上几乎没有区别，只差上下文。你在读别人代码时看到 `int & mask` 这种写法，只能靠"这一行在声明还是在算值"来判断。编译器不会含糊，人会。所以养成习惯：**看到 `&`，先问这一行在干什么。**

### 3.3 为什么说"引用不是新变量"

这不是口号，有三条可以当场验证的证据。

#### 证据一：`sizeof` 骗不了人

```cpp
cout << sizeof(Book)  << endl;     // 112   —— 我的简化版 Book（3 个 string），你项目那个会更大
cout << sizeof(Book&) << endl;     // 112   —— 完全一样
cout << sizeof(Book*) << endl;     // 8     —— 指针自己是个变量，占 8 字节
```

真实输出：

```
  sizeof(Book)   = 112
  sizeof(Book&)  = 112
  sizeof(Book*)  = 8
```

`sizeof(Book&)` 问的是"一个 `Book&` 占多大空间"，答案是 **112，等于被引用的那个对象本身**。因为它不是一块独立的内存，它就是那个对象。

指针的答案是 8 —— 指针**自己**是变量，存在栈上，占 8 字节（64 位下）。

把这个对比换成基本类型更直观：

```cpp
int  x = 5;
int& a = x;
int* px = &x;

cout << sizeof(a) << "   sizeof(px) = " << sizeof(px) << endl;
cout << "&a == &x ?   " << (&a == &x) << endl;
a = 99;
cout << "a = 99 之后 x = " << x << endl;
```

```
  sizeof(a) = 4   sizeof(px) = 8
  &a == &x ?   true
  a = 99 之后 x = 99
```

`a` 占 4 字节，`px` 占 8 字节。**如果 `a` 是个独立变量，它至少得有 8 字节装地址**。它没有，因为它不装地址 —— 它不存在。

#### 证据二：改 `a` 就是改 `x`，没有任何"传递"过程

```
  a = 99 之后 x = 99
```

`a` 和 `x` 之间没有"同步"这种动作，因为它们是同一块内存的两个名字。

#### 证据三：反汇编里，引用参数和指针参数一模一样

这一条最能说明问题。写两个函数，一个收引用、一个收指针，干的活完全相同：

```cpp
void byRef(Book& b) { b.setIsBorrowed(true); }
void byPtr(Book* b) { b->setIsBorrowed(true); }
```

用 `cl /FA` 生成汇编，两个函数的机器码**逐条相同**：

```
?byRef@@YAXAEAVBook@@@Z PROC
    mov     QWORD PTR [rsp+8], rcx
    sub     rsp, 40
    mov     dl, 1
    mov     rcx, QWORD PTR b$[rsp]
    call    ?setIsBorrowed@Book@@QEAAX_N@Z
    add     rsp, 40
    ret     0

?byPtr@@YAXPEAVBook@@@Z PROC
    mov     QWORD PTR [rsp+8], rcx
    sub     rsp, 40
    mov     dl, 1
    mov     rcx, QWORD PTR b$[rsp]
    call    ?setIsBorrowed@Book@@QEAAX_N@Z
    add     rsp, 40
    ret     0
```

（`rcx` 是 Windows x64 传第一个参数用的寄存器。`sub rsp, 40` 是在准备调用下一个函数所需的栈空间。）

两条指令序列一字不差。连编译器生成的异常展开记录也相同。唯一还能区分它们的是**函数名修饰**：`byRef` 修饰成 `?byRef@@YAXAEAVBook@@@Z`，里面 `AEA` 表示"引用"；`byPtr` 修饰成 `?byPtr@@YAXPEAVBook@@@Z`，里面 `PEA` 表示"指针"。差一个字母。

#### 三句话总结这里的分寸

| 层面 | 说的是什么 | 证据 |
|------|-----------|------|
| 语言标准 | 引用**不是对象**，它没有地址、没有大小，它只是一个已经存在的对象的别名 | `sizeof(Book&) == sizeof(Book)`；`&r == &lib` |
| 编译器实现 | 为了把"我指的是哪个对象"这个消息传进函数，编译器**必须**传一个地址过去。所以在寄存器/栈上，它用的就是指针的做法 | 上面那段逐条相同的汇编 |
| 你写代码时该怎么理解 | **按别名理解**。不要说"引用是个指针"，那样你会去猜它占几个字节、能不能改指向，全是错的 | `ref = b2` 不是改指向，是赋值（见 3.5） |

> **答辩时如果被问"引用和指针在底层是不是一回事"**：答"语义上不是 —— 引用不是对象，标准规定它没有地址和大小；实现上编译器通常用地址来实现它，所以反汇编看起来一样。这两句话不矛盾，一个说的是语言规则，一个说的是某台机器上的落地方式。"

### 3.4 内存图

值传递（第二节那个失败版本）：

```
main 的栈帧                                 passByValue 的栈帧
lib
&lib = 0x...F640                            b
┌────────────────────────┐                 &b = 0x...F588
│ m_title   "C++ Primer" │                 ┌────────────────────────┐
│ m_author  "Lippman"    │                 │ m_title   "C++ Primer" │ ← 字符串另拷一份
│ m_ISBN    "9787..."    │                 │ m_author  "Lippman"    │
│ m_price         128.0  │                 │ m_ISBN    "9787..."    │
│ m_isBorrowed    false  │                 │ m_price         128.0  │
└────────────────────────┘                 │ m_isBorrowed    false  │ ← 两个独立的 bool
         ▲                                 └────────────────────────┘
         │                                          │
         └── 函数里改成 true 改的是右边这份 ─────────┘
             左边这份永远不动
```

引用传递：

```
main 的栈帧                                 passByRef 的栈帧
lib
&lib = 0x...F640                            b 这个名字没有自己的格子
┌────────────────────────┐
│ m_title   "C++ Primer" │ ◄────────────────  rcx 寄存器里装着 0x...F640
│ m_author  "Lippman"    │                   （地址进了函数，但外面没有多出
│ m_ISBN    "9787..."    │                    一块叫 b 的内存）
│ m_price         128.0  │
│ m_isBorrowed    false  │
└────────────────────────┘
         ▲
         └── 函数里执行 b.setIsBorrowed(true)
             就是在这块内存上执行 setIsBorrowed(true)
```

指针传递（第四节细讲，先放在这里对照）：

```
main 的栈帧                                 passByPtr 的栈帧
lib
&lib = 0x...F640                            b（指针变量，自己住 0x...F5A0）
┌────────────────────────┐                 ┌────────────────────────┐
│ m_title   "C++ Primer" │ ◄──────────────── │ 值 = 0x...F640          │
│ m_author  "Lippman"    │                   └────────────────────────┘
│ m_ISBN    "9787..."    │                     b 自己有地址，里面装的是
│ m_price         128.0  │                     书的门牌号
│ m_isBorrowed    false  │
└────────────────────────┘
```

三张图放一起，差别就是一句话：

- **值**：函数里多了一整本书（拷贝）
- **引用**：函数里一个名字，直接指到原书
- **指针**：函数里多了一个 8 字节的小盒子，盒子里装着原书的门牌号

### 3.5 引用的四条硬规则

这四条是从编译器那里学来的，不是约定 —— 违反任何一条都编不过，或者编过了但行为不是你想的那样。

#### 规则一：必须初始化，不能先声明后绑

```cpp
Book& r;              // 编译不过
```

MSVC 的报错原文（注意它的措辞顺序）：

```
error C2530: “r”: 必须初始化引用
```

引用一旦存在，就必须有指向。所以不能写 `Book& r; r = lib;` —— 第二行也不是"绑定"，那是给某个对象赋值（而且第一行已经编不过了）。

正确写法只有一种：声明的同时绑上。

```cpp
Book& r = lib;
```

如果业务上真的需要"先空着，晚点再决定指向谁"，那就该用指针：

```cpp
Book* p = nullptr;    // 先空着
p = &lib;             // 以后再指向
```

这条规则直接决定了第四节"什么时候必须用指针"。

#### 规则二：不能改绑，`ref = b2` 是赋值不是改指向

这是第二个大坑，比第一个更隐蔽 —— 因为它**编译通过、运行不崩、只是行为不是你想的那样**。

```cpp
Book b1("书A", "甲", "I-A", 1.0);
Book b2("书B", "乙", "I-B", 2.0);

Book& ref = b1;      // ref 绑到 b1
ref = b2;            // 你以为：ref 改绑到 b2
                     // 实际上：把 b2 的每个成员逐个抄进 b1

cout << "  ref = b2 之后：b1 标题 = " << b1.getTitle() << endl;
cout << "  &ref == &b1 ?    " << (&ref == &b1) << endl;
```

真实输出：

```
  ref = b2 之后：b1 标题 = 书B
  &ref == &b1 ?    true
```

两个结果一起看：

1. `b1` 的标题变成了 `书B` —— 说明 `b1` 被**改写了**。
2. `&ref == &b1` 仍然是 `true` —— 说明 `ref` **没有换绑**，它还绑在 `b1` 身上。

所以 `ref = b2` 的真实含义是"`b1 = b2`"。**引用从来不会换指向，这只是普通赋值。**

对比指针：

```cpp
Book b3("书C", "丙", "I-C", 3.0);
Book b4("书D", "丁", "I-D", 4.0);

Book* p = &b3;
p = &b4;             // 真·改指向

cout << "  p = &b4 之后：p 指向 = " << p->getTitle() << endl;
cout << "  b3 标题还是 = " << b3.getTitle() << endl;
```

```
  p = &b4 之后：p 指向 = 书D
  b3 标题还是 = 书C
```

`p` 现在指着 `b4`，而 `b3` 一个字节都没被碰。想换指，只能用指针。

> **第三课和第五课的伏笔**：正因为引用不能换指向，容器（`vector`）里不能放引用，只能用指针或对象副本。这一条现在先记住结论。

#### 规则三：引用不能为空

C++ 里没有"空引用"这种东西。一个引用在它整个生命周期里，永远绑在一个真实存在的对象上。

```cpp
Book* p = nullptr;                       // 合法
if (p != nullptr) { p->display(); }      // 所以这里要判空
cout << "  （上面什么都没打印，因为指针是空的，if 挡住了）" << endl;

Book& r = ???;                           // 没有"空引用"可以写
```

真实输出：

```
  （上面什么都没打印，因为指针是空的，if 挡住了）
```

注意这一行字不是程序打印的提示 —— **程序本身什么都没干**，是我在 `if` 后面自己加了一句 `cout` 你才看到字。如果你的代码里没有那句 `cout`，屏幕上就是一片空白。这就是判空的意义：**不判空不是"跳过"，是崩溃。**

这条是**优点还是缺点，取决于场景**：

- 写 `borrowBook(Book& book)` 时，它是优点 —— 编译器替你保证了"传进来的一定是一本真实存在的书"，你不需要写任何判空代码。
- 写"查书，查不到就返回"时，它是缺点 —— 你没法用引用表达"没有这本书"，只能用指针返回 `nullptr`，或者返回一个下标（`-1` 表示没有）。

这也是 4.4 节第一个"必须用指针"的场景。

#### 规则四：调用处不加 `&`

```cpp
passByRef(lib);       // 对的
passByRef(&lib);      // 编译不过
```

报错原文：

```
error C2664: “void passByRef(Book &)”: 无法将参数 1 从“Book *”转换为“Book &”
note: 参见“passByRef”的声明
note: 尝试匹配参数列表“(Book *)”时
```

`Book*` 和 `Book&` 是两个完全不相干的类型，编译器不给它们互转。

这是引用比指针舒服的地方：调用者看代码时，`f(lib)` 这个写法跟值传递**完全一样**，不需要记住"这个函数收地址还是收值"。

代价是：**看调用处看不出来它会不会被改**。这是引用在工程上唯一的缺点，只能靠函数签名（`Book&` 还是 `const Book&`）来读。

### 3.6 落回实验二：借书为什么 `Book&` 就够了

作业原文写的是"参数必须是地址（指针或引用）"—— 两者都合规。你选引用，要能说清理由。三条：

**第一条：调用处读起来像人话。**

```cpp
yg.borrowBook(b1);       // 引用：yg 借 b1
yg.borrowBook(&b1);      // 指针：yg 借 b1 的地址
```

第二种写法里那个 `&` 是纯噪音。你在 `main` 里会给五六本书反复调这个函数，每处都多一个 `&`，每处都是一个可能敲漏的地方。

**第二条：引用不可能为空，省掉一整类防御代码。**

用指针就得这么写：

```cpp
bool borrowBook(Book* book)
{
    if (book == nullptr)          // 必须写，否则 nullptr 传进来直接崩
    {
        return false;
    }
    // ...
}
```

用引用，这两行直接不存在。答辩时老师问"你的函数怎么处理空指针"，你答"参数是引用，引用不可能为空，所以这一类输入在类型层面就被排除了" —— 这是一个满分的回答，因为它说明你知道**编译器替你挡住了什么**。

**第三条：函数体里访问成员用 `.`，不用 `->`，少一个出错点。**

```cpp
void borrowBook(Book& book)
{
    book.setIsBorrowed(true);      // 就是点
}
```

指针版每一处都要写 `book->setIsBorrowed(true)`。零基础最常见的错就是 `->` 和 `.` 用混（4.2 节专门讲），用引用直接从根上消掉这个错。

**那么指针留给谁？** 留给"可能没有"和"要换指"的地方。比如你要写一个"在书库里找书"的函数，找不到是很正常的结果：

```cpp
Book* findBook(Book shelf[], int n, const string& title);   // 找不到返回 nullptr
```

这个函数**没法**用引用返回值 —— 因为引用必须给出一个真实存在的对象，而"找不到"时没有对象可给。

**怎么在签名里一眼看出这本书会不会被改？**

| 签名 | 能改实参吗 | 会拷贝吗 | 什么时候用 |
|------|-----------|---------|-----------|
| `void f(Book b)` | 不能 | 会，整个对象拷一份 | 函数里需要的是一份可以随便糟蹋的副本 |
| `void f(Book& b)` | **能** | 不会 | 要把结果写回这本书（`borrowBook` / `returnBook`） |
| `void f(const Book& b)` | 不能（编译器强制） | 不会 | 只读，但对象很大不想拷贝（`display` / `printBook`） |

这三行就是本章签名选择的全部规则。你项目里 `borrowBook(Book& book)`、`returnBook(Book& book)` 属于第二行，是对的 —— 它们都要执行 `book.setIsBorrowed(...)`。

> **答辩常问的连环问题**："为什么 `borrowBook` 不加 `const`？" —— 答"因为它要改这本书的状态"。如果你真把它写成 `void borrowBook(const Book& book)` 再在里面调 `book.setIsBorrowed(true);`，MSVC 会当场拦住：
>
> ```
> error C2662: “void Book::setIsBorrowed(bool)”: 不能将“this”指针从“const Book”转换为“Book &”
> note: 转换丢失限定符
> note: 参见“Book::setIsBorrowed”的声明
> ```
>
> 读法是：`const Book&` 传进来的 `this` 是 `const Book*`，而 `setIsBorrowed` 的 `this` 要的是 `Book*`，`const` 丢了，不许。（`const` 的完整讲法在第五节。）

---

## 四、指针传递：和引用的区别，什么时候非它不可

### 4.1 最小语法对照

三个版本并排，把差别一次看清：

```cpp
// 值传递
void byValue(Book b)
{
    b.setIsBorrowed(true);        // 用点
}

// 引用传递
void byRef(Book& b)
{
    b.setIsBorrowed(true);        // 也是用点
}

// 指针传递
void byPtr(Book* b)
{
    b->setIsBorrowed(true);       // 用箭头
}

Book lib("C++ Primer", "Lippman", "9787121358579", 128.0);

byValue(lib);                     // 直接传
byRef(lib);                       // 直接传
byPtr(&lib);                      // 必须加 & 取地址
```

一张最小的对照：

| | 值 `Book b` | 引用 `Book& b` | 指针 `Book* b` |
|---|---|---|---|
| 形参是什么 | 一个新对象（副本） | 原对象的别名 | 一个装地址的变量 |
| 调用处怎么写 | `f(lib)` | `f(lib)` | `f(&lib)` |
| 函数里访问成员 | `b.getTitle()` | `b.getTitle()` | `b->getTitle()` |
| 能改到原对象吗 | 不能 | 能 | 能 |
| 形参自己的地址 | 和实参不同 | 和实参相同 | 自己有一块内存，值是实参地址 |

指针版跑出来的真实输出，注意第一行和第二行是**同一个数字**：

```
  lib 的地址 = 000000687FD9F740
  形参 b(值)   = 000000687FD9F740      <- b 里存的值，就是 lib 的地址
  调用后 lib 的状态 = 已借出
```

`b` 这个变量自己有地址（在栈帧里），里面**装的内容**是 `lib` 的地址。所以指针是"两层"：一层是变量自己，一层是它指向的东西。

引用只有一层。这就是两者最本质的结构差别。

### 4.2 `->` 和 `.`：零基础在这里必错

先把规则摆死：

> **左边是一个对象（或对象的引用）→ 用 `.`
> 左边是一个指针 → 用 `->`**

就这一句。但零基础几乎一定会写错一次，因为两种写法在源码里看起来"差不多"。

#### `->` 到底是什么

`p->f()` 是 `(*p).f()` 的缩写。展开读一遍：

```cpp
p->setIsBorrowed(true);
// 等价于
(*p).setIsBorrowed(true);
```

意思是：**先顺着 `p` 里存的地址找到那个对象（`*p`），再调它的 `setIsBorrowed`。**

那为什么不直接让你写 `(*p).f()`？因为 `*` 的优先级比 `.` 低。你写

```cpp
*p.setIsBorrowed(true);
```

编译器会先算 `p.setIsBorrowed`（对指针调成员，直接错），再对结果解引用。为了让"顺着指针取对象再取成员"这个高频操作写起来方便，C 语言设计了 `->` 这个符号。

> **记忆法**：箭头 `->` 本身就长得像"从指针走过去"—— 一横是门牌号，箭头指向那栋房子。`.` 是个点，点在一个东西上面，那个东西就在你手边。

#### 写错了会报什么

**错误一：指针上用了 `.`**

```cpp
Book* p = &lib;
p.setIsBorrowed(true);        // 错
```

```
error C2228: “.setIsBorrowed”的左边必须有类/结构/联合
note: 类型为 'Book *'
note: 是否改用“->”?
```

注意第三行 —— **编译器直接把答案告诉你了**。看到 `是否改用 ->` 这四个字，不用再想，就是这个问题。

**错误二：对象上用了 `->`**

```cpp
Book lib;
lib->setIsBorrowed(true);     // 错
```

```
error C2819: “Book”类型没有重载成员“operator ->”
note: 参见“Book”的声明
note: 是否改用“.”?
error C2232: “->Book::setIsBorrowed”: 左操作数有“class”类型，使用“.”
```

这次报了两条 error，第二条更直白：`左操作数有“class”类型，使用“.”`。

> **看报错的规律**：只要 MSVC 的 note 里出现 `是否改用“->”?` 或者 `是否改用“.”?`，那就是 `->` 和 `.` 用反了，改符号就行，不用查别的地方。这是全套报错里最容易被解决的一类，前提是你认得它。

#### 一个连锁的坑：`*p.f()` 为什么也不行

```cpp
*p.setIsBorrowed(true);       // 想表达"对 p 指向的对象调成员"
```

这不是"少写了括号"，是真的语法错。`*` 和 `.` 的优先级决定了它被解析成 `*(p.setIsBorrowed(true))` —— 先对指针 `p` 用 `.`，第一步就报 `C2228`，和你直接写 `p.setIsBorrowed` 报的是一模一样的三行。

要么加括号 `(*p).setIsBorrowed(true);`，要么直接用 `p->setIsBorrowed(true);`。工程上一律用后者。

#### 为什么指针数组特别容易混

因为这两行都合法，但意思完全不同：

```cpp
Book  shelf[3];        // 对象数组：shelf[i] 是一本书（对象）
Book* ptrShelf[3];     // 指针数组：ptrShelf[i] 是一个地址

shelf[0].getTitle();       // 点 —— shelf[0] 是对象
ptrShelf[0]->getTitle();   // 箭头 —— ptrShelf[0] 是指针
```

判断方法：**看 `[]` 前面是什么。** `Book shelf[]` 取出来的东西是 `Book`，用点；`Book* ptrShelf[]` 取出来的东西是 `Book*`，用箭头。

两条声明长得只差一个 `*`，取出的东西却差一整层 —— 这是 `->` 和 `.` 最容易翻车的地方，因为数组下标 `[0]` 把注意力全吸走了，你会忘了看前面那个 `*`。

### 4.3 一张必须背下来的对照表

| 维度 | 引用 `Book&` | 指针 `Book*` |
|------|-------------|-------------|
| 能不能为空 | **不能**，永远绑一个真实对象 | **能**，可以是 `nullptr` |
| 必须初始化吗 | 必须，`Book& r;` 编不过（`C2530`） | 不必，可以先 `Book* p = nullptr;` |
| 能不能重新指向别的对象 | **不能**，`ref = b2` 是赋值（3.5 规则二） | **能**，`p = &b2;` |
| 形参怎么写 | `void f(Book& b)` | `void f(Book* b)` |
| 调用处怎么写 | `f(lib)` | `f(&lib)` |
| 访问成员 | `b.getTitle()`　用点 | `b->getTitle()`　用箭头 |
| 要不要解引用 | 不用，拿到就是对象本体 | 要，`*b` 才是对象，`b->` 是"取对象再取成员" |
| 加 `const` 的写法 | 只有一种：`const Book&` | 三种：`const Book*`（锁数据）、`Book* const`（锁指针）、`const Book* const`（两头都锁） |
| 能不能做算术 | **不能**，没有 `r + 1` 这种东西 | **能**，`p + 1` 是下一个元素 |
| 占多少空间 | 不占（`sizeof(Book&) == sizeof(Book)`） | 8 字节（64 位） |
| 用错时的典型事故 | 几乎没有意外 —— 编译器帮你看住了一切 | 空指针崩溃、悬空指针、忘记解引用、忘记 `delete` |
| 典型用途 | 函数参数（要改或只读大对象）、返回值、给变量起别名 | 可能没有的东西、要换指、动态内存、遍历数组、容器里存位置 |

这张表的用法：**遇到一个"要不要用指针"的选择，从上往下扫一遍，只要有一行落在右边，就必须用指针；全是左边，就用引用。**

> **`const` 那两行的记忆法**：`const` 在 `*` 左边锁数据，在 `*` 右边锁指针。引用没有"右边"（引用天然不能改指向），所以引用只有 `const Book&` 一种写法。完整的 `const` 位置讲法在第五节。

### 4.4 什么时候必须用指针（引用做不到的四件事）

四条，每条一个能跑的例子。

四条共用一个前提 —— 下面这段 `Book` 和这个书库数组，粘在 `main` 开头就能把四条全部跑通，输出和本节贴的一致：

```cpp
class Book
{
private:
    string m_title;
    string m_author;
    string m_ISBN;
    double m_price;
    bool   m_isBorrowed;

public:
    Book() : m_title("(空)"), m_author(""), m_ISBN(""), m_price(0.0), m_isBorrowed(false) {}
    Book(string title, string author, string isbn, double price)
        : m_title(title), m_author(author), m_ISBN(isbn), m_price(price), m_isBorrowed(false) {}

    string getTitle()      const { return m_title; }
    string getAuthor()     const { return m_author; }
    bool   getIsBorrowed() const { return m_isBorrowed; }
    void   setTitle(const string& t) { m_title = t; }
    void   setIsBorrowed(bool b) { m_isBorrowed = b; }
    string statusText()    const { return m_isBorrowed ? "已借出" : "可借"; }

    void display() const
    {
        cout << "    《" << m_title << "》 " << m_author
             << " / " << m_ISBN << " / " << m_price << " 元 / " << statusText() << endl;
    }
};

Book shelf[3] = {
    Book("C++ Primer",    "Lippman", "9787121358579", 128.0),
    Book("算法导论",      "Cormen",  "9787111407010", 128.0),
    Book("Effective C++", "Meyers",  "9787111492245", 65.0)
};
```

`sizeof(Book)` 是 112：三个 `string`（MSVC 下每个 32 字节）共 96、一个 `double` 8 字节、一个 `bool` 1 字节，补齐到 8 的倍数正好 112。后面所有 `sizeof` 的数字都以这个为准，你在自己机器上跑出来必须一模一样 —— 对不上就说明你的 `Book` 成员和上面不同。

#### 第一件：这个参数"可能没有"

引用必须绑在一个真实对象上，所以它没法表达"没有"。而"找东西找不到"是程序里最常见的合法结果。

```cpp
// 在书库里找一本书，找到返回它的地址，找不到返回 nullptr
Book* findBook(Book shelf[], int n, const string& title)
{
    for (int i = 0; i < n; i++)
    {
        if (shelf[i].getTitle() == title)
        {
            return &shelf[i];         // 找到 —— 返回这一格的地址
        }
    }
    return nullptr;                   // 找不到 —— 只有指针能表达这个
}
```

调用方必须判一下：

```cpp
Book* p = findBook(shelf, 3, "算法导论");     // 书库里有
if (p != nullptr)
{
    p->display();
}
else
{
    cout << "书库里没有这本书" << endl;
}

Book* q = findBook(shelf, 3, "不存在的书");   // 书库里没有
if (q != nullptr)
{
    q->display();
}
else
{
    cout << "书库里没有这本书" << endl;
}
```

真实输出：

```
    《算法导论》 Cormen / 9787111407010 / 128 元 / 可借
    书库里没有这本书
```

第二次调用返回的 `nullptr` 被 `if` 挡住了 —— 那一行 `书库里没有这本书` 才是重点：**程序没有崩，它正常地表达了一个"没有"**。换引用写，`findBook` 找不到时无值可返。

> **你项目里的现状**：`LibrarySys.cpp` 的 `int findBook(vector<Book>& books, const string& title)` 返回的是**下标**，找不到返回 `-1`。这就是"用 int 假装一个可能不存在的位置"，能跑，但要靠 `-1` 这个约定俗成的魔法值，调用方还得自己记住 `if (idx >= 0)`。用指针写就是 `Book*`，`nullptr` 是类型系统自带的"没有"，比 `-1` 干净。两种写法都对，答辩时能说出这个取舍就是加分。

#### 第二件：需要反复指向不同的对象

遍历一个对象数组时，你需要一个"游标"在元素之间移动。引用做不到这件事 —— 而且比"做不到"更糟：**硬写会静默改坏数据**。

```cpp
Book* cursor = &shelf[0];
for (int i = 0; i < 3; i++)
{
    cursor->display();
    ++cursor;                 // 换到下一个元素 —— 指针算术
}
```

指针 `+ 1` 的含义是"往后跳 `sizeof(Book)` 个字节"，正好落在下一个元素上。这是编译器按类型自动算的，你不需要知道 `Book` 到底多大。

引用版会长什么样：

```cpp
Book& cursor = shelf[0];
cursor = shelf[1];     // 你以为在换指向
                       // 实际在把 shelf[1] 整个抄进 shelf[0]！
```

这就是 3.5 规则二那个坑。**不是"引用不能换指向"这么轻描淡写，是"写出来会悄悄改坏数据"。** 所以需要移动的场景，一律用指针，别用引用试。

真实输出（指针版）：

```
    《C++ Primer》 Lippman / 9787121358579 / 128 元 / 可借
    《算法导论》 Cormen / 9787111407010 / 128 元 / 可借
    《Effective C++》 Meyers / 9787111492245 / 65 元 / 可借
```

#### 第三件：要在函数里改"指针本身"

这个场景很常见，但看起来很不像。比如"把指针挪到某本书上"这个动作，本身要改的是**指针变量**，不是它指向的对象。

```cpp
// 想改的是调用方那个指针 q，不是 q 指向的书
void pointToValue(Book* p, Book& target)
{
    p = &target;              // 只改了形参 p（一份拷贝），外面的 q 纹丝不动
}

void pointToRef(Book*& p, Book& target)
{
    p = &target;              // 改的是外面那个 q 本人
}
```

`Book*&` 怎么读：**从右往左读**。`&` 是引用，`*` 是指针，`Book` 是类型 —— "`p` 是一个引用，绑在一个 `Book*`（指向 Book 的指针）上"。

也就是"指针的引用"。传进来的是指针变量本身，不是它的副本。

真实跑一遍：

```cpp
Book b3("书C", "丙", "I-C", 3.0);
Book b4("书D", "丁", "I-D", 4.0);
Book* q = &b3;

pointToValue(q, b4);
cout << "  值传递版本后 q 指向 = " << q->getTitle() << "   <- 没改到" << endl;

pointToRef(q, b4);
cout << "  引用传递版本后 q 指向 = " << q->getTitle() << "   <- 改到了" << endl;
```

真实输出：

```
  值传递版本后 q 指向 = 书C   <- 没改到
  引用传递版本后 q 指向 = 书D   <- 改到了
```

同样的事情也能用二级指针写：

```cpp
void pointToPtr(Book** pp, Book& target)
{
    *pp = &target;            // pp 是"指向指针的指针"，解一层才是指针本身
}
```

`Book**` 读作"指向 `Book*` 的指针"。能用 `Book*&` 就别用 `Book**` —— 后者多一层解引用，每处访问都要多一个 `*`，而且调用处得写 `pointToPtr(&q, b4);`。`Book*&` 调用处写 `pointToRef(q, b4);`，干净。

> **这条同时说明了一个反直觉的事实**：引用不只能绑对象，也能绑指针变量。`Book*&` 就是"绑在一个指针上的引用"。你项目里 `User::borrowBook(Book& book)` 的 `book` 是"绑在一个 Book 上的引用"，机理完全一样。

#### 第四件：和 C 风格数组、动态内存打交道

这两样东西的接口天生就是指针，没有选择余地。

**C 风格数组当参数时会"退化"成指针：**

```cpp
void printShelf(Book arr[], int n)          // 看起来收的是数组
{
    cout << "    函数里 sizeof(arr) = " << sizeof(arr) << "   <- 退化成了指针" << endl;
    for (int i = 0; i < n; i++)
    {
        arr[i].display();
    }
}
```

真实输出：

```
    main 里 sizeof(shelf) = 336
    函数里 sizeof(arr) = 8   <- 退化成了指针
    《C++ Primer》 Lippman / 9787121358579 / 128 元 / 可借
    《算法导论》 Cormen / 9787111407010 / 128 元 / 可借
    《Effective C++》 Meyers / 9787111492245 / 65 元 / 可借
```

`main` 里 `sizeof(shelf)` 是 336（3 个元素 × 112 字节）。传进函数后只剩 8 —— 因为 `Book arr[]` 这个写法在形参位置上**就等于** `Book* arr`，数组的"长度信息"丢了。

这解释了两件事：

1. 为什么 `printShelf` 必须多收一个 `int n` —— 函数里已经算不出元素个数了。
2. 为什么你在 `main` 里写 `sizeof(shelf) / sizeof(shelf[0])` 能算长度（336 / 112 = 3），而在函数里这么写算出来永远是错的（8 / 112 → 0，两个指针相除也只能得 1）。**数组长度必须在数组还在的作用域里算好再传进去。**

**动态内存只认指针：**

```cpp
Book* makeBook(const string& t, const string& a)
{
    return new Book(t, a, "ISBN-NEW", 50.0);   // new 返回的永远是地址
}

Book* p = makeBook("堆上的书", "某人");
p->display();
cout << "地址 = " << p << "   sizeof(*heapBook) = " << sizeof(*p) << endl;
delete p;                          // delete 只接受指针
p = nullptr;                       // 删完置空，防悬空
cout << "delete 之后 = " << p << endl;
```

```
    《堆上的书》 某人 / ISBN-NEW / 50 元 / 可借
    地址 = 0000027DCAA250C0   sizeof(*heapBook) = 112
    delete 之后 = 0000000000000000
```

`new` 的返回类型就是 `Book*`，`delete` 的形参就是指针。整个动态内存机制建立在指针上，绕不过去。

> **第五章会撞上的坑**：`new` 出来的内存不会自己释放，`delete` 之后的指针不会自己变空（上面那行 `p = nullptr` 是你手动写的，不写它就是野指针）。用引用就完全不会有这两个问题 —— 引用既不 `new` 也不 `delete`，它只是别人的另一个名字。

### 4.5 落回作业：`User` 里存书目，对象数组还是指针数组

这是本章最后一处需要你自己做判断的地方，也是答辩很可能追问的点。

你的 `User` 要记下"这个人借了哪几本书"。有两种存法，都合法，后果完全不同。

#### 写法 A：对象数组，存副本（项目现在用的）

```cpp
struct BorrowRecord
{
    Book m_book;              // 一整个 Book 对象住在这里
};

class User
{
private:
    BorrowRecord m_records[60];
    int m_borrownum = 0;
};
```

（本节后面为了打印 `sizeof` 时数字好读，统一把 60 缩成 3。你项目里是 60，比例关系一样。）

借书时：

```cpp
m_records[m_borrownum].m_book = book;    // 把 book 的每个成员抄进这一格
m_borrownum++;
```

#### 写法 B：指针数组，存地址

```cpp
class User
{
private:
    Book* m_books[3];        // 只存门牌号，每格 8 字节
    int   m_borrownum = 0;

public:
    User()
    {
        for (int i = 0; i < 3; i++)
        {
            m_books[i] = nullptr;      // 必须手动初始化！
        }
    }
};
```

借书时：

```cpp
m_books[m_borrownum] = &book;    // 只存地址，书还是书库那一本
m_borrownum++;
```

#### 两种写法跑起来差在哪

同一本书，借出去之后改个书名，两种记录里看到的完全不同。先把两边的读写接口对齐（都缩到 3 格、只留能对比的部分）：

```cpp
class UserA
{
private:
    BorrowRecord m_records[3];
    int          m_borrownum = 0;
public:
    void borrow(Book& book) { m_records[m_borrownum].m_book = book; m_borrownum++; }
    const Book& record(int i) const { return m_records[i].m_book; }
};

class UserB
{
private:
    Book* m_books[3];
    int   m_borrownum = 0;
public:
    UserB() { for (int i = 0; i < 3; i++) { m_books[i] = nullptr; } }
    void borrow(Book& book) { m_books[m_borrownum] = &book; m_borrownum++; }
    Book* record(int i) const { return m_books[i]; }
};
```

注意两个 `record` 的返回类型：A 给的是 `const Book&`，B 给的是 `Book*` —— 这不是随便定的，它决定了调用处写 `.` 还是 `->`（见本节末尾）。

```cpp
Book lib("C++ Primer", "Lippman", "9787121358579", 128.0);
UserA ua;
UserB ub;
lib.setIsBorrowed(true);
ua.borrow(lib);
ub.borrow(lib);

lib.setTitle("C++ Primer 第5版");
cout << "  写法A（对象数组）记录里看到 = 《" << ua.record(0).getTitle() << "》" << endl;
cout << "  写法B（指针数组）记录里看到 = 《" << ub.record(0)->getTitle() << "》" << endl;
```

真实输出：

```
[借完之后改书名]
  写法A（对象数组）记录里看到 = 《C++ Primer》
  写法B（指针数组）记录里看到 = 《C++ Primer 第5版》
```

再改书的状态（还书）：

```cpp
lib.setIsBorrowed(false);
cout << "  写法A（对象数组）= " << ua.record(0).statusText() << "   <- 副本，跟书库脱节" << endl;
cout << "  写法B（指针数组）= " << ub.record(0)->statusText() << "   <- 同一本，实时同步" << endl;
```

```
[还书之后，记录里看到的这本书状态]
  写法A（对象数组）= 已借出   <- 副本，停在借出那一刻
  写法B（指针数组）= 可借   <- 同一本，书库改了它就跟着改
```

A 的记录里这本书永远显示"已借出"，因为它是借出那一瞬间的复印件；书库那边早就还了，它不知道。B 的记录里跟着变回"可借"，因为它看的就是书库那一本。

还有一点容易忽略：**B 那行 `ub.record(0)->statusText()` 里，`record(0)` 返回的是 `Book*`，所以后面跟 `->`；A 那行 `ua.record(0).statusText()` 返回的是 `Book&`，所以跟 `.`**。两种写法在调用处就长得不一样，这是 4.2 那条规则在真实代码里的样子。

尺寸也差着数量级：

```
  sizeof(Book)         = 112
  sizeof(BorrowRecord) = 112
  sizeof(UserA)        = 344    3 格副本 + 计数
  sizeof(UserB)        = 32     3 个指针 + 计数
```

344 对 32 —— 十倍还多。你的项目里是 60 格：写法 A 的 `User` 光记录就占 `60 × sizeof(Book)`，按项目那个 `Book`（5 个 `string` + `double` + `bool`，`sizeof` 在 200 上下）算，一个学生对象就得一万二千字节；写法 B 是 `60 × 8 = 480` 字节。

（这里用的是精简版 `Book`，3 个 `string`。你项目里那个多两个 `string`，每一格的绝对值都会更大，但两类写法的比例关系不变：**对象数组每格要装一整本书，指针数组每格恒定 8 字节**。）

#### 后果对照

| | 写法 A　对象数组 | 写法 B　指针数组 |
|---|---|---|
| 记录里存的是什么 | 一整本书的完整副本 | 一个地址 |
| 每格占多少 | `sizeof(Book)`，你的项目里两百字节上下 | 固定 8 字节 |
| 书库那本书改名/改价后，记录跟着变吗 | **不跟**（是快照） | **跟**（同一本书） |
| 书库那本书被销毁后 | 记录仍然完整，安全 | **指针悬空**，一访问就崩 |
| 要求 `Book` 有默认构造函数吗 | **要求**（数组要逐个默认构造） | 不要求 |
| 删除中间一条记录要搬多少字节 | `sizeof(Book)` | 8 字节（只搬指针） |
| 删除记录时要不要负责 `delete` | 不用，自动析构 | **要**，否则内存泄漏 |
| 未初始化时是什么 | `Book` 的默认构造结果（安全） | **垃圾地址**（见下） |

最后一行值得单独看一眼。内置指针数组不做任何初始化：

```cpp
Book* raw[3];       // 没有初始化
Book  objs[3];      // 类类型数组，会自动调用默认构造函数
```

```
[内置指针数组不初始化]
  raw[0] = 0000000000000000
  raw[1] = 0000000000000000
  raw[2] = 000001BDB40076B0
[类类型数组自动构造]
  objs[0].getTitle() = (空)
```

**你跑一遍会得到和我不同的数字，这本身就是重点。** 我这台机器上前两格恰好是 `0`、第三格是垃圾；换台机器、换个编译选项、换个函数，可能三格全是垃圾，也可能全是 `0`。这个"有时看起来没事、有时炸"的特性正是指针最难查的地方 —— 你测试时它恰好是空的，一交上去老师在另一个环境里跑就崩。

`raw[0]` 里那串东西看着像个地址，顺着它访问就是一次崩溃。而 `objs[0]` 是一本构造好的、状态确定的书，`getTitle()` 一定返回合法 `string`。

这就是 4.5 开头那句"必须手动初始化"的原因，也是指针最容易出事的地方：**类类型数组会自动把每个元素构造成合法状态，指针数组什么都不做。**

#### 那到底该用哪个

**你的作业用对象数组（写法 A）。四个理由：**

1. **`BorrowRecord` 的语义是"借条"，不是"书的实时视图"。** 借条记的是"他借了哪本书、什么时候借的"，是借出那一瞬间的快照。书库后来把书名改成第 5 版，借条上应该还是当时那个书名 —— 这是对的，不是 bug。
2. **指针数组在第五章会炸。** 你后面要把书库换成 `vector<Book>`。`vector` 满了会重新分配一块更大的内存、把元素搬过去、**原来所有元素的地址全部失效**。如果你的记录里存着指向旧位置的指针，`vector` 一扩容，你所有借阅记录立刻集体悬空。这是一个非常真实、非常难查的崩溃。
3. **不用管 `delete`。** 对象数组里的 `Book` 跟着 `User` 一起自动析构，你不需要写任何清理代码。指针数组里每存一个 `new` 出来的地址，就欠一次 `delete`，谁欠谁还说不清。
4. **`Book` 已经有默认构造函数。** 第一节的 `Book()` 让对象数组可以直接开，写法 A 没有任何额外成本。（反过来，如果 `Book` 没有默认构造，对象数组根本编不过，会报 `error C2512` —— 这也是指针数组唯一真正的优势场景。）

**指针数组什么时候是对的？**

当你要表达的不是"借条"，而是"**这个人在看哪本书**"这种临时的、实时的指向关系时。比如一个"当前浏览位置"：

```cpp
Book* m_currentLooking = nullptr;      // 正在翻的那本书，翻完就清空
```

或者第五章之后，书库换成 `vector<Book>`，你想在记录里存**下标**而不是指针：

```cpp
struct BorrowRecord
{
    int    m_bookIndex;      // 存书在 vector 里的位置
    string m_borrowDate;     // 存借出时间
};
```

存下标比存指针稳 —— `vector` 扩容后下标照样有效，指针全废。这是第五章一个重要的取舍，现在先知道有这么回事。

> **答辩模板**：老师问"你为什么用对象数组存记录，不用指针数组？"
> 答"因为借阅记录的语义是快照，不是引用。用对象数组，记录和书库解耦，书库怎么改都不影响已成事实的借阅；而且它自动构造、自动析构，没有 `delete` 责任。用指针数组省空间，但书库容器一旦重新分配，所有指针同时悬空，风险远大于收益。"
