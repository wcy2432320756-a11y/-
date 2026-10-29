#include "Book.h"
#include <iostream>

using namespace std;

Book::Book() = default;

Book::Book(string id, string title, string author, string publisher,
           string isbn, double price, int pages, int total)
    : Resource(id, title, publisher, price, total),
      m_author(author), m_isbn(isbn), m_pages(pages)
{
}

bool Book::valid() const
{
    return Resource::valid() && !m_author.empty() && validISBN(m_isbn) && m_pages >= 0;
}

string Book::getAuthor() const { return m_author; }
string Book::getISBN() const { return m_isbn; }
int Book::getPages() const { return m_pages; }

bool Book::setAuthor(string author)
{
    if (author.empty())
        return false;
    m_author = author;
    return true;
}

bool Book::setISBN(const string& isbn)
{
    if (!validISBN(isbn))
        return false;
    m_isbn = isbn;
    return true;
}

bool Book::setPages(int pages)
{
    if (pages < 0)
        return false;
    m_pages = pages;
    return true;
}

bool Book::validISBN(const string& isbn)
{
    string digits;
    for (char c : isbn)  // 先去掉连字符和空格
    {
        if (c == '-' || c == ' ')
            continue;
        digits += c;
    }
    if (digits.size() == 10)  // ISBN-10：权 10~1 加权求和，能被 11 整除
    {
        int sum = 0;
        for (int i = 0; i < 10; ++i)
        {
            char c = digits[i];
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
    if (digits.size() == 13)  // ISBN-13：权 1、3 交替求和，能被 10 整除
    {
        int sum = 0;
        for (int i = 0; i < 13; ++i)
        {
            char c = digits[i];
            if (c < '0' || c > '9')
                return false;
            int value = c - '0';
            sum += (i % 2 == 0) ? value : value * 3;
        }
        return (sum % 10 == 0);
    }
    return false;  // 既不是 10 位也不是 13 位
}

void Book::show() const
{
    showCommon();
    cout << "  作者    : " << m_author << '\n'
         << "  ISBN    : " << (m_isbn.empty() ? "（未填写）" : m_isbn) << '\n'
         << "  页数    : " << m_pages << " 页\n";
}

string Book::getType() const { return "一般图书"; }
