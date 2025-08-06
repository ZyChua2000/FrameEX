/******************************************************************************
/*!
\file       AssetManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset Manager class

/******************************************************************************/

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP
#include <magic_enum/magic_enum.hpp>
#include <Core/AssetHandle.hpp>
#include <Core/Asset.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerManager.hpp>
#include <Core/FileWatcher.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Video.hpp>
#include <map>
#include <filesystem>

#define DEFINE_ASSET_GETTER(TYPE, ASSET_TYPE_ENUM)               \
template<>                                                       \
static Ref<TYPE> GetAsset<TYPE>(AssetHandle handle)       \
{                                                                \
    auto assetIt = mAssets.find(handle);                         \
    if (assetIt == mAssets.end()) { /* error log */ return nullptr; } \
    auto metaIt = mMetaData.find(handle);                        \
    if (metaIt == mMetaData.end()) { /* error log */ return nullptr; } \
    if (metaIt->second.mAssetType != ASSET_TYPE_ENUM) { /* type mismatch log */ return nullptr; } \
    return std::static_pointer_cast<TYPE>(assetIt->second);      \
}

namespace FrameExtractor
{
	struct MetaData
	{
		std::filesystem::path mPath;
		AssetHandle mHandle;
		AssetType mAssetType;
	};

	class AssetManager
	{
	public:
		static void Init();
		static void LoadNewAsset(const std::filesystem::path& path);
		static void LoadAsset(std::filesystem::path path, AssetHandle handle, AssetType type);
		static void LoadAsset(MetaData data);
		static void UnloadAsset(AssetHandle handle);
		static void UnloadAsset(const std::filesystem::path& path);
		static void ReloadAsset(AssetHandle handle);
		static void ReloadAsset(const std::filesystem::path& path);

		static void LoadDirectory(const std::filesystem::path& directory);
		static inline void Clear()
		{
			mAssets.clear();
			mMetaData.clear();
		}

		template <typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			static_assert(sizeof(T) == 0, "GetAsset<T> not specialized for this type.");
			return nullptr;
		}

		DEFINE_ASSET_GETTER(Texture, AssetType::Texture)
			DEFINE_ASSET_GETTER(Video, AssetType::Video)

			static inline const std::unordered_map<AssetHandle, Ref<Asset>>& GetAssets()
		{
			return mAssets;
		}

		static inline const std::unordered_map<AssetHandle, MetaData>& GetMetaDatas()
		{
			return mMetaData;
		}

		static MetaData GetMetaData(AssetHandle handle)
		{
			auto it = mMetaData.find(handle);
			if (it != mMetaData.end())
			{
				return it->second;
			}
			FRAMEEX_CORE_ERROR("MetaData for handle {} not found", static_cast<uint64_t>(handle));
			return MetaData{};
		}
		static FileWatcher mFileWatcher;
	private:

		template<typename T>
		static void RegisterAssetLoaderInternal(const std::vector<std::string>& extensions)
		{
			static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset");
			for (const auto& ext : extensions)
			{
				mLoaderRegistry[ext] = [](const std::filesystem::path& path) -> Ref<Asset>
					{
						auto asset = MakeRef<T>();
						asset->Load(path);
						return asset;
					};
			}
		}

		static void RegisterAssetLoader();
		static std::unordered_map<AssetHandle, MetaData> mMetaData;
		static std::unordered_map<AssetHandle, Ref<Asset>> mAssets;

		using AssetLoaderFunc = std::function<Ref<Asset>(const std::filesystem::path&)>;
		static std::unordered_map<std::string, AssetLoaderFunc> mLoaderRegistry;
	};
}

#endif