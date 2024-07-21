

#include "player/pipeline.h"

#include "log/ddup_log.h"
#include "player/component.h"

#define TAG "Pipeline"

Pipeline::Pipeline(EventListener *listener) : EventListener("Pipeline") {
  listener_ = listener;
}

Pipeline::~Pipeline() {
  if (demux_) {
    delete demux_;
  }
}

int Pipeline::open() {
  int ret = CreateDemux(this, &demux_);
  if (ret < 0) {
    LOGE(TAG, "%s", "create demux failed");
    return ret;
  } else {
    LOGI(TAG, "%s", "create demux success");
  }
  demux_->open();
  return 0;
}

int Pipeline::prepare(const char *url) {
  int ret = demux_->prepare(url);
  if (ret < 0) {
    LOGE(TAG, "prepare failed:%s", url);
  } else {
    LOGI(TAG, "prepare success:%s", url);
  }
  return ret;
}

int Pipeline::set_speed(float speed) {
  int ret = demux_->set_speed(speed);
  if (ret < 0) {
    LOGE(TAG, "play %f failed:%d", speed, ret);
  } else {
    LOGI(TAG, "play %f success", speed);
  }
  return ret;
}

int Pipeline::seek(long long seek_time) {
  int ret = demux_->seek(seek_time);
  if (ret < 0) {
    LOGE(TAG, "seek :%lldms failed:%d", seek_time, ret);
  } else {
    LOGI(TAG, "seek :%lldms success", seek_time);
  }

  return ret;
}

int Pipeline::stop() {
  int ret = demux_->stop();
  if (ret < 0) {
    LOGE(TAG, "stop failed:%d", ret);
  } else {
    LOGI(TAG, "%s", "stop success");
  }

  return ret;
}

int Pipeline::close() {
  int ret = demux_->close();
  if (ret < 0) {
    LOGE(TAG, "close failed:%d", ret);
  } else {
    LOGI(TAG, "%s", "close success");
  }

  return ret;
}

void Pipeline::notify_event(int event_type) {}

void Pipeline::notify_error(int error_type) {}
