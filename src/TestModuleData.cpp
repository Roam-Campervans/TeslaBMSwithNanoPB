
#include "TestModuleData.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// constructor
TestModuleData::TestModuleData(){
    numberOfTestModules = testModuleGen();
    rndmVolt = setRndmVolt(rndmVolt);
    rndmTemp = setRndmTemp(rndmTemp);
}

// returns a random float in range specified
float TestModuleData::rndmFloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

// returns a random cell volt
float TestModuleData::setRndmVolt(float rndmVolt)
{
     return rndmFloat(3.3, 4.2);
}

// returns a random temp
float TestModuleData::setRndmTemp(float rndmtemp)
{
     return rndmFloat(0, 24);
}


// gets user input reguarding how many modules they would like to test.
int TestModuleData::testModuleGen()
{
    int r = rand() % 5;
    return r;
}