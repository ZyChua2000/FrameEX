/******************************************************************************/
/*!
\file       AssetManager.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       July 17, 2025
\brief      Declares the Asset Manager class

******************************************************************************/

#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

// Standard Library includes
#include <map>
#include <filesystem>
#include <mutex>

// Third-party includes
#include <magic_enum/magic_enum.hpp>

// Project includes
#include <Core/AssetHandle.hpp>
#include <Core/Asset.hpp>
#include <Core/Core.hpp>
#include <Core/LoggerManager.hpp>
#include <Core/FileWatcher.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Video.hpp>

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
	/*!***********************************************************************
		\brief
			Structure that holds metadata for an asset.
	*************************************************************************/
	struct MetaData
	{
		std::filesystem::path mPath;
		AssetHandle mHandle;
		AssetType mAssetType;
	};

	/*!***********************************************************************
		\brief
			Class that manages assets in the project.
	*************************************************************************/
	class AssetManager
	{
	public:
		friend class Project;

		/*!***********************************************************************
			\brief
				Initializes the AssetManager.
		*************************************************************************/
		static void Init();

		/*!***********************************************************************
			\brief
				Updates the AssetManager, processing asset queues.
		*************************************************************************/
		static void Update();

		/*!***********************************************************************
			\brief
				Registers an asset with the given metadata.
			\param[in] data
				The metadata of the asset to register.
		*************************************************************************/
		static void RegisterAsset(const MetaData& data);

		/*!***********************************************************************
			\brief
				Registers an asset from a file path.
			\param[in] path
				The path of the asset to register.
		*************************************************************************/
		static void RegisterAsset(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Loads a new asset from the given path.
			\param[in] path
				The path of the asset to load.
		*************************************************************************/
		static void LoadNewAsset(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Loads an asset from the given path with a specific handle and type.
			\param[in] path
				The path of the asset to load.
			\param[in] handle
				The handle for the asset.
			\param[in] type
				The type of the asset.
		*************************************************************************/
		static void LoadAsset(std::filesystem::path path, AssetHandle handle, AssetType type);

		/*!***********************************************************************
			\brief
				Loads an asset using the provided metadata.
			\param[in] data
				The metadata of the asset to load.
		*************************************************************************/
		static void LoadAsset(MetaData data);

		/*!***********************************************************************
			\brief
				Unloads the asset with the given handle.
			\param[in] handle
				The handle of the asset to unload.
		*************************************************************************/
		static void UnloadAsset(AssetHandle handle);

		/*!***********************************************************************
			\brief
				Unloads the asset at the specified path.
			\param[in] path
				The path of the asset to unload.
		*************************************************************************/
		static void UnloadAsset(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Removes the asset with the given handle.
			\param[in] handle
				The handle of the asset to remove.
		*************************************************************************/
		static void RemoveAsset(AssetHandle handle);

		/*!***********************************************************************
			\brief
				Removes the asset at the specified path.
			\param[in] path
				The path of the asset to remove.
		*************************************************************************/
		static void RemoveAsset(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Reloads the asset with the given handle.
			\param[in] handle
				The handle of the asset to reload.
		*************************************************************************/
		static void ReloadAsset(AssetHandle handle);

		/*!***********************************************************************
			\brief
				Reloads the asset at the specified path.
			\param[in] path
				The path of the asset to reload.
		*************************************************************************/
		static void ReloadAsset(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Adds all assets in the specified directory, excluding any paths in the excludedPaths set.
			\param[in] directory
				The directory to scan for assets.
			\param[in] excludedPaths
				A set of paths to exclude from asset registration.
		*************************************************************************/
		static void AddOnDirectory(const std::filesystem::path& directory, const std::unordered_set<std::filesystem::path>& excludedPaths = {});

		/*!***********************************************************************
			\brief
				Loads all assets in the specified directory.
			\param[in] directory
				The directory to load assets from.
		*************************************************************************/
		static void LoadDirectory(const std::filesystem::path& directory);

		/*!***********************************************************************
			\brief
				Clears all registered assets and metadata.
		*************************************************************************/
		static inline void Clear()
		{
			mAssets.clear();
			mMetaData.clear();
		}

		/*!***********************************************************************
			\brief
				Retrieves an asset of type T using its handle.
			\param[in] handle
				The handle of the asset to retrieve.
			\return
				A reference to the asset of type T, or nullptr if not found or type mismatch.
		*************************************************************************/
		template <typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			static_assert(sizeof(T) == 0, "GetAsset<T> not specialized for this type.");
			return nullptr;
		}

		DEFINE_ASSET_GETTER(Texture, AssetType::Texture)
		DEFINE_ASSET_GETTER(Video, AssetType::Video)


		/*!***********************************************************************
			\brief
				Retrieves all registered assets.
			\return
				A constant reference to the map of assets.
		*************************************************************************/
		static inline const std::unordered_map<AssetHandle, Ref<Asset>>& GetAssets()
		{
			return mAssets;
		}

		/*!***********************************************************************
			\brief
				Retrieves all registered metadata.
			\return
				A constant reference to the map of metadata.
		*************************************************************************/
		static inline const std::unordered_map<AssetHandle, MetaData>& GetMetaDatas()
		{
			return mMetaData;
		}

		/*!***********************************************************************
			\brief
				Sets the asset directory.
			\param[in] directory
				The path to the asset directory.
		*************************************************************************/
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
		static FileWatcher mFileWatcher; //<- File watcher for monitoring asset changes
	private:

		/*!***********************************************************************
			\brief
				Registers an asset loader for a specific asset type.
			\param[in] extensions
				A vector of file extensions that the loader supports.
			\details
				This function is used to register asset loaders for specific asset types.
				It uses a static assertion to ensure that T is derived from Asset.
		*************************************************************************/
		template<typename T>
		static void RegisterAssetLoaderInternal(const std::vector<std::string>& extensions)
		{
			static_assert(std::is_base_of<Asset, T>::value, "T must be derived from Asset");
			for (const auto& ext : extensions)
			{
				mLoaderRegistry[ext] = [](const std::filesystem::path& path) -> Ref<Asset>
					{
						auto asset = MakeRef<T>(path);
						return asset;
					};
			}
		}

		/*!***********************************************************************
			\brief
				Registers the asset loaders for all supported asset types.
			\details
				This function registers asset loaders for various asset types such as Texture and Video.
				It is called during the initialization of the AssetManager.
		*************************************************************************/
		static void RegisterAssetLoader();

		static std::unordered_map<AssetHandle, MetaData> mMetaData;		//<- Map of asset handles to their metadata
		static std::unordered_map<AssetHandle, Ref<Asset>> mAssets;		//<- Map of asset handles to their assets
		static std::filesystem::path* mAssetDirectory;					//<- Pointer to the asset directory path

		static std::queue<std::filesystem::path> mAssetRegisterQueue;	//<- Queue for newly registered assets
		static std::queue<std::filesystem::path> mAssetReloadQueue;		//<- Queue for assets to be reloaded
		static std::queue<std::filesystem::path> mAssetRemoveQueue;		//<- Queue for assets to be removed
		static std::mutex mAssetQueueMutex;								//<- Mutex for synchronizing access to asset queues

		using AssetLoaderFunc = std::function<Ref<Asset>(const std::filesystem::path&)>;	//<- Function type for asset loaders
		static std::unordered_map<std::string, AssetLoaderFunc> mLoaderRegistry;			//<- Registry of asset loaders by file extension
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
	struct hash<FrameExtractor::MetaData>
	{
		std::size_t operator()(const FrameExtractor::MetaData& data) const
		{
			return static_cast<uint64_t>(data.mHandle);
		}
	}; // struct hash
} // namespace std
#endif