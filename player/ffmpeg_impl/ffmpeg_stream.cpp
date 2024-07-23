#include "player/ffmpeg_impl/ffmpeg_stream.h"

#include "player/component.h"

int FFmpegStream::create_decoder() {
  CreateDecoder(static_cast<EventListener *>(this), ff_stream_, &decoder_);
  return 0;
};

int FFmpegStream::create_processer() {
  processer_type_t pt;
  if (stream_type_ == AUDIO_STREAM) {
    pt = AUDIO_PROCESSER;
  } else {
    pt = VIDEO_PROCESSER;
  }
  CreateProcesser(static_cast<EventListener *>(this), pt, ff_stream_->codecpar,
                  &processer_);
  return 0;
};
