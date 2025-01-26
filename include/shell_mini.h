//
// Created by sparcman on 2024-02-23.
//

#ifndef PROJECT_SHELL_MINI_H
#define PROJECT_SHELL_MINI_H

#include "DataTypes.h"
#include <Arduino.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
extern AsyncWebSocket ws;
extern AsyncWebServer server;
// 로그 관련 선언
extern std::vector<String> messageLogs;
extern const int MAX_LOGS;  // 상수도 extern으로 선언

void WebSocketStart();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

void WebSerialRecvMsg(uint8_t *data, size_t len);

void ws_printf(const char* format, ...);
void ws_println(const char* message);
void ws_print(const char* message);

#endif //PROJECT_SHELL_MINI_H
