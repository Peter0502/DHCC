/*************************************************
* 文 件 名:  sp9809.c
* 功能描述： 由操作类型\帐号\账户序号\支取日期\支取金额 
*			 回显客户信息
*
* 作    者:  rob
* 完成日期： 2003年1月15日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"


int sp9809()
{
	int	ret;
	char tag[2],tmp_ind2[2];				/*操作类型\违约标志*/
	long tmp_date1;
	long tmp_date2;
	double tmp_amt;							/*变更后支取金额*/

	struct	mo_infrm_c	g_mo_infrm;		
	struct  cif_basic_inf_c     g_cif_basic_inf;
	struct  cif_email_inf_c     g_cif_email_inf;
	struct  cif_id_code_rel_c   g_cif_id_code_rel;
	struct  prdt_ac_id_c        g_prdt_ac_id;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	get_zd_data( "0670", tag ); 	    			/* 操作类型标志 */
	get_zd_data( "0310", g_pub_tx.ac_no ); 			/* 帐号 */
	get_zd_long( "0440", &tmp_date1 ); 				/* 支取日期 */
	get_zd_double( "0410", &g_pub_tx.tx_amt1 ); 	/* 支取金额 */
	get_zd_long( "0450", &tmp_date2 ); 				/* 变更后支取日期 */
	get_zd_double( "0420", &tmp_amt ); 				/* 变更后支取金额 */
	get_zd_data( "0690", tmp_ind2 ); 				/* 违约标志 */

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						  g_pub_tx.ac_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "介质账户对照表中不存在该账户!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找介质账户对照表异常!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W011" );
		goto ErrExit;
	}

	/*** 判断介质状态 ***/
	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已被换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已部提换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已做销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf( acErrMsg, "帐户不存在!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找异常!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	ret =Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "定期参数表不存在该产品!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查找异常!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	vtcp_log("---------------------------->g_td_parm.td_type==[%s]",
									g_td_parm.td_type);

	if( g_td_parm.td_type[0]!='4' )
	{
		sprintf( acErrMsg, "请输入通知存款账号!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O213" );
		goto ErrExit;
	}

	if( tag[0]=='2'||tag[0]=='3' )					/* 撤销、更改 */
	{
	 	/*根据帐号ID、支取日期、支取金额找客户基本信息*/
		ret=Mo_infrm_Sel( g_pub_tx.reply , &g_mo_infrm ,
					"ac_id=%ld and prm_draw_date=%ld and prm_draw_amt=%.2lf",
	     			g_mdm_ac_rel.ac_id,tmp_date1,g_pub_tx.tx_amt1 );	
		if( ret==100 )
		{
			sprintf( acErrMsg, "通知登记薄中未找到该笔账户记录!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "W057" );
			goto	ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询通知登记薄异常!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "W056" );
			goto	ErrExit;
		}

		set_zd_data( "0250", g_mo_infrm.name );			/* 账户名称 */
		set_zd_data( "0610", g_mo_infrm.phone );		/* 电话号码 */
		set_zd_data( "0680", g_mo_infrm.id_knd );		/* 证件类型 */
		set_zd_data( "0620", g_mo_infrm.id_no );		/* 证件号码 */
	}
	else if( tag[0]=='1' )								/* 登记 */
	{
		ret = Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%ld", \
								g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "查询电话登记薄出错!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "C062" );
			goto ErrExit;
		}

		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "查询客户信息表出错!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "B115" );
			goto ErrExit;
		}

		ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
								g_td_mst.cif_no);
		if( ret )
		{
			sprintf( acErrMsg, "查询电话登记薄出错!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "C062" );
			goto ErrExit;
		}

		set_zd_data( "0250", g_cif_basic_inf.name );		/* 账户名称 */
		set_zd_data( "0610", g_cif_email_inf.email );		/* 电话号码 */
		set_zd_data( "0680", g_cif_id_code_rel.id_type );	/* 证件类型 */
		set_zd_data( "0620", g_cif_id_code_rel.id_no );		/* 证件号码 */
	}
	else 
	{
		sprintf( acErrMsg, "操作类型选择出错!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "C062" );
		goto ErrExit;
	}

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
