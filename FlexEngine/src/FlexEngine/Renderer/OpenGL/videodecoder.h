/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// videodecoder.h
//
// Video playback within engine.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

/*********************************
* What is supported:
* - Uhh video
*   > only tested with mp4, but theoretically, ffmpeg will automatically find a decoder
*     that works for the video file loaded.
* - Video seeking: go to xxx second in the video.
* 
* What is not supported, sory:
* - Audio from video
* - Multiple instances of the SAME video file at the same time
*********************************/

/*********************************
* How to use:
* - Attach Video Player component to entity
*   > Drop video file, set parameters
* - Dont attach sprite and animator components.
* - Recall the existence of the z-index component.
* - Pray it works
* - To restart video on scene load/exit, ownself need to settle in the layer (script dont work D:)
*********************************/

#pragma once
#include "opengltexture.h"
#include "Renderer/OpenGL/openglshader.h"
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
  class __FLX_API VideoDecoder {
  public:
    VideoDecoder() = default;
    ~VideoDecoder();

    bool Load(const Path& filepath);
    //bool GetCurrentFrame(uint8_t* outputRGB);
    //bool GetNextFrame(uint8_t* outputRGB);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetLength() const { return m_length; }
    int GetTotalFrames() const { return m_totalframes; }
    double GetNextFrameTime() const { return m_next_frame_time; }

    bool Seek(double time);
    bool RestartVideo();
    bool DecodeNextFrame();
    void Bind(const Asset::Shader& shader, const char* name, unsigned int texture_unit) const;

    double m_current_time = 0.0;

  private:
    std::string m_filepath; //filepath to video to decode
    int m_width = 0;
    int m_height = 0;
    float m_length = 0; //in seconds
    int m_video_stream_index = -1;
    long m_totalframes = 0;
    //int m_framerate = 0;  

    double m_next_frame_time = 0.0; // in seconds, when this frame should be shown

    // FFmpeg Contexts
    AVFormatContext* m_format_ctx = nullptr;
    AVCodecContext* m_codec_ctx = nullptr;
    SwsContext* m_sws_ctx = nullptr;
    AVFrame* m_frame = nullptr;
    AVPacket* m_packet = nullptr;

    //Note that libavcodec only accepts arrays, even if its just one element in the array, therefore **char
    //uint8_t* m_flipped_data = nullptr; //array in memory for us to do flipping operations 
    uint8_t** m_rgba_data = nullptr;
    int* m_line_size = nullptr;

    //Frame m_frame_to_render;

    unsigned int m_texture = 0;

  };

}
