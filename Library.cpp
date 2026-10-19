#include "Library.h"
#include <cstddef>
#include <iostream>

using std::cout;
using std::endl;

Library::Library(std::string name)  // 实验三：组合类 —— 整体类负责成员的初始化
    : m_name(name)
{
}

Library::~Library()  // 实验三：用 delete 释放 new 出来的对象
{
    for (std::size_t i = 0; i < m_resources.size(); ++i)
        delete m_resources[i];  // 实验四：基类虚析构，派生类那部分内存才会被释放
    m_resources.clear();
}

std::string Library::getName() const { return m_name; }
int Library::size() const { return static_cast<int>(m_resources.size()); }

bool Library::addResource(Resource* r)  // 实验三：增加新的图书或资源
{
    if (r == nullptr)
        return false;
    if (r->getId().empty())
        return false;
    if (findById(r->getId()) != nullptr)
        return false;          // 编号重复不允许
    m_resources.push_back(r);  // 实验四：参数是基类指针，三种子类都能进来
    return true;
}

bool Library::removeResource(const std::string& id)  // 实验三：删除某一图书或资源
{
    for (std::size_t i = 0; i < m_resources.size(); ++i)
    {
        if (m_resources[i]->getId() != id)
            continue;

        if (m_resources[i]->getCount() != m_resources[i]->getTotal())
            return false;  // 还有份数借出在外，删了会让借阅记录悬空

        delete m_resources[i];
        m_resources.erase(m_resources.begin() + static_cast<std::ptrdiff_t>(i));
        return true;
    }
    return false;
}

Resource* Library::findById(const std::string& id) const  // 实验四：从基类指针数组中按编号取出对象
{
    for (std::size_t i = 0; i < m_resources.size(); ++i)
    {
        if (m_resources[i]->getId() == id)
            return m_resources[i];
    }
    return nullptr;
}

void Library::showAll() const  // 实验三：显示所有图书或资源情况
{
    cout << "========== " << m_name << " 馆藏清单（共 "
         << m_resources.size() << " 种） ==========" << endl;
    if (m_resources.empty())
    {
        cout << "  （暂无馆藏）" << endl;
        return;
    }
    for (std::size_t i = 0; i < m_resources.size(); ++i)
    {
        cout << "--- 第 " << (i + 1) << " 种 ---" << endl;

        m_resources[i]->display();  // 实验四：基类指针调虚函数，按实际类型走各自版本
    }
}
