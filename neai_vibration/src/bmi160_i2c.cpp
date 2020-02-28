/**********************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/


#include "bmi160.h"


//*****************************************************************************
BMI160_I2C::BMI160_I2C(I2C &i2cBus, uint8_t i2cAdrs)
:m_i2cBus(i2cBus), m_Wadrs(i2cAdrs << 1), m_Radrs((i2cAdrs << 1) | 1)
{
    
}


//*****************************************************************************   
int32_t BMI160_I2C::readRegister(Registers reg, uint8_t *data)
{
    int32_t rtnVal = -1;
    char packet[] = {static_cast<char>(reg)};
    
    if(m_i2cBus.write(m_Wadrs, packet, 1) == 0)
    {
        rtnVal = m_i2cBus.read(m_Radrs, reinterpret_cast<char *>(data), 1);
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160_I2C::writeRegister(Registers reg, const uint8_t data)
{
    char packet[] = {static_cast<char>(reg), static_cast<char>(data)};
    
    return m_i2cBus.write(m_Wadrs, packet, sizeof(packet));
}


//*****************************************************************************
int32_t BMI160_I2C::readBlock(Registers startReg, Registers stopReg, 
uint8_t *data)
{
    int32_t rtnVal = -1;
    int32_t numBytes = ((stopReg - startReg) + 1);
    char packet[] = {static_cast<char>(startReg)};
    
    if(m_i2cBus.write(m_Wadrs, packet, 1) == 0)
    {
        rtnVal = m_i2cBus.read(m_Radrs, reinterpret_cast<char *>(data), numBytes);
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160_I2C::writeBlock(Registers startReg, Registers stopReg, 
const uint8_t *data)
{
    int32_t numBytes = ((stopReg - startReg) + 1);
    char packet[32];
    
    packet[0] = static_cast<char>(startReg);
    
    memcpy(packet + 1, data, numBytes);
    
    return m_i2cBus.write(m_Wadrs, packet, (numBytes+1) * sizeof(char));
}
