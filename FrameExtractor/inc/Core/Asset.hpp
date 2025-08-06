/******************************************************************************
/*!
\file       Asset.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset class

 /******************************************************************************/

#ifndef Asset_HPP
#define Asset_HPP
#include <filesystem>
namespace FrameExtractor
{
	enum class AssetType
	{
		Texture,
		Video,
		Unknown
	};

	class Asset
	{
	public:
		virtual ~Asset() = default;
		virtual void Unload() = 0;
		virtual bool IsLoaded() const = 0;
		virtual void Load(const std::filesystem::path& path) = 0;
		virtual AssetType GetAssetType() const = 0;
	};
}
#endif