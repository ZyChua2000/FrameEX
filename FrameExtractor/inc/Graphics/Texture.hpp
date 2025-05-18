/******************************************************************************
/*!
\file       Texture.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Declares the Texture class that represents a loaded video

 /******************************************************************************/

#ifndef Texture_HPP
#define Texture_HPP
#include <Core/Core.hpp>
namespace FrameExtractor
{
	class Texture
	{
	public:
		Texture();
		Texture(int internalformat, uint32_t width, uint32_t height, int format, int type, const void* data);
		Texture(std::filesystem::path path);
		Texture(uint32_t width, uint32_t height);
		~Texture();
		inline uint32_t GetTextureID() const { return mRendererID; }
		void Update(void* buffer);
		static Ref<Texture> GetInvisibleTexture();
	private:
		uint32_t mRendererID = 0;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		uint32_t mChannels = 0;

	};
}

#endif
