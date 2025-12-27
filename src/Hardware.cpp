#include "Hardware.h"



void screenInit(){
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);

    ESP.getFreeHeap();
    img.setColorDepth(16);
    img.setSwapBytes(true);
    img.createSprite(240, 240);
    img.fillSprite(TFT_BLACK);
    img.pushSprite(0,0);
}

State state = {
    0, 
    -1, -1, -1, 
    -1, -1, 
    0, 
    0.0f, 
    {0},
    true, 
    true, 
    false, false, false, 
    false
};
