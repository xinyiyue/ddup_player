#include "player/ffmpeg_impl/ffmpeg_decoder.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "FfmpegDecoder"

int FFmpegDecoder::open() {
  if (!codec_param_) {
    LOGE(TAG, "%s", "Codec info is null.");
    return -EINVAL;
  }

  const AVCodec *dec = avcodec_find_decoder(codec_param_->codec_id);
  if (!dec) {
    LOGE(TAG, "Failed to find %s codec.",
         av_get_media_type_string(codec_param_->codec_type));
    return -EINVAL;
  }

  dec_ctx_ = avcodec_alloc_context3(dec);
  if (!dec_ctx_) {
    LOGE(TAG, "Failed to allocate the %s codec context.",
         av_get_media_type_string(codec_param_->codec_type));
    return -ENOMEM;
  }

  LOGE(TAG, "create decoder:%s sucessfully ..............",
       dec_ctx_->codec->name);

  int ret = avcodec_parameters_to_context(dec_ctx_, codec_param_);
  if (ret < 0) {
    LOGE(TAG, "Failed to copy %s codec parameters to decoder context",
         av_get_media_type_string(codec_param_->codec_type));
    return ret;
  }

  ret = avcodec_open2(dec_ctx_, dec, NULL);
  if (ret < 0) {
    LOGE(TAG, "Failed to open %s codec",
         av_get_media_type_string(codec_param_->codec_type));
    return ret;
  }

  return 0;
}

int FFmpegDecoder::decode(void *data, out_cb cb, void *arg) {
  int ret = 0;
  bool frame_remain = true;

  AVPacket *pkt = (AVPacket *)data;
  LOGI(TAG, "decoder:%s will decode pkt:%p ..............",
       dec_ctx_->codec->name, pkt);
  ret = avcodec_send_packet(dec_ctx_, pkt);
  if (ret < 0) {
    LOGE(TAG, "Error submitting a packet for decoding (%d)", ret);
    return ret;
  }

  while (frame_remain) {
    AVFrame *frame = NULL;
    frame = av_frame_alloc();
    if (frame == NULL) {
      LOGE(TAG, "%s", "malloc frame failed.");
      return -ENOMEM;
    }

    ret = avcodec_receive_frame(dec_ctx_, frame);
    if (ret < 0) {
      // those two return values are special and mean there is no output
      // frame available, but there were no errors during decoding
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
        LOGD(TAG, "Ignnor error during decoding: %d", ret);
      }
      frame_remain = false;
      av_frame_free(&frame);
    } else {
      // process raw data
      LOGI(TAG, "decoder %s  decode out %s frame", dec_ctx_->codec->name, frame->width ? "video" : "audio");
      cb(frame, arg);
    }
  }
  if (ret == AVERROR_EOF) {
    ret = DECODE_ERROR_EOS;
  } else if (ret == AVERROR(EAGAIN)) {
    ret = DECODE_ERROR_EAGAIN;
  }
  return ret;
}

int FFmpegDecoder::close() {
  avcodec_free_context(&dec_ctx_);
  return 0;
}
