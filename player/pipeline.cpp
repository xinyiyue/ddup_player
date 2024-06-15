

#include "player/pipeline.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "Pipeline"

Pipeline::Pipeline(EventListener *listener) { listener_ = listener; }

Pipeline::~Pipeline() {}

int Pipeline::open(char *url) {
  LOGI(TAG, "open url:%s", url);
  int ret = CreateDemux(this, url, &demux_);
  if (ret < 0) {
    LOGE(TAG, "%s", "create demux failed");
  } else {
    LOGE(TAG, "%s", "create demux success");
  }
  demux_->init();
  demux_->open();
  demux_->create_stream();
  return 0;
}

int Pipeline::play(float speed) { return 0; }

int Pipeline::pause() { return 0; }

int Pipeline::seek(long long seek_time) { return 0; }

int Pipeline::stop() { return 0; }

int Pipeline::close() { return 0; }

void Pipeline::notify_event(int event_type) {}

void Pipeline::notify_error(int error_type) {}
