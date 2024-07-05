#ifndef __DECODER_H__
#define __DECODER_H__

typedef enum DECODE_ERROR_TYPE {
  DECODE_ERROR_EAGAIN = -1,
  DECODE_ERROR_EOS = -2,
} decode_error_t;

typedef int (*out_cb)(void *data, void *handle);

class Decoder {
 public:
  Decoder() { }
  virtual ~Decoder(){};
  virtual int open() = 0;
  virtual int decode(void *data, out_cb cb, void *arg) = 0;
  virtual int close() = 0;
};

#endif
