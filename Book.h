#pragma once
#include <string>
#include "Resource.h"

// 实验一·题目3：图书类（图书馆借阅管理系统）
// 实验三·题目3：作为派生类之一 —— 一般图书
class Book : public Resource
{
private:
    std::string m_author;  // 实验三：不同资源有不同的属性 —— 图书特有的作者
    std::string m_ISBN;    // 实验一：图书isbn
    int m_pages;           // 实验一：页数

public:
    Book();  // 实验一：默认构造
    Book(std::string id, std::string title, std::string author, std::string publisher,
         std::string ISBN, double price, int pages, int count, int total);  // 实验一：重载构造

    std::string getAuthor() const;  // 实验一：基本操作 —— 获取
    std::string getISBN() const;
    int getPages() const;

    void setAuthor(std::string author);  // 实验一：基本操作 —— 修改
    bool setISBN(const std::string& isbn);
    void setPages(int pages);

    static bool checkISBN(const std::string& isbn);  // 实验一：其他操作 —— 验证 isbn 号的合法性

    void display() const override;  // 实验四：派生类重写基类的纯虚函数
    std::string getTypeName() const override;
};
