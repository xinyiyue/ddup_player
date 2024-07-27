#ifndef __DDUP_TIMER__
#define __DDUP_TIMER__

#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include "log/ddup_log.h"

#define TTTAG "DDupTimer"

class DDupTimer {
public:
  DDupTimer() : running_(false) {}
  
  void run(int interval, std::function<void()> task) {
    if (running_) return;
    running_ = true;
    std::thread timer_thread([this, interval, task]() {
      while(running_) {
        LOGI(TTTAG, "timer wait%lld", interval);
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        LOGI(TTTAG, "%s", "timer start run task");
        task();
        LOGI(TTTAG, "%s", "timer run task finish");
        running_ = false;
      }
      LOGD(TTTAG, "%s", "timer thread exit");
     });
    timer_thread.detach();
  }

private:
  std::atomic<bool> running_;

};

#endif
