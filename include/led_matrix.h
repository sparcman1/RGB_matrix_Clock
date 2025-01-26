//
// Created by sparcman on 2024-02-23.
//

#ifndef PROJECT_LED_MATRIX_H
#define PROJECT_LED_MATRIX_H

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "OneEighthScanMatrixPanel.h"
#include "test_pattern.h"

#include "DataTypes.h"

extern OneEighthMatrixPanel *virtualDisp;
extern MatrixPanel_I2S_DMA *dma_display;

extern void bright_control();

extern void drawXbm565(int x, int y, int width, int height, const char *xbm, uint16_t color = 0x94ac);
extern void drawFwVer();
extern void drawMyIp();

extern void connectDisplay();

#endif //PROJECT_LED_MATRIX_H
