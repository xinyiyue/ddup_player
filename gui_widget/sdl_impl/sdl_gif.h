#ifndef __SDL_GIF_H_H__
#define __SDL_GIF_H_H__

#include <thread>
#include <vector>

#include "gui_widget/base/widget.h"
#include "gui_widget/sdl_impl/sdl_texture_builder.h"
#include "player/buffer.h"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavcodec/avcodec.h"
#include "third_party/ffmpeg-7.0.1/include/libavformat/avformat.h"
#include "third_party/ffmpeg-7.0.1/include/libavutil/imgutils.h"
}

typedef enum GIFState {
  GIF_STATE_NULL,
  GIF_STATE_PLAY,
  GIF_STATE_PAUSE,
  GIF_STATE_STOP
} gif_state_t;

class SdlGif : public Widget, public SdlTextureBuilder {
 public:
  SdlGif(const char *name, const char *url, SDL_mutex *renderer_mutex,
         SDL_Renderer *renderer, int x, int y, int w, int h);
  virtual ~SdlGif();

  virtual int draw() final;
  virtual int get_type() final;
  virtual int event_handler(void *event) final;
  virtual int set_event_resp_area(int x, int y, int w, int h) final;
  virtual void *get_window() final;
  virtual void *get_renderer() final;

  int decode_gif();
  int convert_data();
  int set_state(gif_state_t state);
  enum AVPixelFormat pixel_fmt_to_ff_format(pixel_format_t pixel);
  int convert_data(AVFrame *frame, render_buffer_s **out_buff);
  void render_thread(void);

 private:
  SDL_Renderer *renderer_;
  std::string url_;
  SDL_Rect rect_;
  std::vector<render_buffer_s *> raw_buffer_;
  std::thread render_thread_id_;
  bool exit_;
  int frame_h_;
  int frame_w_;
  int frame_rate_;
  gif_state_t state_;
};

#endif
