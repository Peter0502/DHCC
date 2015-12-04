/***
 **	 author        	 lig 
 **      Copyright       Union Tech. BeiJing
 **      Date            2007.03.30
 **      E-mail          lig@keyou.cn 
***/

//#include "HsmApi.h"
/*
将一个密钥加密的PIN转换为另一个密钥加密的PIN
输入参数：cHostIp,cHostPort,caPIKIn, caPIKOut,caPINBlockIn,caPANIn,caPANOut,
cHostIp:加密机服务系统的IP
cHostPort:加密机服务系统的端口
caPIKIn:源密钥。
caPIKOut：目的密钥。
caPINBlockIn：源密钥加密的PIN密文(PIN格式为ANSI9。8)。
caPANIn：源账号/卡号,长度13-19字节字符串(ASCII), 以'\0'结束;卡号为实际卡号;账号和卡号填16个ASCII'0',表示无账号运算。
caPANOut：目标账号/卡号,长度13-19字节字符串(ASCII), 以'\0'结束;卡号为实际卡号,表示卡号参加运算;账号和卡号填16个ASCII'0',表示无账号运算。
输出参数：caPINBlockOut,
caPINBlockOut：目的密钥加密的PIN密文(PIN格式为ANSI9.8), 长度16字节字符串(ASCII), 以'\0'结束
返回值：<0,是错误码。0 成功。
*/

int HSMAPIConvertPINBlock(char *cHostIp,int cHostPort,char *caPIKIn,char *caPIKOut,char *caPINBlockIn,char *caPANIn,char *caPANOut,char *caPINBlockOut);


/*
加密的PIN明文
这个函数,将用户的PIN明文加密成PIN密文。
输入参数： cHostIp,cHostPort,pcPinBuf,pcPanBuf，pcZpkBuf
cHostIp:加密机服务系统的IP
cHostPort:加密机服务系统的端口
pcPinBuf：PIN明文,长度6字节字符串(ASCII), 以'\0'结束。
pcPanBuf：账号/卡号,长度13-19字节字符串(ASCII), 以'\0'结束;卡号为实际卡号;账号和卡号填16个ASCII'0',表示无账号运算。
pcZpkBuf:输入解密时用的密钥。
输出参数：pcPbkBuf
pcPbkBuf：上送中心加密的PIN密文(PIN格式为ANSI9.8), 长度16字节字符串(ASCII), 以'\0'结束
返回值：<0,是错误码。0 成功。
*/

int UnionEncryptPin(char *cHostIp,int cHostPort,char *pcPinBuf,char *pcPanBuf,char *pcZpkBuf,pcPbkBuf);

/*
解密的PIN密文
这个函数,将用户的PIN密文解密成PIN明文。
输入参数：cHostIp,cHostPort,pcZpkBuf，pcPanBuf，pcPbkBuf，
cHostIp:加密机服务系统的IP
cHostPort:加密机服务系统的端口
pcZpkBuf：输入解密时用的密钥。
pcPbkBuf：接收中心加密的PIN密文(PIN格式为ANSI9.8), 长度16字节字符串(ASCII), 以'\0'结束
pcPanBuf：账号/卡号,长度13-19字节字符串(ASCII), 以'\0'结束;卡号为实际卡号;账号和卡号填16个ASCII'0',表示无账号运算。
输出参数：pcPinBuf 
pcPinBuf：PIN明文,长度4-6字节字符串(ASCII), 以'\0'结束。
返回值：<0,是错误码。0 成功。
*/
int UnionDecryptPin(char *cHostIp,int cHostPort,char *pcZpkBuf,char *pcPbkBuf,char *pcPanBuf,char *pcPinBuf);

