
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

/*Encodes NanoPB message
    Good recource for help on encoding repeated messages
    https://stackoverflow.com/questions/45979984/creating-callbacks-and-structs-for-repeated-field-in-a-protobuf-message-in-nanop 
    
    Good resource for help with nested messages
    https://github.com/nanopb/nanopb/issues/364
    uint8_t *buffer, size_t *message_length;
bool status;
*/
typedef struct{
    TeslaBMS_Pack_Module modarr[MAX_MODULE_ADDR];
    int listSize = 0;
}
ModuleList;

// add module to list
void modulelist_add_module(ModuleList * modList, TeslaBMS_Pack_Module module) {
    if (modList->listSize < MAX_MODULE_ADDR) {
        modList->modarr[modList->listSize] = module;
        modList->listSize++;
    }
}

void module_array_maker(ModuleList *list){
    for (int i = 1; i <= bms.getNumOfModules(); i++) {
        TeslaBMS_Pack_Module myModule = TeslaBMS_Pack_Module_init_zero;

        BMSModule thisTestModule = bms.getModules(i);

        myModule.id = i;
        myModule.moduleVoltage = thisTestModule.getModuleVoltage();
        myModule.moduleTemp = thisTestModule.getTemperature();
        myModule.lowestCellVolt = thisTestModule.getLowestCellVolt();
        myModule.highestCellVolt = thisTestModule.getHighestCellVolt();

        list->modarr[i-1] = myModule;
    }
}


/* Module encode callback */
bool modules_encode(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg) {
    ModuleList *source = (ModuleList*)(*arg);

    for(int i=0; i<bms.getNumOfModules();i++)
    {
        printf("\nEncoding TeslaPack_Module[%i].moduleVoltage %f \n",(int)source->modarr[i].id, source->modarr[i].moduleVoltage);
        if (!pb_encode_tag_for_field(stream, field))
        {
            const char * error = PB_GET_ERROR(stream);
            printf("encode_modules error: %s", error);
            return false;
        }
        
        bool status = pb_encode_submessage(stream, TeslaBMS_Pack_Module_fields, &source->modarr[i]);
        if (status) {
            printf("\n...%i bytes written so far...\n", stream->bytes_written);
        }
        else
        {
            const char * error = PB_GET_ERROR(stream);
            printf("SimpleMessage_encode_numbers error: %s", error);
            return false;
        }
    }
    return true;
}

/* Decode module callback*/
bool modules_decode(pb_istream_t *istream, const pb_field_t *field, void **arg) {
    ModuleList * dest = (ModuleList*)(*arg);

    TeslaBMS_Pack_Module module;
    bool status = pb_decode(istream, TeslaBMS_Pack_Module_fields, &module);
    if(!status){
        const char * error = PB_GET_ERROR(istream);
        printf("module_decode error: %s\n", error);
        return false;
    }

    printf("module decoded\n");
    modulelist_add_module(dest, module);

    return true;
}

size_t encoder(pb_byte_t *buffer, size_t length) {
    // Setup pack message
    TeslaBMS_Pack mypack = TeslaBMS_Pack_init_zero;
    // stream to write buffer
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, length);
    // set definitions     
    mypack.averagePacktemp = bms.getAvgTemperature();
    mypack.currentVoltage = bms.getPackVoltage();
    mypack.numberOfModules = bms.getNumOfModules();

    ModuleList modArr;
    module_array_maker(&modArr);
    mypack.modules.arg = &modArr;
    mypack.modules.funcs.encode = modules_encode;
    
    //encode the pack
    bool status = pb_encode(&stream, TeslaBMS_Pack_fields, &mypack);
    if (!status) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    }
    printf("Encoded %d bytes\n", stream.bytes_written);
    return stream.bytes_written;
}

void decode(pb_byte_t *buffer, size_t message_length){
    /* Allocate space for the decoded message. */
    TeslaBMS_Pack myPack = TeslaBMS_Pack_init_zero;
        
    ModuleList modArr;
    module_array_maker(&modArr);
    myPack.modules.arg = &modArr;
    myPack.modules.funcs.decode = modules_decode;
    
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    bool status = pb_decode(&stream, TeslaBMS_Pack_fields, &myPack);
    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return;
    }
    /* Print the data contained in the message. */
    printf("\n********MESSAGE FROM NANOPB!*********\n");
    // printf("Number Of Modules in Pack: ", myPack.numberOfModules);
    printf("Pack Voltage: %.3f\n", myPack.currentVoltage);
    printf("Average Temp: %.3f\n", myPack.averagePacktemp);
    printf("Number of modules: %i\n", (int)myPack.numberOfModules);
    for (size_t i = 0; i < myPack.numberOfModules ; i++)
    {
        printf("\n    ************  Module %i  ************\n", modArr.modarr[i].id);
        printf("       Voltage: %.3f Temperature: %.3f \n" ,modArr.modarr[i].moduleVoltage, modArr.modarr[i].moduleTemp);
    }        
    printf("********MESSAGE FROM NANOPB!*********\n");
}

void setup() 
{
    delay(4000);  //just for easy debugging. It takes a few seconds for USB to come up properly on most OS's
    SERIALCONSOLE.begin(115200);
    SERIALCONSOLE.println("Starting up!");
    SERIALCONSOLE.println("Started serial interface to BMS.");
    
    bms.renumberBoardIDs();
    lastUpdate = 0;
    
    // bms.clearFaults();
}

void loop() 
{
    console.loop();

    if (millis() > (lastUpdate + 2000))
    {    
        lastUpdate = millis();
        // bms.balanceCells();
        bms.getAllVoltTemp();

        pb_byte_t buffer[128];
        size_t msg_length = encoder(buffer, sizeof(buffer));
        decode(buffer, msg_length);
    }
}