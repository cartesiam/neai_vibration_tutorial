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


#ifndef BMI160_H
#define BMI160_H

#include "mbed.h"

/**
@brief The BMI160 is a small, low power, low noise 16-bit inertial measurement
unit designed for use in mobile applications like augmented reality or indoor
navigation which require highly accurate, real-time sensor data.

In full operation mode, with both the accelerometer and gyroscope enabled, the
current consumption is typically 950 μA, enabling always-on applications in
battery driven devices. It is available in a compact 14-pin 2.5 x 3.0 x 0.8 mm³
LGA package."

This class is an abstract base class and can not be instaniated, use BMI160_I2C 
or BMI160_SPI.
*/
class BMI160
{
public:

    ///Return value on success.
    static const uint8_t RTN_NO_ERROR = 0;
    
    ///Sensor types
    enum Sensors
    {
        MAG = 0, ///<Optional external sensor
        GYRO,    ///<Angular rate sensor
        ACC      ///<g sensor
    };
    
    ///Sensor Axis
    enum SensorAxis
    {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    };
    
    ///Structure for axis data
    struct AxisData
    {
        int16_t raw;  ///<Axis raw data
        float scaled; ///<Axis scaled data 
    };
    
    ///Structure for sensor time data
    struct SensorTime
    {
        uint32_t raw;  ///<raw SensorTime
        float seconds; ///<SensorTime as seconds
    };
    
    ///Structure for holding sensor data
    struct SensorData
    {
        AxisData xAxis; ///<Sensor X axis data
        AxisData yAxis; ///<Sensor Y axis data
        AxisData zAxis; ///<Sensor Z axis data
    };
    
    
    ///BMI160 registers
    enum Registers
    {
        CHIP_ID = 0x00,  ///<Chip Identification. 
        ERR_REG = 0x02,  ///<Reports sensor error flags.  Flags reset when read.
        PMU_STATUS,      ///<Reports current power mode for sensors.
        DATA_0,          ///<MAG_X axis bits7:0
        DATA_1,          ///<MAG_X axis bits15:8
        DATA_2,          ///<MAG_Y axis bits7:0
        DATA_3,          ///<MAG_Y axis bits15:8
        DATA_4,          ///<MAG_Z axis bits7:0
        DATA_5,          ///<MAG_Z axis bits15:8
        DATA_6,          ///<RHALL bits7:0
        DATA_7,          ///<RHALL bits15:8
        DATA_8,          ///<GYR_X axis bits7:0
        DATA_9,          ///<GYR_X axis bits15:8
        DATA_10,         ///<GYR_Y axis bits7:0
        DATA_11,         ///<GYR_Y axis bits15:8
        DATA_12,         ///<GYR_Z axis bits7:0
        DATA_13,         ///<GYR_Z axis bits15:8
        DATA_14,         ///<ACC_X axis bits7:0
        DATA_15,         ///<ACC_X axis bits15:8
        DATA_16,         ///<ACC_Y axis bits7:0
        DATA_17,         ///<ACC_Y axis bits15:8
        DATA_18,         ///<ACC_Z axis bits7:0
        DATA_19,         ///<ACC_Z axis bits15:8
        SENSORTIME_0,    ///<24bit counter synchronized with data, bits7:0
        SENSORTIME_1,    ///<24bit counter synchronized with data, bits15:8
        SENSORTIME_2,    ///<24bit counter synchronized with data, bits23:16
        STATUS,          ///<Reports sensors status flags
        INT_STATUS_0,    ///<Contains interrupt status flags
        INT_STATUS_1,    ///<Contains interrupt status flags
        INT_STATUS_2,    ///<Contains interrupt status flags
        INT_STATUS_3,    ///<Contains interrupt status flags
        TEMPERATURE_0,   ///<Contains temperature of sensor, bits7:0
        TEMPERATURE_1,   ///<Contains temperature of sensor, bits15:8
        FIFO_LENGTH_0,   ///<Current fill level of FIFO, bits7:0
        FIFO_LENGTH_1,   ///<Current fill level of FIFO, bits10:8
        FIFO_DATA,       ///<FIFO data read out register, burst read
        ACC_CONF = 0x40, ///<Set ODR, bandwidth, and read mode of accelerometer
        ACC_RANGE,       ///<Sets accelerometer g-range
        GYR_CONF,        ///<Set ODR, bandwidth, and read mode of gyroscope
        GYR_RANGE,       ///<Sets gyroscope angular rate measurement range
        MAG_CONF,        ///<Sets ODR of magnetometer interface
        FIFO_DOWNS,      ///<Sets down sampling ratios of accel and gyro data 
                         ///<for FIFO
        FIFO_CONFIG_0,   ///<Sets FIFO Watermark
        FIFO_CONFIG_1,   ///<Sets which sensor data is available in FIFO, 
                         ///<Header/Headerless mode, Ext Int tagging, Sensortime
        MAG_IF_0 = 0x4B, ///<Magnetometer 7-bit I2C address, bits7:1
        MAG_IF_1,        ///<Magnetometer interface configuration
        MAG_IF_2,        ///<Magnetometer address to read
        MAG_IF_3,        ///<Magnetometer address to write
        MAG_IF_4,        ///<Magnetometer data to write
        INT_EN_0,        ///<Interrupt enable bits
        INT_EN_1,        ///<Interrupt enable bits
        INT_EN_2,        ///<Interrupt enable bits
        INT_OUT_CTRL,    ///<Contains the behavioral configuration of INT pins
        INT_LATCH,       ///<Contains the interrupt rest bit and the interrupt 
                         ///<mode selection
        INT_MAP_0,       ///<Controls which interrupt signals are mapped to the 
                         ///<INT1 and INT2 pins
        INT_MAP_1,       ///<Controls which interrupt signals are mapped to the 
                         ///<INT1 and INT2 pins
        INT_MAP_2,       ///<Controls which interrupt signals are mapped to the 
                         ///<INT1 and INT2 pins
        INT_DATA_0,      ///<Contains the data source definition for the two 
                         ///<interrupt groups
        INT_DATA_1,      ///<Contains the data source definition for the two 
                         ///<interrupt groups
        INT_LOWHIGH_0,   ///<Contains the configuration for the low g interrupt
        INT_LOWHIGH_1,   ///<Contains the configuration for the low g interrupt
        INT_LOWHIGH_2,   ///<Contains the configuration for the low g interrupt
        INT_LOWHIGH_3,   ///<Contains the configuration for the low g interrupt
        INT_LOWHIGH_4,   ///<Contains the configuration for the low g interrupt
        INT_MOTION_0,    ///<Contains the configuration for the any motion and 
                         ///<no motion interrupts
        INT_MOTION_1,    ///<Contains the configuration for the any motion and 
                         ///<no motion interrupts
        INT_MOTION_2,    ///<Contains the configuration for the any motion and 
                         ///<no motion interrupts
        INT_MOTION_3,    ///<Contains the configuration for the any motion and 
                         ///<no motion interrupts
        INT_TAP_0,       ///<Contains the configuration for the tap interrupts
        INT_TAP_1,       ///<Contains the configuration for the tap interrupts
        INT_ORIENT_0,    ///<Contains the configuration for the oeientation 
                         ///<interrupt
        INT_ORIENT_1,    ///<Contains the configuration for the oeientation 
                         ///<interrupt
        INT_FLAT_0,      ///<Contains the configuration for the flat interrupt
        INT_FLAT_1,      ///<Contains the configuration for the flat interrupt
        FOC_CONF,        ///<Contains configuration for the fast offset 
                         ///<compensation for the accelerometer and gyroscope
        CONF,            ///<Configuration of sensor, nvm_prog_en bit
        IF_CONF,         ///<Contains settings for the digital interface
        PMU_TRIGGER,     ///<Sets trigger conditions to change gyro power modes
        SELF_TEST,       ///<Self test configuration
        NV_CONF = 0x70,  ///<Contains settings for the digital interface
        OFFSET_0,        ///<Contains offset comp values for acc_off_x7:0
        OFFSET_1,        ///<Contains offset comp values for acc_off_y7:0
        OFFSET_2,        ///<Contains offset comp values for acc_off_z7:0
        OFFSET_3,        ///<Contains offset comp values for gyr_off_x7:0
        OFFSET_4,        ///<Contains offset comp values for gyr_off_y7:0
        OFFSET_5,        ///<Contains offset comp values for gyr_off_z7:0
        OFFSET_6,        ///<gyr/acc offset enable bit and gyr_off_(zyx) bits9:8
        STEP_CNT_0,      ///<Step counter bits 15:8
        STEP_CNT_1,      ///<Step counter bits 7:0
        STEP_CONF_0,     ///<Contains configuration of the step detector
        STEP_CONF_1,     ///<Contains configuration of the step detector
        CMD = 0x7E       ///<Command register triggers operations like 
                         ///<softreset, NVM programming, etc.
    };
    
    
    ///@name ERR_REG(0x02)
    ///Error register data
    ///@{
        
    static const uint8_t FATAL_ERR_MASK = 0x01;
    static const uint8_t FATAL_ERR_POS = 0x00;
    static const uint8_t ERR_CODE_MASK = 0x1E;
    static const uint8_t ERR_CODE_POS = 0x01;
    static const uint8_t I2C_FAIL_ERR_MASK = 0x20;
    static const uint8_t I2C_FAIL_ERR_POS = 0x05;
    static const uint8_t DROP_CMD_ERR_MASK = 0x40;
    static const uint8_t DROP_CMD_ERR_POS = 0x06;
    static const uint8_t MAG_DRDY_ERR_MASK = 0x80;
    static const uint8_t MAG_DRDY_ERR_POS = 0x08;
    
    ///Enumerated error codes
    enum ErrorCodes
    {
        NO_ERROR = 0,        ///<No Error
        ERROR_1,             ///<Listed as error   
        ERROR_2,             ///<Listed as error
        LPM_INT_PFD,         ///<Low-power mode and interrupt uses pre-filtered 
                             ///<data
        ODR_MISMATCH = 0x06, ///<ODRs of enabled sensors in headerless mode do 
                             ///<not match
        PFD_USED_LPM         ///<Pre-filtered data are used in low power mode
    };
    ///@}
    
    
    ///@name ACC_CONF(0x40) and ACC_RANGE(0x41) 
    ///Data for configuring accelerometer
    ///@{
        
    static const uint8_t ACC_ODR_MASK = 0x0F;
    static const uint8_t ACC_ODR_POS = 0x00;
    static const uint8_t ACC_BWP_MASK = 0x70;
    static const uint8_t ACC_BWP_POS = 0x04;
    static const uint8_t ACC_US_MASK = 0x80;
    static const uint8_t ACC_US_POS = 0x07;
    static const uint8_t ACC_RANGE_MASK = 0x0F;
    static const uint8_t ACC_RANGE_POS = 0x00;
    
    ///Accelerometer output data rates
    enum AccOutputDataRate
    {
        ACC_ODR_1 = 1,  ///< 25/32Hz
        ACC_ODR_2,      ///< 25/16Hz
        ACC_ODR_3,      ///< 25/8Hz
        ACC_ODR_4,      ///< 25/4Hz
        ACC_ODR_5,      ///< 25/2Hz
        ACC_ODR_6,      ///< 25Hz
        ACC_ODR_7,      ///< 50Hz
        ACC_ODR_8,      ///< 100Hz
        ACC_ODR_9,      ///< 200Hz
        ACC_ODR_10,     ///< 400Hz
        ACC_ODR_11,     ///< 800Hz
        ACC_ODR_12      ///< 1600Hz
    };
    
    ///Accelerometer bandwidth parameters
    enum AccBandWidthParam
    {
        ACC_BWP_0 = 0, ///< Average 1 cycle;  when acc_us = 0 OSR4
        ACC_BWP_1,     ///< Average 2 cycles; when acc_us = 0 OSR2
        ACC_BWP_2,     ///< Average 4 cycles; when acc_us = 0 normal mode
        ACC_BWP_3,     ///< Average 8 cycles
        ACC_BWP_4,     ///< Average 16 cycles
        ACC_BWP_5,     ///< Average 32 cycles
        ACC_BWP_6,     ///< Average 64 cycles
        ACC_BWP_7      ///< Average 128 cycles
    };
    
    ///Accelerometer undersampling
    enum AccUnderSampling
    {
        ACC_US_OFF = 0,
        ACC_US_ON
    };
    
    ///Accelerometer ranges
    enum AccRange
    {
        SENS_2G = 0x03,  ///<Accelerometer range +-2G
        SENS_4G = 0x05,  ///<Accelerometer range +-4G
        SENS_8G = 0x08,  ///<Accelerometer range +-8G
        SENS_16G = 0x0C ///<Accelerometer range +-16G
    };
    
    ///Accelerometer configuration data structure
    struct AccConfig
    {
        AccRange range;        ///<Accelerometer range
        AccUnderSampling us;   ///<Accelerometr undersampling mode
        AccBandWidthParam bwp; ///<Accelerometer bandwidth param
        AccOutputDataRate odr; ///<Accelerometr output data rate
    };
    
    ///Accelerometer default configuration
    static const AccConfig DEFAULT_ACC_CONFIG;
    ///@}
    
    
    ///@name GYR_CONF(0x42) and GYR_RANGE(0x43) 
    ///Data for configuring gyroscope
    ///@{
    
    static const uint8_t GYRO_ODR_MASK = 0x0F;
    static const uint8_t GYRO_ODR_POS = 0x00;
    static const uint8_t GYRO_BWP_MASK = 0x30;
    static const uint8_t GYRO_BWP_POS = 0x04;
    static const uint8_t GYRO_RANGE_MASK = 0x07;
    static const uint8_t GYRO_RANGE_POS = 0x00;
    
    ///Gyroscope output data rates
    enum GyroOutputDataRate
    {
        GYRO_ODR_6 = 0x06,  ///<25Hz 
        GYRO_ODR_7 = 0x07,  ///<50Hz
        GYRO_ODR_8 = 0x08,  ///<100Hz
        GYRO_ODR_9 = 0x09,  ///<200Hz
        GYRO_ODR_10 = 0x0A, ///<400Hz
        GYRO_ODR_11 = 0x0B, ///<800Hz
        GYRO_ODR_12 = 0x0C, ///<1600Hz
        GYRO_ODR_13 = 0x0D  ///<3200Hz
    };
    
    ///Gyroscope bandwidth paramaters
    enum GyroBandWidthParam
    {
        GYRO_BWP_0 = 0, ///<OSR4 Over Sampling Rate of 4
        GYRO_BWP_1,     ///<OSR2 Over Sampling Rate of 2
        GYRO_BWP_2      ///<Normal Mode, Equidistant Sampling
    };
    
    ///Gyroscope ranges
    enum GyroRange
    {
        DPS_2000 = 0, ///<+-2000dps, 16.4LSB/dps
        DPS_1000,     ///<+-1000dps, 32.8LSB/dps
        DPS_500,      ///<+-500dps, 65.6LSB/dps
        DPS_250,      ///<+-250dps, 131.2LSB/dps
        DPS_125       ///<+-125dps, 262.4LSB/dps, 
    };
    
    ///Gyroscope configuration data structure    
    struct GyroConfig
    {
        GyroRange range;        ///<Gyroscope range
        GyroBandWidthParam bwp; ///<Gyroscope bandwidth param
        GyroOutputDataRate odr; ///<Gyroscope output data rate
    };
    
    ///Gyroscope default configuration
    static const GyroConfig DEFAULT_GYRO_CONFIG;
    ///@}
    
    
    ///Enumerated power modes
    enum PowerModes
    {
        SUSPEND = 0,  ///<Acc and Gyro, No sampling, No FIFO data readout
        NORMAL,       ///<Acc and Gyro, Full chip operation
        LOW_POWER,    ///<Acc duty-cycling between suspend and normal
        FAST_START_UP ///<Gyro start up delay time to normal mode <= 10 ms
    };
    
    
    ///Enumerated commands used with CMD register
    enum Commands
    {
        START_FOC = 0x03,        ///<Starts Fast Offset Calibrartion 
        ACC_SET_PMU_MODE = 0x10, ///<Sets acc power mode
        GYR_SET_PMU_MODE = 0x14, ///<Sets gyro power mode
        MAG_SET_PMU_MODE = 0x18, ///<Sets mag power mode
        PROG_NVM = 0xA0,         ///<Writes NVM backed registers into NVM
        FIFO_FLUSH = 0xB0,       ///<Clears FIFO
        INT_RESET,               ///<Clears interrupt engine, INT_STATUS, and 
                                 ///<the interrupt pin
        STEP_CNT_CLR,            ///<Triggers reset of the step counter
        SOFT_RESET = 0xB6        ///<Triggers a reset including a reboot.
    };
    
    
    ///@brief BMI160 Destructor.\n
    ///
    ///On Entry:
    ///@param[in] none 
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns none
    virtual ~BMI160(){ }
    
    
    ///@brief Reads a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read register on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readRegister(Registers reg, uint8_t *data) = 0;
    
    
    ///@brief Writes a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - data to write to register
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeRegister(Registers reg, const uint8_t data) = 0;
    
    
    ///@brief Reads a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are readable.  Function reads up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start reading from
    ///@param[in] stopReg - register to stop reading from
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read registers on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readBlock(Registers startReg, Registers stopReg, 
    uint8_t *data) = 0;
    
    
    ///@brief Writes a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are writeable.  Function writes up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start writing at 
    ///@param[in] stopReg - register to stop writing at
    ///@param[in] data - pointer to data to write to registers
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeBlock(Registers startReg, Registers stopReg, 
    const uint8_t *data) = 0;
    
    
    ///@brief Sets sensors power mode through CMD register.\n
    ///@details Observe command execution times given in datasheet.\n 
    ///
    ///On Entry:
    ///@param[in] sensor - Sensor which power mode we are setting
    ///@param[in] pwrMode - Desired powermode of the sensor
    ///
    ///On Exit:
    ///@param[out] 
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t setSensorPowerMode(Sensors sensor, PowerModes pwrMode);
    
    
    ///@brief Configure sensor.\n
    ///
    ///On Entry:
    ///@param[in] config - sSensor configuration data structure
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t setSensorConfig(const AccConfig &config);
    int32_t setSensorConfig(const GyroConfig &config);
    
    
    ///@brief Get sensor configuration.\n
    ///
    ///On Entry:
    ///@param[in] config - Sensor configuration data structure
    ///
    ///On Exit:
    ///@param[out] config - on success, holds sensor's current 
    ///configuration
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getSensorConfig(AccConfig &config);
    int32_t getSensorConfig(GyroConfig &config);
    
    
    ///@brief Get sensor axis.\n
    ///
    ///On Entry:
    ///@param[in] axis - Sensor axis
    ///@param[in] data - AxisData structure
    ///@param[in] range - Sensor range
    ///
    ///On Exit:
    ///@param[out] data - Structure holds raw and scaled axis data
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getSensorAxis(SensorAxis axis, AxisData &data, AccRange range);
    int32_t getSensorAxis(SensorAxis axis, AxisData &data, GyroRange range);
    
    
    ///@brief Get sensor xyz axis.\n
    ///
    ///On Entry:
    ///@param[in] data - SensorData structure
    ///@param[in] range - Sensor range
    ///
    ///On Exit:
    ///@param[out] data - Structure holds raw and scaled data for all three axis
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getSensorXYZ(SensorData &data, AccRange range);
    int32_t getSensorXYZ(SensorData &data, GyroRange range);
    
    
    ///@brief Get sensor xyz axis and sensor time.\n
    ///
    ///On Entry:
    ///@param[in] data - SensorData structure
    ///@param[in] sensorTime - SensorTime structure for data
    ///@param[in] range - Sensor range
    ///
    ///On Exit:
    ///@param[out] data - Structure holds raw and scaled data for all three axis
    ///@param[out] sensorTime - Holds sensor time on success
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getSensorXYZandSensorTime(SensorData &data, SensorTime &sensorTime, 
                                      AccRange range);
    int32_t getSensorXYZandSensorTime(SensorData &data, SensorTime &sensorTime, 
                                      GyroRange range);
    
    
    ///@brief Get Gyroscope/Accelerometer data and sensor time.\n
    ///
    ///On Entry:
    ///@param[in] accData -  Sensor data structure for accelerometer
    ///@param[in] gyroData - Sensor data structure for gyroscope
    ///@param[in] sensorTime - SensorTime data structure
    ///@param[in] accRange - Accelerometer range
    ///@param[in] gyroRange - Gyroscope range
    ///
    ///On Exit:
    ///@param[out] accData -  Synchronized accelerometer data
    ///@param[out] gyroData - Synchronized gyroscope data
    ///@param[out] sensorTime - Synchronized sensor time
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getGyroAccXYZandSensorTime(SensorData &accData, 
                                       SensorData &gyroData, 
                                       SensorTime &sensorTime, 
                                       AccRange accRange, GyroRange gyroRange);
    
    
    ///@brief Get sensor time.\n
    ///
    ///On Entry:
    ///@param[in] sensorTime - SensorTime structure for data
    ///
    ///On Exit:
    ///@param[out] sensorTime - Holds sensor time on success
    ///
    ///@returns returns 0 on success, non 0 on failure
    int32_t getSensorTime(SensorTime &sensorTime);
    
    
    ///@brief Get die temperature.\n
    ///
    ///On Entry:
    ///@param[in] temp - pointer to float for temperature 
    ///
    ///On Exit:
    ///@param[out] temp - on success, holds the die temperature
    ///
    ///@returns 0 on success, non 0 on failure
    int32_t getTemperature(float *temp);
};


/**
@brief BMI160_I2C - supports BMI160 object with I2C interface
*/
class BMI160_I2C: public BMI160
{
public:

    ///BMI160 default I2C address.
    static const uint8_t I2C_ADRS_SDO_LO = 0x68;
    ///BMI160 optional I2C address.
    static const uint8_t I2C_ADRS_SDO_HI = 0x69;
    

    ///@brief BMI160_I2C Constructor.\n
    ///
    ///On Entry:
    ///@param[in] i2cBus - reference to I2C bus for this device
    ///@param[in] i2cAdrs - 7-bit I2C address
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns none
    BMI160_I2C(I2C &i2cBus, uint8_t i2cAdrs);
    
    
    ///@brief Reads a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read register on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readRegister(Registers reg, uint8_t *data);
    
    
    ///@brief Writes a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - data to write to register
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeRegister(Registers reg, const uint8_t data);
    
    
    ///@brief Reads a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are readable.  Function reads up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start reading from
    ///@param[in] stopReg - register to stop reading from
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read registers on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readBlock(Registers startReg, Registers stopReg, 
    uint8_t *data);
    
    
    ///@brief Writes a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are writeable.  Function writes up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start writing at 
    ///@param[in] stopReg - register to stop writing at
    ///@param[in] data - pointer to data to write to registers
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeBlock(Registers startReg, Registers stopReg, 
    const uint8_t *data);
    
private:

    I2C &m_i2cBus;
    uint8_t m_Wadrs, m_Radrs;
};


/**
@brief BMI160_SPI - supports BMI160 object with SPI interface
*/
class BMI160_SPI: public BMI160
{
public:

    ///@brief BMI160_SPI Constructor.\n
    ///
    ///On Entry:
    ///@param[in] spiBus - reference to SPI bus for this device
    ///@param[in] cs - reference to DigitalOut used for chip select
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns none
    BMI160_SPI(SPI &spiBus, DigitalOut &cs);
    
    
    ///@brief Reads a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read register on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readRegister(Registers reg, uint8_t *data);
    
    
    ///@brief Writes a single register.\n
    ///
    ///On Entry:
    ///@param[in] data - data to write to register
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeRegister(Registers reg, const uint8_t data);
    
    
    ///@brief Reads a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are readable.  Function reads up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start reading from
    ///@param[in] stopReg - register to stop reading from
    ///@param[in] data - pointer to memory for storing read data
    ///
    ///On Exit:
    ///@param[out] data - holds contents of read registers on success
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t readBlock(Registers startReg, Registers stopReg, 
    uint8_t *data);
    
    
    ///@brief Writes a block of registers.\n
    ///@detail User must ensure that all registers between 'startReg' and 
    ///'stopReg' exist and are writeable.  Function writes up to, including, 
    ///'stopReg'.\n
    ///
    ///On Entry:
    ///@param[in] startReg - register to start writing at 
    ///@param[in] stopReg - register to stop writing at
    ///@param[in] data - pointer to data to write to registers
    ///
    ///On Exit:
    ///@param[out] none
    ///
    ///@returns 0 on success, non 0 on failure
    virtual int32_t writeBlock(Registers startReg, Registers stopReg, 
    const uint8_t *data);
    
private:

    SPI &m_spiBus;
    DigitalOut m_cs;
};

#endif /* BMI160_H */


///@brief fx documentation template.\n
///
///On Entry:
///@param[in] none 
///
///On Exit:
///@param[out] none
///
///@returns none
