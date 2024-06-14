

#include "player/pipeline.h"

#include "log/ddup_log.h"

#define TAG "Pipeline"

Pipeline::Pipeline(EventListener *listener) {}

Pipeline::~Pipeline() {}

int Pipeline::open(char *url) {
  LOGI(TAG, "%s", "open");
  return 0;
}

int Pipeline::play(float speed) { return 0; }

int Pipeline::pause() { return 0; }

int Pipeline::seek(long long seek_time) { return 0; }

int Pipeline::stop() { return 0; }

int Pipeline::close() { return 0; }

void Pipeline::notify_event(int event_type) {}

void Pipeline::notify_error(int error_type) {}
