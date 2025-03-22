#include "videoframe.h"
#include <FlexEngine.h>



namespace FlexEngine
{

	VideoFrame::~VideoFrame()
	{
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


		m_video_stream_index = -1;

		// Loop through streams to find a video stream
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
			m_codec_ctx->width, m_codec_ctx->height, AV_PIX_FMT_RGB24,
			SWS_BILINEAR, nullptr, nullptr, nullptr
		);

		// Intialize buffer for video frame data
		m_rgb_data[1] = new uint8_t[m_width * m_height * 3]; //Each pixel has RGB, so x3
		m_line_size = new int {m_width * 3};
		m_flipped_data = new uint8_t[m_width * m_height * 3];

		// Init packet and frames
		m_packet = av_packet_alloc();
		m_frame = av_frame_alloc();

		return false;
	}

	//Returns current frame
	bool VideoFrame::GetCurrentFrame(uint8_t* outputRGB)
	{
		return false;
	}

	// Decodes and returns next frame
	bool VideoFrame::GetNextFrame(uint8_t* output_rgb)
	{
		if (DecodeFrame(output_rgb))
		{
			return true;
		}
		else return false;
	}


	bool VideoFrame::DecodeFrame(uint8_t* output_rgb)
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
						// Convert the frame to RGB format
						sws_scale(m_sws_ctx, m_frame->data, m_frame->linesize, 0, m_codec_ctx->height, m_rgb_data, m_line_size);
						// Flip image manually

						for (int y = 0; y < m_codec_ctx->height; y++)
						{
							memcpy(m_flipped_data + y * m_line_size[0], m_rgb_data[0] + (m_codec_ctx->height - 1 - y) * m_line_size[0], m_line_size[0]);
						}

						//Replace buffered data with flipped data
						memcpy(m_rgb_data[0], m_flipped_data, m_line_size[0] * m_codec_ctx->height);
						memcpy(output_rgb, m_flipped_data, m_line_size[0] * m_codec_ctx->height);

						m_frame_to_render.data = m_rgb_data;
						m_frame_to_render.width = m_width;
						m_frame_to_render.height = m_height;

						// Upload texture
						//glBindTexture(GL_TEXTURE_2D, videoTexture);
						//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, codecCtx->width, codecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, flippedData);
					}
				}
			}
			av_packet_unref(m_packet);
		}
		
		
		//Todo error handling bc idk what 
		return false;
	}
}
