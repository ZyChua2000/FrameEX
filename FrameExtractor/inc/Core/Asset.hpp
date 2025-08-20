/******************************************************************************/
/*!
\file       Asset.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset class

 ******************************************************************************/

#ifndef Asset_HPP
#define Asset_HPP

 // Standard Library includes
#include <filesystem>
namespace FrameExtractor
{
	/*!***************************************************************
		\brief
			Enumeration for different types of assets
	*****************************************************************/
	enum class AssetType
	{
		Texture,
		Video,
		Unknown
	};

	/*!***************************************************************
		\brief
			Class that represents a generic asset in the system.
	*****************************************************************/
	class Asset
	{
	public:
		/*!***************************************************************
			\brief
				Virtual destructor for Asset class.
		*****************************************************************/
		virtual ~Asset() = default;

		/*!***************************************************************
			\brief
				Pure virtual function to unload the asset.
		*****************************************************************/
		virtual void Unload() = 0;

		/*!***************************************************************
			\brief
				Pure virtual function to check if the asset is loaded.
		*****************************************************************/
		virtual bool IsLoaded() const = 0;

		/*!***************************************************************
			\brief
				Pure virtual function to load the asset.
		*****************************************************************/
		virtual void Load() = 0;

		/*!***************************************************************
			\brief
				Pure virtual function to load the asset from a specific path.
			\param[in] path
				The path to load the asset from.
		*****************************************************************/
		virtual void Load(const std::filesystem::path& path) = 0;

		/*!***************************************************************
			\brief
				Pure virtual function to get the asset type.
			\return
				The type of the asset.
		*****************************************************************/
		virtual AssetType GetAssetType() const = 0;
	};
}
#endif