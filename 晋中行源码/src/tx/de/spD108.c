/***************************************************************
* 文 件 名: spD108.c
* 功能描述：收费子交易
* 作    者: jack
* 完成日期：2004年1月18日
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
#include "com_chrg_rate_c.h"
#include "com_chrg_hst_c.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"
spD108()
{
	int ret=0;
	FILE *fp;
	char tmp_ac_no[21];
	char tmp_ac_no2[21];
	char tmpname[101];
	char ct_name[9];
	char charge_type[21];
	char memo[101];
	/****
	char tmp_ac_name[51];
	****/
	char tmp_ac_name[31];
	char tmp_ac_name2[51];
	char tmp_acc_hrt[8];
	struct com_chrg_rate_c g_com_chrg_rate;
	struct com_chrg_hst_c g_com_chrg_hst;
	struct dc_acc_rel_c g_dc_acc_rel;
	struct dc_acc_rel_c o_dc_acc_rel;
	struct dd_mst_c 	o_dd_mst;
	struct dd_parm_c 	o_dd_parm;
  struct com_item_c g_com_item;
	memset(&g_com_chrg_rate,0x00,sizeof(struct com_chrg_rate_c));
	memset(&g_com_chrg_hst,0x00,sizeof(struct com_chrg_hst_c));
	memset(&g_dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset(&o_dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
	memset(&o_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&o_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
  memset(&g_com_item,0x00,sizeof(struct com_item_c));
	pub_base_sysinit();

	get_zd_data("122A",g_pub_tx.cur_no);					/***币种***/
	get_zd_data("122B",g_pub_tx.ct_ind);					/***现转标志***/
	get_zd_double("1226",&g_pub_tx.tx_amt1);				/***收费金额***/	
	get_zd_data("1228",g_com_chrg_rate.chrg_rate_code);		/***收费编号***/
vtcp_log("编号=================[%s]",g_com_chrg_rate.chrg_rate_code);
	get_zd_data("1221",tmp_ac_no);							/***扣款帐号***/
	get_zd_data("122E",tmp_ac_name);						/***扣款户名称***/
	get_zd_data("122D",tmp_ac_no2);							/***收费帐号***/
	get_zd_data("122C",tmp_ac_name2);						/***收费户名称***/
	strcpy( g_pub_tx.note_type,"150" );

	vtcp_log("ac_no============[%s]",tmp_ac_no);

	ret = Com_chrg_rate_Sel(g_pub_tx.reply,&g_com_chrg_rate, \
						   "cur_no='%s' and chrg_rate_code='%s' and \
						   beg_date<%ld and end_date>%ld", \
						   g_pub_tx.cur_no,g_com_chrg_rate.chrg_rate_code, \
						   g_pub_tx.tx_date, g_pub_tx.tx_date);
	if( ret==100 )
	{
 	 	sprintf(acErrMsg,"不存在该费率编号!RATE_NO=[%s],CURNO=[%s]", \
				g_com_chrg_rate.chrg_rate_code,g_pub_tx.cur_no);
  	 	WRITEMSG
		strcpy(g_pub_tx.reply,"O168");
     	goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s'", \
					     g_com_chrg_rate.dc_code);
	if( ret==100 )
	{
 	 	sprintf(acErrMsg,"不存在该会计代码!");
  	 	WRITEMSG
		strcpy(g_pub_tx.reply,"O169");
     	goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
		ret = Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'", \
					     g_dc_acc_rel.acc_hrt);
	if( ret==100 )
	{
 	 	sprintf(acErrMsg,"不存在该会计代码!");
  	 	WRITEMSG
		strcpy(g_pub_tx.reply,"O169");
     	goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	strcpy( g_pub_tx.ac_no,g_com_item.acc_no);
	strcpy( g_pub_tx.brf,g_com_chrg_rate.name );
	strcpy( g_pub_tx.ac_id_type,"9" );     /*账户类型为内部*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" );   /*不核对凭证号,零金额不计流水、明细*/
	strcpy(g_pub_tx.add_ind,"1");			/*贷方标志*/
	g_pub_tx.svc_ind=9001;  				/*内部帐存取*/
 	strcpy(g_pub_tx.hst_ind,"1"); 			/*日间入明细*/

	ret = pub_acct_trance( );
	if( ret ) 
	{
 	 	sprintf(acErrMsg,"登记收费明细失败!ret=[%d]",ret);
  	 	WRITEMSG
		goto ErrExit;
	}

	/*登记收费明细*/
	strcpy(g_com_chrg_hst.sts,"Y");
	strcpy(g_com_chrg_hst.ac_no,tmp_ac_no2);
	strcpy(g_com_chrg_hst.tx_code,g_pub_tx.tx_code);
 	strcpy(g_com_chrg_hst.chrg_rate_code,g_com_chrg_rate.chrg_rate_code);
 	/****根据王师傅要求，改为name字段存前台户名 20100819
	strcpy(g_com_chrg_hst.name,"收费业务");
	****/
	strcpy(g_com_chrg_hst.name,tmp_ac_name);
	g_com_chrg_hst.chrg_amt=g_pub_tx.tx_amt1;
	g_com_chrg_hst.tx_amt=0.00;
	g_com_chrg_hst.tx_date=g_pub_tx.tx_date;
	g_com_chrg_hst.trace_no=g_pub_tx.trace_no;

	sprintf(acErrMsg,"JYW---%f",g_com_chrg_hst.chrg_amt);
	WRITEMSG

	ret = Com_chrg_hst_Ins(g_com_chrg_hst,g_pub_tx.reply);
	if( ret )
	{
 	 	sprintf(acErrMsg,"登记收费明细失败!ret=[%d]",ret);
  	 	WRITEMSG
     	goto ErrExit;
	}

	/*** Part For Print ***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
	strcpy(charge_type,"收费");

	/*** 取客户帐科目 ***/
	if( g_pub_tx.ct_ind[0]=='1' )
		strcpy( tmp_acc_hrt,"101");
	else if( g_pub_tx.ct_ind[0]=='2' )
	{
		/*** 取活期文件、参数 ***/
		vtcp_log("tmp_ac_no===========>>>>>>>[%s],", tmp_ac_no );
		if( pub_base_getdd_mst_parm( tmp_ac_no,1,&o_dd_mst,&o_dd_parm ) )
		{
 	 		sprintf(acErrMsg,"账号不存在,tmp_ac_no=[%s],ac_id=[%ld], \
					prdt_no=[%s]",tmp_ac_no,o_dd_mst.ac_id,o_dd_parm.prdt_no);
  	 		WRITEMSG
     		goto ErrExit;
		}

		ret = Dc_acc_rel_Sel( g_pub_tx.reply, &o_dc_acc_rel,"dc_code='%s' and \
							  acc_hrt[1]='2'", o_dd_parm.dc_code );
		if( ret==100 )
		{
 	 		sprintf(acErrMsg,"会计代码不存在==[%s][%s]", \
					g_pub_tx.dc_code,o_dc_acc_rel.acc_hrt);
  	 		WRITEMSG
			strcpy(g_pub_tx.reply,"O169");
     		goto ErrExit;
		}
		else if( ret )
		{
 	 		sprintf(acErrMsg," 查询会计科目表失败!ret=[%d][%s][%s]", \
					ret,g_pub_tx.dc_code,o_dc_acc_rel.acc_hrt );
  		 	WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
     		goto ErrExit;
		}

		strcpy( tmp_acc_hrt,o_dc_acc_rel.acc_hrt );
	}
	else
	{
 	 	sprintf(acErrMsg,"现转类型不正确======[%s]", g_pub_tx.ct_ind );
  	 	WRITEMSG
		strcpy(g_pub_tx.reply,"P156");
     	goto ErrExit;
	}

	/*** 扣款帐号|扣款人名称|收费金额|收费方式|业务类型|收费名称|合计金额 ***/
	fprintf(fp,"SFPT%s|%s|%.2lf|%s|%s|%s|%.2lf|%s|\n",tmp_ac_no2,tmp_ac_name,g_pub_tx.tx_amt1,ct_name,charge_type,g_com_chrg_rate.name,g_pub_tx.tx_amt1,tmp_acc_hrt);

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/*** Print Complete ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"收费子交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"收费子交易失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
