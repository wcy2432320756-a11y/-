// LibrarySys.cpp : 图书馆借阅管理系统（实验一 Book 类 + 实验二 User 类依赖关系）
// 实验一：Book 类的设计（成员变量、构造函数、get/set、display）
// 实验二：User 类与 Book 类之间的依赖关系——借书/还书（参数传引用，双方状态联动）
// 说明：main 函数与菜单逻辑为根据两个视频功能完善补齐（视频中未逐帧展示）

#include <windows.h>   //SetConsoleOutputCP，用于把控制台输出代码页改成 UTF-8
#include "Book.h"
#include "User.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

//工具函数
int readInt()
{
	int n;
	cin >> n;
	return n;
}

//查找图书（按书名），返回在书库中的下标，找不到返回 -1
int findBook(vector<Book>& books, const string& title)
{
	for (int i = 0; i < (int)books.size(); i++)
	{
		if (books[i].getTitle() == title)
		{
			return i;
		}
	}
	return -1;
}

//显示全部图书
void showAllBooks(vector<Book>& books)
{
	if (books.empty())
	{
		cout << "书库为空。" << endl;
		return;
	}
	for (int i = 0; i < (int)books.size(); i++)
	{
		cout << "【第 " << (i + 1) << " 本】" << endl;
		books[i].display();
		if (books[i].getIsBorrowed())
		{
			cout << "状态: 已借出" << endl;
		}
		else
		{
			cout << "状态: 可借" << endl;
		}
		cout << "----------------------" << endl;
	}
}

//录入新书
void addBook(vector<Book>& books)
{
	string title, author, publisher, ISBN, id;
	double price;
	cout << "请输入书名: ";
	getline(cin, title);   //支持书名含空格
	cout << "请输入作者: ";
	cin >> author;
	cout << "请输入出版社: ";
	cin >> publisher;
	cout << "请输入ISBN: ";
	cin >> ISBN;
	cout << "请输入图书编号: ";
	cin >> id;
	cout << "请输入价格: ";
	cin >> price;
	Book b(title, author, publisher, ISBN, id, price, false);
	books.push_back(b);
	cout << "录入成功。" << endl;
}

//借书
void borrowBook(vector<Book>& books, User& user)
{
	if (user.getBorrownum() >= user.getMaxnum())
	{
		cout << "You have borrowed too many books." << endl;
		return;
	}
	string title;
	cout << "请输入要借的书名: ";
	getline(cin, title);   //支持书名含空格
	int idx = findBook(books, title);
	if (idx == -1)
	{
		cout << "书库中没有这本书。" << endl;
		return;
	}
	user.borrowBook(books[idx]);   //依赖关系：传引用，双方状态联动
}

//还书
void returnBook(vector<Book>& books, User& user)
{
	if (user.getBorrownum() == 0)
	{
		cout << "当前没有借书记录，无法还书。" << endl;
		return;
	}
	string title;
	cout << "请输入要还的书名: ";
	getline(cin, title);   //支持书名含空格
	int idx = findBook(books, title);
	if (idx == -1)
	{
		cout << "书库中没有这本书。" << endl;
		return;
	}
	user.returnBook(books[idx]);   //依赖关系：传引用，图书状态恢复
}

int main()
{
	//源文件是 UTF-8，而控制台默认代码页是 GBK(936)，直接输出中文会变乱码
	//这一行把控制台输出代码页也切成 UTF-8(65001)，只影响显示，不影响任何程序逻辑
	SetConsoleOutputCP(65001);

	//预置图书（实验二题目背景：图书馆借阅）
	vector<Book> books;
	books.push_back(Book("C++ Primer", "Stanley Lippman", "电子工业出版社", "9787121358579", "B001", 128.0, false));
	books.push_back(Book("Effective C++", "Scott Meyers", "电子工业出版社", "9787121155887", "B002", 89.0, false));
	books.push_back(Book("DevOps入门与实践", "DevOps引入指南研究会", "人民邮电出版社", "9787115548551", "B003", 79.0, false));
	books.push_back(Book("深入理解计算机系统", "Randal E. Bryant", "机械工业出版社", "9787111544937", "B004", 139.0, false));
	books.push_back(Book("算法导论", "Thomas H. Cormen", "机械工业出版社", "9787111407010", "B005", 128.0, false));

	//用户（学生借书，实验二依赖关系）
	User user("张三", "20240001", 0, 3);

	int choice = -1;
	while (choice != 0)
	{
		cout << "\n======== 图书馆借阅管理系统 ========" << endl;
		cout << "1. 显示全部图书" << endl;
		cout << "2. 借书" << endl;
		cout << "3. 还书" << endl;
		cout << "4. 我的借阅记录" << endl;
		cout << "5. 我的个人信息" << endl;
		cout << "6. 录入新书" << endl;
		cout << "0. 退出" << endl;
		cout << "请选择: ";
		choice = readInt();
		cin.ignore();   //清掉选择后的换行，保证后续 getline 能读到完整书名

		switch (choice)
		{
		case 1:
			showAllBooks(books);
			break;
		case 2:
			borrowBook(books, user);
			break;
		case 3:
			returnBook(books, user);
			break;
		case 4:
			user.displayRecords();
			break;
		case 5:
			user.display();
			break;
		case 6:
			addBook(books);
			break;
		case 0:
			cout << "谢谢使用，再见！" << endl;
			break;
		default:
			cout << "输入无效，请重新选择。" << endl;
			break;
		}
	}
	return 0;
}
