/******************************************************************************/
/*!
\file       TemplateEnums.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       June 23, 2025
\brief      Declares the Enums involved for Templating System

 ******************************************************************************/

#ifndef TEMPLATE_ENUMS_HPP
#define TEMPLATE_ENUMS_HPP

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Enumeration for different data types used in the templating system.
	*************************************************************************/
	enum class DataType
	{
		None,
		Bool,
		Int,
		Float,
		Double,
		Date,
		Time,
		String,
		TextBox,
		Singular,
		Vector,
		Complex
	};

	/*!***********************************************************************
		\brief
			Enumeration for different input/output types used in the templating system.
	*************************************************************************/
	enum class IOType
	{
		None,
		Excel,
		Text
	};
}

#endif