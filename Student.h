#pragma once
#include <string>
#include "Resource.h"

using namespace std;

// 实验二：学生通过资源指针借还书，同时改变自己和资源的状态。
class Student
{
public:
    static constexpr int MaxLoans = 60;

private:
    string m_name;
    string m_id;
    int m_borrowed = 0;
    int m_limit = 5;
    Resource* m_loans[MaxLoans] = {};  // 只引用，不负责 delete

public:
    Student();
    Student(string name, string id, int limit);

    // 借阅记录不能复制，否则两个学生对象会操作同一笔借阅。
    Student(const Student&) = delete;
    Student& operator=(const Student&) = delete;

    string getName() const;
    string getId() const;
    int getBorrowed() const;
    int getLimit() const;

    bool valid() const;
    bool setName(string name);
    bool setId(string id);
    bool setLimit(int limit);

    bool borrow(Resource* item);
    bool giveBack(Resource* item);
    bool hasBorrowed(const Resource* item) const;

    void show() const;
    void showLoans() const;
};
