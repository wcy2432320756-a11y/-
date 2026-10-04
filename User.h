#pragma once
#include <string>
#include "Book.h"
using namespace std;

//借阅记录（第二视频中使用但未展示定义，此处按视频中的使用方式 m_records[i].setBook(book) 补充）
struct BorrowRecord
{
	Book m_book;
	void setBook(Book& book) { m_book = book; }
	Book getBook() { return m_book; }
};

class User
{
private:
	string m_name;
	string m_id;
	int m_borrownum;
	int m_maxnum;
	BorrowRecord m_records[60];

public:
	User();
	User(string name, string id, int borrownum, int maxnum);
	string getName();
	string getId();
	int getBorrownum();
	int getMaxnum();
	void setName(string name);
	void setId(string id);
	void setBorrownum(int borrownum);
	void setMaxnum(int maxnum);
	void display();
	void borrowBook(Book& book);
	void returnBook(Book& book);
	void displayRecords();
};
