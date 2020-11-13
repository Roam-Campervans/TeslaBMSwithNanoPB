

// // ahrenswett  
// void pbs(){
//     uint8_t buffer [128];
//     size_t message_length;
//     bool status;

//     { /*Encode the message*/
        
//         /* Allocate space on the stack to store the message data.
//          *
//          * Nanopb generates simple struct definitions for all the messages.
//          * - check out the contents of simple.pb.h!
//          * It is a good idea to always initialize your structures
//          * so that you do not have garbage data from RAM in there.
//          */
//         TeslaBMS_Pack mypack = TeslaBMS_Pack_init_zero;
//         // stream to write buffer
//         pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        
//         // TODO: needs to be a hash of the user inputed name 
//         mypack.id = 1;
//         mypack.averagePacktemp = bms.getAvgTemperature();
//         mypack.currentVoltage = bms.getPackVoltage();
//         // mypack.numberOfModules = (int32_t) BMSModuleManager::getNumOfModules;
    
//         //encode
//         status = pb_encode(&stream, TeslaBMS_Pack_fields, &mypack);
        

//                 /* Then just check for any errors.. */
//          if (!status)
//         {
//             printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//         }
//     }

//      {
//         /* Allocate space for the decoded message. */
//         TeslaBMS_Pack myPack = TeslaBMS_Pack_init_zero;
        
//         /* Create a stream that reads from the buffer. */
//         pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
        
//         /* Now we are ready to decode the message. */
//         status = pb_decode(&stream, TeslaBMS_Pack_fields, &myPack);
        
//         /* Check for errors... */
//         if (!status)
//         {
//             printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        
//         }
        
//         /* Print the data contained in the message. */
//         printf("********MESSAGE FROM NANOPB!*********");
//         // printf("Number Of Modules in Pack: ", myPack.numberOfModules);
//         printf("Pack Voltage: ", (int)myPack.currentVoltage);
//         printf("Average Temp: ", myPack.averagePacktemp);
//         printf("********MESSAGE FROM NANOPB!*********");
//     }
// }