/******************************************************************************\
 * BAL CRYPTOGRAPHIC SERVER 		Author: Fu, Song  	Date: 1998-11-11
 * 
 * File: balkeycnv.c
 * 
 * Convert between ascii key and binary key.
\******************************************************************************/

#include <stdlib.h>

/******************************************************************************\
 * convert 16B ascii key to 8B binary key.
\******************************************************************************/
void balcrypt_cnv_binary(char asckey[], unsigned char binkey[])
{
	int i;
	char tmp[3];

	memset(binkey, 0, 8);
	memset(tmp, 0, 3);
	for( i = 0; i < 16; i+= 2)
	{
		memcpy(tmp, &asckey[i], 2 );
		binkey[i / 2]  = (unsigned char)strtol(tmp, NULL, 16);
	}
}

/******************************************************************************\
 * convert 8B binary key to 16B ascii key.
\******************************************************************************/
void balcrypt_cnv_ascii(unsigned char binkey[], char asckey[])
{
	int i;
	char tmp[16];

	memset(asckey, 0, 16);
	memset(tmp, 0, 3);
	for( i = 0; i < 8; i++)
	{
		sprintf(tmp, "%02X", binkey[i]);
		memcpy(&asckey[i * 2], tmp, 2);
	}
}
