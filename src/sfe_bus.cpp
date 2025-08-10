/*
  An Arduino Library which allows you to communicate seamlessly with u-blox GNSS modules using the Configuration Interface

  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/15136
  https://www.sparkfun.com/products/16481
  https://www.sparkfun.com/products/16344
  https://www.sparkfun.com/products/18037
  https://www.sparkfun.com/products/18719
  https://www.sparkfun.com/products/18774
  https://www.sparkfun.com/products/19663
  https://www.sparkfun.com/products/17722

  Original version by Nathan Seidle @ SparkFun Electronics, September 6th, 2018
  v2.0 rework by Paul Clark @ SparkFun Electronics, December 31st, 2020
  v3.0 rework by Paul Clark @ SparkFun Electronics, December 8th, 2022

  https://github.com/sparkfun/SparkFun_u-blox_GNSS_v3

  This library is an updated version of the popular SparkFun u-blox GNSS Arduino Library.
  v3 uses the u-blox Configuration Interface (VALSET and VALGET) to:
  detect the module (during begin); configure message intervals; configure the base location; etc..

  This version of the library will not work with older GNSS modules.
  It is specifically written for newer modules like the ZED-F9P, ZED-F9R and MAX-M10S.
  For older modules, please use v2 of the library: https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.19

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  The MIT License (MIT)
  Copyright (c) 2018 SparkFun Electronics
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
  associated documentation files (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to
  do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial
  portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// sfe_bus.cpp

#include "sfe_bus.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
//

namespace SparkFun_UBLOX_GNSS
{

  SfeI2C::SfeI2C(void) : _i2cPort{nullptr}, _address{0}
  {
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // I2C init()
  //
  // Methods to init/setup this device.
  // The caller can provide a Wire Port, or this class will use the default.
  // Always update the address in case the user has changed the I2C address - see Example9
  bool SfeI2C::init(I2C_HandleTypeDef &i2cHandle, uint8_t address)
  {
    // if we don't have a wire port already
    if (!_i2cPort)
    {
      _i2cPort = &i2cHandle;
    }

    _address = address;

    return true;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // ping()
  //
  // Is a device connected?
  bool SfeI2C::ping()
  {

    if (!_i2cPort)
      return false;

    uint8_t ret = HAL_I2C_IsDeviceReady(_i2cPort, _address, 3, 5);
    if (ret != HAL_OK) return false;
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // available()
  //
  // Checks how many bytes are waiting in the GNSS's I2C buffer
  // It does this by reading registers 0xFD and 0xFE
  //
  // From the u-blox integration manual:
  // "There are two forms of DDC read transfer. The "random access" form includes a peripheral register
  //  address and thus allows any register to be read. The second "current address" form omits the
  //  register address. If this second form is used, then an address pointer in the receiver is used to
  //  determine which register to read. This address pointer will increment after each read unless it
  //  is already pointing at register 0xFF, the highest addressable register, in which case it remains
  //  unaltered."

  uint16_t SfeI2C::available()
  {

    if (!_i2cPort)
      return false;

    // Get the number of bytes available from the module
    uint16_t bytesAvailable = 0;
    uint8_t data[2];
    uint8_t ret = HAL_I2C_Mem_Read(_i2cPort, _address, 0xFD, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
    if (ret != HAL_OK)
    {
      return 0;
    }
    else 
    {
      uint8_t msb = data[0];
      uint8_t lsb = data[1];
      bytesAvailable = (uint16_t)msb << 8 | lsb;
    }

    return (bytesAvailable);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // writeBytes()

  uint8_t SfeI2C::writeBytes(uint8_t *dataToWrite, uint8_t length)
  {
    if (length == 0)
      return 0;

    uint8_t ret = HAL_I2C_Master_Transmit(_i2cPort, _address, dataToWrite, length, HAL_MAX_DELAY);
    if(ret != HAL_OK) return 0;
    else return length;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // readBytes()

  uint8_t SfeI2C::readBytes(uint8_t *data, uint8_t length)
  {
    if (length == 0)
      return 0;

    uint8_t ret = HAL_I2C_Master_Receive(_i2cPort, _address, data, length, HAL_MAX_DELAY);
    if(ret != HAL_OK) return 0;
    else return length;
  }
}
