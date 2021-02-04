#include "config.h"
#include "BMSModule.h"
#include "BMSUtil.h"
#include "Logger.h"
#include "TestModuleData.h"

extern TestModuleData modData;

BMSModule::BMSModule()
{    //sets up the cells in the module
    lowestCellVolt = 5.0f;
    highestCellVolt = 0.0f;
    moduleVolt = 0.0f;

    for (int i = 0; i < 6; i++)
    {
        // hard code the cells let the module be calculated off of this
        cellVolt[i] = modData.rndmVolt;
        if (cellVolt[i] < lowestCellVolt) lowestCellVolt = cellVolt[i];
        if (cellVolt[i] > highestCellVolt) highestCellVolt = cellVolt[i];
        moduleVolt += cellVolt[i];
        // lowestCellVolt[i] = 5.0f;
        // highestCellVolt[i] = 0.0f;
        // balanceState[i] = 0;
    }

    // temperatures[0] = 0.0f;
    // temperatures[1] = 0.0f;
    temperature = modData.rndmTemp();
    // lowestTemperature = 200.0f;
    // highestTemperature = -100.0f;
    // lowestModuleVolt = 200.0f;
    // highestModuleVolt = 0.0f;
    exists = false;
    moduleAddress = 0;
    asleep = false;
} 

float BMSModule::getCellVoltage(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return cellVolt[cell];
}

// float BMSModule::getLowCellV()
// {
//     float lowVal = 10.0f;
//     for (int i = 0; i < 6; i++) if (cellVolt[i] < lowVal) lowVal = cellVolt[i];
//     return lowVal;
// }

// float BMSModule::getHighCellV()
// {
//     float hiVal = 0.0f;
//     for (int i = 0; i < 6; i++) if (cellVolt[i] > hiVal) hiVal = cellVolt[i];
//     return hiVal;
// }

float BMSModule::getLowCellV()
{
    return lowestCellVolt;
}

float BMSModule::getHighCellV()
{
    return highestCellVolt;
}
float BMSModule::getAverageV()
{
    float avgVal = 0.0f;
    for (int i = 0; i < 6; i++) avgVal += cellVolt[i];
    avgVal /= 6.0f;
    return avgVal;
}

// float BMSModule::getHighestModuleVolt()
// {
//     return highestModuleVolt;
// }

// float BMSModule::getLowestModuleVolt()
// {
//     return lowestModuleVolt;
// }

// float BMSModule::getHighestCellVolt(int cell)
// {
//     if (cell < 0 || cell > 5) return 0.0f;
//     return highestCellVolt[cell];
// }

// float BMSModule::getLowestCellVolt(int cell)
// {
//     if (cell < 0 || cell > 5) return 0.0f;
//     return lowestCellVolt[cell];
// }

float BMSModule::getHighestCellVolt()
{
    return highestCellVolt;
}

float BMSModule::getLowestCellVolt()
{
    return lowestCellVolt;
}

// float BMSModule::getHighestTemp()
// {
//     return highestTemperature;
// }

// float BMSModule::getLowestTemp()
// {
//     return lowestTemperature;
// }

// float BMSModule::getLowTemp()
// {
//    return (temperatures[0] < temperatures[1]) ? temperatures[0] : temperatures[1]; 
// }

// float BMSModule::getHighTemp()
// {
//    return (temperatures[0] < temperatures[1]) ? temperatures[1] : temperatures[0];
// }

// float BMSModule::getAvgTemp()
// {
//     return (temperatures[0] + temperatures[1]) / 2.0f;
// }

float BMSModule::getModuleVoltage()
{
    return moduleVolt;
}

// float BMSModule::getTemperature(int temp)
// {
//     if (temp < 0 || temp > 1) return 0.0f;
//     return temperatures[temp];
// }

float BMSModule::getTemperature()
{
    return temperature;
}

void BMSModule::setAddress(int newAddr)
{
    if (newAddr < 0 || newAddr > MAX_MODULE_ADDR) return;
    moduleAddress = newAddr;
}

int BMSModule::getAddress()
{
    return moduleAddress;
}

bool BMSModule::isExisting()
{
    return exists;
}


void BMSModule::setExists(bool ex)
{
    exists = ex;
}


