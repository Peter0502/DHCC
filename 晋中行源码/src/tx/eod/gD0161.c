/*************************************************************
* 文 件 名: gD0161.c
* 功能描述：七天乐自动划款，每周日执行
* 					
*		签约七天乐业务后，主帐户余额大于留存金额+虚拟帐户的最小存款额（这里是5W）
						周末晚上翻牌后主帐户余额-留存金额后取1000的整数倍后
转到虚拟帐户的户上
*						在虚拟帐户余额大于5W时只考虑留存金额，这时划转金额不用大于5W
*						记帐分录：主帐户：借方  分录码LC0300
*													虚拟帐户：贷方  分录码LC0200（用的是dc_entry现有的）
* 作    者: wk
* 完成日期: 
*
*			sql: 
insert into com_eod_exec values('1046','Z161','gD0161','七天乐日终划款','1111111111','0','0','00','00','2','Y','Y','N','2','Y');
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYRETERR if(ret){ \
        sprintf(acErrMsg,"sql erro!! [%d]",ret); \
        WRITEMSG \
        return ret; \
	}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
/**#include "mo_prot_chg_hst_c.h"**/


gD0161()
{
	int             ret = 0;
	struct com_sys_parm_c com_sys_parm_c;
	struct mo_prdt_prot_c mo_prdt_prot;

	memset(&com_sys_parm_c, 0x00, sizeof(struct com_sys_parm_c));
	memset(&mo_prdt_prot, 0x00, sizeof(struct mo_prdt_prot_c));
	memset(&g_pub_tx, 0x00, sizeof(struct pub_tx));

	ret = sql_begin();	/* 打开事务 */
	MYRETERR

	/**------- 初始化全局变量 --------**/
	memset(&g_pub_tx, 0x00, sizeof(struct pub_tx));

	/* 查询系统参数表 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply, &com_sys_parm_c, "1=1");
	MYRETERR
	
	if(pub_base_day_week(com_sys_parm_c.lst_date)!=99)
	{
		sprintf(acErrMsg, "不是周末，不划款!");
		WRITEMSG
		goto OkExit;	
	}

	g_pub_tx.tx_date = com_sys_parm_c.sys_date;
	strcpy(g_pub_tx.cur_no, RMB);
	strcpy(g_pub_tx.tx_code, "Z161");
	strcpy(g_pub_tx.sub_tx_code, "Z161");
	g_pub_tx.trace_cnt = 0;
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.tel, "9999");


	ret = Mo_prdt_prot_Dec_Sel1(g_pub_tx.reply,"prot_type='TK7' and sts!='*' order by tx_br_no",g_pub_tx.tx_date);
	MYRETERR

	while (1) 
	{
		memset(&mo_prdt_prot, 0x00, sizeof(struct mo_prdt_prot_c));
		ret = Mo_prdt_prot_Fet_Sel1(&mo_prdt_prot, g_pub_tx.reply);
		if (ret == 100)
			break;
		else if (ret) {
			sprintf(acErrMsg, "mo_prdt_prot_fet_select error!! [%d]", ret);
			WRITEMSG
			return ret;
		}
		memset(g_pub_tx.brf,0x0,sizeof(g_pub_tx.brf));
		strcpy(g_pub_tx.brf, "七天通知存款划款");
		strcpy(g_pub_tx.opn_br_no, mo_prdt_prot.tx_br_no);
		strcpy(g_pub_tx.tx_br_no, mo_prdt_prot.tx_br_no);
		if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply)) {
       sprintf(acErrMsg, "取主机流水号错 [%d]", g_pub_tx.trace_no);
       WRITEMSG
       goto ErrExit;
   }

		ret = pub_eod_prot_jz(mo_prdt_prot);
		if (ret != 0) {
			sprintf(acErrMsg, "日终理财函数错误!!");
			WRITEMSG
			goto ErrExit;
		}
		
	}
	Mo_prdt_prot_Clo_Sel1();

OkExit:
	sql_commit();		/*--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "日终理财处理成功!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0') {
		strcpy(g_pub_tx.reply, "G009");
	}
	sql_rollback();		/*--事务回滚--*/
	sprintf(acErrMsg, "日终理财处理失败!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}


int pub_eod_prot_jz(struct mo_prdt_prot_c mo_prdt_prot_c)
{
	int ret;
	double Tamt=0.00;
	double Tamt_zz=0.00;
	struct dd_parm_c dd_parm_c;
	struct mo_prot_ac_rel_c mo_prot_ac_rel_c;
	struct dd_mst_c dd_mst_app;
	struct dd_mst_c dd_mst_c;
	struct com_sys_parm_c com_sys_parm;
	/**struct mo_prot_chg_hst_c mo_prot_chg_hst_c;**/

	ret=0;
	memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c) );
	memset(&dd_mst_c,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_mst_app,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_parm_c,0x00,sizeof(struct dd_parm_c) );
	memset(&com_sys_parm,0x00,sizeof(struct com_sys_parm_c) );
	/**memset(&mo_prot_chg_hst_c,0x00,sizeof(struct mo_prot_chg_hst_c) );**/

	
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_c,"ac_id=%d ",
	    mo_prdt_prot_c.main_ac_id);
	if ( ret )
	{
		sprintf(acErrMsg,"取活期主文件错误 [%d][%d][%d]",
		    mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
		WRITEMSG
		    return 1;
	}
	if(dd_mst_c.hold_sts[0]=='1'){
	sprintf(acErrMsg,"帐户处于完全冻结状态，下一个 [%d]",dd_mst_c.ac_id);
		WRITEMSG
		    return 0;
		
	}
	/** 加入冻结状态为2 jk**/
        if(dd_mst_c.hold_sts[0]=='2'){
        sprintf(acErrMsg,"帐户处于只进不出冻结状态，下一个 [%d]",dd_mst_c.ac_id);
                WRITEMSG
                    return 0;

        }

	strcpy(g_pub_tx.name,dd_mst_c.name);
	
	ret=Dd_parm_Sel(g_pub_tx.reply,&dd_parm_c,"prdt_no='%s'",dd_mst_c.prdt_no);
	if ( ret )
	{
		sprintf(acErrMsg,"查询产品表出错 [%d][%s]",ret,dd_mst_app.prdt_no);
		WRITEMSG
		    return 1;
	}
	ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&mo_prot_ac_rel_c,"main_ac_id=%ld and main_ac_seqn=%d and prot_type='TK7' and sts!='*' ",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
	if ( ret )
	{
		sprintf(acErrMsg,"签约关系对照表里无此记录 [%d][%d][%d]",
		    mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
		WRITEMSG
		    return 1;
	}
	
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_app,"ac_id=%d ",mo_prot_ac_rel_c.app_ac_id);
	if ( ret )
	{
		sprintf(acErrMsg,"查询定期表出错 [%d][%d]",ret,mo_prot_ac_rel_c.app_ac_id);
		WRITEMSG
		    return 1;
	}
	
	
	/*******mo_prdt_prot_c.turn_amt是产品的最少存款余额TK7是5W*********/
	if( dd_mst_c.bal-dd_mst_c.ctl_amt-dd_mst_c.hold_amt-mo_prdt_prot_c.leave_amt>=1000.00)
		{
			
			g_pub_tx.tx_amt1=((int)(dd_mst_c.bal-dd_mst_c.ctl_amt-dd_mst_c.hold_amt-dd_parm_c.min_bal-mo_prdt_prot_c.leave_amt)/1000)*1000;/**按1000元的整数倍划转**/
		}
	else 
		{
			sprintf(acErrMsg,"不够化转条件!");
			WRITEMSG
			return 0;
		}
	if(pub_base_CompDblVal(dd_mst_app.bal,0.00)<=0 && pub_base_CompDblVal(g_pub_tx.tx_amt1,50000.00)<0){/***虚拟帐户里没钱,这时最少划50000*****/
				sprintf(acErrMsg,"不够化转条件!");
				WRITEMSG
				return 0;
				}
	/*** 活期户划款处理 ***/
	ret=acct_Z016(mo_prdt_prot_c.main_ac_id,1,1001,"0",g_pub_tx.tx_amt1,"LC03");
	if(ret)
	{
		sprintf(acErrMsg,"主户划款处理出错 [%d][%d][%d]",
		    mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
		WRITEMSG
		    return 1;
	}
	
		/******七天乐存款*********/
	vtcp_log("七天乐存款开始");
	ret=acct_Z016(mo_prot_ac_rel_c.app_ac_id,1,1001,"1",g_pub_tx.tx_amt1,"LC02");
	if(ret)
	{
		sprintf(acErrMsg,"虚拟帐户划款处理出错 [%d][%d][%d]",
		    mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn,ret);
		WRITEMSG
		    return 1;
	}
	/**********登记账户变更登记本*************
	mo_prot_chg_hst_c.tx_date=g_pub_tx.tx_date;
	mo_prot_chg_hst_c.trace_no=g_pub_tx.trace_no;
	mo_prot_chg_hst_c.app_ac_id=mo_prot_ac_rel_c.app_ac_id;
	mo_prot_chg_hst_c.app_ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
	mo_prot_chg_hst_c.old_amt=td_mst_c.bal;
	mo_prot_chg_hst_c.new_amt=g_pub_tx.tx_amt1;
	strcpy(mo_prot_chg_hst_c.tx_code,"Z161");
	strcpy(mo_prot_chg_hst_c.tel,"9999");
	strcpy(mo_prot_chg_hst_c.filler,"周末划款");
	ret=mo_prot_chg_hst_Ins(mo_prot_chg_hst_c,g_pub_tx.reply);
	if(ret){
		sprintf(acErrMsg,"登记账户变更登记本出错 [%d]",ret);
		WRITEMSG
		    return 1;
		}
	****/
	ret = Com_sys_parm_Sel(g_pub_tx.reply, &com_sys_parm, "1=1");
	MYRETERR
	if(mo_prdt_prot_c.sts[0]=='2')
	{
		ret = Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"main_ac_id=%d and main_ac_seqn=%d and sts!='*'",mo_prdt_prot_c.main_ac_id,mo_prdt_prot_c.main_ac_seqn);
		MYRETERR
		ret = Mo_prdt_prot_Fet_Upd(&mo_prdt_prot_c,g_pub_tx.reply);
		if(ret)
			{
				sprintf(acErrMsg,"fet mo_prdt_prot表出错!");
				WRITEMSG
				return 1;
			}
		mo_prdt_prot_c.sts[0]='1';
		mo_prdt_prot_c.tx_date=com_sys_parm.sys_date;
		ret = Mo_prdt_prot_Upd_Upd(mo_prdt_prot_c,g_pub_tx.reply);
		if(ret)
			{
				sprintf(acErrMsg,"update mo_prdt_prot表出错!");
				WRITEMSG
				return 1;
			}
		Mo_prdt_prot_Clo_Upd();
	}
	
	  /***zgf add 20130725**/  
    pub_mob_sendmail(  g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, dd_mst_c.bal );
    /***zgf add end**/   
	
	return 0;
	
}

int acct_Z016(long ac_id,int ac_seqn,long deal_code,char *add_ind,double amt,char *sub_tx_code)
{
	strcpy(g_pub_tx.ac_no,"");
	g_pub_tx.ac_id=ac_id;
	g_pub_tx.ac_seqn=ac_seqn;
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.add_ind,add_ind); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	g_pub_tx.svc_ind=deal_code; /**处理代码***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.ac_id_type,"");
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");

	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}
	set_zd_data("102K",g_pub_tx.ct_ind);
	/***修改为使用100域第2子域 modify at 20130517
	set_zd_double("102I",amt);
	****/
	set_zd_double("1002",amt);
	if ( pubf_acct_proc(sub_tx_code) )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}
	return 0;	
}
