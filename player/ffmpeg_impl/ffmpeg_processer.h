#ifndef __FFMPEG_PROCESSER_H__
#define __FFMPEG_PROCESSER_H__

#include "player/processer.h"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavcodec/avcodec.h"
}

class FFmpegAudioProcesser : public Processer {
 public:
  FFmpegAudioProcesser(processer_type_t type, void *codec_param);
  int free_data(void *data) final;
  virtual int process(void *data) final;

 private:
  AVCodecParameters *codec_param_;
};

class FFmpegVideoProcesser : public Processer {
 public:
  FFmpegVideoProcesser(processer_type_t type, void *codec_param);
  int free_data(void *data) final;
  virtual int process(void *data) final;

 private:
  AVCodecParameters *codec_param_;
};

#endif
