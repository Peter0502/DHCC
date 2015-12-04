/***************************************************************
* 文 件 名: spD214.c
* 功能描述：对于写扣划登记部
* 作    者: LiuHuafeng
* 完成日期：2008-11-21 13:50:53
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
* insert into tx_sub_def values('D214','扣划业务写登记簿','spD214','0','0');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "kh_reg_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"
spD214()
{
	struct pub_tx out_pub_tx;              /*通用记帐结构*/
	struct pub_tx in_pub_tx;              /*通用记帐结构*/
	struct prdt_ac_id_c sPrdt_ac_id;
	struct mdm_ac_rel_c sMdm_ac_rel;
	int             ret = 0;
	char            tmpname[100], intst_type[17], ct_name[5], term[3], term_type[3], memo[5], id_name[21];
	char            br_name[31], br_tele[31];
	FILE           *fp;
	char            acc_no[ACC_NO_LVL3_LEN+1];
	double          val = 0.00, dealval = 0.00;
	char            kinbr[6];
	char		ctemp[101];
	char		ctemp1[1205];
	char            cFlws[37]; /** 法律文书**add by chenchao  2010-7-4 0:09:41   ********/
	char		cZfry[9]; /**执法人员姓名**add by chenchao  2010-7-4 0:09:41 ********/
	char		cGwz[19];  /**公务证号码**add by chenchao  2010-7-4 0:09:41  ********/
	char		cZfry1[9]; /**执法人员姓名**add by chenchao  2010-7-4 0:09:41********/
	char		cGwz1[19];  /**公务证号码**add by chenchao  2010-7-4 0:09:41 ********/
	double dIntst=0.0;
	double dTax=0.0;

	struct kh_reg_c g_kh_reg;
	
	memset(kinbr,0,sizeof(kinbr));
	memset(ctemp,0,sizeof(ctemp));
	memset(cFlws,0,sizeof(cFlws));
	memset(cZfry,0,sizeof(cZfry));
	memset(cGwz,0,sizeof(cGwz));
	memset(ctemp1,0,sizeof(ctemp1));
	memset(cZfry1,0,sizeof(cZfry1));
	memset(cGwz1,0,sizeof(cGwz1));
	
	memset(&sMdm_ac_rel , 0 , sizeof(sMdm_ac_rel));
	memset(&sPrdt_ac_id, 0 , sizeof(sPrdt_ac_id));
	memset(&out_pub_tx, 0 , sizeof(out_pub_tx));
	memset(&in_pub_tx , 0 , sizeof(in_pub_tx));
	memset(&g_kh_reg ,0 , sizeof(g_kh_reg));
	g_reply_int = 0;
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("102Y", out_pub_tx.brf);	/* 摘要内容 */
	pub_base_strpack(out_pub_tx.brf);
	if(memcmp(out_pub_tx.brf,"扣划业务",8)!=0)
	{
		goto OkExit;
	}
	get_zd_data("0030", kinbr);
	/* 得到转出账户信息 */
	get_zd_data("1021", out_pub_tx.ac_no);
	pub_base_old_acno(out_pub_tx.ac_no);
	get_zd_int("1022", &out_pub_tx.ac_seqn);
	get_zd_data("1023", out_pub_tx.note_type);
	get_zd_data("1024", out_pub_tx.beg_note_no);
	get_zd_data("1024", out_pub_tx.end_note_no);
	get_zd_data("1025", out_pub_tx.draw_uncon_yn);
	get_zd_data("1026", out_pub_tx.draw_pwd_yn);
	get_zd_data("1027", out_pub_tx.qry_pwd);
	get_zd_data("1028", out_pub_tx.draw_pwd);
	get_zd_data("1029", out_pub_tx.draw_id_yn);
	get_zd_data("102G", out_pub_tx.id_type);
	get_zd_data("102A", out_pub_tx.id_no);
	get_zd_data("102B", out_pub_tx.draw_seal_yn);
	get_zd_data("102C", out_pub_tx.pwd_mach_yn);
	get_zd_data("102D", out_pub_tx.draw_mach);
	get_zd_long("102E", &out_pub_tx.mach_date);
	get_zd_double("102I", &out_pub_tx.tx_amt1);
	get_zd_double("102F", &g_pub_tx.tx_amt1);
	if(pub_base_CompDblVal(g_pub_tx.tx_amt1 , 0.0)==0)
	{
		/* 如果是销户，其交易金额=0 那么取转出账户余额 */
		g_pub_tx.tx_amt1 = out_pub_tx.tx_amt1;
	}

	get_zd_data("102X", g_pub_intst.edu_ind);	/**教育证明 1有**/
	get_zd_data("102J", out_pub_tx.cur_no);
	get_zd_data("102K", out_pub_tx.ct_ind);
	get_zd_data("102O", out_pub_tx.prt_ind);	/* 是否打折标志 */
	/* 得到转入账户信息 */
	
	get_zd_data("1011", in_pub_tx.ac_no);
	pub_base_old_acno(in_pub_tx.ac_no);

	get_zd_int("1012", &in_pub_tx.ac_seqn);
	get_zd_data("101A", in_pub_tx.cur_no);
	get_zd_data("1014", in_pub_tx.note_type);
	get_zd_data("1015", in_pub_tx.beg_note_no);
	get_zd_data("1015", in_pub_tx.end_note_no);
	get_zd_data("101B", in_pub_tx.ct_ind);
	get_zd_data("1016", in_pub_tx.brf);      /***摘要***/
	
	get_zd_data("1230",ctemp1);
        
        pub_base_cut_str(ctemp1,cFlws,"|",1);
        pub_base_cut_str(ctemp1,cZfry,"|",2);
        pub_base_cut_str(ctemp1,cGwz,"|",3);
        pub_base_cut_str(ctemp1,cZfry1,"|",4);
        pub_base_cut_str(ctemp1,cGwz1,"|",5);
	
		
	memcpy(g_kh_reg.br_no,g_pub_tx.tx_br_no,sizeof(g_kh_reg.br_no)-1);	      
	g_kh_reg.out_ac_id	 = out_pub_tx.ac_id;
	g_kh_reg.out_ac_seqn = out_pub_tx.ac_seqn;
	g_kh_reg.tx_date	   = g_pub_tx.tx_date;
	g_kh_reg.kh_amt	     = g_pub_tx.tx_amt1;
	get_zd_double("102P",&dIntst);
	get_zd_double("102Q",&dTax);
	g_kh_reg.intst=dIntst-dTax;
	g_kh_reg.in_ac_id	   = in_pub_tx.ac_id;
	g_kh_reg.in_ac_seqn	 = in_pub_tx.ac_seqn;
	g_kh_reg.trace_no	   = g_pub_tx.trace_no;
	g_kh_reg.tx_time	   = g_pub_tx.tx_time;
	g_kh_reg.auth_code   = g_pub_tx.auth_code;

	strncpy(g_kh_reg.out_ac_no   ,out_pub_tx.ac_no , sizeof(g_kh_reg.out_ac_no  )-1);  
	strncpy(g_kh_reg.in_ac_no	   ,in_pub_tx.ac_no , sizeof(g_kh_reg.in_ac_no	  )-1);
	strncpy(g_kh_reg.tel	       ,g_pub_tx.tel , sizeof(g_kh_reg.tel	      )-1);  
	strncpy(g_kh_reg.auth_tel	   ,g_pub_tx.auth , sizeof(g_kh_reg.auth_tel	  )-1);
	strncpy(g_kh_reg.brf,g_pub_tx.brf,sizeof(g_kh_reg.brf)-1);	        
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s' ",out_pub_tx.ac_no);
	if(ret)
	{
		goto ErrExit;
	}
	if(memcmp(sMdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1)!=0) /***add by chenchao 20100724 只能开户做扣划***/
	{
		sprintf(acErrMsg,"该机构不允许做的业务[%s]", sMdm_ac_rel.opn_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D245");
		goto ErrExit;
	}

	/**if(sMdm_ac_rel.ac_seqn!=9999 && out_pub_tx.ac_seqn==0) out_pub_tx.ac_seqn=1;**/
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%ld",sMdm_ac_rel.ac_id,out_pub_tx.ac_seqn);
	if(ret)
	{
		goto ErrExit;
	}
	strncpy(g_kh_reg.out_prdt_no , sPrdt_ac_id.prdt_no , sizeof(g_kh_reg.out_prdt_no)-1); 
	strncpy(g_kh_reg.out_ac_id_type , sPrdt_ac_id.ac_id_type , sizeof(g_kh_reg.out_ac_id_type)-1);
	
	get_zd_long("1261",&g_kh_reg.tt_no);   /***add by zhanghongbo 20090818***/
	/***add by chenchao ****/
	memcpy(g_kh_reg.low_bo,   cFlws ,sizeof(g_kh_reg.low_bo)-1);
	memcpy(g_kh_reg.zf_name1, cZfry ,sizeof(g_kh_reg.zf_name1)-1);
	memcpy(g_kh_reg.pub_no1,  cGwz  ,sizeof(g_kh_reg.pub_no1)-1);
	memcpy(g_kh_reg.zf_name2, cZfry1,sizeof(g_kh_reg.zf_name2)-1);
	memcpy(g_kh_reg.pub_no2,  cGwz1 ,sizeof(g_kh_reg.pub_no2)-1);
	g_kh_reg.fl_print[0]='0';
	g_kh_reg.flag_intst[0]='0';
	ret=Kh_reg_Ins(g_kh_reg,g_pub_tx.reply);
	if(ret)
	{
		Kh_reg_Debug(&g_kh_reg);
		goto ErrExit;
	}
	/*****销户后不更新回正常了 如果在这里更新为正常是为了能再做销户取扣划利息可以去做支取扣划利息交易
	    shangyongchao  for JZSYYH 2011/11/12 9:26:18 
	ret=sql_execute("update  mdm_ac_rel set note_sts='0' where ac_no='%s' and note_sts='*'", g_kh_reg.out_ac_no);
	if ( ret )
	{
		sprintf(acErrMsg,"更新mdm_ac_rel状态错误[%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O012");
		goto ErrExit;
	}
	ret=sql_execute("update  td_mst  set ac_sts='0' where ac_id=%d and ac_sts='*' and ac_seqn=%d", sMdm_ac_rel.ac_id,g_kh_reg.out_ac_seqn);
	if ( ret )
	{
		sprintf(acErrMsg,"更新td_mst状态错误[%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O012");
		goto ErrExit;
	}
	*****/

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "登记扣划表成功![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "登记扣划表失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}
