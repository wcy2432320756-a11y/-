#ifndef READER_H
#define READER_H
#include <string>
#include <vector>
class Reader {
public:
    std::string id, name;
    std::vector<std::string> borrowedIsbns;
    Reader();
    Reader(std::string i, std::string n);
};
#endif