/**
 * @file SerialConsole.hpp
 * @author Joschka Seydell (joschka@seydell.org)
 * @brief Provides a RAII wrapper around the serial console of the Arduino framework.
 * @version 0.1
 * @date 2021-02-07
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef ARDUINOMODULES__IO_SERIALCONSOLE_HPP_
#define ARDUINOMODULES__IO_SERIALCONSOLE_HPP_

// Platform header
#include <Arduino.h>

namespace ArduinoModules
{
namespace IO
{
/**
 * @brief Wraps the standard serial with RAII style behavior.
 *
 * Simply instantiate it during setup phase and use it with the provided operator().
 */
class SerialConsole
{
 public:
  /**
   * @brief Sets up the serial console.
   *
   * @param baudRate Baud rate to be used (defaults to 115200).
   */
  SerialConsole(int baudRate = 115200) { Serial.begin(baudRate); }
  ~SerialConsole() { Serial.flush(); };

  /**
   * @brief Provides access to the underlying HardwareSerial object.
   *
   * @return HardwareSerial& Instance of the serial, can be used to .println() etc.
   */
  HardwareSerial& operator()() { return Serial; }
};
}  // namespace IO
}  // namespace ArduinoModules

#endif  // ARDUINOMODULES__IO_SERIALCONSOLE_HPP_
