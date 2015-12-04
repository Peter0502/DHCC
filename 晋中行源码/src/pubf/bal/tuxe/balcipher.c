/******************************************************************************\
 * BAL Cryptography Lib Author: Fu, Song Date: 1998-11-11
 * File: balcipher.c
 * Eric Young & Chitty Chen cipher interface
\******************************************************************************/

#include "des.h"

void chitty_encipher(unsigned char key1[], 
				   unsigned char key2[], 
				   char*         text,
				   int           len)
{
	char temp[8];
	long  i;

	for( i = 0; i <  len; i+= 8 )
	{
		EnMAC(&text[i], 8, key1, temp );
		DeMAC(temp, 8, key2, &text[i] );
		EnMAC(&text[i], 8, key1, temp );
		memcpy(&text[i], temp, 8);
	}
}

void chitty_decipher(unsigned char key1[], 
				     unsigned char key2[], 
				     char*         text,
				     int           len)
{
	char temp[8];
	long  i;

	for( i = 0; i <  len; i+= 8 )
	{
		DeMAC(&text[i], 8, key1, temp );
		EnMAC(temp, 8, key2, &text[i] );
		DeMAC(&text[i], 8, key1, temp );
		memcpy(&text[i], temp, 8);
	}
}

void balcipher_encrypt_cbc(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2, ivec;
    des_key_schedule 	sched1, sched2;
	char*		  tempdata;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

	tempdata = (char*)malloc(len);
	if( NULL == tempdata )
	{
		return;
	}

    des_cbc_encrypt((C_Block*)data, 
					(C_Block*)tempdata, 
					len >> 3 << 3,
					sched1, 
					(C_Block*)ivec,
					DES_ENCRYPT);
	memcpy(data, tempdata, len >> 3 << 3);
	des_cbc_encrypt((C_Block*)&data[len & 0x07 ], 
					(C_Block*)&tempdata[len & 0x07 ], 
					len >> 3 << 3,
					sched2, 
					(C_Block*)ivec,
					DES_DECRYPT);
	memcpy(&data[len & 0x07], &tempdata[len & 0x07], len >> 3 << 3);
    des_cbc_encrypt((C_Block*)data, 
					(C_Block*)tempdata, 
					len >> 3 << 3,
					sched1, 
					(C_Block*)ivec,
					DES_ENCRYPT);
	memcpy(data, tempdata, len >> 3 << 3);
	free(tempdata);
}

void balcipher_encrypt_ecb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2;
    des_key_schedule 	sched1, sched2;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

    for( i = 0; i < len >> 3 << 3; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched1, 
						DES_ENCRYPT);
    }
    for( i = len & 0x07; i < len; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched2, 
						DES_DECRYPT);
    }
    for( i = 0; i < len >> 3 << 3; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched1, 
						DES_ENCRYPT);
    }
}

void balcipher_encrypt_cfb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2, ivec;
    des_key_schedule 	sched1, sched2;
	char*				tempdata;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

	tempdata = (char*)malloc(len);
	if( NULL == tempdata )
	{
		return;
	}
	memcpy(tempdata, data, len);

    des_string_to_key((char*)strkey1, (C_Block*)ivec);
    des_cfb_encrypt((unsigned char*)data, 
					(unsigned char*)tempdata, 
					8,
					len,
					sched1, 
					(C_Block*)ivec,
					DES_ENCRYPT);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);
	des_cfb_encrypt((unsigned char*)tempdata,
					(unsigned char*)data,
					8,
					len,
					sched2, 
					(C_Block*)ivec,
					DES_DECRYPT);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);
    des_cfb_encrypt((unsigned char*)data, 
					(unsigned char*)tempdata, 
					8,
					len,
					sched1, 
					(C_Block*)ivec,
					DES_ENCRYPT);
	memcpy(data, tempdata, len);
}

void balcipher_encrypt_ofb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
}

void balcipher_decrypt_cbc(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2, ivec;
    des_key_schedule 	sched1, sched2;
	char*				tempdata;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

	tempdata = (char*)malloc(len);
	if( NULL == tempdata )
	{
		return;
	}
    des_cbc_encrypt((C_Block*)data, 
					(C_Block*)tempdata, 
					len >> 3 << 3,
					sched1, 
					(C_Block*)ivec,
					DES_DECRYPT);
	memcpy(data, tempdata, len >> 3 << 3);
	des_cbc_encrypt((C_Block*)&data[len & 0x07 ], 
					(C_Block*)&tempdata[len & 0x07 ], 
					len >> 3 << 3,
					sched2, 
					(C_Block*)ivec,
					DES_ENCRYPT);
	memcpy(&data[len & 0x07], &tempdata[len & 0x07], len >> 3 << 3);
    des_cbc_encrypt((C_Block*)data, 
					(C_Block*)tempdata, 
					len >> 3 << 3,
					sched1, 
					(C_Block*)ivec,
					DES_DECRYPT);
	memcpy(data, tempdata, len >> 3 << 3);

	free(tempdata);
}

void balcipher_decrypt_ecb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2;
    des_key_schedule 	sched1, sched2;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

    for( i = 0; i < len >> 3 << 3; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched1, 
						DES_DECRYPT);
    }
    for( i = len & 0x07; i < len; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched2, 
						DES_ENCRYPT);
    }
    for( i = 0; i < len >> 3 << 3; i += 8 )
    {
        des_ecb_encrypt((C_Block*)&data[i], 
						(C_Block*)&data[i], 
						sched1, 
						DES_DECRYPT);
    }

}

void balcipher_decrypt_cfb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
	unsigned char strkey1[9];
	unsigned char strkey2[9];
    int i;
    des_cblock 			cbkey1, cbkey2, ivec;
    des_key_schedule 	sched1, sched2;
	char*				tempdata;

	memset(strkey1, 0, 9);
	memset(strkey2, 0, 9);
	memcpy(strkey1, key1, 8);
	memcpy(strkey2, key2, 8);

    if( len < 8 )
		return;

    des_string_to_key((char*)strkey1, (C_Block*)cbkey1);
    des_string_to_key((char*)strkey2, (C_Block*)cbkey2);

    des_set_key((C_Block*)cbkey1, sched1);
    des_set_key((C_Block*)cbkey2, sched2);

	tempdata = (char*)malloc(len);
	if( NULL == tempdata )
	{
		return;
	}
	memcpy(tempdata, data, len);

    des_string_to_key((char*)strkey1, (C_Block*)ivec);
    des_cfb_encrypt((unsigned char*)data, 
					(unsigned char*)tempdata, 
					8,
					len,
					sched1, 
					(C_Block*)ivec,
					DES_DECRYPT);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);
	des_cfb_encrypt((unsigned char*)tempdata,
					(unsigned char*)data,
					8,
					len,
					sched2, 
					(C_Block*)ivec,
					DES_ENCRYPT);
    des_string_to_key((char*)strkey1, (C_Block*)ivec);
    des_cfb_encrypt((unsigned char*)data, 
					(unsigned char*)tempdata, 
					8,
					len,
					sched1, 
					(C_Block*)ivec,
					DES_DECRYPT);
	memcpy(data, tempdata, len);
}

void balcipher_decrypt_ofb(char* 			data, 
						   int 				len, 
						   unsigned char 	key1[],
						   unsigned char	key2[])
{
}
