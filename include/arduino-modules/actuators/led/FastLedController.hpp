/**
 * @file FastLedController.hpp
 * @author Joschka Seydell (joschka@seydell.org)
 * @brief Provides an LED controller based on the FastLED library.
 * @version 0.1
 * @date 2021-02-07
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef ARDUINOMODULES__ACTUATORS_LED_FASTLEDCONTROLLER_HPP_
#define ARDUINOMODULES__ACTUATORS_LED_FASTLEDCONTROLLER_HPP_

// Standard header
#include <chrono>
#include <cstdint>
#include <vector>

// Third-party header
#include "FastLED.h"

#ifdef _LED_CONTROL_DYN_BRIGHTNESS
#pragma message( \
    "ArduinoModules::Actuators::LED::FastLedController compiled with \
support for dynamic (per LED) brightness control.")
#else
#pragma message( \
    "ArduinoModules::Actuators::LED::FastLedController compiled without \
support for dynamic (per LED) brightness control.")
#endif

namespace ArduinoModules::Actuators::LED
{
/**
 * @brief Creates a palete filled with random colors.
 *
 * @return A 16 color palette.
 */
CRGBPalette16 CreateRandomPalette16();

using PaletteColorIndex = uint8_t;               //!< Index of a color in a palette.
using LedIndex = uint16_t;                       //!< Index of the LED on the stripe.
using LedRange = std::pair<LedIndex, LedIndex>;  //!< Range of LEDs (first = begin, second = end).
using LedBrightness = uint8_t;                   //!< Brightness of an LED pixel.
using LedBrightnessDifferential = int16_t;       //!< Differential of brigthness.

/**
 * @brief Helper structure to enable compile time deduction on construction.
 * Essentially, it serves to pass in the actual value of the tparams to be accessible in the
 * constructor of the FastLedController (as ctors cannot be called directly).
 * @tparam LED_PIN Pin of the LED stripe to set up.
 * @tparam LED_COUNT Number of LEDs of the stripe.
 */
template <uint32_t LED_PIN, uint32_t LED_COUNT>
struct LedConfiguration
{
};

/**
 * @brief Provides methods to control single and ranges of LEDs of an underlying stripe.
 *
 * @note All methods changing the LEDs requires a manual call to Show().
 */
class FastLedController
{
 public:
  /**
   * @brief Sets up the controller and initializes the underlying LED stripe.
   *
   * @tparam LED_PIN Pin of the LED stripe to set up.
   * @tparam LED_COUNT Number of LEDs of the stripe.
   * @param config LedConfiguration to be used.
   * @param initialBrightness Initial brightness to be set.
   * @param correction Sets the color correction for the stripe.
   */
  template <uint32_t LED_PIN, uint32_t LED_COUNT>
  FastLedController(const LedConfiguration<LED_PIN, LED_COUNT>& config,
                    const LedBrightness initialBrightness,
                    const CRGB correction = TypicalPixelString)
      : mLeds{LED_COUNT, CRGB::Black}
  /* Use implicit default initialization for mHsvLeds to support the #ifdef
   * _LED_CONTROL_DYN_BRIGHTNESS */
  {
    FastLED.addLeds<WS2812B, LED_PIN>(mLeds.data(), LED_COUNT).setCorrection(correction);
    SetStripeBrightness(initialBrightness);
    Show();
  };
  ~FastLedController() = default;

  /**
   * @brief Fills a single LED pixel with the given color.
   *
   * @param color Color to be set.
   * @param led Index of the LED.
   */
  void FillWithColor(const CRGB color, const LedIndex led);

  /**
   * @brief Fills a range of LEDs with the given color.
   *
   * @param color Color to be set.
   * @param range Range to be set. If left empty, the complete sripe will be set.
   */
  void FillWithColor(const CRGB color, const LedRange& range = LedRange{});

  /**
   * @brief Fills a single LED pixel with the selected color from the palette.
   *
   * @param palette Palette to be used.
   * @param color Index of the color in the palette.
   * @param led Index of the LED pixel.
   * @param blend Flag if color blending should be used (defaults to true).
   */
  void FillFromPalette(const CRGBPalette16& palette, PaletteColorIndex color, const LedIndex led,
                       const bool blend = true);

  /**
   * @brief Fills a range of LEDs with the selected color from the palette.
   *
   * @param palette Palette to be used.
   * @param colorIndex Index of the color in the palette.
   * @param range Range to be set. If left empty, the complete sripe will be set.
   * @param blend Flag if color blending should be used (defaults to true).
   */
  void FillFromPalette(const CRGBPalette16& palette, const PaletteColorIndex color,
                       const LedRange& range = LedRange{}, const bool blend = true);

  /**
   * @brief Sets the brightness for the complete stripe.
   *
   * @param brightness Value to be set.
   */
  void SetStripeBrightness(const LedBrightness brightness);

#ifdef _LED_CONTROL_DYN_BRIGHTNESS
  /**
   * @brief Initializes the dynamic brightness controll (resource costly).
   *
   * Dynamic brightness control is necessary to dim or light up (single) LEDs while preserving their
   * color.
   */
  void EnableSingleLedBrightnessAdoption();

  /**
   * @brief Changes the LED's brightness.
   *
   * @param diff The amount of brightness to be reduced (negative differential) or increased
   * (positive differential).
   * @param led Index of the LED to be dimmed/lightened up.
   */
  void AdoptLedBrightnessTo(const LedBrightnessDifferential diff, const LedIndex led);

  /**
   * @brief Increases the brightness.
   *
   * @param diff The amount of brightness to be reduced (negative differential) or increased
   * (positive differential).
   * @param range Range to be manipulated. If left empty, the complete sripe will be set.
   */
  void AdoptLedBrightnessTo(const LedBrightnessDifferential diff,
                            const LedRange& range = LedRange{});
#endif  // #ifdef _LED_CONTROL_DYN_BRIGHTNESS

  /**
   * @brief Actually show the currently configured state of the LEDs.
   */
  void Show();

 private:
  std::vector<CRGB> mLeds;
#ifdef _LED_CONTROL_DYN_BRIGHTNESS
  // To use HSV operations on the LEDs, a shadow array of HSV values must be stored. Ideally, this
  // is only seldomly build from scratch, as this takes quite some time.
  std::vector<CHSV> mHsvLeds;
#endif  // #ifdef _LED_CONTROL_DYN_BRIGHTNESS

  /**
   * @brief Check if the given range is valid or whether the complete stripe shall be addressed.
   *
   * @param range Range to check
   * @return LedRange Checked / defaulted range.
   */
  LedRange CheckRange(const LedRange& range);
};
}  // namespace ArduinoModules::Actuators::LED

#endif  // ARDUINOMODULES__ACTUATORS_LED_FASTLEDCONTROLLER_HPP_