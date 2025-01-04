#include "gui_widget/sdl_impl/sdl_util.h"

SDL_Color rgba_white = {255, 255, 255, 255};
SDL_Color rgba_red = {255, 0, 0, 100};
SDL_Color rgba_black = {0, 0, 0, 255};
SDL_Color rgba_green = {0, 150, 0, 255};
SDL_Color rgba_blue = {0, 0, 255, 255};
SDL_Color rgba_gray = {200, 200, 200, 100};
SDL_Color baby_blue = {229, 243, 255, 255};
int point_in_rect(int x, int y, SDL_Rect *rect) {
  return x >= rect->x && x < rect->x + rect->w && y >= rect->y &&
         y < rect->y + rect->h;
}
