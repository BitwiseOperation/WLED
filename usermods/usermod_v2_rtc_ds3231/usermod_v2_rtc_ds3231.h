#pragma once

#include "wled.h"
#include <RTClib.h>

class RTCDS3231Usermod : public Usermod
{
private:
  // Private class members. You can declare variables and functions only accessible to your usermod here
  String rtcError = "Working fine";
  bool rtcErrorDisabled = false;
  RTC_DS3231 rtc;

  // ---- Variables modified by settings below -----
  // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
  bool rtcEnabled = true;

public:
  void setup()
  { 
    if(rtcEnabled)
    {
      PinManagerPinType pins[2] = { { i2c_scl, true }, { i2c_sda, true } };
      if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::HW_I2C))
      {
        rtcError         = "Pin Alloc Error";
        rtcErrorDisabled = true;
        return;
      }
    }
    if(!rtc.begin())
    {
      rtcError = "No Chip present";
      rtcErrorDisabled = true;
      return;
    }
    if(!rtc.lostPower())
    {
      DateTime rtcTime = rtc.now();
      toki.setTime(rtcTime.unixtime(), TOKI_NO_MS_ACCURACY, TOKI_TS_RTC);
      updateLocalTime();
    }
  }

  void loop()
  {
    if(rtcEnabled)
    {
      if(strip.isUpdating()) return;
      if(!rtcErrorDisabled && toki.isTick())
      {
        time_t t = toki.second();
        if(t != rtc.now().unixtime())
        {
          rtc.adjust(DateTime(t));
        }
      }
    }
  }

  void addToJsonInfo(JsonObject& root)
  {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonArray rtcArr = user.createNestedArray("Real-Time Clock");
    rtcArr.add(rtcEnabled ? "aktiv" : "inaktiv");
    rtcArr.add(" ");
    
    JsonArray rtcErrorArr = user.createNestedArray("Real-Time Clock Error");
    rtcErrorArr.add(rtcError);
    rtcErrorArr.add(" ");
  }

  void addToConfig(JsonObject &root)
  {
    JsonObject top = root.createNestedObject("Real-Time Clock");
    top["enabled"] = rtcEnabled;
  }

  bool readFromConfig(JsonObject &root)
  {
    JsonObject top = root["Real-Time Clock"];

    bool configComplete = !top.isNull();

    configComplete &= getJsonValue(top["enabled"], rtcEnabled);

    return configComplete;
  }

  uint16_t getId()
  {
    return USERMOD_ID_RTC_DS3231;
  }

};
