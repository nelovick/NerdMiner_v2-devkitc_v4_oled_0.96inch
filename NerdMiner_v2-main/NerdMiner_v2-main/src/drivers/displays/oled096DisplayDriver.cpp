
#include "displayDriver.h"

#ifdef OLED_096_DISPLAY

#include <U8g2lib.h>
#include "monitor.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

static uint8_t btc_icon[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xF0, 0x03, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0xFF, 0x3F, 0x00, 
  0x80, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xE0, 0x3F, 0xFF, 0x01, 
  0xF0, 0x3F, 0xFF, 0x03, 0xF0, 0x0F, 0xFC, 0x03, 0xF0, 0x0F, 0xF8, 0x03, 
  0xF8, 0xCF, 0xF9, 0x07, 0xF8, 0xCF, 0xF9, 0x07, 0xF8, 0x0F, 0xFC, 0x07, 
  0xF8, 0x0F, 0xF8, 0x07, 0xF8, 0xCF, 0xF9, 0x07, 0xF8, 0xCF, 0xF9, 0x07, 
  0xF0, 0x0F, 0xF8, 0x03, 0xF0, 0x0F, 0xFC, 0x03, 0xF0, 0x3F, 0xFF, 0x03, 
  0xE0, 0x3F, 0xFF, 0x01, 0xC0, 0xFF, 0xFF, 0x00, 0x80, 0xFF, 0x7F, 0x00, 
  0x00, 0xFF, 0x3F, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0xF0, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static uint8_t setup_icon[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0xE0, 0x01, 0x00, 
  0x00, 0xF0, 0x03, 0x00, 0xC0, 0xF0, 0xC3, 0x00, 0xE0, 0xF9, 0xE3, 0x01, 
  0xF0, 0xFF, 0xFF, 0x03, 0xF0, 0xFF, 0xFF, 0x03, 0xE0, 0xFF, 0xFF, 0x01, 
  0xC0, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xE0, 0x1F, 0xFE, 0x00, 
  0xF8, 0x0F, 0xFC, 0x07, 0xFE, 0x07, 0xF8, 0x1F, 0xFE, 0x07, 0xF8, 0x1F, 
  0xFE, 0x07, 0xF8, 0x1F, 0xFE, 0x07, 0xF8, 0x1F, 0xF8, 0x0F, 0xFC, 0x07, 
  0xE0, 0x1F, 0xFE, 0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0xFF, 0x00, 
  0xE0, 0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0x03, 0xF0, 0xFF, 0xFF, 0x03, 
  0xE0, 0xF9, 0xE3, 0x01, 0xC0, 0xF0, 0xC3, 0x00, 0x00, 0xF0, 0x03, 0x00, 
  0x00, 0xE0, 0x01, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// U8G2 constructor para SSD1306 128x64 OLED display con I2C (dirección 0x3c)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL_PIN, /* data=*/ SDA_PIN);

void clearScreen(void) {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void serialPrint(unsigned long mElapsed) {
  mining_data data = getMiningData(mElapsed);

  // Print hashrate to serial
  Serial.printf(">>> Completed %s share(s), %s Khashes, avg. hashrate %s KH/s\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str());

  // Print extended data to serial for no display devices
  Serial.printf(">>> Valid blocks: %s\n", data.valids.c_str());
  Serial.printf(">>> Block templates: %s\n", data.templates.c_str());
  Serial.printf(">>> Best difficulty: %s\n", data.bestDiff.c_str());
  Serial.printf(">>> 32Bit shares: %s\n", data.completedShares.c_str());
  Serial.printf(">>> Temperature: %s\n", data.temp.c_str());
  Serial.printf(">>> Total MHashes: %s\n", data.totalMHashes.c_str());
  Serial.printf(">>> Time mining: %s\n", data.timeMining.c_str());
}

void oledDisplay_Init(void)
{
  Serial.println("OLED 0.96 display driver initialized");
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.clear();
  u8g2.setFlipMode(0);
  clearScreen();
}

void oledDisplay_AlternateScreenState(void)
{
  Serial.println("Switching display state");
}

void oledDisplay_AlternateRotation(void)
{
}

void oledDisplay_Screen1(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB24_tf);
  u8g2.drawStr(0, 32, data.currentHashRate.c_str());
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr(80, 32, "KH/s");
  
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr(0, 50, "Shares: ");
  u8g2.drawStr(60, 50, data.completedShares.c_str());
  
  u8g2.sendBuffer();

  serialPrint(mElapsed);
}

void oledDisplay_Screen2(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  char temp[16];
  sprintf(temp, "Temp: %s°c", data.temp.c_str());

  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_helvB18_tf);
  u8g2.drawStr(0, 32, temp);
  
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr(0, 50, "Diff: ");
  u8g2.drawStr(45, 50, data.bestDiff.c_str());
  
  u8g2.sendBuffer();

  serialPrint(mElapsed);
}

void oledDisplay_Screen3(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB12_tf);
  u8g2.drawStr(0, 16, "Mining Stats");
  
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr(0, 32, "Hashes: ");
  u8g2.drawStr(60, 32, data.totalMHashes.c_str());
  
  u8g2.drawStr(0, 48, "Time: ");
  u8g2.drawStr(40, 48, data.timeMining.c_str());
  
  u8g2.drawStr(0, 64, "Valid: ");
  u8g2.drawStr(50, 64, data.valids.c_str());
  
  u8g2.sendBuffer();

  serialPrint(mElapsed);
}

void oledDisplay_LoadingScreen(void)
{
  Serial.println("Initializing...");
  u8g2.clearBuffer();
  u8g2.drawXBMP(40, 15, 30, 30, btc_icon);
  u8g2.setFont(u8g2_font_helvB12_tf);
  u8g2.drawStr(35, 58, "Loading");
  u8g2.sendBuffer();
}

void oledDisplay_SetupScreen(void)
{
  Serial.println("Setup...");
  u8g2.clearBuffer();
  u8g2.drawXBMP(40, 10, 30, 30, setup_icon);
  u8g2.setFont(u8g2_font_helvB12_tf);
  u8g2.drawStr(40, 58, "Setup");
  u8g2.sendBuffer();
}

void oledDisplay_DoLedStuff(unsigned long frame)
{
}

void oledDisplay_AnimateCurrentScreen(unsigned long frame)
{
}

CyclicScreenFunction oledDisplayCyclicScreens[] = {oledDisplay_Screen1, oledDisplay_Screen2, oledDisplay_Screen3};

DisplayDriver oled096DisplayDriver = {
    oledDisplay_Init,
    oledDisplay_AlternateScreenState,
    oledDisplay_AlternateRotation,
    oledDisplay_LoadingScreen,
    oledDisplay_SetupScreen,
    oledDisplayCyclicScreens,
    oledDisplay_AnimateCurrentScreen,
    oledDisplay_DoLedStuff,
    SCREENS_ARRAY_SIZE(oledDisplayCyclicScreens),
    0,
    0,
    0,
};

#endif
