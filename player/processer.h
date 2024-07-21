#ifndef __PROCESSER__H__
#define __PROCESSER__H__

#include "player/event_listener.h"
#include "player/fifo_controller.h"
#include "player/sink.h"

typedef enum PROCESSER_TYPE {
  AUDIO_PROCESSER,
  VIDEO_PROCESSER
} processer_type_t;

class Processer : public BufferProducer,
                  public FreeHandler,
                  public EventListener {
 public:
  Processer(processer_type_t type, EventListener *listener);
  virtual ~Processer();
  virtual int init();
  virtual int process_data(void *data);
  virtual int process(void *data, void **out) = 0;
  virtual int uninit();
  virtual int set_speead(float speed);
  int flush();

 protected:
  video_format_s src_fmt_;
  video_format_s dst_fmt_;
  audio_format_s asrc_fmt_;
  audio_format_s adst_fmt_;

  Sink *sink_;

 private:
  virtual int config() = 0;
  Fifo *raw_fifo_;
  processer_type_t type_;
};

#endif
