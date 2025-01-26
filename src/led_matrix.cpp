//
// Created by sparcman on 2024-02-23.
//

#include "led_matrix.h"
#include "shell_mini.h"

//////////////////////////////////////////////////////
// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

class OneEighthMatrixPanel;

// placeholder for the virtual display object
OneEighthMatrixPanel *virtualDisp = nullptr;

///////////////////////////////////////////////////////
// Bright Smooth Change Control
typedef enum {
    CHANGE_EVENT_WAIT,
    CHANGE_EXEC_UP,
    CHANGE_EXEC_DOWN,
}Typedef_BrightChg_t;

void bright_control() {
    ///////////////////////////////////////////////////////////////
    static uint64_t old_time;
    if ((GD.TickTime - old_time) >= 20)
        old_time = GD.TickTime;
    else return;
    ///////////////////////////////////////////////////////////////
    static Typedef_BrightChg_t state;
    static uint8_t oldBright, diff;
    static bool up, down;

    switch(state){
        case CHANGE_EXEC_DOWN :
            if(diff--){
                oldBright--;
                dma_display->setBrightness8(oldBright);
            }
            else{
                ws_printf("oldBright = %d, DOWN Complete\n", oldBright);
                state = CHANGE_EVENT_WAIT;
            }
            break;

        case CHANGE_EXEC_UP :
            if(diff--){
                oldBright++;
                dma_display->setBrightness8(oldBright);
            }
            else{
                ws_printf("oldBright = %d, UP Complete\n", oldBright);
                state = CHANGE_EVENT_WAIT;
            }
            break;

        default :
            // Current and Previous Values Comparison
            if(oldBright != GD.Brightness){
                if(oldBright < GD.Brightness){
                    diff = GD.Brightness - oldBright;
                    state = CHANGE_EXEC_UP;
                }
                else{
                    diff =  oldBright - GD.Brightness;
                    state = CHANGE_EXEC_DOWN;
                }

                ws_printf("diff : %d, %s\n", diff, state==CHANGE_EXEC_UP ? "UP" :"DOWN");
            }
            break;
    }
}

///////////////////////////////////////////////////////
// LED Dot Matrix Dot Draw
//void drawXbm565(int x, int y, int width, int height, const char *xbm, uint16_t color = 0x94ac) {
void drawXbm565(int x, int y, int width, int height, const char *xbm, uint16_t color) {
    GD.colon_lock = true;

    if (width % 8 != 0) {
        width = ((width / 8) + 1) * 8;
    }
    for (int i = 0; i < width * height / 8; i++) {
        unsigned char charColumn = pgm_read_byte(xbm + i);
        for (int j = 0; j < 8; j++) {
            int targetX = (i * 8 + j) % width + x;
            int targetY = (8 * i / (width)) + y;
            if (bitRead(charColumn, j)) {
                virtualDisp->drawPixel((int16_t)targetX, (int16_t)targetY, color);
            }
        }
    }

    GD.colon_lock = false;
}

///////////////////////////////////////////////////////
//
void drawFwVer() {
    virtualDisp->clearScreen();

    // version info draw
    virtualDisp->setTextSize(2);
    virtualDisp->setCursor(1, 10);
    virtualDisp->setTextColor(virtualDisp->color444(0, 15, 0));  // Green
    virtualDisp->println(/*"V" + */ String(FIRMWARE_VERSION));

    delay(2000);

    virtualDisp->clearScreen();
}

///////////////////////////////////////////////////////
//
void drawMyIp() {
    GD.colon_lock = true;

    virtualDisp->clearScreen();

    // IP 표시
    virtualDisp->setTextSize(1);
    virtualDisp->setTextWrap(false);  // automatic line modification

    //virtualDisp->setTextColor(virtualDisp->color444(0,15,0)); // Green
    virtualDisp->setTextColor(virtualDisp->color444(15, 15, 0));  // Yelow

    String s = WiFi.localIP().toString();

    //virtualDisp->println(WiFi.localIP());
    char ip[4][4];
    char *sP = (char*)s.c_str();

    uint32_t i,j,k;

    for(i=0, j=0, k=0; i<s.length(); i++){
        if(*(sP+i)=='.')
            ip[j++][k] = '\0' , k=0;
        else
            ip[j][k++] = *(sP+i);
    }

    ip[j][k] = '\0';

    //Serial.printf("%s.%s.%s.%s\n", ip[0], ip[1],ip[2], ip[3]);

    String sIp;
    sIp = String(ip[0]) + ".";
    virtualDisp->setCursor(20, 0);
    virtualDisp->println(sIp);

    virtualDisp->setCursor(20, 8);
    sIp = String(ip[1]) + ".";
    virtualDisp->println(sIp);

    virtualDisp->setCursor(20, 16);
    sIp = String(ip[2]) + ".";
    virtualDisp->println(sIp);

    virtualDisp->setCursor(20, 24);
    sIp = String(ip[3]);
    virtualDisp->println(sIp);

    delay(2000);

    virtualDisp->clearScreen();

    GD.colon_lock = false;
}

///////////////////////////////////////////////////////
// CONNECT DISPLAY
void connectDisplay(){

    HUB75_I2S_CFG::i2s_pins _pins = {
            R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
            A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
            LAT_PIN, OE_PIN, CLK_PIN
    };

#if 0
    // 패널 설정 값
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );
#else
    HUB75_I2S_CFG mxconfig(
            PANEL_RES_X * 2,  // DO NOT CHANGE THIS
            PANEL_RES_Y / 2,  // DO NOT CHANGE THIS
            PANEL_CHAIN       // DO NOT CHANGE THIS
            ,
            _pins  // Uncomment to enable custom pins
    );
#endif

    // default brightness set
    GD.Brightness = GD.sys.bright + GD.sys.bright_day_offset;

    // led matrix driver set
    mxconfig.clkphase = false;
    //mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;
    mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
    //mxconfig.driver = HUB75_I2S_CFG::FM6124;
    mxconfig.driver = HUB75_I2S_CFG::ICN2038S;

    // Display Setup
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);

    dma_display->setBrightness8(GD.Brightness);  //0-255

    // Allocate memory and start DMA display
    if (not dma_display->begin())
        ws_println("****** !SPARCMAN! I2S memory allocation failed ***********");

    // create VirtualDisplay object based on our newly created dma_display object
    virtualDisp = new OneEighthMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, SERPENT, TOPDOWN);

    virtualDisp->clearScreen();

#if (DISPLAY_TEST == 1)
    TestPattern0(virtualDisp);
    TestPattern1(virtualDisp);
#endif

    drawFwVer();
}
