/*************************************************************
* 文 件 名: gD995.c
* 功能描述：生成MIS接口文件
*
* 作    者: 
* 完成日期: 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "trace_log_bk_c.h"
#include "tx_sub_def_c.h"
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "mo_stok_rebuy_prot_c.h"
#include "mo_hold_c.h"
#include "ln_pay_pln_c.h"
#include "mo_discnt_c.h"
#include "dc_acc_rel_c.h"
#include "ln_reg_c.h"
#include "com_sys_parm_c.h"
#include "mo_po_co_c.h"
#include "cif_basic_inf_c.h"
#include "cif_rel_chg_hst_c.h"
#include "march_mst_hst_c.h"
#include "protect_note_c.h"

gD995()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm;

vtcp_log("MIS接口程序开始执行");
	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if( ret )
	{ WRITEMSG return 1; }
	g_pub_tx.tx_date=com_sys_parm.lst_date;

vtcp_log("begin create_boco_1 存款账号信息表");
	ret=create_boco_1();	/*** 存款账号信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_2 综合业务网点信息表");
	ret=create_boco_2();	/*** 综合业务网点信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_3 存款类型编码信息表");
	ret=create_boco_3();	/*** 存款类型编码信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_4 卡信息表");
	ret=create_boco_4();	/*** 卡信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_5 账号交易信息表");
	ret=create_boco_5();	/*** 账号交易信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_6 借据信息表");
	ret=create_boco_6();	/*** 借据信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_7 借据补充信息");
	ret=create_boco_7(g_pub_tx.tx_date);	/*** 借据补充信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_8 贷款产品参数");
	ret=create_boco_8();	/*** 贷款产品参数 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_9 借据交易信息");
	ret=create_boco_9();	/*** 借据交易信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_10 开出汇票信息");
	ret=create_boco_10();	/*** 开出汇票信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_11 汇票兑付，垫款信息");
	ret=create_boco_11();	/*** 汇票兑付，垫款信息 ***/ 
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_13 到单信息");
	ret=create_boco_13();	/*** 到单信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_14 到单付款信息");
	ret=create_boco_14();	/*** 到单付款信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_15 到单垫款信息");
	ret=create_boco_15();	/*** 到单垫款信息 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_16 科目表");
	ret=create_boco_16();	/*** 科目表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_17 综合业务系统总帐表");
	ret=create_boco_17();	/*** 综合业务系统总帐表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_18 存放同业数据表");
	ret=create_boco_18();	/*** 存放同业数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_19 同业存放数据表");
	ret=create_boco_19();	/*** 同业存放数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_20 其他应收账款数据表");
	ret=create_boco_20();	/*** 其他应收账款数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_21 向央行借款数据表");
	ret=create_boco_21();	/*** 向央行借款数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_22 应付账款数据表数据表");
	ret=create_boco_22();	/*** 应付账款数据表数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_23 其他应付账款数据表数据表");
	ret=create_boco_23();	/*** 其他应付账款数据表数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_24 债券逆回购到期数据表(借出)");
	ret=create_boco_24();	/*** 债券逆回购到期数据表(借出) ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_25 债券正回购到期数据表(借入)");
	ret=create_boco_25();	/*** 债券正回购到期数据表(借入) ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_26 大额资金进程明细数据表");
	ret=create_boco_26();	/*** 大额资金进程明细数据表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_27 主要业务状况日报表");
	ret=create_boco_27(g_pub_tx.tx_date);	/*** 主要业务状况日报表 ***/
	if( ret )
		goto ErrExit;
		
vtcp_log("begin create_boco_28 客户基本信息表");
	ret=create_boco_28(g_pub_tx.tx_date);	/*** 客户基本信息表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_29 客户关系变更历史表");
	ret=create_boco_29(g_pub_tx.tx_date);	/*** 客户关系变更历史表 ***/
	if( ret )
		goto ErrExit;

vtcp_log("begin create_boco_30 账号关系对照表");
	ret=create_boco_30(g_pub_tx.tx_date);	/*** 账号关系对照表 ***/
	if( ret )
		goto ErrExit;
								
vtcp_log("MIS借口程序成功完成");

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	strcpy(g_pub_tx.reply,"P400");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int create_boco_1()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0001.mis",getenv("MISDIR"),g_pub_tx.tx_date);
vtcp_log("filename=[%s]",filename);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_SaveAcntReg|14|N|\n");
	fprintf(fp,"CstCode|Acnt|NodeNo|InnerFlag|EAcnt|StartDate|DuteDate|Subject|SavBalance|Validity|certificateId|SaveKind|extendflag|saveRate\n");

	ret=Dd_mst_Dec_Sel(g_pub_tx.reply," opn_date=%d ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%d",g_dd_mst.ac_id);
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_mdm_ac_rel.ac_no);
		pub_base_strpack(g_dd_mst.opn_br_no);
		pub_base_strpack(g_mdm_ac_rel.id_no);

		fprintf(fp,"A|%d|%d%04d|%s|1||%d|0||%.2f|0|%s|%s|0|%lf\n",
			g_dd_mst.cif_no,g_dd_mst.ac_id,g_dd_mst.ac_seqn,
			g_dd_mst.opn_br_no,g_pub_tx.tx_date,
			g_dd_mst.bal,g_mdm_ac_rel.id_no,g_dd_mst.prdt_no,
			g_dd_mst.rate);
		count++;
	}
	Dd_mst_Clo_Sel();

	ret=Td_mst_Dec_Sel(g_pub_tx.reply," opn_date=%d ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret=Td_mst_Fet_Sel(&g_td_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%d",g_td_mst.ac_id);
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_mdm_ac_rel.ac_no);
		pub_base_strpack(g_td_mst.opn_br_no);
		pub_base_strpack(g_mdm_ac_rel.id_no);
		fprintf(fp,"A|%d|%d%04d|%s|1||%d|%d||%.2f|0|%s|%s|0|%lf\n",
			g_td_mst.cif_no,g_td_mst.ac_id,g_td_mst.ac_seqn,
			g_td_mst.opn_br_no,g_pub_tx.tx_date,g_td_mst.mtr_date,
			g_td_mst.bal,g_mdm_ac_rel.id_no,g_td_mst.prdt_no,
			g_td_mst.rate);
		count++;
	}
	Td_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_2()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct com_branch_c com_branch;
	struct com_branch_c com_branch1;

	sprintf(filename,"%s/D%8d_0002.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_NODEINFO|5|A|\n");
	fprintf(fp,"INNTERCODE|INSTNAME|NodeNo|NODENAME|NOTES\n");

	ret=Com_branch_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&com_branch,0x00,sizeof(struct com_branch_c));
		memset(&com_branch1,0x00,sizeof(struct com_branch_c));
		ret=Com_branch_Fet_Sel(&com_branch,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Com_branch_Sel1(g_pub_tx.reply,&com_branch1,"br_no='%s' ",com_branch.up_br_no);
		if( ret )
		{	
			vtcp_log("com_branch.up_br_no=[%s][%d]",com_branch.up_br_no,ret);
			WRITEMSG return 1; 
		}

		pub_base_strpack(com_branch1.br_name);	
		pub_base_strpack(com_branch.br_name);	
		fprintf(fp,"A|%s|%s|%s|%s|\n",
			com_branch.br_no,com_branch1.br_name,com_branch.br_no,
			com_branch.br_name);
		count++;
	}
	Com_branch_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_3()
{
	int	ret=0;
	char filename[100],ac_type[2];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0003.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_SAVEKINDINFO|4|A|\n");
	fprintf(fp,"SaveKind|SaveKindName|CurKind|DepostKind\n");

	ret=Dd_parm_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
		ret=Dd_parm_Fet_Sel(&g_dd_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_dd_parm.title);
		
		if( g_dd_parm.ac_type[0]=='5' )			/**个人户**/
			strcpy(ac_type,"5");
		else if( g_dd_parm.ac_type[0]=='1' )		/**基本户**/
			strcpy(ac_type,"0");
		else if( g_dd_parm.ac_type[0]=='2' )		/**一般户**/
			strcpy(ac_type,"1");
		else if( g_dd_parm.ac_type[0]=='3' )		/**专用户**/
			strcpy(ac_type,"3");
		else if( g_dd_parm.ac_type[0]=='4' )		/**临时户**/
			strcpy(ac_type,"2");
		else  
			strcpy(ac_type,"4");
						
		if( g_dd_parm.cif_type[0]=='1' || g_dd_parm.cif_type[0]=='5' ) /*储蓄户*/
		{
			fprintf(fp,"A|1%s|%s|%s|5\n",
				g_dd_parm.prdt_no,g_dd_parm.title,g_dd_parm.cur_no);
		}else{
			fprintf(fp,"A|3%s|%s|%s|5\n",
				g_dd_parm.prdt_no,g_dd_parm.title,g_dd_parm.cur_no);						
		}
		count++;
	}
	Dd_parm_Clo_Sel();

	ret=Td_parm_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
		ret=Td_parm_Fet_Sel(&g_td_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_td_parm.title);
		if( !strcmp(g_td_parm.prdt_no,"241") )
		{
			fprintf(fp,"A|5%s|%s|%s|5\n",
				g_td_parm.prdt_no,g_td_parm.title,g_td_parm.cur_no);
		}else{
			if( g_td_parm.cif_type[0]=='1' || g_td_parm.cif_type[0]=='5' ) /*储蓄户*/
			{
				fprintf(fp,"A|2%s|%s|%s|5\n",
					g_td_parm.prdt_no,g_td_parm.title,g_td_parm.cur_no);
			}else{
				fprintf(fp,"A|4%s|%s|%s|1\n",
					g_td_parm.prdt_no,g_td_parm.title,g_td_parm.cur_no);						
			}		
		}
		count++;
	}
	Td_parm_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_4()
{
	int	ret=0;
	char filename[100],fkbz[1];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0004.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_PCstCardInfo|18|N|\n");
	fprintf(fp,"CardNo|cstcode|ACNT|MainAssistCardFlag|MainCardNo|CardStatus|AssistCardFlag|IsPayPoundage|CardLimit|CardEndDate|CardStartDate|ExchangeCardDate|CardType|Proper1|Proper2|Proper3|Proper4|certificateId\n");

	ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply," beg_date=%d and ac_no[1,9]='940071733' ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		ret=Mdm_ac_rel_Fet_Sel(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_mdm_ac_rel.ac_no);	
		pub_base_strpack(g_mdm_ac_rel.id_no);	
		if( g_mdm_ac_rel.main_ind[0]=='1' )
			strcpy(fkbz,"0");
		else
			strcpy(fkbz,"1");

		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret==100 )	g_dd_mst.cif_no=0;
		else if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%ld|%d|%s|01|%s|1|||%d|0|10|||||%s|%d|\n",
			g_mdm_ac_rel.ac_no,g_dd_mst.cif_no,
			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.main_ind,
			fkbz,g_pub_tx.tx_date,g_mdm_ac_rel.id_no,g_dd_mst.cif_no);
		count++;
	}
	Mdm_ac_rel_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_5()
{
	int	ret=0;
	char filename[100],jylx[2],jyfs[2],zjyw[2];
	FILE *fp;
	int	count=0;
	struct trace_log_bk_c	trace_log;
	struct tx_sub_def_c	tx_sub_def;
	struct mo_hold_c	mo_hold;
	struct march_mst_hst_c	S_march_mst_hst;

	sprintf(filename,"%s/D%8d_0005.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_PCstBusiness|10|N|\n");
	fprintf(fp,"CardNo|BargainingFlow|BargainingDate|BargainingSum|curKind|BargainingType|BargainingMeans|MidTrnType|Notes|Acidseqn\n");

	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," sts='0' and no_show!='1' ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		memset(&tx_sub_def,0x00,sizeof(struct tx_sub_def_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		strcpy(jylx,"");
		strcpy(jyfs,"3");		/*** 默认柜台 ***/
		strcpy(zjyw,"");
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Tx_sub_def_Sel(g_pub_tx.reply,&tx_sub_def,"sub_tx_code='%s'",trace_log.sub_tx_code);
		if( ret==100 )
		{ 
			vtcp_log("[%d]sub_tx_code=[%s][%d]",trace_log.trace_no,trace_log.sub_tx_code,ret);
			continue;
		}else if( ret )
		{ 
			vtcp_log("[%d]sub_tx_code=[%s][%d]",trace_log.trace_no,trace_log.sub_tx_code,ret);
			return 1;
		}
		/** 冻结解冻交易 **/
		if( tx_sub_def.acct_ind[0]=='0' && strcmp(trace_log.tx_code,"2406") )
			continue;
		/** 现金及内部账 ****/
		if( trace_log.ac_id>899999999 || trace_log.ac_id==0 )
			continue;
		/** 排除所有产品号为'3'起的产品(贷款) **/
		if( trace_log.prdt_no[0]=='3' && trace_log.svc_ind!=1001 )
			continue;		
		/** 贷款 不能全排除**/		
		if( trace_log.sub_tx_code[0]=='L' && trace_log.svc_ind!=1001 )
			continue;

		if( trace_log.ct_ind[0]=='1' )
		{
			if( trace_log.add_ind[0]=='1' )
				strcpy(jylx,"2");
			else
				strcpy(jylx,"1");
		}
		else if( trace_log.ct_ind[0]=='2' )
		{
			if( trace_log.add_ind[0]=='1' )
				strcpy(jylx,"3");
			else
				strcpy(jylx,"4");
		}
		
		if( strcmp(trace_log.tel,"0000") )
			strcpy(jyfs,"4");			/*** 自动 ***/

		if( !strcmp(trace_log.tx_code,"2406") )		/** 冻结、解冻 **/
		{
			memset(&mo_hold,0x00,sizeof(struct mo_hold_c));
			ret=Mo_hold_Sel(g_pub_tx.reply,&mo_hold,"wrk_date=%d and trace_no=%d",trace_log.tx_date,trace_log.trace_no);
			/**
			if( ret )
			{ WRITEMSG return 1; }
			**/

			/*** 当日冻结解冻相当于撤销 ***/
			if( mo_hold.beg_hold_date==g_pub_tx.tx_date
				&& mo_hold.hold_sts[0]=='1' )
				continue;
			if( mo_hold.hold_sts[0]=='0' )
				strcpy(jylx,"A");
			else
				strcpy(jylx,"B");
		}

		if(!strcmp(trace_log.tx_code,"2203")||!strcmp(trace_log.tx_code,"2204")||!strcmp(trace_log.tx_code,"2207")||!strcmp(trace_log.tx_code,"2208"))
			strcpy(jylx,"5");			/*** 销户取款 ***/

		if(!strcmp(trace_log.tx_code,"6203")||!strcmp(trace_log.tx_code,"6205")
			||!strcmp(trace_log.tx_code,"6204")||!strcmp(trace_log.tx_code,"6206") )
		{
			strcpy(jyfs,"1");			/*** ATM取款 ***/
			if( trace_log.sub_tx_code[0]=='S' && trace_log.sub_tx_code[1]=='F' )
			{
				strcpy(jylx,"7");			/*** ATM手续费 ***/
			}
		}
		if(!strcmp(trace_log.tx_code,"6301")||!strcmp(trace_log.tx_code,"6302")
			||!strcmp(trace_log.tx_code,"6303")
			||!strcmp(trace_log.tx_code,"6306") 
			||!strcmp(trace_log.tx_code,"6308")
			||!strcmp(trace_log.tx_code,"6310")
			||!strcmp(trace_log.tx_code,"6312")
			||!strcmp(trace_log.tx_code,"6313"))
			strcpy(jyfs,"0");			/*** POS ***/

		if( strncmp(trace_log.ac_no,"9400",4) && strlen(trace_log.ac_no)!=19
			&& strlen(trace_log.ac_no)!=16 )
			strcpy(jyfs,"#");			/*** 非卡交易 ***/

		if( strlen(jyfs)==0 )	strcpy(jyfs,"5");	/*** 其他 ***/

		if( !strcmp(trace_log.tx_code,"6602") )		/*** 批量代缴代发 ***/
		{
			if( trace_log.add_ind[0]=='1' )
				strcpy(zjyw,"2");					/*** 代发 ***/
			else
				strcpy(zjyw,"1");					/*** 代缴 ***/
		}

		pub_base_strpack(trace_log.ac_no);	
		fprintf(fp,"A|%s|%d%d|%d|%.2f|%s|%s|%s|%s|%s|%d%04d\n",
			trace_log.ac_no,trace_log.tx_date,
			trace_log.trace_no,trace_log.tx_date,trace_log.amt,
			trace_log.cur_no,jylx,jyfs,zjyw,"",trace_log.ac_id,trace_log.ac_seqn);
		count++;
	}
	Trace_log_bk_Clo_Sel();

	/* 虚拟流水(为了记录POS商户手续费) */
	ret=March_mst_hst_Dec_Sel(g_pub_tx.reply," acct_date=%ld and deal_sign='Y'  ",g_pub_tx.tx_date );
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&S_march_mst_hst,0x00,sizeof(struct march_mst_hst_c));
		ret=March_mst_hst_Fet_Sel(&S_march_mst_hst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }	
		
		pub_base_strpack(S_march_mst_hst.ac_no);
		fprintf(fp,"A|%s|%d%s|%d|%.2f|01|%s|%s|%s|%s|%d%04d\n",
			S_march_mst_hst.ac_no,S_march_mst_hst.acct_date,"888888",
			S_march_mst_hst.acct_date,S_march_mst_hst.fee_amt,
			"2","0","","虚拟--POS商户手续费存款",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);		
		count++;
		
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",S_march_mst_hst.ac_no);
		if( ret )
		{ WRITEMSG return 1; 	 }
			
		fprintf(fp,"A|%s|%d%s|%d|%.2f|01|%s|%s|%s|%s|%d%04d\n",
			S_march_mst_hst.ac_no,S_march_mst_hst.acct_date,"888888",
			S_march_mst_hst.acct_date,S_march_mst_hst.fee_amt,
			"7","0","","虚拟--POS商户手续费",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);			
		count++;
	}	
	March_mst_hst_Clo_Sel();
	
	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_6()
{
	int	ret=0;
	char filename[100],dklx[2],jxfs[2],ywzl[3],kjdm[8];
	char pjhm[21];
	FILE *fp;
	int	count=0,zqs=0,syqs=0;
	double mqje=0.00;
	struct dc_acc_rel_c	dc_acc_rel;
	struct mdm_ac_rel_c g_mdm_ac_rel1;
	struct ln_pay_pln_c ln_pay_pln;
	struct mo_discnt_c	mo_discnt;
	struct protect_note_c	protect_note;	
	struct mo_bank_acc_po_c	mo_bank_acc_po;	
	struct mo_po_co_c	mo_po_co;
	
	sprintf(filename,"%s/D%8d_0010.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_DueBill|26|A|\n");
	fprintf(fp,"DUEBILLNO|CONTRACTNO|OPTYPE|LOANTYPE|LNTY|CURKIND|LOANSUM|LOANBALANCE|EXECURATE|OVERRATE|INTETYPE|SIGNDATE|MATUREDATE|LOANCLASS|FIVECLASS|ACNTSUBJECT|LOANACNT|RETURNACNT|PERSUM|PAYPERIOD|REMAINPERIOD|OLDDUEBILLNO|FUNDSOURCE|MiddleFee|LoanUsageType|DRAFTNO\n");

	/*** 贷款主文件中只有贷款和贴现，转贴现转入账号 ***/
	/*ret=Ln_mst_Dec_Sel(g_pub_tx.reply," opn_date=%d and ac_sts!='9' and ac_sts!='*' ",g_pub_tx.tx_date);*/
	/*需要历史数据，不仅是当日开户*/
	ret=Ln_mst_Dec_Sel(g_pub_tx.reply," ac_sts!='9' and ac_sts!='*' ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_mdm_ac_rel1,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
		memset(&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c));
		memset(&mo_discnt,0x00,sizeof(struct mo_discnt_c));
		memset(&mo_po_co,0x00,sizeof(struct mo_po_co_c));
		
		ret=Ln_mst_Fet_Sel(&g_ln_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		strcpy(ywzl,"01");		/*** 默认贷款 ***/
		if( !strcmp(g_ln_mst.prdt_no,"3Z2") )	/*** 贴现 ***/
		{
			strcpy(ywzl,"02");
			ret=Mo_discnt_Sel(g_pub_tx.reply,&mo_discnt,"ac_id=%d",g_ln_mst.ac_id);
			if( ret==100 )
			{
				strcpy(pjhm,"未查到");
			}else if( ret){
				WRITEMSG return 1;
			}else{
				strcpy(pjhm,mo_discnt.pnote_no);
			}
		}
		else if( !strcmp(g_ln_mst.prdt_no,"3Z1") )	/*** 转贴现转入 ***/
		{
			strcpy(ywzl,"08");
			ret=Mo_discnt_Sel(g_pub_tx.reply,&mo_discnt,"ac_id=%d",g_ln_mst.ac_id);
			if( ret==100 )
			{
				strcpy(pjhm,"未查到");
			}else if( ret){
				WRITEMSG return 1;
			}else{
				strcpy(pjhm,mo_discnt.pnote_no);
			}
		}
		else if( !strcmp(g_ln_mst.prdt_no,"3Z3") )	/*** 承兑垫款 **/
		{
			strcpy(ywzl,"04");
			ret=Mo_bank_acc_po_Sel(g_pub_tx.reply,&mo_bank_acc_po,"pact_no='%s'",g_ln_mst.pact_no);
			if( ret==100 )
			{
				strcpy(pjhm,"未查到");
			}else if( ret){
				WRITEMSG return 1;
			}else{
				/** modify by xx 2003-12-21 ***/
				ret=Mo_po_co_Sel(g_pub_tx.reply,&mo_po_co,"pact_no='%s'",g_ln_mst.pact_no);
				if( ret==100 )
				{
					strcpy(pjhm,"未查到");
				}else if( ret){
					WRITEMSG return 1;
				}else{				                              
					strcpy(pjhm,mo_po_co.contract_no);
				}
			}
		}
		else if( !strcmp(g_ln_mst.prdt_no,"3Z4") )	/*** 包涵垫款 **/
		{
			strcpy(ywzl,"03");
			ret=Protect_note_Sel(g_pub_tx.reply,&protect_note,"pact_no='%s' and protect_sts='0' ",g_ln_mst.pact_no);
			if( ret==100 )
			{
				strcpy(pjhm,"未查到");
			}else if( ret){
				WRITEMSG return 1;
			}else{
				strcpy(pjhm,g_ln_mst.pact_no);
			}
		}else{
			strcpy(pjhm,"");
		}

		ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
		if( ret )
		{ WRITEMSG return 1; }

		/*** 取贷款账号 ***/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%d",g_ln_mst.ac_id);
		if( ret )
		{ WRITEMSG return 1; }

		/*** 取还款账号 ***/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel1,"ac_id=%d",g_ln_mst.repay_ac_id);
		if( ret==100 )	strcpy(g_mdm_ac_rel1.ac_no,"");
		else if( ret )
		{ 
			vtcp_log("ac_id=[%ld]",g_ln_mst.repay_ac_id);
			WRITEMSG return 1; 
		}

		if( g_ln_mst.intst_type[0]=='6' )
			strcpy(dklx,"1");
		else
			strcpy(dklx,"0");

		/*** 取计息方式 ***/
		if( g_ln_mst.intst_type[0]=='5' )
			strcpy(jxfs,"1");
		else if( g_ln_mst.intst_type[0]=='4' )
			strcpy(jxfs,"2");
		else if( g_ln_mst.intst_type[0]=='3' )
			strcpy(jxfs,"3");
		else if( g_ln_mst.intst_type[0]=='2' )
			strcpy(jxfs,"4");
		else 
			strcpy(jxfs,"5");

		/*** 取会计科目 ***/
		if( g_ln_mst.ac_sts[0]=='2' )
			strcpy(kjdm,"13001");
		else if( g_ln_mst.ac_sts[0]=='3' )
			strcpy(kjdm,"13002");
		else if( g_ln_mst.ac_sts[0]=='4' )
			strcpy(kjdm,"13003");
		else
		{
			ret=Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel,"dc_code='%s'",g_ln_parm.dc_code);
			if( ret )
			{ vtcp_log("dc_code=[%s]",g_ln_parm.dc_code); WRITEMSG return 1; }
			strcpy(kjdm,dc_acc_rel.acc_hrt);
		}

		/*** 取总期数，每期金额 ***/
		if( g_ln_mst.intst_type[0]=='6' )	/*** 按揭 ***/
		{
			ret=Ln_pay_pln_Sel(g_pub_tx.reply,&ln_pay_pln,"ac_id=%d",g_ln_mst.ac_id);
			if( ret )
			{ WRITEMSG return 1; }
			zqs=ln_pay_pln.ttl_cnt;
			syqs=ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;
			mqje=ln_pay_pln.curr_amt;
		}

		pub_base_strpack(g_ln_mst.pact_no);
		pub_base_strpack(g_mdm_ac_rel.ac_no);
		pub_base_strpack(g_mdm_ac_rel1.ac_no);
		pub_base_strpack(mo_discnt.pnote_no);
		/* 对于2003-02-29特殊数据的处理 */
		if( g_ln_mst.opn_date==20030229 )
		{
			g_ln_mst.opn_date=20030228;
		}
		if( g_ln_mst.mtr_date==20030229 )
		{
			g_ln_mst.mtr_date=20030228;
		}
		fprintf(fp,"A|%s|%s|%s|%s|%s|01|%.2f|%.2f|%lf|%lf|%s|%d|%d|%s|%s|%s|%s|%s|%.2f|%d|%d|%s|4|0.00|3|%s\n",
			g_ln_mst.pact_no,g_ln_mst.pact_no,ywzl,g_ln_mst.prdt_no,dklx,
			g_ln_mst.ttl_prvd_amt,g_ln_mst.bal,g_ln_mst.rate,
			g_ln_mst.over_rate,jxfs,g_ln_mst.opn_date,g_ln_mst.mtr_date,
			g_ln_mst.ac_sts,g_ln_mst.five_sts,kjdm,g_mdm_ac_rel.ac_no,
			g_mdm_ac_rel1.ac_no,mqje,zqs,syqs,g_ln_mst.pact_no,
			pjhm);
		count++;
	}
	Ln_mst_Clo_Sel();

	/*** 内部主文件中贴现转出，再贴现账号 ***/
	ret=In_mst_Dec_Sel(g_pub_tx.reply," opn_date=%d and sts!='*' ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_mdm_ac_rel1,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&dc_acc_rel,0x00,sizeof(struct dc_acc_rel_c));
		memset(&mo_discnt,0x00,sizeof(struct mo_discnt_c));
		ret=In_mst_Fet_Sel(&g_in_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		if( strcmp(g_in_mst.prdt_no,"909") && strcmp(g_in_mst.prdt_no,"910") )
			continue;

		/*** 取贴现登记薄 ***/
		ret=Mo_discnt_Sel(g_pub_tx.reply,&mo_discnt,"ac_id=%d",g_in_mst.ac_id);
		if ( ret == 100 )
		{
			strcpy(pjhm,"未查到");
		}
		else if( ret )
		{ 
			WRITEMSG 
			return 1; 
		}else{
			strcpy(pjhm,mo_discnt.pnote_no);
		}

		if( !strcmp(g_in_mst.prdt_no,"909") )
			strcpy(dklx,"09");
		else if( !strcmp(g_in_mst.prdt_no,"910") )
			strcpy(dklx,"0A");

		/*** 取会计科目 ***/
		ret=Dc_acc_rel_Sel(g_pub_tx.reply,&dc_acc_rel,"dc_code='%s'",g_in_parm.dc_code);
		if( ret==100 )	strcpy(dc_acc_rel.acc_hrt,"");
		else if( ret )
		{ WRITEMSG return 1; }
		strcpy(kjdm,dc_acc_rel.acc_hrt);

		/*** 取贷款账号 ***/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%d",g_in_mst.ac_id);
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(mo_discnt.pact_no);
		pub_base_strpack(g_mdm_ac_rel.ac_no);
		pub_base_strpack(mo_discnt.pnote_no);
		fprintf(fp,"A|%d|%s|%s|%s|0|01|%.2f|%.2f|%lf|%lf|0|%d|%d|1|1|%s|%s|%s|0.00|0|0|%s|4|0.00|3|%s\n",
			g_in_mst.ac_id,mo_discnt.pact_no,ywzl,g_in_mst.prdt_no,
			mo_discnt.tranx_bal,g_in_mst.bal,g_in_mst.rate,
			g_in_mst.over_rate,g_in_mst.opn_date,g_in_mst.end_date,
			kjdm,g_mdm_ac_rel.ac_no,mo_discnt.pay_ac_no,mo_discnt.pact_no,
			pjhm);
		count++;
	}
	In_mst_Clo_Sel();


	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_8()
{
	int	ret=0;
	char filename[100];
	char pay_type[3];
	FILE *fp;
	int	count=0;
	char payterm[2];
	char paytermtype[3];

	sprintf(filename,"%s/D%8d_0012.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_LOANTYPE|7|A|\n");
	fprintf(fp,"LOANTYPE|LOANTYPENAME|PAYTERM|PAYTERMTYPE|INTSTDAYSTYPE|INTSTTYPE|INPAYTYPE\n");

	ret=Ln_parm_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		ret=Ln_parm_Fet_Sel(&g_ln_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		if( g_ln_parm.ln_pay_type[0] != '3' && g_ln_parm.ln_pay_type[0] != '4' )    /*非按揭*/
		{
			g_ln_parm.pay_amt_term = 0;
		}
		/* 还款周期类型 */
		if( g_ln_parm.ln_pay_type[0]=='1' )
		{
			strcpy( pay_type,"07");
		}else{
			if( g_ln_parm.pay_term_type[0]=='Y' ){	/* 年 */
				strcpy( pay_type,"06");
			}else if( g_ln_parm.pay_term_type[0]=='Q' ){	/* 季 */
				if( g_ln_parm.pay_amt_term==2 )
				{
					strcpy( pay_type,"05");/*半年*/
				}else{
					strcpy( pay_type,"04");
				}
			}else if( g_ln_parm.pay_term_type[0]=='M' ){	/* 月 */
				if( g_ln_parm.pay_amt_term==6 )
				{
					strcpy( pay_type,"05");/*半年*/
				}else if( g_ln_parm.pay_amt_term==3 ){
					strcpy( pay_type,"04");/*季*/
				}else{
					strcpy( pay_type,"03");
				}
			}else if( g_ln_parm.pay_term_type[0]=='D' ){	/* 日 */
				if( g_ln_parm.pay_amt_term==7 )
				{
					strcpy( pay_type,"02");/*周*/
				}else{
					strcpy( pay_type,"01");
				}
			}
		}

		/* 若是普通贷款，还款周期类型强行赋值为"08-不定期" */
		if( g_ln_parm.ln_pay_type[0]=='2' )
		{
			strcpy( pay_type,"08");
		}
			
		pub_base_strpack(g_ln_parm.title);	
		fprintf(fp,"A|%s|%s|%d|%s|%s|%s|%s\n",
			g_ln_parm.prdt_no,g_ln_parm.title,g_ln_parm.pay_amt_term,
			pay_type,g_ln_parm.intst_days_type,
			g_ln_parm.intst_type,g_ln_parm.ln_pay_type);
		count++;
	}
	Ln_parm_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}
int create_boco_9()
{
	int	ret=0;
	char filename[100],jylx[2],type[2];
	FILE *fp;
	int	count=0;
	struct ln_reg_c	ln_reg;
	struct trace_log_bk_c	trace_log;

	sprintf(filename,"%s/D%8d_0013.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_LoanTrn|8|N|\n");
	fprintf(fp,"DUEBILLNO|FLOW|CURKIND|TRNTYPE|PAYSUM|REPAYDATE|TRNTYPEOTH|NOTES\n");

	/*** 从贷款登记薄中取非按揭本金、利息增加的记录 ***/
	ret=Ln_reg_Dec_Sel(g_pub_tx.reply,"reg_date_beg=%d order by ac_id ",g_pub_tx.tx_date);
	if( ret )
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&ln_reg,0x00,sizeof(struct ln_reg_c));
		ret=Ln_reg_Fet_Sel(&ln_reg,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		strcpy(jylx,"2");
		if( ln_reg.type[0]=='1' )		/*** 本金 ***/
		{
			strcpy(jylx,"0");
			strcpy(type,"0");
		}
		else if( ln_reg.type[0]=='2' )	/*** 表内利息 ***/
		{ strcpy(type,"1"); }
		else if( ln_reg.type[0]=='3' )	/*** 表外利息 ***/
		{ strcpy(type,"2"); }
		else if( ln_reg.type[0]=='4' )	/*** 复利 ***/
		{ strcpy(type,"3"); }
		else if( ln_reg.type[0]=='5' )	/*** 本金积数 ***/
		{ strcpy(type,"4"); }
		else if( ln_reg.type[0]=='6' )	/*** 表内复利 ***/
		{ strcpy(type,"5"); }
		else if( ln_reg.type[0]=='7' )	/*** 表外复利 ***/
		{ strcpy(type,"6"); }
		else if( ln_reg.type[0]=='8' )	/*** 复利复利 ***/
		{ strcpy(type,"7"); }

		fprintf(fp,"A|%d|%d|01|%s|%.2f|%d|%s|\n",
			ln_reg.ac_id,count,jylx,ln_reg.lo_bal,g_pub_tx.tx_date,type);
		count++;
	}
	Ln_reg_Clo_Sel();

	/*** 取按揭贷款增加 ***/
	ret=Ln_mst_Dec_Sel(g_pub_tx.reply," opn_date=%d and ac_sts!='9' and ac_sts!='*' and intst_type='6' ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
		ret=Ln_mst_Fet_Sel(&g_ln_mst,g_pub_tx.reply);
		if( ret==100 )	break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%d|%d|01|0|%.2f|%d|0|\n",
			g_ln_mst.ac_id,count,g_ln_mst.bal,g_pub_tx.tx_date);
		count++;
	}
	Ln_mst_Clo_Sel();

	/*** 从流水中取本金、利息减少的贷款 ***/
	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," tx_code='L201' or tx_code='L202' or tx_code='L203' or tx_code='L204' order by trace_no",g_pub_tx.tx_date);
	if( ret )
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		if( trace_log.add_ind[0]=='1' )		/*** 不取增加的 ***/
			continue;

		strcpy(jylx,"3");
		strcpy(type,"0");
		pub_base_strpack(trace_log.brf);
		if( !strcmp(trace_log.brf,"贷款还本金") )
		{
			strcpy(jylx,"3");
			strcpy(type,"0");
		}
		if( !strcmp(trace_log.brf,"还利息") )
		{
			strcpy(jylx,"4");
			strcpy(type,"4");
		}
		if( !strcmp(trace_log.brf,"按揭还款本金部分") )
		{
			strcpy(jylx,"5");
			strcpy(type,"0");
		}
		if( !strcmp(trace_log.brf,"按揭还款利息部分") )
		{
			strcpy(jylx,"4");
			strcpy(type,"4");
		}
		if( !strcmp(trace_log.sub_tx_code,"L204") )
		{
			strcpy(jylx,"5");
			strcpy(type,"0");
		}

		fprintf(fp,"A|%d|%d|01|%s|%.2f|%d|%s|\n",
			trace_log.ac_id,count,jylx,trace_log.amt,g_pub_tx.tx_date,type);
		count++;
	}
	Trace_log_bk_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}
int create_boco_10()
{
	int	ret=0;
	char mo_sts[2];
	char filename[100];
	double bzj_tt=0.00;
	FILE *fp;
	int	count=0;
	struct mo_bank_po_c	mo_bank_po;
	struct mo_bank_acc_po_c	mo_bank_acc_po;
	struct mo_po_co_c	sMoPoCo;
	/**struct mbfebillopnreg	mbfebillopnreg; **/

	sprintf(filename,"%s/D%8d_0021.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_DraftInfo|27|A|\n");
	fprintf(fp,"CONTRACTNO|DRAFTMARK|REMITTERACNT|CURKIND|DRAFTSUM|BEGINDATE|MATUREDATE|TERM|PAYEECODE|PAYEEACCBANK|PAYEENAME|PAYEEACNT|BANKCODE|GUARSUM|GUARRATE|DRAFTUSAGE|PAYSOURCE|CORCONTRACTNO|CONTRACTDESC|REFUSEOPDATE|REFUSEOPCODE|NOREFOPCODE|CANCELREFDATE|REFUSEDATE|REFUSEREASON|DRAFTSTATUS|Notes\n");

	/*** 承兑汇票 ***/
	ret=Mo_bank_po_Dec_Sel(g_pub_tx.reply,"1=1");	
	if( ret )
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&mo_bank_po,0x00,sizeof(struct mo_bank_po_c));
		memset(&mo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
		memset(&sMoPoCo,0x00,sizeof(struct mo_po_co_c));
		ret=Mo_bank_po_Fet_Sel(&mo_bank_po,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Mo_bank_acc_po_Sel(g_pub_tx.reply,&mo_bank_acc_po,"pact_no='%s' ",mo_bank_po.pact_no);
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(mo_bank_po.pact_no);
		pub_base_strpack(mo_bank_po.po_no);
		pub_base_strpack(mo_bank_acc_po.payer_ac_no);
		pub_base_strpack(mo_bank_acc_po.payee_ac_no);
		pub_base_strpack(mo_bank_acc_po.payee_br_name);
		pub_base_strpack(mo_bank_acc_po.payee_name);
		ret=Mo_po_co_Sel(g_pub_tx.reply,&sMoPoCo,"pact_no='%s'",mo_bank_po.pact_no);
		if( ret == 100 )
		{ strcpy( sMoPoCo.contract_no, "0000000000000000" ); }
		else if( ret != 0 )
		{ WRITEMSG return 1; }
		pub_base_strpack(sMoPoCo.contract_no);
			
		if( mo_bank_po.po_sts[0]=='1' || mo_bank_po.po_sts[0]=='4' )
		{
			strcpy(mo_sts,"1");	/*开立*/
			if( mo_bank_po.at_term_ind[0]=='Y' )
			{
				strcpy(mo_sts,"3");	/*兑付*/
			}
		}
		if( mo_bank_po.po_sts[0]=='2' )
		{
			strcpy(mo_sts,"3");	/*兑付*/
		}			
		if( mo_bank_po.po_sts[0]=='3' )
		{
			strcpy(mo_sts,"6");	/*退票*/
		}
		if( strlen(mo_bank_acc_po.over_ac_no) )
		{
			/***
			ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel , "ac_no='%s'",mo_bank_acc_po.over_ac_no);
			if(ret) return 1;
			
			ret=Ln_mst_Sel( g_pub_tx.reply,&g_ln_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
			if(ret) return 1;
			
			if( pub_base_CompDblVal(g_ln_mst.bal,0.00)!=0 || pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)!=0
			    ||pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)!=0 || pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)!=0
			    ||pub_base_CompDblVal(g_ln_mst.intst_acm,0.00)!=0 || pub_base_CompDblVal(g_ln_mst.in_lo_acm,0.00)!=0
			    ||pub_base_CompDblVal(g_ln_mst.out_lo_acm,0.00)!=0 || pub_base_CompDblVal(g_ln_mst.cmpd_lo_acm,0.00)!=0 )
			***/	
			
			strcpy(mo_sts,"5");	/*垫款*/

		}
		bzj_tt=mo_bank_acc_po.bail_amt/mo_bank_acc_po.sign_amt*mo_bank_po.po_amt;
		
		fprintf(fp,"A|%s|%s|%s|01|%.2f|%d|%d|0|%s|%s|%s|%s|%s|%.2f|%.2f|||||||||||%s|\n",
			sMoPoCo.contract_no,mo_bank_po.po_no,mo_bank_acc_po.payer_ac_no,
			mo_bank_po.po_amt,mo_bank_po.sign_date,mo_bank_acc_po.matr_date,
			mo_bank_acc_po.payee_ac_no,mo_bank_acc_po.payee_br_name,
			mo_bank_acc_po.payee_name,mo_bank_acc_po.payee_ac_no,
			mo_bank_po.sign_br_no,bzj_tt,
			mo_bank_acc_po.bail_amt/mo_bank_acc_po.sign_amt,mo_sts);
		count++;
	}

	/*** 银行汇票 ***
	ret=Mbfebillopnreg_Dec_Sel(g_pub_tx.reply,"consigndate=%d",g_pub_tx.tx_date);	
	if( ret )
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&mbfebillopnreg,0x00,sizeof(struct mbfebillopnreg));
		ret=Mbfebillopnreg_Fet_Sel(&mbfebillopnreg,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%d",mbfebillopnreg.payeracid);
		if( ret )
		{ WRITEMSG return 1; }

		pub_base_strpack(g_mdm_ac_rel.ac_no);
		pub_base_strpack(mbfebillopnreg.recipientacc);
		pub_base_strpack(mbfebillopnreg.payee_br_name);
		pub_base_strpack(mbfebillopnreg.payee_name);
		fprintf(fp,"A|%s|%s|%s|01|%.2f|%d|%d|0|%s|%s|%s|%s|%s|||||||||||||%s|\n",
			mbfebillopnreg.billcode,mbfebillopnreg.billcode, mdm_ac_rel.ac_no,
			mbfebillopnreg.remittanceamnt,mbfebillopnreg.consigndate,
			mbfebillopnreg.wrkdate,
			mbfebillopnreg.recipientacc,mbfebillopnreg.rdficode,
			mbfebillopnreg.recipientname,mbfebillopnreg.recipientacc,
			mbfebillopnreg.payeropenaccbkcode,
			mbfebillopnreg.billsts);
		count++;
	}
	*******/
	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_11()
{
	int	ret=0;
	char filename[100],filename1[100],fkyy[2];
	FILE *fp,*fp1;
	int	count=0;
	int count1=0;
	double bzje,qyje,yhdk;
	long	dkrq=0;
	struct mo_bank_po_c	mo_bank_po;
	struct mo_bank_acc_po_c	mo_bank_acc_po;
	struct trace_log_bk_c	trace_log;

	sprintf(filename,"%s/D%8d_0022.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	sprintf(filename1,"%s/D%8d_0023.mis",getenv("MISDIR"),g_pub_tx.tx_date);

	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fp1=fopen(filename1,"w");
	if( fp1==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_DraftPay|9|N|\n");
	fprintf(fp,"DRAFTMARK|CURKIND|PAYDATE|PAYREASON|GUARPAY|CSTCASH|BANKADVANSUM|ADVANCEDATE|DUEBILLNO\n");

	fprintf(fp1,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp1,"boco.INT_DraftLoan|5|N|\n");
	fprintf(fp1,"DRAFTNO|DUEBILLNO|PAYDATE|PAYSUM|EXERATE\n");

	/*** 承兑汇票 ***/
	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," (sub_tx_code='B334' or sub_tx_code='B337') and sts='0' and ac_no[1,3]!='702' ");	
	if( ret )
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		memset(&mo_bank_po,0x00,sizeof(struct mo_bank_po_c));
		memset(&mo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }
		
		pub_base_strpack(trace_log.note_no);
		ret=Mo_bank_po_Sel(g_pub_tx.reply,&mo_bank_po,"po_no='%s' ",trace_log.note_no);
		if( ret )
		{ WRITEMSG return 1; }

		ret=Mo_bank_acc_po_Sel(g_pub_tx.reply,&mo_bank_acc_po,"pact_no='%s' ",mo_bank_po.pact_no);
		if( ret )
		{ WRITEMSG return 1; }

		if( !strcmp(trace_log.tx_code,"4334") )		/*** 退票 ***/
		{
			strcpy(fkyy,"3");
			bzje=0.00;
			qyje=0.00;
			yhdk=0.00;			/*** 垫款金额 ***/
			dkrq=0;				/*** 垫款日期 ***/
		}
		else										/*** 核销 ***/
		{
			strcpy(fkyy,"1");
			if( mo_bank_acc_po.bail_amt>mo_bank_po.po_amt )	/* 先扣保证金 */
			{
				bzje=mo_bank_po.po_amt;
				qyje=0.00;
				yhdk=0.00;
				dkrq=0.00;
			}
			else
			{
				bzje=mo_bank_acc_po.bail_amt;
				if( mo_bank_acc_po.tot_mort_amt>mo_bank_po.po_amt-bzje )
				{											/* 再扣质押 */
					qyje=mo_bank_po.po_amt-bzje;
					yhdk=0.00;
					dkrq=0.00;
				}
				else										/* 最后银行垫款 */
				{
					yhdk=mo_bank_po.po_amt-bzje-qyje;
					dkrq=g_pub_tx.tx_date;
					fprintf(fp1,"A|%s|%s|%d|%.2f|0.00\n",
						trace_log.note_no,mo_bank_po.pact_no,
						trace_log.tx_date,yhdk);
					count1++;
				}
			}
		}
		fprintf(fp,"A|%s|%s|%d|%s|%.2f|%.2f|%.2f|%d|%s\n",
			trace_log.note_no,trace_log.cur_no,trace_log.tx_date,fkyy,
			bzje,qyje,yhdk,dkrq,mo_bank_po.pact_no);
		count++;
	}

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	fprintf(fp1,"END|%d\n",count1);
	fclose(fp1);

	return 0;
}

int create_boco_16()
{
	int	ret=0;
	char filename[100],kmlx[3];
	FILE *fp;
	int	count=0;
	struct com_item_c	com_item;
	struct com_item_c	com_item1;

	sprintf(filename,"%s/D%8d_0031.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_Subject|5|A|\n");
	fprintf(fp,"SubID|SubLevel|SubCNName|SubType|ParentID\n");

	ret=Com_item_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&com_item,0x00,sizeof(struct com_item_c));
		ret=Com_item_Fet_Sel(&com_item,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		if( com_item.acc_no[0]=='1' )
			strcpy(kmlx,"1");
		else if( com_item.acc_no[0]=='2' )
			strcpy(kmlx,"2");
		else if( com_item.acc_no[0]=='3' )
			strcpy(kmlx,"4");
		else if( com_item.acc_no[0]=='4' )
			strcpy(kmlx,"8");
		else if( com_item.acc_no[0]=='5' )
			strcpy(kmlx,"16");
		else if( com_item.acc_no[0]=='6' )
			strcpy(kmlx,"32");
		else
			strcpy(kmlx,"64");
		
		pub_base_strpack(com_item.acc_name);	

		memset( &com_item1, 0x00, sizeof( struct com_item_c ) );
		ret=Com_item_Sel(g_pub_tx.reply,&com_item1,"acc_hrt='%s'",com_item.up_acc_hrt);
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%s|%s|%s|%s\n",
			com_item.acc_no,com_item.acc_lvl,com_item.acc_name,
			kmlx,com_item1.acc_no);
		count++;
	}
	Com_item_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_17()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct	gl_mst_c	g_gl_mst;
	struct com_item_c	com_item;

	sprintf(filename,"%s/D%8d_0032.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_CoreGenLedger|8|N|\n");
	fprintf(fp,"SubID|DataDate|LDDRBAL|LDCRBAL|DRAMT|CRAMT|DDRBAL|DCRBAL\n");

	ret=Gl_mst_Dec_Sel(g_pub_tx.reply," 1=1 ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
		ret=Gl_mst_Fet_Sel(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		memset( &com_item, 0x00, sizeof( struct com_item_c ) );
		ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt='%s'",g_gl_mst.acc_hrt);
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%d|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f\n",
			com_item.acc_no,g_gl_mst.date,g_gl_mst.ldd_bal,
			g_gl_mst.lcd_bal,g_gl_mst.rdd_amt,g_gl_mst.rcd_amt,
			g_gl_mst.dr_bal,g_gl_mst.cr_bal);
		count++;
	}
	Gl_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_18()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct	gl_mst_c	g_gl_mst;

	sprintf(filename,"%s/D%8d_0033.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_LTData|4|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate|Rate\n");

	ret=Gl_mst_Dec_Sel(g_pub_tx.reply," acc_hrt in ('11400','11500','11600') ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
		ret=Gl_mst_Fet_Sel(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%.2f||0.00\n",g_gl_mst.acc_hrt,g_gl_mst.dr_bal);
		count++;
	}
	Gl_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_19()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0034.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_SLData|4|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate|Rate\n");

	ret=Dd_mst_Dec_Sel(g_pub_tx.reply," prdt_no in ('135','136','137') ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%d|%.2f||%lf\n",
			g_dd_mst.ac_id,g_dd_mst.bal,g_dd_mst.rate);
		count++;
	}
	Dd_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_20()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0035.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_OthARData|3|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate\n");

	ret=In_mst_Dec_Sel(g_pub_tx.reply," prdt_no='907' ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		ret=In_mst_Fet_Sel(&g_in_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%d|%.2f|0\n",g_dd_mst.ac_id,g_dd_mst.bal);
		count++;
	}
	In_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_21()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct	gl_mst_c	g_gl_mst;

	sprintf(filename,"%s/D%8d_0036.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_CentData|4|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate|Rate\n");

	ret=Gl_mst_Dec_Sel(g_pub_tx.reply," acc_hrt='23100' ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
		ret=Gl_mst_Fet_Sel(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%.2f||0.00\n",g_gl_mst.acc_hrt,g_gl_mst.cr_bal);
		count++;
	}
	Gl_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_22()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct	gl_mst_c	g_gl_mst;

	sprintf(filename,"%s/D%8d_0037.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_DealData|3|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate\n");

	ret=Gl_mst_Dec_Sel(g_pub_tx.reply," acc_hrt in ('26300','26400','26500','26600','26700','26800') ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
		ret=Gl_mst_Fet_Sel(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%s|%.2f|\n",g_gl_mst.acc_hrt,g_gl_mst.cr_bal);
		count++;
	}
	Gl_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_23()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0038.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_OthDealData|3|A|\n");
	fprintf(fp,"BussNo|Balance|DueDate\n");

	ret=In_mst_Dec_Sel(g_pub_tx.reply," prdt_no='913' and sts!='*' ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		ret=In_mst_Fet_Sel(&g_in_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }

		fprintf(fp,"A|%d|%.2f|\n",g_in_mst.ac_id,g_in_mst.bal,g_in_mst.lst_date);
		count++;
	}
	In_mst_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_24()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0,trace_no=0,old_trace_no=0;
	double	hlzj=0.00;
	struct trace_log_bk_c	trace_log;
	struct mo_stok_rebuy_prot_c	sMo_stok_rebuy_prot;

	sprintf(filename,"%s/D%8d_0039.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_BondMoneyBack|9|A|\n");
	fprintf(fp,"TranID|DataDate|Maturity|GetAmt|Opponent|BondCode|BondName|BondAmt|StartDate\n");

	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," tx_code='4703' and sts='0' order by trace_no,sub_tx_code ");
	if( ret )	
	{ WRITEMSG return 1; }

	memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));
	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 && count==0 ) break;
		if( ret==100 )
		{
			pub_base_strpack(sMo_stok_rebuy_prot.pact_no);
			pub_base_strpack(sMo_stok_rebuy_prot.opt_name);
			pub_base_strpack(sMo_stok_rebuy_prot.tcht_type);
			fprintf(fp,"A|%s|%d|%d|%.2f|%s|%s||%.2f|%d\n",
				sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_date,
				sMo_stok_rebuy_prot.matr_date,hlzj,
				sMo_stok_rebuy_prot.opt_name,
				sMo_stok_rebuy_prot.tcht_type,
				sMo_stok_rebuy_prot.par_value,
				sMo_stok_rebuy_prot.sign_date);
			count++;
			break;
		}
		if( ret )
		{ WRITEMSG return 1; }
		trace_no=trace_log.trace_no;
		if( count==0 )
			old_trace_no=trace_no;

		if( trace_no!=old_trace_no )
		{
			pub_base_strpack(sMo_stok_rebuy_prot.pact_no);
			pub_base_strpack(sMo_stok_rebuy_prot.opt_name);
			pub_base_strpack(sMo_stok_rebuy_prot.tcht_type);
			fprintf(fp,"A|%s|%d|%d|%.2f|%s|%s||%.2f|%d\n",
				sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_date,
				sMo_stok_rebuy_prot.matr_date,hlzj,
				sMo_stok_rebuy_prot.opt_name,
				sMo_stok_rebuy_prot.tcht_type,
				sMo_stok_rebuy_prot.par_value,
				sMo_stok_rebuy_prot.sign_date);
			count++;
			memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));
			hlzj=0.00;
			old_trace_no=trace_no;
		}

		if( !strcmp(trace_log.sub_tx_code,"4703") )
		{
			ret=Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,"ac_id=%d and sts!='1' and sts!='*' ",trace_log.ac_id);
			if( ret )
			{ WRITEMSG return 1; }
		}
		else if( !strcmp(trace_log.sub_tx_code,"M001") )
			hlzj=trace_log.amt;
		else if( !strcmp(trace_log.sub_tx_code,"A016") )
			hlzj=trace_log.amt;
	}
	Trace_log_bk_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_25()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0,trace_no=0,old_trace_no=0;
	double	hlzj=0.00;
	struct trace_log_bk_c	trace_log;
	struct mo_stok_rebuy_prot_c	sMo_stok_rebuy_prot;

	sprintf(filename,"%s/D%8d_0040.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_BondMoneyOut|9|A|\n");
	fprintf(fp,"TranID|DataDate|Maturity|GetAmt|Opponent|BondCode|BondName|BondAmt|StartDate\n");

	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," tx_code='4705' and ac_id!=0 and sts='0' order by trace_no,sub_tx_code ");
	if( ret )	
	{ WRITEMSG return 1; }

	memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));
	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 && count==0 ) break;
		if( ret==100 )
		{
			pub_base_strpack(sMo_stok_rebuy_prot.pact_no);
			pub_base_strpack(sMo_stok_rebuy_prot.opt_name);
			pub_base_strpack(sMo_stok_rebuy_prot.tcht_type);
			fprintf(fp,"A|%s|%d|%d|%.2f|%s|%s||%.2f|%d\n",
				sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_date,
				sMo_stok_rebuy_prot.matr_date,hlzj,
				sMo_stok_rebuy_prot.opt_name,
				sMo_stok_rebuy_prot.tcht_type,
				sMo_stok_rebuy_prot.par_value,
				sMo_stok_rebuy_prot.sign_date);
			count++;
			break;
		}
		if( ret )
		{ WRITEMSG return 1; }
		trace_no=trace_log.trace_no;
		if( count==0 )
			old_trace_no=trace_no;

		if( trace_no!=old_trace_no )
		{
			pub_base_strpack(sMo_stok_rebuy_prot.pact_no);
			pub_base_strpack(sMo_stok_rebuy_prot.opt_name);
			pub_base_strpack(sMo_stok_rebuy_prot.tcht_type);
			fprintf(fp,"A|%s|%d|%d|%.2f|%s|%s||%.2f|%d\n",
				sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_date,
				sMo_stok_rebuy_prot.matr_date,hlzj,
				sMo_stok_rebuy_prot.opt_name,
				sMo_stok_rebuy_prot.tcht_type,
				sMo_stok_rebuy_prot.par_value,
				sMo_stok_rebuy_prot.sign_date);
			count++;
			memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));
			hlzj=0.00;
			old_trace_no=trace_no;
		}

		if( !strcmp(trace_log.sub_tx_code,"4705") )
		{
			/***xxx 20030902 ***
			ret=Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,"ac_id=%d and sts!='1' and sts!='*' ",trace_log.ac_id);
			***xxx 20030902 ***/
			ret=Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,"ac_id=%d ",trace_log.ac_id);
			if( ret )
			{ WRITEMSG return 1; }
		}
		else if( !strcmp(trace_log.sub_tx_code,"M002") )
			hlzj=trace_log.amt;
		else if( !strcmp(trace_log.sub_tx_code,"A017") )
			hlzj=trace_log.amt;
	}
	Trace_log_bk_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}

int create_boco_26()
{
	int	ret=0;
	char filename[100],ac_type[2],name[51];
	FILE *fp;
	int	count=0;
	struct trace_log_bk_c	trace_log;
	struct tx_sub_def_c	tx_sub_def;
	struct com_branch_c sComBranch;
	char up_br_no[10];

	sprintf(filename,"%s/D%8d_0041.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }
	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_CapitalInOut|6|N|\n");
	fprintf(fp,"BankID|DataDate|TranID|AccountName|DealAmt|Remark\n");

	ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply," tx_code not in('5504','5202','5203') and amt>500000.00 and sts='0' and no_show!='1' order by trace_no,sub_tx_code ");
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&trace_log,0x00,sizeof(struct trace_log_bk_c));
		ret=Trace_log_bk_Fet_Sel(&trace_log,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )

		memset(&tx_sub_def,0x00,sizeof(struct tx_sub_def_c));
		ret=Tx_sub_def_Sel(g_pub_tx.reply,&tx_sub_def,"sub_tx_code='%s'",trace_log.sub_tx_code);
		if( ret==100 ) continue; /**xxx**/
		if( ret )
		{ WRITEMSG return 1; }

		if( tx_sub_def.acct_ind[0]=='0' )
			continue;

		pub_base_idseqn_file(trace_log.ac_id,trace_log.ac_seqn,ac_type);

		if( ac_type[0]=='1' )
			strcpy(name,g_dd_mst.name);
		else if( ac_type[0]=='2' )
			strcpy(name,g_td_mst.name);
		else if( ac_type[0]=='3' )
			strcpy(name,g_ln_mst.name);
		else if( ac_type[0]=='9' )
			strcpy(name,g_in_mst.name);
		else
			strcpy(name,"");

		memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
		g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", trace_log.tx_br_no );
		if( g_reply_int )
		{
			vtcp_log("查询交易流水网点号有误[%s][%ld]", trace_log.tx_br_no, g_reply_int );
			return 1;
		}
		memset( up_br_no, 0x00, sizeof( up_br_no ) );
		strcpy( up_br_no, sComBranch.up_br_no );
		g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log("查询网点名称有误!![%s][%ld]", trace_log.tx_br_no, g_reply_int );
			return 1;
		}

		pub_base_strpack(name);
		fprintf(fp,"A|%s|%d|%s|%s|%.2f|\n",
			sComBranch.br_name,trace_log.tx_date,trace_log.ac_no,
			name,trace_log.amt);
		count++;
	}
	Trace_log_bk_Clo_Sel();

	fprintf(fp,"END|%d\n",count);
	fclose(fp);

	return 0;
}
int create_boco_13()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0024.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_ArriveInfo|13|N|\n");
	fprintf(fp,"CONTRACTNO|ARRIVEFLOW|OPTYPE|CURKIND|ARRIVESUM|ARRIVEDATE|PAYABLEDATE|PAYFLAG|CASHFLAG|CASHSUM|CASHREASON|CASHDATE|CASHABLEDATE\n");

	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}

int create_boco_14()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0025.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_ArrivePay|11|N|\n");
	fprintf(fp,"CONTRACTNO|ARRIVEFLOW|OPTYPE|CURKIND|PAYDATE|PAYREASON|GUARPAY|CSTCASH|BANKADVANSUM|ADVANCEDATE|DUEBILLNO\n");

	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}

int create_boco_15()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0026.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_ArrivePay|6|N|\n");
	fprintf(fp,"CONTRACTNO|ARRIVEFLOW|DUEBILLNO|PAYDATE|PAYSUM|EXERATE\n");

	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}
/*****
int create_boco_13()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0024.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_ArriveInfo|13|N|\n");
	fprintf(fp,"CONTRACTNO|ARRIVEFLOW|OPTYPE|CURKIND|ARRIVESUM|ARRIVEDATE|PAYABLEDATE|PAYFLAG|CASHFLAG|CASHSUM|CASHREASON|CASHDATE|CASHABLEDATE\n");

	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}

int create_boco_14()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;

	sprintf(filename,"%s/D%8d_0025.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);
	fprintf(fp,"boco.INT_ArrivePay|11|N|\n");
	fprintf(fp,"CONTRACTNO|ARRIVEFLOW|OPTYPE|CURKIND|PAYDATE|PAYREASON|GUARPAY|CSTCASH|BANKADVANSUM|ADVANCEDATE|DUEBILLNO\n");

	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}
******/

int create_boco_28()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct cif_basic_inf_c	S_cif_basic_inf;
	
	sprintf(filename,"%s/D%8d_0043.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);	
	fprintf(fp,"boco.INT_CstBase|3|N|\n");
	fprintf(fp,"CSTCODE|CSTTYPE|CSTNAME\n");
			
	ret=Cif_basic_inf_Dec_Sel( g_pub_tx.reply,"crt_date=%ld and sts in ('1','0') ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&S_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
		ret=Cif_basic_inf_Fet_Sel(&S_cif_basic_inf,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }	
		fprintf(fp,"A|%ld|%s|%s\n",S_cif_basic_inf.cif_no,S_cif_basic_inf.type,S_cif_basic_inf.name);
		count++;	
	}
	Cif_basic_inf_Clo_Sel();
	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}

int create_boco_29()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	struct cif_rel_chg_hst_c	S_cif_rel_chg_hst;
	
	sprintf(filename,"%s/D%8d_0044.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);	
	fprintf(fp,"boco.INT_CstBaseHis|6|N|\n");
	fprintf(fp,"CIF_NO|CHG_TABLENAME|CHG_COLUMNNAME|CHG_DATE|BEF_CONTENT|AFT_CONTENT\n");
			
	ret=Cif_rel_chg_hst_Dec_Sel( g_pub_tx.reply,"chg_tablename='cif_basic_inf' and chg_columnname='name' and chg_date=%ld ",g_pub_tx.tx_date);
	if( ret )	
	{ WRITEMSG return 1; }

	while(1)
	{
		memset(&S_cif_rel_chg_hst,0x00,sizeof(struct cif_rel_chg_hst_c));
		ret=Cif_rel_chg_hst_Fet_Sel(&S_cif_rel_chg_hst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }	
		fprintf(fp,"A|%ld|%s|%s|%ld|%s|%s\n",S_cif_rel_chg_hst.cif_no,S_cif_rel_chg_hst.chg_tablename,S_cif_rel_chg_hst.chg_columnname,S_cif_rel_chg_hst.chg_date,S_cif_rel_chg_hst.bef_content,S_cif_rel_chg_hst.aft_content);
		count++;	
	}
	Cif_rel_chg_hst_Clo_Sel();
	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}

int create_boco_30()
{
	int	ret=0;
	char filename[100];
	FILE *fp;
	int	count=0;
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c ));
	
	sprintf(filename,"%s/D%8d_0045.mis",getenv("MISDIR"),g_pub_tx.tx_date);
	fp=fopen(filename,"w");
	if( fp==NULL )
	{ WRITEMSG return 1; }

	fprintf(fp,"%8d|%s|\n",g_pub_tx.tx_date,g_pub_tx.tel);	
	fprintf(fp,"boco.INT_AcntReflect|3|A|\n");
	fprintf(fp,"AC_NO|Acidseqn|MDM_CODE\n");
			
	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply,"1=1");
	if( ret )	
	{ WRITEMSG return 1; }
	pub_base_strpack(g_mdm_ac_rel.ac_no);
	while(1)
	{
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Fet_Sel(&g_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{ WRITEMSG return 1; }	
		fprintf(fp,"A|%s|%ld%04d|%s\n",g_mdm_ac_rel.ac_no,g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,g_mdm_ac_rel.mdm_code);
		count++;	
	}
	Mdm_ac_rel_Clo_Sel();
	fprintf(fp,"END|%d\n",count);
	fclose(fp);
	return 0;
}
