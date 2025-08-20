/******************************************************************************/
/*!
\file       Texture.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 11, 2025
\brief      Declares the Texture class that represents a loaded video

 ******************************************************************************/

#ifndef Texture_HPP
#define Texture_HPP
 // Project includes
#include <Core/Core.hpp>
#include <Core/Asset.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents a texture asset in the system.
	*************************************************************************/
	class Texture : public Asset
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor for Texture class.
		*************************************************************************/
		Texture();

		/*!***********************************************************************
			\brief
				Constructor that initializes the texture with specific parameters.
			\param[in] internalformat
				The internal format of the texture.
			\param[in] width
				The width of the texture.
			\param[in] height
				The height of the texture.
			\param[in] format
				The format of the texture.
			\param[in] type
				The type of the texture data.
			\param[in] data
				The data to initialize the texture with.
		*************************************************************************/
		Texture(int internalformat, uint32_t width, uint32_t height, int format, int type, const void* data);

		/*!***********************************************************************
			\brief
				Constructor that initializes the texture with a file path.
			\param[in] path
				The file path to the texture image.
		*************************************************************************/
		Texture(std::filesystem::path path);

		/*!***********************************************************************
			\brief
				Constructor that initializes the texture with a specific width and height.
			\param[in] width
				The width of the texture.
			\param[in] height
				The height of the texture.
		*************************************************************************/
		Texture(uint32_t width, uint32_t height);

		/*!***********************************************************************
			\brief
				Destructor for Texture class.
		*************************************************************************/
		~Texture();

		/*!***********************************************************************
			\brief
				Loads the texture from the specified path.
		*************************************************************************/
		void Load();

		/*!***********************************************************************
			\brief
				Loads the texture from the specified path.
			\param[in] path
				The path to load the texture from.
		*************************************************************************/
		void Load(const std::filesystem::path& path) override;

		/*!***********************************************************************
			\brief
				Unloads the texture, releasing any resources.
		*************************************************************************/
		void Unload() override;

		/*!***********************************************************************
			\brief
				Checks if the texture is loaded.
			\return
				True if the texture is loaded, false otherwise.
		*************************************************************************/
		bool IsLoaded() const override { return mRendererID != 0; }

		/*!***********************************************************************
			\brief
				Gets the asset type of the texture.
			\return
				The asset type of the texture.
		*************************************************************************/
		AssetType GetAssetType() const override { return AssetType::Texture; }

		/*!***********************************************************************
			\brief
				Gets the Texture ID used by the renderer.
			\return
				The renderer ID of the texture.
		*************************************************************************/
		inline uint32_t GetTextureID() const { return mRendererID; }

		/*!***********************************************************************
			\brief
				Updates the texture with new data.
			\param[in] buffer
				The buffer containing the new texture data.
		*************************************************************************/
		void Update(void* buffer);

		/*!***********************************************************************
			\brief
				Enumeration for different output formats when saving the texture.
		*************************************************************************/
		enum class OutputFormat
		{
			PNG,
			JPEG,
			BMP,
			TGA
		};

		/*!***********************************************************************
			\brief
				Saves the texture to a file.
			\param[in] path
				The path to save the texture to.
			\param[in] format
				The format to save the texture in (PNG, JPEG, etc.).
		*************************************************************************/
		void SaveToFile(const std::filesystem::path& path, OutputFormat format = OutputFormat::JPEG) const;

		/*!***********************************************************************
			\brief
				Gets the width of the texture.
			\return
				The width of the texture.
		*************************************************************************/
		static Ref<Texture> GetInvisibleTexture();
	private:

		/*!***********************************************************************
			\brief
				Saves the texture to a file.
			\param[in] path
				The path to save the texture to.
		*************************************************************************/
		void SaveToFilePNG(const char* filename, int width, int height, int channels, const void* data, int stride) const;

		/*!***********************************************************************
			\brief
				Saves the texture to a file.
			\param[in] path
				The path to save the texture to.
		*************************************************************************/
		void SaveToFileJPEG(const char* filename, int width, int height, int channels, const void* data, int quality) const;

		/*!***********************************************************************
			\brief
				Saves the texture to a file.
			\param[in] path
				The path to save the texture to.
		*************************************************************************/
		void SaveToFileBMP(const char* filename, int width, int height, int channels, const void* data, int quality) const;

		/*!***********************************************************************
			\brief
				Saves the texture to a file.
			\param[in] path
				The path to save the texture to.
		*************************************************************************/
		void SaveToFileTGA(const char* filename, int width, int height, int channels, const void* data, int quality) const;


		using SaveImageFunction = void(Texture::*)(const char* filename, int width, int height, int channels, const void* data, int stride) const;
		/*!***********************************************************************
			\brief
				Saves the texture to a file using the specified save function.
			\param[in] path
				The path to save the texture to.
			\param[in] saveFunc
				The function to use for saving the image.
		*************************************************************************/
		void SaveImageToPath(const std::filesystem::path& path, SaveImageFunction saveFunc) const;

		uint32_t mRendererID = 0;		//<- Renderer ID for OpenGL texture
		uint32_t mWidth = 0;			//<- Width of the texture
		uint32_t mHeight = 0;			//<- Height of the texture
		uint32_t mChannels = 0;			//<- Number of channels in the texture (e.g., RGB = 3, RGBA = 4)
		std::filesystem::path mPath;	//<- Path to the texture file
	};
}

#endif