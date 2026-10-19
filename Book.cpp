#include "Book.h"
#include <iostream>

using std::cout;
using std::endl;

Book::Book()  // 实验一：默认构造
    : Resource(), m_pages(0)
{
}

Book::Book(std::string id, std::string title, std::string author, std::string publisher,
           std::string ISBN, double price, int pages, int count, int total) // 实验一：重载构造
    : Resource(id, title, publisher, price, count, total),
      m_author(author), m_ISBN(ISBN), m_pages(pages)
{
    if (!checkISBN(m_ISBN))  // 实验一：对数据成员进行合法性验证
        m_ISBN.clear();
    if (m_pages < 0)
        m_pages = 0;
}

std::string Book::getAuthor() const { return m_author; }  // 实验一：基本操作 —— 获取
std::string Book::getISBN() const   { return m_ISBN; }
int         Book::getPages() const  { return m_pages; }

void Book::setAuthor(std::string author)  // 实验一：基本操作 —— 修改
{
    if (!author.empty())
        m_author = author;
}

bool Book::setISBN(const std::string& isbn)  // 实验一：改 isbn 同样要验证，不合法就拒绝
{
    if (!checkISBN(isbn))
        return false;
    m_ISBN = isbn;
    return true;
}

void Book::setPages(int pages)
{
    if (pages >= 0)
        m_pages = pages;
}

bool Book::checkISBN(const std::string& isbn)  // 实验一：其他操作 —— 验证 isbn 号的合法性
{
    std::string s;
    for (std::size_t i = 0; i < isbn.size(); ++i)  // 先去掉连字符和空格
    {
        char c = isbn[i];
        if (c == '-' || c == ' ')
            continue;
        s += c;
    }
    if (s.size() == 10)  // ISBN-10：权 10~1 加权求和，能被 11 整除
    {
        int sum = 0;
        for (int i = 0; i < 10; ++i)
        {
            char c = s[static_cast<std::size_t>(i)];
            int value = 0;
            if (c >= '0' && c <= '9')
                value = c - '0';
            else if ((c == 'X' || c == 'x') && i == 9)  // 末位允许 X 表示 10
                value = 10;
            else
                return false;
            sum += value * (10 - i);
        }
        return (sum % 11 == 0);
    }
    if (s.size() == 13)  // ISBN-13：权 1、3 交替求和，能被 10 整除
    {
        int sum = 0;
        for (int i = 0; i < 13; ++i)
        {
            char c = s[static_cast<std::size_t>(i)];
            if (c < '0' || c > '9')
                return false;
            int value = c - '0';
            sum += (i % 2 == 0) ? value : value * 3;
        }
        return (sum % 10 == 0);
    }
    return false;  // 既不是 10 位也不是 13 位
}

void Book::display() const  // 实验三：根据资源类型不同显示的信息也不一样 —— 图书多出作者、ISBN、页数
{
    displayCommon();
    cout << "  作者    : " << m_author << endl;
    cout << "  ISBN    : " << (m_ISBN.empty() ? "（未通过校验）" : m_ISBN) << endl;
    cout << "  页数    : " << m_pages << " 页" << endl;
}

std::string Book::getTypeName() const  // 实验三：类型名用于区分三种资源
{
    return "一般图书";
}
