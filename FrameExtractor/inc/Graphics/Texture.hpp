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

namespace FrameExtractor
{
	class Texture
	{
	public:
		Texture(std::filesystem::path path);
		Texture(uint32_t width, uint32_t height);
		~Texture();
		inline uint32_t GetTextureID() const { return mRendererID; }
		void Update(void* buffer);

	private:
		uint32_t mRendererID;
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mChannels;
	};
}

#endif
