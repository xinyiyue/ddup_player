
#include "player/ffmpeg_impl/ffmpeg_processer.h"

#include <string>

#include "log/ddup_log.h"

#define TAGA "FFmpegAudioProcesser"
#define TAGV "FFmpegVideoProcesser"

static int dump_file_index = 0;
#define DUMP_VIDEO_BUFFER 0
FFmpegAudioProcesser::FFmpegAudioProcesser(processer_type_t type,
                                           void *codec_param,
                                           EventListener *listener)
    : Processer(type, listener) {
  codec_param_ = (AVCodecParameters *)codec_param;
}

int FFmpegAudioProcesser::config() {
  sink_->get_supported_format(&adst_fmt_);

  if (asrc_fmt_.channel_number != adst_fmt_.channel_number ||
      asrc_fmt_.sample_rate != adst_fmt_.sample_rate ||
      asrc_fmt_.sample_format != adst_fmt_.sample_format) {
    AVChannelLayout out_ch_layout;
    av_channel_layout_copy(&out_ch_layout, &codec_param_->ch_layout);

    swr_free(&swr_ctx);

    int ret = swr_alloc_set_opts2(
        &swr_ctx, &out_ch_layout, AV_SAMPLE_FMT_S16, adst_fmt_.sample_rate,
        &codec_param_->ch_layout, (enum AVSampleFormat)codec_param_->format,
        codec_param_->sample_rate, 0, NULL);

    if (ret < 0 || swr_init(swr_ctx) < 0) {
      swr_free(&swr_ctx);
      return -1;
    }
  }

  sink_->set_negotiated_format(&adst_fmt_);
  return 0;
}

int FFmpegAudioProcesser::process(void *data, void **out) {
  int out_size = 0;
  AVFrame *frame = (AVFrame *)data;
  render_buffer_s *dst_buff =
      (render_buffer_s *)malloc(sizeof(render_buffer_s));
  memset(dst_buff, 0, sizeof(render_buffer_s));

  if (swr_ctx) {
    const uint8_t **in = (const uint8_t **)frame->extended_data;
    uint8_t **out = (uint8_t **)&dst_buff->data[0];
    int out_count = frame->nb_samples;
    out_size = av_samples_get_buffer_size(NULL, frame->ch_layout.nb_channels,
                                          out_count, AV_SAMPLE_FMT_S16, 0);

    dst_buff->data[0] = (char *)malloc(out_size * sizeof(uint8_t));
    if (dst_buff->data[0] == NULL) {
      goto out;
    }

    int ret = swr_convert(swr_ctx, out, out_count, in, frame->nb_samples);
    if (ret < 0) {
      swr_free(&swr_ctx);
      goto out;
    }
    dst_buff->len[0] = out_size;
  } else {
    out_size = frame->linesize[0];
    dst_buff->data[0] = (char *)malloc(out_size * sizeof(uint8_t));
    if (dst_buff->data != NULL) {
      memcpy(dst_buff->data, frame->data[0], out_size);
      dst_buff->len[0] = out_size;
    }
  }

out:
  *out = dst_buff;
  AVRational base_ms = {1, 1000};
  dst_buff->pts = av_q2d(frame->time_base) * frame->pts * 1000;
  LOGD(TAGA, "convert audio frame time_base:%d, %d, pts:%lld to ms:%lld",
       frame->time_base.num, frame->time_base.den, frame->pts, dst_buff->pts);
  av_frame_unref(frame);
  av_frame_free(&frame);
  return 0;
}

int FFmpegAudioProcesser::free_data(void *data) {
  render_buffer_s *buff = (render_buffer_s *)data;
  LOGD(TAGA, "audio processer free render buff:%p", buff);
  for (int i = 0; i < 4; ++i) {
    if (buff->data[i]) free(buff->data[i]);
  }
  free(buff);
  return 0;
}

static int save_file(const char *name, char *in_buffer, int len) {
  FILE *file;
  long file_size;

  // 打开文件
  file = fopen(name, "w");
  if (file == NULL) {
    perror("无法打开文件");
    return -1;
  }

  // 读取文件内容到缓冲区
  int ret = fwrite(in_buffer, sizeof(char), len, file);

  LOGE(TAGV, ">>>> write size:%d to file:%s", ret, name);
  // 关闭文件
  fclose(file);
  return ret;
}

pixel_format_t FFmpegVideoProcesser::ff_fmt_to_pixel_fmt(int ff_format) {
  pixel_format_t pixel;
  switch (ff_format) {
    case AV_PIX_FMT_RGB32:
      pixel = PIXELFORMAT_ARGB8888;
      break;
    case AV_PIX_FMT_BGR32_1:
      pixel = PIXELFORMAT_BGRA8888;
      break;
    case AV_PIX_FMT_YUV420P:
      pixel = PIXELFORMAT_IYUV;
      break;
    case AV_PIX_FMT_YUYV422:
      pixel = PIXELFORMAT_YUY2;
      break;
    case AV_PIX_FMT_UYVY422:
      pixel = PIXELFORMAT_UYVY;
      break;
    default:
      pixel = PIXELFORMAT_UNKNOWN;
      break;
  }
  LOGD(TAGV, "map ffmpeg fmt %s to ddup format:%d",
       av_get_pix_fmt_name((enum AVPixelFormat)ff_format), pixel);
  return pixel;
}

enum AVPixelFormat FFmpegVideoProcesser::pixel_fmt_to_ff_format(
    pixel_format_t pixel) {
  enum AVPixelFormat ff_format;
  switch (pixel) {
    case PIXELFORMAT_ARGB8888:
      ff_format = AV_PIX_FMT_RGB32;
      break;
    case PIXELFORMAT_BGRA8888:
      ff_format = AV_PIX_FMT_BGR32_1;
      break;
    case PIXELFORMAT_IYUV:
      ff_format = AV_PIX_FMT_YUV420P;
      break;
    case PIXELFORMAT_YUY2:
      ff_format = AV_PIX_FMT_YUYV422;
      break;
    case PIXELFORMAT_UYVY:
      ff_format = AV_PIX_FMT_UYVY422;
      break;
    default:
      ff_format = AV_PIX_FMT_NONE;
      break;
  }
  LOGD(TAGV, "map ddup fmt %d to ffmpeg format:%s", pixel,
       av_get_pix_fmt_name((enum AVPixelFormat)ff_format));
  return ff_format;
}

FFmpegVideoProcesser::FFmpegVideoProcesser(processer_type_t type,
                                           void *codec_param,
                                           EventListener *listener)
    : Processer(type, listener) {
  codec_param_ = (AVCodecParameters *)codec_param;
  src_fmt_.width = codec_param_->width;
  src_fmt_.height = codec_param_->height;
  src_fmt_.pixel[0] = ff_fmt_to_pixel_fmt(codec_param_->format);
  swsContext_ = nullptr;
}

int FFmpegVideoProcesser::config() {
  sink_->get_supported_format(&dst_fmt_);
  int dst_ff_format = pixel_fmt_to_ff_format(dst_fmt_.pixel[0]);
  if (src_fmt_.pixel[0] != dst_fmt_.pixel[0]) {
    swsContext_ =
        sws_getContext(codec_param_->width, codec_param_->height,
                       (enum AVPixelFormat)codec_param_->format, dst_fmt_.width,
                       dst_fmt_.height, (enum AVPixelFormat)dst_ff_format,
                       SWS_BILINEAR, NULL, NULL, NULL);
    LOGI(TAGV, "need convert video: src fmt %s, dst fmt:%s,swsContext_:%p",
         av_get_pix_fmt_name((enum AVPixelFormat)codec_param_->format),
         av_get_pix_fmt_name((enum AVPixelFormat)dst_ff_format), swsContext_);
    sink_->set_negotiated_format(&dst_fmt_);
  } else {
    LOGI(TAGV, "do not convert video: src fmt %s, dst fmt:%s",
         av_get_pix_fmt_name((enum AVPixelFormat)codec_param_->format),
         av_get_pix_fmt_name((enum AVPixelFormat)dst_ff_format));
  }
  return 0;
}

int FFmpegVideoProcesser::process(void *data, void **out) {
  AVFrame *frame = (AVFrame *)data;
  render_buffer_s *dst_buff =
      (render_buffer_s *)malloc(sizeof(render_buffer_s));
  memset(dst_buff, 0, sizeof(render_buffer_s));
  if (swsContext_) {
    uint8_t *dst_data[AV_NUM_DATA_POINTERS] = {NULL};
    int dst_linesize[AV_NUM_DATA_POINTERS] = {0};
    int dst_ff_format = pixel_fmt_to_ff_format(dst_fmt_.pixel[0]);

    av_image_alloc(dst_data, dst_linesize, dst_fmt_.width, dst_fmt_.height,
                   (enum AVPixelFormat)dst_ff_format, 1);
    LOGD(TAGV, "convert frame to dst_buff:%p, format:%d", dst_buff,
         dst_ff_format);
    sws_scale(swsContext_, frame->data, frame->linesize, 0,
              codec_param_->height, dst_data, dst_linesize);
    int data_length = 0;
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
      LOGD(TAGV, "dst_linesize[%d]:%d", i, dst_linesize[i]);
      data_length += dst_linesize[i];
    }
    dst_buff->data[0] = (char *)dst_data[0];
    dst_buff->len[0] = data_length * 4;
    dst_buff->pixel = dst_fmt_.pixel[0];
    dst_buff->width = dst_fmt_.width;
    dst_buff->height = dst_fmt_.height;
    dst_buff->frame_rate = codec_param_->framerate.num;
    AVRational base_ms = {1, 80000};
    dst_buff->pts = av_rescale_q(frame->pts, AV_TIME_BASE_Q, base_ms);
    LOGD(TAGV, "convert frame time_base:%d, %d, pts:%lld to ms:%lld",
         frame->time_base.num, frame->time_base.den, frame->pts, dst_buff->pts);
    if (DUMP_VIDEO_BUFFER == 1) {
      if (dump_file_index < 20) {
        std::string file_name = "/home/mi/data/ddup_player/res/dump_video_" +
                                std::to_string(dump_file_index) + ".argb";
        save_file(file_name.c_str(), dst_buff->data[0],
                  dst_buff->width * dst_buff->height * 4);
        dump_file_index++;
      }
    }
  } else {
    enum AVPixelFormat dst_ff_format =
        pixel_fmt_to_ff_format(dst_fmt_.pixel[0]);
    LOGD(TAGV, "copy frame, format:%s", av_get_pix_fmt_name(dst_ff_format));

    for (int i = 0; i < 3; i++) {
      dst_buff->len[i] = frame->linesize[i];
      dst_buff->data[i] =
          (char *)av_malloc(dst_buff->len[i] * codec_param_->height);
      memcpy(dst_buff->data[i], frame->data[i],
             dst_buff->len[i] * codec_param_->height);
      LOGD(TAGV, "alloc len[%d]:%d, data[%d]:%p, linesize[%d]:%d", i,
           dst_buff->len[i], i, dst_buff->data[i], i, frame->linesize[i]);
    }
    dst_buff->pixel = dst_fmt_.pixel[0];
    dst_buff->width = codec_param_->width;
    dst_buff->height = codec_param_->height;
    dst_buff->frame_rate = codec_param_->framerate.num;
  }
  av_frame_unref(frame);
  av_frame_free(&frame);
  LOGD(TAGV, "%s", "process dst buffer ok");
  *out = dst_buff;
  return 0;
}

int FFmpegVideoProcesser::free_data(void *data) {
  render_buffer_s *buff = (render_buffer_s *)data;
  LOGD(TAGA, "video processer free render buff:%p", buff);
  for (int i = 0; i < 4; ++i) {
    if (buff->data[i]) free(buff->data[i]);
  }
  free(buff);

  return 0;
}
