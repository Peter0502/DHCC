/***
 **	 author        	 lig 
 **      Copyright       Union Tech. BeiJing
 **      Date            2007.03.30
 **      E-mail          lig@keyou.cn 
***/

//#include "HsmApi.h"
/*
��һ����Կ���ܵ�PINת��Ϊ��һ����Կ���ܵ�PIN
���������cHostIp,cHostPort,caPIKIn, caPIKOut,caPINBlockIn,caPANIn,caPANOut,
cHostIp:���ܻ�����ϵͳ��IP
cHostPort:���ܻ�����ϵͳ�Ķ˿�
caPIKIn:Դ��Կ��
caPIKOut��Ŀ����Կ��
caPINBlockIn��Դ��Կ���ܵ�PIN����(PIN��ʽΪANSI9��8)��
caPANIn��Դ�˺�/����,����13-19�ֽ��ַ���(ASCII), ��'\0'����;����Ϊʵ�ʿ���;�˺źͿ�����16��ASCII'0',��ʾ���˺����㡣
caPANOut��Ŀ���˺�/����,����13-19�ֽ��ַ���(ASCII), ��'\0'����;����Ϊʵ�ʿ���,��ʾ���Ųμ�����;�˺źͿ�����16��ASCII'0',��ʾ���˺����㡣
���������caPINBlockOut,
caPINBlockOut��Ŀ����Կ���ܵ�PIN����(PIN��ʽΪANSI9.8), ����16�ֽ��ַ���(ASCII), ��'\0'����
����ֵ��<0,�Ǵ����롣0 �ɹ���
*/

int HSMAPIConvertPINBlock(char *cHostIp,int cHostPort,char *caPIKIn,char *caPIKOut,char *caPINBlockIn,char *caPANIn,char *caPANOut,char *caPINBlockOut);


/*
���ܵ�PIN����
�������,���û���PIN���ļ��ܳ�PIN���ġ�
��������� cHostIp,cHostPort,pcPinBuf,pcPanBuf��pcZpkBuf
cHostIp:���ܻ�����ϵͳ��IP
cHostPort:���ܻ�����ϵͳ�Ķ˿�
pcPinBuf��PIN����,����6�ֽ��ַ���(ASCII), ��'\0'������
pcPanBuf���˺�/����,����13-19�ֽ��ַ���(ASCII), ��'\0'����;����Ϊʵ�ʿ���;�˺źͿ�����16��ASCII'0',��ʾ���˺����㡣
pcZpkBuf:�������ʱ�õ���Կ��
���������pcPbkBuf
pcPbkBuf���������ļ��ܵ�PIN����(PIN��ʽΪANSI9.8), ����16�ֽ��ַ���(ASCII), ��'\0'����
����ֵ��<0,�Ǵ����롣0 �ɹ���
*/

int UnionEncryptPin(char *cHostIp,int cHostPort,char *pcPinBuf,char *pcPanBuf,char *pcZpkBuf,pcPbkBuf);

/*
���ܵ�PIN����
�������,���û���PIN���Ľ��ܳ�PIN���ġ�
���������cHostIp,cHostPort,pcZpkBuf��pcPanBuf��pcPbkBuf��
cHostIp:���ܻ�����ϵͳ��IP
cHostPort:���ܻ�����ϵͳ�Ķ˿�
pcZpkBuf���������ʱ�õ���Կ��
pcPbkBuf���������ļ��ܵ�PIN����(PIN��ʽΪANSI9.8), ����16�ֽ��ַ���(ASCII), ��'\0'����
pcPanBuf���˺�/����,����13-19�ֽ��ַ���(ASCII), ��'\0'����;����Ϊʵ�ʿ���;�˺źͿ�����16��ASCII'0',��ʾ���˺����㡣
���������pcPinBuf 
pcPinBuf��PIN����,����4-6�ֽ��ַ���(ASCII), ��'\0'������
����ֵ��<0,�Ǵ����롣0 �ɹ���
*/
int UnionDecryptPin(char *cHostIp,int cHostPort,char *pcZpkBuf,char *pcPbkBuf,char *pcPanBuf,char *pcPinBuf);

