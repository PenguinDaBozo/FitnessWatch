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
    0,
    true, 
    true, 
    false, false, false, 
    false,
    false
};  

const lgfx::U8g2font baloo85(u8g2_font_baloo85_num);
const lgfx::U8g2font roboto20(u8g2_font_roboto20_num);
const lgfx::U8g2font baloo46(u8g2_font_baloo46_num);
const lgfx::U8g2font baloo72(u8g2_font_baloo72_num);
const lgfx::U8g2font helvR24(u8g2_font_helvR24_tr);
