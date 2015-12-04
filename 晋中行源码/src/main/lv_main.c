 /***************************************************************************
 *** 程序作者 : Han Xichao                                                ***
 *** 日    期 : 2010-06-08                                                ***
 *** 所属模块 :                                                           ***
 *** 程序名称 : lv_main.c                                                 ***
 *** 程序作用 : 小额来账通知卡系统发送账务变动通知主函数                  ***
 ***                                                                      ***
 *** 使用注意 :                                                           ***
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include "public.h"
#include"sys/types.h"
#include"sys/select.h"
#include"sys/socket.h"
#include"sys/ioctl.h"
#include"sys/stat.h"
#include"netinet/in.h"
#include"netinet/tcp.h"
#include"arpa/inet.h"
#include"fcntl.h"
#include"unistd.h"
#include"netdb.h"
#include"sys/errno.h"
#include"hv_zf_c.h"
#include"lv_pkgreg_c.h"
#include"com_sys_parm_c.h"

void  (*func)() ;
void main(argc,argv)
int argc;
char *argv[];
{
	void * handle;
	void (* myFun)();
	char libso[50];
	char func[20];
	char func_name[20];
	int ret=0,reply=0;
	int flag=0;
	
	/****必须的初始化内容****/
	memset(libso,0x00,sizeof(libso));
	memset(func,0x00,sizeof(func));
	memset(func_name,0x00,sizeof(func_name));
	
	ret = db_open();
	if(ret){
		printf("[%s][%d] db_open error [%d]\n",__FILE__,__LINE__,ret);
		exit(1);
	}
	ret=sysinit();
	if(ret<0){
		printf("系统初始化错误!ret=[%d]\n",ret);
		exit(1);
	}
	
  memset(libso,0x00,sizeof(libso));
	memset(func,0x00,sizeof(func));
  strcpy(func,"lv_telmsg.so");
	sprintf( libso,"%s/lib/%s",getenv("HOME") ,func);
	printf("libso=[%s] len=[%d]\n",libso,strlen(libso));
	
  memset(func_name,0x00,sizeof(func_name));
  strcpy(func_name,"lv_telmsg");
	printf("libso is [%s],fun_name is [%s]\n",libso,func_name);

	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
	if(handle == NULL){
		printf("打开[%s]错误!dlerror = [%d] [%s]\n",libso,errno,dlerror());
		exit(1);
	}
	printf("handle=%d\n",handle);
		
	myFun = (void(*)(int))dlsym(handle,func_name);
	if (myFun == NULL) {
		printf("调用错误!dlerror = [%d] [%s]\n",errno,dlerror());
		dlerror();
    exit(1);
   }
	else (*myFun)();
  printf("调用函数结束![%s][%d]\n",__FILE__,__LINE__);
		
	if(dlclose(handle))
	{
    printf("关闭错误!\n");
		exit(1);
  }
	else
	{
		printf("程序调用结束!\n");
	}
	
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{printf("小额来账通知卡系统运行结束!\n");}
	else
		{printf("大额来账通知卡系统运行失败[%s]\n",g_pub_tx.reply);}
	
	exit(0);
}

