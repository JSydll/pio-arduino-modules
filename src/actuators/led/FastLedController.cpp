#include "arduino-modules/actuators/led/FastLedController.hpp"

// Standard headers
#include <algorithm>

using namespace ArduinoModules::Actuators::LED;

CRGBPalette16 CreateRandomPalette16()
{
  CRGBPalette16 palette;
  for (int i = 0; i < 16; i++)
  {
    palette[i] = CHSV(random8(), 255, 255);
  }
  return palette;
}

LedRange FastLedController::CheckRange(const LedRange& range)
{
  const bool fullStripe = (range == LedRange{});
  const LedIndex begin = (fullStripe ? 0 : range.first);
  const LedIndex end = (fullStripe ? mLeds.size() - 1 : range.second);
  if (begin > end)
  {
    return {};
  }
  return {begin, end};
}

void FastLedController::FillWithColor(const CRGB color, const LedIndex led) { mLeds[led] = color; }

void FastLedController::FillWithColor(const CRGB color, const LedRange& range)
{
  const LedRange checked = CheckRange(range);
  for (LedIndex led = checked.first; led <= checked.second; ++led)
  {
    FillWithColor(color, led);
  }
}

void FastLedController::FillFromPalette(const CRGBPalette16& palette, const PaletteColorIndex color,
                                        const LedIndex led, const bool blend)
{
  mLeds[led] = (blend ? ColorFromPalette(palette, color, 255, LINEARBLEND) : palette[color]);
}

void FastLedController::FillFromPalette(const CRGBPalette16& palette, const PaletteColorIndex color,
                                        const LedRange& range, const bool blend)
{
  const LedRange checked = CheckRange(range);
  for (LedIndex led = checked.first; led <= checked.second; ++led)
  {
    FillFromPalette(palette, color, led, blend);
  }
}

void FastLedController::SetStripeBrightness(const LedBrightness brightness)
{
  FastLED.setBrightness(brightness);
}

#ifdef _LED_CONTROL_DYN_BRIGHTNESS
void FastLedController::EnableSingleLedBrightnessAdoption()
{
  if (not mHsvLeds.empty())
  {
    mHsvLeds.clear();
  }
  mHsvLeds.reserve(mLeds.size());
  for (const auto& led : mLeds)
  {
    mHsvLeds.emplace_back(rgb2hsv_approximate(led));
  }
}

void FastLedController::AdoptLedBrightnessTo(const LedBrightnessDifferential diff,
                                             const LedIndex led)
{
  if (mHsvLeds.empty())
  {
    // Single LED brightnes adoption not enabled.
    return;
  }
  // Calculate the raw result (might be smaller than 0 or bigger than 255)
  int rawResult = mHsvLeds[led].value + diff;
  mHsvLeds[led].value = static_cast<LedBrightness>(std::clamp(rawResult, 0, 255));
  mLeds[led] = mHsvLeds[led];
}

void FastLedController::AdoptLedBrightnessTo(const LedBrightnessDifferential diff,
                                             const LedRange& range)
{
  if (mHsvLeds.empty())
  {
    // Single LED brightnes adoption not enabled.
    return;
  }
  const LedRange checked = CheckRange(range);
  for (LedIndex led = checked.first; led <= checked.second; ++led)
  {
    AdoptLedBrightnessTo(diff, led);
  }
}
#endif  // #ifdef _LED_CONTROL_DYN_BRIGHTNESS

void FastLedController::Show() { FastLED.show(); }