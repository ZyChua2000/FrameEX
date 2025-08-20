/******************************************************************************/
/*!
\file       AssetHandle.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset Handle class

 ******************************************************************************/

#ifndef ASSET_HANDLE_HPP
#define ASSET_HANDLE_HPP

 // Standard Library includes
#include <cstdint>

namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents a handle to an asset.
	*************************************************************************/
	class AssetHandle
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor that initializes the handle with a random ID.
		*************************************************************************/
		AssetHandle();

		/*!***********************************************************************
			\brief
				Copy constructor that initializes the handle with an existing ID.
			\param[in] id
				The ID to initialize the handle with.
		*************************************************************************/
		AssetHandle(const AssetHandle&) = default;

		/*!***********************************************************************
			\brief
				Constructor that initializes the handle with a specific ID.
			\param[in] id
				The ID to initialize the handle with.
		*************************************************************************/
		AssetHandle(uint64_t id) : mID(id) {}

		/*!***********************************************************************
			\brief
				Assignment operator that assigns an existing handle to this handle.
			\param[in] other
				The handle to assign.
			\return
				A reference to this handle.
		*************************************************************************/
		operator uint64_t() const { return mID; }
	private:
		uint64_t mID; //<- Unique identifier for the asset handle
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