
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

int FFmpegAudioProcesser::config() { return 0; }

int FFmpegAudioProcesser::process(void *data, void **out) {
  AVFrame *frame = (AVFrame *)data;
  render_buffer_s *dst_buff =
      (render_buffer_s *)malloc(sizeof(render_buffer_s));
  dst_buff->data = (char *)malloc(10);
  *out = dst_buff;
  av_frame_unref(frame);
  av_frame_free(&frame);
  return 0;
}

int FFmpegAudioProcesser::free_data(void *data) {
  render_buffer_s *buff = (render_buffer_s *)data;
  LOGD(TAGA, "audio processer free render buff:%p", buff);
  if (buff->data) {
    free(buff->data);
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
    case AV_PIX_FMT_ARGB:
      pixel = PIXELFORMAT_ARGB8888;
      break;
    case AV_PIX_FMT_RGBA:
      pixel = PIXELFORMAT_BGRA8888;
      break;
    case AV_PIX_FMT_NV12:
      pixel = PIXELFORMAT_NV12;
      break;
    case AV_PIX_FMT_NV21:
      pixel = PIXELFORMAT_NV21;
      break;
    default:
      pixel = PIXELFORMAT_UNKNOWN;
      break;
  }
  LOGD(TAGV, "map ffmpeg fmt %s to ddup format:%d",
       av_get_pix_fmt_name((enum AVPixelFormat)ff_format), pixel);
  return pixel;
}

int FFmpegVideoProcesser::pixel_fmt_to_ff_format(pixel_format_t pixel) {
  int ff_format;
  switch (pixel) {
    case PIXELFORMAT_ARGB8888:
      ff_format = AV_PIX_FMT_ARGB;
      break;
    case PIXELFORMAT_BGRA8888:
      ff_format = AV_PIX_FMT_RGBA;
      break;
    case PIXELFORMAT_NV12:
      ff_format = AV_PIX_FMT_NV12;
      break;
    case PIXELFORMAT_NV21:
      ff_format = AV_PIX_FMT_NV21;
      break;
    default:
      ff_format = PIXELFORMAT_UNKNOWN;
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
  if (src_fmt_.pixel[0] != dst_fmt_.pixel[0] ||
      src_fmt_.width != dst_fmt_.width || src_fmt_.height != dst_fmt_.height) {
    int dst_ff_format = pixel_fmt_to_ff_format(dst_fmt_.pixel[0]);
    swsContext_ =
        sws_getContext(codec_param_->width, codec_param_->height,
                       (enum AVPixelFormat)codec_param_->format, dst_fmt_.width,
                       dst_fmt_.height, (enum AVPixelFormat)dst_ff_format,
                       SWS_BILINEAR, NULL, NULL, NULL);
    LOGI(TAGV, "need convert video: src fmt %s, dst fmt:%s,swsContext_:%p",
         av_get_pix_fmt_name((enum AVPixelFormat)codec_param_->format),
         av_get_pix_fmt_name((enum AVPixelFormat)dst_ff_format), swsContext_);
    sink_->set_negotiated_format(&dst_fmt_);
  }
  return 0;
}

int FFmpegVideoProcesser::process(void *data, void **out) {
  AVFrame *frame = (AVFrame *)data;
  render_buffer_s *dst_buff =
      (render_buffer_s *)malloc(sizeof(render_buffer_s));
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
    dst_buff->data = (char *)dst_data[0];
    dst_buff->len = data_length * 4;
    dst_buff->pixel = dst_fmt_.pixel[0];
    dst_buff->width = dst_fmt_.width;
    dst_buff->height = dst_fmt_.height;
    dst_buff->frame_rate = codec_param_->framerate.num;
    dst_buff->pts = frame->pts * av_q2d(frame->time_base) * 1000;
    if (DUMP_VIDEO_BUFFER == 1) {
      if (dump_file_index < 20) {
        std::string file_name = "/home/mi/data/ddup_player/res/dump_video_" +
                                std::to_string(dump_file_index) + ".argb";
        save_file(file_name.c_str(), dst_buff->data,
                  dst_buff->width * dst_buff->height * 4);
        dump_file_index++;
      }
    }
  } else {
    LOGI(TAGV, "%s", "copy buffer.....");
    uint8_t *dst_data[AV_NUM_DATA_POINTERS] = {NULL};
    int dst_linesize[AV_NUM_DATA_POINTERS] = {0};
    int dst_ff_format = pixel_fmt_to_ff_format(dst_fmt_.pixel[0]);
    av_image_alloc(dst_data, dst_linesize, codec_param_->width,
                   codec_param_->width,
                   (enum AVPixelFormat)codec_param_->format, 1);
    dst_buff->data = (char *)dst_data[0];
    int data_length = 0;
    for (int i = 0; i < AV_NUM_DATA_POINTERS; i++) {
      data_length += dst_linesize[i];
    }
    av_image_copy_to_buffer((uint8_t *)dst_data, data_length,
                            (const uint8_t **)(frame->data), frame->linesize,
                            (enum AVPixelFormat)codec_param_->format,
                            codec_param_->width, codec_param_->width, 1);
    dst_buff->len = data_length;
    dst_buff->width = codec_param_->width;
    dst_buff->height = codec_param_->height;
    dst_buff->frame_rate = codec_param_->framerate.num;
  }
  av_frame_unref(frame);
  av_frame_free(&frame);
  *out = dst_buff;
  return 0;
}

int FFmpegVideoProcesser::free_data(void *data) {
  render_buffer_s *buff = (render_buffer_s *)data;
  LOGD(TAGA, "video processer free render buff:%p", buff);
  if (buff->data) {
    free(buff->data);
  }
  free(buff);

  return 0;
}
