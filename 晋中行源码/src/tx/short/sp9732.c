/*************************************************
* 文 件 名: sp9732.c
* 功能描述: 根据帐号返回相关信息_[开户确认]
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "mo_unchange_c.h"
#include "mo_opn_cls_c.h"

sp9732()  
{ 		
	int ret=0,ac_seqn;
	char title[40],cur_no[3];
	char tmp_type[2];
	char ac_no[20],name[61];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;
	struct mo_unchange_c g_mo_unchange;
	struct mo_opn_cls_c g_mo_opn_cls;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mo_unchange,0x00,sizeof(struct mo_unchange_c));
	memset(&g_mo_opn_cls,0x00,sizeof(struct mo_opn_cls_c));

	g_mo_unchange.bal=0.00;

	/** 取值、赋值 **/	
	get_zd_data("0310",ac_no);							/*帐号*/
	
	/* 调用根据帐号返回账户序号函数 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(g_prdt_ac_id.ac_id_type[0]=='1')
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
					g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		strcpy(g_prdt_ac_id.prdt_no,g_dd_mst.prdt_no);

		ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						g_prdt_ac_id.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					g_prdt_ac_id.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}

		if(g_dd_parm.cif_type[0]=='1')
		{
			sprintf(acErrMsg,"请输入对公账户!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}

		ret = Mo_opn_cls_Sel(g_pub_tx.reply,&g_mo_opn_cls,"ac_no='%s'",ac_no); 
		if( ret==100 )
		{
			sprintf(acErrMsg,"开销户登记薄无此记录!!ac_no=[%s]",ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}

		bal=g_dd_mst.bal;
		strcpy(name,g_dd_mst.name);
		strcpy(tmp_type,g_dd_mst.ac_type);
		strcpy(title,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
	}
	else
	{
		sprintf(acErrMsg,"该户不需要确认!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O177");
		goto ErrExit;
	}

	pub_base_strpack(name);
	pub_base_strpack(cur_no);
	pub_base_strpack(title);

	/* 输出变量 */
	set_zd_data("0210",cur_no);
	set_zd_data("0250",name);
	set_zd_data("0260",title);
	set_zd_data("0670",tmp_type);
	set_zd_double("1001",bal);

	set_zd_data("0300",g_mo_opn_cls.check_no);
	set_zd_data("0680",g_dd_mst.cash_ind);

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
   
