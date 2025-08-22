/******************************************************************************/
/*!
\file       DataSpecification.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Declares the Data Specifications that specifies what the data entails
 ******************************************************************************/
#ifndef Data_Specification_HPP
#define Data_Specification_HPP
 // Standard Library includes
#include <string>
#include <chrono>

// Project includes
#include <Core/Core.hpp>
#include <Template/TemplateEnums.hpp>
namespace rttr
{
	class variant;
}
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Structure that holds the specification of data, including its name,
			type, and default value.
	*************************************************************************/
	struct DataSpecification
	{
	public:
		Scope<std::string> mName = nullptr;
		Scope<DataType> mType = nullptr;
		Scope<rttr::variant> mDefault = nullptr;
	public:
		/*!***********************************************************************
			\brief
				Default constructor that initializes the data specification.
		*************************************************************************/
		DataSpecification() = default;

		/*!***********************************************************************
			\brief
				Copy constructor.
			\param[in] other
				The DataSpecification object to copy from.
			\details
				Creates a new DataSpecification object as a copy of the given object.
				Performs a deep copy of the internal data if necessary.
		*************************************************************************/
		DataSpecification(const DataSpecification& other);

		/*!***********************************************************************
			  \brief
				  Move constructor.
			  \param[in,out] other
				  The DataSpecification object to move from. After the move, the state of
				  the source object is unspecified but valid.
			  \details
				  Transfers ownership of resources from the source object to the new
				  object without performing deep copies. This constructor is noexcept.
		*************************************************************************/
		DataSpecification(DataSpecification&& other) noexcept = default;

		/*!***********************************************************************
			\brief
				Destructor.
			\details
				Cleans up any resources held by the DataSpecification object.
		*************************************************************************/
		~DataSpecification() = default;

		/*!***********************************************************************
			\brief
				Move assignment operator.
			\param[in,out] other
				The DataSpecification object to move from. After the move, the state of
				the source object is unspecified but valid.
			\return
				Reference to this object after the move assignment.
			\details
				Transfers ownership of resources from the source object to this object
				without performing deep copies. This operator is noexcept.
		*************************************************************************/
		DataSpecification& operator=(DataSpecification&& other) noexcept = default;

		/*!***********************************************************************
			\brief
				Copy assignment operator.
			\param[in] other
				The DataSpecification object to copy from.
			\return
				Reference to this object after the copy assignment.
			\details
				Performs a deep copy of the data from the source object to this object.
		*************************************************************************/
		DataSpecification& operator=(const DataSpecification& other);
	};

	struct AdditionalSpecification
	{
		Ref<std::string> mName = nullptr;
		Scope<DataType> mType = nullptr;
		Scope<std::vector<Ref<DataSpecification>>> mData = nullptr;

		/*!***********************************************************************
			\brief
				Default constructor that initializes the additional specification.
		*************************************************************************/
		AdditionalSpecification() = default;

		/*!***********************************************************************
			\brief
				Copy constructor.
			\param[in] other
				The AdditionalSpecification object to copy from.
			\details
				Creates a new AdditionalSpecification object as a copy of the given object.
				Performs a deep copy of the internal data if necessary.
		*************************************************************************/
		AdditionalSpecification(const AdditionalSpecification& other);

		/*!***********************************************************************
			\brief
				Move constructor.
			\param[in,out] other
				The AdditionalSpecification object to move from. After the move, the state of
				the source object is unspecified but valid.
			\details
				Transfers ownership of resources from the source object to the new
				object without performing deep copies. This constructor is noexcept.
		*************************************************************************/
		AdditionalSpecification(AdditionalSpecification&& other) noexcept = default;

		/*!***********************************************************************
			\brief
				Destructor.
			\details
				Cleans up any resources held by the AdditionalSpecification object.
		*************************************************************************/
		~AdditionalSpecification() = default;

		/*!***********************************************************************
			\brief
				Move assignment operator.
			\param[in,out] other
				The AdditionalSpecification object to move from. After the move, the state of
				the source object is unspecified but valid.
			\return
				Reference to this object after the move assignment.
			\details
				Transfers ownership of resources from the source object to this object
				without performing deep copies. This operator is noexcept.
		*************************************************************************/
		AdditionalSpecification& operator=(AdditionalSpecification&& other) noexcept = default;

		/*!***********************************************************************
			\brief
				Copy assignment operator.
			\param[in] other
				The AdditionalSpecification object to copy from.
			\return
				Reference to this object after the copy assignment.
			\details
				Performs a deep copy of the data from the source object to this object.
		*************************************************************************/
		AdditionalSpecification& operator=(const AdditionalSpecification& other);
	};


	/*!***********************************************************************
		\brief
			Constructs a default value based on the specified data type.
		\param[in] type
			The data type for which to construct the default value.
		\param[in] DefaultData
			A pointer to the default data to be used for construction.
		\return
			A pointer to the constructed default value.
	*************************************************************************/
	void* ConstructDefaultByType(DataType type, void* DefaultData);
}

#endif