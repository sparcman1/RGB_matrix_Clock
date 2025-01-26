#include "DataTypes.h"

#include "backup.h"
#include "shell_mini.h"
#include "led_matrix.h"
#include "clock.h"
#include "weather.h"
#include "udust.h"

//////////////////////////////////////////////////////
// function define
void UploadServerStart();
//////////////////////////////////////////////////////
// Global Data var
TypeDef_Global_t GD;

///////////////////////////////////////////////////////
// Wi-Fi Soft AP Connect Information
const char *soft_ap_ssid     = "Weather_";
const char *soft_ap_password = "12345678";

///////////////////////////////////////////////////////
// Server object for service
AsyncWebServer upload_server(8080);

///////////////////////////////////////////////////////
// Global Timer Update
inline void gTimerUpdate() {
    GD.TickTime = millis();
}

///////////////////////////////////////////////////////
// Chip ID Read
uint32_t chipID_read() {
    uint32_t id = 0;
    for (int i = 0; i < 17; i = i + 8) {
        id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    //Serial.printf("%08X\n", id);

    return id;
}

///////////////////////////////////////////////////////
// AP Service start
void connectAP() {

    GD.chipID = chipID_read();
    Serial.print("ESP32 Mac Address : ");
    Serial.println(GD.chipID, HEX);

    // Soft AP Start
#if (CFG_WIFI_AP_ENABLE)
    WiFi.mode(WIFI_MODE_APSTA);

    char cBuf[64];
    sprintf(cBuf, "%s%04X", soft_ap_ssid, GD.chipID&0xffff);

    // host name 을 입력 하지 않으면 접속이 잘 안되는 문제가 있음
    WiFi.softAPsetHostname( cBuf );

    sprintf(cBuf, "%s%06X", soft_ap_ssid, GD.chipID);
    WiFi.softAP(cBuf, soft_ap_password);

    Serial.print("ESP32 IP as soft AP: ");
    Serial.println(WiFi.softAPIP());
#else
    WiFi.mode(WIFI_MODE_STA);
#endif

    // Upload Server start
    UploadServerStart();
}

///////////////////////////////////////////////////////
// WiFi Connect
typedef enum {
    CONNECT_MSG,
    CONNECT_WAIT,
    CONNECT_LOST_CHK,
    WIFI_RESET,
    CONNECT_HANG
}TypeDef_WiFi_Connect_t;

void connectWiFi() {

    if( GD.wifi_connected) return;

    ///////////////////////////////////////////////////////////////
    static unsigned long old_time;
    if ((GD.TickTime - old_time) >= 500)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////

    static TypeDef_WiFi_Connect_t state;
    static uint64_t loop_cnt;
    static int try_count;

    switch(state){
        case CONNECT_HANG : break;
        case WIFI_RESET :
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            delay(1000);
            WiFi.mode(WIFI_STA);
            state = CONNECT_MSG;
            break;
        case CONNECT_LOST_CHK :
            if(WiFi.status() != WL_CONNECTED) {

                GD.wifi_connected = false;
                Serial.println("Wi-Fi 연결이 끊어졌습니다. 재연결을 시도합니다...");

                if(++try_count >10 ){
                    state = WIFI_RESET;
                }
                else {
                    WiFi.disconnect();

                    state = CONNECT_MSG;
                }
            }
            else
            if(WiFi.status() == WL_CONNECTED){
                GD.wifi_connected = true;
            }
            break;
        case CONNECT_WAIT :
            if(WiFi.status() == WL_CONNECTED) {
                GD.wifi_connected = true;

                WiFi.setAutoReconnect(true);
                WiFi.persistent(true);

                Serial.println("");
                Serial.println("WiFi connected");
                Serial.println("IP address: ");
                Serial.println(WiFi.localIP());

                // NTP Client start
                timeClient.begin();

                //////////////////////////////////////////////////////////////////////////
                // WIFI logo output
                GD.logo_lock = true;
                virtualDisp->fillScreen(virtualDisp->color444(0, 1, 0));
                for (int r = 0; r < 255; r++) {
                    drawXbm565(0, 0, 64, 32, wifi_image1bit, virtualDisp->color565(r, 0, 0));
                    delay(10);
                }
                virtualDisp->clearScreen();
                GD.logo_lock = false;
                //////////////////////////////////////////////////////////////////////////

                drawMyIp();

                loop_cnt = 0;

                WebSocketStart();

                state = CONNECT_LOST_CHK;
            }
            else {
                if(loop_cnt++%64==0) Serial.println("");
                Serial.print(".");

                // 1분 동안 와이파이 접속이 안된다면 타임 오버
                if(loop_cnt++>=(2 * 60 * 1)){
                    Serial.println("WiFi connect Time Over!");

                    state = CONNECT_HANG;

                    GD.wifi_connected = false;
                }
            }
            break;
        default :
            // My Wi-Fi Connecting routine
            Serial.println();
            Serial.println();
            Serial.println("Connecting to ");
            Serial.println(GD.sys.wifi_ssid);

            //WiFi.hostname("WeatherClock");
            WiFi.setHostname((const char*)"WeatherClock");
            //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
            WiFi.begin(GD.sys.wifi_ssid, GD.sys.wifi_pass);

            state = CONNECT_WAIT;
            break;
    }

}

///////////////////////////////////////////////////////
// Upload Server Start
void UploadServerStart() {
    upload_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/update");
    });

    ElegantOTA.begin(&upload_server);    // Start AsyncElegantOTA
    upload_server.begin();

    Serial.println("Upload Server started...");
}

///////////////////////////////////////////////////////
// Setup
void setup() {
    delay(2000);

    Serial.begin(115200);  // 시리얼 통신 시작
    Serial.println("------------------------------------");
    Serial.println("Firmware version " FIRMWARE_VERSION);
    Serial.println("------------------------------------");
#if 0
    Serial.print("MOSI: ");
    Serial.println(MOSI);
    Serial.print("MISO: ");
    Serial.println(MISO);
    Serial.print("SCK: ");
    Serial.println(SCK);
    Serial.print("SS: ");
    Serial.println(SS);
#endif
    connectBACKUP();
    connectDisplay();
    connectRTC();
    connectAP();

    // 데이타 수신 전 초기 아이콘 표시를 위해 현재 값을 설정
    GD.udust.nowPM10 = GD.udust.nowPM25 = D_UNKNOWN;
}


void checkWiFiStatus() {
    static unsigned long lastCheck = 0;
    const unsigned long checkInterval = 5000; // 5초마다 체크

    if(millis() - lastCheck >= checkInterval) {
        lastCheck = millis();

        if(WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi connection lost. Reconnecting...");
            WiFi.reconnect();
        }

        // WiFi 상태 정보를 클라이언트에게 전송
        String statusJson = "{\"type\":\"wifi_status\",\"rssi\":" +
                          String(WiFi.RSSI()) +
                          ",\"status\":\"" +
                          (WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") +
                          "\"}";
        ws.textAll(statusJson);
    }
}

///////////////////////////////////////////////////////
// Loop
void loop() {
    gTimerUpdate();
    connectWiFi();
    clock_update();
    weather_update();
    udust_update();
    backup_update();
    bright_control();
    checkWiFiStatus();
    ElegantOTA.loop();
}
