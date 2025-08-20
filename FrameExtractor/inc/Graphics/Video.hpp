/******************************************************************************/
/*!
\file       Video.hpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 11, 2025
\brief      Declares the Video class that represents a loaded video

 ******************************************************************************/

#ifndef Video_HPP
#define Video_HPP
 // Standard Library includes
#include <filesystem>

// Third-party includes
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// Project includes
#include <Core/Asset.hpp>
#include <Core/Core.hpp>
#include <Graphics/Texture.hpp>
namespace FrameExtractor
{
	/*!***********************************************************************
		\brief
			Class that represents a video asset in the system.
	*************************************************************************/
	class Video : public Asset
	{
	public:
		/*!***********************************************************************
			\brief
				Default constructor for Video class.
		*************************************************************************/
		Video() = default;
		/*!***********************************************************************
			\brief
				Constructor that initializes the video with a file path.
			\param[in] path
				The file path to the video file.
		*************************************************************************/
		Video(const std::filesystem::path& path);

		/*!***********************************************************************
			\brief
				Default destructor for Video class.
		*************************************************************************/
		~Video();

		/*!***********************************************************************
			\brief
				Loads the video from a path previously set in the constructor.
		*************************************************************************/
		void Load();

		/*!***********************************************************************
			\brief
				Loads the video from a specified path.
			\param[in] path
				The file path to the video file.
		*************************************************************************/
		void Load(const std::filesystem::path& path) override;

		/*!***********************************************************************
			\brief
				Unloads the video and frees associated resources.
		*************************************************************************/
		void Unload() override;

		/*!***********************************************************************
			\brief
				Checks if the video is loaded.
			\return
				True if the video is loaded, false otherwise.
		*************************************************************************/
		bool IsLoaded() const override { return formatContext != nullptr; }

		/*!***********************************************************************
			\brief
				Gets the asset type of the video.
			\return
				The asset type of the video.
		*************************************************************************/
		AssetType GetAssetType() const override { return AssetType::Video; }

		/*!***********************************************************************
			\brief
				Gets the current frame as a texture.
			\return
				A reference to the texture representing the current frame.
		*************************************************************************/
		Ref<Texture> GetFrame();

		/*!***********************************************************************
			\brief
				Decodes the video frame by frame based on the specified time delta and speed factor.
			\param[in] dt
				The time delta since the last frame.
			\param[in] speedFactor
				The factor by which to speed up or slow down the video playback.
		*************************************************************************/
		void DecodeTime(float dt, float speedFactor);

		/*!***********************************************************************
			\brief
				Decodes a specific frame by its index.
			\param[in] frameIndex
				The index of the frame to decode.
			\return
				True if the frame was successfully decoded, false otherwise.
		*************************************************************************/
		bool Decode(uint32_t frameIndex);

		/*!***********************************************************************
			\brief
				Gets the FPS (frames per second) of the video.
			\return
				The FPS of the video.
		*************************************************************************/
		inline uint8_t GetFPS() const { return mFPS; }

		/*!***********************************************************************
			\brief
				Gets the maximum number of frames in the video.
			\return
				The maximum number of frames in the video.
		*************************************************************************/
		inline uint32_t GetMaxFrames() const { return mMaxFrames; }

		/*!***********************************************************************
			\brief
				Gets the width of the video.
			\return
				The width of the video in pixels.
		*************************************************************************/
		inline uint32_t GetWidth() const { return mWidth; }

		/*!***********************************************************************
			\brief
				Gets the height of the video.
			\return
				The height of the video in pixels.
		*************************************************************************/
		inline uint32_t GetHeight() const { return mHeight; }

		/*!***********************************************************************
			\brief
				Gets the path of the video file.
			\return
				The path to the video file.
		*************************************************************************/
		inline std::filesystem::path GetPath() const { return mPath; }
	private:
		std::filesystem::path mPath;				// Path to the video file
		uint8_t mFPS;								// Frames per second of the video
		uint32_t mMaxFrames;						// Maximum number of frames in the video
		uint32_t mWidth;							// Width of the video in pixels
		uint32_t mHeight;							// Height of the video in pixels

		AVFormatContext* formatContext = nullptr;	// Container context
		AVCodecContext* codecContext = nullptr;		// Decoder context
		const AVCodec* codec = nullptr;				// Codec for decoding
		AVStream* videoStream = nullptr;			// Video stream
		AVFrame* frame = nullptr;					// Frame to hold decoded data
		AVFrame* RGBframe = nullptr;                // Frame to hold decoded data
		AVFrame* convertedFrame = nullptr;			// Converted frame (e.g., RGB)
		SwsContext* swsContext = nullptr;			// For pixel format conversion
		AVPacket* packet = nullptr;                 // Packet for compressed data

		Ref<Texture> mTexture;						// Vector of textures for each frame
		double time_per_frame = 0.0;
		double last_frame_time = 0.0;
	};
}

#endif