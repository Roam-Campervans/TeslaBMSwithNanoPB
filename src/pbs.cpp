

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
//         mypack.numberOfModules = (int32_t) BMSModuleManager::getNumOfModules;
    
//         //encode
//         status = pb_encode(&stream, TeslaBMS_Pack_fields, &mypack);
        

//                 /* Then just check for any errors.. */
//          if (!status)
//         {
//             printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//         }
//     }

//      




// bool TeslaBMS_encode_numbers(pb_ostream_t *ostream, const pb_field_t *field, void * const *arg)
// {
// // pass in an array of modules

//     BMSModuleManager * source = (BMSModuleManager*)(*arg);
//     // encode all numbers
//     for (int i = 0; i < 6; i++) // all modules have 6 cells
//     {
//         if (!pb_encode_tag_for_field(ostream, field))
//         {
//             const char * error = PB_GET_ERROR(ostream);
//             printf("SimpleMessage_encode_numbers error: %s", error);
//             return false;
//         }
//         /* Encode a submessage field.
//         * You need to pass the pb_field_t array and pointer to struct, just like
//         * with pb_encode(). This internally encodes the submessage twice, first to
//         * calculate message size and then to actually write it out.
//         */

//         if (!pb_encode_submessage(ostream, source->(   i]))
//         {
//             const char * error = PB_GET_ERROR(ostream);
//             printf("SimpleMessage_encode_numbers error: %s", error);
//             return false;
//         }
//     }

//     return true;
// }