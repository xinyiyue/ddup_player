#ifndef __FFMPEG_DECODER_H_H__
#define __FFMPEG_DECODER_H_H__

#include <pthread.h>

#include <string>

#include "player/decoder.h"
#include "player/stream.h"

class Stream;

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavcodec/avcodec.h"
}

class FFmpegDecoder : public Decoder {
 public:
  // codec_param is AVCodecParameters struct type.
  FFmpegDecoder(Stream *stream, void *codec_param) : Decoder(stream) {
    codec_param_ = (AVCodecParameters *)codec_param;
  };

  virtual ~FFmpegDecoder() = default;

  virtual int open() final;
  virtual int decode(void *data) final;
  virtual int flush() final;
  virtual int close() final;

 private:
  AVCodecContext *dec_ctx_;
  AVCodecParameters *codec_param_;
};

#endif
