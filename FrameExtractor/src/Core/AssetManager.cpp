/******************************************************************************/
/*!
\file       AssetManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Defines the Asset Manager class

******************************************************************************/
#include <FrameExtractorPCH.hpp>

// Project includes
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
	std::filesystem::path* AssetManager::mAssetDirectory = nullptr;

	std::queue<std::filesystem::path> AssetManager::mAssetRegisterQueue;
	std::queue<std::filesystem::path> AssetManager::mAssetReloadQueue;
	std::queue<std::filesystem::path> AssetManager::mAssetRemoveQueue;
	std::mutex AssetManager::mAssetQueueMutex;

	FileWatcher AssetManager::mFileWatcher;

	void AssetManager::Init()
	{
		RegisterAssetLoader();
		
		mFileWatcher.SetCallback([&](const std::filesystem::path& file, filewatch::Event event)
		{
			std::lock_guard<std::mutex> lock(mAssetQueueMutex);
			switch (event)
			{
			case filewatch::Event::added:
				mAssetRegisterQueue.push(*mAssetDirectory / file);
				FRAMEEX_CORE_INFO("New asset added: {}", file.string());
				break;
			case filewatch::Event::modified:
				mAssetReloadQueue.push(*mAssetDirectory / file);
				FRAMEEX_CORE_INFO("Asset modified: {}", file.string());
				break;
			case filewatch::Event::removed:
				mAssetRemoveQueue.push(*mAssetDirectory / file);
				FRAMEEX_CORE_INFO("Asset removed: {}", file.string());
				break;
			default:
				break;
			}
		});
	}

	void AssetManager::Update()
	{
		std::lock_guard<std::mutex> lock(mAssetQueueMutex);
		while (!mAssetRegisterQueue.empty())
		{
			RegisterAsset(mAssetRegisterQueue.front());
			mAssetRegisterQueue.pop();
		}
		while (!mAssetReloadQueue.empty())
		{
			ReloadAsset(mAssetReloadQueue.front());
			mAssetReloadQueue.pop();
		}
		while (!mAssetRemoveQueue.empty())
		{
			RemoveAsset(mAssetRemoveQueue.front());
			mAssetRemoveQueue.pop();
		}
	}

	void AssetManager::RegisterAsset(const MetaData& data)
	{
		std::string extension = data.mPath.extension().string();

		auto loaderIt = mLoaderRegistry.find(extension);
		if (loaderIt == mLoaderRegistry.end())
		{
			FRAMEEX_CORE_ERROR("Unsupported asset type: {}", extension);
			return;
		}
		mAssets[data.mHandle] = loaderIt->second(data.mPath);
		mMetaData[data.mHandle] = data;
	}

	void AssetManager::RegisterAsset(const std::filesystem::path& path)
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
		mAssets[data.mHandle]->Load();
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
				return;
			}
		}
	}
	void AssetManager::RemoveAsset(AssetHandle handle)
	{
		auto it = mAssets.find(handle);
		if (it != mAssets.end())
		{
			it->second->Unload();
			mMetaData.erase(handle);
			mAssets.erase(it);
		}
		else
		{
			FRAMEEX_CORE_ERROR("Asset with handle {} not found", static_cast<uint64_t>(handle));
		}
	}
	void AssetManager::RemoveAsset(const std::filesystem::path& path)
	{
		for (auto it = mAssets.begin(); it != mAssets.end(); it++)
		{
			if (mMetaData[it->first].mPath == path)
			{
				it->second->Unload();
				mMetaData.erase(it->first);
				it = mAssets.erase(it);
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
	void AssetManager::AddOnDirectory(const std::filesystem::path& directory, const std::unordered_set<std::filesystem::path>& excludedPaths)
	{
		if (!std::filesystem::exists(directory))
		{
			FRAMEEX_CORE_ERROR("Directory does not exist: {}", directory.string());
			return;
		}
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			if (entry.is_regular_file())
			{
				if (excludedPaths.find(entry.path()) == excludedPaths.end())
				{
					RegisterAsset(entry.path());
				}
			}
			else if (entry.is_directory())
			{
				LoadDirectory(entry.path()); // Recursively load assets in subdirectories
			}
		}
	}
	void AssetManager::LoadDirectory(const std::filesystem::path& directory)
	{
		if (!std::filesystem::exists(directory))
		{
			FRAMEEX_CORE_ERROR("Directory does not exist: {}", directory.string());
			return;
		}
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			if (entry.is_regular_file())
			{
				RegisterAsset(entry.path());
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