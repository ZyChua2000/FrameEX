/******************************************************************************
/*!
\file       Texture.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Defines the Texture class that represents a loaded texture

 /******************************************************************************/
#include <FrameExtractorPCH.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/LoggerManager.hpp>
#include <Graphics/Texture.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
namespace FrameExtractor
{
	Texture::Texture(std::filesystem::path path)
	{
		stbi_set_flip_vertically_on_load(true);
		auto data = stbi_load(path.string().c_str(), (int*)&mWidth, (int*)&mHeight, (int*) & mChannels, 0);
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
		glDeleteTextures(1, &mRendererID);
	}
	void Texture::Update(void* buffer)
	{
		glBindTexture(GL_TEXTURE_2D, mRendererID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	}
}

