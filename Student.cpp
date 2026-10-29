#include "Student.h"
#include <iostream>

using namespace std;

Student::Student() = default;

Student::Student(string name, string id, int limit)
    : m_name(name), m_id(id), m_limit(limit)
{
}

bool Student::valid() const
{
    return !m_name.empty() && !m_id.empty() && m_limit >= 1 && m_limit <= MaxLoans
        && m_borrowed >= 0 && m_borrowed <= m_limit;
}

string Student::getName() const { return m_name; }
string Student::getId() const { return m_id; }
int Student::getBorrowed() const { return m_borrowed; }
int Student::getLimit() const { return m_limit; }

bool Student::setName(string name)
{
    if (name.empty())
        return false;
    m_name = name;
    return true;
}

bool Student::setId(string id)
{
    if (id.empty())
        return false;
    m_id = id;
    return true;
}

bool Student::setLimit(int limit)
{
    if (limit < 1 || limit > MaxLoans || limit < m_borrowed)
        return false;
    m_limit = limit;
    return true;
}

bool Student::borrow(Resource* item)
{
    if (!valid() || item == nullptr || m_borrowed >= m_limit
        || m_borrowed >= MaxLoans || hasBorrowed(item))
        return false;
    if (!item->borrowOne())
        return false;
    m_loans[m_borrowed++] = item;
    return true;
}

bool Student::giveBack(Resource* item)
{
    if (!valid() || item == nullptr)
        return false;
    for (int i = 0; i < m_borrowed; ++i)
    {
        if (m_loans[i] != item)
            continue;
        // 库存更新成功后才删除学生记录，不能忽略归还失败。
        if (!item->returnOne())
            return false;
        for (int j = i; j < m_borrowed - 1; ++j)
            m_loans[j] = m_loans[j + 1];
        m_loans[--m_borrowed] = nullptr;
        return true;
    }
    return false;
}

bool Student::hasBorrowed(const Resource* item) const
{
    for (int i = 0; i < m_borrowed; ++i)
        if (m_loans[i] == item)
            return true;
    return false;
}

void Student::show() const
{
    cout << "  姓名    : " << m_name << '\n'
         << "  学号    : " << m_id << '\n'
         << "  已借/限额: " << m_borrowed << " / " << m_limit << '\n';
}

void Student::showLoans() const
{
    cout << "========== " << m_name << " 的当前借阅（共 "
         << m_borrowed << " 条） ==========\n";
    if (m_borrowed == 0)
        cout << "  （当前没有借阅资源）\n";
    for (int i = 0; i < m_borrowed; ++i)
    {
        const Resource* item = m_loans[i];
        cout << "  " << (i + 1) << ". [" << item->getType() << "] "
             << item->getTitle() << "（编号 " << item->getId() << "）\n";
    }
}
