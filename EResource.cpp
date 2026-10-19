#include "EResource.h"
#include <iostream>

using std::cout;
using std::endl;

EResource::EResource()  // 实验一：默认构造
    : Resource(), m_format("PDF"), m_sizeMB(0.0)
{
}

EResource::EResource(std::string id, std::string title, std::string provider,
                     double price, std::string url, std::string format,
                     double sizeMB, int count, int total)  // 实验一：重载构造
    : Resource(id, title, provider, price, count, total),
      m_url(url), m_format(format), m_sizeMB(sizeMB)
{
    if (m_format != "PDF" && m_format != "EPUB" && m_format != "MP4")  // 实验一：合法性验证
        m_format = "PDF";
    if (m_sizeMB < 0.0)
        m_sizeMB = 0.0;
}

std::string EResource::getUrl() const    { return m_url; }  // 实验一：基本操作 —— 获取
std::string EResource::getFormat() const { return m_format; }
double      EResource::getSizeMB() const { return m_sizeMB; }

void EResource::setUrl(std::string url)  // 实验一：基本操作 —— 修改
{
    if (!url.empty())
        m_url = url;
}

bool EResource::setFormat(std::string format)
{
    if (format != "PDF" && format != "EPUB" && format != "MP4")
        return false;
    m_format = format;
    return true;
}

bool EResource::setSizeMB(double sizeMB)
{
    if (sizeMB < 0.0)
        return false;
    m_sizeMB = sizeMB;
    return true;
}

void EResource::display() const  // 实验三：按资源类型显示不同 —— 电子资源多出地址、格式、大小
{
    displayCommon();
    cout << "  访问地址: " << m_url << endl;
    cout << "  格式/大小: " << m_format << " / " << m_sizeMB << " MB" << endl;
}

std::string EResource::getTypeName() const  // 实验三：类型名用于区分三种资源
{
    return "电子资源";
}
