#include "bal.h"
#include "balcrypt.h"

/******************************************************************************\
 * int bal_decrypt(T_BTR* bal_req)
 * Input: 	bal_req, encrypted service request
 * Output: 	bal_req, plain service request 
 * Return: 	CSuccess, OK
 *          CFailure, ERROR
 * Note: Dependent on bal_req->head.macflg
 *       CRYPT_MAC: message MAC, MAC in bal_req->head.mac
 *       CRYPT_CBC: message CBC decryption
 *       CRYPT_ECB: message ECB decryption
 *       CRYPT_CFB: message CFB decryption
 *       CRYPT_OFB: message OFB decryption
\******************************************************************************/
int bal_decrypt(T_BTR* bal_req)
{
	unsigned char 	wrkkey1[8];
	unsigned char	wrkkey2[8];
	unsigned char	mackey1[8];
	unsigned char	mackey2[8];
	char			branch_no[32];
	char			tty[32];
	char			str_datalen[10];
	int				int_datalen;

	if( g_balinfo.reqversion == BAL3_VERSION )
	{
		return CSuccess;
	}

	return CSuccess;
}
