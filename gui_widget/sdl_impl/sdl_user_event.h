#ifndef __SDL_USER_EVENT__
#define __SDL_USER_EVENT__

typedef enum SDL_USER_EVENT_TYPE {
  SDL_USER_EVENT_REFRESH = 100,
  SDL_USER_EVENT_VIDEO_UPDATE,
  SDL_USER_EVENT_MAX,
} sdl_user_event_t;

#endif
