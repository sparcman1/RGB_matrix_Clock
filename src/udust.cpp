//
// Created by sparcman on 2024-02-24.
//

#include "udust.h"

#include <shell_mini.h>

#define DUST_GOOD_COLOR()     virtualDisp->color565(0, 100, 80)
#define DUST_NORM_COLOR()     virtualDisp->color565(0, 100, 6)
#define DUST_BAD_COLOR()      virtualDisp->color565(100,56, 4)
#define DUST_WORST_COLOR()    virtualDisp->color565(150, 0, 0)
#define DUST_UNKNOWN_COLOR()  virtualDisp->color565(14, 21, 9)

// for Dust server
const char *host_dust = "apis.data.go.kr";

WiFiClient DustClient;

///////////////////////////////////////////////////////
// Client object for service
const int httpPort = 80;

///////////////////////////////////////////////////////
// Get dust Data read
String utf8(String input){
    String output;
    for(int i = 0;i<input.length();i++){
        output += "%" + String(input[i],HEX);
        //Serial.println(text1[i],HEX);
    }
    return output;
}

bool getDustData() {
    bool rtn = false;

    ///////////////////////////////////////////////////////////////
    static unsigned long old_time, cmp_time = 100;
    if ((GD.TickTime - old_time) >= cmp_time)
        old_time = GD.TickTime;
    else return rtn;
    ///////////////////////////////////////////////////////////////

    static TypeDef_ClientState_t state;

    // 미세먼지 관련 데이타 페이지 자동 검색 하기 위해 변수 지정
    static uint8_t PageNum = GD.sys.my_page;
    static int total_count, page_max;

    bool detectF = false;

    switch (state) {
        case DATA_PARSING:
            while (DustClient.available()) {
                String line = DustClient.readStringUntil('\e');

                //만약에 현재 하나의 line을 서버로부터 수신했는데,
                //그 line에 {"response": 문자열이 포함이 되어있다면~
                DynamicJsonDocument doc(6144);
                deserializeJson(doc, line);

                JsonObject response_body = doc["response"]["body"];
                total_count = response_body["totalCount"]; // 125

                // total count 값을 확인해 10개씩 나누어 몇 페이지 나오는지 확인해 적용
                if(total_count!=0){
                    String message = "totalCount = " + String(total_count) + "\n";

                    page_max  = (total_count / 10);
                    page_max += (total_count % 10) ? 1 : 0;

                    message = "PageMax = " + String(page_max);

                    ws_println(message.c_str());


                    for (int i = 0; i < 10; i++) {
                        JsonObject jo = response_body["items"][i];
                        String khaiValue = jo["khaiValue"]; // "116"
                        String so2Value = jo["so2Value"]; // "0.004"
                        String coValue = jo["coValue"]; // "0.8"
                        String pm10Value = jo["pm10Value"]; // "55"

                        String pm25Value = jo["pm25Value"]; // "55"
                        String sidoName = jo["sidoName"]; // "55"
                        String dataTime = jo["dataTime"]; // "2020-12-28 20:00"
                        String no2Value = jo["no2Value"]; // "0.050"
                        String stationName = jo["stationName"]; // "광진구"
                        String o3Value = jo["o3Value"]; // "0.003"

                        String message;

                        message += "------" + String(i) + "------\n";
                        message += "Station=" + stationName + "\n";
                        message += "Time   =" + dataTime + "\n";
                        message += "CAI    =" + khaiValue + "\n";       // 통합 대기 환경 지수
                        message += "SO2    =" + so2Value + "PPM" + "\n"; // 아황 가스
                        message += "CO     =" + coValue + "PPM" + "\n";
                        message += "PM10   =" + pm10Value + "ug/m3" + "\n";
                        message += "PM2.5  =" + pm25Value + "ug/m3" + "\n";
                        message += "O3     =" + o3Value + "PPM" + "\n"; // 오존

                        ws_print(message.c_str());

                        if (!strcmp(stationName.c_str(), GD.sys.my_station)) {
                            GD.udust.khaiValue = khaiValue;
                            GD.udust.so2Value = so2Value;
                            GD.udust.coValue = coValue;
                            GD.udust.pm25Value = pm25Value; // "55"
                            GD.udust.pm10Value = pm10Value; // "55"
                            GD.udust.dataTime = dataTime; // "2020-12-28 20:00"
                            GD.udust.no2Value = no2Value; // "0.050"
                            GD.udust.o3Value = o3Value;

                            Serial.println("==========================================");
                            Serial.println("sidoName =" + sidoName);
                            Serial.println("Station  =" + stationName);
                            Serial.println("Time     =" + dataTime);
                            Serial.println("CAI      =" + khaiValue);       // 통합 대기 환경 지수
                            Serial.println("SO2      =" + so2Value + "PPM"); // 아황 가스
                            Serial.println("CO       =" + coValue + "PPM");
                            Serial.println("NO2      =" + no2Value + "PPM"); //
                            Serial.println("PM10     =" + pm10Value + "ug/m3");
                            Serial.println("PM2.5    =" + pm25Value + "ug/m3");
                            Serial.println("O3       =" + o3Value + "PPM"); // 오존
                            Serial.println("==========================================");

                            rtn = detectF = true;
                        }
                    }
                }
                //Serial.println(line);
                //WebSerial.println(line);
            }

            // Change to check every 5 minutes secs from the second time
            cmp_time = detectF ? 60000*60 : 10000;

            // 미세 먼지 페이지 페이지 검색을 빠르게 하기 위해 Seeking 하도록 만듦
            PageNum = detectF ? PageNum : PageNum+1;

            PageNum = (page_max < PageNum) ? 1:  PageNum;

            state = REQUEST_SEND;
            break;

        case HEADER_RCV_WAIT:
            while (DustClient.connected()) {
                String line = DustClient.readStringUntil('\n');

                if (line == "\r") {
                    ws_println(line.c_str());
                    break;
                }
            }
            state = DATA_PARSING;
            break;

        default:
            if (DustClient.connect(host_dust, httpPort)) {
                //String url = "/B552584/ArpltnInforInqireSvc/getCtprvnRltmMesureDnsty?serviceKey="+String(MY_KEY)+"&returnType=json&numOfRows=10&pageNo="+String(GD.sys.my_page)+"&sidoName=" + utf8(GD.sys.my_sido);
                String url = String(GD.sys.dust_url)+String(GD.sys.my_key)+"&returnType=json&numOfRows=10&pageNo="+String(PageNum)+"&sidoName="+utf8(GD.sys.my_sido)+"&ver=1.3";

                DustClient.print(
                        String("GET ") +
                        url + " HTTP/1.1\r\n" +
                        "Host: " + host_dust + "\r\n" +
                        "Connection: close\r\n\r\n"
                );

                String message = "udust request sent page = " +String(PageNum);
                ws_println(message.c_str());

                // Change to 100msec for data reception confirmation
                cmp_time = 100;
                state = HEADER_RCV_WAIT;
            }
            else rtn = false;
            break;
    }

    return rtn;
}

///////////////////////////////////////////////////////
// udust draw khai (CAI 통합 대기질)
void udust_draw_CAI() {
    static uint8_t width = CFG_DUST_ICON_SIZE, height = CFG_DUST_ICON_SIZE_Y;

    uint16_t khai = strtoul(GD.udust.khaiValue.c_str(), NULL, 10);

    ///////////////////////////////////////////////////////////////////////
    // khai value 처리
    if (0<=khai && khai<=50){
        GD.udust.nowCAI = D_GOOD;      //파랑색
    }
    else if (50<khai && khai<=100){
        GD.udust.nowCAI = D_NORMAL;    //녹색
    }
    else if (100<khai && khai<=250){
        GD.udust.nowCAI = D_BAD;       //주황색
    }
    else if(250<khai && khai<=500){
        GD.udust.nowCAI = D_WORST;     //빨간색
    }
    else GD.udust.nowCAI = D_UNKNOWN;

    if(GD.udust.nowCAI != GD.udust.oldCAI){

        Serial.println("udust CAI Change..."+String(khai));

        // 이전 그림 지우기
        drawXbm565(__DX_CAI, __DY_CAI, width, height, dust_4x4_fill, 0);  //

        GD.udust.oldCAI = GD.udust.nowCAI;

        // 상태에 따라 색상 분리
        uint16_t color;
        char const *icon;

        switch(GD.udust.nowCAI){
            case D_GOOD   :
                color = DUST_GOOD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_NORMAL :
                color = DUST_NORM_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_BAD    :
                color = DUST_BAD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_WORST  :
                color = DUST_WORST_COLOR();
                icon  = dust_4x4_fill;
                break;
            default :
                color = DUST_UNKNOWN_COLOR();
                icon  = dust_4x4_null;
        }

        drawXbm565(__DX_CAI, __DY_CAI, width, height, icon, color);
    }
}

///////////////////////////////////////////////////////
// udust draw pm10
void udust_draw_pm10() {
    static uint8_t width = CFG_DUST_ICON_SIZE, height = CFG_DUST_ICON_SIZE_Y;
    uint16_t pm_10 = strtoul(GD.udust.pm10Value.c_str(), NULL, 10);

    ///////////////////////////////////////////////////////////////////////
    // PM 10 처리
    if (0<=pm_10 & pm_10<=30){
        GD.udust.nowPM10 = D_GOOD;      //파랑색 = PM10 0~ 30
    }
    else if (30<pm_10 && pm_10<=80){
        GD.udust.nowPM10 = D_NORMAL;   //녹색 = PM10 30~ 80
    }
    else if (80<pm_10 && pm_10<=150){
        GD.udust.nowPM10 = D_BAD;   ///주황색 = PM10 80~150
    }
    else if(151 < pm_10){
        GD.udust.nowPM10 = D_WORST;   ///빨간색 = PM10 151 이상
    }
    else GD.udust.nowPM10 = D_UNKNOWN;

    if(GD.udust.nowPM10 != GD.udust.oldPM10){

        Serial.println("udust PM10 Change..."+String(pm_10));

        // 이전 그림 지우기
        drawXbm565(__DX_PM10, __DY_PM10, width, height, dust_4x4_fill, 0);  //

        GD.udust.oldPM10 = GD.udust.nowPM10;

        // 상태에 따라 색상 분리
        uint16_t color;
        char const *icon;

        switch(GD.udust.nowPM10){
            case D_GOOD   :
                color = DUST_GOOD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_NORMAL :
                color = DUST_NORM_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_BAD    :
                color = DUST_BAD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_WORST  :
                color = DUST_WORST_COLOR();
                icon  = dust_4x4_fill;
                break;
            default :
                color = DUST_UNKNOWN_COLOR();
                icon  = dust_4x4_null;
        }

        drawXbm565(__DX_PM10, __DY_PM10, width, height, icon, color);
    }
}

///////////////////////////////////////////////////////
// udust draw pm25
void udust_draw_pm25() {
    static uint8_t width = CFG_DUST_ICON_SIZE, height = CFG_DUST_ICON_SIZE_Y;

    uint16_t pm_25 = strtoul(GD.udust.pm25Value.c_str(), NULL, 10);

    ///////////////////////////////////////////////////////////////////////
    // PM 2.5 처리
    if (0<=pm_25 && pm_25<=15){
        GD.udust.nowPM25 = D_GOOD;      //파랑색
    }
    else if (15<pm_25 && pm_25<=50){
        GD.udust.nowPM25 = D_NORMAL;    //녹색
    }
    else if (50<pm_25 && pm_25<=100){
        GD.udust.nowPM25 = D_BAD;       //주황색
    }
    else if(101 < pm_25){
        GD.udust.nowPM25 = D_WORST;     //빨간색
    }
    else GD.udust.nowPM25 = D_UNKNOWN;

    if(GD.udust.nowPM25 != GD.udust.oldPM25){

        Serial.println("udust PM25 Change..."+String(pm_25));

        // 이전 그림 지우기
        drawXbm565(__DX_PM25, __DY_PM25, width, height, dust_4x4_fill, 0);  //

        GD.udust.oldPM25 = GD.udust.nowPM25;

        // 상태에 따라 색상 분리
        uint16_t color;
        char const *icon;

        switch(GD.udust.nowPM25){
            case D_GOOD   :
                color = DUST_GOOD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_NORMAL :
                color = DUST_NORM_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_BAD    :
                color = DUST_BAD_COLOR();
                icon  = dust_4x4_fill;
                break;
            case D_WORST  :
                color = DUST_WORST_COLOR();
                icon  = dust_4x4_fill;
                break;
            default :
                color = DUST_UNKNOWN_COLOR();
                icon  = dust_4x4_null;
        }

        drawXbm565(__DX_PM25, __DY_PM25, width, height, icon, color);
    }
}

///////////////////////////////////////////////////////
// udust Update
void udust_update() {

    // Use only when connected to Wi-Fi
    if(!GD.wifi_connected) return;

    if(!getDustData() && !GD.udust.manual) return;

    if(GD.udust.manual) {
        Serial.println("udust manual Change...");
        GD.udust.manual = false;
    }

#if(CFG_CAI_DISPLAY_ON==1)
    udust_draw_CAI();
#endif
    udust_draw_pm10();
    udust_draw_pm25();

}