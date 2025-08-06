/******************************************************************************
/*!
\file       AssetHandle.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset Handle class

 /******************************************************************************/

#ifndef ASSET_HANDLE_HPP
#define ASSET_HANDLE_HPP
#include <cstdint>

namespace FrameExtractor
{
	class AssetHandle
	{
	public:
		AssetHandle();
		AssetHandle(const AssetHandle&) = default;
		AssetHandle(uint64_t id) : mID(id) {}
		operator uint64_t() const { return mID; }
	private:
		uint64_t mID;
	};
}

namespace std
{
	/*!***********************************************************************
		\brief
			Hash function for UUID
		\param[in] id
			The UUID to be hashed
	*************************************************************************/
	template <typename T> struct hash;
	template <>
	struct hash<FrameExtractor::AssetHandle>
	{
		std::size_t operator()(const FrameExtractor::AssetHandle& id) const
		{
			return static_cast<uint64_t>(id);
		}
	}; // struct hash
} // namespace std
#endif