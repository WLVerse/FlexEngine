/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// videodecoder.cpp
//
// Video playback within engine.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "videodecoder.h"
#include <FlexEngine.h>



namespace FlexEngine
{

	VideoDecoder::~VideoDecoder()
	{
		if (m_rgba_data)
		{
			delete m_rgba_data[0];
			delete m_rgba_data;
		}
		if (m_line_size) delete m_line_size;

		if (m_packet) av_packet_free(&m_packet);
		if (m_frame) av_frame_free(&m_frame);
		if (m_format_ctx) avformat_close_input(&m_format_ctx);
		
	}

	// Open file, find stream, initialize decoder
	bool VideoDecoder::Load(const Path& path)
	{
		// Intialize decoeders
		m_format_ctx = avformat_alloc_context();
		if (avformat_open_input(&m_format_ctx, path.string().c_str(), nullptr, nullptr) != 0)
		{
			Log::Error("Could not open video file!");
			return false;
		}

		// Get video info
		if (avformat_find_stream_info(m_format_ctx, nullptr) < 0)
		{
			Log::Error("Could not retrieve stream info!");
			return false;
		}

		// Loop through streams to find a video stream
		m_video_stream_index = -1;
		for (unsigned int i = 0; i < m_format_ctx->nb_streams; i++)
		{
			if (m_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				m_video_stream_index = i;
				break;
			}
		}
		if (m_video_stream_index == -1)
		{
			Log::Error("No video stream found!");
			return false;
		}

		//Save video info - length and framerate
		m_length = static_cast<float>(m_format_ctx->duration) / AV_TIME_BASE;
		
		//Find out framerate of video
		//AVStream* video_stream = m_format_ctx->streams[m_video_stream_index];
		//// Try using avg_frame_rate first
		//if (video_stream->avg_frame_rate.num != 0 && video_stream->avg_frame_rate.den != 0) 
		//{
		//	m_framerate = av_q2d(video_stream->avg_frame_rate);
		//}
		//// If avg_frame_rate is unavailable, fallback to r_frame_rate
		//else if (video_stream->r_frame_rate.num != 0 && video_stream->r_frame_rate.den != 0) 
		//{
		//	m_framerate = av_q2d(video_stream->r_frame_rate);
		//}
		//// If no frame rate found, then just uhh default 30 lmao
		//else 
		//{
		//	m_framerate = 30.0;
		//}

		// Find a codec that works for our video
		AVCodecParameters* codec_params = m_format_ctx->streams[m_video_stream_index]->codecpar;
		const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
		m_codec_ctx = avcodec_alloc_context3(codec);
		
		if (!codec || !m_codec_ctx)
		{
			Log::Error("Could not find codec!");
			return false;
		}
		// Copy codec parameters
		if (avcodec_parameters_to_context(m_codec_ctx, codec_params) < 0)
		{
			Log::Error("Could not copy codec parameters!");
			return false;
		}

		// Open the codec
		if (avcodec_open2(m_codec_ctx, codec, nullptr) < 0)
		{
			Log::Error("Could not open codec!");
			return false;
		}

		//Save video resolution
		m_width = m_codec_ctx->width;
		m_height= m_codec_ctx->height;

		m_sws_ctx = sws_getContext(
			m_codec_ctx->width, m_codec_ctx->height, m_codec_ctx->pix_fmt,
			m_codec_ctx->width, m_codec_ctx->height, AV_PIX_FMT_RGBA,
			SWS_BILINEAR, nullptr, nullptr, nullptr
		);

		// Intialize buffers for video frame data
		m_rgba_data = new uint8_t* [1];	//allocate an array that can store one pointer
		m_rgba_data[0] = new uint8_t[m_width * m_height * 4]; //Each pixel has RGBA channel, so x4
		m_line_size = new int{ m_width * 4 }; // Width * bytes per pixel (4 for RGBA)

		//m_flipped_data = new uint8_t[m_width * m_height * 4];

		// Init packet and frames
		m_packet = av_packet_alloc();
		m_frame = av_frame_alloc();

		#pragma region OpenGL 
		// Create a OpenGL texture identifier
		glGenTextures(1, &m_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for pixel art, find some way to toggle this for non-pixel art
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload pixels into opengl texture
		if (!DecodeNextFrame()) Log::Error("Video first frame Decode failed");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_rgba_data[0]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		#pragma endregion

		return true;
	}

	void VideoDecoder::Bind(const Asset::Shader& shader, const char* name, unsigned int texture_unit) const
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);

		std::string texture_name = name;
		texture_name += std::to_string(texture_unit);
		glUniform1i(glGetUniformLocation(shader.Get(), texture_name.c_str()), texture_unit);

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_rgba_data[0]);
	}

	bool VideoDecoder::Seek(double time)
	{
		if (!m_format_ctx || m_video_stream_index == -1) return false;

		// Convert time in seconds to target frame
		double time_base = av_q2d(m_format_ctx->streams[m_video_stream_index]->time_base);
		int64_t target_timestamp = static_cast<int64_t>(time / time_base);

		// Seek to the target timestamp
		if (av_seek_frame(m_format_ctx, m_video_stream_index, target_timestamp, AVSEEK_FLAG_BACKWARD) < 0) 
		{
			Log::Error("Seek to time " + std::to_string(time_base) + " failed!");
			return false;
		}
		// Flush the codec buffers to clear old frames
		avcodec_flush_buffers(m_codec_ctx);

		// Reset frame timing
		m_current_time = time;
		m_next_frame_time = time;

		return true;
	}

	bool VideoDecoder::RestartVideo()
	{
		return Seek(0);
	}

	bool VideoDecoder::DecodeNextFrame()
	{
		if (av_read_frame(m_format_ctx, m_packet) >= 0)
		{
			if (m_packet->stream_index == m_video_stream_index)
			{
				if (avcodec_send_packet(m_codec_ctx, m_packet) == 0)
				{
					while (avcodec_receive_frame(m_codec_ctx, m_frame) == 0)
					{
						// Convert the frame to RGBA format
						sws_scale(m_sws_ctx, m_frame->data, m_frame->linesize, 0, m_codec_ctx->height, m_rgba_data, m_line_size);
						
						// Compute presentation time for this frame:
						double time_base = av_q2d(m_format_ctx->streams[m_video_stream_index]->time_base);
						m_next_frame_time = m_frame->pts * time_base;
						
						av_packet_unref(m_packet);
						return true;
					}
				}
			}
			av_packet_unref(m_packet);
		}
		else
		{
			return false;	//error, or EOF
		}
		
		//Todo error handling bc idk what 
		return true;
	}
}



//bool VideoDecoder::GetCurrentFrame(uint8_t* m_rgba_data)
//{
//	return false;
//}
//
//bool VideoDecoder::GetNextFrame(uint8_t* m_rgba_data)
//{
//	return false;
//}

// Flip image manually

						//for (int y = 0; y < m_codec_ctx->height; y++)
						//{
						//	memcpy(m_flipped_data + y * m_line_size[0], m_rgba_data[0] + (m_codec_ctx->height - 1 - y) * m_line_size[0], m_line_size[0]);
						//}

						////Replace buffered data with flipped data
						//memcpy(m_rgba_data[0], m_flipped_data, m_line_size[0] * m_codec_ctx->height);

						//m_frame_to_render.data = m_rgba_data;
						//m_frame_to_render.width = m_width;
						//m_frame_to_render.height = m_height;