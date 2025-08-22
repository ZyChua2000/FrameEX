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
#include <rttr/variant.h>
namespace FrameExtractor
{
	Scope<rttr::variant> ConstructDefaultByType(DataType type, rttr::variant* DefaultData)
	{
		switch (type)
		{
		case DataType::Bool:   return MakeScope<rttr::variant>(*reinterpret_cast<bool*>(DefaultData));
		case DataType::Int:    return MakeScope<rttr::variant>(*reinterpret_cast<int*>(DefaultData));
		case DataType::Float:  return MakeScope<rttr::variant>(*reinterpret_cast<float*>(DefaultData));
		case DataType::Double: return MakeScope<rttr::variant>(*reinterpret_cast<double*>(DefaultData));
		case DataType::String: return MakeScope<rttr::variant>(*reinterpret_cast<std::string*>(DefaultData));
		case DataType::Date:   return MakeScope<rttr::variant>(*reinterpret_cast<Date*>(DefaultData));
		case DataType::Time:   return MakeScope<rttr::variant>(*reinterpret_cast<Time*>(DefaultData));
		default:               return nullptr;
		}
	}

	DataSpecification::DataSpecification(const DataSpecification& other)
	{
		if (other.mName)
			mName = MakeScope<std::string>(*other.mName);

		if (other.mType)
		{
			mType = MakeScope<DataType>(*other.mType);

			if (other.mDefault)
				mDefault = ConstructDefaultByType(*other.mType, other.mDefault.get());
		}
	}

	DataSpecification& DataSpecification::operator=(const DataSpecification& other)
	{
		if (this != &other)
		{
			mName = other.mName ? MakeScope<std::string>(*other.mName) : nullptr;
			mType = other.mType ? MakeScope<DataType>(*other.mType) : nullptr;
			mDefault = other.mDefault ? ConstructDefaultByType(*other.mType, other.mDefault.get()) : nullptr;
		}
		return *this;
	}

	AdditionalSpecification::AdditionalSpecification(const AdditionalSpecification& other)
	{
		if (other.mName)
			mName = MakeRef<std::string>(*other.mName);

		if (other.mType)
			mType = MakeScope<DataType>(*other.mType);

		if (other.mData)
		{
			mData = MakeScope<std::vector<Ref<DataSpecification>>>();
			for (const auto& dataSpec : *other.mData)
			{
				// Deep copy each DataSpecification
				mData->push_back(dataSpec ? MakeRef<DataSpecification>(*dataSpec) : nullptr);
			}
		}
	}

	AdditionalSpecification& AdditionalSpecification::operator=(const AdditionalSpecification& other)
	{
		if (this != &other)
		{
			// Copy mName and mType
			mName = other.mName ? MakeRef<std::string>(*other.mName) : nullptr;
			mType = other.mType ? MakeScope<DataType>(*other.mType) : nullptr;

			// Deep copy mData
			if (other.mData)
			{
				mData = MakeScope<std::vector<Ref<DataSpecification>>>();
				for (const auto& dataSpec : *other.mData)
				{
					mData->push_back(dataSpec ? MakeRef<DataSpecification>(*dataSpec) : nullptr);
				}
			}
			else
			{
				mData.reset(); // clear if other.mData is null
			}
		}
		return *this;
	}
}