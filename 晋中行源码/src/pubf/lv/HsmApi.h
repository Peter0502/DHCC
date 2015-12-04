/***
 **	 author          Flyger Zhuang
 **      Copyright       Union Tech. BeiJing
 **      Date            2003.12.02
***/
int  UnionCreatSocketClient(char *ipaddr,int port);
int  UnionCloseSocket(int sckid);
int  UnionSendToSocket(int sckid,unsigned char *buf,int len,long timeout);
int  UnionReceiveFromSocket(int sckid,char *buf,int len,long timeout);
int ConnectToHsmSJL(char *cHostIp,int cHostPort);
int CloseConnect(int iSocket);
/***
	fucntion :	HSMAPIConvertPINBlock_()
	input	 :	iKeyLenIn caPIKIn iFormatIn iKeyLenOut caPIKOut
			iFormatOut caPINBlockIn caPANIn caPANOut
	output	 :	caPINBlockOut
	description
		To translate a pin encryption under from one Zpk to another.
***/
int HSMAPIConvertPINBlock_(int sckid,int iKeyLenIn,char *caPIKIn,int iFormatIn,
	int iKeyLenOut,char *caPIKOut,int iFormatOut,char *caPINBlockIn,
	char *caPANIn,char *caPANOut,char *caPINBlockOut,char *pcRstCde);

/***
	fucntion :	UnionDecryptPin_()
	input	 :	
	output	 :	
	description
		UnionDecryptPin_
***/
int UnionDecryptPin_(int sckid,int iZpkLen,char *pcZpkBuf,char *pcPbkBuf,
	char *pcPanBuf,char *pcPinBuf,char *pcRstCde);
