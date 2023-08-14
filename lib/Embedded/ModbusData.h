#ifndef MODBUS_DATA_H
#define MODBUS_DATA_H

#include <Arduino.h>
#include <ModbusClientRTU.h>

struct ModbusData
{
    ModbusMessage msg;
    uint32_t token;
};

#endif
