#include <dection_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"
#include "interface.h"
#include "camera_index.h"

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

#define OLED_WIDTH 64
#define OLED_LENGTH 128

#define Menu_selection_key D6
#define Enter_key D5
#define return_key D4

#define SDA_PIN D7
#define SCL_PIN D8

/* 图片大小 */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
#define EI_CAMERA_FRAME_BYTE_SIZE                 3

/* 私有变量 ------------------------------------------------------- */
static bool debug_nn = false; // 设置为 true 以查看原始信号生成的特征
static bool is_initialised = false;
uint8_t *snapshot_buf; // 指向捕获图像的输出

static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    // XCLK 20MHz 或 OV2640 双倍 FPS 时使用 10MHz（实验性）
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, // YUV422、GRAYSCALE、RGB565、JPEG
    .frame_size = FRAMESIZE_QVGA,    // QQVGA-UXGA 在不使用 JPEG 时请不要使用超过 QVGA 的尺寸

    .jpeg_quality = 12, // 0-63，数字越小质量越高
    .fb_count = 1,       // 若大于 1，则 i2s 将以连续模式运行。仅与 JPEG 一起使用
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/* 函数定义 ------------------------------------------------------- */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) ;


const char *ssid = "DXG_2.4G";
const char *password = "DXG123456";

volatile unsigned short int x = 13;
volatile unsigned short int enter = 0;
bool clc = 0;

const unsigned int JITTERTIME=200;//消抖时间
volatile unsigned long int Enter_key_LASTTIME=0;
volatile unsigned long int Return_key_LASTTIME=0;
volatile unsigned long int Menu_selection_key_LASTTIME=0;
volatile unsigned long int Current_time=0;

volatile bool Web_status=false;
volatile bool Dection_status=false;

Adafruit_SSD1306 display(OLED_LENGTH, OLED_WIDTH, &Wire, -1);

const char *ntpServer = "ntp.ntsc.ac.cn";
const long gmtOffset_sec = 8 * 3600; // GMT+8(东八区)
const int daylightOffset_sec = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

String previousTime = "";

void startCameraServer();
void setupLedFlash(int pin);

extern void wait_to_wifi();
extern void display_signal();
extern void display_battery();
extern void Get_net(struct Network *net); 
extern void display_bar();
extern void Menu_selection();
extern void return_down();
extern void enter_down();
extern void select_icon(int cur);

void setup()
{
  display.clearDisplay();
  display_bar();
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;

  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  pinMode(Menu_selection_key, INPUT_PULLUP);//坐标键
  pinMode(Enter_key, INPUT_PULLUP);//确认键
  pinMode(return_key, INPUT_PULLUP);//返回键
  attachInterrupt(Menu_selection_key, Menu_selection, FALLING);
  attachInterrupt(return_key, return_down, FALLING);
  attachInterrupt(Enter_key, enter_down, FALLING);
  Wire.begin(SDA_PIN,SCL_PIN);
  WiFi.begin(ssid, password);
  /*初始化消抖时间*/
   Return_key_LASTTIME=millis();
   Enter_key_LASTTIME=millis();
   Menu_selection_key_LASTTIME=millis();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
   if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

/*相机初始化*/
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); 
    s->set_brightness(s, 1); 
    s->set_saturation(s, -2); 
  }
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif
  if (WiFi.status() == WL_CONNECTED) {
    display.clearDisplay();
    display_battery();
    displayout("WiFi connected!", 11, 20);
    display_signal();
    display.display();
  }
  delay(1000);
  display.clearDisplay();
  displayout("Initializing time...", 1, 20);
  display_signal();
  display_battery();
  display.display();
  timeClient.begin();
  timeClient.setUpdateInterval(3600);
}

void loop()
{
  switch (enter) {
    case 0:
      if (clc == 1) {
        display.clearDisplay();
        display_signal();
        display_battery();
        String currentTime = timeClient.getFormattedTime();
        currentTime.remove(5, 3);
        display.setTextSize(2);
        display.setCursor(30, 30);
        display.print(currentTime);
        display.display();
        clc = false;
      }
      //使用更新时间
      if (timeClient.update()) {
        display.clearDisplay();
        display_signal();
        display_battery();
        displayout("Online", 46, 4);
        String currentTime = timeClient.getFormattedTime();
        currentTime.remove(5, 3);
        if (currentTime != previousTime) {
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(30, 30);
          display.print(currentTime);
          display_signal();
          displayout("Online", 46, 4);
          display_battery();
          display.display();
          previousTime = currentTime;
        }
      }
      break;
    case 1:
    //stopCameraServer();
    WiFi.setSleep(true);
     display.clearDisplay();
     //display.display();
     displayout("Menu", 46, 4);
     display_signal();
     display_battery();
      interface_menu(&menu_1);
      select_icon(x);
      display.display();
      break;
    case 2:
      display_signal();
      display_battery();
      //select_icon(x);
      display.display();
      switch (x) {
        case 13:
          display.clearDisplay();
          //display.display();
          displayout("Network", 46, 4);
          display_signal();
          display_battery();
          interface_menu(&menu_1_1);
          Internet();
          display.display();
          break;
        case 23:
    display.clearDisplay();
    display_signal();
    display_battery();
    interface_menu(&menu_1_2);
    display.display();

// 检查是否需要进行检测
if (Dection_status) {
   if (ei_sleep(5) != EI_IMPULSE_OK) {
        return;
    }

    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);

    // check if allocation was successful
    if(snapshot_buf == nullptr) {
       menu_1_2.fourth_text="buffer error";
        return;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
        free(snapshot_buf);
        return;
    }

    // Run the classifier
    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK) {
        return;
    }

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    bool bb_found = result.bounding_boxes[0].value > 0;
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
        auto bb = result.bounding_boxes[ix];
        if (bb.value == 0) {
            continue;
        }
        // Display only the label on the screen
        displayout_s(bb.label, 10, 10); // Adjust position as needed
    }
    if (!bb_found) {
        menu_1_2.fourth_text="no object";
        
    }
#else
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        // Display only the label on the screen
        displayout_s(result.classification[ix].label, 10, 10); // Adjust position as needed
        menu_1_2.fourth.text=result.classification[ix].label;
    }
#endif

    free(snapshot_buf);
    display.display();
}
    break;
        case 33:
          display.clearDisplay();
          //display.display();
          display_signal();
          display_battery();
          interface_menu(&menu_1_3);
          display.display();
            WiFi.setSleep(false);
            startCameraServer();
          while(Web_status)
          {
            delay(10000);
          }
          break;
        case 43:
          display.clearDisplay();
          //display.display();
          display_signal();
          display_battery();
          interface_menu(&menu_1_4);
          display.display();
          WiFi.begin(ssid, password);
          //Get_net(&ESP32_NET);
          break;
      }
      break;
  }

}


bool ei_camera_init(void) {

    if (is_initialised) return true;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

    // 初始化相机
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
      Serial.printf("相机初始化失败，错误码 0x%x\n", err);
      return false;
    }

    sensor_t * s = esp_camera_sensor_get();
    // 初始传感器垂直翻转并且颜色略饱和
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1); // 将其翻转回来
      s->set_brightness(s, 1); // 提高亮度一点点
      s->set_saturation(s, 0); // 降低饱和度
    }

#if defined(CAMERA_MODEL_M5STACK_WIDE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#elif defined(CAMERA_MODEL_ESP_EYE)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    s->set_awb_gain(s, 1);
#endif

    is_initialised = true;
    return true;
}


void ei_camera_deinit(void) {

    // 反初始化相机
    esp_err_t err = esp_camera_deinit();

    if (err != ESP_OK)
    {
        ei_printf("相机反初始化失败\n");
        return;
    }

    is_initialised = false;
    return;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;

    if (!is_initialised) {
        ei_printf("错误：相机未初始化\r\n");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
        ei_printf("相机捕获失败\n");
        return false;
    }

   bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);

   esp_camera_fb_return(fb);

   if(!converted){
       ei_printf("转换失败\n");
       return false;
   }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
        || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
        out_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf,
        img_width,
        img_height);
    }


    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // 我们已经有一个 RGB888 缓冲区，因此重新计算偏移量以得到像素索引
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix + 2];

        // 到下一个像素
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }
    // 完成！
    return 0;
}
extern void displayout(char *pt,int x,int y)
{
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(x,y);
    display.print(pt);
}
extern void displayout_s(const String pt,int x,int y)
{
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(x,y);
    display.print(pt);
}
