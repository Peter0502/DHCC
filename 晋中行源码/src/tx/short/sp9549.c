/*****************************************************
* �� �� ��:  sp9549.c
* ���������� ����ĳЩԭ�ӽ��� 
* ��    ��:  SSH 
* ������ڣ� 2006��9��18�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include"errno.h"
#include <dlfcn.h>
#include "public.h"
static char caDlls[][20]={
	"rpt001",
	"rpt014",
	"rpt004",
	""
};
int sp9549()
{
	void *handle=NULL;
	int ret=0;
	char libsoname[256];
	int sIndex;
	void (* execFun)();
 	vtcp_log("sp9549��ʼ��[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	sIndex=0;
	while(caDlls[sIndex][0]!='\0'){
		sprintf(libsoname,"%s/lib/%s.so",getenv("HOME"),caDlls[sIndex]);
		handle = (void *)dlopen(libsoname, RTLD_LAZY|RTLD_GLOBAL);
		if ( handle == NULL )
		{
			sprintf( acErrMsg, "�򿪶�̬�����dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( caDlls[sIndex]);
			sprintf( acErrMsg, "[%s]�����˳�", caDlls[sIndex] );
			WRITEMSG
			return(-1);
		}

		execFun = (void(*)(int))dlsym( handle,caDlls[sIndex] );
		if ( execFun == NULL )
		{
			sprintf( acErrMsg, "����ִ�г������dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( caDlls[sIndex] );
			sprintf( acErrMsg, "[%s]�����˳�", caDlls[sIndex] );
			WRITEMSG

			return(-1);
		}
		else
		{
			sprintf(acErrMsg,"cExecProg[j]=[%s]",caDlls[sIndex]);
			WRITEMSG
			( *execFun )();		/*---ִ�г���---*/
		}

		vtcp_log("prog=[%s][%s][%d]",caDlls[sIndex],__FILE__,__LINE__);
		if ( dlclose(handle) )
		{
			sprintf( acErrMsg, "�رմ���dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* ��¼������־ */
			pub_base_strpack( caDlls[sIndex] );
			sprintf( acErrMsg, "%s�����˳�", caDlls[sIndex] );
			WRITEMSG

			return(-1);
		}

		vtcp_log("prog=[%s][%s][%d]",caDlls[sIndex],__FILE__,__LINE__);
		if (strcmp(g_pub_tx.reply,"0000"))
		{
			vtcp_log("g_pub_tx.reply=[%s][%s][%d]",g_pub_tx.reply,__FILE__,__LINE__);
			return (-1);
		}
		sIndex++;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

