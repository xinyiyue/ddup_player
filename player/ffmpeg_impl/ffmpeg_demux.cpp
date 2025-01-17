#include "player/ffmpeg_impl/ffmpeg_demux.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "FfmpegDemux"

int FFmpegDemux::prepare(const char *url) {
  int ret = 0;
  fmt_ctx_ = avformat_alloc_context();
  ret = avformat_open_input(&fmt_ctx_, url, NULL, NULL);
  if (ret < 0) {
    LOGE(TAG, "open input error:%d", ret);
    return ret;
  }

  ret = avformat_find_stream_info(fmt_ctx_, NULL);
  if (ret < 0) {
    LOGE(TAG, "find stream info error:%d", ret);
    return ret;
  }
  AVRational time_base = {1, AV_TIME_BASE};
  duration_ = av_q2d(time_base) * fmt_ctx_->duration * 1000;
  LOGI(TAG, "ffmpeg prepare ok, duration:%lld, call demux prepare:%d",
       duration_, ret);
  Demux::prepare(url);
  return 0;
}

int FFmpegDemux::stop() {
  if (!fmt_ctx_) {
    LOGE(TAG, "%s", "stop failed, fmt_ctx is null");
    return -1;
  }
  avformat_close_input(&fmt_ctx_);
  Demux::stop();
  return 0;
}

int FFmpegDemux::create_stream() {
  if (!fmt_ctx_) {
    LOGE(TAG, "%s", "create stream failed, fmt_ctx is null");
    return -1;
  }
  int ret = 0;
  ret = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  if (ret < 0) {
    LOGE(TAG, "Could not find %s stream",
         av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
  } else {
    video_stream_index_ = ret;
    AVStream *vstream = fmt_ctx_->streams[video_stream_index_];
    ret = CreateStream(static_cast<EventListener *>(this), VIDEO_STREAM,
                       (void *)vstream, &video_stream_);
    if (ret < 0) {
      LOGE(TAG, "create video stream failed:%d", ret);
    }
  }

  ret = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
  if (ret < 0) {
    LOGE(TAG, "Could not find %s stream",
         av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
  } else {
    audio_stream_index_ = ret;
    AVStream *astream = fmt_ctx_->streams[audio_stream_index_];
    ret = CreateStream(static_cast<EventListener *>(this), AUDIO_STREAM,
                       (void *)astream, &audio_stream_);
    if (ret < 0) {
      LOGE(TAG, "create audio stream failed:%d", ret);
    }
    LOGI(TAG, "%s", "create audio stream success");
  }
  if (video_stream_index_ == -1 && audio_stream_index_ == -1) {
    LOGE(TAG, "%s", "no stream found");
    return -1;
  }
  return 0;
}

int FFmpegDemux::seek_impl(long long seek_time) {
  if (!fmt_ctx_) {
    LOGE(TAG, "seek:%d ms failed, fmt_ctx is null", seek_time);
    return -1;
  }
  int flag = AVSEEK_FLAG_BACKWARD;
  int ret = avformat_seek_file(fmt_ctx_, -1, INT64_MIN, seek_time * 1000,
                               INT64_MAX, flag);
  if (ret < 0) {
    LOGE(TAG, "seek:%d ms failed", seek_time);
  }
  LOGE(TAG, "seek:%d ms success", seek_time);
  return ret;
}

demux_event_t FFmpegDemux::read_input_impl(av_data_s *data) {
  if (!fmt_ctx_) {
    LOGD(TAG, "%s", "demux ctx is null, return");
    return DEMUX_ERROR;
  }
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

  AVRational base_ms = {1, 1000};
  long long pts =
      av_q2d(fmt_ctx_->streams[pkt->stream_index]->time_base) * pkt->pts * 1000;
  if (pkt->stream_index == video_stream_index_) {
    LOGD(TAG, "demux ctx read video ptk:%lld, addr:%p", pts, data->data);
    data->type_ = VIDEO_STREAM;
  } else {
    LOGD(TAG, "demux ctx read audio ptk:%lld, addr:%p", pts, data->data);
    data->type_ = AUDIO_STREAM;
  }
  return DEMUX_OK;
}

int FFmpegDemux::free_data(void *data) {
  AVPacket *pkt = (AVPacket *)data;
  if (!pkt) return -1;
  if (pkt->stream_index == audio_stream_index_) {
    LOGD(TAG, "free audio pkt:%p", pkt);
  } else {
    LOGD(TAG, "free video pkt:%p", pkt);
  }
  av_packet_unref(pkt);
  av_packet_free(&pkt);
  return 0;
}
