/*************************************************
* 文 件 名:  sp9622.c
* 功能描述： 查询长短款登记簿
*
* 作    者:  robbit
* 完成日期： 
*
* 修改记录： 
* 日   期:   20080416
* 修 改 人:  
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_rmny_smny_c.h"

int sp9622()
{
       int ret=0;
       int cdk_seqn =0;
	int cdk_date;
	struct mo_rmny_smny_c sMo;
	memset(&sMo,0,sizeof(sMo));
	pub_base_sysinit();
       get_zd_int( "0280",&cdk_seqn );
       get_zd_int("0440",&cdk_date);
       vtcp_log("%s,%d,长短款序号==[%d],登记日期[%d]",cdk_seqn,cdk_date);
       
       ret = Mo_rmny_smny_Sel( g_pub_tx.reply,&sMo," seqn=%d and occ_date=%d ",cdk_seqn,cdk_date );
	if( ret )
	{
		sprintf( acErrMsg,"长短款登记簿不存在该记录[%d]",cdk_seqn );
		WRITEMSG
		strcpy( g_pub_tx.reply,"CD30" );
		goto ErrExit;
	}
	set_zd_double("0420",sMo.amt);	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
