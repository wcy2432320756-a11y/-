#pragma once
#include <string>

using namespace std;

// 实验三、四：资源基类，统一保存公共属性，通过虚函数显示各类资源。
class Resource
{
private:
    string m_id;
    string m_title;
    string m_publisher;
    double m_price = 0.0;
    int m_left = 0;   // 可借数量
    int m_total = 0;  // 馆藏总数

public:
    // 默认构造生成空对象；带参构造保存参数，入馆前用 valid() 检查。
    Resource();
    Resource(string id, string title, string publisher,
             double price, int total);
    virtual ~Resource() = default;

    string getId() const;
    string getTitle() const;
    string getPublisher() const;
    double getPrice() const;
    int getLeft() const;
    int getTotal() const;
    virtual bool valid() const;
    bool isAvailable() const;

    // 修改失败统一返回 false，保留原值。
    bool setTitle(string title);
    bool setPublisher(string publisher);
    bool setPrice(double price);
    bool setTotal(int total);

    bool addCopies(int count);
    bool reduceCopies(int count);
    bool borrowOne();
    bool returnOne();

    virtual void show() const = 0;
    virtual string getType() const = 0;

protected:
    void showCommon() const;
};
