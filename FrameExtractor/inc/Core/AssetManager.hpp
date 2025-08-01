/******************************************************************************
/*!
\file       AssetManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2024
\brief      Declares the Asset Manager class

/******************************************************************************/

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP
#include <magic_enum/magic_enum.hpp>
#include <Core/AssetHandle.hpp>
#include <Core/Asset.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerManager.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Video.hpp>
#include <map>
#include <filesystem>
namespace FrameExtractor
{
	enum class AssetType
	{
		Texture,
		Video
	};

	struct MetaData
	{
		std::filesystem::path mPath;
		AssetHandle mHandle;
		AssetType mAssetType;
	};

	class AssetManager
	{
	public:

		void LoadNewAsset(std::filesystem::path path)
		{
			MetaData data
			{
				.mPath = path,
				.mHandle = AssetHandle()
			};

			if (path.extension() == ".png" || path.extension() == ".jpg" || path.extension() == ".jpeg")
			{
				data.mAssetType = AssetType::Texture;
				mAssets[data.mHandle] = MakeRef<Texture>(path);
			}
			else if (path.extension() == ".mp4" || path.extension() == ".avi" || path.extension() == ".mov")
			{
				data.mAssetType = AssetType::Video;
				mAssets[data.mHandle] = MakeRef<Video>(path);
			}
			else
			{
				FRAMEEX_CORE_ERROR("Unsupported asset type: {}", path.extension().string());
				return;
			}

			mMetaData[data.mHandle] = data;
		}

		void LoadAsset(std::filesystem::path path, AssetHandle handle, AssetType type)
		{
			MetaData data
			{
				.mPath = path,
				.mHandle = handle,
				.mAssetType = type
			};
			if (mAssets.find(handle) != mAssets.end())
			{
				FRAMEEX_CORE_ERROR("Asset with handle {} already exists", static_cast<uint64_t>(handle));
				return;
			}
			LoadAsset(data);
		}

		void LoadAsset(MetaData data)
		{
			switch (data.mAssetType)
			{
			case AssetType::Texture:
				mAssets[data.mHandle] = MakeRef<Texture>(data.mPath);
				break;
			case AssetType::Video:
				mAssets[data.mHandle] = MakeRef<Video>(data.mPath);
				break;
			default:
				FRAMEEX_CORE_ERROR("Unsupported asset type: {}", magic_enum::enum_name<AssetType>(data.mAssetType));
				return;
			}

			mMetaData[data.mHandle] = data;
		}

		void UnloadAsset(AssetHandle handle)
		{
			auto it = mAssets.find(handle);
			if (it != mAssets.end())
			{
				it->second->Unload();
				mAssets.erase(it);
				mMetaData.erase(handle);
			}
			else
			{
				FRAMEEX_CORE_ERROR("Asset with handle {} not found", static_cast<uint64_t>(handle));
			}
		}

		template <typename T>
		Ref<T> GetAsset(AssetHandle handle)
		{
			// check if T is derived from Asset
			static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset");
			auto it = mAssets.find(handle);
			if (it != mAssets.end())
			{
				return std::dynamic_pointer_cast<T>(it->second);
			}
			return nullptr;
		}
	private:
		std::map<AssetHandle, MetaData> mMetaData;
		std::map<AssetHandle, Ref<Asset>> mAssets;
		
	};

}

#endif