/***************************************************************
* 文 件 名: spD107.c
* 功能描述：开户确认交易
* 作    者: jack
* 完成日期：2004年1月18日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "dd_mst_c.h"
spD107()
{
	int ret=0;
	char	tmp_no[20],tmp_ind[2];
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_hst_c	g_dd_mst_hst;
	struct td_mst_hst_c	g_td_mst_hst;
	struct dd_mst_c g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0310",g_pub_tx.ac_no);					/*取帐号*/
	get_zd_data("0300",tmp_no);							/*取开户核准号*/
	get_zd_data("0680",tmp_ind);						/*取可提现标志*/

	/*根据显示帐号到介质帐号表找出账户id和账户序号 */
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*根据账户id和账户序号到产品帐号对照表找出产品类型*/
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						 g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/*根据账户id和账户序号到活期存款主文件找出帐户状态
	g_mdm_ac_rel.ac_id  g_mdm_ac_rel.ac_seqn
	martin update 2006.9.14  g_dd_mst_c.ac_sts[0] =='0'
	*/
	ret = Dd_mst_Sel1( g_pub_tx.reply , &g_dd_mst , "ac_id=%ld and ac_seqn=%ld" , g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
	if( ret==100 )
	{
		sprintf(acErrMsg,"活期存款主文件中不存在此帐户信息");
		WRITEMSG
		strcpy(g_pub_tx.reply,"B114");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_prdt_ac_id.prdt_no[0]=='1'&&g_dd_mst.ac_sts[0] =='0')  				/* 1-活期 && 0-开户待确认*/
	{
		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_prdt_ac_id.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"该产品类型不存在");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W024" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		
		ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"定义更改主文件游标出错[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D101" );
			goto ErrExit;
		}

		ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"活期主文件中未找到该记录[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		/*专用户、临时户修改可提现标志*/
		if(g_dd_mst.ac_type[0]=='3'||g_dd_mst.ac_type[0]=='4')
		{
			strcpy(g_dd_mst.cash_ind,tmp_ind);
		}

		if(g_dd_mst.ac_sts[0]=='0') 
		{
			g_dd_mst.ac_sts[0]='1';
		}

		ret = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"UPDATE dd_mst error %d",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D105" );
			goto ErrExit;
		}

		Dd_mst_Clo_Upd( );

		ret = sql_execute("update mo_opn_cls set check_no='%s' where ac_no='%s'", tmp_no,g_pub_tx.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"UPDATE mo_opn_cls error %d",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D106" );
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"该账户不需要开户确认!!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "O096" );
		goto ErrExit;
	}

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开户确认成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开户确认失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
