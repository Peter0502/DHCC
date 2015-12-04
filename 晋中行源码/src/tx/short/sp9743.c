/*************************************************
* 文 件 名:    sp9743.c
* 功能描述：   
*              根据贷款账号取出账户序号
*
* 作    者:    lance
* 完成日期：   2003年03月22日
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
	
sp9743()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	
	
	struct ln_mst_c		ln_mst_tmp;
	
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0370" , ac_no );	/* 贷款账号 */

	
	/* 根据贷款账号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* 根据贷款账户ID查询贷款主文件个数 */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"根据贷款账户ID查询贷款主文件个数[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
 		set_zd_int( "0660", redflag );	/* 是否输入序号标志 */   
 		sprintf(acErrMsg,"贷款主文件表中无此记录[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L002");
 		goto ErrExit;
   	 }	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"取贷款主文件异常 erro code=[%s]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"取贷款主文件PASS");
	WRITEMSG 

	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	redflag=1;
	set_zd_long( "0350", ln_mst_tmp.ac_seqn );	/* 贷款账户序号 */
	
	set_zd_int( "0660", redflag );	/* 是否输入序号标志 1-唯一记录*/    	
    	goto OkExit;
    	}
    	
    	
  	redflag=9;
	set_zd_int( "0660", redflag );	/* 是否输入序号标志 9-多条记录*/ 	 		
		
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
   
