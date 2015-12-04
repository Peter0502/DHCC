 /***************************************************************************
 *** �������� : Han Xichao                                                ***
 *** ��    �� : 2010-06-08                                                ***
 *** ����ģ�� :                                                           ***
 *** �������� : lv_main.c                                                 ***
 *** �������� : С������֪ͨ��ϵͳ��������䶯֪ͨ������                  ***
 ***                                                                      ***
 *** ʹ��ע�� :                                                           ***
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
	
	/****����ĳ�ʼ������****/
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
		printf("ϵͳ��ʼ������!ret=[%d]\n",ret);
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
		printf("��[%s]����!dlerror = [%d] [%s]\n",libso,errno,dlerror());
		exit(1);
	}
	printf("handle=%d\n",handle);
		
	myFun = (void(*)(int))dlsym(handle,func_name);
	if (myFun == NULL) {
		printf("���ô���!dlerror = [%d] [%s]\n",errno,dlerror());
		dlerror();
    exit(1);
   }
	else (*myFun)();
  printf("���ú�������![%s][%d]\n",__FILE__,__LINE__);
		
	if(dlclose(handle))
	{
    printf("�رմ���!\n");
		exit(1);
  }
	else
	{
		printf("������ý���!\n");
	}
	
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{printf("С������֪ͨ��ϵͳ���н���!\n");}
	else
		{printf("�������֪ͨ��ϵͳ����ʧ��[%s]\n",g_pub_tx.reply);}
	
	exit(0);
}

