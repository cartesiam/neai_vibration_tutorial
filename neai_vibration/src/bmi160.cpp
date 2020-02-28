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
* Products, Inc. shall not be used except as stated in the Maxim
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/


#include "bmi160.h"


const struct BMI160::AccConfig BMI160::DEFAULT_ACC_CONFIG = {SENS_2G, 
                                                             ACC_US_OFF, 
                                                             ACC_BWP_2, 
                                                             ACC_ODR_8};
                                                             
const struct BMI160::GyroConfig BMI160::DEFAULT_GYRO_CONFIG = {DPS_2000, 
                                                               GYRO_BWP_2, 
                                                               GYRO_ODR_8};

///Period of internal counter
static const float SENSOR_TIME_LSB = 39e-6;

static const float SENS_2G_LSB_PER_G = 16384.0F;
static const float SENS_4G_LSB_PER_G = 8192.0F;
static const float SENS_8G_LSB_PER_G = 4096.0F;
static const float SENS_16G_LSB_PER_G = 2048.0F;

static const float SENS_2000_DPS_LSB_PER_DPS = 16.4F;
static const float SENS_1000_DPS_LSB_PER_DPS = 32.8F;
static const float SENS_500_DPS_LSB_PER_DPS = 65.6F;
static const float SENS_250_DPS_LSB_PER_DPS = 131.2F;
static const float SENS_125_DPS_LSB_PER_DPS = 262.4F;
    

//*****************************************************************************
int32_t BMI160::setSensorPowerMode(Sensors sensor, PowerModes pwrMode)
{
    int32_t rtnVal = -1;
    
    switch(sensor)
    {
        case MAG:
            rtnVal = writeRegister(CMD, (MAG_SET_PMU_MODE | pwrMode));
        break;
        
        case GYRO:
            rtnVal = writeRegister(CMD, (GYR_SET_PMU_MODE | pwrMode));
        break;
        
        case ACC:
            rtnVal = writeRegister(CMD, (ACC_SET_PMU_MODE | pwrMode));
        break;
        
        default:
            rtnVal = -1;
        break;
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160::setSensorConfig(const AccConfig &config)
{
    uint8_t data[2];
    
    data[0] = ((config.us << ACC_US_POS) | (config.bwp << ACC_BWP_POS) | 
               (config.odr << ACC_ODR_POS));
    data[1] = config.range;
    
    return writeBlock(ACC_CONF, ACC_RANGE, data);
}


//*****************************************************************************
int32_t BMI160::setSensorConfig(const GyroConfig &config)
{
    uint8_t data[2];
    
    data[0] = ((config.bwp << GYRO_BWP_POS) | (config.odr << GYRO_ODR_POS));
    data[1] = config.range;
    
    return writeBlock(GYR_CONF, GYR_RANGE, data);
}


//*****************************************************************************
int32_t BMI160::getSensorConfig(AccConfig &config)
{
    uint8_t data[2];
    int32_t rtnVal = readBlock(ACC_CONF, ACC_RANGE, data);
    
    if(rtnVal == RTN_NO_ERROR)
    {
        config.range = static_cast<BMI160::AccRange>(
        (data[1] & ACC_RANGE_MASK));
        config.us = static_cast<BMI160::AccUnderSampling>(
        ((data[0] & ACC_US_MASK) >> ACC_US_POS));
        config.bwp = static_cast<BMI160::AccBandWidthParam>(
        ((data[0] & ACC_BWP_MASK) >> ACC_BWP_POS));
        config.odr = static_cast<BMI160::AccOutputDataRate>(
        ((data[0] & ACC_ODR_MASK) >> ACC_ODR_POS));
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160::getSensorConfig(GyroConfig &config)
{
    uint8_t data[2];
    int32_t rtnVal = readBlock(GYR_CONF, GYR_RANGE, data);
    
    if(rtnVal == RTN_NO_ERROR)
    {
        config.range = static_cast<BMI160::GyroRange>(
        (data[1] & GYRO_RANGE_MASK));
        config.bwp = static_cast<BMI160::GyroBandWidthParam>(
        ((data[0] & GYRO_BWP_MASK) >> GYRO_BWP_POS));
        config.odr = static_cast<BMI160::GyroOutputDataRate>(
        ((data[0] & GYRO_ODR_MASK) >> GYRO_ODR_POS));
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160::getSensorAxis(SensorAxis axis, AxisData &data, AccRange range)
{
    uint8_t localData[2];
    int32_t rtnVal;
    
    switch(axis)
    {
        case X_AXIS:
            rtnVal = readBlock(DATA_14, DATA_15, localData);
        break;
        
        case Y_AXIS:
            rtnVal = readBlock(DATA_16, DATA_17, localData);
        break;
        
        case Z_AXIS:
            rtnVal = readBlock(DATA_18, DATA_19, localData);
        break;
        
        default:
            rtnVal = -1;
        break;
    }
    
    if(rtnVal == RTN_NO_ERROR)
    {
        data.raw = ((localData[1] << 8) | localData[0]);
        switch(range)
        {
            case SENS_2G:
                data.scaled = (data.raw/SENS_2G_LSB_PER_G);
            break;
            
            case SENS_4G:
                data.scaled = (data.raw/SENS_4G_LSB_PER_G);
            break;
            
            case SENS_8G:
                data.scaled = (data.raw/SENS_8G_LSB_PER_G);
            break;
            
            case SENS_16G:
                data.scaled = (data.raw/SENS_16G_LSB_PER_G);
            break;
        }
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160::getSensorAxis(SensorAxis axis, AxisData &data, GyroRange range)
{
    uint8_t localData[2];
    int32_t rtnVal;
    
    switch(axis)
    {
        case X_AXIS:
            rtnVal = readBlock(DATA_8, DATA_9, localData);
        break;
        
        case Y_AXIS:
            rtnVal = readBlock(DATA_10, DATA_11, localData);
        break;
        
        case Z_AXIS:
            rtnVal = readBlock(DATA_12, DATA_13, localData);
        break;
        
        default:
            rtnVal = -1;
        break;
    }
    
    if(rtnVal == RTN_NO_ERROR)
    {
        data.raw = ((localData[1] << 8) | localData[0]);
        switch(range)
        {
            case DPS_2000:
                data.scaled = (data.raw/SENS_2000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_1000:
                data.scaled = (data.raw/SENS_1000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_500:
                data.scaled = (data.raw/SENS_500_DPS_LSB_PER_DPS);
            break;
            
            case DPS_250:
                data.scaled = (data.raw/SENS_250_DPS_LSB_PER_DPS);
            break;
            
            case DPS_125:
                data.scaled = (data.raw/SENS_125_DPS_LSB_PER_DPS);
            break;
        }
    }
    
    return rtnVal;
}
    

//*****************************************************************************    
int32_t BMI160::getSensorXYZ(SensorData &data, AccRange range)
{
    uint8_t localData[6];
    int32_t rtnVal = readBlock(DATA_14, DATA_19, localData);
    
    if(rtnVal == RTN_NO_ERROR)
    {
        data.xAxis.raw = ((localData[1] << 8) | localData[0]);
        data.yAxis.raw = ((localData[3] << 8) | localData[2]);
        data.zAxis.raw = ((localData[5] << 8) | localData[4]);
        
        switch(range)
        {
            case SENS_2G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_2G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_2G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_2G_LSB_PER_G);
            break;
            
            case SENS_4G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_4G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_4G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_4G_LSB_PER_G);
            break;
            
            case SENS_8G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_8G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_8G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_8G_LSB_PER_G);
            break;
            
            case SENS_16G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_16G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_16G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_16G_LSB_PER_G);
            break;
        }
    }
    
    return rtnVal;
}


//*****************************************************************************    
int32_t BMI160::getSensorXYZ(SensorData &data, GyroRange range)
{
    uint8_t localData[6];
    int32_t rtnVal = readBlock(DATA_8, DATA_13, localData);
    
    if(rtnVal == RTN_NO_ERROR)
    {
        data.xAxis.raw = ((localData[1] << 8) | localData[0]);
        data.yAxis.raw = ((localData[3] << 8) | localData[2]);
        data.zAxis.raw = ((localData[5] << 8) | localData[4]);
        
        switch(range)
        {
            case DPS_2000:
                data.xAxis.scaled = (data.xAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_1000:
                data.xAxis.scaled = (data.xAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_500:
                data.xAxis.scaled = (data.xAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_500_DPS_LSB_PER_DPS);
            break;
            
            case DPS_250:
                data.xAxis.scaled = (data.xAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_250_DPS_LSB_PER_DPS);
            break;
            
            case DPS_125:
                data.xAxis.scaled = (data.xAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_125_DPS_LSB_PER_DPS);
            break;
        }
    }
    
    return rtnVal;
}


//***************************************************************************** 
int32_t BMI160::getSensorXYZandSensorTime(SensorData &data, 
                                          SensorTime &sensorTime, 
                                          AccRange range)
{
    uint8_t localData[9];
    int32_t rtnVal = readBlock(DATA_14, SENSORTIME_2, localData);
    if(rtnVal == RTN_NO_ERROR)
    {
        data.xAxis.raw = ((localData[1] << 8) | localData[0]);
        data.yAxis.raw = ((localData[3] << 8) | localData[2]);
        data.zAxis.raw = ((localData[5] << 8) | localData[4]);
        
        switch(range)
        {
            case SENS_2G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_2G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_2G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_2G_LSB_PER_G);
            break;
            
            case SENS_4G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_4G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_4G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_4G_LSB_PER_G);
            break;
            
            case SENS_8G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_8G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_8G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_8G_LSB_PER_G);
            break;
            
            case SENS_16G:
                data.xAxis.scaled = (data.xAxis.raw/SENS_16G_LSB_PER_G);
                data.yAxis.scaled = (data.yAxis.raw/SENS_16G_LSB_PER_G);
                data.zAxis.scaled = (data.zAxis.raw/SENS_16G_LSB_PER_G);
            break;
        }
        
        sensorTime.raw = ((localData[8] << 16) | (localData[7] << 8) | 
                           localData[6]);
        sensorTime.seconds = (sensorTime.raw * SENSOR_TIME_LSB);
    }
    
    return rtnVal;
}


//***************************************************************************** 
int32_t BMI160::getSensorXYZandSensorTime(SensorData &data, 
                                          SensorTime &sensorTime, 
                                          GyroRange range)
{
    uint8_t localData[16];
    int32_t rtnVal = readBlock(DATA_8, SENSORTIME_2, localData);
    if(rtnVal == RTN_NO_ERROR)
    {
        data.xAxis.raw = ((localData[1] << 8) | localData[0]);
        data.yAxis.raw = ((localData[3] << 8) | localData[2]);
        data.zAxis.raw = ((localData[5] << 8) | localData[4]);
        
        switch(range)
        {
            case DPS_2000:
                data.xAxis.scaled = (data.xAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_1000:
                data.xAxis.scaled = (data.xAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_500:
                data.xAxis.scaled = (data.xAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_500_DPS_LSB_PER_DPS);
            break;
            
            case DPS_250:
                data.xAxis.scaled = (data.xAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_250_DPS_LSB_PER_DPS);
            break;
            
            case DPS_125:
                data.xAxis.scaled = (data.xAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                data.yAxis.scaled = (data.yAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                data.zAxis.scaled = (data.zAxis.raw/SENS_125_DPS_LSB_PER_DPS);
            break;
        }
        
        sensorTime.raw = ((localData[14] << 16) | (localData[13] << 8) | 
                           localData[12]);
        sensorTime.seconds = (sensorTime.raw * SENSOR_TIME_LSB);
    }
    
    return rtnVal;
}


//***************************************************************************** 
int32_t BMI160::getGyroAccXYZandSensorTime(SensorData &accData, 
                                           SensorData &gyroData, 
                                           SensorTime &sensorTime, 
                                           AccRange accRange, 
                                           GyroRange gyroRange)
{
    uint8_t localData[16];
    int32_t rtnVal = readBlock(DATA_8, SENSORTIME_2, localData);
    if(rtnVal == RTN_NO_ERROR)
    {
        gyroData.xAxis.raw = ((localData[1] << 8) | localData[0]);
        gyroData.yAxis.raw = ((localData[3] << 8) | localData[2]);
        gyroData.zAxis.raw = ((localData[5] << 8) | localData[4]);
        
        accData.xAxis.raw = ((localData[7] << 8) | localData[6]);
        accData.yAxis.raw = ((localData[9] << 8) | localData[8]);
        accData.zAxis.raw = ((localData[11] << 8) | localData[10]);
        
        switch(gyroRange)
        {
            case DPS_2000:
                gyroData.xAxis.scaled = (gyroData.xAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                gyroData.yAxis.scaled = (gyroData.yAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
                gyroData.zAxis.scaled = (gyroData.zAxis.raw/SENS_2000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_1000:
                gyroData.xAxis.scaled = (gyroData.xAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                gyroData.yAxis.scaled = (gyroData.yAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
                gyroData.zAxis.scaled = (gyroData.zAxis.raw/SENS_1000_DPS_LSB_PER_DPS);
            break;
            
            case DPS_500:
                gyroData.xAxis.scaled = (gyroData.xAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                gyroData.yAxis.scaled = (gyroData.yAxis.raw/SENS_500_DPS_LSB_PER_DPS);
                gyroData.zAxis.scaled = (gyroData.zAxis.raw/SENS_500_DPS_LSB_PER_DPS);
            break;
            
            case DPS_250:
                gyroData.xAxis.scaled = (gyroData.xAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                gyroData.yAxis.scaled = (gyroData.yAxis.raw/SENS_250_DPS_LSB_PER_DPS);
                gyroData.zAxis.scaled = (gyroData.zAxis.raw/SENS_250_DPS_LSB_PER_DPS);
            break;
            
            case DPS_125:
                gyroData.xAxis.scaled = (gyroData.xAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                gyroData.yAxis.scaled = (gyroData.yAxis.raw/SENS_125_DPS_LSB_PER_DPS);
                gyroData.zAxis.scaled = (gyroData.zAxis.raw/SENS_125_DPS_LSB_PER_DPS);
            break;
        }
        
        switch(accRange)
        {
            case SENS_2G:
                accData.xAxis.scaled = (accData.xAxis.raw/SENS_2G_LSB_PER_G);
                accData.yAxis.scaled = (accData.yAxis.raw/SENS_2G_LSB_PER_G);
                accData.zAxis.scaled = (accData.zAxis.raw/SENS_2G_LSB_PER_G);
            break;
            
            case SENS_4G:
                accData.xAxis.scaled = (accData.xAxis.raw/SENS_4G_LSB_PER_G);
                accData.yAxis.scaled = (accData.yAxis.raw/SENS_4G_LSB_PER_G);
                accData.zAxis.scaled = (accData.zAxis.raw/SENS_4G_LSB_PER_G);
            break;
            
            case SENS_8G:
                accData.xAxis.scaled = (accData.xAxis.raw/SENS_8G_LSB_PER_G);
                accData.yAxis.scaled = (accData.yAxis.raw/SENS_8G_LSB_PER_G);
                accData.zAxis.scaled = (accData.zAxis.raw/SENS_8G_LSB_PER_G);
            break;
            
            case SENS_16G:
                accData.xAxis.scaled = (accData.xAxis.raw/SENS_16G_LSB_PER_G);
                accData.yAxis.scaled = (accData.yAxis.raw/SENS_16G_LSB_PER_G);
                accData.zAxis.scaled = (accData.zAxis.raw/SENS_16G_LSB_PER_G);
            break;
        }
        
        sensorTime.raw = ((localData[14] << 16) | (localData[13] << 8) | 
                           localData[12]);
        sensorTime.seconds = (sensorTime.raw * SENSOR_TIME_LSB);
    }
    
    return rtnVal;
}


//***************************************************************************** 
int32_t BMI160::getSensorTime(SensorTime &sensorTime)
{
    uint8_t localData[3];
    int32_t rtnVal = readBlock(SENSORTIME_0, SENSORTIME_2, localData);
    
    if(rtnVal == RTN_NO_ERROR)
    {
        sensorTime.raw = ((localData[2] << 16) | (localData[1] << 8) | 
                           localData[0]);
        sensorTime.seconds = (sensorTime.raw * SENSOR_TIME_LSB);
    }
    
    return rtnVal;
}


//*****************************************************************************
int32_t BMI160::getTemperature(float *temp)
{
    uint8_t data[2];
    uint16_t rawTemp;
    
    int32_t rtnVal = readBlock(TEMPERATURE_0, TEMPERATURE_1, data);
    if(rtnVal == RTN_NO_ERROR)
    {
        rawTemp = ((data[1] << 8) | data[0]);
        if(rawTemp & 0x8000)
        {
            *temp = (23.0F - ((0x10000 - rawTemp)/512.0F));
        }
        else
        {
            *temp = ((rawTemp/512.0F) + 23.0F);
        }
    }
    
    return rtnVal;
}
