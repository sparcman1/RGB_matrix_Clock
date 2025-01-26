//
// Created by sparcman on 2024-02-23.
//

#include "weather.h"
#include "shell_mini.h"

WiFiClient WeatherClient;

// for Weather server
//const char *host_weather = "www.kma.go.kr";
const char *host_weather = "www.weather.go.kr";

static const int httpPort = 80;
///////////////////////////////////////////////////////
// Get Weather Data

int8_t getWeatherData() {
    int8_t rtn = 0;
    ///////////////////////////////////////////////////////////////
    static unsigned long old_time, cmp_time = 100;
    if ((GD.TickTime - old_time) >= cmp_time)
        old_time = GD.TickTime;
    else return rtn;
    ///////////////////////////////////////////////////////////////

    static TypeDef_ClientState_t state;

    switch (state) {
        case DATA_PARSING:
            while (WeatherClient.available()) {
                String line = WeatherClient.readStringUntil('\e');

                // temperature
                int temp = line.indexOf("</temp>");
                if (temp > 0) {
                    String tmp_str = "<temp>";
                    String wt_temp = line.substring(line.indexOf(tmp_str) + tmp_str.length(), temp);
                    GD.weather.Temperature = wt_temp;
                }

                //Humidity
                int reh = line.indexOf("</reh>");
                if (reh > 0) {
                    String tmp_str = "<reh>";
                    String wt_humi = line.substring(line.indexOf(tmp_str) + tmp_str.length(), reh);
                    GD.weather.Humidity = wt_humi;
                }

                //weather english
                int wfEn = line.indexOf("</wfEn>");
                if (wfEn > 0) {
                    String tmp_str = "<wfEn>";
                    String wt_wfEn = line.substring(line.indexOf(tmp_str) + tmp_str.length(), wfEn);
                    GD.weather.WfEn = wt_wfEn;
                }

                //weather korean
                int wfKor = line.indexOf("</wfKor>");
                if (wfKor > 0) {
                    String tmp_str = "<wfKor>";
                    String wt_wfKor = line.substring(line.indexOf(tmp_str) + tmp_str.length(), wfKor);
                    GD.weather.WfKor = wt_wfKor;
                }

                rtn = 1;
            }
            // Change to check every 10 minutes from the second time
            cmp_time = 120000;

            state = REQUEST_SEND;
            break;

        case HEADER_RCV_WAIT:
            while (WeatherClient.connected()) {
                String line = WeatherClient.readStringUntil('\n');

                if (line == "\r") {
                    //Serial.println("headers received");
                    //WebSerial.println("headers received");
                    break;
                }
            }
            state = DATA_PARSING;
            break;

        default:
            if (WeatherClient.connect(host_weather, httpPort)) {
                WeatherClient.print(String("GET ") + GD.sys.rss_url + " HTTP/1.1\r\n" + "Host: " + host_weather + "\r\n" + "Connection: close\r\n\r\n");
                //Serial.println("request sent");
                //WebSerial.println("request sent");

                // Change to 100msec for data reception confirmation
                cmp_time = 100;
                state = HEADER_RCV_WAIT;
            }
            else rtn = -1;
            break;
    }

    return rtn;
}

///////////////////////////////////////////////////////
// draw temperature
void draw_temp(const String t, uint16_t color) {
    char cbuf[10];
    int8_t lTemp;

    uint16_t x = TEMPERATURE_START_X, y = TEMPERATURE_START_Y;   // 온도 표시

    // temperature display
    if (t.length() > 4) return;

    strcpy(cbuf, t.c_str());
    lTemp = atoi(cbuf);

    if (lTemp < 0) {
        // - display
        drawXbm565(x, y, 5, 6, num_5x6h_b, color);

        x += 6;
        lTemp *= -1;
    }

    // 0~99 까지 값 이라면
    if (lTemp / 10) {
        drawXbm565(x, y, 5, 6, num_5x6[lTemp / 10], color);
        x += 6;
        drawXbm565(x, y, 5, 6, num_5x6[lTemp % 10], color);
    } else {
        drawXbm565(x, y, 5, 6, num_5x6[lTemp], color);
    }

    x += 6;

    // `c` display
    drawXbm565(x, y, 5, 6, num_5x6h_a, color);
}

///////////////////////////////////////////////////////
//  Weather Update
void weather_update() {
    uint16_t color;
    char cBuf[64];
    int Temp_int, Humidity_int;

    bool chgF = false;

    static int unknown_count;
    static int old_Temp_int, old_Humidity_int;
    static uint8_t width = CFG_WEATHER_ICON_SIZE, height = CFG_WEATHER_ICON_SIZE;

    ///////////////////////////////////////////////////////////////
    static unsigned long old_time;
    if ((GD.TickTime - old_time) >= 100)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////

    // Run below routine only when connected to wifi
    if(!GD.wifi_connected) return;

    if (getWeatherData()==-1) {
        GD.weather.WfEn = "Unknown";

        goto DAY_NIGHT_CHECK;
    }

    Temp_int = GD.weather.Temperature.toInt();
    Humidity_int = GD.weather.Humidity.toInt();

#if 1
    if (old_Temp_int != Temp_int) {
        chgF = true;

        sprintf(cBuf, "%d", old_Temp_int);

        String sTemp = cBuf;

        draw_temp(sTemp, 0); // black

        // FIX ME
        old_Temp_int = Temp_int;
    }

    // 온도에 따라 색깔 구분 되게 변경
    if(Temp_int<=0)
        color = virtualDisp->color565(30, 0, 150);
    else
    if(10 > Temp_int && Temp_int >= 1)
        color = virtualDisp->color565(51, 153, 150);
    else
    if(20 > Temp_int && Temp_int >= 10)
        color = virtualDisp->color565(120, 51, 80);
    else
    if(30 > Temp_int && Temp_int >= 20)
        color = virtualDisp->color565(180, 0, 180);
    else
    if(Temp_int >= 30)
        color = virtualDisp->color565(255, 0, 0);

    draw_temp(GD.weather.Temperature, color); // orange
#else
    // 글자 크기 설정
    virtualDisp->setTextSize(1);

    if ((old_Temp_int != Temp_int) || (old_Humidity_int != Humidity_int)) {
        // Write the same data in black to prevent overlap
        virtualDisp->setTextColor(virtualDisp->color444(0, 0, 0));  // Black

        // temperature display
        virtualDisp->setCursor(38, 13);
        virtualDisp->println(old_Temp_int);
        virtualDisp->setCursor(48, 7);
        virtualDisp->println(".");
        virtualDisp->setCursor(53, 13);
        virtualDisp->println("C");

        // humidity display
        virtualDisp->setCursor(38, 22);
        virtualDisp->print(old_Humidity_int);
        virtualDisp->setCursor(52, 22);
        virtualDisp->println("%");

        // FIX ME
        old_Temp_int = Temp_int;
        old_Humidity_int = Humidity_int;
    }

    // temperature display
    virtualDisp->setTextColor(virtualDisp->color444(0, 8, 0));  // Green

    virtualDisp->setCursor(38, 13);
    virtualDisp->println(Temp_int);
    virtualDisp->setCursor(48, 7);
    virtualDisp->println(".");
    virtualDisp->setCursor(53, 13);
    virtualDisp->println("C");

    // humidity display
    virtualDisp->setTextColor(virtualDisp->color444(8, 8, 0));  // Yellow

    virtualDisp->setCursor(38, 22);
    virtualDisp->print(Humidity_int);
    virtualDisp->setCursor(52, 22);
    virtualDisp->println("%");
#endif
    // Clear 맑음
    // Cloudy 흐림
    // Mostly Cloudy 구름 많음
    // Shower 소나기
    // Rain 비
    // Snow 눈

    /*----------날씨 별 아이콘 출력-------*/
    static bool oldNight = false;

    if ((GD.weather.oldWfEn != GD.weather.WfEn) || (oldNight != GD.night)) {
        chgF = true;

        if (oldNight != GD.night) {
            oldNight = GD.night;

            sprintf(cBuf, "(%s) %s change detect...\n", __FUNCTION__, GD.night ? "Night":"Day");
            ws_print(cBuf);
        }
        else
        if (GD.weather.oldWfEn != GD.weather.WfEn){
            GD.weather.oldWfEn = GD.weather.WfEn;

            sprintf(cBuf, "(%s) Weather change detect...\n", __FUNCTION__);
            ws_print(cBuf);
        }
    }

DAY_NIGHT_CHECK:
    if (chgF) {
        String message;
        message += " 온도= " + GD.weather.Temperature;
        message += ",습도= " + GD.weather.Humidity;
        message += ",날씨= " + GD.weather.WfEn;
        message += "(" + GD.weather.WfKor + ")";
        ws_println(message.c_str());
    }
    // icon clear
    drawXbm565(__WX, __WY, width, height, icon_bits[22], 0);  // icon_flush

    if (!GD.night) {  // 낮시간 날씨 아이콘 표시
        if (GD.weather.WfEn == "Clear")
            drawXbm565(__WX, __WY, width, height, icon_bits[20]);  // sun_bits
        else if (GD.weather.WfEn == "Party Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[1]);  // cloud_sun_bits
        else if (GD.weather.WfEn == "Mostly Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[6]);  // cloud_bits
        else if (GD.weather.WfEn == "Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[2]);  // clouds_bits
        else if (GD.weather.WfEn == "Rain")
            drawXbm565(__WX, __WY, width, height, icon_bits[12]);  // rain1_sun_bits
        else if (GD.weather.WfEn == "Snow/Rain")
            drawXbm565(__WX, __WY, width, height, icon_bits[16]);  // rain_snow_bits
        else if (GD.weather.WfEn == "Snow")
            drawXbm565(__WX, __WY, width, height, icon_bits[18]);  // snow_sun_bits
        else {
            drawXbm565(__WX, __WY, width, height, icon_bits[23]);  // unknown_bits}

            // 1분 동안 정상적으로 데이타 수신이 어려우면 Reset 되도록 만듦
            if(unknown_count++> (10 * 60 * 1)) {
                resetFunc(); //call reset
            }
        }
    } else {
        if (GD.weather.WfEn == "Clear")
            drawXbm565(__WX, __WY, width, height, icon_bits[8]);  // moon_bits
        else if (GD.weather.WfEn == "Party Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[0]);  // cloud_moon_bits
        else if (GD.weather.WfEn == "Mostly Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[6]);  // cloud_bits
        else if (GD.weather.WfEn == "Cloudy")
            drawXbm565(__WX, __WY, width, height, icon_bits[2]);  // clouds_bits
        else if (GD.weather.WfEn == "Rain")
            drawXbm565(__WX, __WY, width, height, icon_bits[11]);  // rain1_moon_bits
        else if (GD.weather.WfEn == "Snow/Rain")
            drawXbm565(__WX, __WY, width, height, icon_bits[16]);  // rain_snow_bits
        else if (GD.weather.WfEn == "Snow")
            drawXbm565(__WX, __WY, width, height, icon_bits[17]);  // snow_moon_bits
        else {
            drawXbm565(__WX, __WY, width, height, icon_bits[23]);  // unknown_bits}

            // 1분 동안 정상적으로 데이타 수신이 어려우면 Reset 되도록 만듦
            if(unknown_count++> (10 * 60 * 5)) {
                resetFunc(); //call reset
            }
        }
    }
}