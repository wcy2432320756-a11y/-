#include "Magazine.h"
#include <iostream>

using namespace std;

Magazine::Magazine() = default;

Magazine::Magazine(string id, string title, string publisher,
                   double price, string issue, int month, int total)
    : Resource(id, title, publisher, price, total), m_issue(issue), m_month(month)
{
}

bool Magazine::valid() const
{
    return Resource::valid() && !m_issue.empty() && m_month >= 1 && m_month <= 12;
}

string Magazine::getIssue() const { return m_issue; }
int Magazine::getMonth() const { return m_month; }

bool Magazine::setIssue(string issue)
{
    if (issue.empty())
        return false;
    m_issue = issue;
    return true;
}

bool Magazine::setMonth(int month)
{
    if (month < 1 || month > 12)
        return false;
    m_month = month;
    return true;
}

void Magazine::show() const
{
    showCommon();
    cout << "  期号    : " << m_issue << '\n'
         << "  出版月份: " << m_month << " 月\n";
}

string Magazine::getType() const { return "杂志"; }
