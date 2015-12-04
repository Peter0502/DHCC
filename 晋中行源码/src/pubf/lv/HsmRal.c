/***
**     	Author     	Flyger Zhuang
**	Copyright	Union Tech. BeiJing
**	Date	   	2003.12.02
**	E-mail		flyger@163.net
**	Last Modified	lig
***/
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
int UnionHSMCmd(int sckid,char *inbuf,int inlen,char *outbuf,long timeout);
long lTmeOut = 4;

/***
	function :	HsmIstCK_()
	input	 :	iPinLen iZpkVar iZpkLen pcZpkBuf pcPinFmt pcPanBuf pcPinBuf
	output	 :	pcPbkBuf pcRstBuf			
	return	 :	== 1
	description
		Translate a Pin from Lmk to Zpk encryption
***/
int HsmIstCK_(int sckid,int iSrcLen,int iSrcVar,char *pcSrcZpk,int iDstLen,int iDstVar,
	char *pcDstZpk,char *pcSrcFmt,char *pcDstFmt,char *pcMaxPin,
	char *pcSrcPan,char *pcDemPan,char *pcSrcPbk,char *pcDstPbk,
	char *pcRstCde)
{
    int		iCmdLen = 0;
    int		iRstLen = 0;
    char	*p;
    char	pcCmdBuf[500];
    char	pcRstBuf[500];
    
    memset(pcCmdBuf,0,sizeof(pcCmdBuf));
    memset(pcRstBuf,0,sizeof(pcRstBuf));
    
    p = pcCmdBuf;
    memcpy(p,"CK",2);
    p += 2;
    
    switch (iSrcLen) {
    	case  16:
    	      break;
    	case  32:
    	      if (iSrcVar == 1)
    	         memcpy(p,"X",1);
    	      else 
    	         memcpy(p,"U",1);
    	      p += 1;
    	      break;
    	case  48:
    	      if (iSrcVar == 1)
    	         memcpy(p,"Y",1);
    	      else 
    	         memcpy(p,"T",1);
    	      p += 1;
    	      break;
    	default	:
    	      break;
    }
    memcpy(p,pcSrcZpk,iSrcLen);
    p += iSrcLen;
    
    switch(iDstLen) {
    	case  16:
    	      break;
    	case  32:
    	      if (iDstVar == 1)
    	         memcpy(p,"X",1);
    	      else 
    	         memcpy(p,"U",1);
    	      p += 1;
    	      break;
    	case  48:
    	      if (iDstVar == 1)
    	         memcpy(p,"Y",1);
    	      else 
    	         memcpy(p,"T",1);
    	      p += 1;
    	      break;
    	default	:
    	      break;
    }
    memcpy(p,pcDstZpk,iDstLen);
    p += iDstLen;
    
    memcpy(p,pcMaxPin,2);
    p += 2;
    memcpy(p,pcSrcPbk,16);
    p += 16;
    memcpy(p,pcSrcFmt,2);
    p += 2;
    memcpy(p,pcDstFmt,2);
    p += 2;
    memcpy(p,pcSrcPan,12);
    p += 12;
    memcpy(p,pcDemPan,12);
    p += 12;
    
    *p = 0;
    iCmdLen = p - pcCmdBuf;
    iRstLen=UnionHSMCmd(sckid,pcCmdBuf,iCmdLen,pcRstBuf,lTmeOut);
		if(iRstLen<0)
        {
                HsmErrLog("In HsmIstCK_ ::UnionHSMCmd wrong!");
                return (-2);
        }
    if (memcmp(pcRstBuf + 2,"00",2) != 0) {
    	memcpy(pcRstCde,pcRstBuf + 2,2);
    	return (-1);
    }

    memcpy(pcRstCde,"00",2);
    memcpy(pcDstPbk,pcRstBuf + 6,16);

    return (1);
}
/***
	function :	HsmIstBA_()
	input	 :	iPinLen pcPanBuf pcPinBuf
	output	 :	pcOutBuf pcRstBuf			
	return	 :	== 1
	description
		Encrypt a Pin using Lmk
***/
int HsmIstBA_(int sckid,int iPinLen,char *pcPinBuf,char *pcPanBuf,char *pcOutBuf,
	char *pcRstCde)
{
    int		iCmdLen = 0;
    int		iRstLen = 0;
    char	*p;
    char	pcCmdBuf[500];
    char	pcRstBuf[500];
    
    memset(pcCmdBuf,0,sizeof(pcCmdBuf));
    memset(pcRstBuf,0,sizeof(pcRstBuf));
    
    p = pcCmdBuf;
    memcpy(p,"BA",2);
    p += 2;
    
    memcpy(p,pcPinBuf,iPinLen);
    p += iPinLen;
    memcpy(p,"F",1);
    p += 1;
    
    memcpy(p,pcPanBuf,12);
    p += 12;
    
    *p = 0;
    iCmdLen = p - pcCmdBuf;
   iRstLen=UnionHSMCmd(sckid,pcCmdBuf,iCmdLen,pcRstBuf,lTmeOut);
	if(iRstLen<0)
        {
                HsmErrLog("In HsmIstBA_ ::UnionHSMCmd wrong!");
                return (-2);
        }
    if (memcmp(pcRstBuf + 2,"00",2) != 0) {
    	memcpy(pcRstCde,pcRstBuf + 2,2);
    	return (-1);
    }

    memcpy(pcRstCde,"00",2);
    memcpy(pcOutBuf,pcRstBuf + 4,iPinLen + 1);

    return (1);
}

/***
	function :	HsmIstJG()
	input	 :	iPinLen iZpkVar iZpkLen pcZpkBuf pcPinFmt pcPanBuf
			pcPinBuf
	output	 :	pcPbkBuf pcRstBuf			
	return	 :	== 1
	description
		Translate a Pin from Lmk to Zpk encryption
***/
int HsmIstJG_(int sckid,int iPinLen,int iZpkVar,int iZpkLen,char *pcZpkBuf,
	char *pcPinFmt,char *pcPanBuf,char *pcPinCls,char *pcPbkBuf,
	char *pcRstCde)
{
    int		iCmdLen = 0;
    int		iRstLen = 0;
    char	*p;
    char	pcCmdBuf[500];
    char	pcRstBuf[500];
    
    memset(pcCmdBuf,0,sizeof(pcCmdBuf));
    memset(pcRstBuf,0,sizeof(pcRstBuf));
    
    p = pcCmdBuf;
    memcpy(p,"JG",2);
    p += 2;
     
    switch (iZpkLen) {
	case 	16:
		break;
	case	32:
    		if (iZpkVar == 1) 
    	   	   memcpy(p,"X",1);
		else
		   memcpy(p,"U",1);
    	   	p += 1;
		break;
	case	48:
    		if (iZpkVar == 1) 
    	   	   memcpy(p,"Y",1);
		else
		   memcpy(p,"T",1);
    	   	p += 1;
		break;
	default	  :
		break;
    }
    memcpy(p,pcZpkBuf,iZpkLen);
    p += iZpkLen;
    memcpy(p,pcPinFmt,2);
    p += 2;
    
    memcpy(p,pcPanBuf,12);
    p += 12;
    memcpy(p,pcPinCls,iPinLen);
    p += iPinLen;
    
    *p = 0;
    iCmdLen = p - pcCmdBuf;
    iRstLen=UnionHSMCmd(sckid,pcCmdBuf,iCmdLen,pcRstBuf,lTmeOut);
	if(iRstLen<0)
        {
                HsmErrLog("In HsmIstJG_ ::UnionHSMCmd wrong!");
                return (-2);
        }
    if (memcmp(pcRstBuf + 2,"00",2) != 0) {
    	memcpy(pcRstCde,pcRstBuf + 2,2);
    	return (-1);
    }

    memcpy(pcRstCde,"00",2);
    memcpy(pcPbkBuf,pcRstBuf + 4,16);

    return (1);
}

/***
	function :	HsmIstNG_()
	input	 :	iPinLen pcPanBuf pcPinBuf
	output	 :	pcOutBuf pcRstBuf			
	return	 :	== 1
	description
		Decrypt a Pin using Lmk
***/
int HsmIstNG_(int sckid,int *iPinLen,char *pcPinBuf,char *pcPanBuf,char *pcOutBuf,
	char *pcRstCde)
{
    int		iCmdLen = 0;
    int		iRstLen = 0;
    char	*p;
    char	pcCmdBuf[500];
    char	pcRstBuf[500];
    
    memset(pcCmdBuf,0,sizeof(pcCmdBuf));
    memset(pcRstBuf,0,sizeof(pcRstBuf));
    
    p = pcCmdBuf;
    memcpy(p,"NG",2);
    p += 2;
    
    memcpy(p,pcPanBuf,12);
    p += 12;
    
    *iPinLen = strlen(pcPinBuf) - 1;
    memcpy(p,pcPinBuf,*iPinLen + 1);
    p += (*iPinLen + 1);
     
    *p = 0;
    iCmdLen = p - pcCmdBuf;
    iRstLen=UnionHSMCmd(sckid,pcCmdBuf,iCmdLen,pcRstBuf,lTmeOut);
	if(iRstLen<0)
        {
                HsmErrLog("In HsmIstNG_ ::UnionHSMCmd wrong!");
                return (-2);
        }
    if (memcmp(pcRstBuf + 2,"00",2) != 0) {
    	memcpy(pcRstCde,pcRstBuf + 2,2);
    	return (-1);
    }
     memcpy(pcRstCde,"00",2);
    memcpy(pcOutBuf,pcRstBuf + 4,*iPinLen);

    return (1);
}

/***
	function :	HsmIstJE()
	input	 :	iZpkVar iZpkLen pcZpkBuf pcPbkBuf pcPinFmt pcPanBuf
	output	 :	pcPinBuf pcRstBuf			
	return	 :	== 1
	description
		Translate a Pin from Zpk to Lmk encryption
***/
int HsmIstJE_(int sckid,int iZpkVar,int iZpkLen,char *pcZpkBuf,char *pcPbkBuf,
	char *pcPinFmt,char *pcPanBuf,char *pcPinBuf,char *pcRstCde)
{
    int		iCmdLen = 0;
    int		iRstLen = 0;
    char	*p;
    char	pcCmdBuf[500];
    char	pcRstBuf[500];
    
    memset(pcCmdBuf,0,sizeof(pcCmdBuf));
    memset(pcRstBuf,0,sizeof(pcRstBuf));
    
    p = pcCmdBuf;
    memcpy(p,"JE",2);
    p += 2;
     
    if (iZpkLen == 16) {
    	memcpy(p,pcZpkBuf,iZpkLen);
    	p += iZpkLen;
    }
    else if (iZpkLen == 32) {
    	if (iZpkVar == 1) {
    	   memcpy(p,"X",1);
    	   p += 1;
    	   memcpy(p,pcZpkBuf,iZpkLen);
    	   p += iZpkLen;
    	}
    	else {
    	   memcpy(p,"U",1);
    	   p += 1;
    	   memcpy(p,pcZpkBuf,iZpkLen);
    	   p += iZpkLen;
    	}
    }
    else if (iZpkLen == 48) {
    	if (iZpkVar == 1) {
    	   memcpy(p,"Y",1);
    	   p += 1;
    	   memcpy(p,pcZpkBuf,iZpkLen);
    	   p += iZpkLen;
    	}
    	else {
    	   memcpy(p,"Z",1);
    	   p += 1;
    	   memcpy(p,pcZpkBuf,iZpkLen);
    	   p += iZpkLen;
    	}
    }
    
    memcpy(p,pcPbkBuf,16);
    p += 16;
    
    memcpy(p,pcPinFmt,2);
    p += 2;
  
    memcpy(p,pcPanBuf,12);
    p += 12;
    
    *p = 0;
    iCmdLen = p - pcCmdBuf;
    iRstLen=UnionHSMCmd(sckid,pcCmdBuf,iCmdLen,pcRstBuf,lTmeOut);
	if(iRstLen<0)
        {
                HsmErrLog("In HsmIstJE_ ::UnionHSMCmd wrong!");
                return (-2);
        }

    if (memcmp(pcRstBuf + 2,"00",2) != 0) {
    	memcpy(pcRstCde,pcRstBuf + 2,2);
    	return (-1);
    }

    memcpy(pcRstCde,"00",2);
    strcpy(pcPinBuf,pcRstBuf + 4);

    return (1);
}
