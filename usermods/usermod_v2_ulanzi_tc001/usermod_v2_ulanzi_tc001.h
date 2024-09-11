#pragma once

#include "wled.h"

class UlanziTc001Usermod : public Usermod
{
private:
  // Private class members. You can declare variables and functions only accessible to your usermod here
  unsigned long lastTime = 0;

  // ---- Variables modified by settings below -----
  // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
  bool clockEnabled = true;
  int colorR = 0xFF;
  int colorG = 0xFF;
  int colorB = 0xFF;

  // ---- Variables for correct LED numbering below, edit only if your clock is built different ----
  const int cols = 32;
  const int rows = 8;
  int baseHH[2] = {  2, 0 };   // Top Left for the tens place of the hours
  int baseH[2]  = {  6, 0 };   // Top Left for the one place of the hours
  int baseMM[2] = { 12, 0 };   // Top Left for the tens place of the minutes
  int baseM[2]  = { 15, 0 };   // Top Left for the one place of the minutes
  int baseSS[2] = { 22, 0 };   // Top Left for the tens place of the seconds
  int baseS[2]  = { 26, 0 };   // Top Left for the one place of the seconds
  int days[7][2] = 
    {
      {227, 228, 229},
      {231, 232, 233},
      {235, 236, 237},
      {239, 240, 241},
      {243, 244, 245},
      {247, 248, 249},
      {251, 252, 253}
    };

  // Matrix for the illumination of the numbers
  const bool numbers[10][15] = 
    {
      { 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 }, // 0: null
      { 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1 }, // 1: eins
      { 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1 }, // 2: zwei
      { 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1 }, // 3: drei
      { 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1 }, // 4: vier
      { 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1 }, // 5: fünf
      { 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 }, // 6: sechs
      { 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 }, // 7: sieben
      { 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 }, // 8: acht
      { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 }  // 9: neun
    };

  // Pixels consist of indices 0 to 14. To calculate offset add column offset and row offset. column offset can be specified also row offset. But additionally base row offset must be applied.
  // 1 / 3 = 0 => base row offset 0; 1 % 3 = 1 base column offset
  // 4 / 3 = 1 => base row offset 1; 4 % 3 = 1 base column offset
  // 8 / 3 = 2 => base row offset 2; 8 % 3 = 2 base column offset

public:
  void setup()
  { }

  void loop()
  {
    if (millis() - lastTime > 1000)
    {
      lastTime = millis();
    }
  }

  void addToJsonInfo(JsonObject& root)
  {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonArray lightArr = user.createNestedArray("Uhrzeit-Anzeige"); //name
    lightArr.add(clockEnabled ? "aktiv" : "inaktiv"); //value
    lightArr.add(""); //unit
  }

  void addToConfig(JsonObject &root)
  {
    JsonObject top = root.createNestedObject("Ulanzi TC001");
    top["enabled"] = clockEnabled;
    top["colorR"]   = colorR;
    top["colorG"]   = colorG;
    top["colorB"]   = colorB;
  }

  bool readFromConfig(JsonObject &root)
  {
    JsonObject top = root["Ulanzi TC001"];

      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top["enabled"], clockEnabled);
      configComplete &= getJsonValue(top["colorR"], colorR);
      configComplete &= getJsonValue(top["colorG"], colorG);
      configComplete &= getJsonValue(top["colorB"], colorB);

      return configComplete;
  }

  void drawNumber(int base, int number)
  {
    for(int i = 0; i < 10; i++)
    {
      if(numbers[number][i] > -1)
        strip.setPixelColor(numbers[number][i] + base, RGBW32(colorR, colorG, colorB, 0));
    }
  }

  void handleOverlayDraw()
  {
    if(clockEnabled){
      drawNumber(baseHH, (hour(localTime) / 10) % 10);
      drawNumber(baseH, hour(localTime) % 10); 
      drawNumber(baseM, (minute(localTime) / 10) % 10);
      drawNumber(baseMM, minute(localTime) % 10);
      drawNumber(baseS, (second(localTime) / 10) % 10);
      drawNumber(baseSS, second(localTime) % 10);
    }
  }

  uint16_t getId()
  {
    return USERMOD_ID_ULANZI_TC001;
  }

};
