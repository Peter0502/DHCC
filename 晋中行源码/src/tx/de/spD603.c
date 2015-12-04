/***************************************************************
* 文 件 名: spD603.c
* 功能描述：支付密码信息管理
* 作    者: mike
* 完成日期：2003年06月09日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "paymach_mana_inf_c.h"
#include "prdt_ac_id_c.h"

spD603()
{   
	int ret=0; 
	char opt_code[2];
	char tmp_ac_no[128];
	char inf[50];										/*返回响应信息*/

	struct paymach_mana_inf_c	g_paymach_mana_inf;
	struct prdt_ac_id_c			g_prdt_ac_id;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_paymach_mana_inf,0x00,sizeof(struct paymach_mana_inf_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0750",tmp_ac_no);  			        	/* 账号 */
	get_zd_data("0730",g_paymach_mana_inf.id_no );      	/* 机具号码 */
	get_zd_data("0070",g_paymach_mana_inf.oper_no ); 		/* tel_no */
	get_zd_data("0670",opt_code );					      	/* 处理标志 */
								/*0－增加；1－删除；2－停用；3－启用；4－注销*/
	get_zd_long("0440",&g_paymach_mana_inf.pr_date );  	 	/* 传输日期 */

	sprintf(acErrMsg,"---------->截前的账号为[%s]", tmp_ac_no );
	WRITEMSG
	memcpy( g_paymach_mana_inf.ac_no, tmp_ac_no+12, 20 );
	g_paymach_mana_inf.ac_no[20] = '\0';
   	sprintf(acErrMsg,"---------->截后的账号为[%s]", g_paymach_mana_inf.ac_no );
	WRITEMSG
	
	if ( g_paymach_mana_inf.ac_no[0] == '0' )
	{
		memset( g_paymach_mana_inf.ac_no, 0x0, sizeof(g_paymach_mana_inf.ac_no) );
		memcpy( g_paymach_mana_inf.ac_no, tmp_ac_no+19, 13 );
		g_paymach_mana_inf.ac_no[13] = '\0';
   		sprintf(acErrMsg,"---------->截后的账号为[%s]", g_paymach_mana_inf.ac_no );
		WRITEMSG
	}

	if( opt_code[0] != '0' )
	{   		
		pub_base_old_acno( g_paymach_mana_inf.ac_no );
	}

	switch(opt_code[0])									
	{		
		case '0':										/***0-增加***/
				strcpy(g_paymach_mana_inf.flag,"0");	/***0-正常***/
														/***1-停用***/
														/***4-注销***/
				ret = Paymach_mana_inf_Ins(g_paymach_mana_inf,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"登记支付密码信息管理表出错!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O202");
	   				goto ErrExit;
				}

				/***修改支付密码标志***/
				ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										 g_paymach_mana_inf.ac_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"定义游标出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret==100 )
  				{
	   				sprintf(acErrMsg,"帐号不存在!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	   				goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				strcpy(g_mdm_ac_rel.pwd_mach_yn,"Y");

				ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"修改记录出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				Mdm_ac_rel_Clo_Upd( );
				break;
		case '1':										/***1-删除***/

				ret = sql_count("paymach_mana_inf","ac_no='%s' and id_no='%s'",
							g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
				if( ret<0 )
  				{
	   				sprintf(acErrMsg,"调用sql_count出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}
				else if( ret==0 )
  				{
	   				sprintf(acErrMsg,"不存在该帐户和机具关系!ac_no='%s' and id_no='%s'",g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O207");
	   				goto ErrExit;
				}

				ret = sql_execute("delete from paymach_mana_inf where ac_no='%s' and id_no='%s'",g_paymach_mana_inf.ac_no,g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"删除支付密码信息管理表记录出错!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O203");
	   				goto ErrExit;
				}
				vtcp_log("--------INSERT PAYMACHINE TABLE SUCCESS--------");

				/***修改支付密码标志***/
				ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
										 g_paymach_mana_inf.ac_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"定义游标出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret==100 )
  				{
	   				sprintf(acErrMsg,"帐号不存在!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	   				goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				strcpy(g_mdm_ac_rel.pwd_mach_yn,"N");

				ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"修改记录出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}

				Mdm_ac_rel_Clo_Upd( );
				break;
		case '2':										/***2-停用***/
				ret = sql_execute("update paymach_mana_inf set flag='1' where id_no='%s' and flag='0'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"更新支付密码信息管理表记录出错!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O204");
	   				goto ErrExit;
				}
				vtcp_log("--------UPDATE PAYMACHINE TABLE SUCCESS--------");
				break;
		case '3':										/***3-启用***/
				ret = sql_execute("update paymach_mana_inf set flag='0' where id_no='%s' and flag='1'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"更新支付密码信息管理表记录出错!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O204");
	   				goto ErrExit;
				}
				vtcp_log("--------UPDATE PAYMACHINE TABLE SUCCESS--------");
				break;
		case '4':										/***4-注销***/
				ret = sql_count("paymach_mana_inf","id_no='%s'", \
								 g_paymach_mana_inf.id_no);
				if(ret<0)
  				{
	   				sprintf(acErrMsg,"调用sql_count出错!");
	  				WRITEMSG
	   				goto ErrExit;
				}
				else if(ret==0)
  				{
	   				sprintf(acErrMsg,"不存在该机具!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O208");
	   				goto ErrExit;
				}

				ret = sql_execute("delete from paymach_mana_inf where id_no='%s'",g_paymach_mana_inf.id_no);
  				if( ret )
  				{
	   				sprintf(acErrMsg,"删除支付密码信息管理表记录出错!");
	  				WRITEMSG
	   				strcpy(g_pub_tx.reply,"O203");
	   				goto ErrExit;
				}
				vtcp_log("--------DELETE PAYMACHINE TABLE SUCCESS--------");
				break;
		default:
	   			sprintf(acErrMsg,"支付密码处理类型不存在!!");
	  			WRITEMSG
	   			strcpy(g_pub_tx.reply,"O205");
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
	sprintf(acErrMsg,"支付密码管理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"支付密码管理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	pub_base_regetinf(g_pub_tx.reply,inf);
	set_zd_data("0130",inf);
	return 1;
}
