#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define TEXT_X 10

#define IPSIZE 20

/* 获取网络信息结构体 */

/*未知原因：无法初始化结构体成员后将ip地址打印到屏幕上，选择直接打印，每次调用库*/

/*struct Network {
   const char IPADDRESS[IPSIZE];
   const char GATEWAY[IPSIZE];
   const char DNS[IPSIZE];
};

void Get_net(struct Network *net) {
    strncpy(net->IPADDRESS, WiFi.localIP().toString().c_str(), IPSIZE - 1);
    net->IPADDRESS[IPSIZE - 1] = '\0'; // 确保字符串以空字符结尾
    strncpy(net->GATEWAY, WiFi.gatewayIP().toString().c_str(), IPSIZE - 1);
    net->GATEWAY[IPSIZE - 1] = '\0'; // 确保字符串以空字符结尾
    strncpy(net->DNS, WiFi.dnsIP().toString().c_str(), IPSIZE - 1);
    net->DNS[IPSIZE - 1] = '\0'; // 确保字符串以空字符结尾
}

*/

/* 输出函数 */
extern void displayout(char *pt, int x, int y);
extern void displayout_s(const String pt, int x, int y);

extern const char *ssid;
extern const char *password;

/* 定义屏幕显示内容结构体 */
struct interface {
    unsigned int first_text_x;
    unsigned int first_text_y;
    char *first_text;
    unsigned int second_text_x;
    unsigned int second_text_y;
    char *second_text;
    unsigned int third_text_x;
    unsigned int third_text_y;
    char *third_text;
    unsigned int fourth_text_x;
    unsigned int fourth_text_y;
    char *fourth_text;
};

/* 定义显示菜单的函数 */
void interface_menu(struct interface *menu) {
    displayout(menu->first_text, menu->first_text_x, menu->first_text_y);
    displayout(menu->second_text, menu->second_text_x, menu->second_text_y);
    displayout(menu->third_text, menu->third_text_x, menu->third_text_y);
    displayout(menu->fourth_text, menu->fourth_text_x, menu->fourth_text_y);
}

/*输出网络信息*/
void Internet() {
    displayout_s(WiFi.localIP().toString().c_str(), 50, 17);
    displayout_s(WiFi.dnsIP().toString().c_str(), 50, 27);
    displayout_s(WiFi.gatewayIP().toString().c_str(), 50, 37);
    displayout_s("xfp23", 50, 47);
}


/* 定义菜单项结构体 */
struct interface menu_1 = {
    .first_text_x = TEXT_X,
    .first_text_y = 17,
    .first_text = "NetWork",
    .second_text_x = TEXT_X,
    .second_text_y = 27,
    .second_text = "Face dection",
    .third_text_x = TEXT_X,
    .third_text_y = 37,
    .third_text = "LAN camera",
    .fourth_text_x = TEXT_X,
    .fourth_text_y = 47,
    .fourth_text = "Restart network",
};

struct interface menu_1_1 = {
    .first_text_x = 5,
    .first_text_y = 17,
    .first_text = "IP: ",
    .second_text_x = 5,
    .second_text_y = 27,
    .second_text = "Gateway: ",
    .third_text_x = 5,
    .third_text_y = 37,
    .third_text = "DNS: ",
    .fourth_text_x = 5,
    .fourth_text_y = 47,
    .fourth_text = "SSID: ",
};

struct interface menu_1_2 = {
    .first_text_x = 46,
    .first_text_y = 4,
    .first_text = "Dection",
    .second_text_x = TEXT_X,
    .second_text_y = 17,
    .second_text = "Object dection",
    .third_text_x = TEXT_X,
    .third_text_y = 27,
    .third_text = "Dection resultS:",
    .fourth_text_x = TEXT_X,
    .fourth_text_y = 47,
    .fourth_text = " "
};

struct interface menu_1_3 = {
    .first_text_x = 46,
    .first_text_y = 4,
    .first_text = "IPC",
    .second_text_x = TEXT_X,
    .second_text_y = 17,
    .second_text = "Enter the IP address view",
    .third_text_x = TEXT_X,
    .third_text_y = 27,
    .third_text = " ",
    .fourth_text_x = TEXT_X,
    .fourth_text_y = 47,
    .fourth_text = " "
};

struct interface menu_1_4 = {
    .first_text_x = 46,
    .first_text_y = 4,
    .first_text = "REBOOT",
    .second_text_x = TEXT_X,
    .second_text_y = 17,
    .second_text = "Network reconnecting   ......",
    .third_text_x = TEXT_X,
    .third_text_y = 27,
    .third_text = " ",
    .fourth_text_x = TEXT_X,
    .fourth_text_y = 47,
    .fourth_text = " "
};
