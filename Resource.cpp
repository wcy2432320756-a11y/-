#include "Resource.h"
#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

Resource::Resource() = default;

Resource::Resource(string id, string title, string publisher,
                   double price, int total)
    : m_id(id), m_title(title), m_publisher(publisher),
      m_price(price), m_left(total), m_total(total)
{
}

bool Resource::valid() const
{
    if (m_id.empty() || m_title.empty() || m_publisher.empty())
        return false;
    if (!isfinite(m_price) || m_price < 0.0)
        return false;
    return m_left >= 0 && m_left <= m_total;
}

string Resource::getId() const { return m_id; }
string Resource::getTitle() const { return m_title; }
string Resource::getPublisher() const { return m_publisher; }
double Resource::getPrice() const { return m_price; }
int Resource::getLeft() const { return m_left; }
int Resource::getTotal() const { return m_total; }
bool Resource::isAvailable() const { return m_left > 0; }

bool Resource::setTitle(string title)
{
    if (title.empty())
        return false;
    m_title = title;
    return true;
}

bool Resource::setPublisher(string publisher)
{
    if (publisher.empty())
        return false;
    m_publisher = publisher;
    return true;
}

bool Resource::setPrice(double price)
{
    if (!isfinite(price) || price < 0.0)
        return false;
    m_price = price;
    return true;
}

bool Resource::setTotal(int total)
{
    int borrowed = m_total - m_left;
    if (total < 0 || total < borrowed)
        return false;
    m_total = total;
    m_left = total - borrowed;
    return true;
}

bool Resource::addCopies(int count)
{
    if (!valid() || count <= 0 || count > numeric_limits<int>::max() - m_total)
        return false;
    m_total += count;
    m_left += count;
    return true;
}

bool Resource::reduceCopies(int count)
{
    if (!valid() || count <= 0 || count > m_left)
        return false;
    m_total -= count;
    m_left -= count;
    return true;
}

bool Resource::borrowOne()
{
    if (!valid() || !isAvailable())
        return false;
    --m_left;
    return true;
}

bool Resource::returnOne()
{
    if (!valid() || m_left >= m_total)
        return false;
    ++m_left;
    return true;
}

void Resource::showCommon() const
{
    cout << "  编号    : " << (m_id.empty() ? "（未编号）" : m_id)
         << "    类型: " << getType() << '\n'
         << "  名称    : " << m_title << '\n'
         << "  出版/来源: " << m_publisher << '\n'
         << "  价格    : " << m_price << " 元\n"
         << "  可借/馆藏: " << m_left << " / " << m_total
         << (isAvailable() ? "   （可借）" : "   （暂无可借份数）") << '\n';
}
