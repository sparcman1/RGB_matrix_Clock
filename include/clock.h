//
// Created by sparcman on 2024-02-23.
//

#ifndef PROJECT_CLOCK_H
#define PROJECT_CLOCK_H

#include "DataTypes.h"
#include "led_matrix.h"

extern NTPClient timeClient;

extern void clock_update();
extern void colon_update();

extern void rtc_read();
extern void connectRTC();
#endif //PROJECT_CLOCK_H
