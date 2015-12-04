/*************************************************************
* 文 件 名: gD501.c
* 功能描述: 晋中中医院POS收入旬末划款 
* 作    者: 刘增盈
* 完成日期: 20150114
* insert into com_eod_exec values (126, 'D501', 'gD501', '中医院POS收入划转', '1111111111', '3', '0', '99', '99', '0', 'Y', 'Y', 'N', '2', 'Y');
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if(ret) {strcpy( g_pub_tx.reply,"AT03" ); sprintf(acErrMsg,"SQL ERR [%d]",ret); WRITEMSG goto ErrExit;}
#define  EXTERN
#define  MRCHNO  "059175080620001" /*晋中市中医院商户编号*/
#define  LIMITAMT 5000.00  /*内部账户保留金额*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "march_info_c.h"
#include "com_sys_parm_c.h"

gD501()
{
	int ret=0;
	double amt=0.00;
	struct com_sys_parm_c com_sys_parm_c;
	struct dd_mst_c dd_mst_c;
	struct dd_mst_c dd_mst_s;
	struct in_mst_c in_mst_c;	
	struct mdm_ac_rel_c  smdm_ac_rel;
	struct mdm_ac_rel_c  smdm_ac_rel2;
	struct march_info_c  smarch_info;
	
	memset(&dd_mst_c,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_mst_s,0x00,sizeof(struct dd_mst_c) );
	memset(&in_mst_c,0x00,sizeof(struct in_mst_c) );
	memset(&com_sys_parm_c, 0x00, sizeof(struct com_sys_parm_c));
	memset(&smdm_ac_rel, 0x00, sizeof(smdm_ac_rel));
	memset(&smdm_ac_rel2, 0x00, sizeof(smdm_ac_rel2));
	memset(&smarch_info, 0x00, sizeof(smarch_info));

	ret = sql_begin();	/* 打开事务 */
	MYSQLERR

	/* 查询系统参数表 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply, &com_sys_parm_c, "1=1");
	MYSQLERR
	
	g_pub_tx.tx_date = com_sys_parm_c.sys_date;
	strcpy(g_pub_tx.cur_no, RMB);
	strcpy(g_pub_tx.tx_code, "D501");
	strcpy(g_pub_tx.sub_tx_code, "D501");
	g_pub_tx.trace_cnt = 0;
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.tel, "9999");
	
	/*取主机流水号*/
	if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
	{
		sprintf(acErrMsg, "取主机流水号出错! trace_no=[%ld]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("晋中市中医院POS收入旬末划款开始");
	/*读表数据*/
	strncpy(smarch_info.mrchno,MRCHNO,sizeof(smarch_info.mrchno)-1);
	ret = March_info_Sel(g_pub_tx.reply,&smarch_info,"mrchno = '%s'",smarch_info.mrchno);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"查询商户管理表出错,ret[%d],商户编号[%s]",ret,smarch_info.mrchno);
		WRITEMSG
		goto ErrExit;
	}
  
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&smdm_ac_rel," ac_no='%s' ", smarch_info.mrchacno_ls);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"查询介质账户关系表出错,ret[%d],账号[%s]",ret,smarch_info.mrchacno_ls);
		WRITEMSG
		goto ErrExit;
	}

	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_s," ac_id='%ld' ", smdm_ac_rel.ac_id);
	if ( ret )
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"查询过渡账户主表出错,ret[%d],账户ID[%s]",ret,smdm_ac_rel.ac_id);
		WRITEMSG
		goto ErrExit;
	}

	if(dd_mst_s.ac_sts[0] != '1')
	{
		sprintf(acErrMsg,"过渡账户状态不正常,状态为[%s]", dd_mst_s.ac_sts);
		WRITEMSG
		goto ErrExit;
	}
	if(dd_mst_s.hold_sts[0]=='1')
	{
		sprintf(acErrMsg, "过渡账户为完全冻结状态!");
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&smdm_ac_rel2," ac_no='%s' ", smarch_info.mrchacno);
	if ( ret )                                                                     
	{
		strcpy( g_pub_tx.reply,"AT06" );
		sprintf(acErrMsg,"查询介质账户关系表出错,ret[%d],账号[%s]",ret,smarch_info.mrchacno);
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_c, " ac_id='%ld' ",smdm_ac_rel2.ac_id);
	if ( ret )
	{
		sprintf(acErrMsg,"查询活期主表出错,ret[%d],账户ID[%d]",ret,smdm_ac_rel2.ac_id);
		WRITEMSG
		goto ErrExit;
	}
	
	if(dd_mst_c.ac_sts[0] != '1')
	{
		sprintf(acErrMsg,"对公账户状态不正常,状态为[%s]", dd_mst_c.ac_sts);
		WRITEMSG
		goto ErrExit;
	}
	if(dd_mst_c.hold_sts[0]=='1')
	{
		sprintf(acErrMsg, "对公账户为完全冻结状态!");
		WRITEMSG
		goto ErrExit;
	}
	
	if( pub_base_CompDblVal(dd_mst_s.bal,LIMITAMT) != 1 )
	{
		sprintf(acErrMsg,"过渡账户金额达不到保留限额,不需划款!保留限额[%.2f],账户余额[%.2f]",LIMITAMT,dd_mst_s.bal);
		WRITEMSG
		goto OkExit;
	}
	  
	vtcp_log("开始过渡账户转出记账");
	strcpy(g_pub_tx.sub_tx_code,"D003");
	strcpy(g_pub_tx.tx_br_no,dd_mst_s.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,dd_mst_s.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_no,smarch_info.mrchacno_ls);
	g_pub_tx.ac_id=dd_mst_s.ac_id;
	g_pub_tx.ac_seqn=1;
	strcpy( g_pub_tx.ac_id_type,"1" );
	g_pub_tx.tx_amt1=dd_mst_s.bal-LIMITAMT;
	vtcp_log("[%s][%d]本次划转金额[%.2f]",__FILE__,__LINE__,dd_mst_s.bal-LIMITAMT);
	strcpy(g_pub_tx.add_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
	g_pub_tx.svc_ind=1001; /**处理代码***/
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.brf,"中医院POS划转转出");
	strcpy( g_pub_tx.prt_ind,"0" );
	
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}
	set_zd_data( "102J", "01" );
	set_zd_data( "102K", "2" );
	set_zd_double( "102F", g_pub_tx.tx_amt1 );
		
	if ( pubf_acct_proc("D003") )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}

	vtcp_log("开始对公账户转入记账");
	strcpy(g_pub_tx.sub_tx_code,"D099");
	strcpy(g_pub_tx.ac_no,smarch_info.mrchacno);
	strcpy(g_pub_tx.tx_br_no,smdm_ac_rel2.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,smdm_ac_rel2.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	g_pub_tx.ac_id=smdm_ac_rel2.ac_id;
	g_pub_tx.tx_amt1=dd_mst_s.bal-LIMITAMT;
	g_pub_tx.ac_seqn=1;
	strcpy( g_pub_tx.ac_id_type,"1" );
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy( g_pub_tx.prt_ind,"0" );
	strcpy(g_pub_tx.add_ind,"1"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
	g_pub_tx.svc_ind=1001; /**处理代码***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.brf,"中医院POS划转转入");
	
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_double("1013",g_pub_tx.tx_amt1);

	if ( pubf_acct_proc("D099") )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	sql_commit();		/*--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data("0120", "0000");
	sprintf(acErrMsg, "晋中市总医院POS收入旬末划款成功![%s]", g_pub_tx.reply);
	WRITEMSG
	return 0;
	
ErrExit:
	sql_rollback();  /*--事务回滚--*/
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0') 
	{
		strcpy(g_pub_tx.reply, "G009");
	}
	set_zd_data("0120", g_pub_tx.reply);
	sprintf(acErrMsg, "晋中市总医院POS收入旬末划款失败![%s]", g_pub_tx.reply);
	WRITEMSG
	return 1;
}