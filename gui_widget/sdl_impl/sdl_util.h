#ifndef __SDL_UTIL__
#define __SDL_UTIL__

#include <SDL2/SDL.h>

extern SDL_Color rgba_white, rgba_black, rgba_blue, rgba_green, rgba_red,
    rgba_gray, baby_blue;

struct DirentInfo {
  char *file_name;
  int file_type;  // 0:dir,1:video,2:audio,3:image,4:other
};

typedef struct DirentInfo DirentInfo;

int point_in_rect(int x, int y, SDL_Rect *rect);

#endif
