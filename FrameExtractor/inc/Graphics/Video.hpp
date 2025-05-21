/******************************************************************************
/*!
\file       Video.hpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Declares the Video class that represents a loaded video

 /******************************************************************************/

#ifndef Video_HPP
#define Video_HPP

#include <filesystem>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <Core/Core.hpp>
#include <Graphics/Texture.hpp>
namespace FrameExtractor
{
	class Video
	{
	public:
		Video(const std::filesystem::path& path);
		~Video();
		Ref<Texture> GetFrame();
		void DecodeTime(float dt, float speedFactor);
		bool Decode(uint32_t frameIndex);

		inline uint8_t GetFPS() const { return mFPS; }
		inline uint32_t GetMaxFrames() const { return mMaxFrames; }
		inline uint32_t GetWidth() const { return mWidth; }
		inline uint32_t GetHeight() const { return mHeight; }
		inline std::filesystem::path GetPath() const { return mPath; }
	private:
		std::filesystem::path mPath;
		uint8_t mFPS;
		uint32_t mMaxFrames;
		uint32_t mWidth;
		uint32_t mHeight;

		AVFormatContext* formatContext = nullptr;  // Container context
		AVCodecContext* codecContext = nullptr;    // Decoder context
		const AVCodec* codec = nullptr;            // Codec for decoding
		AVStream* videoStream = nullptr;           // Video stream
		AVFrame* frame = nullptr;                  // Frame to hold decoded data
		AVFrame* RGBframe = nullptr;                  // Frame to hold decoded data
		AVFrame* convertedFrame = nullptr;         // Converted frame (e.g., RGB)
		SwsContext* swsContext = nullptr;			// For pixel format conversion
		AVPacket* packet = nullptr;                           // Packet for compressed data

		Ref<Texture> mTexture;       // Vector of textures for each frame
		double time_per_frame = 0.0;
		double last_frame_time = 0.0;

			
	};
}

#endif
