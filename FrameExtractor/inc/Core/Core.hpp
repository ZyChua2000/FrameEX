/******************************************************************************/
/*!
\file       LoggerManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 11, 2025
\brief      Declares the Core structure of the engine

 ******************************************************************************/

#ifndef CORE_HPP
#define CORE_HPP
 // Standard Library includes
#include <memory>

namespace FrameExtractor
{
	template <typename ClassObject>
	using Ref = std::shared_ptr<ClassObject>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename ClassObject>
	using Scope = std::unique_ptr<ClassObject>;

	template <typename T, typename... Args>
	Scope<T> MakeScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename ClassObject>
	using WeakRef = std::weak_ptr<ClassObject>;

}

#endif