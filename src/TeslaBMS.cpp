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
void encoder(){
    // Setup pack message
    TeslaBMS_Pack mypack = TeslaBMS_Pack_init_zero;
    // Setup module message
    TeslaBMS_Pack_Module myModule = TeslaBMS_Pack_Module_init_zero;
    // Setup cell message
    // TeslaBMS_Pack_Module_Cell myCells = TeslaBMS_Pack_Module_Cell_init_zero;
    // stream to write buffer
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    
    /* TODO: Set a unique pack id sing Uuid and store in 
    maybe use the current ip address with some salt then hash*/

    // mypack.packName = userinput yet to be defined 
    mypack.averagePacktemp = bms.getAvgTemperature();
    mypack.currentVoltage = bms.getPackVoltage();
    printf("Pack Voltage: \n");
    printf(" %.3f \n", mypack.currentVoltage);
    printf("Average Temp: \n");
    printf(" %.3f \n", mypack.averagePacktemp);
    mypack.numberOfModules = bms.getNumOfModules();

    if (!(status = pb_encode(&stream, TeslaBMS_Pack_fields, &mypack))){
        // message_length = ostream.bytes_written;

        /* Then just check for any errors.. */
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    }        
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
    
    /* Print the data contained in the message. */
    printf("********MESSAGE FROM NANOPB!*********");
    // printf("Number Of Modules in Pack: ", myPack.numberOfModules);
    printf("Pack Voltage: ", (int)myPack.currentVoltage);
    printf("Average Temp: ", myPack.averagePacktemp);
    printf("********MESSAGE FROM NANOPB!*********");
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
    CAN_FRAME incoming;

    console.loop();

    if (millis() > (lastUpdate + 1000))
    {    
        lastUpdate = millis();
        bms.balanceCells();
        bms.getAllVoltTemp();

        {
        encoder();
        }

        {
        decode();
        }
    }

}