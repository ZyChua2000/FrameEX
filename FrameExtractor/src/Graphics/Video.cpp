/******************************************************************************/
/*!
\file       Video.cpp
\author     Chua Zheng Yang
\par		email: 2202829\@sit.singaporetech.edu.sg
\par    	email: zhengyang.chua\@hendrickscorp.com
\par		email: chuazhengyang2000\@gmail.com
\date       May 11, 2025
\brief      Defines the Video class that represents a loaded video

 ******************************************************************************/
#include <FrameExtractorPCH.hpp>
 // Third-party includes
#include <GLFW/glfw3.h>
extern "C"
{
#include <libavutil/imgutils.h>
}
#include <FileWatch.hpp>

// Project includes
#include <Core/LoggerManager.hpp>
#include <Graphics/Video.hpp>
namespace FrameExtractor
{
	Video::Video(const std::filesystem::path& path)
	{
		mPath = path;
	}
	Video::~Video()
	{
		if (formatContext)
		{
			Unload();
		}
	}
	void Video::Load()
	{
		Load(mPath);
	}
	void Video::Load(const std::filesystem::path& path)
	{
		if (formatContext)
		{
			Unload();
		}

		formatContext = avformat_alloc_context();
		if (avformat_open_input(&formatContext, path.string().c_str(), nullptr, nullptr) != 0)
		{
			FRAMEEX_CORE_ERROR("Failed to open video file: {}", path.string());
			return;
		}

		if (avformat_find_stream_info(formatContext, nullptr) < 0)
		{
			FRAMEEX_CORE_ERROR("Failed to find stream information");
			Unload();
			return;
		}

		for (unsigned int i = 0; i < formatContext->nb_streams; i++)
		{
			if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				videoStream = formatContext->streams[i];
				break;
			}
		}

		if (!videoStream)
		{
			FRAMEEX_CORE_ERROR("Failed to find video stream");
			Unload();
			return;
		}

		auto codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
		if (!codec)
		{
			FRAMEEX_CORE_ERROR("Failed to find codec");
			Unload();
			return;
		}

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext)
		{
			FRAMEEX_CORE_ERROR("Failed to allocate codec context");
			Unload();
			return;
		}

		if (avcodec_parameters_to_context(codecContext, videoStream->codecpar) < 0)
		{
			FRAMEEX_CORE_ERROR("Failed to copy codec parameters");
			Unload();
			return;
		}

		if (avcodec_open2(codecContext, codec, nullptr) < 0)
		{
			FRAMEEX_CORE_ERROR("Failed to open codec");
			Unload();
			return;
		}

		// Get video dimensions
		mWidth = codecContext->width;
		mHeight = codecContext->height;

		swsContext = sws_getContext(mWidth, mHeight, codecContext->pix_fmt, mWidth, mHeight, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

		// Allocate frame structures
		frame = av_frame_alloc();
		if (!frame)
		{
			FRAMEEX_CORE_ERROR("Failed to allocate frame");
			Unload();
			return;
		}
		RGBframe = av_frame_alloc();
		if (!RGBframe)
		{
			FRAMEEX_CORE_ERROR("Failed to allocate RGB frame");
			Unload();
			return;
		}

		int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, mWidth, mHeight, 1);
		uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
		av_image_fill_arrays(RGBframe->data, RGBframe->linesize, buffer, AV_PIX_FMT_RGB24, mWidth, mHeight, 1);

		packet = av_packet_alloc();

		AVRational& fps = videoStream->avg_frame_rate;
		mFPS = static_cast<float>(fps.num) / fps.den; // Convert to milliseconds for FPS
		mMaxFrames = (uint32_t)(videoStream->duration * av_q2d(videoStream->time_base) * (static_cast<float>(fps.num) / fps.den));
		mTexture = MakeRef<Texture>(mWidth, mHeight);
		mPath = path;
	}
	void Video::Unload()
	{
		if (packet)
		{
			av_packet_free(&packet);
			packet = nullptr;
		}
		if (RGBframe)
		{
			av_frame_free(&RGBframe);
			RGBframe = nullptr;
		}
		if (frame)
		{
			av_frame_free(&frame);
			frame = nullptr;
		}
		if (swsContext)
		{
			sws_freeContext(swsContext);
			swsContext = nullptr;
		}
		if (codecContext)
		{
			avcodec_free_context(&codecContext);
			codecContext = nullptr;
		}
		if (formatContext)
		{
			avformat_close_input(&formatContext);
			formatContext = nullptr;
		}
	}
	Ref<Texture> Video::GetFrame()
	{
		return mTexture;
	}
	void Video::DecodeTime(float dt, float speedFactor)
	{
		// Standard time per frame based on FPS
		float time_per_frame = 1.0f / mFPS;
		time_per_frame /= speedFactor; // Adjust for speed factor

		// Increment the last frame timer
		last_frame_time += dt;

		// Only process if enough time has passed for the next frame
		while (last_frame_time >= time_per_frame)
		{
			// Read one packet at a time
			if (av_read_frame(formatContext, packet) >= 0)
			{
				if (packet->stream_index == videoStream->index)
				{
					// Send packet to decoder
					if (avcodec_send_packet(codecContext, packet) == 0)
					{
						// Receive frame from decoder
						if (avcodec_receive_frame(codecContext, frame) == 0)
						{
							// Convert to RGB (or your target format)
							sws_scale(
								swsContext,
								frame->data, frame->linesize,
								0, mHeight,
								RGBframe->data, RGBframe->linesize
							);

							// Update texture
							mTexture->Update(RGBframe->data[0]);
						}
					}
				}

				// Always unref packet regardless of whether frame was decoded
				av_packet_unref(packet);
			}

			// Subtract the frame interval for next frame calculation
			last_frame_time -= time_per_frame;
		}
	}

	bool Video::Decode(uint32_t frameIndex)
	{
		int fps = static_cast<int>(av_q2d(formatContext->streams[videoStream->index]->r_frame_rate));
		int64_t timestamp = av_rescale_q(frameIndex, { 1, fps }, formatContext->streams[videoStream->index]->time_base);

		av_seek_frame(formatContext, videoStream->index, timestamp, AVSEEK_FLAG_BACKWARD);
		avcodec_flush_buffers(codecContext);

		AVPacket pkt;
		bool frameLoaded = false;

		while (av_read_frame(formatContext, &pkt) >= 0)
		{
			if (pkt.stream_index == videoStream->index)
			{
				if (avcodec_send_packet(codecContext, &pkt) == 0)
				{
					while (avcodec_receive_frame(codecContext, frame) == 0)
					{
						// Recalculate frame PTS (if needed)
						int64_t current_frame = av_rescale_q(frame->pts, formatContext->streams[videoStream->index]->time_base, { 1, mFPS });
						if (current_frame >= frameIndex)
						{
							sws_scale(swsContext, frame->data, frame->linesize, 0, mHeight, RGBframe->data, RGBframe->linesize);

							mTexture->Update(RGBframe->data[0]);

							frameLoaded = true;
							break;
						}
					}
				}
			}
			av_packet_unref(&pkt);
			if (frameLoaded) break;
		}

		return frameLoaded;
	}
}