/*************************************************
* 文 件 名:  sp9886.c
* 功能描述： 判断显示账号(活期)是否存在
*
* 作    者:  rob
* 完成日期： 2003年4月22日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_branch_c.h"

int sp9886()
{
    int ret;
	char flag[2];
	char br_no[6];
	struct dd_mst_c s_dd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct com_branch_c sCom_branch;
   
    memset(&s_dd_mst, 0x0, sizeof(struct dd_mst_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	get_zd_data("0680",flag);
	if (flag[0] == '1')
	{
    	get_zd_data("0300",g_pub_tx.ac_no);
		pub_base_old_acno( g_pub_tx.ac_no );
	}
	else
	{
    	get_zd_data("0310",g_pub_tx.ac_no);
		pub_base_old_acno( g_pub_tx.ac_no );
	}

	if (strcmp(g_pub_tx.ac_no,"") == 0)
	{
		goto OkExit;
	}

    sprintf(acErrMsg,"ac_no=[%s]!! ",g_pub_tx.ac_no);
    WRITEMSG

    strcpy(g_pub_tx.cur_no,"01");

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'", 
			g_pub_tx.ac_no); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"查询介质关系表错误!! [%d] [%s]",ret, g_pub_tx.ac_no); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d", sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn); 
	if (ret != 0 && ret != 100) 
	{ 
		sprintf(acErrMsg,"查询产品帐号对照表错误!! [%d]",ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	else if (ret == 100) 
	{
		ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn); 
		if ( ret != 0) 
		{ 
			sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]", 
						g_pub_tx.cur_no); 
			WRITEMSG 
			goto ErrExit; 
		} 
	} 
	else 
	{ 
		g_pub_tx.ac_seqn = sPrdt_ac_id.ac_seqn; 
	}

sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
WRITEMSG

	ret=pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.ac_type);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询账号错误!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D169");
		goto ErrExit;
	}
	
    /*根据显示账号,序号取得账户id*
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &s_mdm_ac_rel,"ac_no='%s' ", 
		  g_pub_tx.ac_no); 
	if ( ret==100 ) 
	{ 
		sprintf(acErrMsg,"介质帐号表中不存在该记录[%d]",ret); 
		WRITEMSG 
		strcpy (g_pub_tx.reply,"W010"); 
		goto ErrExit;
	} 
	else if (ret && ret!=100) 
	{ 
		sprintf(acErrMsg,"查询介质帐号表异常[%d]",ret); 
		WRITEMSG 
		strcpy (g_pub_tx.reply,"W011"); 
		goto ErrExit;
	}

	*根据活期帐户id和账户序号取活期主文件*
	memset(s_dd_mst, 0x00, sizeof(struct dd_mst_c) );

	sprintf(acErrMsg,"ac_id=[%ld] ac_seqn=[%d]",s_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	WRITEMSG

	ret = Dd_mst_Sel(g_pub_tx.reply, &s_dd_mst, 
		"ac_id=%ld and ac_seqn=%d",s_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn); 
	if ( ret==100 ) 
	{ 
		sprintf(acErrMsg,"没有该活期帐户[%d]",ret); 
		WRITEMSG 
		strcpy (g_pub_tx.reply,"D162"); 
		goto ErrExit;
	} 
	else if(ret && ret!=100) 
	{ 
		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret); 
		WRITEMSG 	
		strcpy (g_pub_tx.reply,"W015"); 
		goto ErrExit;
	}
	***/

	if (atoi(g_pub_tx.ac_type) == 1)
	{ 
		set_zd_data("0910",g_dd_mst.opn_br_no); 
		strcpy(br_no, g_dd_mst.opn_br_no);
		sprintf(acErrMsg,"opn_br_no=[%s]",g_dd_mst.opn_br_no);
		WRITEMSG
	} 
	else if (atoi(g_pub_tx.ac_type) == 2) 
	{ 
		set_zd_data("0910",g_td_mst.opn_br_no); 
		strcpy(br_no, g_td_mst.opn_br_no);
	} 
	else if (atoi(g_pub_tx.ac_type) == 3) 
	{ 
		set_zd_data("0910",g_ln_mst.opn_br_no); 
		strcpy(br_no, g_ln_mst.opn_br_no);
	} 
	else if (atoi(g_pub_tx.ac_type) == 4) 
	{ 
		set_zd_data("0910",g_od_mst.opn_br_no); 
		strcpy(br_no, g_od_mst.opn_br_no);
	}
	else if (atoi(g_pub_tx.ac_type) == 9)
	{
		set_zd_data("0910",g_in_mst.opn_br_no); 
		strcpy(br_no, g_in_mst.opn_br_no);
	}

	ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no = '%s'", br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询公共机构编码表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0830", sCom_branch.br_name);
	if (flag[0] == '1')
	{
    	set_zd_data("0300",g_pub_tx.ac_no);
	}
	else
	{
    	set_zd_data("0310",g_pub_tx.ac_no);
	}

    set_zd_data("0690","0");

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply, "0000" );
    set_zd_data("0690","1");
	sprintf(acErrMsg,"该帐户系统中不存在: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
