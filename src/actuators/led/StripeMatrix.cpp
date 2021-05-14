#include <arduino-modules/actuators/led/StripeMatrix.hpp>

// Preprocessor macros
#ifdef STRIPE_MATRIX_START_RIGHT
#define GET_ABSOLUTE_X(sizeX, x) (sizeX + 1 - x)
#else
#define GET_ABSOLUTE_X(sizeX, x) (x)
#endif

#ifdef STRIPE_MATRIX_START_BOTTOM
#define GET_ABSOLUTE_Y(sizeY, y) (sizeY + 1 - y)
#else
#define GET_ABSOLUTE_Y(sizeY, y) (y)
#endif

using namespace ArduinoModules::Actuators::LED;

void StripeMatrix::SetDotsInRow(const MatrixCoordinate row, const MatrixCoordinate from,
                                const MatrixCoordinate to, const CRGB color, const bool show)
{
  const LedRange range{GetDotLedIndex(Dot{from, row}), GetDotLedIndex(Dot{to, row})};
  mLedController.FillWithColor(color, range);
  if (show)
  {
    mLedController.Show();
  }
}

void StripeMatrix::SetDotRange(const Dot& begin, const Dot& end, const CRGB color, const bool show)
{
  for (auto y = begin.y; y <= end.y; ++y)
  {
    SetDotsInRow(y, begin.x, end.x, color, show);
  }
}

FastLedController& StripeMatrix::GetLedController() { return mLedController; }

LedIndex StripeMatrix::GetDotLedIndex(const Dot& dot)
{
  const auto absX = GET_ABSOLUTE_X(mMatrixSizeX, dot.x);
  const auto absY = GET_ABSOLUTE_Y(mMatrixSizeY, dot.y);
  const auto fullRows = (absY - 1) * mMatrixSizeX;
  const auto fullColumnForEvenRowCount = (mMatrixSizeX + 1) * ((absY + 1) % 2);
  const auto evenOddMultiplier = (absY % 2) - ((absY + 1) % 2);
  const auto partialRow = fullColumnForEvenRowCount + evenOddMultiplier * absX;
  return ((fullRows + partialRow) - 1);
}