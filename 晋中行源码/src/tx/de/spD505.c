/***************************************************************
* 文 件 名: spD505.c
* 功能描述：0零余额帐户销户(内部帐、对公活期)
* 作    者: mike
* 完成日期：2003年3月1日
*
*    修改记录：liuxuan
*    日    期: 20070713
*    修 改 人:
*    修改内容:
*    按财务要求冻结状态不能销户 82行增加 modify by wudawei 20130425 
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "note_cheq_mst_c.h"
#include "mo_prdt_prot_c.h" /****20130715 wudawei 协定存款判断****/
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/
spD505()
{
	int ret=0;
	char ac_no[20];
	char tmp_ind1[2],tmp_ind2[2],flag[2];
	char tmp_name[50],tmp_phone[20],tmp_id_knd[1],tmp_id_no[20];
	long tmp_date1,tmp_date2;
	double tmp_amt;

	struct mo_infrm_c		g_mo_infrm;
	struct in_mst_c			f_in_mst;
	struct mdm_ac_rel_c		f_mdm_ac_rel;
	struct dd_mst_c			f_dd_mst;
	struct note_cheq_mst_c		g_note_cheq_mst;
	struct mo_prdt_prot_c g_mo_prdt_prot; /****20130715 wudawei 协定存款判断****/
	struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,'\0',sizeof(g_dd_mst)); memset(&g_in_mst,'\0',sizeof(g_in_mst));
	memset(&f_dd_mst,'\0',sizeof(f_dd_mst)); memset(&f_in_mst,'\0',sizeof(f_in_mst));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(&g_note_cheq_mst,0x00,sizeof(struct note_cheq_mst_c));
	memset(&g_mo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));/****20130715 wudawei 协定存款判断****/

	g_reply_int=0;
	flag[0]='0';

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* 帐号 */
	get_zd_data("0250",tmp_name);         		/* 账户名称 */
	get_zd_double("0410",&g_pub_tx.tx_amt1);     	/* 账户余额 */

	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
		"ac_no='%s'",g_pub_tx.ac_no);
  	if( g_reply_int==100 )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if(g_reply_int && g_reply_int!=100)
	{
	   sprintf(acErrMsg,"根据显示帐号查询介质账户对照表失败!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"P147");
	   goto ErrExit;
	}
	
	/****增加关于协定存款帐户销户时的判断，若未解约即来销户，让其先去解约  add at 20130715 wudawei****/
		ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&g_mo_prdt_prot,"main_ac_id = %ld and prot_type ='XD1' and sts != '*' ", \
				g_mdm_ac_rel.ac_id);
		if(ret==0)
		{
			sprintf(acErrMsg,"该帐户为协定存款帐户，请先解约再销户!![%s] ret=[%d]",g_mdm_ac_rel.ac_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P182");
			goto ErrExit;
		}else if(ret==100){/****不是签约帐户，继续往下走****/
			vtcp_log("[%s][%d] 该帐户不是协定存款帐户!![%s] ret=[%d]",__FILE__,__LINE__,g_mdm_ac_rel.ac_no,ret);
		}else if(ret){
			sprintf(acErrMsg,"查询理财协议登记簿异常!!ret=[%d] 帐号=[%s]",ret,g_mdm_ac_rel.ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"X013");
			goto ErrExit;
		}
	/****end at 20130715****/
	
	  /*增加控制短信存在签约信息不能进行账户销户,必须先进行解约----add by zgf 2013-07-25*/
	  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s' and mob_sts='1' ",g_pub_tx.ac_no);
	  if(ret==100){
	          vtcp_log("[%s][%d]账号[%s]未查询到短信签约记录,可以销户!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	  }else if(ret==0){
	          vtcp_log("[%s][%d]账号[%s]存在短信签约记录!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "该账号存在短信通知业务，尚未解约!!");
	          strcpy(g_pub_tx.reply,"F037");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	
	  }else{
	          vtcp_log("[%s][%d]查询短信银行签约表错误!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "查询短信银行签约表错误!!");
	          strcpy(g_pub_tx.reply,"XXXX");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	  }
	  /*end add by zgf*/
	
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
	g_pub_tx.ct_ind[0]='2';
	if(g_pub_tx.ac_id<900000000){
		/**sprintf(acErrMsg,"对公,非内部帐号!");
		WRITEMSG
	  strcpy(g_pub_tx.reply, "A029");
		goto ErrExit;*/
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, "ac_id=%ld and ac_seqn=%d", g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if( g_reply_int )
		{
		   sprintf(acErrMsg,"活期账中不存在该记录");
		   WRITEMSG
		   strcpy(g_pub_tx.reply,"P102");
		   goto ErrExit;
		}
		/** 按财务要求冻结状态不能销户 add by wudawei 20130425 **/
		if( g_dd_mst.hold_sts[0]!='0')
		{
				sprintf(acErrMsg,"该账户处于冻结状态,不得销户!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O084");
		    goto ErrExit;
		}
		/** add end **/
			/*检查是否有利息积数*/
		if( g_dd_mst.intst_type[0]!='0')
		if(pub_base_CompDblVal(g_dd_mst.intst_acm,0.00))
		{
		 	sprintf(acErrMsg,"该账户尚存利息,不得销户!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O076");
		    goto ErrExit;
		}
		/* 控制凭证,活期户凭证必须为空才能销户 add by martin 2008/12/22 11:56*/
		g_reply_int=Note_cheq_mst_Sel(g_pub_tx.reply,&g_note_cheq_mst, "ac_id=%ld and cnt>0 ", g_mdm_ac_rel.ac_id);
		if( g_reply_int==0 )
		{
		   sprintf(acErrMsg,"活期账中存在凭证记录");
		   WRITEMSG
		   strcpy(g_pub_tx.reply,"N037");
		   goto ErrExit;
		}
	}
	else{
			g_reply_int=In_mst_Sel(g_pub_tx.reply,&g_in_mst, \
				"ac_id=%ld", g_mdm_ac_rel.ac_id);
			if( g_reply_int )
			{
			   sprintf(acErrMsg,"内部账中不存在该记录");
			   WRITEMSG
			   strcpy(g_pub_tx.reply,"W010");
			   goto ErrExit;
			}
				/*检查是否有利息积数*/
		if(g_in_mst.intst_knd[0]!='0')
		if(pub_base_CompDblVal(g_in_mst.intst_acm,0.00))
		{
		 	sprintf(acErrMsg,"该账户尚存利息,不得销户!!");
		  	WRITEMSG
		    strcpy(g_pub_tx.reply,"O076");
		    goto ErrExit;
		}			
	}
	/*检查账户状态*/
	if(g_mdm_ac_rel.note_sts[0]=='*' || g_in_mst.sts[0]=='*' || g_dd_mst.ac_sts[0]=='*')
	{
	 	sprintf(acErrMsg,"该账户已销户!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"P172");
	    goto ErrExit;
	}

	/*检查是否有余额*/
	g_reply_int=pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00);
	if(g_reply_int)
	{
	 	sprintf(acErrMsg,"该账户尚存余额,不得销户!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"O077");
	    goto ErrExit;
	}


	if(g_pub_tx.ac_id<900000000){	
		g_reply_int=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_dd_mst.ac_sts,"*");
		/*****add by liuyue  20070208加上上次交易日期***/
		f_dd_mst.lst_date = g_pub_tx.tx_date;
		/****add end****/

		g_reply_int=Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Dd_mst_Clo_Upd( );
		/*** xxx ---- @20040907@ ---- 为撤销用 ---**/
		g_pub_tx.svc_ind=1003;
	}
	else{
		/*修改内部台账为销户*/
		g_reply_int=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"定义游标错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=In_mst_Fet_Upd(&f_in_mst,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"取记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(f_in_mst.sts,"*");
		/*****add by liuyue  20070208加上上次交易日期***/
		f_in_mst.lst_date = g_pub_tx.tx_date;
		/****add end****/

		g_reply_int=In_mst_Upd_Upd(f_in_mst,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"修改记录错误!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		In_mst_Clo_Upd( );
		/*** xxx ---- @20040907@ ---- 为撤销用 ---**/
		g_pub_tx.svc_ind=9002;
	}

			/*修改介质帐号表为销户*/
	g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
									g_mdm_ac_rel.ac_no);
    if(g_reply_int)
   	{
   		sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int=Mdm_ac_rel_Fet_Upd(&f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int&&g_reply_int!=100)
	{
		sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(f_mdm_ac_rel.note_sts,"*");

	g_reply_int=Mdm_ac_rel_Upd_Upd(f_mdm_ac_rel,g_pub_tx.reply);
	if(g_reply_int)
	{
	 	sprintf(acErrMsg,"修改记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Mdm_ac_rel_Clo_Upd( );

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"0余额帐户销户交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"0余额帐户销户交易失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
