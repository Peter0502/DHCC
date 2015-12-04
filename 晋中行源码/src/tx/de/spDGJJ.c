/***************************************************************
* 文 件 名: spDGJJ.c
* 功能描述：储蓄公积金存款销户
* 作    者: jack
* 完成日期：2004年6月6日
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

spDGJJ()
{
	char tmpname[100],intst_type[17],flag[2];
	int  mode;
	struct pub_tx	tmp_pub_tx;
	double dq_amt=0.00;
	double tmp_amt1=0.00;
	double tmp_amt2=0.00;
	double tmp_amt3=0.00;
	FILE *fp;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&tmp_pub_tx,0x00,sizeof(struct pub_tx));
	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	get_zd_data("1021",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );                 /**对旧帐号的处理**/
	g_pub_tx.ac_seqn=1;
	strcpy(g_pub_tx.note_type,"0000");
	get_zd_data("102G",g_pub_tx.id_type);
	get_zd_data("102A",g_pub_tx.id_no);	
    get_zd_data("102J",g_pub_tx.cur_no);
	get_zd_data("102K",g_pub_tx.ct_ind);
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.add_ind,"0");
    strcpy(g_pub_tx.hst_ind,"1"); /* 日间入明细 */
	strcpy( g_pub_tx.brf,"储蓄公积金取款" );
	get_zd_data("102M",flag);

	get_zd_data("0191",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}

	/*** 将定期户余额化回活期户 ***/
	memcpy(&tmp_pub_tx,&g_pub_tx,sizeof(struct pub_tx));
	if( flag[0]=='1' ) 			/*** 取款 ***/
		mode=3;
	else 
		mode=2;
	g_reply_int=pub_base_prot("Z02",mode);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"理财协定登记主控出错!");
		WRITEMSG
		goto ErrExit;
	}

	dq_amt=g_pub_tx.tmp_amt1;		/*** 定期取款总金额 ***/
	memcpy(&g_pub_tx,&tmp_pub_tx,sizeof(struct pub_tx));
	tmp_amt1=g_pub_intst.dealval;
	tmp_amt2=g_pub_intst.keepval;
	tmp_amt3=g_pub_intst.val;
	g_pub_intst.dealval=0.00;
	g_pub_intst.keepval=0.00;
	g_pub_intst.val=0.00;
	vtcp_log("HAO -- LOOK3 dealval=[%lf][%lf] keepval=[%lf][%lf] val=[%lf][%lf]",g_pub_intst.dealval,tmp_amt1,g_pub_intst.keepval,tmp_amt2,g_pub_intst.val,tmp_amt3);
	if( mode==2 )		/*** 销户 ***/
	{ g_pub_tx.svc_ind = 1010; }
	else 
	{ g_pub_tx.svc_ind = 1009; }

	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	/***Add By Azz.Part For Print***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(g_dd_mst.name);
	pub_base_strpack(g_dd_parm.title);
	pub_base_dic_show_str(intst_type,"intst_knd",g_dd_mst.intst_type);

	fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|\n",g_pub_tx.ac_no,g_dd_mst.name,dq_amt+g_pub_tx.tx_amt1,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no,0.00,g_pub_intst.dealval+g_pub_intst.keepval+tmp_amt1+tmp_amt2,g_pub_intst.val+tmp_amt3,0.20,(g_pub_intst.val+tmp_amt3)/0.20,g_pub_intst.dealval+tmp_amt1+g_pub_intst.keepval+tmp_amt2-g_pub_intst.val-tmp_amt3,dq_amt+g_pub_tx.tx_amt1+g_pub_intst.dealval+tmp_amt1+g_pub_intst.keepval+tmp_amt2-g_pub_intst.val-tmp_amt3,g_dd_mst.intst_acm,intst_type);

	fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no,g_dd_mst.name,dq_amt+g_pub_tx.tx_amt1,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.cif_no,0,0,"",0.00,0.00,"","","","","","公积金","",0.00);

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");

    set_zd_double("102F",g_pub_tx.tx_amt1);							
    set_zd_double("102I",g_pub_tx.tx_amt1+dq_amt);							
    set_zd_double("102P",g_pub_intst.dealval+tmp_amt1+g_pub_intst.keepval+tmp_amt2);	/*实付利息*/
    set_zd_double("102Q",g_pub_intst.val+tmp_amt3);				/*利 息 税*/
    set_zd_double("102R",g_pub_intst.keepval+tmp_amt2);			/*保值利息*/
    set_zd_double("102S",
    g_pub_tx.tx_amt1+dq_amt+g_pub_intst.dealval+tmp_amt1+g_pub_intst.keepval+tmp_amt2-g_pub_intst.val-tmp_amt3); 								/*应付总额*/

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
