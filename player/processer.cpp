
#include "player/processer.h"

#include "log/ddup_log.h"
#include "player/buffer.h"
#include "player/component.h"

#define TAG "Processer"

Processer::Processer(processer_type_t type, EventListener *listener)
    : EventListener("Processer"),
      BufferProducer(type == AUDIO_PROCESSER ? "audio_processer"
                                             : "video_processer") {
  type_ = type;
  listener_ = listener;
}

Processer::~Processer() {
  if (raw_fifo_) delete raw_fifo_;
  if (sink_) delete sink_;
}

int Processer::init() {
  fifo_type_t fifo_type;
  sink_type_t sink_type;
  if (type_ == AUDIO_PROCESSER) {
    fifo_type = AUDIO_FIFO;
    sink_type = AUDIO_SINK;
  } else {
    fifo_type = VIDEO_FIFO;
    sink_type = VIDEO_SINK;
  }
  raw_fifo_ = new Fifo(type_ == AUDIO_PROCESSER ? "araw_fifo" : "vraw_fifo",
                       DEFAULT_FIFO_SIZE, sizeof(void *), fifo_type, this);
  bind_fifo(raw_fifo_);
  LOGI(TAG, "init, bind fifo:%s", type_ == AUDIO_PROCESSER ? "audio" : "video");
  CreateSink(static_cast<EventListener *>(this), sink_type, &sink_);
  LOGI(TAG, "init, create %s sink:%p",
       type_ == AUDIO_PROCESSER ? "audio" : "video", sink_);
  sink_->bind_fifo(raw_fifo_);
  sink_->init();
  config();
  return 0;
}

int Processer::flush() {
  LOGI(TAG, "flush, %s sink discard data",
       type_ == AUDIO_PROCESSER ? "audio" : "video", sink_);
  sink_->discard_buffer(type_ == AUDIO_PROCESSER ? AUDIO_FIFO : VIDEO_FIFO);
  return 0;
}

int Processer::process_data(void *data) {
  void *out = nullptr;
  process(data, &out);
  LOGD(TAG, "process data, append %s data:%p",
       type_ == AUDIO_PROCESSER ? "audio" : "video", out);
  append_buffer(&out, type_ == AUDIO_PROCESSER ? AUDIO_FIFO : VIDEO_FIFO);
  return 0;
}

int Processer::uninit() {
  flush();
  sink_->uninit();
  return 0;
}

int Processer::set_speead(float speed) { return 0; }
