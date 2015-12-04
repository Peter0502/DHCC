#include "bal.h"
#include "balcrypt.h"

/******************************************************************************\
 * int bal_encrypt(T_BTR* bal_rply)
 * Input: 	bal_rply, plain service reply
 * Output: 	bal_rply, encrypted service reply
 * Return: 	CSuccess, OK
 *          CFailure, ERROR
 * Note: Dependent on bal_rply->head.macflg
 *       CRYPT_MAC: message MAC, MAC in bal_rply->head.mac
 *       CRYPT_CBC: message CBC encryption
 *       CRYPT_ECB: message ECB encryption
 *       CRYPT_CFB: message CFB encryption
 *       CRYPT_OFB: message OFB encryption
\******************************************************************************/
int bal_encrypt(T_BTR* bal_rply)
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

	if( bal_rply->head.macflg == CRYPT_NONE )
	{
		return CSuccess;
	}
	return CSuccess;

}
