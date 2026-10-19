## 七、依赖：把 `borrowBook` / `returnBook` 彻底写对

### 7.1 先把话说明白：本章有两套名字，别抄错

前面几节为了方便，用的是一个只有三格记录的练手类。你项目里的 `User` 不是那个样子。两份代码长得像，名字不一样，直接搬会撞车。

先把对照表摆出来，后面所有代码一律用**右边这一列**（你项目里的真名）：

| 练手版里的写法 | 你项目里的真名（`User.h` / `User.cpp`） |
|---|---|
| `m_count` | `m_borrownum` |
| `m_maxnum` 上限 3，数组也开 3 格 | `m_maxnum` 由构造参数给（项目里是 3），数组固定开 **60** 格 `BorrowRecord m_records[60]` |
| `m_records[i].m_date`（借书日期字段） | **不存在**。`BorrowRecord` 只有 `Book m_book` 一个成员 |
| `m_records[i].m_book.getIsbn()` | `m_records[i].getBook().getISBN()`（注意是大写 ISBN） |
| `bool borrowBook(...)` | `void borrowBook(Book& book);`（`User.h` 第 34 行） |
| `bool returnBook(...)` | `void returnBook(Book& book);`（`User.h` 第 35 行） |

两条纪律，先刻在脑子里：

1. **抄代码之前先看签名。** 返回类型也算签名的一部分。`User.h` 里写的是 `void`，你在 `User.cpp` 里写成 `bool`，编译器立刻报 `C2556`（第九节第一条展开讲）。
2. **上限和容量是两件事。** `m_maxnum` 是"他最多能借几本"（运行时决定，可能是 3、可能是 5），`m_records[60]` 是"这张表物理上能放几格"（编译期定死）。两个数字必须各管各的，绝不能拿一个当另一个用。

### 7.2 `borrowBook`：三道闸全过才放行

借书这件事，成功需要三个条件**同时**成立：

- **闸 1**：他自己没借满（`m_borrownum < m_maxnum`）；
- **闸 2**：这本书在书库里是"可借"的（没被别人借走）；
- **闸 3**：他自己没借过这本书（不能同一本借两遍）。

任何一道没过，就一个字都不能改 —— 包括 `m_borrownum`、`m_records`、`book` 的状态，三样都得原封不动。这叫**先判断、后动手**。

下面是能直接替换进 `User.cpp` 的完整版本：

```cpp
bool User::borrowBook(Book& book)
{
	// 闸 1：额度
	if (m_borrownum >= m_maxnum)
	{
		cout << "借书失败：你已经借满 " << m_maxnum << " 本了。" << endl;
		return false;
	}

	// 闸 2：这本书在书库里是不是已经被别人借走了
	if (book.getIsBorrowed())
	{
		cout << "借书失败：《" << book.getTitle() << "》已经被借走了。" << endl;
		return false;
	}

	// 闸 3：他自己是不是已经借过这本（按 ISBN 认书）
	for (int i = 0; i < m_borrownum; i++)
	{
		if (m_records[i].getBook().getISBN() == book.getISBN())
		{
			cout << "借书失败：你已经借过《" << book.getTitle() << "》了。" << endl;
			return false;
		}
	}

	// 三道闸全过了，才动状态。两边必须一起改，不能只改一边
	m_records[m_borrownum].setBook(book);   // ① 抄一份进记录表
	m_borrownum++;                          // ② 借书数加一
	book.setIsBorrowed(true);               // ③ 书库里那本书标成已借出

	cout << "借书成功：《" << book.getTitle() << "》" << endl;
	return true;
}
```

**配套改 `User.h` 第 34 行**（不改就编译不过，`C2556`）：

```cpp
	bool borrowBook(Book& book);
```

`LibrarySys.cpp` 第 108 行 `user.borrowBook(books[idx]);` 不需要动 —— 返回值扔掉了，语法上完全合法。

逐句说清 ①②③ 为什么缺一不可：

- 少了 ①，书库里标成"已借出"，但他自己的记录里查不到这本书 —— 他还书时按 ISBN 找不到，永远还不掉。
- 少了 ②，记录写了进去，但计数还是 0 —— 下一次借书会**覆盖同一格**，前一本凭空消失。
- 少了 ③，记录和计数都对，但书库那边不知道这本书出去了 —— 下一个人还能再借一遍，两个人同时"拥有"同一本。

这三行就是这一章的核心动作：**一份事实（这本书被谁借走了）在两个地方（书库对象、学生记录）各存一份，改的时候必须两手一起改。**

### 7.3 `returnBook`：先认人，再删记录，最后解锁

还书比借书多一步麻烦：**你得先确认"你还的这本，是不是他手上真有的那本"**。这一步不做，整个系统会烂掉（7.5 节缺陷 3 会演给你看）。

完整版本，可直接替换：

```cpp
bool User::returnBook(Book& book)
{
	// 闸 1：他手上到底有没有书
	if (m_borrownum <= 0)
	{
		cout << "还书失败：当前没有任何借阅记录。" << endl;
		return false;
	}

	// 闸 2：这本是不是他借过的那本 —— 按 ISBN 在记录表里找
	int pos = -1;
	for (int i = 0; i < m_borrownum; i++)
	{
		if (m_records[i].getBook().getISBN() == book.getISBN())
		{
			pos = i;
			break;
		}
	}
	if (pos == -1)
	{
		cout << "还书失败：你没有借过《" << book.getTitle() << "》。" << endl;
		return false;
	}

	// 找到位置 pos 了。从 pos 开始，把后面每一格往前挪一格
	for (int i = pos; i < m_borrownum - 1; i++)
	{
		m_records[i] = m_records[i + 1];
	}
	m_borrownum--;

	// 尾巴上空出来的那一格清掉，避免残留一份旧副本
	m_records[m_borrownum] = BorrowRecord();

	// 最后才解锁书库里那本书
	book.setIsBorrowed(false);

	cout << "还书成功：《" << book.getTitle() << "》" << endl;
	return true;
}
```

**配套改 `User.h` 第 35 行**：

```cpp
	bool returnBook(Book& book);
```

`LibrarySys.cpp` 第 123 行 `user.returnBook(books[idx]);` 同样不用动。

#### 为什么要"前移一格"，不能只把计数减一

这是本章最容易做错的一步。记录表是一段**连续**的数组，`m_borrownum` 既表示"借了几本"，也表示"下一个空位在哪一格"。这两个含义只有在**从尾部删除**时才一致；要从中间删一本书，就必须有人把数组重新压紧。

拿 `m_borrownum = 3`、借的是 A、B、C，现在要还 A（下标 0）举例：

```
还之前（m_borrownum = 3）：
  下标：      [0]        [1]        [2]
  内容：       A          B          C
               ↑ 要还的是下标 0

前移（i 从 pos=0 走到 m_borrownum-2=1）：
  i=0：m_records[0] = m_records[1]  →   B          B          C
  i=1：m_records[1] = m_records[2]  →   B          C          C

m_borrownum--  →  2
清尾巴：m_records[2] = BorrowRecord()  →   B          C        （空）

结果：表里躺着 B、C，正好是他手上剩下的两本。
```

一句话记住：**循环变量从 `pos` 开始，条件是 `i < m_borrownum - 1`，循环体是"后一格抄到前一格"。** 三个地方错一个就漏一格或者越界。

#### 改完以后，现象差在哪

同一段场景（借 A、借 B、还 A、再借 C）跑一遍：

**改之前**：借书全程一声不吭（成功路径一句打印都没有），还完 A 之后表里还是 A、C，而他手上明明是 B、C。

**改之后**：

```
--- 借 A ---
借书成功：《C++ Primer》
--- 借 B ---
借书成功：《算法导论》
--- 还 A ---
还书成功：《C++ Primer》
--- 再借 C ---
借书成功：《Effective C++》

当前 m_borrownum = 2
第 1 本: 算法导论
第 2 本: Effective C++
书库里三本书的状态：
  A C++ Primer = 可借
  B 算法导论 = 已借出
  C Effective C++ = 已借出
```

记录显示的两本，和书库显示"已借出"的两本，对上了。**"能对上"就是这段代码合格的标准。**

### 7.4 现在拆你项目里的三个真实缺陷

下面三处都在你现在的源码里，不是我编的。每一处都按 `错在哪一行 → 会出什么后果 → 为什么会出这个错 → 改成什么` 走一遍。

---

#### 缺陷 1：`Book` 的带参构造函数把 `isBorrowed` 形参丢了

**错在哪一行** —— `Book.cpp` 第 18～27 行。整个函数长这样：

```cpp
Book::Book(string title, string author, string publisher, string ISBN, string id, double price, bool isBorrowed)
{
	m_title = title;
	m_author = author;
	m_publisher = publisher;
	m_ISBN = ISBN;
	m_id = id;
	m_price = price;
	m_isBorrowed = false;      // ← 第 26 行：坏在这里
}
```

一句话点破：**函数头上接了一个 `isBorrowed`，函数体里从头到尾没读过它一次，反而把成员写死成 `false`。** 外面传什么进来都会被踩掉。

**会出什么后果** —— 一切"构造时就该带状态"的书全都变成"可借"。走一遍：

```cpp
Book b("C++ Primer", "Stanley Lippman", "电子工业出版社", "9787121358579", "B001", 128.0, true);
cout << b.getIsBorrowed();     // 期望 1，实际打 0
```

状态在构造这一步就被丢掉了，后面无论怎么改都补不回来。数据流是这样的：

```
调用：  Book b(..., true)
                 │
                 ▼
形参：  bool isBorrowed = true        ← 收下了，放在栈上
                 ╳                    ← 没有任何一条语句把它读出来
成员：  bool m_isBorrowed = false     ← 直接被写成常量 false
```

**这个缺陷最阴的地方**：你项目里 `LibrarySys.cpp` 第 145～147 行 push_back 那三本书，传进去的都是 `false`，所以改之前和改之后跑起来**看不出任何区别**。没有一行输出会变，没有一个功能会坏。它要到"以后加一个从文件读入已借出图书"的功能时才会爆。

也就是说：**你现在的测试用例根本测不到它**。这类缺陷只能靠"读代码 + 对照签名"抓出来，靠运行抓不出来。

**为什么会出这个错** —— 把"形参"和"成员"当成同一个东西了。

`bool isBorrowed` 是函数进门时接的一个包裹，函数一返回就没了；`m_isBorrowed` 是对象身上的柜子，活得比函数长。这两者之间**没有任何自动通道**，必须由你亲手写一行 `m_isBorrowed = isBorrowed;` 才能把包裹放进柜子。

这和第一节讲"参数怎么进函数"是同一件事，只是方向反过来：

- 借书、还书那种**成员函数**，方向是"里面的改动要传出去" → 用引用（`Book&`），改的是本体；
- **构造函数**，方向是"外面的值要传进来" → 用赋值或初始化列表，把形参抄进成员。

同一个参数通道，两个方向，两个方向都**不会自动**。这就是零基础最容易漏的一环：以为"参数传进来了就等于成员有值了"。

**改成什么** —— 只改第 26 行，一个词：

```cpp
Book::Book(string title, string author, string publisher, string ISBN, string id, double price, bool isBorrowed)
{
	m_title = title;
	m_author = author;
	m_publisher = publisher;
	m_ISBN = ISBN;
	m_id = id;
	m_price = price;
	m_isBorrowed = isBorrowed;     // 形参 → 成员，唯一的一行
}
```

**注意不要顺手把默认构造函数的第 14 行也改了。** `Book.cpp` 第 6～15 行是默认构造函数 `Book::Book()`，它**没有** `isBorrowed` 这个形参，那里写 `m_isBorrowed = false;` 是**对的**。两个函数里都有 `m_isBorrowed = false;` 这一行文本，改错一个就编译不过（`isBorrowed` 未声明，`C2065`）。

改完的现象差别：`Book b(..., true).getIsBorrowed()` 从 `0` 变成 `1`。你现在的命令行程序一切如常 —— 因为它本来就只传 `false`。

---

#### 缺陷 2：`returnBook` 只把计数减一，没把记录从数组里挪走

**错在哪一行** —— `User.cpp` 第 96～107 行：

```cpp
void User::returnBook(Book& book)
{
	if (m_borrownum <= 0)
	{
		cout << "当前没有借书记录，无法还书。" << endl;
		return;
	}
	book.setIsBorrowed(false);                                  // 第 104 行
	m_borrownum--;                                              // 第 105 行
	cout << "还书成功：" << book.getTitle() << endl;
}
```

一句话点破：**第 104 行和第 105 行之间，本该有"把这一格从数组里删掉、后面整体前移"的代码，但一行都没有。** 计数减了，表没动。

**会出什么后果** —— 记录表会越来越歪，最后歪到"表里的书"和"手上的书"完全是两批。

具体走一遍：借 A（C++ Primer）、借 B（算法导论）、还 A、再借 C（Effective C++）。

| 时刻 | `m_borrownum` | `[0]` | `[1]` | `[2]` |
|---|---|---|---|---|
| 起点 | 0 | 空 | 空 | 空 |
| 借 A | 1 | **A** | 空 | 空 |
| 借 B | 2 | A | **B** | 空 |
| 还 A | 1 | **A ← 没删掉！** | B | 空 |
| 再借 C | 2 | A ← 幽灵 | **C ← 把 B 覆盖了** | 空 |

这是真的跑出来的结果（用你现在的源码，一点没改）：

```
--- 借 A ---
--- 借 B ---
--- 还 A ---
还书成功：C++ Primer
--- 再借 C ---

当前 m_borrownum = 2
第 1 本: C++ Primer
第 2 本: Effective C++
书库里三本书的状态：
  A C++ Primer = 可借
  B 算法导论 = 已借出
  C Effective C++ = 已借出
```

对着看：**他手上拿的是 B（算法导论）和 C（Effective C++），表里却写着 A 和 C。**

三重崩：

1. `displayRecords()` 打出来的名单是假的 —— 显示 A，实际 A 已经在书架上；
2. B 明明借走了，记录里**查不到**，他没法通过记录证明自己借过 B；
3. 新借的 C 写进了本该属于 B 的那一格，B 的记录被物理覆盖 —— 不是"丢失引用"，是整个副本被 `operator=` 盖掉，找不回来了。

**为什么会出这个错** —— 一个变量背了两个含义，删了中间一个，两个含义就分家了。

`m_borrownum` 同时表示：

- 借了几本（**数量**）；
- 下一个空位在第几格（**位置**）。

只要永远从末尾追加、从末尾删除，这两个含义永远相等，一个变量够用。可一旦从**中间**删掉一本，数量减了一、位置没变，两个含义立刻分家。这段代码只做了"数量减一"，忘了"位置也要跟着压紧"。

往概念层接：**组合的代价就在这里。** `User` 拥有一个 `Book` 数组（`m_records[60]`），"拥有"意味着这个数组的一致性**由你自己负责**。C++ 不会帮你在删元素时自动压缩数组 —— 那是 `std::vector` 干的活（第五课会讲），普通数组不会。

所以组合不是"把两个类放一起"，而是"你接手了一份数据，你就得自己维护它的每一处不变量"。这里的不变量只有一句：**`m_records[0]` 到 `m_records[m_borrownum-1]` 这连续的一段，必须一格不漏地对应他手上的每一本书。**

**改成什么** —— 完整的 `returnBook`（代码见 7.3 节，包含按 ISBN 认人 + 前移 + 清尾）。连带要改 `User.h` 第 35 行的返回类型。

改完那同一段场景的输出变成 `第 1 本: 算法导论 / 第 2 本: Effective C++` —— 和他手上的两本完全一致。

---

#### 缺陷 3：还书不检查"还的是不是他借的那本"

**错在哪一行** —— 还是 `User.cpp` 第 104 行：

```cpp
	book.setIsBorrowed(false);      // 无条件执行：随便传一本进来都会解锁
```

整个函数唯一的前置检查是第 99 行的 `if (m_borrownum <= 0)` —— 那是在数**他有没有书**，不是在认**你手上这本是不是他的**。数量对得上就放行，这是缺陷。

一句话点破：**`returnBook` 收了别人的一本书，连"这是不是他借过的"都不问，直接解锁、直接扣额度。**

**会出什么后果** —— 两种，第二种能把书永久锁死。

**后果一：冤枉一本无辜的书。** 如果传进来的是书库里正被别人借着的 B，`B.setIsBorrowed(false)` 一执行，B 在系统里就变回"可借"了 —— 但 B 还在别人手上。下一个人来借 B，闸 2 通过，成功借出。**现在两个人同时"拥有"B，而系统认为只有一个人。**

**后果二：把一本书永久锁死。** 这是最狠的一种。走一遍：他只借了 A，然后错误地还了一本没借过的 D。

```
借完 A：m_borrownum = 1，A 的状态 = 已借出
还书成功：从没借过的书
错误地还了 D：m_borrownum = 0，A 的状态 = 已借出

--- 现在他再想借 A ---
This book is borrowed.
--- 他想把 A 还掉，好解锁 ---
当前没有借书记录，无法还书。

最终：m_borrownum = 0，A 的状态 = 已借出
暂无借阅记录。
```

（这段也是真跑出来的。）看清楚这个死锁：

- 他还想借 A → 闸 2 拦：`book.getIsBorrowed()` 是 `true`，报 `This book is borrowed.`；
- 他想干脆把 A 还掉解锁 → 第 99 行 `m_borrownum <= 0` 成立，**函数在碰任何东西之前就 return 了**；
- 最后：`m_borrownum = 0`，A 状态"已借出"，记录表空的。

**A 从此锁死：借不了、还不了、记录里没有、书架上也没有。** 除了重启程序（内存清零），没有任何操作能把它救回来。这就是缺陷 2 和缺陷 3 叠加之后的真实破坏力 —— 不是"数据有点脏"，是**这本书在系统里彻底消失了，而且它占着一个"已借出"的位置永久不还**。

**为什么会出这个错** —— 把"引用"当成了"身份"。

`Book& book` 这个参数的意思是"我手上有一个 Book 对象，我用引用把它交给你，你能直接改它"。它只保证**能改**，不保证**改得对**。引用是一把能开门的钥匙，但开门之前得先抬头看一眼门牌号 —— 这扇门是不是你要开的那扇。

依赖关系的纪律就在这句：**对方是谁，必须由你自己核对。引用不会替你核对，类型系统也不会。** `Book&` 只说"这是本合法的书"，不说"这是这个学生借的那本"。

**改成什么** —— 加按 ISBN 查表那一段（7.3 节的闸 2），找到 `pos` 才往下走，找不到就打印"你没有借过《X》"并 `return false`。

改完的现象差别：错误地还 D 时，一行 `还书失败：你没有借过《从没借过的书》`，而且 `m_borrownum` **停在 1 不动**，A 的状态**还是已借出** —— 因为什么都没被改。死锁不会再发生。

---

#### 另外还有第四处：`Book::display()` 里的空 `if`

Lead 让我确认"到底是不是三处"。我读完了四个文件，**逻辑缺陷是三处**，但还有一处明显的半成品，性质不同，单独列出来。

**错在哪一行** —— `Book.cpp` 第 110～113 行：

```cpp
	if (m_isBorrowed)
	{
		//（视频中未完成：此处可输出"已借出"状态）
	}
```

一句话点破：**条件判断写好了，`if` 体里一个字都没有，状态算对了却不打印。**

**会出什么后果** —— 每个直接调用 `book.display()` 的地方，都看不到这本书的借阅状态。你现在之所以没觉得缺，是因为 `LibrarySys.cpp` 第 61 行在 `display()` 之外又补打了一行：

```cpp
	cout << "状态: " << (books[i].getIsBorrowed() ? "已借出" : "可借")
```

相当于在门外另贴了一张纸条。功能上看起来正常，但 `Book::display()` 自己是不完整的 —— 答辩时老师点开这个文件，一眼就能看见一个空 `if` 和一行"视频中未完成"的注释，这是直接扣分的。

**为什么会出这个错** —— 这不是设计错误，是**没写完**。但它值得单独说，因为它属于"状态同步"链条上的第三环：

1. 状态算对（`m_isBorrowed` 值是对的）；
2. 状态存对（存在对象里）；
3. **状态被看见**（能打印出来）。

前两环对了、第三环断了，对使用者来说和"没有这个状态"是一样的。**系统里"没人知道"和"不存在"，是一回事。**

**改成什么** —— 一行：

```cpp
	if (m_isBorrowed)
	{
		cout << "（已借出）";
	}
	else
	{
		cout << "（可借）";
	}
```

改完注意一个副作用：`LibrarySys.cpp` 第 61 行会再打一遍状态，你的输出会变成 `… （可借） 状态: 可借` 这种重复。要么把第 61 行那行删掉（让 `display()` 全权负责），要么保留重复但知道它为什么重复。**不要两边各改一半，改完自己跑一遍看输出对不对。**

### 7.5 这三处错误的共同点

一句话：**它们全都是"同一份事实在两个地方各存一份，改的时候只改了一边"的变种。**

把它们摊在一条时间轴上，你能看见这是同一个病在四个不同环节发作：

| 环节 | 事实是什么 | 哪边没同步 | 对应 |
|---|---|---|---|
| 进门 | "这本书是不是已借出" | 形参收下了，成员没接住 | 缺陷 1（`Book.cpp:26`） |
| 出去（借） | "这本书归他了" | 记录改了、计数改了、书库没改（或反之） | 7.2 节 ①②③ |
| 回来（还） | "这本书回到书架了" | 计数减了、数组没删 | 缺陷 2（`User.cpp:104-105`） |
| 认人 | "还的是他借的那本吗" | 根本没核对，改了别人的书 | 缺陷 3（`User.cpp:104`） |
| 被看见 | "系统里这本书什么状态" | 状态有了，没打印 | 第四处（`Book.cpp:110-113`） |

这一章讲"组合与依赖"，听起来像在讲怎么把两个类放一起。不是的。真正的纪律是这一句：

**只要同一份事实存在两个副本（书库里的 `m_isBorrowed`、学生记录里的 `m_book` 副本、计数 `m_borrownum`），你就必须为"它们永远一致"负责。C++ 不会替你做，编译器也不会提醒你 —— 因为它根本不知道这两份数据说的是同一件事。**

这就是为什么这三处缺陷全都编译通过、全都不报警告、全都"看起来能跑"。它们不是语法问题，是**你在心里要维护的那张一致性清单**没维护住。

写下一个成员函数之前，先问自己三句话：

1. 这个动作会动到哪几份数据？（列出来）
2. 每一份都改到了吗？（逐个数）
3. 如果中途有一份改不了（比如闸 2 拦住了），前面已经改的那些要不要退回去？

第三问就是"先判断、后动手"的由来 —— 判断全做完再动手，就不存在"改了一半要回滚"的问题。

---

## 八、本章思考题（老师会问）

### Q1（官方思考题）组合类构造函数的调用过程是怎样的？请打断点观察

**老师问什么**

"`User` 里面有一个 `BorrowRecord` 数组，`BorrowRecord` 里面有一个 `Book`。那么创建一个 `User` 对象的时候，`Book` 的构造函数什么时候被调用？谁先谁后？"

**标准答案（可直接背）**

分四步，顺序固定：

1. **分配内存**：`User` 对象需要的那块内存（含 `m_records` 数组的 60 格、每格里的那个 `Book`）一次性分配好。这一步不调用任何构造函数。
2. **构造成员**：按成员在类里**声明的顺序**，逐个构造。`m_name`（`string`）→ `m_id`（`string`）→ `m_borrownum`（`int`）→ `m_maxnum`（`int`）→ `m_records[60]`（每个元素里的 `Book` 依次默认构造 60 次）。**注意：只有声明顺序说了算，和你在初始化列表里写的先后无关。**
3. **执行构造函数体**：`{ ... }` 里面的语句。
4. 对象构造完成，返回。

析构严格**逆序**：先执行析构函数体，再按声明的**反序**销毁成员（`m_records` → `m_maxnum` → `m_borrownum` → `m_id` → `m_name`）。

**能直接粘进 Visual Studio 打断点的代码**

新建一个空项目，粘这段进去（不依赖你的项目文件，单独跑）：

```cpp
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

class Item
{
public:
	string m_tag;
	Item() { m_tag = "(默认)"; cout << "  Item 默认构造: " << m_tag << endl; }
	Item(const string& t) : m_tag(t) { cout << "  Item 带参构造: " << m_tag << endl; }
	~Item() { cout << "  Item 析构: " << m_tag << endl; }
};

class Holder
{
private:
	string m_name;
	Item   m_first;
	Item   m_second;
	int    m_count;
public:
	// 注意：初始化列表里故意把 m_second 写在 m_first 前面
	Holder(string name) : m_name(name), m_second("second"), m_first("first"), m_count(0)
	{
		cout << "  Holder 构造函数体" << endl;
	}
	~Holder() { cout << "  Holder 析构函数体" << endl; }
};

int main()
{
	SetConsoleOutputCP(65001);
	cout << "--- 进入作用域 ---" << endl;
	{
		Holder h("H");
		cout << "--- 作用域结束 ---" << endl;
	}
	cout << "--- 已离开 ---" << endl;
	return 0;
}
```

**逐行说明：断点停在哪、盯哪个变量、会看到什么**

**第一轮：在 `Item` 的带参构造函数里打断点。**

把光标放在 `Item(const string& t) : m_tag(t) { ... }` 这一行，按 `F9` 下一个红点，然后 `F5` 启动。

- 程序会**第一次**停在这个红点上。这时看屏幕下方的**调用堆栈（Call Stack）**窗口，从上到下是：

  ```
  Item::Item(const std::string &)      ← 当前停在这里
  Holder::Holder(std::string)          ← 调用者
  main()                               ← 再上一层
  ```

- **这就是铁证**：停在这一行的时候，`main` 还什么都没干，`Holder` 的构造体 `{ cout << ... }` 也还没执行 —— 但 `Item` 的构造函数已经在跑了。也就是说：**成员对象的构造，发生在 `Holder` 函数体之前。**
- 看"自动窗口（Autos）"里的 `t`，值是 `"first"`。注意初始化列表里写的是 `m_second("second")` 在前，可先停下来的却是 `first` —— **构造顺序只由声明顺序决定**（`m_first` 声明在 `m_second` 前面），初始化列表怎么写都不影响。
- 按 `F5` 继续，第二次停在同一个断点，`t` 的值是 `"second"`。再按 `F5`，不会再停第三次 —— 两个 `Item` 成员都构造完了。

**第二轮：在 `Holder` 构造函数体里打断点。**

把红点挪到 `cout << "  Holder 构造函数体" << endl;` 这一行。

- 停住的时候，在"自动窗口"里展开 `this`，能看到：
  - `m_name` = `"H"`
  - `m_first.m_tag` = `"first"` ← **已经有值了**
  - `m_second.m_tag` = `"second"` ← 已经有值了
  - `m_count` = `0`
- 关键观察：**这个断点绝不会在 `Item` 构造之前停下来。** 你按多少次 `F5` 都一样 —— `Holder` 的函数体永远排在所有成员构造之后。这就证明了"函数体是最后一步"。

**第三轮：在 `~Holder()` 和 `~Item()` 里各打一个断点。**

按 `F5` 让作用域结束，你会看到停止顺序是：`~Holder()` 函数体 → `~Item()`（`m_second`，值 `"second"`）→ `~Item()`（`m_first`，值 `"first"`）。**和构造顺序完全相反。**

完整的运行输出长这样（我是真跑出来的）：

```
--- 进入作用域 ---
  Item 带参构造: first
  Item 带参构造: second
  Holder 构造函数体
--- 作用域结束 ---
  Holder 析构函数体
  Item 析构: second
  Item 析构: first
--- 已离开 ---
```

**加分一句**

"如果某个成员没有出现在初始化列表里，它并不会'不构造' —— 它会用**默认构造函数**构造一次，然后在构造函数体里被你赋一次值，等于白干一次。所以能在初始化列表里写的，就不要拖到函数体里赋值。对我们项目的 `User` 来说，`m_records[60]` 这 60 个 `Book` 一定会被默认构造 60 次，哪怕你后面一本都不借 —— 这就是固定数组的代价。"

---

### Q2（官方思考题）值传递和地址传递的区别是什么？分别适用什么场景？

**老师问什么**

"你写 `void f(Book b)` 和 `void f(Book& b)`，除了少打一个 `&`，还有什么区别？什么时候用哪个？"

**标准答案（可直接背）**

区别只有一个，但后果很硬：**值传递会复制一份新对象，函数里改的是副本；引用传递不复制，函数里改的就是本体。**

用地址一看就明白（`lesson02.exe` 的真实输出）：

```
  书库那本书的地址 = 000000168ECFF2A0

  [1a] 值传递：passByValue(lib)
    函数内副本的地址   = 000000168ECFF250     ← 和上面不一样，是另一个对象
  调用后书库那本书状态 = 可借                 ← 没变，改的是副本

  [1b] 引用传递：passByRef(lib)
    函数内引用的地址   = 000000168ECFF2A0     ← 和上面完全一样
  调用后书库那本书状态 = 已借出               ← 变了，改的就是本体
```

适用场景，一句话分工：

- **要改本体 → 用引用**（`Book&`）。项目的 `borrowBook(Book& book)`、`returnBook(Book& book)` 都是这一类，因为它们必须把 `m_isBorrowed` 改到书库那本书身上。
- **只读，不想复制 → 用 const 引用**（`const Book&`）。大对象（含 `string` 成员的 `Book` 有 7 个字段）复制一次要真拷贝所有 `string`，代价不小；`const Book&` 既不复制也不许改。
- **要改，但传的是指针 → 用指针**（`Book*`）。需要表达"可能没有这本书"（传 `nullptr`）时才用。日常能不用就不用，多一层解引用多一处空指针风险。
- **小类型（`int`、`double`、`char`）→ 直接值传递**。复制一个 `int` 就 4 字节，比传引用还便宜，没必要绕。

**加分一句**

"引用在底层就是一个自动解引用的常量指针 —— `sizeof(Book&)` 是 8，和 `sizeof(Book*)` 一样。所以引用的好处不是'不占空间'，是'不会空、不用写 `*`、语法上当作本体用'。另外 C++ 里引用一旦绑定就不能改绑，`Book& r = a; r = b;` 不是让 `r` 改指 `b`，而是把 `b` 的内容赋给 `a` —— 这个坑第八章会再踩一次。"

---

### Q3 `User` 和 `Book` 之间，到底是组合还是依赖？

**老师问什么**

"你这张 UML 图里，`User` 到 `Book` 画的是实心菱形还是虚线箭头？为什么？"

**标准答案（可直接背）**

**两种关系同时存在，但落点不同：**

- `User` → `BorrowRecord` → `Book`：`BorrowRecord` 里有一个 `Book m_book;` 成员 —— 是**按值拥有的真对象**，这部分形式上像组合（`User` 拥有记录，记录拥有一本 `Book` 的副本）。
- 但业务上，`User` 并**不拥有**书库里那本书。那本书是 `LibrarySys` 里的 `Book` 对象，`User` 借它只是"临时用到"。所以 `User` 对**书库里的** `Book` 是**依赖**关系。

一个更精确的说法：**`User` 依赖书库里的 `Book` 对象（借的时候通过 `Book&` 拿到它），同时在自己的记录表里存了它的副本（组合关系）。** 前者是"用"，后者是"存"。

**加分一句**

"判断标准就一句话：**生命周期归谁管。** 如果 `User` 死了这本书就该没了，那是组合；如果 `User` 死了书架上这本书还在，那是依赖。我们项目里 `User` 死了书当然还在 —— 所以我们**没有**真的'拥有'书，只拥有书的一份复印件。这个区别在第七节缺陷 2 里就要了命：复印件归你管，你得自己维护它。"

---

### Q4 `const Book&` 和 `Book&` 到底差在哪，我该用哪个？

**老师问什么**

"`getBook()` 为什么返回 `Book` 而不是 `Book&`？`setBook(Book& book)` 为什么参数不是 `const Book&`？"

**标准答案（可直接背）**

`Book&` 和 `const Book&` 的区别是**写权限**：

| 写法 | 能读 | 能改 | 能接受 `const Book` 类型的实参 |
|---|---|---|---|
| `Book&` | 能 | 能 | **不能**（报 `C2664`） |
| `const Book&` | 能 | **不能** | 能 |
| `Book`（值） | 能 | 能（改的是副本） | 能（复制一份） |

所以规则很简单：**函数里要改本体 → `Book&`；只读 → `const Book&`。**

回到项目里这两处：

- `setBook(Book& book)`：这里其实**不该**要写权限 —— 它只是把 `book` 抄进 `m_book`，一个字段都没改 `book`。严格说应该是 `setBook(const Book& book)`。项目里写的是 `Book&`，能跑，但限制了调用方（传 `const Book` 就报错）。
- `getBook()` 返回 `Book`（值，不是引用）：这是**故意的**，它返回的是一份副本，调用方改它改不到记录里的原件。代价是每次调用都复制一次。写成 `Book getBook() const` 更好（加 `const` 尾缀，见 Q8），但返回类型保持值是对的。

**加分一句**

"`const` 引用能绑到临时对象上，普通引用不能。所以 `void f(Book&)` 你写 `f(Book("书名", ...));` 会报错，而 `void f(const Book&)` 可以。这条规则叫'const 引用延长临时对象的生命周期'，是 C++ 里非常常用的一个便利。"

---

### Q5 给 `User` 加一个新的成员对象，会连带影响哪些地方？

**老师问什么**

"如果我现在让你给 `User` 加一个 `Date m_registerDate;`（注册日期）成员，你要改几个地方？"

**标准答案（可直接背）**

四个地方，一个都不能漏：

1. **`User.h` 的成员声明**：加 `Date m_registerDate;`。注意位置 —— 它会改变成员的声明顺序，从而改变构造和析构顺序。
2. **`User.h` 加 `#include "Date.h"`**：`User.h` 里出现了一个**完整的对象**（不是指针、不是引用），编译器必须知道 `Date` 有多大，所以必须 include 头文件。**这是组合和依赖在写法上最直观的差别**：组合要 `#include`，依赖只要前置声明（`class Book;`）就够。
3. **`User` 的构造函数初始化列表**：如果 `Date` 没有默认构造函数，你必须在这里显式构造它（`m_registerDate(...)`），否则报 `C2512`（第九节展开）。如果 `Date` 有默认构造，可以省略，但它照样会被默认构造一次。
4. **拷贝/赋值行为**：`User` 的默认拷贝构造函数和默认赋值运算符会自动连 `m_registerDate` 一起拷贝 —— 这部分不用你写，但你要**知道**它会自动做。如果 `Date` 里有指针成员，默认拷贝就是浅拷贝，那就必须自己写（第十课）。

**加分一句**

"加成员这种事，最容易被漏掉的是第 2 条：很多人只在 `.cpp` 里 `#include "Date.h"`，`User.h` 里没加，结果 `User.h` 一被别的文件包含就报一堆莫名其妙的错 —— 因为那个文件根本不知道 `Date` 是什么。**头文件里出现了完整类型（成员对象），就必须在头文件里 include，不能指望 `.cpp` 帮你兜。**"

---

### Q6 还书的时候，为什么不能只把 `m_borrownum` 减一？

**老师问什么**

"还书不就是'他少了一本书'吗？`m_borrownum--` 一下不就行了，为什么还要动数组？"

**标准答案（可直接背）**

因为 `m_borrownum` 同时承担两个含义：**"借了几本"**和**"下一个空位在第几格"**。只在末尾增删时这两个含义相等；一旦从中间删，数量减了、位置没变，两个含义就分家了，后面新借的书会覆盖掉本该保留的记录（详见 7.4 节缺陷 2 的表）。

所以正确的还书要做三件事，顺序不能换：

1. **找到位置** `pos`（按 ISBN 在 `m_records[0..m_borrownum-1]` 里找）；
2. **前移压紧**：`for (int i = pos; i < m_borrownum - 1; i++) m_records[i] = m_records[i+1];`；
3. **计数减一，清尾巴**：`m_borrownum--; m_records[m_borrownum] = BorrowRecord();`。

**加分一句**

"这套'找位置 → 前移 → 减计数'的操作，`std::vector` 的 `erase()` 内部做的是一模一样的事（一次 `erase` 是 O(n) 的原因就在这儿）。第五课学了 `vector` 之后，你可以把 `m_records` 换成 `vector<BorrowRecord>`，还书就变成一行 `m_records.erase(m_records.begin() + pos);` —— 但前提是你要先**理解**它内部为什么必须搬数据，不然用 `vector` 也只是把问题藏起来。"

---

### Q7 为什么按 ISBN 认书，不按书名？

**老师问什么**

"`returnBook` 里你写的是 `getISBN() == book.getISBN()`，为什么不是 `getTitle() == book.getTitle()`？"

**标准答案（可直接背）**

因为**书名不唯一，ISBN 唯一**。两本不同的书可以同名（《C++ Primer》有第 5 版、第 6 版；不同出版社可以出同名书），按书名认会认错：他想还第 6 版，系统可能把第 5 版的记录删了。

ISBN 是国际标准书号，一本书一个号，全球唯一，天然适合当"身份证"。这也解释了为什么 `Book` 里要专门存一个 `m_ISBN` 字段 —— 它存在的唯一意义就是给别的模块当索引用。

**加分一句**

"这个字段在数据库里的说法叫'主键'。判断一个字段能不能当主键，就问三句话：**唯一吗？不为空吗？不会变吗？** ISBN 三条都满足。书名三条都不满足。以后你设计任何'查找'功能，都先找那个满足三条的字段，找不到就自己造一个（比如这里的 `m_id`）。"

---

### Q8 什么是 `const` 成员函数？为什么 `getter` 要加 `const` 尾缀？

**老师问什么**

"你的 `Book.h` 里 `double getPrice();` 后面为什么不写 `const`？写了会怎样？不写会怎样？"

**标准答案（可直接背）**

`const` 尾缀（`double getPrice() const;`）的意思是：**这个函数承诺不改对象里的任何成员。** 编译器会检查这句话是不是真的 —— 你在里面写 `m_price = 1;` 会直接报错。

三个后果：

1. **不写 `const`**：`const Book` 对象（或 `const Book&` 参数）**不能**调用它，报 `C2662`。也就是说你的 getter 对 const 对象完全不可用。
2. **写 `const`**：`const Book` 对象可以调用，普通对象也可以调用。适用范围只增不减。
3. **`const` 对象只能调 `const` 成员函数** —— 这是 const 正确性的传导：你承诺了不改，编译器就不许你调用任何"可能改"的函数。

所以规则是：**getter 一律加 `const`，setter 一律不加。** 你项目里所有 getter 都缺这个 `const` —— 现在能跑是因为你没写过 `const Book&` 参数；一旦开始写（比如 `void printBook(const Book& b) { cout << b.getTitle(); }`），立刻报 `C2662`（第九节第二条展开讲真实报错文本）。

**加分一句**

"`const` 尾缀修饰的是隐含的 `this` 指针 —— 加了 `const` 就等于把 `this` 从 `Book* const` 变成 `const Book* const`。所以严格说，`const` 成员函数里连 `this` 都是只读的，这才是它不能改成员的根本原因。同一个函数名，一个 const 版一个非 const 版，算重载，编译器按对象是不是 const 来选。"

---

### Q9 `BorrowRecord` 里存的那本 `Book`，和书库里那本是同一个对象吗？

**老师问什么**

"`m_records[0].getBook()` 拿到的 `Book`，是不是就是书库里的 `Book b`？我改 `m_records[0]` 里的书名，书库里那本会跟着变吗？"

**标准答案（可直接背）**

**不是同一个对象，不会跟着变。** `BorrowRecord` 里是 `Book m_book;` —— 一个**完整的真对象**，不是指针、不是引用。`setBook` 里那句 `m_book = book;` 是逐字段**拷贝赋值**，把书名、作者、价格、状态全部抄了一份。

所以记录表里躺的是那本书的**复印件**。改复印件，原件不动；改原件，复印件也不动。

验证这件事不需要撬开 `User` 的私有成员 —— `setBook` 里那句 `m_book = book;` 干的事，和下面这句 `Book copy = a;` 是**完全同一个操作**（编译器生成的逐字段拷贝赋值）。单独跑这段就够：

```cpp
#include <windows.h>
#include <iostream>
#include "Book.h"
using namespace std;

int main()
{
	SetConsoleOutputCP(65001);
	Book a("C++ Primer", "Stanley Lippman", "电子工业出版社", "9787121358579", "B001", 128.0, false);

	Book copy = a;              // 这就是 setBook 里那句 m_book = book 干的事
	a.setPrice(999.0);          // 改书库里那本

	cout << "书库里 a 的价格 = " << a.getPrice() << endl;
	cout << "副本里的价格     = " << copy.getPrice() << endl;
	return 0;
}
```

真跑出来的结果：

```
书库里 a 的价格 = 999
副本里的价格     = 128
```

两个数字不一样。原件涨到 999，复印件还停在 128 —— 这就是"记录里那份是复印件，不是那本书本身"的铁证。

**这个设计的好处**：记录能长期保存"借出那一刻"的快照（当时的价格、当时的书名），就算以后书库改了价，历史记录不会被篡改。

**这个设计的代价**：你要花内存（每借一本书就复制一整个 `Book`，含 5 个 `string`），而且**两份数据可能不一致** —— 这正是第七节缺陷 2 里"表里的书和手上的书是两批"能发生的根本原因。

**加分一句**

"如果想让记录'指向'书库里那本（改一处两边都变，还省内存），就得把 `Book m_book;` 改成 `Book* m_book;` 或 `Book& m_book;`。但那是**聚合**关系，代价是要自己管指针的生命周期（书被删了，指针就悬空）。**组合省心但费内存、要手动同步；聚合省内存但费心、要管生命周期。** 选哪个取决于你更怕哪件事 —— 我们这里怕的是悬空指针，所以选了拷贝。"

---

## 九、五个你一定会遇到的报错

先把五条摆成一张表，括号里是真实报错编号：

| 报错原文（节选） | 什么意思 | 怎么改 |
|---|---|---|
| `error C2664: "void passByRef(Book &)": 无法将参数 1 从"const Book"转换为"Book &"` | 你把一个只读对象传给了一个可能改它的参数 | 参数改成 `const Book&`，或者传一个非 const 的实参 |
| `error C2662: "double Book::getPrice(void)": 不能将"this"指针从"const Book"转换为"Book &"` | 在 const 对象上调了一个没标 `const` 的成员函数 | 给函数加 `const` 尾缀 |
| `error C2512: "Holder": 没有合适的默认构造函数可用` | 成员的类没有默认构造函数，而你没有在初始化列表里显式构造它 | 在初始化列表里补上 `m(v)`，或者给那个类加默认构造 |
| `error C2530: "r": 必须初始化引用` | 引用声明了却没绑定对象 | 声明时立刻绑定：`Book& r = b;` |
| `error LNK2019: 无法解析的外部符号 "public: void __cdecl User::returnBook(void)" ...` | 声明了、调用了，但没写实现（或者没把实现那个 `.cpp` 加进项目） | 补上函数体，或把 `User.cpp` 加进项目 |

下面逐条展开。

### 第 1 条：`C2664` —— const 对象传不进普通引用

**真实报错（在 `/W4`、`/std:c++17`、`/utf-8` 下用 `cl` 实测，全文照抄）**：

```
error C2664: “void passByRef(Book &)”: 无法将参数 1 从“const Book”转换为“Book &”
note: 转换丢失限定符
note: 参见“passByRef”的声明
note: 尝试匹配参数列表“(const Book)”时
```

**触发的代码**：

```cpp
void passByRef(Book& b) { b.setPrice(1.0); }

void f(const Book& cb)
{
	passByRef(cb);      // ← 报 C2664
}
```

**为什么**：`const Book&` 承诺了"我不会改这个对象"，可 `passByRef` 的参数是 `Book&`，它有权改。如果让这个调用通过，那 const 承诺就形同虚设 —— 编译器必须拦住。

**怎么改**，两个方向选一个：

```cpp
// 方向一：让函数遵守 const（推荐，如果函数本来就不该改）
void passByRef(const Book& b) { cout << b.getPrice() << endl; }

// 方向二：让调用方拿一个能改的对象
Book b(...);
passByRef(b);
```

**你会在哪遇到**：写 `void printBook(const Book& b)` 之后，里面调 `b.getTitle()` —— 如果 `getTitle()` 没标 `const`，报的是下面的 `C2662`，不是这条。这条通常出现在"我想在 const 函数里调用会改对象的东西"。

### 第 2 条：`C2662` —— 在 const 对象上调了非 const 成员函数

**真实报错**：

```
error C2662: “double Book::getPrice(void)”: 不能将“this”指针从“const Book”转换为“Book &”
note: 转换丢失限定符
Book.h(28): note: 参见“Book::getPrice”的声明
note: 尝试匹配参数列表“()”时
```

**触发的代码**（这就是你项目里加了 `const` 参数之后立刻会撞上的那条）：

```cpp
void printPrice(const Book& b)
{
	cout << b.getPrice() << endl;   // ← 报 C2662，因为 Book.h 第 28 行 getPrice() 没标 const
}
```

**为什么**：`b` 是 const 对象，它的 `this` 类型是 `const Book*`。`getPrice()` 没标 `const`，它的 `this` 类型是 `Book*`（能改的）。把"能改的指针"从"只读的对象"上取下来，编译器不许。

**注意后面还会跟着一条连带错误。** 如果你写的是 `cout << b.getPrice()`（上面那段就是），C2662 之后立刻还有一条：

```
error C2088: 内置运算符 '<<' 无法应用于类型为 'std::ostream' 的操作数
```

这是**连锁反应**，不是第二个独立问题：`b.getPrice()` 那一句已经废了，`cout << (...)` 拿不到 `double`，`<<` 自然匹配不上。**只看第一条**，把 `const` 补上，C2088 会自己消失。

新手最容易在这条上跑偏的地方：C2088 的报错文本里出现了 `std::ostream`，于是有人跑去改 `#include <iostream>`、改 `using namespace std;`、甚至怀疑编译器坏了 —— 全都是白忙。**报错要从第一条往下读，第一条才是根因。**

**怎么改**：给 `Book.h` 里的 getter 加 `const` 尾缀（这一条要改 7 个 getter，一次性全改）：

```cpp
	string getTitle() const;
	string getAuthor() const;
	string getPublisher() const;
	string getISBN() const;
	string getId() const;
	double getPrice() const;
	bool getIsBorrowed() const;
```

对应的 `Book.cpp` 里，实现部分也要跟着加 `const`：

```cpp
double Book::getPrice() const
{
	return m_price;
}
```

**注意**：声明和实现的 `const` 必须**两边都写**。只改 `.h` 不改 `.cpp`，编译器认为是两个不同的函数，报 `LNK2019`（找不到 `getPrice() const` 的定义）—— 只有声明没实现，正好撞上下面第 5 条。

### 第 3 条：`C2512` / `C2280` —— 成员没有默认构造函数

这条要分两种场景说，因为**报错编号不一样**，很多人以为是两个不同的错误。

**场景 A：你自己写了构造函数但没在初始化列表里构造成员。**

**真实报错**：

```
error C2512: “Holder”: 没有合适的默认构造函数可用
note: 参见“Holder”的声明
```

**触发的代码**：

```cpp
struct Item
{
	Item(int v) : x(v) {}        // 有带参构造，默认构造被"挤掉"了
	int x;
};

struct Holder
{
	Item m;
	Holder(int v) : m(v) {}      // 这个是对的
};

int main()
{
	Holder h;                    // ← 报 C2512：需要 Holder 的默认构造，但它要求传 v
	return 0;
}
```

看清楚：`C2512` 报的是 **`Holder`** 没有默认构造函数，不是 `Item`。因为 `Holder h;` 要求一个能无参调用的 `Holder` 构造函数，而 `Holder` 只写了一个要 `v` 的。

**怎么改**：

```cpp
	// 改法一：给它一个默认构造函数
	Holder() : m(0) { }
	// 改法二：不要无参构造
	Holder h(1);
```

**场景 B：`Holder` 完全没写构造函数。**

**真实报错**：

```
error C2280: “Holder::Holder(void)”: 尝试引用已删除的函数
note: 编译器已在此处生成“Holder::Holder”
note: “Holder::Holder(void)”: 由于 数据成员“Holder::m”不具备相应的 默认构造函数 或重载解决不明确，因此已隐式删除函数
```

**触发的代码**：

```cpp
struct Holder
{
	Item m;      // Item 没有默认构造函数
};               // Holder 一个构造函数都没写

int main()
{
	Holder h;    // ← 报 C2280
	return 0;
}
```

**为什么两个编号不一样**：`Holder` 自己没写构造函数时，编译器会"尽力"给你合成一个（`= default`）。但它检查成员时发现 `Item` 没默认构造，合成的那个没法实现，于是把它标成 **`= delete`**（已删除）。你用到它 → 报"尝试引用已删除的函数" = `C2280`。而场景 A 里你自己写了构造函数，编译器就不合成了，于是报"没有合适的默认构造函数" = `C2512`。

**怎么改**：给 `Item` 加一个默认构造函数，或者让 `Holder` 的构造函数显式初始化 `m`。

```cpp
struct Item
{
	Item() : x(0) { }            // 补一个默认构造
	Item(int v) : x(v) { }
	int x;
};
```

**你会在哪遇到**：给 `Book` 加一个新的成员对象（Q5 那种情况），如果那个类没有默认构造函数，`Book` 的默认构造函数立刻编译不过。项目里 `Book::Book()` 是手写的，它必须能构造 `m_title` 等全部成员；一旦加了需要参数的新成员，`Book()` 就要跟着改初始化列表。

### 第 4 条：`C2530` —— 引用声明了却没绑定

**真实报错**：

```
error C2530: “r”: 必须初始化引用
```

**触发的代码**：

```cpp
	Book& r;           // ← 报 C2530。以为"先声明，后面再赋值"
	r = someBook;
```

**为什么**：引用不是"一个可以后填的变量"，它从出生那一刻起就必须绑在一个已存在的对象上，而且**一辈子不能改绑**。所以"先声明后赋值"这条路根本不存在，语法上就堵死了。

**怎么改**：

```cpp
	Book& r = someBook;      // 声明的同时绑定
```

**顺带一个陷阱**（同一个 `C2530` 的变种）：

```cpp
	Book& r;                    // C2530，编译到这一行就停
	if (x) r = a; else r = b;   // 就算上面过了编译，这句也不是"改绑"，是把 a 或 b 的内容赋给 r 绑定的那个对象
```

**正确写法用指针**：

```cpp
	Book* p = (x ? &a : &b);    // 指针可以后赋、可以改指
```

**你会在哪遇到**：想写 `Book& r;` 然后根据条件绑不同对象 —— 这个需求只能用指针。引用只在"确定绑谁、只绑一次"时用。

### 第 5 条：`LNK2019` —— 声明了、调用了，就是没有实现

**真实报错**：

```
e5_LNK2019.obj : error LNK2019: 无法解析的外部符号 "public: void __cdecl User::returnBook(void)" (?returnBook@User@@QEAAXXZ)，函数 main 中引用了该符号
e5_LNK2019.exe : fatal error LNK1120: 1 个无法解析的外部命令
```

**注意这条报的是 `LNK` 不是 `C`** —— 它是**链接器**报的，不是编译器。含义完全不同：编译阶段每个 `.cpp` 单独过一遍，编译器只检查"这个名字我用得对不对、类型对不对"，它不管这个名字在别处有没有实现。等所有 `.obj` 交给链接器拼成 `.exe` 时，链接器发现"`User::returnBook` 有人要、没人给"，才报 `LNK2019`。

**触发的三种情况**（都会报这一条）：

1. **`User.h` 里声明了，`User.cpp` 里没写实现**：
   ```cpp
   // User.h
   void returnBook(Book& book);
   // User.cpp —— 完全没写 User::returnBook 的函数体
   ```
2. **写了实现，但实现时把类名漏了**（写成全局函数）：
   ```cpp
   void returnBook(Book& book) { ... }        // ← 漏了 User::
   // 这是全局函数，不是 User 的成员，链接器找不到 User::returnBook
   ```
3. **`User.cpp` 没被加进项目**：`User.cpp` 文件在磁盘上，但没在"解决方案资源管理器"里，编译时它根本没参与，自然没有 `.obj`。

**怎么改**：

- 情况 1：把函数体补上。
- 情况 2：改成 `void User::returnBook(Book& book) { ... }`。
- 情况 3：右键项目 → 添加 → 现有项 → 选 `User.cpp`。（或者把实现直接写进 `User.h` 里，那就成了内联，不需要 `.cpp`。）

**看懂那个乱码一样的修饰名**：`?returnBook@User@@QEAAXXZ` 是 MSVC 的"修饰名"（mangled name），里面 `@User@` 说明它属于 `User` 类，`QEAA` 编码了调用约定，`XXZ` 里的 `X` 表示 `void` 返回值、`X` 表示 `void` 参数。**看到 `@类名@` 就直接去那个类里找函数体有没有写。**
