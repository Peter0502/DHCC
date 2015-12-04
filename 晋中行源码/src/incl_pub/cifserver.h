#ifndef __CIFSERVER_H__
#define __CIFSERVER_H__
typedef struct{
	char cifip[32];	/**验印服务器地址**/
	int  cifport;	/**验印服务器端口**/
	int  localport;	/**本地端守候端口**/
	char dbname[32];/**数据库名**/
	char dbuser[32];/**数据库用户**/
	char dbpasswd[32];/**数据库用户密码**/	
}TCIFCONFIG;	
extern TCIFCONFIG *g_poCfg;
char* pcCifGetTime(char *pcTimeStr);
#endif
