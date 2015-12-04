#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_my_func.c
* 功能描述：1、在程序中自行控制打开函数。无需定义
*  			2、
*        	3、
*        	4、
*        	5、
* 作    者: dong     
* 完成日期：2003年08月16日
* 修改记录：    
* 1. 日    期:
*    修 改 人:
*    修改内容:
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
* 函数名：    pub_base_my_func
* 函数功能：  在程序中自行控制打开函数。无需定义
* 作者/时间：    
* 
* 参数：
*     输入：
*           
*     输出：
 *
*
* 返 回 值: >=0:成功 <0:失败
*           
* 修改历史：
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
		vtcp_log("子交易代码：txno[%s]未定义！",my_func);
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
       	vtcp_log("MY 打开错误！[%s->%d]",__FILE__,__LINE__);
       	goto Exit ;
   	}

   	myFun = (void(*)(int))dlsym(handle,tx_sub.tx_func);
   	if (myFun == NULL)
   	{
       	vtcp_log("MY dlerror = [%d] [%s] [%s->%d]",errno,dlerror(),__FILE__,__LINE__);
       	strcpy(reply,"S027");
       	vtcp_log("调用错误！");
       	dlerror();
       	goto Exit ;
   	}
   	else (*myFun)();

  	if(dlclose(handle))
   	{
       	strcpy(reply,"S027");
       	vtcp_log("MY 关闭错误！[%s->%d]",__FILE__,__LINE__);
       	goto Exit ;
   	}
	get_zd_data(DC_REPLY,reply);

	if( strncmp(reply,"000",3) ) goto Exit;
	if( !strncmp(reply,"000",3) ) /**响应码000X视同成功***/
	{
		strcpy( tmprsp,reply );
		if ( acct_ind[0]=='1' )
		{
			if ( pubf_acct_proc(tx_sub.sub_tx_code) )
			{
				vtcp_log("调用会计接口程序出错");
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
