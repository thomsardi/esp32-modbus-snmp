#include <Arduino.h>

struct ReadSrneData {
    union BatteryCapacity {
        struct Data
        {
            uint16_t soc : 8;
            uint16_t reserved : 8;
        } data;
        uint16_t value;
    } batteryCapacity;
    uint16_t batteryVoltage;
    uint16_t chargingCurrent;
    union Temperature {
        struct Data
        {
            int16_t battery : 8;
            int16_t controller : 8;
        } data;
        int16_t value;
        
    } temperature;
    uint16_t loadVoltage;
    uint16_t loadCurrent;
    uint16_t loadPower;
    uint16_t solarVoltage;
    uint16_t solarCurrent;
    uint16_t solarPower;

    union LoadStatus {
        struct Data
        {
            uint16_t chargingState : 8;
            uint16_t loadBrightness : 7;
            uint16_t loadStatus : 1;
        } data;
        uint16_t value;
    } loadStatus;
};

struct WriteSrneData
{
    uint16_t boostChargingRecoveryVoltage;
    uint16_t overdischargeRecoveryVoltage;
    uint16_t undervoltageWarning;
    uint16_t overdischargeVoltage;
    uint16_t dischargingLimitVoltage;
    uint16_t unrealized;
    uint16_t overdischargeTimeDelay;
    uint16_t equalizingChargingTime;
    uint16_t boostChargingTime;
    uint16_t equalizingChargingInterval;
    uint16_t temperatureCompensation;
    uint16_t loadWorkingMode;

    uint16_t nominalBatteryCapacity;
    union ParameterVoltage
    {
        struct Data
        {
            uint16_t recognizedVoltage : 8;
            uint16_t systemVoltage : 8;
        } data;
        uint16_t value;
    } parameterVoltage;

    uint16_t batteryType;
    uint16_t overvoltageThreshold;
    uint16_t chargingVoltageLimit;
    uint16_t equalizingChargingVoltage;
    uint16_t boostChargingVoltage;
    uint16_t floatingChargeVoltage;
};


struct MPPTSrneData {
    ReadSrneData readSrneData;
    WriteSrneData writeSrneData;
};