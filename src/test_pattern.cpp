//
// Created by sparcman on 2024-02-23.
//

#include "test_pattern.h"

void TestPattern0(OneEighthMatrixPanel  *virtualDisp) {
    //Serial.println(""); Serial.println(""); Serial.println("");
    //Serial.println("*****************************************************");
    //Serial.println("*         1/8 Scan Panel Demonstration              *");
    //Serial.println("*****************************************************");

    virtualDisp->clearScreen();

    virtualDisp->drawPixel(0, 0, myRED);
    virtualDisp->drawPixel(8, 0, myGREEN);
    virtualDisp->drawPixel(16, 0, myBLUE);
    virtualDisp->drawPixel(24, 0, myWHITE);
    virtualDisp->drawPixel(32, 0, myRED);
    virtualDisp->drawPixel(40, 0, myGREEN);
    virtualDisp->drawPixel(48, 0, myBLUE);
    virtualDisp->drawPixel(56, 0, myWHITE);
    virtualDisp->drawPixel(63, 0, myRED);

    virtualDisp->drawPixel(0,  8, myRED);
    virtualDisp->drawPixel(8,  8, myGREEN);
    virtualDisp->drawPixel(16, 8, myBLUE);
    virtualDisp->drawPixel(24, 8, myWHITE);
    virtualDisp->drawPixel(32, 8, myRED);
    virtualDisp->drawPixel(40, 8, myGREEN);
    virtualDisp->drawPixel(48, 8, myBLUE);
    virtualDisp->drawPixel(56, 8, myWHITE);
    virtualDisp->drawPixel(63, 8, myRED);

    virtualDisp->drawPixel(0,  16, myRED);
    virtualDisp->drawPixel(8,  16, myGREEN);
    virtualDisp->drawPixel(16, 16, myBLUE);
    virtualDisp->drawPixel(24, 16, myWHITE);
    virtualDisp->drawPixel(32, 16, myRED);
    virtualDisp->drawPixel(40, 16, myGREEN);
    virtualDisp->drawPixel(48, 16, myBLUE);
    virtualDisp->drawPixel(56, 16, myWHITE);
    virtualDisp->drawPixel(63, 16, myRED);

    virtualDisp->drawPixel(0,  24, myRED);
    virtualDisp->drawPixel(8,  24, myGREEN);
    virtualDisp->drawPixel(16, 24, myBLUE);
    virtualDisp->drawPixel(24, 24, myWHITE);
    virtualDisp->drawPixel(32, 24, myRED);
    virtualDisp->drawPixel(40, 24, myGREEN);
    virtualDisp->drawPixel(48, 24, myBLUE);
    virtualDisp->drawPixel(56, 24, myWHITE);
    virtualDisp->drawPixel(63, 24, myRED);

    delay(2000);
}

void TestPattern1(OneEighthMatrixPanel  *virtualDisp) {

    for (int x=0; x <= 63; x++) {
        // Red 8 Rows
        virtualDisp->drawPixel(x,0,virtualDisp->color565(255, 0, 0)); // ROW 1
        virtualDisp->drawPixel(x,1,virtualDisp->color565(210, 0, 0)); // ROW 2
        virtualDisp->drawPixel(x,2,virtualDisp->color565(180, 0, 0)); // ROW 3
        virtualDisp->drawPixel(x,3,virtualDisp->color565(150, 0, 0)); // ROW 4
        virtualDisp->drawPixel(x,4,virtualDisp->color565(125, 0, 0)); // ROW 5
        virtualDisp->drawPixel(x,5,virtualDisp->color565(90, 0, 0)); // ROW 6
        virtualDisp->drawPixel(x,6,virtualDisp->color565(60, 0, 0)); // ROW 7
        virtualDisp->drawPixel(x,7,virtualDisp->color565(40, 0, 0)); // ROW 8

        // Blue 8 Rows
        virtualDisp->drawPixel(x,8,virtualDisp->color565(0, 0, 255)); // ROW 9
        virtualDisp->drawPixel(x,9,virtualDisp->color565(0, 0, 210)); // ROW 10
        virtualDisp->drawPixel(x,10,virtualDisp->color565(0, 0, 180)); // ROW 11
        virtualDisp->drawPixel(x,11,virtualDisp->color565(0, 0, 150)); // ROW 12
        virtualDisp->drawPixel(x,12,virtualDisp->color565(0, 0, 125)); // ROW 13
        virtualDisp->drawPixel(x,13,virtualDisp->color565(0, 0, 90)); // ROW 14
        virtualDisp->drawPixel(x,14,virtualDisp->color565(0, 0, 60)); // ROW 15
        virtualDisp->drawPixel(x,15,virtualDisp->color565(0, 0, 40)); // ROW 16

        // White 8 Rows
        virtualDisp->drawPixel(x,16,virtualDisp->color565(255, 255, 255)); // ROW 17
        virtualDisp->drawPixel(x,17,virtualDisp->color565(210, 210, 210)); // ROW 18
        virtualDisp->drawPixel(x,18,virtualDisp->color565(180, 180, 180)); // ROW 19
        virtualDisp->drawPixel(x,19,virtualDisp->color565(150, 150, 150)); // ROW 20
        virtualDisp->drawPixel(x,20,virtualDisp->color565(125, 125, 125)); // ROW 21
        virtualDisp->drawPixel(x,21,virtualDisp->color565(90, 90, 90)); // ROW 22
        virtualDisp->drawPixel(x,22,virtualDisp->color565(60, 60, 60)); // ROW 23
        virtualDisp->drawPixel(x,23,virtualDisp->color565(40, 40, 40)); // ROW 24

        // Green 8 Rows
        virtualDisp->drawPixel(x,24,virtualDisp->color565(0, 255, 0)); // ROW 25
        virtualDisp->drawPixel(x,25,virtualDisp->color565(0, 210, 0)); // ROW 26
        virtualDisp->drawPixel(x,26,virtualDisp->color565(0, 180, 0)); // ROW 27
        virtualDisp->drawPixel(x,27,virtualDisp->color565(0, 150, 0)); // ROW 28
        virtualDisp->drawPixel(x,28,virtualDisp->color565(0, 125, 0)); // ROW 29
        virtualDisp->drawPixel(x,29,virtualDisp->color565(0, 90, 0)); // ROW 30
        virtualDisp->drawPixel(x,30,virtualDisp->color565(0, 60, 0)); // ROW 31
        virtualDisp->drawPixel(x,31,virtualDisp->color565(0, 40, 0)); // ROW 32
    }

    delay(2000);
}
