/***************************************************************
* 文 件 名:     spD110.c
* 功能描述：	开立虚拟总帐号交易
*
* 作    者:     jack
* 完成日期：    2004年7月13日
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
#include "cif_id_code_rel_c.h"
#include "mo_prot_ac_rel_c.h"

spD110()
{
	int	ret=0;
	struct cif_id_code_rel_c	sCif_id_code_rel;
	struct mo_prot_ac_rel_c	sMo_prot_ac_rel;
	char opn_cls_flag[2],ac_type[2];

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&sCif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	get_zd_data("0310",g_pub_tx.ac_no);			/*** 子帐号 ***/
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*** 子帐号序号 ***/
	get_zd_data("0250",g_pub_tx.name);			/*** 总帐号户名 ***/
	get_zd_data("0670",g_pub_tx.id_type);		/*** 总帐号证件类型 ***/
	get_zd_data("0620",g_pub_tx.id_no);			/*** 总帐号证件号码 ***/
	get_zd_data("0680",opn_cls_flag);
	strcpy(g_pub_tx.brf,"开虚拟总帐号");
	strcpy(g_pub_tx.cur_no,"01");				/*** 默认人民币 ***/
	g_pub_tx.trace_cnt=0;
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.tx_amt1=0.00;

	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel, \
							"id_type='%s' and id_no='%s' ", \
							g_pub_tx.id_type,g_pub_tx.id_no);
	if( ret )
	{
		sprintf(acErrMsg,"客户信息不存在![%s][%s]", \
				g_pub_tx.id_type,g_pub_tx.id_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C103");
		goto ErrExit;
	}

	ret=pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,ac_type);
	if( ret )
	{
		sprintf(acErrMsg,"子帐号不存在![%s][%d]", \
				g_pub_tx.ac_no,g_pub_tx.ac_seqn);
		WRITEMSG
		goto ErrExit;
	}

	if( ac_type[0]=='1' )
		g_pub_tx.ac_id=g_dd_mst.ac_id;
	else if( ac_type[0]=='2' )
		g_pub_tx.ac_id=g_td_mst.ac_id;
	else
	{
		sprintf(acErrMsg,"子帐号非存款帐号![%s][%d]", \
				g_pub_tx.ac_no,g_pub_tx.ac_seqn);
		WRITEMSG
		goto ErrExit;
	}

	if( opn_cls_flag[0]=='1' )
	{
		/*** 检查该子帐号是否登记过 ***/
		ret=sql_count("mo_prot_ac_rel","main_ac_id=%ld and app_ac_id=%ld", \
						sCif_id_code_rel.cif_no,g_pub_tx.ac_id);
		if( ret>0 && ret!=100 )
		{
			sprintf(acErrMsg,"该子母帐号已经登记过了!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P455");
			goto ErrExit;
		}
		strcpy(sMo_prot_ac_rel.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(sMo_prot_ac_rel.prot_type,"Z01");
		sMo_prot_ac_rel.main_ac_id=sCif_id_code_rel.cif_no;
		sMo_prot_ac_rel.main_ac_seqn=0;
		sMo_prot_ac_rel.app_ac_id=g_pub_tx.ac_id;
		sMo_prot_ac_rel.app_ac_seqn=g_pub_tx.ac_seqn;
		sMo_prot_ac_rel.tx_date=g_pub_tx.tx_date;
		sMo_prot_ac_rel.trace_no=g_pub_tx.trace_no;
		strcpy(sMo_prot_ac_rel.tel,g_pub_tx.tel);
		strcpy(sMo_prot_ac_rel.sts,"1");
		ret=Mo_prot_ac_rel_Ins(sMo_prot_ac_rel,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记虚拟母帐号错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P456");
			goto ErrExit;
		}
	}
	else if( opn_cls_flag[0]=='2' )
	{
		ret=sql_execute("update mo_prot_ac_rel set sts='*' where main_ac_id=%ld and app_ac_id=%ld and app_ac_seqn=%d ",sCif_id_code_rel.cif_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret )
		{
			sprintf(acErrMsg,"更改虚拟总帐号错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P457");
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"业务操作码错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P458");
		goto ErrExit;
	}
	sprintf(g_pub_tx.ac_no1,"%ld",sCif_id_code_rel.cif_no);	/*** 总帐号 ***/
	set_zd_data("0300",g_pub_tx.ac_no1);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开子帐号成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开子帐号失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
