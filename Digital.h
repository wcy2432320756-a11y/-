#pragma once
#include "Resource.h"

using namespace std;

// 电子资源：文档、视频等。
class Digital : public Resource
{
private:
    string m_url;
    string m_format = "PDF";
    double m_sizeMB = 0.0;

public:
    Digital();
    Digital(string id, string title, string provider,
            double price, string url, string format,
            double sizeMB, int total);

    string getUrl() const;
    string getFormat() const;
    double getSizeMB() const;
    bool setUrl(string url);
    bool setFormat(string format);
    bool setSizeMB(double sizeMB);

    bool valid() const override;
    void show() const override;
    string getType() const override;
};
