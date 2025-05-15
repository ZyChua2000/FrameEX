/******************************************************************************
/*!
\file       LoggerManager.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Declares the Core structure of the engine

 /******************************************************************************/

#ifndef CORE_HPP
#define CORE_HPP
#include <memory>

namespace FrameExtractor
{
	template <typename ClassObject>
	using Ref = std::shared_ptr<ClassObject>;

	template <typename T, typename... Args>
	Ref<T> MakeRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename ClassObject>
	using Scope = std::unique_ptr<ClassObject>;

	template <typename T, typename... Args>
	Scope<T> MakeScope(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename ClassObject>
	using WeakRef = std::weak_ptr<ClassObject>;

	template <typename classObject>
	class RefDraft
	{
	public:
		RefDraft(classObject* pointer) : rawPointer(pointer), refCount(1) {}
		RefDraft(const RefDraft& other) : rawPointer(other.rawPointer), refCount(other.refCount) { refCount++; }
		RefDraft& operator=(const RefDraft& other) { rawPointer = other.rawPointer; refCount = other.refCount; refCount++; return *this; }
		~RefDraft() { refCount--; if (refCount == 0) delete rawPointer; }
		classObject* operator->() { return rawPointer; }
		classObject& operator*() { return *rawPointer; }
	private:
		classObject* rawPointer;
		unsigned int refCount;
	};
}


#endif

