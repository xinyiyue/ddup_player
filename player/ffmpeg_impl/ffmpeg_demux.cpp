#include "player/ffmpeg_impl/ffmpeg_demux.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "FfmpegDemux"

int FFmpegDemux::prepare(char *url) {
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
  return 0;
}

int FFmpegDemux::seek(long long seek_time) {
  AutoLock lock(&cmd_mutex_);
  if (!fmt_ctx_) {
    LOGE(TAG, "seek:%d ms failed, fmt_ctx is null", seek_time);
    return -1;
  }
  Demux::seek(seek_time);
  int flag = AVSEEK_FLAG_BACKWARD;
  int ret = avformat_seek_file(fmt_ctx_, -1, INT64_MIN, seek_time * 1000,
                               INT64_MAX, flag);
  if (ret < 0) {
    LOGE(TAG, "seek:%d ms failed", seek_time);
  }
  LOGE(TAG, "seek:%d ms success", seek_time);
  return ret;
}

demux_event_t FFmpegDemux::read_input_data(av_data_s *data) {
  AutoLock lock(&cmd_mutex_);
  if (!fmt_ctx_) {
    LOGE(TAG, "%s", "demux ctx is null");
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
  if (pkt->stream_index == video_stream_index_) {
    data->type_ = VIDEO_STREAM;
  } else {
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
