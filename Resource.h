#pragma once
#include <string>

// 实验三·题目3：图书资源基类，派生出一般图书、杂志、电子资源
// 实验四·题目3：需要在派生类变化的函数声明为虚函数
class Resource
{
private:
    std::string m_id;         // 资源编号，三种资源共有
    std::string m_title;      // 实验一：图书名称
    std::string m_publisher;  // 实验一：出版社信息
    double m_price;           // 实验一：价格
    int m_count;              // 实验二：可借数量（在馆状态），>0 即可借
    int m_total;              // 馆藏总数

public:
    Resource();  // 实验一：默认构造
    Resource(std::string id, std::string title, std::string publisher,
             double price, int count, int total);  // 实验一：重载构造
    virtual ~Resource();                           // 实验四：接口基类必须虚析构

    std::string getId() const;  // 实验一：基本操作 —— 获取
    std::string getTitle() const;
    std::string getPublisher() const;
    double getPrice() const;
    int getCount() const;
    int getTotal() const;
    bool isAvailable() const;  // 实验二：还有可借数量即可借

    void setTitle(std::string title);  // 实验一：基本操作 —— 修改
    void setPublisher(std::string publisher);
    bool setPrice(double price);
    bool setTotal(int total);

    bool addCopies(int n);     // 实验三：增加已有资源的数量
    bool reduceCopies(int n);  // 实验三：减少已有资源的数量
    bool borrowOne();          // 实验二：借出，可借数量减一
    bool returnOne();          // 实验二：归还，可借数量加一

    virtual void display() const = 0;             // 实验四：纯虚；实验三：按类型显示不同信息
    virtual std::string getTypeName() const = 0;  // 三种资源的类型名

protected:
    void displayCommon() const;  // 三种资源共有字段的输出，供派生类 display() 复用
};
