#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>


extern void displayout(char *pt,int x,int y);
/********************显示信号图标********************/
extern void display_signal()
{
if(WiFi.status()==WL_CONNECTED)
  {
for(int x=115;x<116;x++)
{
  for(int y=5;y>4;y--)
  {
    displayout(".",x,y);
  }
}
for(int x=118;x<119;x++)
{
  for(int y=5;y>2;y--)
  {
    displayout(".",x,y);
  }
}
for(int x=122;x<123;x++)
{
  for(int y=5;y>0;y--)
  {
    displayout(".",x,y);
  }
}
//display.display();
}
}
