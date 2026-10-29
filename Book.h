#pragma once
#include "Resource.h"

using namespace std;

// 实验一：图书的构造、获取、修改、输出和 ISBN 校验；实验三：继承公共资源。
class Book : public Resource
{
private:
    string m_author;
    string m_isbn;
    int m_pages = 0;

public:
    Book();
    Book(string id, string title, string author, string publisher,
         string isbn, double price, int pages, int total);

    string getAuthor() const;
    string getISBN() const;
    int getPages() const;

    bool setAuthor(string author);
    bool setISBN(const string& isbn);
    bool setPages(int pages);
    static bool validISBN(const string& isbn);

    bool valid() const override;
    void show() const override;
    string getType() const override;
};
