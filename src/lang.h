#pragma once
#include <Arduino.h>

enum CoreLanguage {
  LANG_PT_BR = 0,
  LANG_EN_US = 1
};

extern CoreLanguage GlobalLanguage;

inline String tr(const String& pt, const String& en) {
  return (GlobalLanguage == LANG_EN_US) ? en : pt;
}
