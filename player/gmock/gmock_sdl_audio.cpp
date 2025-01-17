#include <SDL2/SDL.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <chrono>
#include <ctime>

#include "log/ddup_log.h"

#define TAG "SDL_TEST"

struct AudioParams {
  int freq;
  SDL_AudioFormat format;
  Uint8 channels;
  FILE *file;
};

// 模拟SDL音频回调函数（实际上不需要在测试中使用）
void audio_callback(void *userdata, Uint8 *stream, int len) {
  AudioParams *params = (AudioParams *)userdata;
  size_t bytesRead = fread(stream, sizeof(char), len, params->file);
  if (bytesRead != len) {
    LOGI(TAG, "read data len%d less than expected:%d", bytesRead, len);
  }
  auto now = std::chrono::high_resolution_clock::now();

  // 将时间点转换为自纪元以来的纳秒数，然后转换为毫秒
  auto duration = now.time_since_epoch();
  long long millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  LOGI(TAG, "[%lld]read data len:%d EQ expected:%d", millis, bytesRead, len);
}

// 尝试使用SDL初始化音频设备并验证参数
bool try_open_audio(int wanted_freq, SDL_AudioFormat wanted_format,
                    Uint8 wanted_channels, AudioParams *params) {
  SDL_AudioSpec wanted_spec, spec;
  SDL_AudioDeviceID audio_dev;

  SDL_zero(wanted_spec);
  SDL_zero(spec);

  wanted_spec.freq = wanted_freq;
  wanted_spec.format = wanted_format;
  wanted_spec.channels = wanted_channels;
  wanted_spec.samples = 1024;  // 适当的缓冲区大小
  wanted_spec.callback = audio_callback;
  wanted_spec.userdata = params;
  audio_dev = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, &spec,
                                  SDL_AUDIO_ALLOW_ANY_CHANGE);
  if (audio_dev < 2) {
    LOGI(TAG, "sdl open device failed:%s", SDL_GetError());
    return false;
  }
  LOGI(TAG, "sdl open device success, device id:%d", audio_dev);
  SDL_PauseAudioDevice(audio_dev, 0);  //开始播放
  sleep(20);
  SDL_CloseAudioDevice(audio_dev);  // 关闭设备，因为我们只是测试
  LOGI(TAG, "sdl close device success, device id:%d", audio_dev);

  if (params != nullptr) {
    params->freq = spec.freq;
    params->format = spec.format;
    params->channels = spec.channels;
  }

  return true;
}

// gtest测试
TEST(SDLAudioTest, OpenAudioWithCorrectParams) {
  AudioParams actual_params;
  memset(&actual_params, 0, sizeof(actual_params));
  const char *file_name = "/home/mi/data/ddup_player/res/tianhou.pcm";

  LOGI(TAG, "open file:%s, handle:%p", file_name, actual_params.file);
  EXPECT_NE(actual_params.file, nullptr);
  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    LOGE(TAG, "SDL_Init audio error: %s", SDL_GetError());
    return;
  }
  int count = 5;
  while (count > 0) {
    actual_params.file = fopen(file_name, "rb");
    bool success = try_open_audio(44100, AUDIO_S16SYS, 2, &actual_params);
    fclose(actual_params.file);
    EXPECT_TRUE(success);
    EXPECT_EQ(actual_params.freq, 44100);
    EXPECT_EQ(actual_params.format, AUDIO_S16SYS);
    EXPECT_EQ(actual_params.channels, 2);
    count--;
  }
}
