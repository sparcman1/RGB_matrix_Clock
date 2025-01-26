//
// Created by sparcman on 2024-02-23.
//

#include "clock.h"
#include "shell_mini.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org", 32400); // 1시간당 3600 : 32400 +9시간 << 한국 시간 기준

hw_timer_t *timer = nullptr;

void IRAM_ATTR onTimer(){
    colon_update();
}

void interrupt_init(){//timer interrupt freq is 80Mhz
    timer = timerBegin(0,80, true);//division 80=1Mhz
    timerAttachInterrupt(timer,&onTimer,true);
    //timerAlarmWrite(timer,1000000,true); //count 1000000 = 1sec,1000=1msec
    timerAlarmWrite(timer,500000,true); //count 500000 = 0.5sec,1000=1msec
    timerAlarmEnable(timer);
}

///////////////////////////////////////////////////////
//
void rtc_read() {
    if (!GD.rtc_connected) return;

    ///////////////////////////////////////////////////////////////
    static unsigned long old_time;
    if ((GD.TickTime - old_time) >= 1000)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////

    GD.rtc_now = GD.rtc.now();
/*
  ws_println(now.second(), DEC);
  ws_println(now.minute(), DEC);
  ws_println(now.hour(), DEC);
  ws_println(now.day(), DEC);
  ws_println(now.month(), DEC);
  ws_println(now.year(), DEC);
*/
/*
  char cBuf[32];

  sprintf(cBuf, "RTC : %02d-%02d-%02d, %02d:%02d:%02d\n", GD.rtc_now.year(), GD.rtc_now.month(), GD.rtc_now.day(), GD.rtc_now.hour(), GD.rtc_now.minute(), GD.rtc_now.second());

  Serial.print(cBuf);
  ws_println(cBuf);
*/
}

///////////////////////////////////////////////////////
// draw clock
void draw_time(String h, String m, uint16_t color) {
    char cbuf[10];
    uint8_t hour, min;

    uint16_t x = TIME_START_X, y = TIME_START_Y;   // 시간 표시

    // hour display
    if(h.length()>3) return;

    strcpy(cbuf, h.c_str());
    hour = atoi(cbuf);

    // 10~23 까지 값이라면
    //if(hour/10){
        drawXbm565(x, y, 10, 16, num_10x16[hour/10], color);
        x += 10;
        drawXbm565(x, y, 10, 16, num_10x16[hour%10], color);
    //}
    //else{
        //x += 10;
        //drawXbm565(x, y, 10, 16, num_10x16[hour], color);
    //}

    x += 14;

    // minute display
    if(m.length()>3) return;

    strcpy(cbuf, m.c_str());
    min = atoi(cbuf);

    drawXbm565(x, y, 10, 16, num_10x16[min/10], color);
    x += 10;
    drawXbm565(x, y, 10, 16, num_10x16[min%10], color);

}

///////////////////////////////////////////////////////
// draw date
void draw_date(String t, uint16_t color) {
    char cbuf[10];
    int8_t lTemp;

    uint16_t x = DATE_X, y = DATE_Y;   // 날짜표시

    // date display
    if (t.length() > 3) return;

    strcpy(cbuf, t.c_str());
    lTemp = atoi(cbuf);

    // 0~99 까지 값 이라면
    if (lTemp / 10) {
        drawXbm565(x, y, 5, 6, num_5x6[lTemp / 10], color);
        x += 6;
        drawXbm565(x, y, 5, 6, num_5x6[lTemp % 10], color);
    } else {
        x += 6;
        drawXbm565(x, y, 5, 6, num_5x6[lTemp], color);
    }
}

///////////////////////////////////////////////////////
// clock Update
void clock_update() {

    ///////////////////////////////////////////////////////////////
    static unsigned long old_time;
    if ((GD.TickTime - old_time) >= 500)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////

    rtc_read();

    char cBuf[64];
    bool bDataSend = false;

    int RtcYear, RtcMonth, RtcDay, RtcWeek;
    int RtcHour, RtcMinute, RtcSeconds;

    int NtpYear, NtpMonth, NtpDay, NtpWeek;
    int NtpHour, NtpMinute, NtpSeconds;

    // 월,일,Week 변경 상태 확인용
    static uint8_t oldMonth, oldDay, oldWeek;
    String sMonth, sDay;

    // 시,분 변경 상태 확인용
    static uint8_t oldHour, oldMinute, oldSeconds;
    String sHour, sMin;//, sSec;

    uint8_t mx, dx;

    if(GD.rtc_connected){   //Use the rtc value if the system has rtc
        //GD.rtc_now.year(), GD.rtc_now.month(), GD.rtc_now.day(), GD.rtc_now.hour(), GD.rtc_now.minute(), GD.rtc_now.second()
        RtcYear    = GD.rtc_now.year();
        RtcMonth   = GD.rtc_now.month();
        RtcDay     = GD.rtc_now.day();
        RtcWeek    = GD.rtc_now.dayOfTheWeek();
        RtcHour    = GD.rtc_now.hour();
        RtcMinute  = GD.rtc_now.minute();
        RtcSeconds = GD.rtc_now.second();
    }

    // Use only when connected to Wi-Fi
    if(GD.wifi_connected){
        timeClient.update();

        //time_t epochTime = timeClient.getEpochTime();
        String formattedTime = timeClient.getFormattedTime();

        if(!GD.rtc_connected){
            RtcYear    = timeClient.getYear();
            RtcMonth   = timeClient.getMonth();
            RtcDay     = timeClient.getDate();

            RtcWeek = timeClient.getEpochTime() / 86400L % 7;

            RtcHour    = timeClient.getHours();
            RtcMinute  = timeClient.getMinutes();
            RtcSeconds = timeClient.getSeconds();
        }
        else{
            NtpYear    = timeClient.getYear();
            NtpMonth   = timeClient.getMonth();
            NtpDay     = timeClient.getDate();

            NtpWeek = timeClient.getEpochTime() / 86400L % 7;

            NtpHour    = timeClient.getHours();
            NtpMinute  = timeClient.getMinutes();
            NtpSeconds = timeClient.getSeconds();

            // Check every 10 minutes
            if((NtpMinute % 10)==0){

                // Update if the RTC value is different from the previous value
                if(RtcHour!=NtpHour || RtcMinute!=NtpMinute/* || RtcSeconds!=NtpSeconds*/){
                    RtcYear    = NtpYear;
                    RtcMonth   = NtpMonth;
                    RtcDay     = NtpDay;

                    RtcHour    = NtpHour;
                    RtcMinute  = NtpMinute;
                    RtcSeconds = NtpSeconds;

                    //GD.rtc.adjust(DateTime(GD.rtc_now.year(), GD.rtc_now.month(), GD.rtc_now.day(), RtcHour, RtcMinute, RtcSeconds));
                    GD.rtc.adjust(DateTime(RtcYear, RtcMonth, RtcDay, RtcHour, RtcMinute, RtcSeconds));
                    sprintf(cBuf, "rtc time sync  %04d-%02d-%-2d %d %02d:%02d:%02d\n", RtcYear, RtcMonth, RtcDay, RtcWeek, RtcHour, RtcMinute, RtcSeconds);
                    ws_print(cBuf);
                }
            }
        }
    }

    // Return if there is a problem with rtc and Wi-Fi
    if(!GD.rtc_connected && !GD.wifi_connected) return;

    // Set the font size and change it to auto-line
    virtualDisp->setTextSize(1);  // size 1 ~ 8 까지 설정
    virtualDisp->setTextWrap(false);

    // led date display
    // If the date or month has changed, update the LED so that it does not overlap
    if ((oldMonth != RtcMonth) || (oldDay != RtcDay) || (oldWeek != RtcWeek)) {
        // 같은 장소에 쓰지 않으면 Overap 됨 그래서 이전 데이타 한번 써줌 (지우는 역활)
        sprintf(cBuf, "%d\n", oldMonth);
        sMonth = cBuf;

        sprintf(cBuf,  "%d\n", oldDay);
        sDay = cBuf;

        // Month & Day Clear
        virtualDisp->setTextColor(virtualDisp->color444(0, 0, 0));  // black
#if(CFG_MONTH_DISPLAY_ON==1)
        mx = sMonth.length()==1 ? __MONTH_X_1 : __MONTH_X_2;
        virtualDisp->setCursor(mx, __MONTH_Y);
        virtualDisp->print(sMonth);

        dx = sDay.length()==1 ? __DAY_X_1: __DAY_X_2;
        virtualDisp->setCursor(dx, __DAY_Y);
        virtualDisp->print(sDay);
#else
        // day clear
        draw_date(sDay,0);

        // week clear
        drawXbm565(__WEEK_X, __WEEK_Y, 8, 8, week_days[7], 0);
#endif
    }

    // !FIX ME
    oldMonth = RtcMonth;
    oldDay   = RtcDay;
    oldWeek  = RtcWeek;

    //now date update
    sprintf(cBuf, "%d\n", RtcMonth);
    sMonth = cBuf;

    sprintf(cBuf,  "%d\n", RtcDay);
    sDay = cBuf;

    //sprintf(cBuf, "(%d) , (%d)\n", sMonth.length(), sDay.length());
    //WebSerial.print(cBuf);

    // Only hour and minute are displayed in the led matrix
    //virtualDisp->setTextColor(virtualDisp->color444(10, 0, 10));  // Pink
    virtualDisp->setTextColor(virtualDisp->color444(8, 8, 0));  // Orange
#if(CFG_MONTH_DISPLAY_ON==1)
    mx = sMonth.length()==2 ? __MONTH_X_1 : __MONTH_X_2;
    virtualDisp->setCursor(mx, __MONTH_Y);
    virtualDisp->print(sMonth);

    dx = sDay.length()==2 ? __DAY_X_1: __DAY_X_2;
    virtualDisp->setCursor(dx, __DAY_Y);
    virtualDisp->print(sDay);
#else
    // day display
    draw_date(sDay,virtualDisp->color444(8, 8, 0));

    //week display
    if(RtcWeek>7) RtcWeek = 7;

    // 토요일과 일요일은 빨간색으로 표시 되도록 함
    uint16_t  color = (RtcWeek==0 || RtcWeek==6) ? virtualDisp->color444(10,0, 0) : virtualDisp->color444(6, 8, 0);

    drawXbm565(__WEEK_X, __WEEK_Y, 8, 8, week_days[RtcWeek], color);

    //sprintf(cBuf, "(%d) Draw Week...\n", RtcWeek);
    //WebSerial.print(cBuf);
#endif

    // led time display
    // If the time or minute has changed, update the LED so that it does not overlap
    if ((oldHour != RtcHour) || (oldMinute != RtcMinute)) {

        // If dimming control is turned on
        if(GD.sys.bright_ctrl_on){
            // 하절기
            if((3<=RtcMonth)&&(RtcMonth<11)) {
                // Bright Control
                if (4 <= RtcHour && RtcHour < 5)  //
                    GD.Brightness = CFG_BRIGHT_LOW + GD.sys.bright_day_offset;
                else if (6 <= RtcHour && RtcHour < 7)  //
                    GD.Brightness = CFG_BRIGHT_MID + GD.sys.bright_day_offset;
                if (7 <= RtcHour && RtcHour < 10)  //
                    GD.Brightness = CFG_BRIGHT_HIGH + GD.sys.bright_day_offset;
                else if (10 <= RtcHour && RtcHour < 19)  //
                    GD.Brightness = CFG_BRIGHT_MID + GD.sys.bright_day_offset;
                else if (19 <= RtcHour && RtcHour < 20) {
                    if((CFG_BRIGHT_MID_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_MID_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_MID_LOW;
                }
                else if (20 <= RtcHour && RtcHour < 23) {
                    if((CFG_BRIGHT_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_LOW;
                }
                else {
                    if((CFG_BRIGHT_LOW_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_LOW_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_LOW_LOW;
                }
            }
            // 동절기
            else{
                // Bright Control
                if (5 <= RtcHour && RtcHour < 6)  //
                    GD.Brightness = CFG_BRIGHT_LOW + GD.sys.bright_day_offset;
                //else if (7 <= RtcHour && RtcHour < 8)  //
                //    GD.Brightness = CFG_BRIGHT_MID + GD.sys.bright_day_offset;
                else
                if (7 <= RtcHour && RtcHour < 11)  //
                    GD.Brightness = CFG_BRIGHT_HIGH + GD.sys.bright_day_offset;
                else if (11 <= RtcHour && RtcHour < 18)  //
                    GD.Brightness = CFG_BRIGHT_MID + GD.sys.bright_day_offset;
                else if (18 <= RtcHour && RtcHour < 19) {
                    if((CFG_BRIGHT_MID_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_MID_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_MID_LOW;
                }
                else if (19 <= RtcHour && RtcHour < 23) {
                    if((CFG_BRIGHT_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_LOW;
                }
                else {
                    if((CFG_BRIGHT_LOW_LOW - GD.sys.bright_night_offset)>1)
                        GD.Brightness = CFG_BRIGHT_LOW_LOW - GD.sys.bright_night_offset;
                    else
                        GD.Brightness = CFG_BRIGHT_LOW_LOW;
                }
            }
        }

        // 같은 장소에 쓰지 않으면 Overap 됨 그래서 이전 데이타 한번 써줌 (지우는 역활)
        sprintf(cBuf, "%02d\n", oldHour);
        sHour = cBuf;

        sprintf(cBuf,  "%02d\n", oldMinute);
        sMin = cBuf;

#if 0
        virtualDisp->setTextColor(virtualDisp->color444(0, 0, 0));  // black
        virtualDisp->setCursor(34, 3);
        virtualDisp->print(sHour);
        virtualDisp->setCursor(51, 3);
        virtualDisp->print(sMin);
#else
        draw_time(sHour, sMin, virtualDisp->color444(0, 0, 0)); // black
#endif
        // !FIX ME
        oldHour    = RtcHour;
        oldMinute  = RtcMinute;
        oldSeconds = RtcSeconds;

        bDataSend = true;

       // In the morning and evening, if it's the same weather, the icon needs to be changed...
        if (MORNING_DETECT_HOUR <= RtcHour && RtcHour < EVENING_DETECT_HOUR) {
            if(GD.night) {
                GD.night = false;

                sprintf(cBuf, "(%s) Day changed...\n", __FUNCTION__);
                ws_print(cBuf);
            }
        }
        else{
            if(!GD.night) {
                GD.night = true;
                sprintf(cBuf, "(%s) Night changed...\n", __FUNCTION__);
                ws_print(cBuf);
            }
        }
    }

    //now time update
    sprintf(cBuf, "%02d\n", RtcHour);
    sHour = cBuf;

    sprintf(cBuf,  "%02d\n", RtcMinute);
    sMin = cBuf;
/*
    sprintf(cBuf,  "%02d\n", RtcSeconds);
    sSec = cBuf;
*/

#if 0
    // Only hour and minute are displayed in the led matrix
    virtualDisp->setTextColor(virtualDisp->color444(12, 12, 12));  // White
    virtualDisp->setCursor(34, 3);
    virtualDisp->print(sHour);
    virtualDisp->setCursor(51, 3);
    virtualDisp->print(sMin);
#else
    draw_time(sHour, sMin, virtualDisp->color444(10, 10, 10));  // White
#endif

    //colon_update();  // 인터럽트에서 처리 하므로...여기서는 더이상 필요 없음

    // Send information to serial and web serial
    if (!bDataSend) return;

    sprintf(cBuf, "My IP : %s\n" ,WiFi.localIP().toString().c_str());
    ws_print(cBuf);

    sprintf(cBuf, "%04d-%02d-%02d %d %02d:%02d:%02d\n", RtcYear, RtcMonth, RtcDay, RtcWeek, RtcHour, RtcMinute, RtcSeconds);
    ws_print(cBuf);
}

///////////////////////////////////////////////////////
// Colon Update
typedef enum {
    COLON_ON,
    COLON_OFF,
} TypeDef_ColonState_t;

void colon_update() {
    static TypeDef_ColonState_t state;
    /*
    ///////////////////////////////////////////////////////////////
    static unsigned long old_time;
    if((GD.TickTime-old_time)>= 1000)
      old_time = GD.TickTime;
    else return;
      ///////////////////////////////////////////////////////////////
  */
#if 0
    virtualDisp->setTextWrap(false);
    virtualDisp->setTextSize(2);

    switch (state) {
        case COLON_OFF:
            virtualDisp->setTextColor(virtualDisp->color444(10, 10, 10));
            virtualDisp->setCursor(43, 0);
            virtualDisp->println(":");
            state = COLON_ON;
            break;
        default:
            // 같은 장소에 동일 데이타 써 주거나 네모를 써야 하는데 같은 데이타 써 주는게 나음
            virtualDisp->setTextColor(virtualDisp->color444(0, 0, 0));
            virtualDisp->setCursor(43, 0);
            virtualDisp->println(":");
            state = COLON_OFF;
    }
#else
    // 엉뚱한 곳에 쓰레기 값 디스플레이 되는 것 방지 하기 위해...
    if(GD.colon_lock || GD.logo_lock) return;

    switch (state) {
        case COLON_OFF:
            drawXbm565(COLON_START_X, COLON_START_Y, 2, 16, colon_2x16, virtualDisp->color444(10, 10, 10));
            state = COLON_ON;
            break;
        default:
            // 같은 장소에 동일 데이타 써 주거나 네모를 써야 하는데 같은 데이타 써 주는게 나음
            drawXbm565(COLON_START_X, COLON_START_Y, 2, 16, colon_2x16, virtualDisp->color444(0, 0, 0));
            state = COLON_OFF;
    }
#endif
}

///////////////////////////////////////////////////////
// CONNECT RTC
void connectRTC() {

    interrupt_init();

    if (!GD.rtc.begin()) {
        ws_println("Couldn't find RTC");
        return;
    }

    // rtc 가 접속 Flag set
    GD.rtc_connected = true;

    if (GD.rtc.lostPower()) {
        ws_println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        GD.rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    GD.rtc_now = GD.rtc.now();
}