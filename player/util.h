#ifndef _UTIL_H__
#define _UTIL_H__

#include <SDL2/SDL.h>
#include <pthread.h>

class AutoLock {
 public:
  AutoLock(pthread_mutex_t *mutex) {
    mutex_ = mutex;
    sdl_mutex_ = nullptr;
    pthread_mutex_lock(mutex_);
  };
  AutoLock(SDL_mutex *mutex) {
    sdl_mutex_ = mutex;
    mutex_ = nullptr;
    SDL_LockMutex(sdl_mutex_);
  };
  ~AutoLock() {
    if (mutex_) pthread_mutex_unlock(mutex_);
    if (sdl_mutex_) SDL_UnlockMutex(sdl_mutex_);
  }

 private:
  pthread_mutex_t *mutex_;
  SDL_mutex *sdl_mutex_;
};

#endif
