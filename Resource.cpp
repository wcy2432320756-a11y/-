#include "Resource.h"
#include <iostream>

using std::cout;
using std::endl;

Resource::Resource()  // 实验一：默认构造，成员初始化为一定的值
    : m_price(0.0), m_count(0), m_total(0)
{
}

Resource::Resource(std::string id, std::string title, std::string publisher,
                   double price, int count, int total)  // 实验一：重载构造 + 数据成员的合法性验证
    : m_id(id), m_title(title), m_publisher(publisher),
      m_price(price), m_count(count), m_total(total)
{
    if (m_price < 0.0)  // 价格不能为负
        m_price = 0.0;
    if (m_count < 0)  // 可借数量不能为负
        m_count = 0;
    if (m_total < m_count)  // 馆藏总数不能小于可借数量
        m_total = m_count;
}

Resource::~Resource()  // 实验四：虚析构，delete 基类指针时派生类析构也会执行
{
}

std::string Resource::getId() const        { return m_id; }  // 实验一：基本操作 —— 获取
std::string Resource::getTitle() const     { return m_title; }
std::string Resource::getPublisher() const { return m_publisher; }
double      Resource::getPrice() const     { return m_price; }
int         Resource::getCount() const     { return m_count; }
int         Resource::getTotal() const     { return m_total; }

bool Resource::isAvailable() const  // 实验二：书的状态 —— 还有可借数量就可借
{
    return m_count > 0;
}

void Resource::setTitle(std::string title)  // 实验一：基本操作 —— 修改
{
    if (!title.empty())
        m_title = title;
}

void Resource::setPublisher(std::string publisher)
{
    m_publisher = publisher;
}

bool Resource::setPrice(double price)
{
    if (price < 0.0)
        return false;
    m_price = price;
    return true;
}

bool Resource::setTotal(int total)
{
    int borrowed = m_total - m_count;
    if (total < 0 || total < borrowed)
        return false;
    m_total = total;
    m_count = total - borrowed;
    return true;
}

bool Resource::addCopies(int n)  // 实验三：增加已有图书或资源的数量
{
    if (n <= 0)
        return false;
    m_total += n;
    m_count += n;
    return true;
}

bool Resource::reduceCopies(int n)  // 实验三：减少已有图书或资源的数量（只减没借出去的）
{
    if (n <= 0 || n > m_count)
        return false;
    m_total -= n;
    m_count -= n;
    return true;
}

bool Resource::borrowOne()  // 实验二：借阅成功时书的状态改变 —— 可借数量减一
{
    if (m_count <= 0)
        return false;
    --m_count;
    return true;
}

bool Resource::returnOne()  // 实验二：归还时书的状态改回可借 —— 可借数量加一
{
    if (m_count >= m_total)
        return false;
    ++m_count;
    return true;
}

void Resource::displayCommon() const  // 实验三：三种资源共有的信息，派生类 display() 先调这里
{
    cout << "  编号    : " << m_id << "    类型: " << getTypeName() << endl;
    cout << "  名称    : " << m_title << endl;
    cout << "  出版/来源: " << m_publisher << endl;
    cout << "  价格    : " << m_price << " 元" << endl;
    cout << "  可借/馆藏: " << m_count << " / " << m_total
         << (m_count > 0 ? "   （可借）" : "   （已全部借出）") << endl;
}
