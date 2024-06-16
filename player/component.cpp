#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "player/component.h"

#include "player/ffmpeg_impl/ffmpeg_demux.h"
#include "player/ffmpeg_impl/ffmpeg_stream.h"

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

#endif
