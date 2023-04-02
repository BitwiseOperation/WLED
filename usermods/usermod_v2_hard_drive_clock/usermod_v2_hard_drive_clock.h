#pragma once

#include "wled.h"

#define MINUTE_LED_COUNT 24
#define MINUTE_DIVIDER 150 //150 results in having 24 Leds to display 60 Minutes times 60 seconds. 60/24*60 = 150
#define HOUR_LED_COUNT 12

class HardDriveClockUsermod : public Usermod
{
private:
  // Private class members. You can declare variables and functions only accessible to your usermod here
  unsigned long lastTime = 0;

  int hourStartLed = 24;
  int hourStartOffset = 5;
  
  int minuteStartLed = 0;
  int minuteStartOffset = 11;

  // ---- Variables modified by settings below -----
  // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
  bool hardDriveClockEnabled = true;
  bool reactOnGlobalBrightness = false;
  int colorR = 0xFF;
  int colorG = 0xFF;
  int colorB = 0xFF;

public:
  void setup()
  { }

  void loop()
  { }

  void addToJsonInfo(JsonObject& root)
  {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonArray lightArr = user.createNestedArray("Uhrzeit-Anzeige"); //name
    lightArr.add(hardDriveClockEnabled ? "aktiv" : "inaktiv"); //value
    lightArr.add(" "); //unit
  }

  void addToConfig(JsonObject &root)
  {
    JsonObject top = root.createNestedObject("Hard Drive Clock");
    top["enabled"]      = hardDriveClockEnabled;
    top["brightness"]   = reactOnGlobalBrightness;
    top["colorR"]       = colorR;
    top["colorG"]       = colorG;
    top["colorB"]       = colorB;
  }

  bool readFromConfig(JsonObject &root)
  {
    JsonObject top = root["Hard Drive Clock"];

    bool configComplete = !top.isNull();

    configComplete &= getJsonValue(top["enabled"], hardDriveClockEnabled);
    configComplete &= getJsonValue(top["brightness"], reactOnGlobalBrightness);
    configComplete &= getJsonValue(top["colorR"], colorR);
    configComplete &= getJsonValue(top["colorG"], colorG);
    configComplete &= getJsonValue(top["colorB"], colorB);

    return configComplete;
  }
  
  void drawHour(int hour)
  {
    hour = hour % 12;
    int led = (hour - hourStartOffset + HOUR_LED_COUNT) % HOUR_LED_COUNT;
    strip.setPixelColor(hourStartLed + led, RGBW32(colorR, colorG, colorB, 0));
  }

  void drawMinute(int minute, int second)
  {
    int seconds = (minute * 60) + second;
    double divider = seconds * 1.0 / MINUTE_DIVIDER; // Einerstelle zeigt die LED an, welche Leuchten muss, Nachkommastelle zeigt an um wie viel Prozent dunkler sie bereits leuchten muss 

    int firstMinuteLed = ((int)divider - minuteStartOffset + MINUTE_LED_COUNT) % MINUTE_LED_COUNT;
    int secondMinuteLed = (firstMinuteLed + 1 + MINUTE_LED_COUNT) % MINUTE_LED_COUNT;
    float brightness = divider - (int)divider;

    int r = (int)colorR * brightness;
    int g = (int)colorG * brightness;
    int b = (int)colorB * brightness;

    strip.setPixelColor(minuteStartLed + firstMinuteLed, RGBW32(colorR - r, colorG - g, colorB - b, 0));
    strip.setPixelColor(minuteStartLed + secondMinuteLed, RGBW32(r, g, b, 0));
  }

  void handleOverlayDraw()
  {
    if(hardDriveClockEnabled){
      time_t time = localTime;
      drawHour(hour(time));
      drawMinute(minute(time), second(time));
    }
  }

  uint16_t getId()
  {
    return USERMOD_ID_HARD_DRIVE_CLOCK;
  }

};
