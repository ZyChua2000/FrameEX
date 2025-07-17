/******************************************************************************
/*!
\file       TemplateDataType.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 3, 2024
\brief      Defines the Data Types required for templating purposes
 /******************************************************************************/
#include <FrameExtractorPCH.hpp>
#include <Template/TemplateDataType.hpp>
#include <rttr/registration>

using namespace rttr;

namespace FrameExtractor
{
    RTTR_REGISTRATION
    {
        rttr::registration::class_<std::string>("string");
        registration::class_<Date>("Date")
            .property("year", &Date::year)
            .property("month", &Date::month)
            .property("day", &Date::day)
            .constructor<int, int, int>()
            .constructor<std::chrono::year_month_day>()
            .constructor<>()
            .method("to_chrono", &Date::to_chrono)
            .method("to_string", &Date::to_string);


        registration::class_<Time>("Time")
            .property("hours", &Time::hours)
            .property("minutes", &Time::minutes)
            .property("seconds", &Time::seconds)
            .constructor<int, int, int>()
            .constructor<std::chrono::hh_mm_ss<std::chrono::seconds>>()
            .constructor<>()
            .method("to_chrono", &Time::to_chrono)
            .method("to_string", &Time::to_string);


    }
    std::chrono::year_month_day Date::to_chrono() const
    {
        return std::chrono::year{ year } /
            std::chrono::month{ month } /
            std::chrono::day{ day };
    }

    std::string Date::to_string() const
    {
        // dd/mm/yyyy
        return ((day < 10) ? "0" : "") + std::to_string(day) + "/" +
            ((month < 10) ? "0" : "") + std::to_string(month) + "/" +
            std::to_string(year);
    }

    Time::Time(int h, int m, int s) : hours(h), minutes(m), seconds(s)
    {
        if (seconds >= 60)
        {
            minutes += s / 60;
            seconds = s % 60;
        }
        if (minutes >= 60)
        {
            hours += m / 60;
            minutes = m % 60;
        }
        if (hours >= 24)
        {
            hours = hours % 24; // wrap around if hours exceed 24
        }
    }

    Time::Time(int s)
    {
        hours = s / 3600;
        s %= 3600;
        minutes = s / 60;
        seconds = s % 60;

        if (hours >= 24)
        {
            hours = hours % 24; // wrap around if hours exceed 24
        }
    }

    std::chrono::hh_mm_ss<std::chrono::seconds> Time::to_chrono() const
    {
        return std::chrono::hh_mm_ss<std::chrono::seconds>(std::chrono::seconds(hours * 3600 + minutes * 60 + seconds));
    }

    std::string Time::to_string() const
    {
        // xx:xx:xx
        return ((hours < 10) ? "0" : "") + std::to_string(hours) + ":" +
            ((minutes < 10) ? "0" : "") + std::to_string(minutes) + ":" +
            ((seconds < 10) ? "0" : "") + std::to_string(seconds);
    }
}