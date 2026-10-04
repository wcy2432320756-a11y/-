#include <windows.h>
#include <iostream>
#include <vector>      
#include <string>
#include <cstdlib>     
#include "Book.h"
#include "User.h"
using namespace std;
int readInt(string tip)
{
	int value = 0;
	cout << tip;
	if (!(cin >> value))
	{
		cin.clear();
		cin.ignore(1024, '\n');
		cout << "输入格式不正确，请输入数字" << endl;
		return -1;
	}
	cin.ignore(1024, '\n');
	return value;
}


double readDouble(string tip)
{
	string line;
	cout << tip;
	getline(cin, line);
	if (line.length() == 0)
	{
		return 0;
	}
	return atof(line.c_str());
}

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

void showAllBooks(vector<Book>& books)
{
	if (books.empty())
	{
		cout << "书库为空。" << endl;
		return;
	}
	cout << "书库中共有 " << books.size() << " 本图书" << endl;
	for (int i = 0; i < (int)books.size(); i++)
	{
		cout << "【第 " << (i + 1) << " 本】" << endl;
		books[i].display();
		cout << "状态: " << (books[i].getIsBorrowed() ? "已借出" : "可借") << endl;
	}
}

void addBook(vector<Book>& books)
{
	string title, author, publisher, ISBN, id;
	double price;
	cout << "\n===== 录入新书 =====" << endl;
	cout << "请输入书名: ";
	getline(cin, title);
	if (title.length() == 0)
	{
		cout << "录入失败：书名不能为空" << endl;
		return;
	}
	cout << "请输入作者: ";
	getline(cin, author);
	cout << "请输入出版社: ";
	getline(cin, publisher);
	cout << "请输入ISBN: ";
	getline(cin, ISBN);
	cout << "请输入图书编号: ";
	getline(cin, id);
	price = readDouble("请输入价格: ");
	Book b(title, author, publisher, ISBN, id, price, false);
	books.push_back(b);              
	cout << "录入成功，当前藏书 " << books.size() << " 本。" << endl;
}

//借书：把书库里那本书的引用交给 User
void borrowBook(vector<Book>& books, User& user)
{
	if (user.getBorrownum() >= user.getMaxnum())
	{
		cout << "You have borrowed too many books." << endl;
		return;
	}
	string title;
	cout << "\n请输入要借的书名: ";
	getline(cin, title);
	int idx = findBook(books, title);
	if (idx == -1)
	{
		cout << "书库中没有这本书。" << endl;
		return;
	}
	user.borrowBook(books[idx]);      //书的状态改变会写回容器
}

//还书
void returnBook(vector<Book>& books, User& user)
{
	string title;
	cout << "\n请输入要还的书名: ";
	getline(cin, title);
	int idx = findBook(books, title);
	if (idx == -1)
	{
		cout << "书库中没有这本书。" << endl;
		return;
	}
	user.returnBook(books[idx]);
}

//显示菜单
void showMenu()
{
	cout << "\n======== 图书馆借阅管理系统 ========" << endl;
	cout << "1. 显示全部图书" << endl;
	cout << "2. 借书" << endl;
	cout << "3. 还书" << endl;
	cout << "4. 我的借阅记录" << endl;
	cout << "5. 我的个人信息" << endl;
	cout << "6. 录入新书" << endl;
	cout << "0. 退出" << endl;
}

int main()
{
	SetConsoleOutputCP(65001);

	//书库
	vector<Book> books;
	books.push_back(Book("C++ Primer", "Stanley Lippman", "电子工业出版社", "9787121358579", "B001", 128.0, false));
	books.push_back(Book("Effective C++", "Scott Meyers", "电子工业出版社", "9787121155887", "B002", 89.0, false));
	books.push_back(Book("算法导论", "Thomas H. Cormen", "机械工业出版社", "9787111407010", "B003", 128.0, false));

	//借阅人
	User user("张三", "20240001", 0, 3);

	cout << "欢迎使用图书馆借阅管理系统" << endl;
	cout << "当前书库 " << books.size() << " 本图书，借阅人: " << user.getName()
		<< "（最多可借 " << user.getMaxnum() << " 本）" << endl;

	int choice = -1;
	while (choice != 0)
	{
		showMenu();
		choice = readInt("请选择: ");
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
