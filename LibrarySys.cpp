// 实验一：多文件组织；实验二：学生借书；实验三、四：继承和虚函数多态。
#define NOMINMAX
#include <windows.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include "Book.h"
#include "Magazine.h"
#include "Digital.h"
#include "Library.h"
#include "Student.h"

using namespace std;

// 读取成功返回 true；输入结束或读取失败返回 false，由调用处退出操作。
bool readLine(const string& prompt, string& text)
{
    cout << prompt;
    string line;
    if (!getline(cin, line))
        return false;
    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        text = "";
    else
    {
        size_t last = line.find_last_not_of(" \t\r\n");
        text = line.substr(first, last - first + 1);
    }
    return true;
}

bool readRequired(const string& prompt, string& text)
{
    while (readLine(prompt, text))
    {
        if (!text.empty())
            return true;
        cout << "  输入不能为空，请重新输入。\n";
    }
    return false;
}

bool readInt(const string& prompt, int& value, int minimum,
             int maximum = numeric_limits<int>::max())
{
    string line;
    while (readLine(prompt, line))
    {
        istringstream input(line);
        int number = 0;
        if ((input >> number) && (input >> ws).eof()
            && number >= minimum && number <= maximum)
        {
            value = number;
            return true;
        }
        cout << "  请输入 " << minimum << " 到 " << maximum << " 之间的整数。\n";
    }
    return false;
}

bool readDouble(const string& prompt, double& value)
{
    string line;
    while (readLine(prompt, line))
    {
        istringstream input(line);
        double number = 0.0;
        if ((input >> number) && (input >> ws).eof()
            && isfinite(number) && number >= 0.0)
        {
            value = number;
            return true;
        }
        cout << "  请输入有限的非负数，不能包含其他字符。\n";
    }
    return false;
}

void showMenu(const Student& stu)
{
    cout << "\n================ 图书资源借阅管理系统 ================\n"
         << "  当前学生：" << stu.getName() << "（" << stu.getId() << "）"
         << "  已借 " << stu.getBorrowed() << " / " << stu.getLimit() << '\n'
         << "  1. 显示全部资源          2. 显示某一资源\n"
         << "  3. 借阅资源              4. 归还资源\n"
         << "  5. 我的借阅记录          6. 新增资源\n"
         << "  7. 删除资源              8. 增加馆藏数量\n"
         << "  9. 减少馆藏数量         10. 类与对象演示\n"
         << "  0. 退出\n"
         << "=====================================================\n";
}

Resource* pick(const Library& lib)
{
    string id;
    if (!readLine("请输入资源编号（0 或留空取消）: ", id) || id.empty() || id == "0")
        return nullptr;
    Resource* item = lib.find(id);
    if (item == nullptr)
        cout << "  没有找到编号为 " << id << " 的资源。\n";
    return item;
}

void query(const Library& lib)
{
    Resource* item = pick(lib);
    if (item != nullptr)
        item->show();  // 基类指针调用虚函数，自动选择实际资源类型。
}

void borrowItem(const Library& lib, Student& stu)
{
    Resource* item = pick(lib);
    if (item == nullptr)
        return;
    if (stu.borrow(item))
        cout << "  借阅成功：《" << item->getTitle() << "》，剩余可借 "
             << item->getLeft() << " 份。\n";
    else
        cout << "  借阅失败：重复借阅、已达借阅上限，或资源暂无可借份数。\n";
}

void returnItem(const Library& lib, Student& stu)
{
    stu.showLoans();
    if (stu.getBorrowed() == 0)
        return;
    Resource* item = pick(lib);
    if (item == nullptr)
        return;
    if (stu.giveBack(item))
        cout << "  归还成功：《" << item->getTitle() << "》，现在可借 "
             << item->getLeft() << " 份。\n";
    else
        cout << "  归还失败：未借过该资源，或库存状态异常。\n";
}

void addItem(Library& lib)
{
    cout << "  1-一般图书   2-杂志   3-电子资源   0-返回\n";
    int type = 0;
    if (!readInt("请选择资源类型: ", type, 0, 3) || type == 0)
        return;
    string id;
    if (!readLine("  资源编号（0 或留空取消）: ", id) || id.empty() || id == "0")
        return;
    if (lib.find(id) != nullptr)
    {
        cout << "  编号已经存在，不能重复添加。\n";
        return;
    }

    // 三种资源的公共输入只写一次，各分支只处理自己特有的数据。
    string title, publisher;
    double price = 0.0;
    int total = 0;
    if (!readRequired("  名称: ", title)
        || !readRequired("  出版/来源: ", publisher)
        || !readDouble("  价格（元，免费填 0）: ", price)
        || !readInt("  馆藏数量/可借份数: ", total, 1))
        return;
    Resource* item = nullptr;
    if (type == 1)
    {
        string author;
        if (!readRequired("  作者: ", author))
            return;
        string isbn;
        while (true)
        {
            if (!readLine("  ISBN（10 或 13 位，可带连字符，0 或留空取消）: ", isbn)
                || isbn.empty() || isbn == "0")
                return;
            if (Book::validISBN(isbn))
                break;
            cout << "  ISBN 校验不通过，请重新输入。\n";
        }
        int pages = 0;
        if (!readInt("  页数: ", pages, 0))
            return;
        item = new Book(id, title, author, publisher, isbn, price, pages, total);
    }
    else if (type == 2)
    {
        string issue;
        int month = 0;
        if (!readRequired("  期号（如 2026年第10期）: ", issue)
            || !readInt("  出版月份: ", month, 1, 12))
            return;
        item = new Magazine(id, title, publisher, price, issue, month, total);
    }
    else
    {
        string url;
        if (!readRequired("  访问地址: ", url))
            return;
        string format;
        do
        {
            if (!readRequired("  格式（PDF / EPUB / MP4）: ", format))
                return;
            if (format != "PDF" && format != "EPUB" && format != "MP4")
                cout << "  只支持 PDF / EPUB / MP4。\n";
        } while (format != "PDF" && format != "EPUB" && format != "MP4");
        double sizeMB = 0.0;
        if (!readDouble("  文件大小（MB）: ", sizeMB))
            return;
        item = new Digital(id, title, publisher, price, url, format, sizeMB, total);
    }
    if (lib.add(item))
        cout << "  已新增" << item->getType() << "《" << title << "》。\n";
    else
    {
        delete item;
        cout << "  新增失败。\n";
    }
}

void deleteItem(Library& lib)
{
    Resource* item = pick(lib);
    if (item == nullptr)
        return;
    string confirm;
    if (!readLine("  确认删除《" + item->getTitle() + "》？(y/n): ", confirm))
        return;
    if (confirm != "y" && confirm != "Y")
    {
        cout << "  已取消。\n";
        return;
    }
    string id = item->getId();
    if (lib.remove(id))
        cout << "  已删除资源 " << id << "。\n";
    else
        cout << "  删除失败：资源尚有借出未还的份数，不能删除。\n";
}

void changeStock(const Library& lib, bool increase)
{
    Resource* item = pick(lib);
    if (item == nullptr)
        return;
    cout << "  当前可借/馆藏：" << item->getLeft()
         << " / " << item->getTotal() << '\n';
    int count = 0;
    if (!readInt(increase ? "  增加几份: " : "  减少几份: ", count, 1))
        return;
    bool success = increase ? item->addCopies(count) : item->reduceCopies(count);
    if (success)
        cout << (increase ? "  已增加 " : "  已减少 ") << count << " 份，现在 "
             << item->getLeft() << " / " << item->getTotal() << '\n';
    else
        cout << "  修改失败：减少量不能超过可借数量，增加量不能超出整数范围。\n";
}

void demo()
{
    cout << "========== 实验一：默认构造、修改、获取和校验 ==========\n";
    Book book;  // 独立演示对象，不加入馆藏，不影响实际借阅。
    cout << "  默认构造：价格 " << book.getPrice() << "，页数 " << book.getPages() << '\n';
    book.setTitle("演示图书");
    book.setAuthor("演示作者");
    book.setPublisher("演示出版社");
    book.setISBN("9780306406157");
    book.setPages(100);
    book.setPrice(20.0);
    book.setTotal(2);
    book.show();
    bool changed = book.setPrice(-1.0);
    cout << "  尝试改为负价格：" << (changed ? "成功" : "拒绝")
         << "，原价格保持 " << book.getPrice() << " 元。\n";
    cout << "  错误 ISBN 校验结果："
         << (Book::validISBN("9780306406158") ? "通过" : "不通过") << '\n';
}

int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    Library lib("校图书馆");
    Student stu("王cy", "5120251767", 3);
    cout << "欢迎使用" << lib.getName() << "借阅管理系统，馆藏共 "
         << lib.size() << " 种资源。\n";
    while (cin)
    {
        showMenu(stu);
        int choice = 0;
        if (!readInt("请选择功能（0-10）: ", choice, 0, 10))
            break;
        switch (choice)
        {
        case 1: lib.showAll(); break;
        case 2: query(lib); break;
        case 3: borrowItem(lib, stu); break;
        case 4: returnItem(lib, stu); break;
        case 5: stu.showLoans(); break;
        case 6: addItem(lib); break;
        case 7: deleteItem(lib); break;
        case 8: changeStock(lib, true); break;
        case 9: changeStock(lib, false); break;
        case 10: demo(); break;
        case 0: cout << "再见。\n"; return 0;
        }
    }
    if (cin.eof())
    {
        cout << "\n输入已结束，程序退出。\n";
        return 0;
    }
    cerr << "读取输入失败，程序退出。\n";
    return 1;
}
