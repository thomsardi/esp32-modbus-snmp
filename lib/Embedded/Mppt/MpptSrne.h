#ifndef MPPTSRNE_H
#define MPPTSRNE_H

#include <Arduino.h>
#include <ModbusClientRTU.h>
#include "MpptSrneData.h"
#include <ModbusData.h>

class MpptSrne {
    public :
        MpptSrne(ModbusClientRTU *modbusClientRtu, uint8_t id);
        void changeId(uint8_t id);
        void scan();
        int parseData(const ModbusMessage &modbusMessage, int32_t token);
        void run();
        void print();
        int* getBatteryVoltageAddr();
    private :
        ModbusClientRTU *_modbusClientRtu;
        MPPTSrneData _mpptSrneData;
        int _batteryVoltage;
        uint8_t _id;
        int32_t _token[4] = {1000, 1001, 1002, 1003};
};

#endif