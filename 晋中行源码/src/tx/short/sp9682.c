/*************************************************
* 文 件 名:  sp9682.c
* 功能描述： 根据显示账号查询户名
*
* 作    者:  rob
* 完成日期： 2003年12月08日 
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
#include "cif_addr_inf_c.h"
#include "sub_dd_mst_c.h"
#include "com_item_c.h"

int sp9682()
{
    int ret;
	int	ac_seqn;
	char name[51],cur_no[3],note_no[3];
	char ac_type[2];
	int flag=0;
	double bal;
	struct prdt_ac_id_c	prdt_ac_id_c;
	struct cif_addr_inf_c	cif_addr_inf_v;
	struct prdt_ac_id_c prdt_ac_id_v;
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct sub_dd_mst_c sub_dd_mst_c;
	struct com_item_c sCom_item;
   
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_ln_mst,0x0,sizeof(struct ln_mst_c));
    memset(&g_in_mst,0x0,sizeof(struct in_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&cif_addr_inf_v,0x00,sizeof(struct cif_addr_inf_c));
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	memset(&sCom_item, 0x0, sizeof(sCom_item));
	ret=0;
	g_reply_int=0;
	bal=0.00;

    get_zd_data("0300",g_mdm_ac_rel.ac_no);
	pub_base_old_acno(g_mdm_ac_rel.ac_no);
    set_zd_data("0300",g_mdm_ac_rel.ac_no);

    strcpy(g_pub_tx.cur_no,"01");

sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_mdm_ac_rel.ac_no,g_pub_tx.ac_seqn);
WRITEMSG

	if ( strlen(g_mdm_ac_rel.ac_no) <= 7 )
	{
		/* 科目*/
		ret = Com_item_Sel(g_pub_tx.reply, &sCom_item, "acc_no='%s'",
			g_mdm_ac_rel.ac_no);
		if ( ret )
		{
			sprintf(acErrMsg, "该科目号不正确 [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "S033");
			goto ErrExit;
		}

		g_pub_tx.ac_type[0] = '7';
		goto RRRR;
	}

    g_reply_int= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",
				g_mdm_ac_rel.ac_no);
	if( g_reply_int ) 
	{
       	sprintf(acErrMsg,"查询对方账号错误!![%s] [%d]", g_mdm_ac_rel.ac_no, g_reply_int);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]!='0' &&
		g_mdm_ac_rel.note_sts[0]!='1' )
	{
       	strcpy(g_pub_tx.reply, "P172");
		TRACE;
       	goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_v,"ac_id=%ld and \ 
			ac_seqn=%d", g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn); 
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
		g_pub_tx.ac_seqn = prdt_ac_id_v.ac_seqn;
	}

	strcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
	sprintf(acErrMsg,"ac_no=[%s],ac_seqn=[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG

	ret = pub_base_disac_file(g_pub_tx.ac_no, g_pub_tx.ac_seqn, 
		g_pub_tx.ac_type); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"查询账号错误!! "); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"D169"); 
		goto ErrExit; 
	}

	RRRR:
	if (atoi(g_pub_tx.ac_type) == 1) 
	{ 
		set_zd_data("0270",g_dd_mst.name); 
	} 
	else if (atoi(g_pub_tx.ac_type) == 2) 
	{ 
		set_zd_data("0270",g_td_mst.name); 
	} 
	else if (atoi(g_pub_tx.ac_type) == 3) 
	{ 
		set_zd_data("0270",g_ln_mst.name); 
	} 
	else if (atoi(g_pub_tx.ac_type) == 4) 
	{ 
		sprintf(acErrMsg,"透支账户主文件没有户名"); 
		WRITEMSG 
	} 
	else if ( atoi(g_pub_tx.ac_type) == 7 )
	{
		set_zd_data("0270", sCom_item.acc_name);
	}
	else if (atoi(g_pub_tx.ac_type) == 9) 
	{ 
		set_zd_data("0270", g_in_mst.name); 
	}

	g_reply_int=Sub_dd_mst_Sel( g_pub_tx.reply,&sub_dd_mst_c,
		"ac_no='%s'",g_mdm_ac_rel.ac_no );
	if( g_reply_int==0 )
	{
		vtcp_log(" is subdd");
		set_zd_data("0700","1");
	}
	else if(g_reply_int==100)
	{
		vtcp_log(" is't subdd");
		set_zd_data("0700","0");
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询子帐号主文件表出错[%d]",g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
