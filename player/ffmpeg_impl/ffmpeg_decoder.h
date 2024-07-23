#ifndef __FFMPEG_DECODER_H_H__
#define __FFMPEG_DECODER_H_H__

#include <string>

#include "player/decoder.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class FFmpegDecoder : public Decoder {
 public:
  // codec_param is AVCodecParameters struct type.
  FFmpegDecoder(void *ff_stream, EventListener *listener) : Decoder(listener) {
    stream_ = (AVStream *)ff_stream;
    codec_param_ = stream_->codecpar;
  };

  virtual ~FFmpegDecoder() = default;

  virtual int open() final;
  virtual int decode(void *data, out_cb cb, void *arg) final;
  virtual int close() final;

 private:
  AVCodecContext *dec_ctx_;
  AVCodecParameters *codec_param_;
  AVStream *stream_;
};

#endif
