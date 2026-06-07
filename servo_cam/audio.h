#pragma once
#include <driver/i2s.h>
#include "config.h"
#include "audio_data.h"

static volatile bool audioPlaying = false;
static const int16_t* currentClipData = nullptr;
static size_t currentClipSamples = 0;
static size_t currentClipPos = 0;

void initAudio() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE,
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
  Serial.println("I2S audio ready");
}

void playRandomClip() {
  if (audioPlaying) return;
  int idx = random(0, audio_clip_count);
  currentClipData = audio_clips[idx].data;
  currentClipSamples = audio_clips[idx].samples;
  currentClipPos = 0;
  audioPlaying = true;
}

void stopAudio() {
  audioPlaying = false;
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void handleAudioLoop() {
  if (!audioPlaying) return;

  size_t remaining = currentClipSamples - currentClipPos;
  if (remaining == 0) {
    audioPlaying = false;
    i2s_zero_dma_buffer(I2S_NUM_0);
    return;
  }

  size_t chunk = min(remaining, (size_t)256);
  int16_t buf[256];
  memcpy_P(buf, &currentClipData[currentClipPos], chunk * sizeof(int16_t));

  size_t bytes_written;
  i2s_write(I2S_NUM_0, buf, chunk * sizeof(int16_t), &bytes_written, portMAX_DELAY);
  currentClipPos += bytes_written / sizeof(int16_t);
}

bool isAudioPlaying() {
  return audioPlaying;
}
