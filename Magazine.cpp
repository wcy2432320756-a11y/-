#include "Magazine.h"
#include <iostream>

using std::cout;
using std::endl;

Magazine::Magazine()  // 实验一：默认构造
    : Resource(), m_month(1)
{
}

Magazine::Magazine(std::string id, std::string title, std::string publisher,
                   double price, std::string issue, int month, int count, int total) // 实验一：重载构造
    : Resource(id, title, publisher, price, count, total),
      m_issue(issue), m_month(month)
{
    if (m_month < 1 || m_month > 12)  // 实验一：数据成员的合法性验证
        m_month = 1;
}

std::string Magazine::getIssue() const { return m_issue; }  // 实验一：基本操作 —— 获取
int         Magazine::getMonth() const { return m_month; }

void Magazine::setIssue(std::string issue)  // 实验一：基本操作 —— 修改
{
    if (!issue.empty())
        m_issue = issue;
}

bool Magazine::setMonth(int month)
{
    if (month < 1 || month > 12)
        return false;
    m_month = month;
    return true;
}

void Magazine::display() const  // 实验三：按资源类型显示不同 —— 杂志多出期号、出版月份
{
    displayCommon();
    cout << "  期号    : " << m_issue << endl;
    cout << "  出版月份: " << m_month << " 月" << endl;
}

std::string Magazine::getTypeName() const  // 实验三：类型名用于区分三种资源
{
    return "杂志";
}
