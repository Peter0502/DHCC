#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
/**
int UnionHSMCmd(int sckid,char *inbuf,int inlen,char *outbuf,long timeout);
**/
int HsmIstCK_(int sckid,int iSrcLen,int iSrcVar,char *pcSrcZpk,int iDstLen,int iDstVar,
	char *pcDstZpk,char *pcSrcFmt,char *pcDstFmt,char *pcMaxPin,
	char *pcSrcPan,char *pcDemPan,char *pcSrcPbk,char *pcDstPbk,
	char *pcRstCde);
int HsmIstBA_(int sckid,int iPinLen,char *pcPinBuf,char *pcPanBuf,char *pcOutBuf,
	char *pcRstCde);
int HsmIstJG_(int sckid,int iPinLen,int iZpkVar,int iZpkLen,char *pcZpkBuf,
	char *pcPinFmt,char *pcPanBuf,char *pcPinCls,char *pcPbkBuf,
	char *pcRstCde);
int HsmIstNG_(int sckid,int *iPinLen,char *pcPinBuf,char *pcPanBuf,char *pcOutBuf,
	char *pcRstCde);
int HsmIstJE_(int sckid,int iZpkVar,int iZpkLen,char *pcZpkBuf,char *pcPbkBuf,
		char *pcPinFmt,char *pcPanBuf,char *pcPinBuf,char *pcRstCde);
