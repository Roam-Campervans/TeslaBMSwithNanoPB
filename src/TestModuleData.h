#pragma once


class TestModuleData
{
private:
    /* data */
public:
    TestModuleData();
    int numberOfTestModules;
    float rndmVolt;
    float rndmTemp;
    float setRndmVolt(float rndmVolt);
    float setRndmTemp(float rndmtemp);
    float rndmFloat(float min, float max);
    int testModuleGen();
};
