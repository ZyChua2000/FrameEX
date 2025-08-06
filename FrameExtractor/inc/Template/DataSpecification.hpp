/******************************************************************************
/*!
\file       DataSpecification.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Declares the Data Specifications that specifies what the data entails
 /******************************************************************************/
#ifndef Data_Specification_HPP
#define Data_Specification_HPP
#include <string>
#include <Template/TemplateEnums.hpp>
#include <chrono>
namespace FrameExtractor
{
	struct DataSpecification
	{
	public:
		std::string* mName = nullptr;
		DataType* mType = nullptr;
		void* mDefault = nullptr;
	public:
		DataSpecification() = default;
		DataSpecification(const DataSpecification& other);
		DataSpecification(DataSpecification&& other) noexcept;
		~DataSpecification();
		DataSpecification& operator=(DataSpecification&& other) noexcept;
		DataSpecification& operator=(const DataSpecification& other);
	};
	void* ConstructDefaultByType(DataType type, void* DefaultData);
}

#endif