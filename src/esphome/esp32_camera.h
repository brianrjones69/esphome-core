#ifndef ESPHOME_CORE_ESP32_CAMERA_H
#define ESPHOME_CORE_ESP32_CAMERA_H

#include "esphome/defines.h"

#ifdef USE_ESP32_CAMERA

#include "esphome/component.h"
#include <esp_camera.h>

ESPHOME_NAMESPACE_BEGIN

class ESP32Camera;

class CameraImage {
 public:
  CameraImage(camera_fb_t *buffer);
  camera_fb_t *get_raw_buffer();
  uint8_t *get_data_buffer();
  size_t get_data_length();

 protected:
  camera_fb_t *buffer_;
};

class CameraImageReader {
 public:
  void set_image(std::shared_ptr<CameraImage> image);
  size_t available() const;
  uint8_t *peek_data_buffer();
  void consume_data(size_t consumed);
  void return_image();
 protected:
  std::shared_ptr<CameraImage> image_;
  size_t offset_{0};
};

enum ESP32CameraFrameSize {
  ESP32_CAMERA_SIZE_160X120,  // QQVGA
  ESP32_CAMERA_SIZE_128X160,  // QQVGA2
  ESP32_CAMERA_SIZE_176X144,  // QCIF
  ESP32_CAMERA_SIZE_240X176,  // HQVGA
  ESP32_CAMERA_SIZE_320X240,  // QVGA
  ESP32_CAMERA_SIZE_400X296,  // CIF
  ESP32_CAMERA_SIZE_640X480,  // VGA
  ESP32_CAMERA_SIZE_800X600,  // SVGA
  ESP32_CAMERA_SIZE_1024X768,  // XGA
  ESP32_CAMERA_SIZE_1280X1024,  // SXGA
  ESP32_CAMERA_SIZE_1600X1200,  // UXGA
};

class ESP32Camera : public PollingComponent, public Nameable {
 public:
  ESP32Camera(uint32_t update_interval, const std::string &name);
  void set_data_pins(std::array<uint8_t, 8> pins);
  void set_vsync_pin(uint8_t pin);
  void set_href_pin(uint8_t pin);
  void set_pixel_clock_pin(uint8_t pin);
  void set_external_clock(uint8_t pin, uint32_t frequency);
  void set_i2c_pins(uint8_t sda, uint8_t scl);
  void set_frame_size(ESP32CameraFrameSize size);
  void set_jpeg_quality(uint8_t quality);
  void set_framebuffer_count(uint8_t count);
  void set_reset_pin(uint8_t pin);
  void set_power_down_pin(uint8_t pin);
  void setup() override;
  void update() override;
  void loop() override;
  void dump_config() override;
  void add_image_callback(std::function<void(std::shared_ptr<CameraImage>)> f);

 protected:
  static void framebuffer_task(void *pv);

  camera_config_t config_{};
  bool vertical_flip_{true};
  bool horizontal_mirror_{true};
  int contrast_{0};
  int brightness_{0};
  int saturation_{0};

  esp_err_t init_error_{ESP_OK};
  std::shared_ptr<CameraImage> current_image_;
  QueueHandle_t framebuffer_get_queue_;
  QueueHandle_t framebuffer_return_queue_;
  CallbackManager<void(std::shared_ptr<CameraImage>)> new_image_callback_;
};

extern ESP32Camera *global_esp32_camera;

ESPHOME_NAMESPACE_END

#endif //USE_ESP32_CAMERA

#endif //ESPHOME_CORE_ESP32_CAMERA_H
