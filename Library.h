#pragma once
#include <string>
#include <vector>
#include "Resource.h"

using namespace std;

// 实验四：图书馆拥有资源，基类指针容器统一管理三个派生类。
class Library
{
private:
    string m_name;
    vector<Resource*> m_items;

public:
    Library(string name = "校图书馆");
    ~Library();
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;

    string getName() const;
    int size() const;

    // 添加成功后由图书馆负责 delete；失败时仍由调用者负责释放。
    bool add(Resource* item);
    bool remove(const string& id);
    Resource* find(const string& id) const;
    void showAll() const;
};
