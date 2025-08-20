/******************************************************************************/
/*!
\file       TemplateDataType.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 3, 2025
\brief      Declares the Data Types required for templating purposes
 ******************************************************************************/

#ifndef Template_Data_Type_HPP
#define Template_Data_Type_HPP
 // Standard Library includes
#include <string>
#include <chrono>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Structure that holds date information.
	*************************************************************************/
	struct Date
	{
	public:
		unsigned short year;
		unsigned char month;
		unsigned char day;
	public:
		Date() = default;

		/*!***********************************************************************
			\brief
				Constructor that initializes the date with specific values.
			\param[in] d
				The day of the month.
			\param[in] m
				The month of the year.
			\param[in] y
				The year.
		*************************************************************************/
		Date(int d, int m, int y) : year(y), month(m), day(d) {}

		/*!***********************************************************************
			\brief
				Constructor that initializes the date from a std::chrono::year_month_day object.
			\param[in] ymd
				The year_month_day object to initialize the date from.
		*************************************************************************/
		Date(std::chrono::year_month_day ymd)
			: year(static_cast<int>(ymd.year())),
			month(static_cast<unsigned>(unsigned{ ymd.month() })),
			day(static_cast<unsigned>(unsigned{ ymd.day() }))
		{}

		/*!***********************************************************************
			\brief
				Converts the Date object to a std::chrono::year_month_day object.
			\return
				The corresponding std::chrono::year_month_day object.
		*************************************************************************/
		std::chrono::year_month_day to_chrono() const;

		/*!***********************************************************************
			\brief
				Converts the Date object to a string representation.
			\return
				A string in the format "DD/MM/YYYY".
		*************************************************************************/
		std::string to_string() const;
	};


	/*!***********************************************************************
		\brief
			Structure that holds time information.
	*************************************************************************/
	struct Time
	{
		unsigned char hours;
		unsigned char minutes;
		unsigned char seconds;

		Time() = default;
		/*!***********************************************************************
			\brief
				Constructor that initializes the time with specific values.
			\param[in] h
				The hour of the day (0-23).
			\param[in] m
				The minute of the hour (0-59).
			\param[in] s
				The second of the minute (0-59).
		*************************************************************************/
		Time(int h, int m, int s);

		/*!***********************************************************************
			\brief
				Constructor that initializes the time with a total number of seconds.
			\param[in] s
				The total number of seconds since midnight.
			\details
				This constructor calculates the hours, minutes, and seconds from the total seconds.
		*************************************************************************/
		Time(int s);

		/*!***********************************************************************
			\brief
				Constructor that initializes the time from a std::chrono::hh_mm_ss object.
			\param[in] time
				The hh_mm_ss object to initialize the time from.
		*************************************************************************/
		Time(std::chrono::hh_mm_ss<std::chrono::seconds> time)
			: hours(static_cast<unsigned char>(time.hours().count())),
			minutes(static_cast<unsigned char>(time.minutes().count())),
			seconds(static_cast<unsigned char>(time.seconds().count()))
		{}


		/*!***********************************************************************
			\brief
				Converts the Time object to a std::chrono::hh_mm_ss object.
			\return
				The corresponding std::chrono::hh_mm_ss object.
		*************************************************************************/
		std::chrono::hh_mm_ss<std::chrono::seconds> to_chrono() const;


		/*!***********************************************************************
			\brief
				Converts the Time object to a string representation.
			\return
				A string in the format "HH:MM:SS".
		*************************************************************************/
		std::string to_string() const;
	};
}
#endif