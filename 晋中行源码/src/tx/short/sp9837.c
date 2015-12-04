/*************************************************
* 文 件 名:    sp9837.c
* 功能描述：   
*              根据账号取出账户名称和帐户余额
*
* 作    者:    rob
* 完成日期：   2003年01月25日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9837()  
{ 		
	int ret=0;
	char ac_no[20],ac_type[2],tmp_name[50];
	double tmp_bal;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data("0310",ac_no);						/* 帐号 */
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"介质账号对照表中不存在此记录!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}
	else if( ret )
	{
		sprintf( acErrMsg,"查询介质帐号对照表异常!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}

	if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)g_mdm_ac_rel.ac_seqn=1;

	/* 根据显示帐号和账号序号查询主文件 */
	ret=pub_base_disac_file(ac_no, g_mdm_ac_rel.ac_seqn, ac_type);
	if( ret )
	{
		sprintf( acErrMsg,"根据显示帐号和账号序号查询主文件erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}

	switch(ac_type[0])
	{
		case '1':	tmp_bal=g_dd_mst.bal;
					strcpy(tmp_name,g_dd_mst.name);
					break;
		case '2':   tmp_bal=g_td_mst.bal;
					strcpy(tmp_name,g_td_mst.name);
					break;
		 default:	
					break;
	}

	/* 输出变量 */
	set_zd_data( "0250", tmp_name );				/* 户名 */
	set_zd_double( "0410", tmp_bal);				/* 余额	*/    	
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
