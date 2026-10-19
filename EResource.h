#pragma once
#include <string>
#include "Resource.h"

// 实验三·题目3：图书资源有不同的类型 —— 本类是电子资源
class EResource : public Resource
{
private:
    std::string m_url;     // 实验三：电子资源特有的访问地址
    std::string m_format;  // 实验三：格式，合法性验证 PDF / EPUB / MP4
    double m_sizeMB;       // 实验三：文件大小（MB）

public:
    EResource();  // 实验一：默认构造
    EResource(std::string id, std::string title, std::string provider,
              double price, std::string url, std::string format,
              double sizeMB, int count, int total);  // 实验一：重载构造

    std::string getUrl() const;  // 实验一：基本操作 —— 获取
    std::string getFormat() const;
    double getSizeMB() const;

    void setUrl(std::string url);  // 实验一：基本操作 —— 修改
    bool setFormat(std::string format);
    bool setSizeMB(double sizeMB);

    void display() const override;  // 实验四：派生类重写基类的纯虚函数
    std::string getTypeName() const override;
};
