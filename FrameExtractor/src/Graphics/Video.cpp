/******************************************************************************
/*!
\file       Video.cpp
\author     Chua Zheng Yang
\par    	email: zhengyang.chua\@hendrickscorp.com
\date       May 11, 2024
\brief      Defines the Video class that represents a loaded video

 /******************************************************************************/
#include <FrameExtractorPCH.hpp>
#include <Core/LoggerManager.hpp>
#include <Graphics/Video.hpp>
#include <GLFW/glfw3.h>
extern "C"
{
#include <libavutil/imgutils.h>
}

namespace FrameExtractor
{
	Video::Video(const std::filesystem::path& path)
	{

		formatContext = avformat_alloc_context();
		if (avformat_open_input(&formatContext, path.string().c_str(), nullptr, nullptr) != 0) {
			FRAMEEX_CORE_ERROR("Failed to open video file: {}", path.string());
		}

		if (avformat_find_stream_info(formatContext, nullptr) < 0) {
			FRAMEEX_CORE_ERROR("Failed to find stream information");
		}

		for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
			if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStream = formatContext->streams[i];
				break;
			}
		}

		if (!videoStream) {
			FRAMEEX_CORE_ERROR("Failed to find video stream");
		}

		auto codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
		if (!codec) {
			FRAMEEX_CORE_ERROR("Failed to find codec");
		}

		codecContext = avcodec_alloc_context3(codec);
		if (!codecContext) {
			FRAMEEX_CORE_ERROR("Failed to allocate codec context");
		}

		if (avcodec_parameters_to_context(codecContext, videoStream->codecpar) < 0) {
			FRAMEEX_CORE_ERROR("Failed to copy codec parameters");
		}

		if (avcodec_open2(codecContext, codec, nullptr) < 0) {
			FRAMEEX_CORE_ERROR("Failed to open codec");
		}

		// Get video dimensions
		mWidth = codecContext->width;
		mHeight = codecContext->height;

		swsContext = sws_getContext(mWidth, mHeight, codecContext->pix_fmt, mWidth, mHeight, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

		// Allocate frame structures
		frame = av_frame_alloc();
		if (!frame) {
			FRAMEEX_CORE_ERROR("Failed to find allocate frame");
		}
		RGBframe = av_frame_alloc();
		if (!RGBframe)
		{
			FRAMEEX_CORE_ERROR("Failed to find allocate RGB frame");
		}


	
		int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, mWidth, mHeight, 1);
		uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
		av_image_fill_arrays(RGBframe->data, RGBframe->linesize, buffer, AV_PIX_FMT_RGB24, mWidth, mHeight, 1);

		packet = av_packet_alloc();

		AVRational fps = videoStream->avg_frame_rate;
		mFPS = static_cast<float>(fps.num) / fps.den;
		int64_t duration = videoStream->duration;
		// Convert stream duration to seconds
		double duration_seconds = duration * av_q2d(videoStream->time_base);
		// Calculate the maximum number of frames based on the FPS and duration
		double max_frames = duration_seconds * mFPS;
		mMaxFrames = (uint32_t)max_frames;
		mTexture = MakeRef<Texture>(mWidth, mHeight);
		mPath = path;

	}
	Video::~Video()
	{
		if (formatContext) {
			avformat_close_input(&formatContext);
		}
		if (codecContext) {
			avcodec_free_context(&codecContext);
		}
		if (frame) {
			av_frame_free(&frame);
		}
		if (RGBframe) {
			av_frame_free(&RGBframe);
		}
		if (swsContext)
		{
			sws_freeContext(swsContext);
		}
		if (packet)
		av_packet_free(&packet);

	}
	Ref<Texture> Video::GetFrame()
	{
		return mTexture;
	}
	void Video::DecodeTime(float dt, float speedFactor)
	{
		time_per_frame = 1.0f / mFPS;  // Standard time per frame based on FPS

		// Adjust the time per frame based on the speed factor (this reduces the time interval between frames)
		time_per_frame /= speedFactor; // If speedFactor = 2, then time_per_frame is halved, i.e., 2x speed

		// Increment last frame time by dt (with speed factor applied)
		last_frame_time += dt;

		// Skip frames and only process frames when enough time has passed
		while (last_frame_time >= time_per_frame)
		{
			// Read frames from the stream
			while (av_read_frame(formatContext, packet) >= 0)
			{
				if (packet->stream_index == videoStream->index)
				{
					avcodec_send_packet(codecContext, packet);

					// Process only the first frame (skip subsequent frames within the same time slot)
					if (avcodec_receive_frame(codecContext, frame) == 0)
					{
						// Convert frame to the desired format
						sws_scale(swsContext, frame->data, frame->linesize, 0, mHeight, RGBframe->data, RGBframe->linesize);

						// Update the texture with the new frame data
						mTexture->Update(RGBframe->data[0]);

						// Break to stop after processing just one frame (to skip others)
						break;
					}
				}
				av_packet_unref(packet);
			}

			// Reduce last_frame_time by time_per_frame for the next frame's calculation
			last_frame_time -= time_per_frame;
		}
	}

	bool Video::Decode(uint32_t frameIndex)
	{
		int fps = av_q2d(formatContext->streams[videoStream->index]->r_frame_rate);
		int64_t timestamp = av_rescale_q(frameIndex,  { 1, fps }, formatContext->streams[videoStream->index]->time_base);

		av_seek_frame(formatContext, videoStream->index, timestamp, AVSEEK_FLAG_BACKWARD);
		avcodec_flush_buffers(codecContext);

		AVPacket pkt;
		bool frameLoaded = false;

		while (av_read_frame(formatContext, &pkt) >= 0) {
			if (pkt.stream_index == videoStream->index) {
				if (avcodec_send_packet(codecContext, &pkt) == 0) {
					while (avcodec_receive_frame(codecContext, frame) == 0) {
						// Recalculate frame PTS (if needed)
						int64_t current_frame = av_rescale_q(frame->pts, formatContext->streams[videoStream->index]->time_base, { 1, mFPS });
						if (current_frame >= frameIndex) {
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

