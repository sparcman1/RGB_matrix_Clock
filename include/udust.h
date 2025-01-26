//
// Created by sparcman on 2024-02-24.
//

#ifndef PROJECT_UDUST_H
#define PROJECT_UDUST_H

#include "DataTypes.h"
#include "led_matrix.h"
// for json
#include "ArduinoJson.h"

extern WiFiClient DustClient;

extern void udust_update();
#endif //PROJECT_UDUST_H
