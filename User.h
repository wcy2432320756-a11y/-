#pragma once
#include <string>
#include "Resource.h"

// 实验二·题目3：学生借书 —— 依赖关系。学生抽象出借了的书的信息和当前借了多少本
struct BorrowRecord
{
    Resource* m_resource;  // 存指针不存副本，指向图书馆里那一本

    BorrowRecord() : m_resource(nullptr) {}

    void set(Resource* r) { m_resource = r; }
    Resource* getResource() const { return m_resource; }
};

class User
{
private:
    std::string m_name;
    std::string m_id;
    int m_borrownum;             // 实验二：当前借了多少本书
    int m_maxnum;                // 借书上限
    BorrowRecord m_records[60];  // 实验二：借了的书的相关信息（对象数组）

public:
    User();                                              // 实验一：默认构造
    User(std::string name, std::string id, int maxnum);  // 实验一：重载构造

    std::string getName() const;  // 实验一：基本操作 —— 获取
    std::string getId() const;
    int getBorrownum() const;
    int getMaxnum() const;

    void setName(std::string name);  // 实验一：基本操作 —— 修改
    void setId(std::string id);
    bool setMaxnum(int maxnum);

    bool borrowResource(Resource* r);           // 实验二：依赖关系，参数传地址；判断双方状态才借成功
    bool returnResource(Resource* r);           // 实验三：学生归还图书或资源
    bool hasBorrowed(const Resource* r) const;  // 判断这本书是不是已经借过

    void display() const;         // 实验一：基本操作 —— 输出
    void displayRecords() const;  // 实验二：输出借了的书的信息
};
