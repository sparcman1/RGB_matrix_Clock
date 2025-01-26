#ifndef _DATA_TYPE_H
#define _DATA_TYPE_H

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include "AsyncTCP.h"
#endif

#include <esp_event_legacy.h>

#include <HTTPClient.h>
#include "NTPClient.h"
#include <WiFiUdp.h>
#include <WiFiMulti.h>

// for WiFiOTA
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// for WebSerial
#include "ESPAsyncWebServer.h"
#include <ElegantOTA.h>
#include "WebSocket.h"

// for rtc
#include "RTClib.h"

// for hardware serial
#include <HardwareSerial.h>

// for weather icons
#define CFG_CAI_DISPLAY_ON    1     // 통합 대기질 표시

#define CFG_WEATHER_ICON_SIZE    16

#if(CFG_WEATHER_ICON_SIZE==32)
    #include "Dhole_weather_icons32px.h"

    #define __WX     0
    #define __WY     0
#else
    #include "Dhole_weather_icons16px.h"

#if(CFG_CAI_DISPLAY_ON==1)
    #define __WX     0
#else
    #define __WX     8
#endif
    #define __WY     0
#endif

// for uDust icons
#define CFG_DUST_ICON_SIZE      4
#define CFG_DUST_ICON_SIZE_Y    4

#include "uDust_icons16px.h"
#include "uDust_icons10x16px.h"
#include "uDust_icons4x4px.h"
#include "week_days_8x8px.h"
#include "number_10x16px.h"
#include "number_5x6px.h"
#include "colon_2x16px.h"

#if 1 /* for 4x4 px */
    #define __DX_PM10     47
    #define __DY_PM10     31-4

    #define __DX_PM25     __DX_PM10+5
    #define __DY_PM25     __DY_PM10

    #define __DX_CAI      __DX_PM25+5
    #define __DY_CAI      __DY_PM10
#else
    #define __DX_PM10     0
    #define __DY_PM10     16

    #define __DX_PM25     11
    #define __DY_PM25     16

    #define __DX_CAI      22
    #define __DY_CAI      16
#endif

#define CFG_MONTH_DISPLAY_ON    0     // Month Display

#if(CFG_MONTH_DISPLAY_ON==1)
    #define __MONTH_X_1   22
    #define __MONTH_X_2   __MONTH_X_1-3
    #define __MONTH_Y     1

    #define __DAY_X_1     19
    #define __DAY_X_2     __DAY_X_1-0
    #define __DAY_Y       9
#else
    #define __WEEK_X      5
    #define __WEEK_Y      16

    #define __DAY_X_1     6
    #define __DAY_X_2     __DAY_X_1-3
    #define __DAY_Y       24

    #define DATE_X        3
    #define DATE_Y        25
#endif

#define     TIME_START_X    18
#define     TIME_START_Y    5

#define     COLON_START_X   TIME_START_X*2+3
#define     COLON_START_Y   TIME_START_Y

#define     TEMPERATURE_START_X     18
#define     TEMPERATURE_START_Y     25

#include "LedPortConfig.h"

//#define FIRMWARE_VERSION "1.0.3"  // WebSerial 로 ssid, password, rss url 수정 할 수 있도록 변경
//#define FIRMWARE_VERSION "1.0.4"    // info 명령 추가
//#define FIRMWARE_VERSION "1.0.5"    // ElegantOTA 로 변경
//#define FIRMWARE_VERSION "1.0.6"    // WiFi Connect -> scheduler 로 변경
//#define FIRMWARE_VERSION "1.0.7"    // Serial 로 ssid, password, rss url 수정 할 수 있도록 변경
//#define FIRMWARE_VERSION "1.0.8"    // Bright dimming 제어 추가
//#define FIRMWARE_VERSION "1.0.9"    // RTC 와 NTP 동기화 기능 추가
//#define FIRMWARE_VERSION "1.1.0"    // Bright control on/off 가능 하도록 수정
//#define FIRMWARE_VERSION "1.1.1"    // Default Bright value 조정 명령 추가
//#define FIRMWARE_VERSION "1.1.2"    // EEPROM 데이타 표시 부분 Skip 및 Help 및 Info 명령어 버퍼 512로 키움
//#define FIRMWARE_VERSION "1.1.3"    // Clion 에서 제안 하는 대로 최적화 시킴
//#define FIRMWARE_VERSION "1.1.4"    // 날씨 아이콘 중첩 문제가 있어 수정함 (시험중)
//#define FIRMWARE_VERSION "1.1.5"    // Wi-Fi AP 시 SSID Mac Address(chipID) 로 수정
//#define FIRMWARE_VERSION "1.1.6"    // 날씨 아이콘 중첩 문제 Algorithm 수정 및, 밝기 Offset 기능 추가
//#define FIRMWARE_VERSION "1.1.7"    // icon x16 추가
//#define FIRMWARE_VERSION "1.1.8"    // uDust 기능 추가
//#define FIRMWARE_VERSION "1.1.9"    // uDust Icon 밝기 변경
//#define FIRMWARE_VERSION "1.2.0"    // CAI 아이콘 표출 하도록 변경
//#define FIRMWARE_VERSION "1.2.1"    // CAI 아이콘 표출 또는 안 하도록 설정 해서 변경
//#define FIRMWARE_VERSION "1.2.2"    // 날짜 및 요일 표시 하도록 변경
//#define FIRMWARE_VERSION "2.0.0"    // 날씨 및 시간 표시만 하도록 변경
//#define FIRMWARE_VERSION "2.0.1"    // 시간 표시에 문제 있어 변경
//#define FIRMWARE_VERSION "2.0.2"    // 온도 표시 되도록 수정 (폰트 크기 변경 )
//#define FIRMWARE_VERSION "2.0.3"    // Colon 그리는 타이머 인터럽트로 인해 화면에 쓰레기 점 찍히는 문제 있어 인터럽트 마스킹 적용
//#define FIRMWARE_VERSION "2.0.4"    // Air 정보 읽는 루틴 Total Item Count에 따라 Page 번호 변경 하도록 수정
//#define FIRMWARE_VERSION "2.0.5"    // 하절기/동절기에 나누어 Bright 조정 시간 변경
//#define FIRMWARE_VERSION "2.0.6"    // 낮/밤 아이콘 표시 위치를 날씨와 상관없이 변경하도록 수정
//#define FIRMWARE_VERSION "2.0.7"    // 와이파이 접속 후 와이파이 끊어 지면 일정 시간 지연 후 리셋 되도록 변경
//#define FIRMWARE_VERSION "2.0.8"    // 5분을 1분으로 출임
//#define FIRMWARE_VERSION "2.0.9"    // WiFi Reconnect 기능 추가
#define FIRMWARE_VERSION "2.1.0"    // 날씨 데이타 연속으로 시리얼 포트로 뿌리는 것을 변경 됐을 때만 뿌리도록 변경

//#define CFG_MAGIC_GOOD      0x55  // backup data 바뀌 때는 무조건 변경 해줘야 함
//#define CFG_MAGIC_GOOD      0x56
//#define CFG_MAGIC_GOOD      0x57
//#define CFG_MAGIC_GOOD      0x58
//#define CFG_MAGIC_GOOD      0x59
//#define CFG_MAGIC_GOOD      0x5A
//#define CFG_MAGIC_GOOD      0x5B
#define CFG_MAGIC_GOOD      0x5C

#define CFG_MAGIC_UPDATE    0xAA

#define CFG_ARG_CNT         6
#define CFG_ARG_LENGTH      256

//////////////////////////////////////////////////////
// Bright define
#define CFG_BRIGHT_LOW_LOW  3
#define CFG_BRIGHT_LOW      5
#define CFG_BRIGHT_MID_LOW  10
#define CFG_BRIGHT_MID      50
#define CFG_BRIGHT_HIGH     80
#define CFG_BRIGHT_MAX      255

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for my Wi-Fi
#define CFG_WIFI_AP_ENABLE      1

#define MY_WIFI_SSID        "my_ssid"
#define MY_WIFI_PASS        "my_passwd"
#define MY_LOCATION_RSS     "my_location"
#define MY_DUST_STATION_NAME    "my_dong"


#define MY_DUST_URL             "Owner"
#define MY_DUST_SIDO            "경기"

#define MY_DUST_PAGE            1
#define MY_KEY_1                "MyKey"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for weather icon change
#define MORNING_DETECT_HOUR        6
#define MORNING_DETECT_MINUTE      0

#define EVENING_DETECT_HOUR        19
#define EVENING_DETECT_MINUTE      0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AP SSID LIST
typedef struct wifi_ap_t{
    const char* ssid;
    const char* pass;
}TypeDef_WiFi_AP_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for clients
typedef enum {
    REQUEST_SEND,
    HEADER_RCV_WAIT,
    DATA_PARSING,
} TypeDef_ClientState_t;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for mini shell
typedef struct shell_t{
    bool    event;
    uint8_t argc;
    char    argv[CFG_ARG_CNT][CFG_ARG_LENGTH];
}TypeDef_Shell_t;

typedef struct cmd_list{            /* cmd_table[] entries prototype    */
    char *cmd;
    void (*func)();
    char *help;
} CMD_LIST;

//const char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for weather
typedef struct weather_t {
    bool event;
    String Temperature;     // 온도
    String Humidity;     // 습도
    String WfEn;     // 날씨(영어)
    String oldWfEn;  // 날씨(영어)
    String WfKor;    // 날씨(한글)
}TypeDef_Weather_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for udust
typedef enum dust_status_t{
    D_INIT,
    D_UNKNOWN,
    D_GOOD,
    D_NORMAL,
    D_BAD,
    D_WORST
}TypeDef_DustStatus_t;

typedef struct udust_t {
    bool manual;

    TypeDef_DustStatus_t nowCAI,  oldCAI;
    TypeDef_DustStatus_t nowPM10, oldPM10;
    TypeDef_DustStatus_t nowPM25, oldPM25;

    String khaiValue;
    String so2Value;
    String coValue;
    String pm25Value;
    String pm10Value;
    String dataTime;
    String no2Value;
    String stationName;
    String o3Value;
}TypeDef_uDust_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for flash backup
typedef struct backup_t{
  uint8_t magic_code;   // magic code
  char wifi_ssid[16];   // WIFI ID
  char wifi_pass[16];   // WIFI PASSWORD
  char rss_url[128];    // Weather site RSS Code

  bool    bright_ctrl_on;  // bright control auto on

  uint8_t bright;       // default day bright value
  uint8_t bright_day_offset;

  char dust_url[256];       //
  char my_sido[32];         // 경기
  char my_station[32];    // 천천동 , 광교동, 영통동
  char my_key[256];

  uint8_t my_page;

  uint8_t bright_night_offset;     // default night bright value

}TypeDef_Backup_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Data
typedef struct global_t{
    // for global tick time
    uint64_t TickTime;

    // for system backup data
    TypeDef_Backup_t  sys;

    TypeDef_Weather_t weather;

    TypeDef_uDust_t udust;

    // for chip id
    uint32_t chipID;

    // for rtc (ds3231)
    RTC_DS3231 rtc;
    DateTime rtc_now;   // rtc 에서 읽어 들인 현재 시간
    volatile bool rtc_connected;

    // for Wi-Fi connect
    volatile bool wifi_connected;

    // for led matrix brightness
    uint8_t Brightness;

    // for serial input
    char ser_buf[CFG_ARG_LENGTH];
    uint16_t ser_in, ser_out;

    bool weather_icon_clear_req;
    bool weather_icon_clear_complete;

    bool night;

    bool colon_lock;    // colon display lock
    bool logo_lock;    // logo display lock
}TypeDef_Global_t;


extern TypeDef_Global_t GD;

extern void(* resetFunc) ();

#endif