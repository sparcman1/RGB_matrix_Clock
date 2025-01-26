//
// Created by sparcman on 2024-02-23.
//

#include "shell_mini.h"


AsyncWebSocket ws("/ws");
AsyncWebServer server(80);  // 포트 8090으로 웹서버 생성
std::vector<String> messageLogs;
const int MAX_LOGS = 1000;  // 여기서 실제 값 정의

void(* resetFunc) () = nullptr;  // declare reset function at address 0



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mini Shell
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TypeDef_Shell_t sh;
///////////////////////////////////////////////////////
// SSID change Command
static
void do_ssid_change(){
    if(sh.argc<2) return;

    strcpy(GD.sys.wifi_ssid, sh.argv[1]);

    GD.sys.magic_code = CFG_MAGIC_UPDATE;
}

///////////////////////////////////////////////////////
// PASSWORD change Command
static
void do_pw_change(){
    if(sh.argc<2) return;

    strcpy(GD.sys.wifi_pass, sh.argv[1]);

    GD.sys.magic_code = CFG_MAGIC_UPDATE;
}

///////////////////////////////////////////////////////
// RSS change Command
static
void do_rss_change(){
    if(sh.argc<2) return;

    strcpy(GD.sys.rss_url, sh.argv[1]);

    GD.sys.magic_code = CFG_MAGIC_UPDATE;
}

///////////////////////////////////////////////////////
// Reset Command
static
void do_reset(){
    ws_println("Now system reset...");
    delay(5000);

    resetFunc(); //call reset
}

///////////////////////////////////////////////////////
// Info Command
static
void do_info(){
    char cBuf[512];
    String message;

    sprintf(cBuf,"ESP32 ChipID   : %06X\n", GD.chipID);
    message += cBuf;

    sprintf(cBuf,"WiFi SSID      : %s\n",GD.sys.wifi_ssid);
    message += cBuf;

    sprintf(cBuf,"RSS URL        : %s\n", GD.sys.rss_url);
    message += cBuf;

    sprintf(cBuf,"Bright ctrl    : %d\n", GD.sys.bright_ctrl_on);
    message += cBuf;

    sprintf(cBuf,"Default Bright : %d\n", GD.sys.bright);
    message += cBuf;

    sprintf(cBuf,"Bright D-Offset: %d\n", GD.sys.bright_day_offset);
    message += cBuf;

    sprintf(cBuf,"Bright N-Offset: %d\n", GD.sys.bright_night_offset);
    message += cBuf;

    sprintf(cBuf,"MY URL         : %s\n", GD.sys.dust_url);
    message += cBuf;

    sprintf(cBuf,"MY KEY         : %s\n", GD.sys.my_key);
    message += cBuf;

    sprintf(cBuf,"MY SIDO        : %s\n", GD.sys.my_sido);
    message += cBuf;

    sprintf(cBuf,"MY DONG        : %s\n", GD.sys.my_station);
    message += cBuf;

    sprintf(cBuf,"MY PAGE        : %d\n", GD.sys.my_page);
    message += cBuf;

    sprintf(cBuf,"Now Bright     : %d\n", GD.Brightness);
    message += cBuf;

    sprintf(cBuf,"Day & Night    : %s\n", GD.night?"Night":"Day");
    message += cBuf;

    sprintf(cBuf,"RTC Connect    : %d\n", GD.rtc_connected);
    message += cBuf;

    sprintf(cBuf,"WiFi Connect   : %d\n", GD.wifi_connected);
    message += cBuf;

    Serial.print(message);
    ws.textAll(message);
}

///////////////////////////////////////////////////////
// Bright adjust Command
static
void do_bright(){
    if(sh.argc<2) return;

    if(!strcmp( sh.argv[1], "on")) {
        GD.sys.bright_ctrl_on = true;
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "off")) {
        GD.sys.bright_ctrl_on = false;
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "set")) {
        if(sh.argc<3) return;

        uint8_t bright = strtoul(sh.argv[2], nullptr, 10);
        if((bright > 0) && (bright <=255)) {
            GD.sys.bright = bright;

            GD.sys.magic_code = CFG_MAGIC_UPDATE;
        }
    }
    else
    if(!strcmp( sh.argv[1], "d.off")) {
        if(sh.argc<3) return;

        uint8_t offset = strtoul(sh.argv[2], nullptr, 10);
        if((offset > 0) && (offset <=255)) {
            GD.sys.bright_day_offset = offset;

            GD.sys.magic_code = CFG_MAGIC_UPDATE;
        }
    }
    else
    if(!strcmp( sh.argv[1], "n.off")) {
        if(sh.argc<3) return;

        uint8_t offset = strtoul(sh.argv[2], nullptr, 10);
        if((offset >= 0) && (offset <=255)) {
            GD.sys.bright_night_offset = offset;

            GD.sys.magic_code = CFG_MAGIC_UPDATE;
        }
    }
    else{
        uint8_t bright = strtoul(sh.argv[1], nullptr, 10);

        if((bright > 0) && (bright <=255))
            GD.Brightness = bright;
    }
}

///////////////////////////////////////////////////////
// RTC adjust Command
static
void do_rtc(){
    // rtc 없으면 처리 안함
    if(!GD.rtc_connected) return;

    if(sh.argc<2) return;

    // 입력 값이 6보다 작다면 잘못 입력 한 값
    if(strlen(sh.argv[1])<6) return;

    int year, month, days, hour, minute, seconds;
    bool timeF = false;
    char cBuf[256];

    // year
    cBuf[0] = sh.argv[1][0];
    cBuf[1] = sh.argv[1][1];
    cBuf[2] = '\0';
    year = strtol(cBuf, nullptr, 10)+2000;
    // month
    cBuf[0] = sh.argv[1][2];
    cBuf[1] = sh.argv[1][3];
    cBuf[2] = '\0';
    month = strtol(cBuf, nullptr, 10);
    // days
    cBuf[0] = sh.argv[1][4];
    cBuf[1] = sh.argv[1][5];
    cBuf[2] = '\0';
    days = strtol(cBuf, nullptr, 10);

    if((sh.argc==3) && (strlen(sh.argv[2])==6)){
        // hour
        cBuf[0] = sh.argv[2][0];
        cBuf[1] = sh.argv[2][1];
        cBuf[2] = '\0';
        hour = strtol(cBuf, nullptr, 10);
        // minute
        cBuf[0] = sh.argv[2][2];
        cBuf[1] = sh.argv[2][3];
        cBuf[2] = '\0';
        minute = strtol(cBuf, nullptr, 10);
        // seconds
        cBuf[0] = sh.argv[2][4];
        cBuf[1] = sh.argv[2][5];
        cBuf[2] = '\0';
        seconds = strtol(cBuf, nullptr, 10);

        timeF = true;
    }

    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    if(!timeF){  // date 만 입력
        GD.rtc.adjust(DateTime(year, month, days, GD.rtc_now.hour(), GD.rtc_now.minute(), GD.rtc_now.second()));
        sprintf(cBuf, "rtc changed  %02d-%02d-%02d\n", year, month, days);
        ws_print(cBuf);
    }
    else{         // date and time 입력
        GD.rtc.adjust(DateTime(year, month, days, hour, minute, seconds));
        sprintf(cBuf, "rtc changed  %02d-%02d-%02d %02d:%02d:%02d\n", year, month, days, hour, minute, seconds);
        ws_print(cBuf);
    }
}

///////////////////////////////////////////////////////
// dust adjust Command
static
void do_dust(){
    if(sh.argc<3) return;

    if(!strcmp( sh.argv[1], "url")) {
        strcpy(GD.sys.dust_url, sh.argv[2]);
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "sido")) {
        strcpy(GD.sys.my_sido, sh.argv[2]);
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "dong")) {
        strcpy(GD.sys.my_station, sh.argv[2]);
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "page")) {
        GD.sys.my_page = strtoul(sh.argv[2], nullptr, 10);
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
    else
    if(!strcmp( sh.argv[1], "pm10")) {
        GD.udust.pm10Value = String(sh.argv[2]);
        GD.udust.manual = true;
    }
    else
    if(!strcmp( sh.argv[1], "pm25")) {
        GD.udust.pm25Value = String(sh.argv[2]);
        GD.udust.manual = true;
    }
    else
    if(!strcmp( sh.argv[1], "key")) {
        strcpy(GD.sys.my_key, sh.argv[2]);
        GD.sys.magic_code = CFG_MAGIC_UPDATE;
    }
}

///////////////////////////////////////////////////////
// HELP Command
extern CMD_LIST cmd_tbl[];

static
void do_help(){
    char cBuf[512];

    for( uint8_t i = 0; (*cmd_tbl [i].func)!=nullptr; i++ ) {
        if(cmd_tbl [i].help == nullptr) continue;
        sprintf(cBuf, "-%s\t: %s\n", cmd_tbl [i].cmd, cmd_tbl [i].help);
        ws_print(cBuf);
    }
}

///////////////////////////////////////////////////////
// Command Tables
//static
CMD_LIST    cmd_tbl[]={
        {(char*)"ssid", do_ssid_change, (char*)"ssid <your_wifi_ssid>    : ssid change"},
        {(char*)"pass", do_pw_change,   (char*)"pass <your_wifi_password>: password change"},
        {(char*)"rss",  do_rss_change,  (char*)"rss <weather_site_rss>   : rss url change"},
        {(char*)"rst",  do_reset,       (char*)"rst                      : system reset command"},
        {(char*)"info", do_info,        (char*)"info                     : system info view"},
        {(char*)"bright",do_bright,     (char*)"bright <on|off|set|1~255>: brightness adjust command\n"
                                               "\t    bright on|off      : bright auto control on or off\n"
                                               "\t    bright set 1~255   : default bright set\n"
                                               "\t    bright n.off|d.off 1~255   : day or night bright offset"
        },
        {(char*)"rtc",  do_rtc,         (char*)"rtc yymmdd [hhmmss]      : rtc adjust \n"
                                               "\t   rtc 240320 120000\n"
                                               "\t   rtc 240320"
        },
        {(char*)"dust",  do_dust,         (char*)"dust url|key|sido|page string : dust adjust \n"
                                               "\t   dust url /B552584/ArpltnInforInqireSvc/getCtprvnRltmMesureDnsty?servicekey=\n"
                                               "\t   dust key oRmlP17YwkFEyLOkR9Z8uBNQvTq1Sfafag%2BqmKfUYEPnzggbJNsmwuYZk0AAVWZA11kEUdzS5tsrCOWnEtFuJg%3D%3D\n"
                                               "\t   dust page 1~10\n"
                                               "\t   dust pm10|pm25 1~999\n"
                                               "\t   dust sido 경기|서울|인천\n"
                                               "\t   dust dong 영통동|천천동"
        },
        {(char*)"?"  ,  do_help,        (char*)"?                        : help message view"},
        nullptr,
};

///////////////////////////////////////////////////////
// Receive Command Execute
static
void con_cmd_execute()
{
    sh.event = false;     // 재진입 방지를 위해 플래그 지움
    if(sh.argc==0) return;    // 처리할 명령이 없다면 리턴

    for( uint8_t i = 0; (*cmd_tbl [i].func)!=nullptr; i++ ) {
        if(!strcmp( sh.argv[0], cmd_tbl [i].cmd)) {
            /*rtn =*/(*cmd_tbl [i].func)();      // call the function
        }
    }
}

///////////////////////////////////////////////////////
// received Data Parse
static
void con_cmd_parse(char* pCbuf)
{
    uint16_t in = strlen(pCbuf) + 1;
    uint16_t out = 0;
    char c_buf[CFG_ARG_LENGTH];
    uint16_t buf_cnt = 0;

    sh.argc = 0;

    while(in - out) {
        char buf = pCbuf[out++];

        switch(buf) {
            case ' ':
                if(!buf_cnt) break;
            if(buf_cnt<CFG_ARG_LENGTH) c_buf[buf_cnt++] = '\0';
            if(sh.argc<CFG_ARG_CNT) strcpy(sh.argv[sh.argc++], c_buf);
            buf_cnt = 0;
            break;
            case '\0':
                if(!buf_cnt && !sh.argc) return;
            if(buf_cnt<CFG_ARG_LENGTH) c_buf[buf_cnt++] = '\0';
            if(sh.argc<CFG_ARG_CNT) strcpy(sh.argv[sh.argc++], c_buf);
            con_cmd_execute();
            break;
            default:
                if(buf_cnt<CFG_ARG_LENGTH) c_buf[buf_cnt++] = buf;
            break;
        }
    }
}

///////////////////////////////////////////////////////
// Serial Event
void serialEvent(){

    char buf = (char)Serial.read();

    Serial.write(buf);  // echo

    switch(buf){
        case '\n' :
        case '\r' :
            if(GD.ser_in){
                GD.ser_buf[GD.ser_in++] = '\0';

                // 수신한 데이타 Parsing (space 별로 argument 분리)
                char c_buf[CFG_ARG_LENGTH];
                uint16_t buf_cnt = 0;

                sh.argc = 0;
                GD.ser_out = 0;

                while(GD.ser_in - GD.ser_out){
                    char c = GD.ser_buf[GD.ser_out++];

                    switch(c){
                        case ' ' :  // Argument 구분자
                            if(!buf_cnt) break; // 아무 것도 없이 space 만 있다면 다음을 읽어 들인다
                            if(buf_cnt<CFG_ARG_LENGTH)  c_buf[buf_cnt++] = '\0';   // 문자열 끝임을 알리기 위해
                            if(sh.argc<CFG_ARG_CNT)  strcpy(sh.argv[sh.argc++], c_buf);
                            buf_cnt = 0;
                            break;
                        case '\0':  // 명령 자체가 끝남
                            if(!buf_cnt && !sh.argc) break; // 알수 없는 입력은 무시
                            if(buf_cnt<CFG_ARG_LENGTH)  c_buf[buf_cnt++] = '\0';   // 문자열 끝임을 알리기 위해
                            if(sh.argc<CFG_ARG_CNT)  strcpy(sh.argv[sh.argc++], c_buf);

                            con_cmd_execute();
                            break;
                        default  :
                            if(buf_cnt<CFG_ARG_LENGTH) c_buf[buf_cnt++] = c;
                            break;
                    }
                }

                GD.ser_in = 0;
            }
            break;
        default :
            if(GD.ser_in<CFG_ARG_LENGTH-1){
                GD.ser_buf[GD.ser_in++] = buf;
            }
            break;
    }
}

// 웹소켓 시작 함수
void WebSocketStart() {
    // WiFi 연결 상태 확인
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi is not connected!");
        return;
    }

    // 서버 및 웹소켓 초기화
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    // 웹소켓 이벤트 핸들러 설정
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                 void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("Client #%u connected from IP: %s\n",
                    client->id(),
                    client->remoteIP().toString().c_str());
                ws.textAll("New client connected\n");
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("Client #%u disconnected\n", client->id());
                ws.textAll("Client disconnected\n");
                break;
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
                break;
            case WS_EVT_ERROR:
                Serial.println("WebSocket Error");
                break;
        }
    });

    server.addHandler(&ws);

    // 웹 페이지 라우트 설정
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
        Serial.println("Main page requested");
    });

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", log_html);
        Serial.println("Log page requested");
    });

    // 로그 데이터 요청 처리
    server.on("/getlogs", HTTP_GET, [](AsyncWebServerRequest *request){
        String logs;
        for(const String& log : messageLogs) {
            logs += log + "\n";
        }
        request->send(200, "text/plain", logs);
    });

    // 로그 클리어 요청 처리
    server.on("/clearlogs", HTTP_GET, [](AsyncWebServerRequest *request){
        messageLogs.clear();
        request->send(200);
    });

    // 로그 다운로드 처리
    server.on("/downloadlogs", HTTP_GET, [](AsyncWebServerRequest *request){
        String logs;
        for(const String& log : messageLogs) {
            // HTML 태그 제거하고 순수 텍스트만 추출
            String plainText = log;

            // timestamp 추출 [HH:MM:SS]
            int startPos = plainText.indexOf("[");
            int endPos = plainText.indexOf("]") + 1;
            String timestamp = plainText.substring(startPos, endPos);

            // 명령어/응답 텍스트 추출
            String content = plainText;
            content.replace("<div class='log-entry'>", "");
            content.replace("<span class='timestamp'>", "");
            content.replace("</span>", "");
            content.replace("<span class='command'>>", "");
            content.replace("<span class='response'>", "");
            content.replace("</div>", "");
            content.replace("&lt;", "<");
            content.replace("&gt;", ">");

            // 앞뒤 공백 제거
            while(content.startsWith(" ")) content.remove(0, 1);
            while(content.endsWith(" ")) content.remove(content.length()-1);

            // 정리된 형식으로 로그 추가
            logs += timestamp + " " + content + "\n";
        }

        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", logs);
        response->addHeader("Content-Disposition", "attachment; filename=terminal_logs.txt");
        request->send(response);
    });

    // 404 에러 처리
    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not found");
    });

    // 서버 시작
    server.begin();
    Serial.println("WebSocket Server Started");
    Serial.print("Web server IP address: ");
    Serial.println(WiFi.localIP());
}

void addToLog(const String& message, bool isCommand) {
    // 현재 시간 가져오기
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // 시간 형식 포맷팅 [HH:MM:SS]
    char timestamp[10];
    snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d",
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);

    String logEntry = "<div class='log-entry'><span class='timestamp'>[" + String(timestamp) + "]</span>";

    // HTML 특수문자 이스케이프 처리
    String escapedMessage = message;
    escapedMessage.replace("<", "&lt;");
    escapedMessage.replace(">", "&gt;");

    if(isCommand) {
        logEntry += "<span class='command'>> " + escapedMessage + "</span>";
    } else {
        logEntry += "<span class='response'>" + escapedMessage + "</span>";
    }
    logEntry += "</div>";

    messageLogs.push_back(logEntry);
    if(messageLogs.size() > MAX_LOGS) {
        messageLogs.erase(messageLogs.begin());
    }
}

// 웹소켓 메시지 처리
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        String d = String((char*)data);

        // 명령어를 로그에 저장
        addToLog(d, true);  // true는 이것이 명령어임을 표시

        char *pCbuf = (char*)d.c_str();
        con_cmd_parse(pCbuf);
    }
}

void ws_printf(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    String message = String(buffer);
    ws.textAll(message);
    Serial.print(message);

    // 응답을 로그에 저장
    addToLog(message, false);  // false는 이것이 응답임을 표시
}

void ws_println(const char* message) {
    String msg = String(message) + "\n";
    ws.textAll(msg);
    Serial.println(message);
    addToLog(msg, false);
}

void ws_print(const char* message) {
    String msg = String(message);
    ws.textAll(msg);
    Serial.print(message);
    addToLog(msg, false);
}