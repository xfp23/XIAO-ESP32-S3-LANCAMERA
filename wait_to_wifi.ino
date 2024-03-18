#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

extern void display_signal();
extern void display_battery();
extern void displayout(char *pt, int x, int y);

extern void wait_to_wifi() {
  static unsigned int point_x = 20, point_y = 30; // 声明为静态变量
  display_battery();
  display_signal();
  while (WiFi.status() != WL_CONNECTED) {
    display_battery();
    displayout("Connecting to WiFi", 11, 20);
    point_x += 5;
    displayout(".", point_x, point_y);
    display.display();
    delay(500);
    if(point_x>=60)
    {
      display.clearDisplay();
      display.display();
      point_x=20;
      }
  }
}
