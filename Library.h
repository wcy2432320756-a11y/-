#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "Resource.h"

// 实验三·题目3：将图书馆设计为一个组合类（实验二：以一个类的对象作为另一个类的成员变量）
// 实验四·题目3：图书馆类包含基类（资源类）对象指针数组或动态数组（vector）
class Library
{
private:
    std::string m_name;
    std::vector<Resource*> m_resources;  // 基类指针容器，三种子类对象统一存放

public:
    Library(std::string name = "校图书馆");
    ~Library();  // 实验三：用 delete 释放 new 出来的对象

    Library(const Library&) = delete;               // 实验一·三法则：类里管理着堆内存
    Library& operator=(const Library&) = delete;    // 禁止按字节拷贝，否则会 double free

    std::string getName() const;
    int size() const;

    bool addResource(Resource* r);               // 实验三：增加新的图书或资源
    bool removeResource(const std::string& id);  // 实验三：删除某一图书或资源

    Resource* findById(const std::string& id) const;  // 按编号定位，供各功能使用

    void showAll() const;  // 实验三：显示所有资源；实验四：用基类指针调虚函数
};
