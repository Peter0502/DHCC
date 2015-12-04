#include "bal.h"
#include "balcrypt.h"

void balmac_xormac(char* 			data, 
				   int 				len, 
				   unsigned char 	mackey1[],
				   unsigned char	mackey2[],
				   char*			mac)
{
	int i;

	memset(mac, 0, CSLMac);
	for( i = 0; i < len ; i++)
	{
		mac[ i & ( CSLMac - 1 )] ^= data[i];
	}

	chitty_encipher(mackey1, mackey2, mac, CSLMac);
}

void balmac_macxor(char*			data,
				   int				len,
				   unsigned char	mackey1[],
				   unsigned char	mackey2[],
				   char*			mac)
{
	int i, j;
	char tempmac[CSLMac];

	memset(mac, 0, CSLMac);

	for( i = 0; i < len; i+= CSLMac )
	{
		memset(tempmac, 0, CSLMac);
		memcpy(tempmac, &data[i], len - i > CSLMac ? CSLMac : len - i );
		chitty_encipher(mackey1, mackey2, tempmac, CSLMac);
		for( j = 0; j < CSLMac; j ++)
		{
			mac[j] ^= tempmac[j];
		}
	}
}
