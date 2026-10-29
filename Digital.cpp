#include "Digital.h"
#include <cmath>
#include <iostream>

using namespace std;

Digital::Digital() = default;

Digital::Digital(string id, string title, string provider,
                 double price, string url, string format,
                 double sizeMB, int total)
    : Resource(id, title, provider, price, total),
      m_url(url), m_format(format), m_sizeMB(sizeMB)
{
}

bool Digital::valid() const
{
    return Resource::valid() && !m_url.empty()
        && (m_format == "PDF" || m_format == "EPUB" || m_format == "MP4")
        && isfinite(m_sizeMB) && m_sizeMB >= 0.0;
}

string Digital::getUrl() const { return m_url; }
string Digital::getFormat() const { return m_format; }
double Digital::getSizeMB() const { return m_sizeMB; }

bool Digital::setUrl(string url)
{
    if (url.empty())
        return false;
    m_url = url;
    return true;
}

bool Digital::setFormat(string format)
{
    if (format != "PDF" && format != "EPUB" && format != "MP4")
        return false;
    m_format = format;
    return true;
}

bool Digital::setSizeMB(double sizeMB)
{
    if (!isfinite(sizeMB) || sizeMB < 0.0)
        return false;
    m_sizeMB = sizeMB;
    return true;
}

void Digital::show() const
{
    showCommon();
    cout << "  访问地址: " << m_url << '\n'
         << "  格式/大小: " << m_format << " / " << m_sizeMB << " MB\n";
}

string Digital::getType() const { return "电子资源"; }
