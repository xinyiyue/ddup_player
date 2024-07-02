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

  int ret = avcodec_parameters_to_context(dec_ctx_, codec_param_);
  if (ret < 0) {
    LOGE(TAG, "Failed to copy %s codec parameters to decoder context\n",
         av_get_media_type_string(codec_param_->codec_type));
    return ret;
  }

  ret = avcodec_open2(dec_ctx_, dec, NULL);
  if (ret < 0) {
    LOGE(TAG, "Failed to open %s codec\n",
         av_get_media_type_string(codec_param_->codec_type));
    return ret;
  }

  Decoder::open();

  return 0;
}

int FFmpegDecoder::decode(void *data) {
  int ret = 0;
  AVFrame *frame = NULL;
  AVPacket *pkt = (AVPacket *)data;
  ret = avcodec_send_packet(dec_ctx_, pkt);
  if (ret < 0) {
    LOGE(TAG, "Error submitting a packet for decoding (%d)\n", ret);
    return ret;
  }

  frame = av_frame_alloc();
  if (frame == NULL) {
    LOGE(TAG, "%s", "malloc frame failed.\n");
    return -ENOMEM;
  }

  // get all the available frames from the decoder
  while (ret >= 0) {
    ret = avcodec_receive_frame(dec_ctx_, frame);
    if (ret < 0) {
      // those two return values are special and mean there is no output
      // frame available, but there were no errors during decoding
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) goto out;

      LOGE(TAG, "Error during decoding %d\n", ret);
      break;
    }

    // todo: send decode frame to output fifo.
    LOGI(TAG, "decodc a %s frame.", frame->width ? "video" : "audio");

    av_frame_unref(frame);
  }

out:
  av_frame_free(&frame);

  return (ret == AVERROR(EAGAIN)) ? 0 : ret;
}

int FFmpegDecoder::flush() {
  // send null frame to decoder
  decode(NULL);
  return 0;
}


int FFmpegDecoder::close() {
  avcodec_free_context(&dec_ctx_);
  Decoder::close();
  return 0;
}

