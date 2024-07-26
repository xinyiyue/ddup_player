#include "player/sdl_impl/sdl_audio_sink.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "SdlAudioSink"

SdlAudioSink::SdlAudioSink(sink_type_t type, EventListener *listener)
    : Sink(type, listener) {
  exit_ = false;
}

SdlAudioSink::~SdlAudioSink() {}

int SdlAudioSink::init() {
  render_thread_id_ = std::thread(std::bind(&SdlAudioSink::audio_render_thread,
                                            this, std::placeholders::_1),
                                  (void *)this);

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    LOGE(TAG, "SDL_Init audio error: %s", SDL_GetError());
    return -1;
  }

  SDL_AudioSpec desired_spec;
  SDL_AudioSpec have_spec;

  desired_spec.freq = 44100;
  desired_spec.format = AUDIO_S16SYS;
  desired_spec.channels = 2;
  desired_spec.silence = 0;
  desired_spec.samples = 1024;
  desired_spec.callback = NULL;

  if ((audio_dev = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &have_spec,
                                       SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
    LOGE(TAG, "SDL_OpenAudioDevice with error deviceID : %d", audio_dev);
    return -1;
  }

  if (desired_spec.format != have_spec.format ||
      desired_spec.channels != have_spec.channels ||
      desired_spec.freq != have_spec.freq) {
    LOGE(TAG, "SDL format don't match, desired:[%d %d %d], have:[%d %d %d] ",
         desired_spec.format, desired_spec.channels, desired_spec.freq,
         have_spec.format, have_spec.channels, have_spec.freq);
    return -1;
  }

  aformat.channel_number = have_spec.channels;
  aformat.sample_rate = have_spec.freq;
  aformat.sample_format = have_spec.format;

  return 0;
}

int SdlAudioSink::uninit() {
  LOGI(TAG, "%s", "uninit");
  exit_ = true;
  SDL_CloseAudio();
  consume_abort(AUDIO_FIFO);
  if (render_thread_id_.joinable()) render_thread_id_.join();
  return 0;
}

void *SdlAudioSink::audio_render_thread(void *arg) {
  SdlAudioSink *sink = (SdlAudioSink *)arg;
  while (!exit_) {
    render_buffer_s *buff;
    bool ret = sink->consume_buffer(&buff, AUDIO_FIFO);
    if (!ret) {
      LOGE(TAG, "%s", "consume buffer error");
      continue;
    }

    if (!is_start && audio_dev != 0) {
      SDL_PauseAudioDevice(audio_dev, 0);
      is_start = true;
    }

    listener_->notify_event(DDUP_EVENT_POSITION, (void *)&(buff->pts));
    SDL_QueueAudio(audio_dev, buff->data, buff->len);

    free(buff->data);
    free(buff);
  }
  LOGI(TAG, "%s", "audio render thread exit!!!!");
  return nullptr;
}

int SdlAudioSink::get_supported_format(audio_format_s *format) {
  format->channel_number = aformat.channel_number;
  format->sample_rate = aformat.sample_rate;
  format->sample_format = aformat.sample_format;
  return 0;
}

int SdlAudioSink::set_negotiated_format(audio_format_s *format) { return 0; }
