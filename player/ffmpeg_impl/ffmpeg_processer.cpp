
#include "player/ffmpeg_impl/ffmpeg_processer.h"

#include "log/ddup_log.h"

#define TAGA "FFmpegAudioProcesser"
#define TAGV "FFmpegVideoProcesser"

FFmpegAudioProcesser::FFmpegAudioProcesser(processer_type_t type,
                                           void *codec_param)
    : Processer(type, codec_param) {
  codec_param_ = (AVCodecParameters *)codec_param;
}

int FFmpegAudioProcesser::process(void *data) { return 0; }

int FFmpegAudioProcesser::free_data(void *data) {
  AVFrame *frame = (AVFrame *)data;
  LOGD(TAGA, "audio processer free frame:%p", frame);
  av_frame_unref(frame);
  av_frame_free(&frame);
  return 0;
}

FFmpegVideoProcesser::FFmpegVideoProcesser(processer_type_t type,
                                           void *codec_param)
    : Processer(type, codec_param) {
  codec_param_ = (AVCodecParameters *)codec_param;
}

int FFmpegVideoProcesser::process(void *data) { return 0; }

int FFmpegVideoProcesser::free_data(void *data) {
  AVFrame *frame = (AVFrame *)data;
  LOGD(TAGV, "video processer free frame:%p", frame);
  av_frame_unref(frame);
  av_frame_free(&frame);
  return 0;
}
