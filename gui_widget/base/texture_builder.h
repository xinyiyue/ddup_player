#ifndef __TEXTURE_BUILDER_H__
#define __TEXTURE_BUILDER_H__

#include <string>

#include "player/buffer.h"

class TextureBuilder {
 public:
  TextureBuilder(const char *name) { name_ = name; }
  virtual int get_supported_format(video_format_s *format) = 0;
  virtual int set_negotiated_format(video_format_s *format) = 0;
  virtual int build_texture(render_buffer_s *buff) = 0;
  virtual int render_texture() = 0;
  std::string name_;
};

#endif
