#ifndef __FFMPEG_PROCESSER_H__
#define __FFMPEG_PROCESSER_H__

#include "player/buffer.h"
#include "player/processer.h"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavcodec/avcodec.h"
#include "third_party/ffmpeg-7.0.1/include/libavutil/imgutils.h"
#include "third_party/ffmpeg-7.0.1/include/libswresample/swresample.h"
#include "third_party/ffmpeg-7.0.1/include/libswscale/swscale.h"
}

class FFmpegAudioProcesser : public Processer {
 public:
  FFmpegAudioProcesser(processer_type_t type, void *codec_param,
                       EventListener *listener);
  int free_data(void *data) final;
  virtual int process(void *data, void **out) final;

 private:
  virtual int config() final;
  AVCodecParameters *codec_param_;
  SwrContext *swr_ctx = NULL;
};

class FFmpegVideoProcesser : public Processer {
 public:
  FFmpegVideoProcesser(processer_type_t type, void *codec_param,
                       EventListener *listener);
  int free_data(void *data) final;
  virtual int process(void *data, void **out) final;
  pixel_format_t ff_fmt_to_pixel_fmt(int ff_format);
  int pixel_fmt_to_ff_format(pixel_format_t pixel);

 private:
  virtual int config() final;
  AVCodecParameters *codec_param_;
  struct SwsContext *swsContext_;
};

#endif
