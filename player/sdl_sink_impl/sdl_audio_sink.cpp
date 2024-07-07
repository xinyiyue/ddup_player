#include "sdl_audio_sink.h"

#include "log/ddup_log.h"

#define TAG "SDLAudioSink"

extern "C" {
#include "third_party/ffmpeg-7.0.1/include/libavformat/avformat.h"
#include "third_party/ffmpeg-7.0.1/include/libavutil/channel_layout.h"
}

void SDLAudioSink::sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
  void *data = NULL;
  SDLAudioSink *instance = static_cast<SDLAudioSink *>(opaque);
  SDL_memset(stream, 0, len);
  instance->consume(&data, AUDIO_FIFO);
  if (data) {
    AVFrame *frame = (AVFrame *)(data);
    int data_size = frame->linesize[0];
    if (data_size > len) {
      SDL_MixAudioFormat(stream, frame->data[0], AUDIO_S16SYS, len,
                         SDL_MIX_MAXVOLUME);
    } else {
      SDL_MixAudioFormat(stream, frame->data[0], AUDIO_S16SYS, data_size,
                         SDL_MIX_MAXVOLUME);
    }
  }
}

int SDLAudioSink::open(void *codec_param) {
  SDL_Init(SDL_INIT_AUDIO);
  AVCodecParameters *codec_param_ = (AVCodecParameters *)codec_param;
  open_sdl_device(&codec_param_->ch_layout, codec_param_->sample_rate);
  return 0;
}

int SDLAudioSink::start() {
  SDL_PauseAudioDevice(audio_dev, 0);
  return 0;
}

int SDLAudioSink::stop() { return 0; }

int SDLAudioSink::close() {
  SDL_CloseAudioDevice(audio_dev);
  // SDL_Quit();
  return 0;
}

int SDLAudioSink::get_audio_frame_info(void *frame) {
  AVFrame *frame_ = (AVFrame *)frame;
  if (!frame_ || frame_->nb_samples <= 0 || !frame_->data[0]) {
    LOGE(TAG, "%s", "get audio info error");
    return 0;
  }

  enum AVSampleFormat sampleFormat = (enum AVSampleFormat)frame_->format;
  int channels = frame_->ch_layout.nb_channels;
  int bytes_per_sample = av_get_bytes_per_sample(sampleFormat);
  int dataSize = frame_->nb_samples * channels * bytes_per_sample;

  LOGI(TAG, "channels %d,bytes_per_sample %d, dataSize %d.", channels,
       bytes_per_sample, dataSize);
  return dataSize;
}

int SDLAudioSink::open_sdl_device(void *channel_layout,
                                  int wanted_sample_rate) {
  SDL_AudioSpec wanted_spec, spec;
  AVChannelLayout *wanted_channel_layout = (AVChannelLayout *)channel_layout;
  const char *env;
  static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
  static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
  int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;
  int wanted_nb_channels = wanted_channel_layout->nb_channels;

  // get the SDL audio device spec
  env = SDL_getenv("SDL_AUDIO_CHANNELS");
  if (env) {
    wanted_nb_channels = atoi(env);
    av_channel_layout_uninit(wanted_channel_layout);
    av_channel_layout_default(wanted_channel_layout, wanted_nb_channels);
  }

  if (wanted_channel_layout->order != AV_CHANNEL_ORDER_NATIVE) {
    av_channel_layout_uninit(wanted_channel_layout);
    av_channel_layout_default(wanted_channel_layout, wanted_nb_channels);
  }

  wanted_nb_channels = wanted_channel_layout->nb_channels;
  wanted_spec.channels = wanted_nb_channels;
  wanted_spec.freq = wanted_sample_rate;
  if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
    LOGE(TAG, "%s", "Invalid sample rate or channel count!\n");
    return -1;
  }

  while (next_sample_rate_idx &&
         next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
    next_sample_rate_idx--;

  wanted_spec.format = AUDIO_S16SYS;
  wanted_spec.silence = 0;
  wanted_spec.samples =
      FFMAX(sdl_audio_min_buffer_size,
            2 << av_log2(wanted_spec.freq / sdl_max_cb_per_sec));
  wanted_spec.callback = sdl_audio_callback_static;
  wanted_spec.userdata = this;

  while (
      !(audio_dev = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec,
                                        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE |
                                            SDL_AUDIO_ALLOW_CHANNELS_CHANGE))) {
    LOGI(TAG, "SDL_OpenAudio (%d channels, %d Hz): %s\n", wanted_spec.channels,
         wanted_spec.freq, SDL_GetError());
    wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
    if (!wanted_spec.channels) {
      wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
      wanted_spec.channels = wanted_nb_channels;
      if (!wanted_spec.freq) {
        LOGI(TAG, "%s", "No more combinations to try, audio open failed\n");
        return -1;
      }
    }
    av_channel_layout_default(wanted_channel_layout, wanted_spec.channels);
  }
  if (spec.format != AUDIO_S16SYS) {
    LOGI(TAG, "SDL advised audio format %d is not supported!\n", spec.format);
    return -1;
  }
  if (spec.channels != wanted_spec.channels) {
    av_channel_layout_uninit(wanted_channel_layout);
    av_channel_layout_default(wanted_channel_layout, spec.channels);
    if (wanted_channel_layout->order != AV_CHANNEL_ORDER_NATIVE) {
      LOGI(TAG, "SDL advised channel count %d is not supported!\n",
           spec.channels);
      return -1;
    }
  }

  return spec.size;
}