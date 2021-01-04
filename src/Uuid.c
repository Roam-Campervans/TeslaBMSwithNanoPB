// $Id: Uuid.c $
// $Date: 2012-09-09 07:47Z $
// $Revision: 1.0 $
// $Author: dai $

// *************************** COPYRIGHT NOTICE ******************************
// This code was originally written by David Ireland and is copyright
// (C) 2012 DI Management Services Pty Ltd <www.di-mgt.com.au>.
// Provided "as is". No warranties. Use at your own risk. You must make your
// own assessment of its accuracy and suitability for your own purposes.
// It is not to be altered or distributed, except as part of an application.
// You are free to use it in any application, provided this copyright notice
// is left unchanged.
// ************************ END OF COPYRIGHT NOTICE **************************

// This module uses functions from the CryptoSys (tm) PKI Toolkit available from
// <www.cryptosys.net/pki/>.
// Include a reference to `diCrSysPKINet.dll` in your project.

// REFERENCE:
// RFC 4122 "A Universally Unique IDentifier (UUID) URN Namespace", P. Leach et al,
// July 2005, <http://www.ietf.org/rfc/rfc4122.txt>.

#include <stdio.h>
#include <string.h>
// #include "diCrPKI.h"
// NB: Link with diCrPKI.lib

#define NBYTES 16

char *UUID_Make(char *uuid, int maxchars)
{
    //                                           12345678 9012 3456 7890 123456789012
    // Returns a 36-character string in the form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    // where "X" is an "upper-case" hexadecimal digit [0-9A-F].
    // Use the LCase function if you want lower-case letters.

    unsigned char abData[NBYTES];
    char strHex[2*NBYTES+1];

    // 1. Generate 16 random bytes = 128 bits
    RNG_Bytes(abData, NBYTES, "", 0);

    // 2. Adjust certain bits according to RFC 4122 section 4.4.
    // This just means do the following
    // (a) set the high nibble of the 7th byte equal to 4 and
    // (b) set the two most significant bits of the 9th byte to 10'B,
    //     so the high nibble will be one of {8,9,A,B}.
    abData[6] = 0x40 | (abData[6] & 0xf);
    abData[8] = 0x80 | (abData[8] & 0x3f);

    // 3. Convert the adjusted bytes to hex values
    CNV_HexStrFromBytes(strHex, sizeof(strHex)-1, abData, NBYTES);
    // DEBUGGING...
    //printf("ADJ=%s\n", strHex);
    //printf("                ^   ^\n"); // point to the nibbles we've changed

    // 4. Add four hyphen '-' characters
    memset(uuid, '\0', maxchars+1);
    strncpy(uuid, strHex, 8);
    strcat(uuid, "-");
    strncat(uuid, &strHex[8], 4);
    strcat(uuid, "-");
    strncat(uuid, &strHex[12], 4);
    strcat(uuid, "-");
    strncat(uuid, &strHex[16], 4);
    strcat(uuid, "-");
    strncat(uuid, &strHex[20], 12);

    // Return the UUID string
    return uuid;
}


int main(void)
{
    char uuid[37];
    int i;
    for (i = 0; i < 10; i++)
    {
        UUID_Make(uuid, sizeof(uuid)-1);
        printf("%s\n", uuid);
    }
    return 0;
}