/***************************************************************
* 文 件 名: spD401.c
* 功能描述: 捡拾交易
* 作    者: mike
* 完成日期: 2004年1月10日
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_coll_c.h"

spD401()
{
	int ret=0;

	/*定义结构*/
	struct mo_coll_c	mo_coll_c;
	struct mo_coll_c	g_mo_coll;

	/*清空结构*/
	memset(&g_mo_coll,0x00,sizeof(struct mo_coll_c));
	memset(&mo_coll_c,0x00,sizeof(struct mo_coll_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	/*取值、赋值*/
	get_zd_data("0670",mo_coll_c.sts);				/*处理标志*/
	get_zd_data("1021",mo_coll_c.ac_no);			/*卡/帐号*/
	strcpy(g_pub_tx.ac_no,mo_coll_c.ac_no);
	get_zd_data("0250",mo_coll_c.name);				/*户名*/

	get_zd_data("1142",g_pub_tx.draw_pwd_yn);		/*是否凭密*/
	get_zd_data("1144",g_pub_tx.draw_pwd);			/*支取密码*/
	get_zd_data("1145",g_pub_tx.draw_id_yn);		/*证件支取*/
	get_zd_data("1148",g_pub_tx.id_type);			/*证件类型*/
	get_zd_data("1146",g_pub_tx.id_no);				/*证件号码*/

	mo_coll_c.wrk_date = g_pub_tx.tx_date; 
	mo_coll_c.trance_no = g_pub_tx.trace_no; 

	if( mo_coll_c.sts[0]=='1' )			 			/*1 - 捡拾*/
	{
		mo_coll_c.pick_date = g_pub_tx.tx_date;
		strcpy(mo_coll_c.pick_br_no ,g_pub_tx.tx_br_no); 
		strcpy(mo_coll_c.pick_tel , g_pub_tx.tel); 

		mo_coll_c.ret_date = 0; 
		strcpy(mo_coll_c.ret_br_no, ""); 
		strcpy(mo_coll_c.ret_tel, ""); 

		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",mo_coll_c.ac_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"定义取介质账号对照表游标错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"介质账号对照表中不存在该账户![%d]",ret);
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

		if( g_mdm_ac_rel.coll_sts[0]=='1' )
		{
			sprintf(acErrMsg,"该账户已经处于捡拾状态![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W038");
			goto ErrExit;
		}
		else 
			strcpy(g_mdm_ac_rel.coll_sts,"1");

		ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"修改介质账号对照表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}

		Mdm_ac_rel_Clo_Upd( );


		/*登记捡拾登记薄*/
		ret = Mo_coll_Ins(mo_coll_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记捡拾登记薄错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W041");
			goto ErrExit;
		}
	}
	else if( mo_coll_c.sts[0]=='2' )				/* 2-归还 */
	{
		ret = Mo_coll_Dec_Upd( g_pub_tx.reply,"ac_no='%s' and sts='1'", \
						   g_pub_tx.ac_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该账户的捡拾记录![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W039");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret = Mo_coll_Fet_Upd(&g_mo_coll,g_pub_tx.reply);
		if(ret&&ret!=100)
		{
			sprintf(acErrMsg,"数据库失败[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
			
		}
		else if(ret==100)
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		if( strncmp(g_pub_tx.tx_br_no,g_mo_coll.pick_br_no,5) )
		{
			sprintf(acErrMsg,"本网点未捡拾该介质，请到捡拾行办理归还![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D199");
			goto ErrExit;
		}

		mo_coll_c.pick_date = 0;
		strcpy(mo_coll_c.pick_br_no,""); 
		strcpy(mo_coll_c.pick_tel, ""); 

		mo_coll_c.ret_date = g_pub_tx.tx_date; 
		strcpy(mo_coll_c.ret_br_no , g_pub_tx.tx_br_no); 
		strcpy(mo_coll_c.ret_tel , g_pub_tx.tel); 

		/*****不能少的*****/
		memcpy(mo_coll_c.rowid,g_mo_coll.rowid,sizeof(mo_coll_c.rowid)-1);
		
		ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",mo_coll_c.ac_no);
		if( ret )
		{
			sprintf(acErrMsg,"定义取介质账号对照表游标错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"介质账号对照表中不存在该账户![%d]",ret);
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

		if( g_mdm_ac_rel.coll_sts[0]=='0' )
		{
			sprintf(acErrMsg,"不存在该账户的捡拾记录![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W039");
			goto ErrExit;
		}
		else
		{
			/*检验支取方式*/
			ret=pub_base_check_draw("001");
			if (ret)
			{
				sprintf(acErrMsg,"支取方式不正确![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}

			/*状态修改为正常*/
			strcpy(g_mdm_ac_rel.coll_sts,"0");
		}

		ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"修改介质账号对照表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}

		Mdm_ac_rel_Clo_Upd( );

		/*登记捡拾登记薄*/
		ret = Mo_coll_Upd_Upd(mo_coll_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记捡拾登记薄错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W041");
			goto ErrExit;
		}

		set_zd_data("0680",g_mdm_ac_rel.note_sts);	/*返回挂失状态*/
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
	sprintf(acErrMsg,"捡拾处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"捡拾处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

