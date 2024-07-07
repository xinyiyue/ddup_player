
#include "player/component.h"

#include "player/ffmpeg_impl/ffmpeg_decoder.h"
#include "player/ffmpeg_impl/ffmpeg_demux.h"
#include "player/ffmpeg_impl/ffmpeg_processer.h"
#include "player/ffmpeg_impl/ffmpeg_stream.h"

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

int CreateStream(stream_type_t type, void *stream_info, Stream **stream) {
  *stream = new FFmpegStream(type, (AVStream *)stream_info);
  if (*stream == nullptr) {
    return -1;
  }
  return 0;
}

int CreateDecoder(void *codec_param, Decoder **decoder) {
  *decoder = new FFmpegDecoder(codec_param);
  if (*decoder == nullptr) {
    return -1;
  }
  return 0;
}

int CreateProcesser(processer_type_t type, void *codec_param,
                    Processer **processer) {
  int ret = 0;
  if (type == AUDIO_PROCESSER) {
    *processer = new FFmpegAudioProcesser(type, codec_param);
  } else if (type == VIDEO_PROCESSER) {
    *processer = new FFmpegVideoProcesser(type, codec_param);
  } else {
    ret = -1;
  }
  return ret;
}

int CreateSink(sink_type_t type, Sink **sink) {
  *sink = new Sink(type);  // just test, need to create impl sink
  return 0;
}
