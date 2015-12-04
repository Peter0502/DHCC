#ifndef _ERR_H
#define _ERR_H

#define	EBALSETLOCKWAIT		-201
#define	EBALTUXERR			-202
#define	EBALREQGLOBAL		-203
#define	EBALTXBEGIN			-204
#define EBALTXCOMMIT		-205
#define EBALTXABORT			-206
#define	EBALCALLAUDIT		-207
#define	EBALCALLBACKRPLY	-208
#define EBALMACERROR		-209
#define EBALGETCLKEY		-210
#define EBALCALLCRYPT		-211
#define EBALTXOPEN  		-212
#define EBALTXCLOSE	 		-213

#define	ERequestWaitFail	-1000
#define ERequestNowaitFail	-1001

/********* For commom service *************/
#define EBalUnknownSvc		   -1
#define EBalGetGlobal		   -2
#define EBalGetDate            -3

#endif
