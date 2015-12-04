/***************************************************************
* 文 件 名: spD290.c
* 功能描述：不动户处理交易[操作久悬登记薄]
* 作    者: jack
* 完成日期：2004年1月15日
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
#include "mo_unchange_c.h"
#include "com_parm_c.h"

spD290()
{
	int ret=0;
	char ac_no[20],change_ind[2],tmp_name[50];
	char in_acno[20];
	char cBrf[50];
	double intst;

	struct prdt_ac_id_c		g_prdt_ac_id;
	struct mo_unchange_c	g_mo_unchange;
	struct com_parm_c		g_com_parm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_com_parm,0x00,sizeof(struct com_parm_c));
	memset(&g_mo_unchange,0x00,sizeof(struct mo_unchange_c));
	memset(cBrf,0x00,sizeof(cBrf));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

    get_zd_data( "0670", change_ind );             	/* 操作类型标志 */
	get_zd_data( "0310", g_pub_tx.ac_no );          /* 帐号 */
	
	get_zd_int( "0340",  &g_pub_tx.ac_seqn );       /* 账户序号 */
	get_zd_data( "0210", g_pub_tx.cur_no );         /* 币种 */
	get_zd_double( "1001", &g_pub_tx.tx_amt1 );     /* 支取金额 */
	get_zd_data( "0250", tmp_name );         		/* 账户名称 */
	/*get_zd_data( "0810", g_mo_unchange.brf ); */ 		/* 摘要 */
	get_zd_data( "0810", cBrf );  /*mod by lwb 20150508*/		/* 摘要 */
	get_zd_data( "0680", g_pub_tx.ct_ind ); 		/* 现转标志 */
	strcpy(g_mo_unchange.brf,cBrf);

	/*******************计算利息！！！******************/
	vtcp_log("[%s][%d] 久悬账户[%s] ct_ind",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ct_ind);

	/*根据帐号查出账户ID*/
    	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
					   "ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表中不存在该记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}

	if ( g_mdm_ac_rel.ac_seqn==9999 )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!");
			WRITEMSG
			goto ErrExit;
		}
		else if(ret>1)
		{
			g_mdm_ac_rel.ac_id = g_prdt_ac_id.ac_id;
		}
	}

	if(change_ind[0]=='1')								/*1-不动户处理*/
	{
		get_zd_data( "0330", in_acno );          /* 内部帐号 */
		vtcp_log("[%s][%d] 内部账户[%s]",__FILE__,__LINE__,in_acno);
		g_mo_unchange.ac_id=g_mdm_ac_rel.ac_id;
		g_mo_unchange.ac_seqn=g_mdm_ac_rel.ac_seqn;
		g_mo_unchange.tx_date=g_pub_tx.tx_date;
		get_zd_double( "102S", &g_mo_unchange.bal); /* 本金＋利息*/
		get_zd_double( "102P", &intst); /* 利息*/
		strcpy(g_mo_unchange.in_acno,in_acno); /***存放内部账户号**/
		/**
		intst=0.00;
		g_mo_unchange.intst=intst;
		**/
		g_mo_unchange.bal+=intst;

		strcpy(g_mo_unchange.ind,"N");
		strcpy(g_mo_unchange.prdt_no,g_prdt_ac_id.prdt_no);
		strcpy(g_mo_unchange.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(g_mo_unchange.tel,g_pub_tx.tel);

		vtcp_log("[%s][%d] 登记久悬登记薄",__FILE__,__LINE__);

		ret=Mo_unchange_Ins(g_mo_unchange,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记久悬登记薄失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O083");
			goto ErrExit;
		}
		vtcp_log("[%s][%d] 登记久悬登记薄[%d]",__FILE__,__LINE__,ret);
		
		
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
						   "ac_no='%s' and ac_seqn=%d",in_acno,0);
		if( ret )
		{
			sprintf(acErrMsg,"介质帐号对照表中未找到记录!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W010");
			goto ErrExit;
		}
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%ld'",g_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"未找到内部帐久悬帐号!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O080");
			goto ErrExit;
		}

		/**add by ligl 2006-10-2 16:01**/
		if(g_mdm_ac_rel.note_sts[0]=='5'||g_mdm_ac_rel.note_sts[0]=='*')
		{
	      goto ErrExit1;/*久悬内部账户状态不正常,新开户!*/
	  }	
	  /**end**/
		vtcp_log("[%s][%d] 登记久悬登记薄[%d]",__FILE__,__LINE__,ret);

		goto ErrExit2;
	}
	else if(change_ind[0]=='2')					/*2-不动户回取*/
	{       /*add by lwb   20141015  加一个ind 为N的判断*/
		ret=Mo_unchange_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d  and ind='N'",
								g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"Declare Mo_unchange_Dec_Upd失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O083");
			goto ErrExit;
		}

		ret=Mo_unchange_Fet_Upd(&g_mo_unchange,g_pub_tx.reply);
		if(ret==100)
		{
			sprintf(acErrMsg,"未找到该账户的不动户记录!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O081");
			goto ErrExit;
		}
		if(g_mo_unchange.ind[0]=='Y')
		{
			sprintf(acErrMsg,"该账户已经回取!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O082");
			goto ErrExit;
		}

		strcpy(g_mo_unchange.ind,"Y");

		ret=Mo_unchange_Upd_Upd(g_mo_unchange,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"Update Mo_unchange_Upd_Upd失败!!");
			WRITEMSG
			goto ErrExit;
		}

		Mo_unchange_Clo_Upd( );

		
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
						   "ac_no='%s' and ac_seqn=%d",g_mo_unchange.in_acno,0);
		if( ret )
		{
			sprintf(acErrMsg,"介质帐号对照表中未找到记录!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W010");
			goto ErrExit;
		}
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%ld'",g_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"未找到内部帐久悬帐号!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O080");
			goto ErrExit;
		}
		
		if(g_pub_tx.ct_ind[0] == '1')  /*现金支取*/
		{
			vtcp_log("[%s][%d] 久悬账户[%s]",__FILE__,__LINE__,g_mdm_ac_rel.ac_no);
			goto ErrExit3;
		}
		if(g_pub_tx.ct_ind[0]=='2')					/*1-转帐支取*/
			vtcp_log("[%s][%d] 久悬账户2[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
			vtcp_log("[%s][%d] 久悬账户3[%s]",__FILE__,__LINE__,g_mdm_ac_rel.ac_no);
			
			goto ErrExit4;
	}

	set_zd_data("0230",g_prdt_ac_id.prdt_no);

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

ErrExit1:
	set_zd_data("0230",in_acno);
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"[处理不动户]操作久悬登记薄成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit2:
	set_zd_data("0230",g_mdm_ac_rel.ac_no);
	set_zd_data("112A",g_mdm_ac_rel.ac_no);
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"[处理不动户]操作久悬登记薄成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit3:  /*add by lwb 20141010现金支取时走这个出口*/
	set_zd_data("0230",g_mdm_ac_rel.ac_no);
	set_zd_data("0320",g_mdm_ac_rel.ac_no);
	set_zd_double("1001",g_mo_unchange.bal);
	set_zd_double("1013",g_mo_unchange.bal);
	set_zd_data("1205",g_pub_tx.ct_ind);
	strcpy(g_pub_tx.reply,"0003");
	sprintf(acErrMsg,"[处理不动户]操作久悬登记薄成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit4:
       	set_zd_data( "1011", g_pub_tx.ac_no );          
	set_zd_int("1012",g_mo_unchange.ac_seqn);
	set_zd_data("1016",cBrf);
	set_zd_double("1001",g_mo_unchange.bal);
	set_zd_double("1013",g_mo_unchange.bal);
	set_zd_data("0230",g_mdm_ac_rel.ac_no);   /*内部帐号*/
	set_zd_data("0320",g_mdm_ac_rel.ac_no);
	strcpy(g_pub_tx.reply,"0004");
	sprintf(acErrMsg,"[不动户回取]操作久悬登记薄成功->转帐[%s]",
			g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"操作久悬登记薄失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
