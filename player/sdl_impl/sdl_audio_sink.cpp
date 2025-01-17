#include "player/sdl_impl/sdl_audio_sink.h"

#include <functional>

#include "log/ddup_log.h"

#define TAG "SdlAudioSink"

SdlAudioSink::SdlAudioSink(sink_type_t type, EventListener *listener)
    : Sink(type, listener) {
  exit_ = false;
  last_buff_ = nullptr;
  last_buff_offset_ = 0;
  LOGI(TAG, "SdlAudioSink %p", this);
}

SdlAudioSink::~SdlAudioSink() { LOGI(TAG, "%s", "~SdlAudioSink exit"); }

int SdlAudioSink::init() {
  // render_thread_id_ =
  //    std::thread(std::bind(&SdlAudioSink::audio_render_thread, this));
  SDL_AudioSpec desired_spec;
  SDL_AudioSpec have_spec;

  desired_spec.freq = 44100;
  desired_spec.format = AUDIO_S16SYS;
  desired_spec.channels = 2;
  desired_spec.silence = 0;
  desired_spec.samples = 1024;
  desired_spec.callback = &SdlAudioSink::audio_callback;
  desired_spec.userdata = this;

  if ((audio_dev_ = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &have_spec,
                                        SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
    LOGE(TAG, "SDL_OpenAudioDevice with error deviceID : %d", audio_dev_);
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
  SDL_PauseAudioDevice(audio_dev_, 0);
  return 0;
}

int SdlAudioSink::uninit() {
  LOGI(TAG, "%s", "uninit");
  exit_ = true;
  consume_abort(AUDIO_FIFO);
  SDL_Delay(20);
  SDL_CloseAudioDevice(audio_dev_);
  LOGI(TAG, "%s", "uninit finish");
  // if (render_thread_id_.joinable()) render_thread_id_.join();
  return 0;
}

void SdlAudioSink::audio_callback(void *userdata, Uint8 *stream, int len) {
  SdlAudioSink *sink = (SdlAudioSink *)userdata;
  if (!sink) {
    LOGE(TAG, "%s", "Audio sink is null");
    return;
  }
  if (sink->eos_ && sink->is_fifo_empty(AUDIO_FIFO)) {
    sink->listener_->notify_event(DDUP_EVENT_AUDIO_EOS, nullptr);
    LOGI(TAG, "%s", "Notify AUDIO_EOS");
    sink->eos_ = false;
    return;
  }

  while (len > 0 && !sink->exit_) {
    render_buffer_s *buff = sink->last_buff_;
    if (buff) {
      int to_copy = std::min(buff->len[0], len);
      LOGD(TAG, "last buff len: %d, wanted:%d, last_buff: %p", buff->len[0],
           to_copy, buff);
      memcpy(stream, buff->data[0] + sink->last_buff_offset_, to_copy);
      stream += to_copy;
      len -= to_copy;
      sink->last_buff_offset_ += to_copy;
      buff->len[0] -= to_copy;
      if (buff->len[0] == 0) {
        for (int i = 0; i < 4; ++i) {
          if (buff->data[i]) free(buff->data[i]);
        }
        free(buff);
        sink->last_buff_ = nullptr;
        sink->last_buff_offset_ = 0;
        if (len == 0) {
          LOGD(TAG, "after consume:%d last buff len: %d, last_buff: %p finish",
               to_copy, 0, sink->last_buff_);
          return;
        }
      } else {
        LOGD(TAG, "after consume:%d last buff len: %d, last_buff: %p", to_copy,
             buff->len[0], sink->last_buff_);
        return;
      }
    }

    if (len > 0) {
      render_buffer_s *new_buff = nullptr;
      bool ret = sink->consume_buffer(&new_buff, AUDIO_FIFO);
      if (!ret) {
        LOGD(TAG, "%s", "Consume buffer error or abort");
        continue;
      }
      LOGD(TAG, "consume new buffer:%p len: %d, wanted:%d last_buff: %p",
           new_buff, new_buff->len[0], len, sink->last_buff_);
      sink->listener_->notify_event(DDUP_EVENT_POSITION,
                                    (void *)&(new_buff->pts));

      int to_copy = std::min(new_buff->len[0], len);
      memcpy(stream, new_buff->data[0], to_copy);
      stream += to_copy;
      len -= to_copy;

      if (to_copy < new_buff->len[0]) {
        new_buff->len[0] -= to_copy;
        sink->last_buff_ = new_buff;
        sink->last_buff_offset_ += to_copy;
        LOGD(TAG,
             "after consume:%d buff:%p not finish, last buff len: %d, "
             "last_buff: %p",
             to_copy, new_buff, sink->last_buff_->len[0], sink->last_buff_);
      } else {
        LOGD(
            TAG,
            "after consume:%d buff:%p finish, last buff len: %d, last_buff: %p",
            to_copy, new_buff, sink->last_buff_->len[0], sink->last_buff_);
        for (int i = 0; i < 4; ++i) {
          if (new_buff->data[i]) free(new_buff->data[i]);
        }
        free(new_buff);
      }
      if (len > 0) {
        continue;
      }
    }
  }
  // LOGE(TAG, "%s", "callback out");
}

void SdlAudioSink::audio_render_thread(void) {
  while (!exit_) {
    if (eos_ && is_fifo_empty(AUDIO_FIFO)) {
      listener_->notify_event(DDUP_EVENT_AUDIO_EOS, nullptr);
      LOGI(TAG, "%s", "notify AUDIO_EOS");
      eos_ = false;
    }
    render_buffer_s *buff;
    bool ret = consume_buffer(&buff, AUDIO_FIFO);
    if (!ret) {
      LOGE(TAG, "%s", "consume buffer error or abort");
      continue;
    }

    if (!is_start && audio_dev_ != 0) {
      SDL_PauseAudioDevice(audio_dev_, 0);
      is_start = true;
    }
    listener_->notify_event(DDUP_EVENT_POSITION, (void *)&(buff->pts));
    SDL_QueueAudio(audio_dev_, buff->data[0], buff->len[0]);
    for (int i = 0; i < 4; ++i) {
      if (buff->data[i]) free(buff->data[i]);
    }
    free(buff);
  }
  LOGI(TAG, "%s", "audio render thread exit!!!!");
}

int SdlAudioSink::get_supported_format(audio_format_s *format) {
  format->channel_number = aformat.channel_number;
  format->sample_rate = aformat.sample_rate;
  format->sample_format = aformat.sample_format;
  return 0;
}

int SdlAudioSink::set_negotiated_format(audio_format_s *format) { return 0; }
