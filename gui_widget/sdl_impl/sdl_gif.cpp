
#include "gui_widget/sdl_impl/sdl_gif.h"

#include "gui_widget/sdl_impl/sdl_user_event.h"
#include "log/ddup_log.h"

#define TAG "SdlGif"

SdlGif::SdlGif(const char *name, const char *url, SDL_mutex *renderer_mutex,
               SDL_Renderer *renderer, int x, int y, int w, int h)
    : Widget(name),
      SdlTextureBuilder("SdlVideoTextureBuilder", renderer_mutex, renderer, x,
                        y, w, h) {
  url_ = url;
  rect_ = {x, y, w, h};
  renderer_ = renderer;
  exit_ = false;
  speed_ = 1.0;
}

SdlGif::~SdlGif() {
  exit_ = true;
  if (render_thread_id_.joinable()) render_thread_id_.join();
  for (int i = 0; i < raw_buffer_.size(); i++) {
    free(raw_buffer_[i]);
    raw_buffer_[i] = nullptr;
  }
}

int SdlGif::decode_gif() {
  AVFormatContext *fmt_ctx = NULL;
  int video_stream_index = -1;
  AVCodecContext *dec_ctx = NULL;
  AVCodec *dec = NULL;
  AVFrame *frame = NULL;
  AVPacket *pkt = NULL;
  int response;
  // 打开输入文件
  if (avformat_open_input(&fmt_ctx, url_.c_str(), NULL, NULL) != 0) {
    LOGE(TAG, "Could not open input file '%s'", url_.c_str());
    return -1;
  }

  // 检索流信息
  if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
    LOGE(TAG, "%s", "Could not find stream information");
    return -1;
  }

  // 查找视频流
  for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
    if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
      break;
    }
  }
  if (video_stream_index == -1) {
    LOGE(TAG, "%s", "Could not find a video stream");
    return -1;
  }

  // 查找解码器
  dec = (AVCodec *)avcodec_find_decoder(
      fmt_ctx->streams[video_stream_index]->codecpar->codec_id);
  if (dec == NULL) {
    LOGE(TAG, "%s", "Unsupported codec!");
    return -1;
  }

  // 分配解码器上下文
  dec_ctx = avcodec_alloc_context3(dec);
  if (!dec_ctx) {
    LOGE(TAG, "%s", "Failed to allocate the codec context");
    return -1;
  }

  // 复制编解码器参数到解码器上下文
  if (avcodec_parameters_to_context(
          dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar) < 0) {
    LOGE(TAG, "%s", "Failed to copy codec parameters to decoder context");
    return -1;
  }

  // 打开解码器
  if (avcodec_open2(dec_ctx, dec, NULL) < 0) {
    LOGE(TAG, "%s", "Could not open codec");
    return -1;
  }

  frame_h_ = fmt_ctx->streams[video_stream_index]->codecpar->height;
  frame_w_ = fmt_ctx->streams[video_stream_index]->codecpar->width;
  AVRational fps = fmt_ctx->streams[video_stream_index]->avg_frame_rate;
  if (fps.num && fps.den)
    frame_rate_ = fps.num / fps.den;
  else
    frame_rate_ = 30;
  LOGI(TAG, "codec:%s w:%d, h:%d, fps:%d", dec_ctx->codec_descriptor->name,
       frame_w_, frame_h_, frame_rate_);

  // 分配AVFrame
  frame = av_frame_alloc();
  if (!frame) {
    LOGE(TAG, "%s", "Could not allocate AVFrame");
    return -1;
  }

  // 分配AVPacket
  pkt = av_packet_alloc();
  if (!pkt) {
    LOGE(TAG, "%s", "Failed to allocate AVPacket");
    return -1;
  }

  for (int i = 0;; i++) {
    // 读取帧
    response = av_read_frame(fmt_ctx, pkt);
    if (response < 0) break;

    if (pkt->stream_index == video_stream_index) {
      // 解码帧
      response = avcodec_send_packet(dec_ctx, pkt);
      if (response < 0) {
        LOGE(TAG, "Error while sending a packet to the decoder: %d", response);
        break;
      }

      while (response >= 0) {
        response = avcodec_receive_frame(dec_ctx, frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
          break;
        else if (response < 0) {
          LOGE(TAG, "Error while receiving a frame from the decoder: %d",
               response);
          goto end;
        }

        // 检查帧是否为YUV420P格式
        if (frame->format != AV_PIX_FMT_YUV420P) {
          LOGI(TAG, "Frame is not in YUV420P format:%s, codec:%s",
               av_get_pix_fmt_name((enum AVPixelFormat)frame->format),
               dec_ctx->codec_descriptor->name);
          // goto end;
        }
        render_buffer_s *buf = nullptr;
        int ret = convert_data(frame, &buf);
        if (!ret && buf) {
          raw_buffer_.push_back(buf);
        }
      }
    }
    av_packet_unref(pkt);
    av_frame_unref(frame);
  }
end:
  // 释放资源
  av_frame_free(&frame);
  av_packet_free(&pkt);
  avcodec_free_context(&dec_ctx);
  avformat_close_input(&fmt_ctx);
  return 0;
}

enum AVPixelFormat SdlGif::pixel_fmt_to_ff_format(pixel_format_t pixel) {
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
  LOGD(TAG, "map ddup fmt %d to ffmpeg format:%s", pixel,
       av_get_pix_fmt_name((enum AVPixelFormat)ff_format));
  return ff_format;
}

int SdlGif::convert_data(AVFrame *frame, render_buffer_s **out_buff) {
  video_format_s support_fmt;
  video_format_s dst_fmt;
  get_supported_format(&support_fmt);
  bool need_convert = true;
  for (int i = 0; i < SUPPORTED_PIXEL_FORMAT_COUNT; i++) {
    int dst_ff_format = pixel_fmt_to_ff_format(support_fmt.pixel[i]);
    if (frame->format == dst_ff_format) {
      LOGD(TAG, "match format: src fmt %s, dst fmt:%s",
           av_get_pix_fmt_name((enum AVPixelFormat)frame->format),
           av_get_pix_fmt_name((enum AVPixelFormat)dst_ff_format));
      need_convert = false;
      dst_fmt.pixel[0] = support_fmt.pixel[i];
      set_negotiated_format(&dst_fmt);
    }
  }

  if (need_convert) {
    LOGE(TAG, "not support format: src fmt %s",
         av_get_pix_fmt_name((enum AVPixelFormat)frame->format));
    return -1;
  }

  render_buffer_s *dst_buff =
      (render_buffer_s *)malloc(sizeof(render_buffer_s));
  memset(dst_buff, 0, sizeof(render_buffer_s));

  for (int i = 0; i < 3; i++) {
    dst_buff->len[i] = frame->linesize[i];
    dst_buff->data[i] = (char *)av_malloc(dst_buff->len[i] * frame_h_);
    memcpy(dst_buff->data[i], frame->data[i], dst_buff->len[i] * frame_h_);
    LOGD(TAG, "alloc len[%d]:%d, data[%d]:%p, linesize[%d]:%d", i,
         dst_buff->len[i], i, dst_buff->data[i], i, frame->linesize[i]);
  }
  dst_buff->pixel = dst_fmt.pixel[0];
  dst_buff->width = frame_w_;
  dst_buff->height = frame_h_;
  dst_buff->frame_rate = frame_rate_;
  *out_buff = dst_buff;
  return 0;
}

void SdlGif::render_thread(void) {
  int index = 0;
  int size = raw_buffer_.size();
  while (!exit_) {
    render_buffer_s *buff = raw_buffer_[index];
    build_texture(buff);
    int sleep_time = 1000 / buff->frame_rate;
    SDL_Delay(sleep_time);  // 50ms
    if (speed_ > 0.0f) index++;
    if (index >= size) index = 0;
  }
}

int SdlGif::render_gif() {
  LOGI(TAG, "%s", "create render thread");
  render_thread_id_ = std::thread(std::bind(&SdlGif::render_thread, this));
  return 0;
}

int SdlGif::set_speed(float speed) {
  LOGI(TAG, "%s", "create render thread");
  speed_ = speed;
  return 0;
}

int SdlGif::draw() { return render_texture(); }

int SdlGif::get_type() { return 0; }

int SdlGif::event_handler(void *event) {
  SDL_Event *e = (SDL_Event *)event;
  if (e->type == SDL_USER_EVENT_VIDEO_UPDATE) {
    dirty_ = true;
    LOGD(TAG, "%s", "catch event video update event, will update gif picture");
    return 1;
  }
  return 0;
}

int SdlGif::set_event_resp_area(int x, int y, int w, int h) { return 0; }

void *SdlGif::get_window() { return nullptr; }

void *SdlGif::get_renderer() { return nullptr; }
