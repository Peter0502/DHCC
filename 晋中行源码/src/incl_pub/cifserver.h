#ifndef __CIFSERVER_H__
#define __CIFSERVER_H__
typedef struct{
	char cifip[32];	/**��ӡ��������ַ**/
	int  cifport;	/**��ӡ�������˿�**/
	int  localport;	/**���ض��غ�˿�**/
	char dbname[32];/**���ݿ���**/
	char dbuser[32];/**���ݿ��û�**/
	char dbpasswd[32];/**���ݿ��û�����**/	
}TCIFCONFIG;	
extern TCIFCONFIG *g_poCfg;
char* pcCifGetTime(char *pcTimeStr);
#endif
