/*************************************************************
* 文 件 名: spL888.c
* 功能描述：季末提呆账准备金
**************************************************************/
#define EXTERN
#include "public.h"
#include "gl_mst_c.h"
spL888()
{
	  int ret=0;
	  double HTOTAMT;
		char   HBR_NO[6];
		double HBADAMT;
		double HCAMT;
	  HTOTAMT=0.0;
	  HBADAMT=0.0;
	  HCAMT=0.0;
    memset(HBR_NO,0x00,sizeof(HBR_NO)); 
    ret=sql_begin(); /*打开事务*/
    /**------- 初始化全局变量 --------**/		
		pub_base_sysinit();
		ret=pub_eod_dzzbj();
		if( ret ) 
		{
			goto ErrExit;
		}		
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终昨日余额处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终昨日余额处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
                                                                  