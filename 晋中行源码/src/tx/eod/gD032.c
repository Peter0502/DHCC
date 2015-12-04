/*************************************************************
* 文 件 名: gD032.c
* 功能描述：对私活期户计息程序
**************************************************************/
#define MYSQLERR if( ret ){\
	strcpy( g_pub_tx.reply,"AT03" );\
		sprintf(acErrMsg,"SQLERR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
}
#define MYERR if( ret ){\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_eod_exec_c.h"
#include "intst_hst_c.h"

static struct com_sys_parm_c com_sys_parm;
extern struct com_eod_exec_c grz_com_eod_exec;	/* 全局,用于向子程序传递控制序列 */

gD032()
{
	int    ret = 0;
	struct dd_mst_c dd_mst_c;
	struct dd_mst_c o_dd_mst;
	struct dd_parm_c dd_parm_c;
	struct com_branch_c com_branch_c;
	struct com_branch_c o_com_branch_c;
	struct S_pub_intst sum_intst;
	long   first = 0;
	long   second= 0;
	memset(&o_com_branch_c, 0x0, sizeof(o_com_branch_c));
	memset(&o_dd_mst, 0x0, sizeof(o_dd_mst));
	memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));
	memset(&g_pub_tx, 0x0, sizeof(g_pub_tx));
	memset(&dd_parm_c, 0x0, sizeof(struct dd_parm_c));
	memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
	memset(&sum_intst, 0x0, sizeof(sum_intst));
	memset(&com_sys_parm, 0x0, sizeof(struct com_sys_parm_c));

	vtcp_log("-----------begin--------------");

	ret = sql_begin();	/* 打开事务 */
	MYSQLERR

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
		goto ErrExit;


	ret = pub_base_GetSysparm(&com_sys_parm);
	MYERR
	g_pub_tx.tx_date = com_sys_parm.sys_date;

	/*
	* 5储蓄活期 7个人公积金活期
	* substr(spe_ind,2,1)='0'储蓄结息标志0是未结 
	*/
	/**换成先循环机构 按就够找账号 部分提交
	ret = Eod_Dd_mst_Dec_Sel(g_pub_tx.reply,
	"(ac_type='5' or ac_type='7') and ac_sts='1' and prdt_no!='102' and opn_br_no in(select br_no from com_branch where substr(spe_ind,2,1)='0') order by opn_br_no,prdt_no ");**/

	ret= Com_branch_Dec_Sel(g_pub_tx.reply,"br_type='4' and substr(spe_ind,2,1)='0' order by br_no");
	MYSQLERR
	while (1)
	{
		memset(&com_branch_c,0,sizeof(com_branch_c));
		memset(&sum_intst, 0x0, sizeof(sum_intst));
		first=0;
		ret = Com_branch_Fet_Sel(&com_branch_c, g_pub_tx.reply);
		if (ret==100)
			break;
		MYSQLERR


			/*ret = Dd_mst_Dec_Upd2(g_pub_tx.reply,"(ac_type='5' or ac_type='7') and ac_sts='1' and prdt_no!='103' and opn_br_no ='%s' order by opn_br_no,prdt_no",com_branch_c.br_no); 20130317 wjl 添加七天乐虚拟户产品107 不计息*/
			/*添加104产品 金太阳卡不计息 20151201 wudawei*/
			ret = Dd_mst_Dec_Upd2(g_pub_tx.reply,"(ac_type='5' or ac_type='7') and ac_sts='1' and prdt_no!='103' and prdt_no!='104' and  prdt_no!='107' and opn_br_no ='%s' order by opn_br_no,prdt_no",com_branch_c.br_no);
		MYSQLERR
		while (1) 
		{
			memset(&g_dd_mst, 0x0, sizeof(struct dd_mst_c));
			ret = Dd_mst_Fet_Upd2(&g_dd_mst, g_pub_tx.reply);
			if (ret == 100)
				break;
			MYSQLERR
			if (first == 0)
			{
				memcpy(&o_dd_mst, &g_dd_mst, sizeof(dd_mst_c));
			}
			if (strcmp(g_dd_mst.prdt_no, o_dd_mst.prdt_no))
			{
				ret = sub_intst_acct(o_dd_mst, g_dd_parm, sum_intst);
				MYERR
					memset(&sum_intst, 0x0, sizeof(sum_intst));
			}
			if (first==0 ||strcmp(g_dd_mst.prdt_no, o_dd_mst.prdt_no))
			{
				memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
				strcpy(g_dd_parm.prdt_no,g_dd_mst.prdt_no);
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",g_dd_parm.prdt_no);
				if (ret) 
				{
					sprintf(acErrMsg, "查询活期产品表错误[%s]!!!", dd_mst_c.prdt_no);
					WRITEMSG
						goto ErrExit;
				}
			}
			memcpy(&o_dd_mst, &g_dd_mst, sizeof(struct dd_mst_c));
			first++;
			if (pub_base_CompDblVal(g_dd_mst.bal, 0.00) < 0)
			{
				continue;
			}
			ret = sub_cal_intst(g_dd_mst, g_dd_parm);
			MYERR
				sum_intst.dealval += g_pub_intst.dealval;
			sum_intst.val += g_pub_intst.val;
			Dd_mst_Upd_Upd2(g_dd_mst,g_pub_tx.reply);
			MYSQLERR
		}
		Dd_mst_Clo_Upd2();
		if (first) 
		{
			ret = sub_intst_acct(o_dd_mst, g_dd_parm, sum_intst);
			MYERR
				memset(&sum_intst, 0x0, sizeof(sum_intst));
		}
		set_zd_data(DC_CODE, "");
		set_zd_double("102P", 0.00);
		set_zd_double("102Q", 0.00);
		set_zd_data("102J", "");
		memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
		if (first)
		{
			ret = sql_execute("update com_branch set spe_ind=substr(spe_ind,1,1)||'1'||substr(spe_ind,3,6) where br_no='%s'",com_branch_c.br_no);
			MYERR
			sql_commit();
			sql_begin();
		}	
	}
	Com_branch_Clo_Sel();
	ret = sql_execute("update com_branch set spe_ind=substr(spe_ind,1,1)||'0'||substr(spe_ind,3,6) where 1=1");
	MYERR
		OkExit:
		sql_commit();		/*--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "对私活期户计息程序成功!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
	ErrExit:
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0')
		strcpy(g_pub_tx.reply, "G009");
	sql_rollback();		/*--事务回滚--*/
	sprintf(acErrMsg, "对私活期户计息程序失败!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}
int sub_cal_intst(dd_mst_c, dd_parm_c)
struct dd_mst_c dd_mst_c;
struct dd_parm_c dd_parm_c;
{
	long            caldate, tmp_end_date = 0;
	int             ret = 0, tmpday = 0;
	double          tmpacm = 0.00, tmpbal = 0.00;
	char            tmptype[2];
	struct intst_hst_c T_intst_hst;
	struct dd_mst_c tmp_dd_mst;
	memset(tmptype, 0 , sizeof(tmptype));
	memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));

	memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
	strcpy(g_pub_tx.ac_no, "");
	g_pub_tx.ac_id = g_dd_mst.ac_id;
	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;
	strcpy(g_pub_tx.cur_no, g_dd_parm.cur_no);
	strcpy(g_pub_tx.prdt_code, g_dd_parm.prdt_no);
	strcpy(g_pub_tx.ct_ind, "2");

	if (grz_com_eod_exec.do_type[0] == '0')
		pub_base_deadlineD(g_pub_tx.tx_date, 1, &caldate);
	else
		caldate = g_pub_tx.tx_date;

	/***计算分段利息*暂略**/
	/* 计算积数 */
	/*---- hao ---- @20050915@ ---- 将倒算时多产生的积数刨除 ----*/
	if (caldate == 20050921) 
	{
		tmp_end_date = 20060701;
		tmpbal = g_dd_mst.bal;
		tmpacm = pub_base_CalTriDays(g_dd_mst.lst_date, tmp_end_date) * floor(tmpbal);
		vtcp_log("tmpday=[%ld],tmpacm=[%lf],ac_id=[%ld],bal=[%lf]", pub_base_CalTriDays(g_dd_mst.lst_date, tmp_end_date), tmpacm, g_dd_mst.ac_id, tmpbal);
		g_dd_mst.intst_acm = g_dd_mst.intst_acm - tmpacm;
		if (pub_base_CompDblVal(g_dd_mst.intst_acm, 0.00) < 0) 
		{
			g_dd_mst.intst_acm = 0.00;
		}
	}
	if (caldate == 20050921) 
	{
		/** 20050921以前按每月３０天计息 **/
		strcpy(tmptype, "1");
		if (pub_base_CalAcm(g_dd_mst.lst_date, caldate, tmptype, g_dd_parm.acm_calc_type, g_dd_mst.bal, 0.00, &g_dd_mst.intst_acm, g_dd_mst.intst_type, g_dd_parm.intst_mon, g_dd_parm.intst_day)) 
		{
			sprintf(acErrMsg, "计算利息积数错误!");
			WRITEMSG
				goto ErrExit;
		}
	} 
	else 
	{
		/*-----------------------------------------------------------*/
		if (pub_base_CalAcm(g_dd_mst.lst_date, caldate,
			g_dd_parm.intst_term_type, g_dd_parm.acm_calc_type,
			g_dd_mst.bal, 0.00, &g_dd_mst.intst_acm,
			g_dd_mst.intst_type, g_dd_parm.intst_mon, g_dd_parm.intst_day)) 
		{
			sprintf(acErrMsg, "计算利息积数错误!");
			WRITEMSG
				goto ErrExit;
		}
	}
	sprintf(acErrMsg, "计算利息前 intst_acm=[%lf]", g_dd_mst.intst_acm);
	WRITEMSG

		g_pub_tx.ac_id_type[0] = '1';

	ret = pub_base_CalIntst(&g_pub_intst);
	if (ret != 0) 
	{
		sprintf(acErrMsg, "计算利息错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "A032");
		goto ErrExit;
	}
	sprintf(acErrMsg, "利息为=[%.2lf] 利息税=[%.2lf]",
		g_pub_intst.dealval, g_pub_intst.val);
	WRITEMSG

	/*
	 * 计算利息积数 *以后正算积数了 if(
	 * pub_base_CalAcm(g_dd_mst.lst_date,caldate,
	 * g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
	 * 0.00,g_dd_mst.bal,&g_dd_mst.intst_acm,
	 * g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) ) {
	 * sprintf(acErrMsg,"计算利息积数错误!"); WRITEMSG goto ErrExit; } ************ 
	 */

	sprintf(acErrMsg, "计算利息后 intst_acm=[%lf] ", g_dd_mst.intst_acm);
	WRITEMSG
	T_intst_hst.bic_bal = g_dd_mst.bal;
	T_intst_hst.bic_acm = g_dd_mst.intst_acm;
	T_intst_hst.fct_intst = g_pub_intst.dealval;
	T_intst_hst.intst_tax = g_pub_intst.val;
	T_intst_hst.ic_date = g_dd_mst.ic_date;

	g_dd_mst.intst_acm = 0.00;
	dd_mst_c.intst_acm = 0.00;
	strcpy(g_pub_tx.ac_wrk_ind, "0200000J1");
	g_pub_tx.svc_ind = 1012;
	strcpy(g_pub_tx.hst_ind, "1");
	strcpy(g_pub_tx.add_ind, "1");
	strcpy(g_pub_tx.ac_get_ind, "011");
	strcpy(g_pub_tx.brf, "结息");
	g_pub_tx.tx_amt1 = g_pub_intst.dealval;
	g_dd_mst.ic_date = caldate;
	g_dd_mst.lst_date = g_pub_tx.tx_date;
        
        /***houyi 20110329 下行为避免活期明细的开户行和交易行不一致的问题***/
        strcpy(g_pub_tx.tx_br_no,g_dd_mst.opn_br_no);
        /**houyi end**/
	
        ret = pub_acct_trance_jx();
	MYERR

	strcpy(g_pub_tx.add_ind, "0");
	g_pub_tx.tx_amt1 = g_pub_intst.val;
	strcpy(g_pub_tx.brf, "扣税");

	ret = pub_acct_trance_jx();
	MYERR
	/**
	 *ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",
	 *g_dd_mst.ac_id, g_dd_mst.ac_seqn);
	 *MYERR
	 *ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
	 *MYERR
	 *ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply);
	 *Dd_mst_Clo_Upd();*
	 */
	/*--- hao --- @20050915@ ---- 登记结息登记薄(要求在数据库中保留明细) ----*/
	strcpy(T_intst_hst.br_no, g_dd_mst.opn_br_no);
	T_intst_hst.ac_id = g_dd_mst.ac_id;
	T_intst_hst.ac_seqn = g_dd_mst.ac_seqn;
	strcpy(T_intst_hst.prdt_no, g_dd_mst.prdt_no);
	strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
	T_intst_hst.aic_bal = g_dd_mst.bal;
	T_intst_hst.aic_acm = g_dd_mst.intst_acm;
	T_intst_hst.intst_date = caldate;
	strcpy(T_intst_hst.ac_ind, "1");
	strcpy(T_intst_hst.pt_ind, "1");
	T_intst_hst.rate = g_dd_mst.rate;
	ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
	MYERR

		return 0;
	ErrExit:
		return 1;
}
int sub_intst_acct(dd_mst_c, dd_parm_c, sum_intst)
struct dd_mst_c dd_mst_c;
struct dd_parm_c dd_parm_c;
struct S_pub_intst sum_intst;
{
	int             ret = 0;

	vtcp_log("汇总记帐[%s][%.2lf]", dd_parm_c.prdt_no, sum_intst.dealval);

	strcpy(g_pub_tx.tx_br_no, dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.opn_br_no, dd_mst_c.opn_br_no);
	strcpy(g_pub_tx.cur_no, dd_parm_c.cur_no);
	strcpy(g_pub_tx.ac_wrk_ind, "0000000");
	g_pub_tx.svc_ind = 1005;
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.add_ind, "1");
	strcpy(g_pub_tx.prdt_code, dd_parm_c.prdt_no);
	strcpy(g_pub_tx.ac_no, "");
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.note_type, "");
	strcpy(g_pub_tx.beg_note_no, "");
	g_pub_tx.tx_amt1 = sum_intst.dealval;
	strcpy(g_pub_tx.brf, "结息");
	strcpy(g_pub_tx.no_show, "0");

	/*** 调用会计分录自动记 ***
	 *ret=pub_ins_trace_log();
	 *if( ret ) goto ErrExit;
	 ****/
	strcpy(g_pub_tx.add_ind, "0");
	strcpy(g_pub_tx.ac_wrk_ind, "0000000");
	strcpy(g_pub_tx.hst_ind, "1");
	g_pub_tx.svc_ind = 1001;
	strcpy(g_pub_tx.brf, "扣税");
	g_pub_tx.tx_amt1 = sum_intst.val;

	/*** 调用会计分录自动记 ***
	 *ret=pub_ins_trace_log();
	 *if( ret ) goto ErrExit;
	 ****/

	/* 进行会计记帐 */
	set_zd_data(DC_CODE, dd_parm_c.dc_code);
	set_zd_double("102P", sum_intst.dealval);
	set_zd_double("102Q", sum_intst.val);
	set_zd_data("102J", dd_parm_c.cur_no);
	strcpy(g_pub_tx.prdt_code, dd_mst_c.prdt_no);

	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (ret != 0) 
	{
		sprintf(acErrMsg, "会计记帐不成功!!");
		WRITEMSG
			goto ErrExit;
	}
	return 0;
	ErrExit:
		return 1;
}
