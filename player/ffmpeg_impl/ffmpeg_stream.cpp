#include "player/ffmpeg_impl/ffmpeg_stream.h"
#include "player/component.h"

  
int FFmpegStream::create_decoder() { 
  CreateDecoder(ff_stream_->codecpar, &decoder_);
  return 0;
};
