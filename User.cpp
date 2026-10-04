#include "User.h"
#include <iostream>
using namespace std;

//默认构造函数
User::User()
{
	m_name = "";
	m_id = "";
	m_borrownum = 0;
	m_maxnum = 0;
}

//带参构造函数
User::User(string name, string id, int borrownum, int maxnum)
{
	m_name = name;
	m_id = id;
	m_borrownum = borrownum;
	m_maxnum = maxnum;
}

//获取
string User::getName()
{
	return m_name;
}

string User::getId()
{
	return m_id;
}

int User::getBorrownum()
{
	return m_borrownum;
}

int User::getMaxnum()
{
	return m_maxnum;
}

//修改 设置
void User::setName(string name)
{
	m_name = name;
}

void User::setId(string id)
{
	m_id = id;
}

void User::setBorrownum(int borrownum)
{
	m_borrownum = borrownum;
}

void User::setMaxnum(int maxnum)
{
	m_maxnum = maxnum;
}

//展示个人信息
void User::display()
{
	cout << "姓名: " << m_name << endl;
	cout << "学号: " << m_id << endl;
	cout << "已借数量: " << m_borrownum << " 本（最多可借 " << m_maxnum << " 本）" << endl;
}

//借书（依赖关系：参数传引用，图书状态会改变）—— 第二视频逐帧确认
void User::borrowBook(Book& book)
{
	if (m_borrownum < m_maxnum)
	{
		if (book.getIsBorrowed())
		{
			cout << "This book is borrowed." << endl;
			return;
		}
		else
		{
			m_records[m_borrownum].setBook(book);
			m_borrownum++;
			book.setIsBorrowed(true);
		}
	}
	else
	{
		cout << "You have borrowed too many books." << endl;
	}
}

//还书（依赖关系：参数传引用，图书状态恢复）—— 视频未展示，按依赖关系逻辑补齐
void User::returnBook(Book& book)
{
	if (m_borrownum <= 0)
	{
		cout << "当前没有借书记录，无法还书。" << endl;
		return;
	}
	book.setIsBorrowed(false);
	m_borrownum--;
	cout << "还书成功：" << book.getTitle() << endl;
}

//显示借阅记录—— 视频未展示，按记录数组补齐
void User::displayRecords()
{
	if (m_borrownum == 0)
	{
		cout << "暂无借阅记录。" << endl;
		return;
	}
	for (int i = 0; i < m_borrownum; i++)
	{
		cout << "第 " << (i + 1) << " 本: " << m_records[i].getBook().getTitle() << endl;
	}
}
