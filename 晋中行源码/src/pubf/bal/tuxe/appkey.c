/******************************************************************************\
 * BAL Cryptography Server    Author: Fu, Song Date: 1998-11-11
 * File: appkey.c
 * Application Key 
\******************************************************************************/

void balcrypt_get_appkey(unsigned char *appkey1, unsigned char *appkey2)
{
	appkey1[0] = '(';
	appkey1[1] = '.';
	appkey1[2] = '6';
	appkey1[3] = 'C';
	appkey1[4] = 'n';
	appkey1[5] = '0';
	appkey1[6] = 'n';
	appkey1[7] = '8';

	appkey2[0] = 'P';
	appkey2[1] = '>';
	appkey2[2] = '8';
	appkey2[3] = '5';
	appkey2[4] = 'a';
	appkey2[5] = '?';
	appkey2[6] = '-';
	appkey2[7] = '.';
}
