/******************************************************************************/
/*!
\file       DataSpecifications.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Defines the Data Specifications that specifies what the data entails
 ******************************************************************************/

#include <FrameExtractorPCH.hpp>

 // Project includes
#include <Template/DataSpecification.hpp>
#include <Template/TemplateDataType.hpp>

namespace FrameExtractor
{
	void* ConstructDefaultByType(DataType type, void* DefaultData)
	{
		switch (type)
		{
		case DataType::Bool: return new bool(*reinterpret_cast<bool*>(DefaultData));
		case DataType::Int: return new int(*reinterpret_cast<int*>(DefaultData));
		case DataType::Float: return new float(*reinterpret_cast<float*>(DefaultData));
		case DataType::Double: return new double(*reinterpret_cast<double*>(DefaultData));
		case DataType::String: return new std::string(*reinterpret_cast<std::string*>(DefaultData));
		case DataType::Date: return new Date(*reinterpret_cast<Date*>(DefaultData));
		case DataType::Time: return new Time(*reinterpret_cast<Time*>(DefaultData));
		default: return nullptr;
		}
	}

	DataSpecification::DataSpecification(const DataSpecification& other)
	{
		if (other.mName)
			mName = new std::string(*other.mName);
		if (other.mType)
		{
			mType = new DataType(*other.mType);
			if (other.mDefault)
			{
				mDefault = ConstructDefaultByType(*other.mType, other.mDefault);
			}
		}
	}
	DataSpecification::DataSpecification(DataSpecification&& other) noexcept
	{
		mName = other.mName;
		mType = other.mType;
		mDefault = other.mDefault;

		other.mName = nullptr;
		other.mType = nullptr;
		other.mDefault = nullptr;
	}
	DataSpecification::~DataSpecification()
	{
		if (mName) delete mName;
		if (mType) delete mType;
		if (mDefault) delete mDefault;
	}
	DataSpecification& DataSpecification::operator=(DataSpecification&& other) noexcept
	{
		if (this == &other) return *this;

		// Clean up current
		if (mName)
			delete mName;
		if (mType)
			delete mType;
		if (mDefault)
			delete mDefault;

		mName = other.mName;
		mType = other.mType;
		mDefault = other.mDefault;

		other.mName = nullptr;
		other.mType = nullptr;
		other.mDefault = nullptr;

		return *this;
	}
	DataSpecification& DataSpecification::operator=(const DataSpecification& other)
	{
		if (this == &other) return *this;

		// Clean up current
		if (mName)
			delete mName;
		if (mType)
			delete mType;
		if (mDefault)
			delete mDefault;

		mName = other.mName ? new std::string(*other.mName) : nullptr;
		mType = other.mType ? new DataType(*other.mType) : nullptr;
		if (other.mType && other.mDefault)
		{
			mDefault = ConstructDefaultByType(*other.mType, other.mDefault);
		}
		return *this;
	}
}