/*************************************************
* 文 件 名:  spL317.c
* 功能描述： 贷款罚息启用/止用
*
* 作    者:  lance
* 完成日期： 2003年08月19日
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "mo_pun_acc_c.h"

int spL317()
{
	char oprt_flag[2];
	double rate=0.00;
	struct mo_pun_acc_c sMoPunAcc;
	struct mdm_ac_rel_c sMdmAcRel;

	memset( &sMoPunAcc, 0x00, sizeof( struct mo_pun_acc_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );

	pub_base_sysinit();

	get_zd_data( "0700", oprt_flag );
	get_zd_data( "0370", sMdmAcRel.ac_no );
	get_zd_long( "0340", &sMdmAcRel.ac_seqn );


	g_reply_int = pub_ln_lndis_file(sMdmAcRel.ac_no,sMdmAcRel.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( g_reply_int ) 
	{
		goto ErrExit;
	}		
	pub_ln_GetRate ( g_ln_mst , g_ln_parm , &rate );	/*当前利率值*/
	memcpy( &sMdmAcRel, &g_mdm_ac_rel, sizeof(struct mdm_ac_rel_c) ); 
	
	/*****
	g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, \
		"ac_no='%s' and ac_seqn='%d'", sMdmAcRel.ac_no, sMdmAcRel.ac_seqn );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询介质账号对照表出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}
	g_reply_int = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "ac_id=%d and ac_seqn=%d", \
		sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "查询贷款参数表出错![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	**********/
	if( oprt_flag[0] == '0' ) /*启用*/
	{
		/*检查*/
		g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, "ac_id=%d and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if ( g_reply_int )
		{
			sprintf( acErrMsg, "查询贷款主文件出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		g_reply_int = pub_base_CompDblVal( g_ln_mst.fine_rate, 0.00 );
		if ( g_reply_int <= 0 )
		{
			strcpy(g_pub_tx.reply,"L229");
			sprintf(acErrMsg,"该账户尚未定义罚息利率!");
			WRITEMSG
			goto ErrExit;
		}	

		g_reply_int = sql_count("mo_pun_acc", "sts=0 and ac_id='%d' and ac_seqn='%d'", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "统计贷款罚息登记簿记录个数出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
		else if( g_reply_int > 0 )
		{
			strcpy( g_pub_tx.reply, "L227" );
			sprintf( acErrMsg, "该账户已启用罚息!ac_id[%d]ac_seqn[%d]", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			WRITEMSG
			goto ErrExit;			

		}

		/*登记*/
		sMoPunAcc.ac_id = sMdmAcRel.ac_id;
		sMoPunAcc.ac_seqn = sMdmAcRel.ac_seqn;
		sMoPunAcc.sts = 0;    /*罚息状态：0-启用 >0-止用*/
		sMoPunAcc.beg_date = g_pub_tx.tx_date;
		strcpy( sMoPunAcc.beg_tel, g_pub_tx.tel );
		strcpy( sMoPunAcc.beg_br_no, g_pub_tx.tx_br_no );
		get_zd_data( "0260", sMoPunAcc.brf );

		vtcp_log("\n%d\n%d\n%d\n%d\n%s\n%s\n",
		sMoPunAcc.ac_id,
		sMoPunAcc.ac_seqn,
		sMoPunAcc.sts,
		sMoPunAcc.beg_date,
		sMoPunAcc.beg_tel,
		sMoPunAcc.beg_br_no);

		g_reply_int = Mo_pun_acc_Ins( sMoPunAcc, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "登记贷款罚息登记簿出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
	}
	else if( oprt_flag[0] == '1' ) /*止用*/
	{
		/*检查*/
		g_reply_int = sql_count("mo_pun_acc", "sts=0 and ac_id='%d' and ac_seqn='%d'", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "统计贷款罚息登记簿记录个数出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
		else if( g_reply_int == 0 )
		{
			strcpy( g_pub_tx.reply, "L228" );
			sprintf( acErrMsg, "该账户没有启用罚息!ac_id[%d]ac_seqn[%d]", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			WRITEMSG
			goto ErrExit;			
		}

		/*修改*/
		g_reply_int = sql_max_int( "mo_pun_acc", "sts", "ac_id=%d and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "取贷款罚息登记簿记录最大sts值出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}

		g_reply_int = sql_execute( "update mo_pun_acc set sts=%d,end_date=%d,end_tel='%s',end_br_no='%s' where sts=0 and ac_id=%d and ac_seqn=%d", \
			g_reply_int + 1, g_pub_tx.tx_date, g_pub_tx.tel, \
			g_pub_tx.tx_br_no, sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "更新贷款罚息登记簿出错![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
	}
	else goto ErrExit;
	
	/*登记分段积数表*/
	g_reply_int= pub_ln_Ins_sect_rate ( g_ln_mst, g_ln_parm, rate );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "登记分段积数表失败![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}	
	
	/* Pub_tx结构赋值 */
	strcpy( g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
	g_pub_tx.ac_id=g_ln_mst.ac_id;
	g_pub_tx.ac_seqn=g_ln_mst.ac_seqn;
	strcpy(g_pub_tx.add_ind,"1");		/* 增减:0减1加2销户 */	
	strcpy( g_pub_tx.ct_ind,"2");		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3063;			
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );	
	strcpy ( g_pub_tx.brf,"贷款罚息启用" );	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
		
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
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
