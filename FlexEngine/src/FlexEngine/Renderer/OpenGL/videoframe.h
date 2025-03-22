#pragma once
#include "flx_api.h"
#include "Utilities/file.h"

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
  #include <libavutil/imgutils.h>
}

namespace FlexEngine
{
  struct __FLX_API Frame
  {
    uint8_t** data = nullptr;
    int width, height;
  };

  class __FLX_API VideoFrame {
  public:
    VideoFrame() = default;
    ~VideoFrame();

    bool Load(const Path& filepath);
    bool GetCurrentFrame(uint8_t* outputRGB);
    bool GetNextFrame(uint8_t* outputRGB);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetLength() const { return m_length; }
    int GetTotalFrames() const { return m_totalframes; }

  private:
    std::string m_filepath;
    int m_width = 0;
    int m_height = 0;
    int m_totalframes = 0;
    float m_length = 0; //in seconds
    int m_video_stream_index = -1;

    // FFmpeg Contexts
    AVFormatContext* m_format_ctx = nullptr;
    AVCodecContext* m_codec_ctx = nullptr;
    SwsContext* m_sws_ctx = nullptr;
    AVFrame* m_frame = nullptr;
    AVPacket* m_packet = nullptr;

    //Note that libavcodec only accepts arrays, even if its just one element in the array, therefore **char
    uint8_t* m_flipped_data = nullptr; //array in memory for us to do flipping operations 
    uint8_t** m_rgb_data = nullptr;
    int* m_line_size = 0;

    Frame m_frame_to_render;

    bool DecodeFrame(uint8_t* outputRGB);
  };

}
