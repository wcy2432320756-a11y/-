#include "Library.h"
#include <iostream>

using namespace std;

Library::Library(string name) : m_name(name) {}

Library::~Library()
{
    for (Resource* item : m_items)
        delete item;  // 基类虚析构，保证派生对象正确释放。
}

string Library::getName() const { return m_name; }
int Library::size() const { return static_cast<int>(m_items.size()); }

bool Library::add(Resource* item)
{
    if (item == nullptr || !item->valid()
        || find(item->getId()) != nullptr)
        return false;
    m_items.push_back(item);
    return true;
}

bool Library::remove(const string& id)
{
    for (auto it = m_items.begin(); it != m_items.end(); ++it)
    {
        Resource* item = *it;
        if (item->getId() != id)
            continue;
        if (item->getLeft() != item->getTotal())
            return false;  // 借出未还时不能删除，避免学生记录悬空。
        delete item;
        m_items.erase(it);
        return true;
    }
    return false;
}

Resource* Library::find(const string& id) const
{
    for (Resource* item : m_items)
        if (item->getId() == id)
            return item;
    return nullptr;
}

void Library::showAll() const
{
    cout << "========== " << m_name << " 馆藏清单（共 "
         << m_items.size() << " 种） ==========\n";
    if (m_items.empty())
        cout << "  （暂无馆藏）\n";
    for (Resource* item : m_items)
    {
        cout << "------------------------------\n";
        item->show();  // 实验四：基类指针调用虚函数，显示各自特有信息。
    }
}
