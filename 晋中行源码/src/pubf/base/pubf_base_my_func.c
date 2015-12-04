#include "find_debug.h" 
/*************************************************
* �� �� ��: pubf_base_my_func.c
* ����������1���ڳ��������п��ƴ򿪺��������趨��
*  			2��
*        	3��
*        	4��
*        	5��
* ��    ��: dong     
* ������ڣ�2003��08��16��
* �޸ļ�¼��    
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*************************************************/

#include "stdio.h"
#include <string.h>
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <dlfcn.h>
#define EXTERN
#include "public.h"
#include "svrpub_c.h"

#include "tx_sub_def_c.h"
/*
#define loc_t sys_loc_t
*/


/**********************************************************************
* ��������    pub_base_my_func
* �������ܣ�  �ڳ��������п��ƴ򿪺��������趨��
* ����/ʱ�䣺    
* 
* ������
*     ���룺
*           
*     �����
 *
*
* �� �� ֵ: >=0:�ɹ� <0:ʧ��
*           
* �޸���ʷ��
*
********************************************************************/
int pub_base_my_func(char my_func[10],char acct_ind[2],char reply[5])
{
	char libso[50];
	char tmprsp[5];
	void * handle;
	void (* myFun)();
	struct tx_sub_def_c tx_sub;

	vtcp_log("code[%s]",my_func);TRACE
	RespCode=Tx_sub_def_Sel(RtCode,&tx_sub,"sub_tx_code='%s'",my_func);
	if ( RespCode )
	{
		strcpy ( reply , "S026" );
		vtcp_log("�ӽ��״��룺txno[%s]δ���壡",my_func);
       	goto Exit ;
	}

	set_zd_data(DC_SUB_TX_CODE,my_func);
   	sprintf( libso,"%s/lib/",getenv("HOME") );
   	ldchar( tx_sub.tx_func,strlen(tx_sub.tx_func),tx_sub.tx_func );
   	pub_base_strpack ( libso );
   	strcat ( libso , tx_sub.tx_func);
    libso[strlen(libso)]=0;
    ldchar(libso,strlen(libso),libso);
    strcat(libso,".so");
    vtcp_log("MY libso-name1:[%s] [%s] [%s->%d]",libso,tx_sub.tx_func,__FILE__,__LINE__);

   	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
   	if(handle == NULL)
   	{
       	vtcp_log("dlerror = [%d] [%s]",errno,dlerror());
       	strcpy(reply,"S027");
       	vtcp_log("MY �򿪴���[%s->%d]",__FILE__,__LINE__);
       	goto Exit ;
   	}

   	myFun = (void(*)(int))dlsym(handle,tx_sub.tx_func);
   	if (myFun == NULL)
   	{
       	vtcp_log("MY dlerror = [%d] [%s] [%s->%d]",errno,dlerror(),__FILE__,__LINE__);
       	strcpy(reply,"S027");
       	vtcp_log("���ô���");
       	dlerror();
       	goto Exit ;
   	}
   	else (*myFun)();

  	if(dlclose(handle))
   	{
       	strcpy(reply,"S027");
       	vtcp_log("MY �رմ���[%s->%d]",__FILE__,__LINE__);
       	goto Exit ;
   	}
	get_zd_data(DC_REPLY,reply);

	if( strncmp(reply,"000",3) ) goto Exit;
	if( !strncmp(reply,"000",3) ) /**��Ӧ��000X��ͬ�ɹ�***/
	{
		strcpy( tmprsp,reply );
		if ( acct_ind[0]=='1' )
		{
			if ( pubf_acct_proc(tx_sub.sub_tx_code) )
			{
				vtcp_log("���û�ƽӿڳ������");
           		set_zd_data ( DC_REPLY , RtCode );
				goto Exit;
			}
		}
		strcpy( reply,tmprsp );
	}
	else
		goto Exit;

	return 0;

Exit:

	return 1;
}
