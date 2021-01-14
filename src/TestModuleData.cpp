#pragma once

#include "TestModuleData.h"
#include <iostream>
using namespace std;

// constructor
TestModuleData::TestModuleData(){
    numberOfTestModules = testModuleGen();
    rndmVolt = setRndmVolt(rndmVolt);
    rndmTemp = setRndmTemp(rndmTemp);
}

// returns a random float in range specified
float rndmFloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

// returns a random cell volt
float setRndmVolt(float rndmVolt)
{
     return rndmFloat(3.3, 4.2);
}

// returns a random temp
float setRndmTemp(float rndmtemp)
{
     return rndmFloat(0, 106);
}


// gets user input reguarding how many modules they would like to test.
int testModuleGen(TestModuleData testMod)
{
   string holder;
    while(testMod.numberOfTestModules <= 0 & testMod.numberOfTestModules >= 62)
    {
        cout << "please enter the number of test modules \n The number of modules can be no more than 62" ;
        cin >> holder;
        if(stoi(holder) > 0 & stoi(holder) < 63)
        {
            testMod.numberOfTestModules = stoi(holder);
            break;
        }
    }
}