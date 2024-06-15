#include "player/ffmpeg_impl/ffmpeg_demux.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "FfmpegDemux"

int FFmpegDemux::open() {
  int ret = 0;
  fmt_ctx_ = avformat_alloc_context();
  ret = avformat_open_input(&fmt_ctx_, (char *)url_.c_str(), NULL, NULL);
  if (ret < 0) {
    LOGE(TAG, "open input error:%d", ret);
    return ret;
  }

  ret = avformat_find_stream_info(fmt_ctx_, NULL);
  if (ret < 0) {
    LOGE(TAG, "find stream info error:%d", ret);
    return ret;
  }

  return 0;
}

int FFmpegDemux::create_stream() {
  int ret = 0;
  ret = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  if (ret < 0) {
    LOGE(TAG, "Could not find %s stream",
         av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    return ret;
  } else {
    video_stream_index_ = ret;
    AVStream *vstream = fmt_ctx_->streams[video_stream_index_];
    ret = CreateStream(VIDEO_STREAM, (void *)vstream, &video_stream_);
    if (ret < 0) {
      LOGE(TAG, "create video stream failed:%d", ret);
      return ret;
    }
  }

  ret = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  if (ret < 0) {
    LOGE(TAG, "Could not find %s stream",
         av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
    return ret;
  } else {
    audio_stream_index_ = ret;
    AVStream *astream = fmt_ctx_->streams[audio_stream_index_];
    ret = CreateStream(AUDIO_STREAM, (void *)astream, &audio_stream_);
    if (ret < 0) {
      LOGE(TAG, "create audio stream failed:%d", ret);
      return ret;
    }
  }
  request_input_data();
  return 0;
}

demux_event_t FFmpegDemux::read_input_data(av_data_s *data) {
  int ret;
  AVPacket *pkt = av_packet_alloc();
  if (!pkt) return DEMUX_ERROR;
  ret = av_read_frame(fmt_ctx_, pkt);
  if (ret < 0) {
    if (ret == AVERROR_EOF)
      return DEMUX_EOS;
    else
      return DEMUX_ERROR;
  }
  data->data = (void *)pkt;
  if (pkt->stream_index == video_stream_index_) {
    data->type_ = VIDEO_STREAM;
    LOGI(TAG, "%s", "ffmpag read video pkt");
  } else {
    data->type_ = AUDIO_STREAM;
    LOGI(TAG, "%s", "ffmpag read audio pkt");
  }
  return DEMUX_OK;
}

int FFmpegDemux::play(float speed) { return 0; }

int FFmpegDemux::pause() { return 0; }

int FFmpegDemux::seek(long long seek_time) { return 0; }

int FFmpegDemux::stop() { return 0; }

int FFmpegDemux::close() { return 0; }
