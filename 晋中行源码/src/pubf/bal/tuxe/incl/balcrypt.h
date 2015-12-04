/******************************************************************************\
 * BAL Cryptography Server    Author: Fu, Song  Date: 1998-11-11
 * File: balcrypt.h
 *
 *
\******************************************************************************/

/******** BAL Cryptographic Error Log File******/
#define	BALCRYPT_ERRORLOG		"CRYPTERR.LOG"

/*********Request Key Type *****/
#define	CRYPTREQ_NOMAC					1
#define	CRYPTREQ_NOWRK					2
#define	CRYPTREQ_NOMACWRK				3

/******** BAL Cryptographic Error Type ******/
#define CRYPTERROR_DATABBASE			1001
#define	CRYPTERROR_GETDOWNLOADKEY		1002 
#define	CRYPTERROR_BEGINTIME			1003 

/******** Key Type in balcrypkey ***********/
#define	CRYPTKEY_DOWNLOAD				'5'

/******** User Return Codes Defnitions ******/
#define		BALCRYPT_UNKNOWN_REQ		-100
#define		BALCRYPT_ALLOCATE_ERROR		-101
#define		BALCRYPT_REQ_INVALID		-102
#define		BALCRYPT_KEYGEN_ERROR		-103
#define		BALCRYPT_SAVEKEY_ERROR		-104
#define		BALCRYPT_ENCRYPTKEY_ERROR	-105
#define		BALCRYPT_READKEY_ERROR		-106

/******** Bal cryptogrphy type *************/
#define 	CRYPT_DESMOD				0x07
#define		CRYPT_MACMOD				0x18

#define 	CRYPT_NONE					0x00

#define 	CRYPT_XORMAC				0x08
#define		CRYPT_MACXOR				0x10

#define		CRYPT_CBC					0x01
#define		CRYPT_ECB					0x02
#define		CRYPT_CFB					0x03
#define		CRYPT_OFB					0x04

/************ reqtype *******************/
#define		CRYPTREQ_ALLOCKEY			1
#define		CRYPTREQ_READCLKEY			2
#define		CRYPTREQ_GENDLKEY			3

/**** Cryption Request  *****/
typedef struct TAG_CRYPTREQ {
	char		reqtype;
	char 		branch_no[5];
	char 		tty[10];
	char		ip[8];
	char		pid[5];
	char		flags;
} T_CryptReq;

/**** Cryption Request  *****/
typedef struct TAG_CRYPTREQ_GENDLKEY {
	char		reqtype;
	char 		branch_no[5];
	char 		tty[10];
	char		ip[8];
	char		pid[5];
	char		flags;
} T_CryptGenDlkey;

/**** Cryption Response*****/
typedef struct TAG_CRYPTRPLY {
	char		reqtype;
	char		branch_no[5];
	char		tty[10];
	char		wrkkey[32];
	char		mackey[32];
	char		flags;
} T_CryptRply;
