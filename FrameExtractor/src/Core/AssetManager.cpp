/******************************************************************************
/*!
\file       AssetManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Defines the Asset Manager class

/******************************************************************************/
#include <FrameExtractorPCH.hpp>
#include "Core/AssetManager.hpp"
namespace FrameExtractor
{
	static AssetType DetermineAssetType(const std::string& extension)
	{
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
			return AssetType::Texture;
		if (extension == ".mp4" || extension == ".avi" || extension == ".mov")
			return AssetType::Video;
		return AssetType::Unknown;
	}

	// static variables
	std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::mAssets;
	std::unordered_map<AssetHandle, MetaData> AssetManager::mMetaData;
	std::unordered_map<std::string, AssetManager::AssetLoaderFunc> AssetManager::mLoaderRegistry;
	FileWatcher AssetManager::mFileWatcher;

	void AssetManager::Init()
	{
		RegisterAssetLoader();

		mFileWatcher.SetCallback([&](const std::filesystem::path& file, filewatch::Event event)
		{
			switch (event)
			{
			case filewatch::Event::added:
				LoadNewAsset(file);
				FRAMEEX_CORE_INFO("New asset added: {}", file.string());
				break;
			case filewatch::Event::modified:
				ReloadAsset(file);
				FRAMEEX_CORE_INFO("Asset modified: {}", file.string());
				break;
			case filewatch::Event::removed:
				UnloadAsset(file);
				FRAMEEX_CORE_INFO("Asset removed: {}", file.string());
				break;
			default:
				break;
			}
		});
	}
	void AssetManager::LoadNewAsset(const std::filesystem::path& path)
	{
		std::string extension = path.extension().string();

		auto loaderIt = mLoaderRegistry.find(extension);
		if (loaderIt == mLoaderRegistry.end())
		{
			FRAMEEX_CORE_ERROR("Unsupported asset type: {}", extension);
			return;
		}

		MetaData data{
			.mPath = path,
			.mHandle = AssetHandle(),
			.mAssetType = DetermineAssetType(extension) // Need a function for this
		};

		mAssets[data.mHandle] = loaderIt->second(path);
		mMetaData[data.mHandle] = data;
	}

	void AssetManager::LoadAsset(std::filesystem::path path, AssetHandle handle, AssetType type)
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
	void AssetManager::LoadAsset(MetaData data)
	{
		switch (data.mAssetType)
		{
		case AssetType::Texture:
			mAssets[data.mHandle] = MakeRef<Texture>();
			break;
		case AssetType::Video:
			mAssets[data.mHandle] = MakeRef<Video>();
			break;
		default:
			FRAMEEX_CORE_ERROR("Unsupported asset type: {}", magic_enum::enum_name<AssetType>(data.mAssetType));
			return;
		}
		mAssets[data.mHandle]->Load(data.mPath);
		mMetaData[data.mHandle] = data;
	}
	void AssetManager::UnloadAsset(AssetHandle handle)
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
	void AssetManager::UnloadAsset(const std::filesystem::path& path)
	{
		for (auto it = mAssets.begin(); it != mAssets.end(); it++)
		{
			if (mMetaData[it->first].mPath == path)
			{
				it->second->Unload();
				it = mAssets.erase(it);
				mMetaData.erase(it->first);
				return;
			}
		}
	}
	void AssetManager::ReloadAsset(AssetHandle handle)
	{
		auto it = mAssets.find(handle);
		if (it != mAssets.end())
		{
			auto metaIt = mMetaData.find(handle);
			if (metaIt == mMetaData.end())
			{
				FRAMEEX_CORE_ERROR("MetaData for handle {} not found", static_cast<uint64_t>(handle));
				return;
			}
			it->second->Unload();
			it->second->Load(metaIt->second.mPath);
		}
		else
		{
			FRAMEEX_CORE_ERROR("Asset with handle {} not found", static_cast<uint64_t>(handle));
		}
	}
	void AssetManager::ReloadAsset(const std::filesystem::path& path)
	{
		for (auto& [handle, asset] : mAssets)
		{
			if (mMetaData[handle].mPath == path)
			{
				asset->Unload();
				asset->Load(path);
				return;
			}
		}
		FRAMEEX_CORE_ERROR("Asset with path {} not found", path.string());
	}
	void AssetManager::LoadDirectory(const std::filesystem::path& directory)
	{
		if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
		{
			FRAMEEX_CORE_ERROR("Directory does not exist or is not a directory: {}", directory.string());
			return;
		}
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			if (entry.is_regular_file())
			{
				LoadNewAsset(entry.path());
			}
			else if (entry.is_directory())
			{
				LoadDirectory(entry.path()); // Recursively load assets in subdirectories
			}
		}
	}

	void AssetManager::RegisterAssetLoader()
	{
		RegisterAssetLoaderInternal<Texture>({ ".png", ".jpg", ".jpeg" });
		RegisterAssetLoaderInternal<Video>({ ".mp4", ".avi", ".mov" });
	}
}