/***************************************************************
* 文 件 名: spJC01.c
* 功能描述：单位定期销户和部提之后的金额不允许转入个人活期账户上
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"

spJC01()
{
	char tmpname[100],intst_type[17],ct_name[5],term[3],memo[5],id_name[21];
	char br_name[31],br_tele[31];
	FILE *fp;
	char acc_no[8];
	double val=0.00 , dealval=0.00;
	int ret=0;

	struct cif_id_code_rel_c g_cif_id_code_rel;

	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
 vtcp_log("哈哈找到了");
	get_zd_data("102K",g_pub_tx.ct_ind);
	get_zd_data("1011",g_pub_tx.ac_no);
	get_zd_data("1021",g_pub_tx.ac_no1);
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	pub_base_old_acno( g_pub_tx.ac_no );
	pub_base_old_acno( g_pub_tx.ac_no1 );
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", 
			g_pub_tx.ac_no1);
		if( ret ) goto ErrExit;
	if( g_mdm_ac_rel.ac_seqn==0 )
	{
		/*if( g_pub_tx.ct_ind[0]=='2' && 
			( g_pub_tx.ac_no[0]=='1' || strlen(g_pub_tx.ac_no)==19 ||
				strlen(g_pub_tx.ac_no)==16 ))
		{
			TRACE
			strcpy( g_pub_tx.reply,"D216" );
			goto ErrExit;
		}*/
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"销户成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	/*
    strcpy(g_pub_tx.reply,"A033");
	*/
	sprintf(acErrMsg,"销户失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
