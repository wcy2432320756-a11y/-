#ifndef BOOK_H
#define BOOK_H
#include<string>
class book {
public:
	std::string isbn, title, author;
	int total, available;
	book() :total(0), available(0) {}
	book(std::string i, std::string t, std::string a, int tot, int av);

};
#endif











