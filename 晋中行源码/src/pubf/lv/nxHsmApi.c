/***
 **	 author          lig 
 **      Copyright       Union Tech. BeiJing
 **      Date            2007.03.30
 **      E-mail         lig@keyou.cn 
***/
#include <string.h>
#include "HsmApi.h"
/***
	fucntion :	HSMAPIConvertPINBlock()
	input	 :
	output	 :	
	description
		To translate a pin encryption under from one Zpk to another.
***/
int HSMAPIConvertPINBlock(char *cHostIp,int cHostPort,char *caPIKIn,char *caPIKOut,char *caPINBlockIn,char *caPANIn,char *caPANOut,char *caPINBlockOut)
{
	int sckid;
	int ret=0;
	char pcRstCde[3];
	memset(pcRstCde,0x00,sizeof(pcRstCde));
	if((cHostIp == NULL)||(cHostPort<=0)||(caPIKIn == NULL)||(caPIKOut == NULL)||(caPINBlockIn==NULL)||(caPANIn==NULL)||(caPANOut==NULL))
	{
		HsmErrLog("in HSMAPIConvertPINBlock:: parameter error!\n");
		return(-1);
	}
	sckid=ConnectToHsmSJL(cHostIp,cHostPort);
		if(sckid<0)
			return -2;
	ret=HSMAPIConvertPINBlock_(sckid,strlen(caPIKIn)/16,caPIKIn,1,
	strlen(caPIKOut)/16,caPIKOut,1,caPINBlockIn,
	caPANIn,caPANOut,caPINBlockOut,pcRstCde);
		if(ret<0)
			return (-1);
	CloseConnect(sckid);
   	 return (0);	
}
/******
	fucntion :	UnionEncryptPin()
	input	 :	
	output	 :	
	description
		EncryptPin
		
***/
int UnionEncryptPin(char *cHostIp,int cHostPort,char *pcPinBuf,char *pcPanBuf,char *pcZpkBuf,char *pcPbkBuf)
{
	int sckid=0;
	int ret=0;
	char pcRstCde[3]; 
	memset(pcRstCde,0x00,sizeof(pcRstCde));
	if ((cHostIp == NULL)||(cHostPort<=0)||(pcPinBuf ==NULL)||(pcPanBuf==NULL)||(pcZpkBuf==NULL))
	{
		HsmErrLog("in UnionEncryptPin:: parameter error!\n");
		return(-1);
	}
	
	sckid=ConnectToHsmSJL(cHostIp,cHostPort);
		if(sckid<0)
			return (-2);
	ret=UnionEncryptPin_(sckid,strlen(pcPinBuf),pcPinBuf,pcPanBuf,strlen(pcZpkBuf)/16,pcZpkBuf,
pcPbkBuf,pcRstCde);
			if(ret<0)
			return (-1);
	CloseConnect(sckid);
	return(0);
}

/***
	fucntion :	UnionDecryptPin()
	input	 :	
	output	 :	
	description
		UnionDecryptPin
***/
int UnionDecryptPin(char *cHostIp,int cHostPort,char *pcZpkBuf,char *pcPbkBuf,char *pcPanBuf,char *pcPinBuf)
{
	int sckid=0;
	int ret=0;
	char pcRstCde[3]; 
	memset(pcRstCde,0x00,sizeof(pcRstCde));
	if ((cHostIp == NULL)||(cHostPort<=0)||(pcZpkBuf ==NULL)||(pcPbkBuf==NULL)||(pcPanBuf==NULL))
	{
		HsmErrLog("in UnionDecryptPin:: parameter error!\n");
		return(-1);
	}
	sckid=ConnectToHsmSJL(cHostIp,cHostPort);
		if(sckid<0)
			return (-2);
	ret=UnionDecryptPin_(sckid,strlen(pcZpkBuf)/16,pcZpkBuf,pcPbkBuf,
	pcPanBuf,pcPinBuf,pcRstCde);
		if(ret<0)
			return (-1);
	CloseConnect(sckid);
	return(0);
}
