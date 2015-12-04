/***************************************************************
* 文 件 名: gD0293.c
* 功能描述: 支行40801转入到清算40801科目
*
* 作    者: ligl
* 完成日期: 2006-11-3 16:11
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
#include "com_parm_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
char  def_br_no[6];
char  def_tel[5];
char  cTestbrno[BRNO_LEN+1];
gD0293()
{
   int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_parm_c sCom_parm;
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct gl_sub_c sGl_sub;
	struct com_item_c sCom_item;
	memset(&sGl_sub , 0 , sizeof(sGl_sub));
	memset(&sCom_parm , 0 , sizeof(sCom_parm));
	memset(&sCom_branch, 0  ,sizeof(sCom_branch));
	memset(&sCom_sys_parm, 0 , sizeof(sCom_sys_parm));
	memset(&sCom_item, 0 , sizeof(sCom_item));
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	memset(cTestbrno, 0 , sizeof(cTestbrno));
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(ret != 0)
	{
		sprintf( acErrMsg, "得到交易日期失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	g_pub_tx.tx_date = sCom_sys_parm.sys_date;
	ret=Com_branch_Dec_Sel(g_pub_tx.reply,"br_type='4'");
	if(ret)
	{
		strcpy(acErrMsg,"定义机构游标出错!");
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret=Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
		if(ret==100) break;
		if(ret)
		{
		strcpy(acErrMsg,"读取机构游标出错!");
		WRITEMSG
		goto ErrExit;
		}
		if(strlen(cTestbrno)!=0)
		{
			if(memcmp(cTestbrno,sCom_branch.br_no,BRNO_LEN))
				continue;
		}
		memcpy(g_pub_tx.tel,sCom_branch.br_no+1,2);
		memcpy(g_pub_tx.tel+2,"99",2);
		vtcp_log("%s,%d tel===[%s]",__FILE__,__LINE__,g_pub_tx.tel);
		set_zd_data(DC_TEL,g_pub_tx.tel);
		memcpy(g_pub_tx.tx_br_no,sCom_branch.br_no,5);
		ret=Gl_sub_Dec_Sel(g_pub_tx.reply,"acc_hrt='40801' and br_no='%s'",sCom_branch.br_no);
		if(ret)
		{
		strcpy(acErrMsg,"定义机构总帐游标出错!");
		WRITEMSG
		goto ErrExit;
		}
		while(1)
		{
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			ret=Gl_sub_Fet_Sel(&sGl_sub,g_pub_tx.reply);
			if(ret==100)
			{
				break;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			if(ret)
			{
				strcpy(acErrMsg,"读取帐务游标出错!");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			strncpy(g_pub_tx.brf,"数据过渡转出",sizeof(g_pub_tx.brf));
			
			if(sGl_sub.cr_bal!=0)
			{
				char cAc_no[8];
				char cAcc_hrt[6];
				memset(cAcc_hrt, 0 , sizeof(cAcc_hrt));
				memset(cAc_no, 0 , sizeof(cAc_no));
				/* 贷方处理支行 */
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",sGl_sub.acc_hrt);
				if(ret)
				{
					sprintf(acErrMsg,"根据[%s]查询科目错误",sGl_sub.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				g_pub_tx.tx_amt1=0.0;
				memcpy(g_pub_tx.opn_br_no,sCom_branch.br_no,5);
				g_pub_tx.tx_amt1=-(sGl_sub.cr_bal);
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.add_ind[0]='1';
				memcpy(g_pub_tx.cur_no,"01",2);
				vtcp_log("%s,%d ",__FILE__,__LINE__);
				sprintf(acErrMsg, "处理机构[%s]科目[%s]贷方余额[%.2f]---------->",sCom_branch.br_no,sGl_sub.acc_hrt,sGl_sub.dr_bal);
				WRITEMSG
				set_zd_data("1211",sCom_item.acc_no);
				set_zd_double("1218",g_pub_tx.tx_amt1);/**贷支行负**/
				set_zd_data("121A",g_pub_tx.brf);
				set_zd_data("1215",g_pub_tx.ct_ind); 
				set_zd_data("1214",g_pub_tx.cur_no);
				set_zd_data("0152",sGl_sub.acc_hrt);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret = pubf_acct_proc("A017");
				if (ret != 0)
				{
					sprintf(acErrMsg, "记机构%s的%s科目会计帐错误!! ret=[%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				/* 处理贷方清算 
				memcpy(cAc_no,sGl_sub.acc_hrt,sizeof(cAc_no)-1);
				memcpy(cAcc_hrt,sGl_sub.acc_hrt,sizeof(cAcc_hrt)-1);
				pub_base_strpack(cAc_no);*/
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",sGl_sub.acc_hrt);
				if(ret)
				{
					sprintf(acErrMsg,"根据[%s]查询科目错误",sGl_sub.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				g_pub_tx.tx_amt1=0.0;
				g_pub_tx.tx_amt1=sGl_sub.cr_bal;/**贷清算正**/
				
				g_pub_tx.ct_ind[0]='2';
				g_pub_tx.add_ind[0]='1';
				memcpy(g_pub_tx.cur_no,"01",2);
				memcpy(g_pub_tx.opn_br_no,QS_BR_NO,5);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				sprintf(acErrMsg, "同时处理[%s]到清算科目[%s]---------->",sCom_branch.br_no,cAcc_hrt);
				WRITEMSG
				set_zd_data("1211",sCom_item.acc_no);
				set_zd_double("1218",g_pub_tx.tx_amt1);
				set_zd_data("121A",g_pub_tx.brf);
				set_zd_data("1215",g_pub_tx.ct_ind); 
				set_zd_data("1214",g_pub_tx.cur_no);
				set_zd_data("0152",sGl_sub.acc_hrt);
				vtcp_log(acErrMsg, "%s,%d ----------",__FILE__,__LINE__);
				ret = pubf_acct_proc("A017");
				if (ret != 0)
				{
					sprintf(acErrMsg, "记机构%s的%s科目会计帐错误!! ret=[%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
			}
		}
		Gl_sub_Clo_Sel();
	}
	Com_branch_Clo_Sel();



OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"40801转入40801账号处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"40801转入40801账号处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
