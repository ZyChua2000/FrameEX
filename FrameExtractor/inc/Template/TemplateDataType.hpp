/******************************************************************************
/*!
\file       TemplateDataType.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 3, 2024
\brief      Declares the Data Types required for templating purposes
 /******************************************************************************/
#include <string>
#include <chrono>
#ifndef Template_Data_Type_HPP
#define Template_Data_Type_HPP

namespace FrameExtractor
{
    struct Date
    {
    public:
        unsigned short year;
        unsigned char month;
        unsigned char day;
    public:
        Date() = default;
        Date(int d, int m, int y) : year(y), month(m), day(d) {}
        Date(std::chrono::year_month_day ymd)
            : year(static_cast<int>(ymd.year())),
            month(static_cast<unsigned>(unsigned{ ymd.month() })),
            day(static_cast<unsigned>(unsigned{ ymd.day() }))
        {
        }

        std::chrono::year_month_day to_chrono() const;
        std::string to_string() const;
    };

    struct Time
    {
        unsigned char hours;
        unsigned char minutes;
        unsigned char seconds;

        Time() = default;
        Time(int h, int m, int s);
        Time(int s);
        Time(std::chrono::hh_mm_ss<std::chrono::seconds> time)
            : hours(time.hours().count()),
            minutes(time.minutes().count()),
            seconds(time.seconds().count())
        {
        }

        std::chrono::hh_mm_ss<std::chrono::seconds> to_chrono() const;

        std::string to_string() const;
    };
}
#endif