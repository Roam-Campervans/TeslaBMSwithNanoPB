#if defined (__arm__) && defined (__SAM3X8E__)

#include <chip.h>
#endif
#include <Arduino.h>
#include "Logger.h"
#include "SerialConsole.h"
#include "BMSModuleManager.h"
#include "SystemIO.h"
// #include <due_can.h>
// #include <due_wire.h>
// #include <Wire_EEPROM.h>
//Nanopb dependacies
#include  "pb_encode.c"
#include "pb_decode.c"
#include "pb_common.c"
#include "TestModuleData.h"
#include "tesla_bms.pb.h"


TestModuleData modData;
BMSModuleManager bms;
SerialConsole console;
uint32_t lastUpdate;

// nanopb stuff
uint8_t buffer [128];
size_t message_length;
bool status;

/*Encodes NanoPB message
    Good recource for help on encoding repeated messages
    https://stackoverflow.com/questions/45979984/creating-callbacks-and-structs-for-repeated-field-in-a-protobuf-message-in-nanop 
    
    Good resource for help with nested messages
    https://github.com/nanopb/nanopb/issues/364
    uint8_t *buffer, size_t *message_length;
bool status;
*/



/* Module encode callback */
bool modules_encode(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
{    
    TeslaBMS_Pack * source =(TeslaBMS_Pack*)(*arg);
    TeslaBMS_Pack_Module mod =TeslaBMS_Pack_Module_init_default;

    for(int i=1; i <= source->numberOfModules; i++){
        mod.id = i;
        mod.highestCellVolt= bms.getModules(i).getHighestCellVolt();
        mod.lowestCellVolt= bms.getModules(i).getLowestCellVolt();
        mod.moduleTemp=bms.getModules(i).getTemperature();
        mod.moduleVoltage = bms.getModules(i).getModuleVoltage();
        printf(" \n Mod %i is at %f \n",(int)mod.id, mod.moduleVoltage);
        if (!pb_encode_tag_for_field(stream, field))
        {
            const char * error = PB_GET_ERROR(stream);
            printf("encode_modules error: %s", error);
            return false;
        }
    }

    return pb_encode_submessage(stream, &TeslaBMS_Pack_Module_msg, &mod);
}


/* Decode module callback*/
bool modules_decode(pb_istream_t *istream, const pb_field_t *field, void **arg){
    TeslaBMS_Pack_Module * dest = (TeslaBMS_Pack_Module*)(*arg);
    if(!pb_dec_submessage(istream, field)){
        const char * error = PB_GET_ERROR(istream);
        printf("module_decode error: %s", error);
        return false;
    }
    
    return true;
}





// makin mods
void module_list_add_mod(TeslaBMS_Pack_Module * list){
    TeslaBMS_Pack_Module moduleArray;

    for (int i = 1; i <= 3; i++) {
        TeslaBMS_Pack_Module myModule = TeslaBMS_Pack_Module_init_zero;

        BMSModule thisTestModule = bms.getModules(i);

        myModule.id = i;
        myModule.moduleVoltage = thisTestModule.getModuleVoltage();
        myModule.moduleTemp = thisTestModule.getTemperature();
        myModule.lowestCellVolt = thisTestModule.getLowestCellVolt();
        myModule.highestCellVolt = thisTestModule.getHighestCellVolt();

        // moduleArray[i - 1] = myModule;
    }

}


    // printf("Pack Voltage: \n");
    // printf(" %.3f \n", mypack.currentVoltage);
    // printf("Average Temp: \n");
    // printf(" %.3f \n", mypack.averagePacktemp);

void encoder(){
    // Setup pack message
    TeslaBMS_Pack mypack = TeslaBMS_Pack_init_zero;
    // stream to write buffer
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    // set deffinitions     
    mypack.averagePacktemp = bms.getAvgTemperature();
    mypack.currentVoltage = bms.getPackVoltage();
    mypack.numberOfModules = bms.getNumOfModules();

// ?????? I know I have to get the data and pass it in,
//  but not sure how to pass it from this array ???????
    BMSModule moduleArray[(bms.getNumOfModules())];
    for (int i = 0; i < bms.getNumOfModules(); i++)
    {
        moduleArray[i] = bms.getModules(i);
    }
    // set the arg to data needed
    mypack.modules.arg = &moduleArray;
    // encode the modules
    mypack.modules.funcs.encode = modules_encode;
    //encode the pack
    status = pb_encode(&stream, TeslaBMS_Pack_fields, &mypack);
    message_length = stream.bytes_written;
        
        if (!status) printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
}      




void decode(){
    /* Allocate space for the decoded message. */
    TeslaBMS_Pack myPack = TeslaBMS_Pack_init_zero;
    
    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    
    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, TeslaBMS_Pack_fields, &myPack);
    
    /* Check for errors... */
    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    }
    else
    {
        /* Print the data contained in the message. */
        printf("\n********MESSAGE FROM NANOPB!*********\n");
        // printf("Number Of Modules in Pack: ", myPack.numberOfModules);
        printf("Pack Voltage: %.3f\n", myPack.currentVoltage);
        printf("Average Temp: %.3f\n", myPack.averagePacktemp);
        printf("Number of modules: %i\n", (int)myPack.numberOfModules);
        myPack.modules.funcs.decode= modules_decode;
        printf("********MESSAGE FROM NANOPB!*********\n");
    }
    
    
}


void setup() 
{
    delay(4000);  //just for easy debugging. It takes a few seconds for USB to come up properly on most OS's
    SERIALCONSOLE.begin(115200);
    SERIALCONSOLE.println("Starting up!");
    SERIALCONSOLE.println("Started serial interface to BMS.");
    printf("There will be %i modules for this test",bms.getNumOfModules());
    
    bms.renumberBoardIDs();
    lastUpdate = 0;
    
    // bms.clearFaults();
}

void loop() 
{
    // CAN_FRAME incoming;

    console.loop();

    if (millis() > (lastUpdate + 1000))
    {    
    
        lastUpdate = millis();
        // bms.balanceCells();
        bms.getAllVoltTemp();

        {
        encoder();
        }
        {
        decode();
        }
    }

}