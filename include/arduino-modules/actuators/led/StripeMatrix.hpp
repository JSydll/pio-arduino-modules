/**
 * @file StripeMatrix.hpp
 * @author Joschka Seydell (joschka@seydell.org)
 * @brief Provides a controller for an LED matrix made from a single stripe.
 * @version 0.1
 * @date 2021-02-07
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef ARDUINOMODULES__ACTUATORS_LED_STRIPEMATRIX_HPP_
#define ARDUINOMODULES__ACTUATORS_LED_STRIPEMATRIX_HPP_

// Standard header
#include <cstdint>

// Project header
#include "FastLedController.hpp"

// Available compile time options
// Stripe start (defaults to top-left):
// #define STRIPE_MATRIX_START_BOTTOM       // Define if the stripe begins at the bottom of the
// matrix #define STRIPE_MATRIX_START_RIGHT        // Define if the stripe begins at the right of
// the matrix

namespace ArduinoModules::Actuators::LED
{
/**
 * @brief Helper structure to enable compile time deduction on construction.
 *
 * @tparam LED_PIN Pin on which the stripe's data wire is connected.
 * @tparam LED_COUNT_X Number of dots in horizontal direction.
 * @tparam LED_COUNT_Y Number of dots in vertical direction.
 */
template <uint32_t LED_PIN, uint32_t LED_COUNT_X, uint32_t LED_COUNT_Y>
struct MatrixConfiguration
{
};

/** @brief Type used to represent a coordinate on the matrix */
using MatrixCoordinate = uint16_t;

/**
 * @brief Dot matrix made up from a single sequential LED stripe.
 *
 * Performs the necessary mapping from matrix coordinates to LED indices.
 */
class StripeMatrix
{
 public:
  /**
   * @brief Sets up the matrix and initializes the underlying LED stripe.
   *
   * @tparam LED_PIN Pin on which the stripe's data wire is connected.
   * @tparam LED_COUNT_X Number of dots in horizontal direction.
   * @tparam LED_COUNT_Y Number of dots in vertical direction.
   * @param config MatrixConfiguration to be used.
   * @param initialBrightness Brightness of the stripe to be set initially.
   */
  template <uint32_t LED_PIN, uint32_t LED_COUNT_X, uint32_t LED_COUNT_Y>
  StripeMatrix(const MatrixConfiguration<LED_PIN, LED_COUNT_X, LED_COUNT_Y>& config,
               const LedBrightness initialBrightness)
      : mLedController{LedConfiguration<LED_PIN, LED_COUNT_X * LED_COUNT_Y>{}, initialBrightness},
        mMatrixSizeX{LED_COUNT_X},
        mMatrixSizeY{LED_COUNT_Y}
  {
  }

  /** @brief Represents a single dot on the matrix. */
  struct Dot
  {
    MatrixCoordinate x;  //!< Coordinate in the horizontal direction.
    MatrixCoordinate y;  //!< Coordinate in the vertical direction.
  };

  /**
   * @brief Sets all dots on a horizontal row to the specified color.
   *
   * @param row Coordinate of the row.
   * @param from Starting coordinate of the first dot to be enlightened.
   * @param to Coordinate of the last dot to be enlightened.
   * @param color Color to be set on the dots.
   */
  void SetDotsInRow(const MatrixCoordinate row, const MatrixCoordinate from,
                    const MatrixCoordinate to, const CRGB color);

  /**
   * @brief Sets a range of dots (e.g. a rectangle)
   * 
   * @param from Upper left dot in the range.
   * @param to Lower right dot in the range.
   * @param color Color to be set on the dots.
   */
  void SetDotRange(const Dot& from, const Dot& to, const CRGB color);

  /**
   * @brief Exposes the underlying LedController.
   * 
   * @return FastLedController& Controller to manipulate single LEDs.
   */
  FastLedController& GetLedController();

  /**
   * @brief Calculates the LED index from the 
   * 
   * @param dot Dot on the matrix.
   * @return LedIndex Native index on the underlying stripe.
   */
  LedIndex GetDotLedIndex(const Dot& dot);

 private:
  FastLedController mLedController;
  const uint32_t mMatrixSizeX;
  const uint32_t mMatrixSizeY;
};

}  // namespace ArduinoModules::Actuators::LED

#endif  // ARDUINOMODULES__ACTUATORS_LED_STRIPEMATRIX_HPP_