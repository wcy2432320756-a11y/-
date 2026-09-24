#include "Book.h"
Book::Book() : total(0), available(0) {}

Book::Book(std::string i, std::string t, std::string a, int tot, int av) :
	isbn(i),title(t),author(a),total(tot),available(av){ }