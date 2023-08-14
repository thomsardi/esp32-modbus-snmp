#include "MpptSrne.h"

MpptSrne::MpptSrne(ModbusClientRTU *modbusClientRtu, uint8_t id)
{
    _modbusClientRtu = modbusClientRtu;
    _id = id;
}

void MpptSrne::print()
{
    Serial.println("Battery SoC : " + String(_mpptSrneData.readSrneData.batteryCapacity.data.soc));
    Serial.println("Battery Voltage : " + String(_mpptSrneData.readSrneData.batteryVoltage));
    Serial.println("Charging Current : " + String(_mpptSrneData.readSrneData.chargingCurrent));
    Serial.println("Controller Temperature : " + String(_mpptSrneData.readSrneData.temperature.data.controller));
    Serial.println("Battery Temperature : " + String(_mpptSrneData.readSrneData.temperature.data.battery));
    Serial.println("Load Voltage : " + String(_mpptSrneData.readSrneData.loadVoltage));
    Serial.println("Load Current : " + String(_mpptSrneData.readSrneData.loadCurrent));
    Serial.println("Load Power : " + String(_mpptSrneData.readSrneData.loadPower));
    Serial.println("Solar Voltage : " + String(_mpptSrneData.readSrneData.solarVoltage));
    Serial.println("Solar Current : " + String(_mpptSrneData.readSrneData.solarCurrent));
    Serial.println("Solar Power : " + String(_mpptSrneData.readSrneData.solarPower));
}

int* MpptSrne::getBatteryVoltageAddr()
{
    return &_batteryVoltage;
}

int MpptSrne::parseData(const ModbusMessage &modbusMessage, int32_t token)
{
    int status = 0;
    int index;
    if (modbusMessage.getServerID() == _id) {
        switch (token)
        {
        case 1000:
            index = modbusMessage.get(3, _mpptSrneData.readSrneData.batteryCapacity.value);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.batteryVoltage);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.chargingCurrent);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.temperature.value);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.loadVoltage);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.loadCurrent);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.loadPower);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.solarVoltage);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.solarCurrent);
            index = modbusMessage.get(index, _mpptSrneData.readSrneData.solarPower);
            _batteryVoltage = _mpptSrneData.readSrneData.batteryVoltage;
            status = 1;
            break;
        case 1001:
            index = modbusMessage.get(3, _mpptSrneData.writeSrneData.nominalBatteryCapacity);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.parameterVoltage.value);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.batteryType);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.overvoltageThreshold);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.chargingVoltageLimit);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.equalizingChargingVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.boostChargingVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.floatingChargeVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.boostChargingRecoveryVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.overdischargeRecoveryVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.undervoltageWarning);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.overdischargeVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.dischargingLimitVoltage);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.unrealized);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.overdischargeTimeDelay);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.equalizingChargingTime);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.boostChargingTime);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.equalizingChargingInterval);
            index = modbusMessage.get(index, _mpptSrneData.writeSrneData.temperatureCompensation);
            break;
        case 1002:
            index = modbusMessage.get(3, _mpptSrneData.writeSrneData.loadWorkingMode);
            break;
        case 1003:
            index = modbusMessage.get(3, _mpptSrneData.readSrneData.loadStatus.value);
            break;

        default:
            break;
        }
    }
    
    return status;
}