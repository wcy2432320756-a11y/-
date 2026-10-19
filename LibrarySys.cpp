// 实验一·实验要求：多文件方式组织代码（.h 声明 / .cpp 定义 / 本文件生成对象）
// Resource 基类 | Book 一般图书 | Magazine 杂志 | EResource 电子资源 | Library 组合类 | User 依赖类

#define NOMINMAX
#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "Resource.h"
#include "Book.h"
#include "Magazine.h"
#include "EResource.h"
#include "Library.h"
#include "User.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

string trim(const string& s)  // 去掉首尾空白
{
    std::size_t begin = s.find_first_not_of(" \t\r\n");
    if (begin == string::npos)
        return string();
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

string readLine(const string& tip)  // 实验三：界面提示友好，给出输入提示
{
    cout << tip;
    string line;
    if (!std::getline(cin, line))
        return string();
    return trim(line);
}

int readInt(const string& tip)  // 输入不是合法整数时返回 -1
{
    string line = readLine(tip);
    if (line.empty())
        return -1;
    char* end = nullptr;
    long value = std::strtol(line.c_str(), &end, 10);
    if (end == line.c_str() || *end != '\0')
        return -1;
    return static_cast<int>(value);
}

double readDouble(const string& tip)  // 输入不是合法小数时返回 -1.0
{
    string line = readLine(tip);
    if (line.empty())
        return -1.0;
    char* end = nullptr;
    double value = std::strtod(line.c_str(), &end);
    if (end == line.c_str() || *end != '\0')
        return -1.0;
    return value;
}

void showMenu(const User& user)  // 实验三：给出菜单选项，每条对应功能清单一项
{
    cout << endl;
    cout << "================ 图书资源借阅管理系统 ================" << endl;
    cout << "  当前学生：" << user.getName() << "（" << user.getId() << "）"
         << "  已借 " << user.getBorrownum() << " / " << user.getMaxnum() << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "  1. 显示全部资源          2. 显示某一资源" << endl;
    cout << "  3. 借阅资源              4. 归还资源" << endl;
    cout << "  5. 我的借阅记录          6. 新增资源" << endl;
    cout << "  7. 删除资源              8. 增加馆藏数量" << endl;
    cout << "  9. 减少馆藏数量          0. 退出" << endl;
    cout << "=====================================================" << endl;
}

void showAll(const Library& lib)  // 实验三：显示所有图书或资源情况
{
    lib.showAll();
}

void queryById(const Library& lib)  // 实验三：显示某一资源，按类型显示不同信息
{
    string id = readLine("请输入要显示的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* p = lib.findById(id);
    if (p == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    const Resource& ref = *p;  // 实验四：必须用基类引用调虚函数，不能是子类对象
    cout << "---------- 资源信息 ----------" << endl;
    ref.display();
}

void borrowResource(Library& lib, User& user)  // 实验三：学生借阅；实验二：判断双方状态才借成功
{
    string id = readLine("请输入要借阅的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* r = lib.findById(id);
    if (r == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    if (user.hasBorrowed(r))  // 实验二：可以输出借书是否成功的结果
    {
        cout << "  你已经借了《" << r->getTitle() << "》，还没有归还。" << endl;
        return;
    }
    if (user.getBorrownum() >= user.getMaxnum())  // 学生状态不允许
    {
        cout << "  已借 " << user.getBorrownum() << " 本，达到上限 "
             << user.getMaxnum() << " 本，不能再借。" << endl;
        return;
    }
    if (!r->isAvailable())  // 书的状态不允许
    {
        cout << "  《" << r->getTitle() << "》已经全部借出，暂时借不到。" << endl;
        return;
    }
    if (user.borrowResource(r))  // 两边状态都允许，交给 User 完成状态改变
        cout << "  借阅成功：《" << r->getTitle() << "》"
             << "   剩余可借 " << r->getCount() << " / " << r->getTotal() << endl;
    else
        cout << "  借阅失败。" << endl;
}

void returnResource(Library& lib, User& user)  // 实验三：学生归还图书或资源
{
    if (user.getBorrownum() == 0)
    {
        cout << "  你当前没有借任何资源。" << endl;
        return;
    }
    user.displayRecords();

    string id = readLine("请输入要归还的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* r = lib.findById(id);
    if (r == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    if (!user.hasBorrowed(r))  // 只能还自己借过的那本
    {
        cout << "  你的借阅记录里没有这本，无法归还。" << endl;
        return;
    }
    if (user.returnResource(r))
        cout << "  归还成功：《" << r->getTitle() << "》"
             << "   现在可借 " << r->getCount() << " / " << r->getTotal() << endl;
    else
        cout << "  归还失败。" << endl;
}

void showMyRecords(const User& user)  // 实验二：学生借了的书的相关信息
{
    user.displayRecords();
}

void addBook(Library& lib)  // 实验三：增加新的图书或资源 —— 一般图书
{
    cout << "---- 新增一般图书 ----" << endl;

    string id = readLine("  资源编号（如 B004，0 取消）: ");
    if (id == "0" || id.empty())
        return;
    if (lib.findById(id) != nullptr)
    {
        cout << "  编号 " << id << " 已经存在，不能重复添加。" << endl;
        return;
    }
    string title = readLine("  书名: ");
    if (title.empty())
    {
        cout << "  书名不能为空，已取消。" << endl;
        return;
    }
    string author = readLine("  作者: ");
    string publisher = readLine("  出版社: ");

    string isbn;
    for (int attempt = 1; attempt <= 3; ++attempt)  // 实验一：验证 isbn 合法性，最多重输 3 次
    {
        isbn = readLine("  ISBN（10 位或 13 位，可带连字符）: ");
        if (isbn.empty())
        {
            cout << "  ISBN 不能为空，已取消。" << endl;
            return;
        }
        if (Book::checkISBN(isbn))
            break;
        cout << "  ISBN 校验不通过（位数不对或校验位错误）。" << endl;
        if (attempt == 3)
        {
            cout << "  连续 3 次不合法，放弃新增。" << endl;
            return;
        }
    }
    double price = readDouble("  价格（元）: ");
    while (price < 0.0)
        price = readDouble("  价格不能为负，请重新输入: ");

    int pages = readInt("  页数: ");
    while (pages < 0)
        pages = readInt("  页数不能为负，请重新输入: ");

    int total = readInt("  馆藏数量: ");
    while (total <= 0)
        total = readInt("  馆藏数量必须大于 0，请重新输入: ");

    Book* b = new Book(id, title, author, publisher, isbn, price, pages, total, total); // 实验三：new 派生类对象
    if (lib.addResource(b))
        cout << "  已新增一般图书《" << title << "》。" << endl;
    else
    {
        cout << "  新增失败。" << endl;
        delete b;
    }
}

void addMagazine(Library& lib)  // 实验三：增加新的图书或资源 —— 杂志
{
    cout << "---- 新增杂志 ----" << endl;

    string id = readLine("  资源编号（如 M002，0 取消）: ");
    if (id == "0" || id.empty())
        return;
    if (lib.findById(id) != nullptr)
    {
        cout << "  编号 " << id << " 已经存在，不能重复添加。" << endl;
        return;
    }
    string title = readLine("  杂志名称: ");
    if (title.empty())
    {
        cout << "  名称不能为空，已取消。" << endl;
        return;
    }
    string publisher = readLine("  出版单位: ");
    string issue = readLine("  期号（如 2024年第3期）: ");

    int month = readInt("  出版月份（1-12）: ");  // 实验一：数据成员的合法性验证
    while (month < 1 || month > 12)
        month = readInt("  月份只能是 1-12，请重新输入: ");

    double price = readDouble("  单期价格（元）: ");
    while (price < 0.0)
        price = readDouble("  价格不能为负，请重新输入: ");

    int total = readInt("  馆藏数量: ");
    while (total <= 0)
        total = readInt("  馆藏数量必须大于 0，请重新输入: ");

    Magazine* m = new Magazine(id, title, publisher, price, issue, month, total, total); // 实验三：new 派生类对象
    if (lib.addResource(m))
        cout << "  已新增杂志《" << title << "》。" << endl;
    else
    {
        cout << "  新增失败。" << endl;
        delete m;
    }
}

void addEResource(Library& lib)  // 实验三：增加新的图书或资源 —— 电子资源
{
    cout << "---- 新增电子资源 ----" << endl;

    string id = readLine("  资源编号（如 E002，0 取消）: ");
    if (id == "0" || id.empty())
        return;
    if (lib.findById(id) != nullptr)
    {
        cout << "  编号 " << id << " 已经存在，不能重复添加。" << endl;
        return;
    }
    string title = readLine("  资源名称: ");
    if (title.empty())
    {
        cout << "  名称不能为空，已取消。" << endl;
        return;
    }
    string provider = readLine("  提供方: ");
    string url = readLine("  访问地址: ");

    string format = readLine("  格式（PDF / EPUB / MP4）: ");  // 实验一：数据成员的合法性验证
    while (format != "PDF" && format != "EPUB" && format != "MP4")
        format = readLine("  只支持 PDF / EPUB / MP4，请重新输入: ");

    double sizeMB = readDouble("  文件大小（MB）: ");
    while (sizeMB < 0.0)
        sizeMB = readDouble("  大小不能为负，请重新输入: ");

    double price = readDouble("  价格（元，免费填 0）: ");
    while (price < 0.0)
        price = readDouble("  价格不能为负，请重新输入: ");

    int total = readInt("  可借份数: ");
    while (total <= 0)
        total = readInt("  可借份数必须大于 0，请重新输入: ");

    EResource* e = new EResource(id, title, provider, price, url, format, sizeMB, total, total); // 实验三：new 派生类对象
    if (lib.addResource(e))
        cout << "  已新增电子资源《" << title << "》。" << endl;
    else
    {
        cout << "  新增失败。" << endl;
        delete e;
    }
}

void addResource(Library& lib)  // 实验三：增加新的图书或资源，先选类型
{
    cout << "  1-一般图书   2-杂志   3-电子资源   0-返回" << endl;
    int choice = readInt("请选择资源类型: ");
    if (choice == 1)
        addBook(lib);
    else if (choice == 2)
        addMagazine(lib);
    else if (choice == 3)
        addEResource(lib);
    else if (choice == 0)
        return;
    else
        cout << "  没有这个选项。" << endl;
}

void removeResource(Library& lib)  // 实验三：删除某一图书或资源
{
    string id = readLine("请输入要删除的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* r = lib.findById(id);
    if (r == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    if (r->getCount() != r->getTotal())  // 有份数借出在外就不能删，否则借阅记录悬空
    {
        cout << "  《" << r->getTitle() << "》还有 "
             << (r->getTotal() - r->getCount()) << " 份借出未还，不能删除。" << endl;
        return;
    }
    string confirm = readLine("  确认删除《" + r->getTitle() + "》？(y/n): ");
    if (confirm != "y" && confirm != "Y")
    {
        cout << "  已取消。" << endl;
        return;
    }
    if (lib.removeResource(id))
        cout << "  已删除资源 " << id << "。" << endl;
    else
        cout << "  删除失败。" << endl;
}

void addCopies(Library& lib)  // 实验三：增加已有图书或资源的数量
{
    string id = readLine("请输入要增加数量的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* r = lib.findById(id);
    if (r == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    cout << "  《" << r->getTitle() << "》当前可借/馆藏："
         << r->getCount() << " / " << r->getTotal() << endl;

    int n = readInt("  增加几份: ");
    if (n <= 0)
    {
        cout << "  数量必须大于 0。" << endl;
        return;
    }
    r->addCopies(n);
    cout << "  已增加 " << n << " 份，现在 " << r->getCount()
         << " / " << r->getTotal() << endl;
}

void reduceCopies(Library& lib)  // 实验三：减少已有图书或资源的数量
{
    string id = readLine("请输入要减少数量的资源编号（输入 0 取消）: ");
    if (id == "0" || id.empty())
        return;

    Resource* r = lib.findById(id);
    if (r == nullptr)
    {
        cout << "  没有找到编号为 " << id << " 的资源。" << endl;
        return;
    }
    cout << "  《" << r->getTitle() << "》当前可借/馆藏："
         << r->getCount() << " / " << r->getTotal() << endl;

    int n = readInt("  减少几份: ");
    if (n <= 0)
    {
        cout << "  数量必须大于 0。" << endl;
        return;
    }
    if (r->reduceCopies(n))
        cout << "  已减少 " << n << " 份，现在 " << r->getCount()
             << " / " << r->getTotal() << endl;
    else
        cout << "  减少失败：可借数量只有 " << r->getCount()
             << " 份，借出去的那些不能减。" << endl;
}

void initLibrary(Library& lib)  // 实验一：生成对象让功能发挥作用；实验三：new 三种资源
{
    lib.addResource(new Book("B001", "C++ Primer 中文版（第5版）", "Stanley B. Lippman",
                             "电子工业出版社", "9787121358579", 128.0, 838, 3, 3));
    lib.addResource(new Book("B002", "算法导论（原书第3版）", "Thomas H. Cormen",
                             "机械工业出版社", "9787111407010", 128.0, 780, 2, 2));
    lib.addResource(new Book("B003", "Effective C++（第3版）", "Scott Meyers",
                             "电子工业出版社", "9787121155888", 89.0, 300, 1, 1));

    lib.addResource(new Magazine("M001", "程序员", "《程序员》编辑部",
                                 25.0, "2024年第3期", 3, 2, 2));

    lib.addResource(new EResource("E001", "C++ 标准文档 N4950", "ISO/IEC",
                                  0.0, "https://eel.is/c++draft/", "PDF", 12.5, 5, 5));
}

int main()
{
    SetConsoleOutputCP(65001);

    Library lib("校图书馆");  // 实验三：图书馆设计为一个组合类
    initLibrary(lib);

    User user("张三", "20240001", 3);  // 实验二：生成学生对象完成借阅功能

    cout << "欢迎使用" << lib.getName() << "借阅管理系统。" << endl;
    cout << "馆藏共 " << lib.size() << " 种资源。" << endl;

    bool running = true;
    while (running)
    {
        showMenu(user);
        int choice = readInt("请选择功能（0-9）: ");

        switch (choice)
        {
        case 1:  // 实验三：显示所有图书或资源情况
            showAll(lib);
            break;
        case 2:  // 实验三：显示某一图书或资源情况
            queryById(lib);
            break;
        case 3:  // 实验三：学生借阅图书或资源
            borrowResource(lib, user);
            break;
        case 4:  // 实验三：学生归还图书或资源
            returnResource(lib, user);
            break;
        case 5:  // 实验二：学生借了的书的相关信息
            showMyRecords(user);
            break;
        case 6:  // 实验三：增加新的图书或资源
            addResource(lib);
            break;
        case 7:  // 实验三：删除某一图书或资源
            removeResource(lib);
            break;
        case 8:  // 实验三：增加已有图书或资源的数量
            addCopies(lib);
            break;
        case 9:  // 实验三：减少已有图书或资源的数量
            reduceCopies(lib);
            break;
        case 0:
            running = false;
            cout << "再见。" << endl;
            break;
        default:
            cout << "  输入无效，请输入 0 到 9 之间的数字。" << endl;
            break;
        }
    }
    return 0;
}
