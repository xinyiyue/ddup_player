
#include "player/component.h"

#include "player/ffmpeg_impl/ffmpeg_decoder.h"
#include "player/ffmpeg_impl/ffmpeg_demux.h"
#include "player/ffmpeg_impl/ffmpeg_processer.h"
#include "player/ffmpeg_impl/ffmpeg_stream.h"
#include "player/sdl_impl/sdl_audio_sink.h"
#include "player/sdl_impl/sdl_video_sink.h"

int CreatePipeline(EventListener *listener, Pipeline **pipeline) {
  *pipeline = new Pipeline(listener);
  if (*pipeline == nullptr) {
    return -1;
  }
  return 0;
}

int CreateDemux(EventListener *listener, Demux **demux) {
  *demux = new FFmpegDemux(listener);
  if (*demux == nullptr) {
    return -1;
  }
  return 0;
}

int CreateStream(EventListener *listener, stream_type_t type, void *stream_info,
                 Stream **stream) {
  *stream = new FFmpegStream(type, (AVStream *)stream_info, listener);
  if (*stream == nullptr) {
    return -1;
  }
  return 0;
}

int CreateDecoder(EventListener *listener, void *codec_param,
                  Decoder **decoder) {
  *decoder = new FFmpegDecoder(codec_param, listener);
  if (*decoder == nullptr) {
    return -1;
  }
  return 0;
}

int CreateProcesser(EventListener *listener, processer_type_t type,
                    void *codec_param, Processer **processer) {
  int ret = 0;
  if (type == AUDIO_PROCESSER) {
    *processer = new FFmpegAudioProcesser(type, codec_param, listener);
  } else if (type == VIDEO_PROCESSER) {
    *processer = new FFmpegVideoProcesser(type, codec_param, listener);
  } else {
    ret = -1;
  }
  return ret;
}

int CreateSink(EventListener *listener, sink_type_t type, Sink **sink) {
  if (type == AUDIO_SINK) {
    *sink = new SdlAudioSink(type,
                             listener);  // just test, need to create impl sink
  } else {
    *sink = new SdlVideoSink(type, listener);
  }
  return 0;
}
