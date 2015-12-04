/******************************************************************************\
 * BAL Cryptography Server    Author: Fu, Song Date: 1998-11-11
 * File: cryperr.c
 * Error Handling
\******************************************************************************/

/*** C Header ***/
#include <stdio.h>

/*** App Header ***/
#include "balcrypt.h"

void balcrypt_error(T_CryptReq *crypt_req, int errtype, int errcode)
{

#ifdef  CRYPT_ERRLOG

	FILE* fp;
	char  str_branch_no[100];
	char  str_tty[100];

	fp = fopen(BALCRYPT_ERRORLOG, "a+");
	if( NULL == fp )
		return;
	setbuf(fp, NULL);

	memset(str_branch_no, 0, 100);
	memset(str_tty, 0, 100);

	if( NULL != crypt_req )
	{
		memcpy(str_branch_no, 
			   crypt_req->branch_no, 
			   sizeof(crypt_req->branch_no));
		memcpy(str_tty, 
			   crypt_req->tty, 
			   sizeof(crypt_req->tty));
	}

	switch(errtype)
	{
	case CRYPTERROR_DATABBASE:
		fprintf(fp, "ERROR:[%s][%s]: Database error, sqlcode[%d].\n",
				str_branch_no, str_tty, errcode);
		break;
	case CRYPTERROR_GETDOWNLOADKEY:
		fprintf(fp, "ERROR:[%s][%s]: Get download key failed\n",
				str_branch_no, str_tty);
		break;
	case CRYPTERROR_BEGINTIME:
		fprintf(fp, "ERROR:[%s][%s]: error begin time\n",
				str_branch_no, str_tty);
		break;
	default:
		fprintf(fp, "ERROR:[%s][%s]: Unkown type[%d], code[%d]\n",
				str_branch_no, str_tty, errtype, errcode);
		break;
	}
	fclose(fp);
#endif
}
