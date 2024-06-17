#ifndef __DECODER_H__
#define __DECODER_H__
class Stream;
class Decoder {
 public:
  Decoder(Stream *stream) { stream_ = stream; }
  virtual ~Decoder() {};
  virtual int open(void *codec_info) = 0;
  virtual int decode(void *data) = 0;
  virtual int flush() = 0;
  virtual int close();
 private:
  Stream *stream_;
};


#endif
