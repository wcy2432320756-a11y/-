#include "User.h"
#include <iostream>

using std::cout;
using std::endl;

User::User()  // 实验一：默认构造
    : m_borrownum(0), m_maxnum(5)
{
}

User::User(std::string name, std::string id, int maxnum)  // 实验一：重载构造
    : m_name(name), m_id(id), m_borrownum(0), m_maxnum(maxnum)
{
    if (m_maxnum < 1)  // 实验一：合法性验证
        m_maxnum = 1;
}

std::string User::getName() const      { return m_name; }  // 实验一：基本操作 —— 获取
std::string User::getId() const        { return m_id; }
int         User::getBorrownum() const { return m_borrownum; }
int         User::getMaxnum() const    { return m_maxnum; }

void User::setName(std::string name)  // 实验一：基本操作 —— 修改
{
    if (!name.empty())
        m_name = name;
}

void User::setId(std::string id)
{
    if (!id.empty())
        m_id = id;
}

bool User::setMaxnum(int maxnum)
{
    if (maxnum < m_borrownum)
        return false;
    m_maxnum = maxnum;
    return true;
}

bool User::borrowResource(Resource* r)  // 实验二：学生借书 —— 依赖关系，参数传地址
{
    if (r == nullptr)
        return false;
    if (m_borrownum >= m_maxnum)  // 学生这边：借书数量已达上限
        return false;
    if (hasBorrowed(r))  // 学生这边：同一本不能重复借
        return false;
    if (!r->isAvailable())  // 书这边：还有没有可借的数量
        return false;

    if (!r->borrowOne())  // 书的状态改变：可借数量减一
        return false;

    m_records[m_borrownum].set(r);  // 学生的状态改变：记录里添一条
    ++m_borrownum;
    return true;
}

bool User::returnResource(Resource* r)  // 实验三：学生归还图书或资源
{
    if (r == nullptr)
        return false;

    for (int i = 0; i < m_borrownum; ++i)
    {
        if (m_records[i].m_resource != r)  // 只能还自己借过的那本，防止借 A 还 B
            continue;

        r->returnOne();  // 书的状态改回可借：可借数量加一

        for (int j = i; j < m_borrownum - 1; ++j)  // 记录槽左移，把洞填掉
            m_records[j] = m_records[j + 1];

        m_records[m_borrownum - 1].m_resource = nullptr;
        --m_borrownum;
        return true;
    }
    return false;
}

bool User::hasBorrowed(const Resource* r) const  // 实验二：判断学生和书的状态时要用
{
    for (int i = 0; i < m_borrownum; ++i)
    {
        if (m_records[i].m_resource == r)
            return true;
    }
    return false;
}

void User::display() const  // 实验一：基本操作 —— 输出
{
    cout << "  姓名    : " << m_name << endl;
    cout << "  学号    : " << m_id << endl;
    cout << "  已借/限额: " << m_borrownum << " / " << m_maxnum << endl;
}

void User::displayRecords() const  // 实验二：学生借了的书的相关信息
{
    cout << "========== " << m_name << " 的借阅记录（共 "
         << m_borrownum << " 条） ==========" << endl;
    if (m_borrownum == 0)
    {
        cout << "  （还没有借过任何资源）" << endl;
        return;
    }
    for (int i = 0; i < m_borrownum; ++i)
    {
        const Resource* r = m_records[i].m_resource;

        cout << "  " << (i + 1) << ". [" << r->getTypeName() << "] "
             << r->getTitle() << "（编号 " << r->getId() << "）" << endl;
    }
}
