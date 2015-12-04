/*************************************************
* 文 件 名: pubf_ln_AcCheck.c
* 功能描述：   
*              pub_ln_AcCheck     根据活期显示账户检查相应状态及返回客户号
*
* 作    者:    rob
* 完成日期：   2004年01月11日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"

/**********************************************************************
* 函 数 名：  pub_ln_AcCheck
* 函数功能：  根据输入的活期显示账号查询其介质状态；
*             根据查询得出的账户ID和账户序号检查活期主文件，查看账户状态
* 作    者：  rob
* 完成时间：  2004年01月11日
*
* 参    数： 
*     输  入：ac_no      char(19)  显示账号    
*      	      ac_seqn   integer	  账户序号 
*     输  出: cif_no    long      客户号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/

int pub_ln_AcCheck( char *ac_no,int ac_seqn,long *cif_no,long *ac_id )
/*
int pub_ln_AcCheck( char *ac_no,int ac_seqn,long *cif_no )
*/
{	
	int ret=0;
	
	sprintf(acErrMsg,"进入pub_ln_AcCheck函数!");
	WRITEMSG	
	
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
					" ac_no='%s' " , ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质与账户关系信息中无此卡号或账号，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"读取介质与账户关系信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L015");
		goto ErrExit;
	}
	sprintf(acErrMsg,"根据输入的活期显示账号查询其介质状态 PASS!");
	WRITEMSG
		
	if( g_pub_tx.tx_date>g_mdm_ac_rel.end_date )
	{
		sprintf(acErrMsg,"该介质已经过期，请销户!");
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"P148");
		goto ErrExit;
		**/
	}
	sprintf(acErrMsg,"该介质已经过期，请销户 PASS!");
	WRITEMSG
		
	if ( strcmp( g_mdm_ac_rel.note_sts , "0" ))
	{
		sprintf(acErrMsg,"该介质非正常状态，请检查!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L016");
		goto ErrExit;
	}
	sprintf(acErrMsg,"该介质已经过期，请销户 PASS!");
	WRITEMSG
				
	/* 检查 活期基本户 的状态 */
	sprintf(acErrMsg,"该介质非正常状态，请检查[%ld][%d] PASS!",g_mdm_ac_rel.ac_id , ac_seqn);
	WRITEMSG
			
	ret = Dd_mst_Sel( g_pub_tx.reply , &g_dd_mst , 
					" ac_id=%ld and ac_seqn=%d " , 
					g_mdm_ac_rel.ac_id , ac_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"活期主文件中无此卡号或账号，请检查![%s]" , 
					g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L017");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取活期主文件信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
	sprintf(acErrMsg,"读取活期主文件 PASS!");
	WRITEMSG
		
	if ( strcmp( g_dd_mst.ac_sts , "1" ))
	{
		sprintf(acErrMsg,"该卡/帐号处于非正常状态，请检查!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L018");
		goto ErrExit;
	}	
	sprintf(acErrMsg,"该卡/帐号处于正常状态 PASS!");
	WRITEMSG

	sprintf(acErrMsg, "取出的客户号为[%ld] ", g_dd_mst.cif_no );
	WRITEMSG
		
	*cif_no = g_dd_mst.cif_no;
	TRACE	
	
	*ac_id = g_dd_mst.ac_id;
	TRACE
	sprintf(acErrMsg, "最后取出的客户号为[%ld][%ld] ", *cif_no,*ac_id );
	WRITEMSG
	TRACE
	sprintf(acErrMsg, "最后取出的客户号为[%ld]", *cif_no );	
	WRITEMSG
		
	return 0;
	
ErrExit:
	return 1; 
}
