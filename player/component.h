#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "player/demux.h"
#include "player/event_listener.h"
#include "player/pipeline.h"
#include "player/processer.h"
#include "player/sink.h"
#include "player/stream.h"

int CreatePipeline(EventListener *listener, Pipeline **pipeline);
int CreateDemux(EventListener *listener, Demux **demux);
int CreateStream(EventListener *listener, stream_type_t type, void *stream_info,
                 Stream **Stream);
int CreateDecoder(EventListener *listener, void *codec_param,
                  Decoder **decoder);
int CreateProcesser(EventListener *listener, processer_type_t type,
                    void *codec_param, Processer **processer);
int CreateSink(EventListener *listener, sink_type_t type, Sink **sink);

#endif
