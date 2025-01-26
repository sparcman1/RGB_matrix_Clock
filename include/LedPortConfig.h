#ifndef _LED_PORT_CONFIG_H
#define _LED_PORT_CONFIG_H

//////////////////////////////////////////////////////
// Hardware Port define
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13

#define A_PIN 23
#define B_PIN 19  // 원본은 22이나 DS3321 사용을 위해 19번으로 변경
#define C_PIN 5
#define D_PIN -1  // Connected to GND on panel (21 if exist)
#define E_PIN -1  // Connected to GND on panel

#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16

// 64 X 32 패널 , 1채널 기준
#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32  // Number of pixels tall of each INDIVIDUAL panel module.

#define NUM_ROWS 1                      // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1                      // Number of INDIVIDUAL PANELS per ROW
#define PANEL_CHAIN NUM_ROWS *NUM_COLS  // total number of panels chained one to another

// Change this to your needs, for details on VirtualPanel pls read the PDF!
#define SERPENT true
#define TOPDOWN false

#define DISPLAY_TEST 1  // Show test pattern at startup

#endif