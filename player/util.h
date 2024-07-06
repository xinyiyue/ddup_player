#ifndef _UTIL_H__
#define _UTIL_H__

class AutoLock {
 public:
  AutoLock(pthread_mutex_t *mutex) {
    mutex_ = mutex;
    pthread_mutex_lock(mutex_);
  };
  ~AutoLock() { pthread_mutex_unlock(mutex_); }

 private:
  pthread_mutex_t *mutex_;
};

#endif
