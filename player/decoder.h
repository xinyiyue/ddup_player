#ifndef __DECODER_H__
#define __DECODER_H__
#include <thread>

#include "player/stream.h"

class Stream;

class Decoder {
 public:
  Decoder(Stream *stream) { stream_ = stream; }
  virtual ~Decoder(){};
  virtual int open();
  virtual int decode(void *data) = 0;
  virtual int flush() = 0;
  virtual int close();

 private:
  void *decode_thread(void *arg);

 private:
  Stream *stream_;
  std::thread dec_thread_;
  bool running = true;
};

#endif
