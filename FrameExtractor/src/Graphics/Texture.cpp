/******************************************************************************/
/*!
\file       Texture.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 11, 2025
\brief      Defines the Texture class that represents a loaded texture

 ******************************************************************************/
#include <FrameExtractorPCH.hpp>

 // Third-party includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION // This line is necessary to include the implementation of stb_image
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION // This line is necessary to include the implementation of stb_image_write
#include <stb_image_write.h>

// Project includes
#include <Core/LoggerManager.hpp>
#include <Graphics/Texture.hpp>
namespace FrameExtractor
{
	Texture::Texture()
	{}

	Texture::Texture(int internalformat, uint32_t width, uint32_t height, int format, int type, const void* data)
	{
		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Texture::Texture(std::filesystem::path path)
	{
		if (mRendererID)
			Unload();
		mPath = path;
	}
	// Fill with solid red (255, 0, 0)
	Texture::Texture(uint32_t width, uint32_t height) : mWidth(width), mHeight(height)
	{
		mChannels = 3;
		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Texture::~Texture()
	{
		if (mRendererID)
			Unload();
	}
	void Texture::Load()
	{
		Load(mPath);
	}
	void Texture::Load(const std::filesystem::path& path)
	{
		auto data = stbi_load(path.string().c_str(), (int*)&mWidth, (int*)&mHeight, (int*)&mChannels, 0);
		if (mChannels == 0)
		{
			FRAMEEX_CORE_ERROR("Failed to load texture from path: {0}", path.string());
			return;
		}

		glGenTextures(1, &mRendererID);
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (mChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (mChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			FRAMEEX_CORE_ERROR("Unsupported texture format with {0} channels", mChannels);
			return;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	void Texture::SaveImageToPath(const std::filesystem::path& path, SaveImageFunction saveFunc) const
	{
		if (mRendererID && IsLoaded())
		{
			glBindTexture(GL_TEXTURE_2D, mRendererID);
			std::vector<unsigned char> pixels(mWidth * mHeight * mChannels);
			glGetTexImage(GL_TEXTURE_2D, 0, (mChannels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
			(this->*saveFunc)(path.string().c_str(), mWidth, mHeight, mChannels, pixels.data(), mWidth * mChannels);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			FRAMEEX_CORE_ERROR("Texture not loaded or invalid renderer ID. Cannot save to file: {0}", path.string());
		}
	}

	void Texture::SaveToFilePNG(const char* filename, int width, int height, int channels, const void* data, int stride) const
	{
		stbi_write_png(filename, width, height, channels, data, stride);
	}
	void Texture::SaveToFileJPEG(const char* filename, int width, int height, int channels, const void* data, int quality) const
	{
		stbi_write_jpg(filename, width, height, channels, data, 100); // 100 is the quality level for JPEG
	}

	void Texture::SaveToFileBMP(const char* filename, int width, int height, int channels, const void* data, int quality) const
	{
		stbi_write_bmp(filename, width, height, channels, data);
	}

	void Texture::SaveToFileTGA(const char* filename, int width, int height, int channels, const void* data, int quality) const
	{
		stbi_write_tga(filename, width, height, channels, data);
	}

	void Texture::Unload()
	{
		glDeleteTextures(1, &mRendererID);
		mRendererID = 0;
	}
	void Texture::Update(void* buffer)
	{
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	}

	void Texture::SaveToFile(const std::filesystem::path& path, OutputFormat format) const
	{
		switch (format)
		{
			case OutputFormat::PNG:
				SaveImageToPath(path, &Texture::SaveToFilePNG);
				return;
			case OutputFormat::JPEG:
				SaveImageToPath(path, &Texture::SaveToFileJPEG);
				return;
			case OutputFormat::BMP:
				SaveImageToPath(path, &Texture::SaveToFileBMP);
				return;
			case OutputFormat::TGA:
				SaveImageToPath(path, &Texture::SaveToFileTGA);
				return;
			default:
				FRAMEEX_CORE_ERROR("Unsupported output format for texture saving: {0}", static_cast<int>(format));
				return;
		}
	}

	Ref<Texture> Texture::GetInvisibleTexture()
	{
		static unsigned char data[4]{ 0,0,0,0 };
		static Ref<Texture> sInvisibleTexture = MakeRef<Texture>(GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return sInvisibleTexture;
	}
}