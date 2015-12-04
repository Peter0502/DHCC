/***
 **	 author          Flyger Zhuang
 **      Copyright       Union Tech. BeiJing
 **      Date            2003.12.02
 **      E-mail          flyger@163.net
 **	Last Modified	 lig
***/

#include "HsmRal.h"
int ConnectToHsmSJL(char *cHostIp,int cHostPort)
{
  int sckid;
 if ((sckid = UnionCreatSocketClient(cHostIp,cHostPort)) < 0)
  {
	HsmErrLog("In ConnectToHsmSJL::HsmCreateSocketClient failed!");	
    return -2;
  }
  return sckid;
}
int CloseConnect(int iSocket)
{
        return (UnionCloseSocket(iSocket));
}
/***
	fucntion :	HSMAPIConvertPINBlock()
	input	 :	iKeyLenIn caPIKIn iFormatIn iKeyLenOut caPIKOut
			iFormatOut caPINBlockIn caPANIn caPANOut
	output	 :	caPINBlockOut
	description
		To translate a pin encryption under from one Zpk to another.
***/
int HSMAPIConvertPINBlock_(int sckid,int iKeyLenIn,char *caPIKIn,int iFormatIn,
	int iKeyLenOut,char *caPIKOut,int iFormatOut,char *caPINBlockIn,
	char *caPANIn,char *caPANOut,char *caPINBlockOut,char *pcRstCde)
{
    int		iPanLenIn = 0;
    int		iPanLenOut = 0;
    char	pcSrcFmt[3];
    char	pcDstFmt[3];
    char	pcPanBufIn[13];
    char	pcPanBufOut[13];
    
    
    memset(pcSrcFmt,0x00,sizeof(pcSrcFmt));
    memset(pcDstFmt,0x00,sizeof(pcDstFmt));
    memset(pcPanBufIn,0x00,sizeof(pcPanBufIn));
    memset(pcPanBufOut,0x00,sizeof(pcPanBufOut));
    
    sprintf(pcSrcFmt,"%02d",iFormatIn);
    sprintf(pcDstFmt,"%02d",iFormatOut);
    
    iPanLenIn = strlen(caPANIn);
    memcpy(pcPanBufIn,caPANIn + iPanLenIn - 13,12);
    iPanLenOut = strlen(caPANOut);
    memcpy(pcPanBufOut,caPANOut + iPanLenOut - 13,12);
    
    HsmIstCK_(sckid,iKeyLenIn * 16,1,caPIKIn,iKeyLenOut * 16,1,caPIKOut,pcSrcFmt,
	pcDstFmt,"12",pcPanBufIn,pcPanBufOut,caPINBlockIn,caPINBlockOut,
	pcRstCde);

    if (memcmp(pcRstCde,"00",2) != 0)
	{
		HsmErrLog("In HSMAPIConvertPINBlock_:: pcRstCde=[%s]\n",pcRstCde);
    	return (-1);
   	} 
    
    return (0);		
}
/******
	fucntion :	UnionEncryptPin()
	input	 :	
	output	 :	
	description
		EncryptPin
		
***/
int UnionEncryptPin_(int sckid,int iPinLen,char *pcPinBuf,char *pcPanBuf,int iZpkLen,char *pcZpkBuf,
char *pcPbkBuf,char *pcRstCde)
{
	char 	pcOutBuf[20];
	int	iPanLenIn=0;
	char	pcPanBufIn[13];
	memset(pcPanBufIn,0x00,sizeof(pcPanBufIn));
	memset(pcOutBuf,0x00,sizeof(pcOutBuf));
	
	iPanLenIn = strlen(pcPanBuf);
    	memcpy(pcPanBufIn,pcPanBuf + iPanLenIn - 13,12);
	HsmIstBA_(sckid,iPinLen,pcPinBuf,pcPanBufIn,pcOutBuf,pcRstCde);
	if (memcmp(pcRstCde,"00",2) != 0)
	{
                HsmErrLog("In UnionEncryptPin_BA:: pcRstCde=[%s]\n",pcRstCde);
      	 	 return (-1);
        }
	HsmIstJG_(sckid,iPinLen+1,1,iZpkLen*16,pcZpkBuf,"01",pcPanBufIn,pcOutBuf,pcPbkBuf,pcRstCde);
	if (memcmp(pcRstCde,"00",2) != 0)
	{
                HsmErrLog("In UnionEncryptPin_JG:: pcRstCde=[%s]\n",pcRstCde);
		 return (-1);
        }

	return(0);
}
/***
	fucntion :	UnionDecryptPin()
	input	 :	
	output	 :	
	description
		UnionDecryptPin
***/
int UnionDecryptPin_(int sckid,int iZpkLen,char *pcZpkBuf,char *pcPbkBuf,
	char *pcPanBuf,char *pcPinBuf,char *pcRstCde)
{	
	char 	pcOutBuf[20];
	int	iPanLenIn;
	char	pcPanBufIn[13];
	int	iPinLen=0;
	memset(pcPanBufIn,0x00,sizeof(pcPanBufIn));
	memset(pcOutBuf,0x00,sizeof(pcOutBuf));
	
	iPanLenIn = strlen(pcPanBuf);
    	memcpy(pcPanBufIn,pcPanBuf + iPanLenIn - 13,12);	
	HsmIstJE_(sckid,1,iZpkLen*16,pcZpkBuf,pcPbkBuf,"01",pcPanBufIn,pcOutBuf,pcRstCde);
	if (memcmp(pcRstCde,"00",2) != 0)
	{
                HsmErrLog("In UnionDecryptPin_JE:: pcRstCde=[%s]\n",pcRstCde);
		return (-1);
        }
		
	iPinLen=strlen(pcOutBuf);
	HsmIstNG_(sckid,&iPinLen,pcOutBuf,pcPanBufIn,pcPinBuf,pcRstCde);
	if (memcmp(pcRstCde,"00",2) != 0)
	{
                HsmErrLog("In UnionDecryptPin_NG:: pcRstCde=[%s]\n",pcRstCde);
		return (-1);
        }

		return (0);

}
