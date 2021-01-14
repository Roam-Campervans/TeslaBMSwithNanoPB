#include "config.h"
#include "BMSModule.h"
#include "BMSUtil.h"
#include "Logger.h"
#include "TestModuleData.h"

extern EEPROMSettings settings;

BMSModule::BMSModule()
{    //sets up the cells in the module
    for (int i = 0; i < 6; i++)
    {
        // hard code the cells let the module be calculated off of this
        cellVolt[i] = testMod.rndmVolt;
        lowestCellVolt[i] = 5.0f;
        highestCellVolt[i] = 0.0f;
        balanceState[i] = 0;
        
         
    }
    moduleVolt = testMod.rndmVolt * 6;
    temperatures[0] = 0.0f;
    temperatures[1] = 0.0f;
    lowestTemperature = 200.0f;
    highestTemperature = -100.0f;
    lowestModuleVolt = 200.0f;
    highestModuleVolt = 0.0f;
    exists = false;
    moduleAddress = 0;
    asleep = false;
    // goodPackets = 0;
    // badPackets = 0;
} 

float BMSModule::getCellVoltage(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return cellVolt[cell];
}

float BMSModule::getLowCellV()
{
    float lowVal = 10.0f;
    for (int i = 0; i < 6; i++) if (cellVolt[i] < lowVal) lowVal = cellVolt[i];
    return lowVal;
}

float BMSModule::getHighCellV()
{
    float hiVal = 0.0f;
    for (int i = 0; i < 6; i++) if (cellVolt[i] > hiVal) hiVal = cellVolt[i];
    return hiVal;
}

float BMSModule::getAverageV()
{
    float avgVal = 0.0f;
    for (int i = 0; i < 6; i++) avgVal += cellVolt[i];
    avgVal /= 6.0f;
    return avgVal;
}

float BMSModule::getHighestModuleVolt()
{
    return highestModuleVolt;
}

float BMSModule::getLowestModuleVolt()
{
    return lowestModuleVolt;
}

float BMSModule::getHighestCellVolt(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return highestCellVolt[cell];
}

float BMSModule::getLowestCellVolt(int cell)
{
    if (cell < 0 || cell > 5) return 0.0f;
    return lowestCellVolt[cell];
}

float BMSModule::getHighestTemp()
{
    return highestTemperature;
}

float BMSModule::getLowestTemp()
{
    return lowestTemperature;
}

float BMSModule::getLowTemp()
{
   return (temperatures[0] < temperatures[1]) ? temperatures[0] : temperatures[1]; 
}

float BMSModule::getHighTemp()
{
   return (temperatures[0] < temperatures[1]) ? temperatures[1] : temperatures[0];
}

float BMSModule::getAvgTemp()
{
    return (temperatures[0] + temperatures[1]) / 2.0f;
}

float BMSModule::getModuleVoltage()
{
    return moduleVolt;
}

float BMSModule::getTemperature(int temp)
{
    if (temp < 0 || temp > 1) return 0.0f;
    return temperatures[temp];
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

void BMSModule::balanceCells()
{
    uint8_t payload[4];
    uint8_t buff[30];
    uint8_t balance = 0;//bit 0 - 5 are to activate cell balancing 1-6

    payload[0] = moduleAddress << 1;
    payload[1] = REG_BAL_CTRL;
    payload[2] = 0; //writing zero to this register resets balance time and must be done before setting balance resistors again.
    BMSUtil::sendData(payload, 3, true);
    delay(2);
    BMSUtil::getReply(buff, 30);

    for (int i = 0; i < 6; i++)
    {
        if ( (balanceState[i] == 0) && (getCellVoltage(i) > settings.balanceVoltage) ) balanceState[i] = 1;

        if ( /*(balanceState[i] == 1) &&*/ (getCellVoltage(i) < (settings.balanceVoltage - settings.balanceHyst)) ) balanceState[i] = 0;

        if (balanceState[i] == 1) balance |= (1<<i);
    }

    if (balance != 0) //only send balance command when needed
    {
        payload[0] = moduleAddress << 1;
        payload[1] = REG_BAL_TIME;
        payload[2] = 0x82; //balance for two minutes if nobody says otherwise before then
        BMSUtil::sendData(payload, 3, true);
        delay(2);
        BMSUtil::getReply(buff, 30);

        payload[0] = moduleAddress << 1;
        payload[1] = REG_BAL_CTRL;
        payload[2] = balance; //write balance state to register
        BMSUtil::sendData(payload, 3, true);
        delay(2);
        BMSUtil::getReply(buff, 30);

        if (Logger::isDebug()) //read registers back out to check if everthing is good
        {
            Logger::debug("Reading back balancing registers:");
            delay(50);
            payload[0] = moduleAddress << 1;
            payload[1] = REG_BAL_TIME;
            payload[2] = 1; //expecting only 1 byte back
            BMSUtil::sendData(payload, 3, false);
            delay(2);
            BMSUtil::getReply(buff, 30);

            payload[0] = moduleAddress << 1;
            payload[1] = REG_BAL_CTRL;
            payload[2] = 1; //also only gets one byte
            BMSUtil::sendData(payload, 3, false);
            delay(2);
            BMSUtil::getReply(buff, 30);
        }
    }
}

uint8_t BMSModule::getBalancingState(int cell)
{
    if (cell < 0 || cell > 5) return 0;
    // cout << cell +" = "+ cellVolt;
    return balanceState[cell];
}