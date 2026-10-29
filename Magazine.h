#pragma once
#include "Resource.h"

using namespace std;

class Magazine : public Resource
{
private:
    string m_issue;
    int m_month = 1;

public:
    Magazine();
    Magazine(string id, string title, string publisher,
             double price, string issue, int month, int total);

    string getIssue() const;
    int getMonth() const;
    bool setIssue(string issue);
    bool setMonth(int month);

    bool valid() const override;
    void show() const override;
    string getType() const override;
};
