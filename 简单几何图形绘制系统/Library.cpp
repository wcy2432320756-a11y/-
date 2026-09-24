#include "Library.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

// 安全地把字符串转成整数，转换失败时返回 def
static int toInt(const string& s, int def = 0) {
    try {
        return stoi(s);
    } catch (...) {
        return def;
    }
}

// ==================== 私有成员函数 ====================

int Library::findBookIndex(const string& isbn) const {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].isbn == isbn) return static_cast<int>(i);
    }
    return -1;
}

int Library::findReaderIndex(const string& id) const {
    for (size_t i = 0; i < readers.size(); ++i) {
        if (readers[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

// books.txt   每行：isbn|title|author|total|available
// readers.txt 每行：id|name|isbn1,isbn2,...
void Library::load() {
    books.clear();
    readers.clear();

    ifstream fin(bookFile.c_str());
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string isbn, title, author, total, available;
        getline(ss, isbn, '|');
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, total, '|');
        getline(ss, available, '|');
        if (isbn.empty()) continue;
        books.push_back(Book(isbn, title, author, toInt(total), toInt(available)));
    }
    fin.close();

    ifstream fr(readerFile.c_str());
    while (getline(fr, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, isbns;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, isbns);
        if (id.empty()) continue;

        Reader r(id, name);
        stringstream sl(isbns);
        string one;
        while (getline(sl, one, ',')) {
            if (!one.empty()) r.borrowedIsbns.push_back(one);
        }
        readers.push_back(r);
    }
    fr.close();
}

void Library::save() const {
    ofstream fout(bookFile.c_str());
    for (size_t i = 0; i < books.size(); ++i) {
        const Book& b = books[i];
        fout << b.isbn << '|' << b.title << '|' << b.author << '|'
             << b.total << '|' << b.available << '\n';
    }
    fout.close();

    ofstream fr(readerFile.c_str());
    for (size_t i = 0; i < readers.size(); ++i) {
        const Reader& r = readers[i];
        fr << r.id << '|' << r.name << '|';
        for (size_t j = 0; j < r.borrowedIsbns.size(); ++j) {
            if (j > 0) fr << ',';
            fr << r.borrowedIsbns[j];
        }
        fr << '\n';
    }
    fr.close();
}

// ==================== 构造 / 析构 ====================

Library::Library() { load(); }
Library::~Library() { save(); }

// ==================== 图书相关 ====================

void Library::addBook() {
    string isbn, title, author;
    int total = 0;

    cout << "请输入 ISBN：";
    cin >> isbn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "请输入书名：";
    getline(cin, title);

    cout << "请输入作者：";
    getline(cin, author);

    cout << "请输入总册数：";
    while (!(cin >> total) || total < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "输入无效，请重新输入总册数：";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int idx = findBookIndex(isbn);
    if (idx >= 0) {
        books[idx].total += total;
        books[idx].available += total;
        cout << "该书已存在，已累加库存。\n";
    } else {
        books.push_back(Book(isbn, title, author, total, total));
        cout << "添加图书成功。\n";
    }
}

void Library::showBooks() const {
    if (books.empty()) {
        cout << "暂无图书。\n";
        return;
    }
    cout << left << setw(16) << "ISBN" << setw(24) << "书名" << setw(16) << "作者"
         << setw(8) << "总数" << setw(8) << "可借" << '\n';
    cout << string(72, '-') << '\n';
    for (size_t i = 0; i < books.size(); ++i) {
        const Book& b = books[i];
        cout << left << setw(16) << b.isbn << setw(24) << b.title << setw(16) << b.author
             << setw(8) << b.total << setw(8) << b.available << '\n';
    }
}

// ==================== 读者相关 ====================

void Library::addReader() {
    string id, name;

    cout << "请输入读者 ID：";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (findReaderIndex(id) >= 0) {
        cout << "该读者已存在。\n";
        return;
    }

    cout << "请输入读者姓名：";
    getline(cin, name);

    readers.push_back(Reader(id, name));
    cout << "添加读者成功。\n";
}

void Library::showReaders() const {
    if (readers.empty()) {
        cout << "暂无读者。\n";
        return;
    }
    for (size_t i = 0; i < readers.size(); ++i) {
        const Reader& r = readers[i];
        cout << "ID：" << r.id << "  姓名：" << r.name
             << "  已借 " << r.borrowedIsbns.size() << " 本";
        if (!r.borrowedIsbns.empty()) {
            cout << "（";
            for (size_t j = 0; j < r.borrowedIsbns.size(); ++j) {
                if (j > 0) cout << ", ";
                cout << r.borrowedIsbns[j];
            }
            cout << "）";
        }
        cout << '\n';
    }
}

// ==================== 借书 / 还书 ====================

void Library::borrowBook() {
    string id, isbn;
    cout << "请输入读者 ID：";
    cin >> id;
    cout << "请输入 ISBN：";
    cin >> isbn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int ri = findReaderIndex(id);
    if (ri < 0) {
        cout << "读者不存在。\n";
        return;
    }
    int bi = findBookIndex(isbn);
    if (bi < 0) {
        cout << "图书不存在。\n";
        return;
    }
    if (books[bi].available <= 0) {
        cout << "该书已全部借出。\n";
        return;
    }

    vector<string>& list = readers[ri].borrowedIsbns;
    if (find(list.begin(), list.end(), isbn) != list.end()) {
        cout << "该读者已借过这本书。\n";
        return;
    }

    list.push_back(isbn);
    books[bi].available--;
    cout << "借阅成功。\n";
}

void Library::returnBook() {
    string id, isbn;
    cout << "请输入读者 ID：";
    cin >> id;
    cout << "请输入 ISBN：";
    cin >> isbn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int ri = findReaderIndex(id);
    if (ri < 0) {
        cout << "读者不存在。\n";
        return;
    }
    int bi = findBookIndex(isbn);
    if (bi < 0) {
        cout << "图书不存在。\n";
        return;
    }

    vector<string>& list = readers[ri].borrowedIsbns;
    vector<string>::iterator it = find(list.begin(), list.end(), isbn);
    if (it == list.end()) {
        cout << "该读者没有借这本书。\n";
        return;
    }

    list.erase(it);
    if (books[bi].available < books[bi].total) books[bi].available++;
    cout << "归还成功。\n";
}

// ==================== 查询 ====================

void Library::searchBook() const {
    string key;
    cout << "请输入 ISBN 或书名关键字：";
    cin >> key;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool found = false;
    for (size_t i = 0; i < books.size(); ++i) {
        const Book& b = books[i];
        if (b.isbn == key || b.title.find(key) != string::npos) {
            cout << "ISBN：" << b.isbn << "  书名：" << b.title
                 << "  作者：" << b.author
                 << "  总数：" << b.total << "  可借：" << b.available << '\n';
            found = true;
        }
    }
    if (!found) cout << "没有找到匹配的图书。\n";
}

// ==================== 菜单 ====================

void Library::menu() {
    int choice = -1;
    while (choice != 0) {
        cout << "\n========== 图书管理系统 ==========\n"
             << "1. 添加图书\n"
             << "2. 显示所有图书\n"
             << "3. 添加读者\n"
             << "4. 显示所有读者\n"
             << "5. 借书\n"
             << "6. 还书\n"
             << "7. 查询图书\n"
             << "0. 退出\n"
             << "请选择：";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "输入无效，请重新选择。\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: addBook();     break;
            case 2: showBooks();   break;
            case 3: addReader();   break;
            case 4: showReaders(); break;
            case 5: borrowBook();  break;
            case 6: returnBook();  break;
            case 7: searchBook();  break;
            case 0: cout << "已退出，数据已保存。\n"; break;
            default: cout << "无效选项，请重新选择。\n"; break;
        }
    }
}
