#pragma once
#include <string>
#include "Resource.h"

// 实验三·题目3：图书资源有不同的类型 —— 本类是杂志
class Magazine : public Resource
{
private:
    std::string m_issue;  // 实验三：杂志特有的期号
    int m_month;          // 实验三：出版月份，合法性验证 1-12

public:
    Magazine();  // 实验一：默认构造
    Magazine(std::string id, std::string title, std::string publisher,
             double price, std::string issue, int month, int count, int total); // 实验一：重载构造

    std::string getIssue() const;  // 实验一：基本操作 —— 获取
    int getMonth() const;

    void setIssue(std::string issue);  // 实验一：基本操作 —— 修改
    bool setMonth(int month);

    void display() const override;  // 实验四：派生类重写基类的纯虚函数
    std::string getTypeName() const override;
};
