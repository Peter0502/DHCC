/***************************************************************
* 文 件 名:     spD001.c
* 功能描述：	开空介质
*
* 作    者:     jack
* 完成日期：    2004年1月6日
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
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_id_code_rel_c.h"

spD001()
{
	/*** 初始化全局变量 ***/
	double amt;
	pub_base_sysinit();
	set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140110**/
	get_zd_data("1161",g_pub_tx.mdm_code);	
	get_zd_data("1162",g_pub_tx.crd_no);	
	get_zd_data("1164",g_pub_tx.id_type);	
	get_zd_data("1165",g_pub_tx.id_no);	
	get_zd_data("1166",g_pub_tx.name);
	vtcp_log("[%s][%d] beg_note =[%s]\n",__FILE__,__LINE__,g_pub_tx.beg_note_no);	
	get_zd_data("1168",g_pub_tx.beg_note_no);	
	vtcp_log("[%s][%d] beg_note =[%s]\n",__FILE__,__LINE__,g_pub_tx.beg_note_no);	
	get_zd_data("1151",g_pub_tx.draw_uncon_yn);
	get_zd_data("1152",g_pub_tx.draw_pwd_yn);
	get_zd_data("1153",g_pub_tx.qry_pwd);
	get_zd_data("1154",g_pub_tx.draw_pwd);
	get_zd_data("1155",g_pub_tx.draw_id_yn);
	get_zd_data("1157",g_pub_tx.draw_seal_yn);
	get_zd_data("1159",g_pub_tx.pwd_mach_yn);		/*支付密码*/
	get_zd_double("102F",&amt);
    vtcp_log("grt------lj[%.2lf]",amt);

	strcpy(g_pub_tx.brf,"开户");

	pub_base_strpack(g_pub_tx.crd_no);
	pub_base_strpack(g_pub_tx.beg_note_no);
    vtcp_log("wyb--[%s][%s][%s]",g_pub_tx.mdm_code,g_pub_tx.id_type,g_pub_tx.beg_note_no);
	get_zd_data("0191",g_pub_tx.using_ind);
    vtcp_log("wyb--[%s]",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
	g_pub_tx.svc_ind=1002;
	if(g_pub_tx.id_type[0]=='Z')/**验资户 2007-6-8 9:33**/
  {
  	g_reply_int=pub_acc_crt_cif_yzh(g_pub_tx.id_type);
  	if(g_reply_int)
  	{
  		sprintf(acErrMsg,"开介质失败![%d][%s]",g_reply_int,g_pub_tx.reply);
		  WRITEMSG
		  goto ErrExit;
  	}
  	
  }
	g_reply_int=pub_acct_opn_cif_mdm();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"开介质失败![%d][%s]",g_reply_int,g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*** 输出变量 ***/
	vtcp_log("[%s]",g_pub_tx.ac_no);
	set_zd_data("1167",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"开介质成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"开介质失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int pub_acc_crt_cif_yzh(char * id_type)
{
		long cif_no,ret=0;
	static struct cif_alias_inf_c    cif_alias_inf;
	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_id_code_rel_c  cif_id_code_rel;	
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	strcpy(cif_basic_inf.type,"2");		  /* 客户类型 */
	strcpy(cif_id_code_rel.id_type,id_type);/* 证件类型 */
	strcpy(cif_basic_inf.name,g_pub_tx.name);		/* 客户名称 */
  strcpy(cif_basic_inf.local,"1");			/* 本地标志1-本地2-异地3-国外 */
	strcpy(cif_basic_inf.poten,"Y");			/* 潜在标志Y-潜在 N-非潜在*/
	strcpy(cif_basic_inf.shareholder_ind,"N");	/* 本行股东Y-是 N-否 */
	strcpy(cif_basic_inf.lvl,"4");
	ret = pub_cif_CreYzh_cif_no( &cif_no );
	if ( ret )
		{
			sprintf(acErrMsg,"生成客户id失败!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
		return 1;
		}
		cif_basic_inf.cif_no = cif_no ;
	  cif_id_code_rel.cif_no = cif_no;
	  sprintf(cif_id_code_rel.id_no,"%ld",cif_no);
	  sprintf(g_pub_tx.id_no,"%ld",cif_no);
	  ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
	  if ( ret )
	 {
		sprintf(acErrMsg,"登记客户证件与客户号对照表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C004");
		return 1;
	 }
	 /* 登记客户基本信息表 */
	cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* 建立日期 */
	cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* 建立时间 */
	strcpy( cif_basic_inf.sts , "1" ) ;         /* 客户状态：1正常 2注销 */
	cif_basic_inf.cif_seqn = 0 ;                /* 客户序号 */
	ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C005");
		return 1;
	}
	strcpy(cif_alias_inf.alias,cif_basic_inf.name);		/* 客户名称 */
	cif_alias_inf.cif_no = cif_basic_inf.cif_no ; 		/* 客 户 号 */
	cif_alias_inf.alias_seqn = 1 ;                		/* 别名序号 */
	ret=Cif_alias_inf_Ins(cif_alias_inf,g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg,"登记客户别名信息表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C083");
		return 1;
	}
	/* 登记交易流水 */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		return 1;
	} 
	return 0;
}
