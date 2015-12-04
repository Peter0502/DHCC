/***************************************************************
* 文 件 名: spD099.c
* 功能描述：存款交易
* 作    者: jack
* 完成日期：2005年1月11日
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
#include "com_rate_c.h"

spD099()
{
	int ret;

	char tmpname[101],ct_name[5],memo[51],id_name[21];
	char br_name[31],br_tele[21],term[3],term_type[3];
	FILE *fp;
	char acc_no[8];
	struct com_rate_c sCom_rate;
	struct cif_id_code_rel_c g_cif_id_code_rel;

	memset(&sCom_rate,0x00,sizeof(struct com_rate_c));
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
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("1011",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	get_zd_data("101A",g_pub_tx.cur_no);
	get_zd_data("1014",g_pub_tx.note_type);
	get_zd_data("1015",g_pub_tx.beg_note_no);
	get_zd_data("1015",g_pub_tx.end_note_no);
	get_zd_data("101B",g_pub_tx.ct_ind);
	get_zd_data("1016",g_pub_tx.brf);				/***摘要***/
	
	vtcp_log("[%s][%d] beg_note_no=[%s]end_note_no=[%s] ",__FILE__,__LINE__,g_pub_tx.beg_note_no,g_pub_tx.end_note_no);
	
	/***add zgf 20131022 从0630域值中取摘要信息****/
	char tmp_brf1[30];
	memset(tmp_brf1,'\0',sizeof(tmp_brf1));
	get_zd_data("0630",tmp_brf1);
	/***add zgf end***/
        sprintf(acErrMsg,"brf=[%s]",g_pub_tx.brf);
        WRITEMSG
	
	/*char tmp_brf[4];*/
	char tmp_brf[30]; /***add zgf 20131022 修改定义大小为30**/
	memset(tmp_brf,'\0',sizeof(tmp_brf));
	get_zd_data("101C",tmp_brf);  
	vtcp_log("[%s][%d] 摘要0000001=[%c] ",__FILE__,__LINE__,tmp_brf);
	if(strlen(tmp_brf)==0)
	{
		memset(tmp_brf,'\0',sizeof(tmp_brf));
		memcpy(tmp_brf,tmp_brf1,sizeof(tmp_brf));
	}
	if(g_pub_tx.ct_ind[0]=='1')
	{
		pub_base_dic_show_str(g_pub_tx.brf,"cash_imemo",tmp_brf);
		/*add by lwb 20150123*/
		if(memcmp(tmp_brf,"031",3)==0)	/* 031表示自输摘要 */
		{
			get_zd_data("0330",g_pub_tx.brf);
		}
	}
	else
	{
		pub_base_dic_show_str(g_pub_tx.brf,"tran_memo",tmp_brf);
		if(memcmp(tmp_brf,"031",3)==0)	/* 031表示自输摘要 */
		{
			get_zd_data("0330",g_pub_tx.brf);
		}
	}
	vtcp_log("[%s][%d] 摘要=[%s] ",__FILE__,__LINE__,g_pub_tx.brf);
	if( strlen(g_pub_tx.brf)==0 )
	{
		strcpy( g_pub_tx.brf,"存款" );
	}
	get_zd_double("1013",&g_pub_tx.tx_amt1);

	if(g_pub_tx.ct_ind[0]=='2')
	{
		strcpy( memo,"转存" );
	}
	else strcpy( memo,"存款" );

	sprintf(acErrMsg,"传入金额[%lf],!!!brf[%s]",g_pub_tx.tx_amt1,g_pub_tx.brf);
	WRITEMSG
	
	if(!memcmp(g_pub_tx.tx_code,"2409",4))  /*add by lwb 20150508*/
			get_zd_data("1016",g_pub_tx.brf);
	sprintf(acErrMsg,"aaaa brf[%s]",g_pub_tx.brf);
	WRITEMSG

	get_zd_data("1017",g_pub_tx.prt_ind);
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ac_wrk_ind,"100");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=1001;
	get_zd_data("0191",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
    if (g_pub_tx.ct_ind[0] != '1')
    {
        strcpy(g_pub_tx.ct_ind,"2");
    }
	sprintf(acErrMsg,"BEG D099[%s][%s][%s][%d]",g_pub_tx.ac_no,
		g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
	WRITEMSG
	/** 币种 **/

	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("g_pub_tx.prdt_code=============[%s]",g_pub_tx.prdt_code);

	/***Part For Print By Azz***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
	if(g_pub_tx.prdt_code[0]=='1')
	{
		/*** 取证件信息 ***/
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_dd_mst.cif_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"该客户不存在证件信息!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"取客户证件信息出错");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}

		pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

		ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret==100 )
		{
			sprintf(acErrMsg,"无此活期参数!");
			WRITEMSG
			goto ErrExit;
		}

		pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
		pub_base_strpack(br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(g_dd_parm.title);
		pub_base_strpack(g_dd_mst.name);
		set_zd_data("1018", g_dd_mst.name);
		set_zd_double("1019", g_dd_mst.bal);
		if(  pub_base_prdt_accno( g_dd_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );
		
		/***此处增加返回交易行号 add by wanglei 20060920***/
		vtcp_log("[%s][%d] pub_txbrno =%s,rate=%lf\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_dd_mst.rate);
		/***取得利率***/
		ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
		if( ret)
		{
			sprintf(acErrMsg,"取利率错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"CKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|\n",
		g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
		g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.cif_no,0,0,"",
		0.00,g_dd_mst.bal,"","","","","",memo,"",0.00,ct_name,
		g_pub_tx.ac_seqn,g_td_mst.ic_date,g_mdm_ac_rel.prt_line,
		g_dd_mst.prdt_no,g_td_mst.book_line,g_pub_tx.brf,"","","",
		id_name,g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
		"",acc_no,g_pub_tx.tx_br_no,"","",sCom_rate.rate_val);
	}
	else if(g_pub_tx.prdt_code[0]=='2')
	{
		
		/*** 取证件信息 ***/
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_td_mst.cif_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"该客户不存在证件信息!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"取客户证件信息出错");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}

		pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret )
		{
			sprintf(acErrMsg,"无此定期参数!");
			WRITEMSG
			goto ErrExit;
		}

		/***返回存期和存期单位  add by wanglei 20060920***/
		sprintf(term,"%02d",g_td_parm.term);
		
		if(g_td_parm.term_type[0] == 'Y')
			strcpy(term_type,"年");
		else if(g_td_parm.term_type[0] == 'M')
			strcpy(term_type,"月");
		else if(g_td_parm.term_type[0] == 'D')
			strcpy(term_type,"日");

		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(br_tele);
		pub_base_strpack(g_td_parm.title);
		pub_base_strpack(g_td_mst.name);
		if(  pub_base_prdt_accno( g_td_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );
		/***此处增加返回交易行号 add by wanglei 20060920***/	
		vtcp_log("[%s][%d] pub_txbrno =%s,rate=%lf,term=%s,term_type=%s\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_dd_mst.rate,term,term_type);
		fprintf(fp,"CKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|\n",
		g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
		g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.cif_no,0,0,"",
		0.00,g_td_mst.bal,"","","","","",memo,"",0.00,ct_name,
		g_td_mst.ac_seqn,0,0,g_td_mst.prdt_no,0,g_pub_tx.brf,"","","",
		id_name,g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
		"", acc_no,g_pub_tx.tx_br_no,term,term_type,g_td_mst.rate);
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/***FINISH***/

	set_zd_data("101A",g_pub_tx.cur_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"存款成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"存款失败![%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		strcpy(g_pub_tx.reply,"D103");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
