
#include "player/processer.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "Processer"

Processer::Processer(processer_type_t type, void *codec_param)
    : BufferProducer(type == AUDIO_PROCESSER ? "audio_processer"
                                             : "video_processer") {
  type_ = type;
  codec_param_ = codec_param;
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
  CreateSink(sink_type, &sink_);
  if (sink_->open(codec_param_) < 0) {
    LOGE(TAG, "%s", "open sink failed");
    return -1;
  }

  sink_->start();

  LOGI(TAG, "init, create %s sink:%p",
       type_ == AUDIO_PROCESSER ? "audio" : "video", sink_);
  sink_->bind_fifo(raw_fifo_);
  return 0;
}

int Processer::flush() {
  LOGI(TAG, "flush, %s sink discard data",
       type_ == AUDIO_PROCESSER ? "audio" : "video", sink_);
  sink_->discard_buffer(type_ == AUDIO_PROCESSER ? AUDIO_FIFO : VIDEO_FIFO);
  return 0;
}

int Processer::process_data(void *data) {
  process(data);
  append(&data, type_ == AUDIO_PROCESSER ? AUDIO_FIFO : VIDEO_FIFO);
  return 0;
}

int Processer::uninit() {
  sink_->close();
  flush();
  return 0;
}

int Processer::set_speead(float speed) { return 0; }
