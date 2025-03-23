#include "videoframe.h"
#include <FlexEngine.h>



namespace FlexEngine
{

	VideoFrame::~VideoFrame()
	{
		if(m_rgba_data[0]) delete m_rgba_data[0];
		if (m_rgba_data) delete m_rgba_data;
		if (m_line_size) delete m_line_size;

		if (m_packet) av_packet_free(&m_packet);
		if (m_frame) av_frame_free(&m_frame);
		if (m_format_ctx) avformat_close_input(&m_format_ctx);
		
	}

	// Open file, find stream, initialize decoder
	bool VideoFrame::Load(const Path& path)
	{
		// Intialize decoeders
		m_format_ctx = avformat_alloc_context();
		if (avformat_open_input(&m_format_ctx, path.string().c_str(), nullptr, nullptr) != 0)
		{
			Log::Error("Could not open video file!");
			return -1;
		}

		// Get video info
		if (avformat_find_stream_info(m_format_ctx, nullptr) < 0)
		{
			Log::Error("Could not retrieve stream info!");
			return -1;
		}
		//Save video info:
		m_length = m_format_ctx->duration / AV_TIME_BASE;

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
			return -1;
		}

		AVCodecParameters* codec_params = m_format_ctx->streams[m_video_stream_index]->codecpar;
		const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
		m_codec_ctx = avcodec_alloc_context3(codec);
		
		if (!codec || !m_codec_ctx)
		{
			Log::Error("Could not find codec!");
			return -1;
		}
		// Copy codec parameters
		if (avcodec_parameters_to_context(m_codec_ctx, codec_params) < 0)
		{
			Log::Error("Could not copy codec parameters!");
			return -1;
		}

		// Open the codec
		if (avcodec_open2(m_codec_ctx, codec, nullptr) < 0)
		{
			Log::Error("Could not open codec!");
			return -1;
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
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for pixel art, find some way to toggle this for non-pixel art
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload pixels into opengl texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		#pragma endregion

		return true;
	}

	//Returns current frame
	bool VideoFrame::GetCurrentFrame(uint8_t* m_rgba_data)
	{
		return false;
	}

	// Decodes and returns next frame
	bool VideoFrame::GetNextFrame(uint8_t* m_rgba_data)
	{
		return false;
	}

	void VideoFrame::Bind(const Asset::Shader& shader, const char* name, unsigned int texture_unit) const
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);

		std::string texture_name = name;
		texture_name += std::to_string(texture_unit);
		glUniform1i(glGetUniformLocation(shader.Get(), texture_name.c_str()), texture_unit);

		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_rgba_data[0]);
	}


	bool VideoFrame::DecodeNextFrame()
	{
		if (av_read_frame(m_format_ctx, m_packet) >= 0)
		{
			if (m_packet->stream_index == m_video_stream_index)
			{
				// Send packet to decoder
				if (avcodec_send_packet(m_codec_ctx, m_packet) == 0)
				{
					while (avcodec_receive_frame(m_codec_ctx, m_frame) == 0)
					{
						// Convert the frame to RGBA format
						sws_scale(m_sws_ctx, m_frame->data, m_frame->linesize, 0, m_codec_ctx->height, m_rgba_data, m_line_size);
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

						// Compute presentation time for this frame:
						double timebase = av_q2d(m_format_ctx->streams[m_video_stream_index]->time_base);
						m_next_frame_time = m_frame->pts * timebase;

						// Upload texture
						//glBindTexture(GL_TEXTURE_2D, videoTexture);
						//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, codecCtx->width, codecCtx->height, GL_RGBA, GL_UNSIGNED_BYTE, flippedData);
						
						av_packet_unref(m_packet);
						return true;
					}
				}
			}
			av_packet_unref(m_packet);
		}
		
		
		//Todo error handling bc idk what 
		return false;
	}
}
