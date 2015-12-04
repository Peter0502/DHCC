/*************************************************************
* 文 件 名: sp8889.c
* 功能描述：短信解约单笔扣费计算
*
* 作    者: xujian
* 完成日期: 2010-12-26 15:09
* 功    能:          接收前台传的扣费帐户、金额、解约账号等要素，校验扣费帐户，修改签约表中lst_fee_date字段，
*                    借扣费帐户、贷短信服务业务收入
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:
* insert into tx_def values('8889','短信银行解约扣费计算','10000000000000010000000010000000000010011000000100000000000000000000111100000000000001000000000000000000000000000000000000000100','0','5','0','3','');
* insert into tx_flow_def values('8889','0','8889','#$');
* insert into tx_sub_def values('8889','短信银行解约扣费计算','sp8889','0','0');
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"
#include "com_item_c.h"
#include "mdm_ac_rel_c.h"
#include "mob_server_type_c.h"
#include "com_branch_c.h"
#include "mob_fee_mx_c.h"
#include "com_parm_c.h"
#include "mob_acct_type_hst_c.h"

sp8889()
{
	int  months, months2;
	int feetype, counttype ; /*feetype 0--预算,1--实际计费 counttype 0 --按日计费 */
	int  iDate,iMon,iYear;
	double dSum,dGot;/**合计金额，收取金额，未收取金额**/
	double dAmt_should,dAmt_fact;/**单笔应收，单笔实收**/
	double bal;
	long cnt;
	char dc_ind[2], ct_ind[2] ,fee_ind[2] ;
	char cOpt[2];/**操作类型0开通1修改2注销**/
	struct dd_mst_c stDd_mst;
	struct mdm_ac_rel_c stMdm_ac_rel;
	struct com_item_c stCom_item;
	struct mob_acct_type_c stMob_acct_type;
	struct mob_fee_mx_c sMob_fee;
	 struct com_parm_c stCom_parm;
	
	memset(&stCom_item,0x00,sizeof(stCom_item));
	memset(&stMdm_ac_rel,0x00,sizeof(stMdm_ac_rel));
	memset(&stDd_mst,0x00,sizeof(stDd_mst));
	memset(&stMob_acct_type,0x00,sizeof(stMob_acct_type));
	memset(&stCom_parm, 0x00, sizeof(stCom_parm));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(cOpt,0x00,sizeof(cOpt));

	
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"%s","初始化公用结构错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no);   /*签约帐号*/
	get_zd_data("0660",dc_ind);		/*借贷标志 1--借 2---贷*/
	get_zd_data("0680",ct_ind);		/*现转标志 1--现金 2--转账*/
	get_zd_data("0720",cOpt);

	get_zd_data("0790",g_pub_tx.draw_pwd); /*支付密码*/
	get_zd_data("0310",g_pub_tx.ac_no1);   /*扣款帐号*/
	
	zip_space(g_pub_tx.crd_no);

	/** modify by liaohx 2011-2-23 16:31:47 不打印密码明文
	vtcp_log("%s,%d,操作类型[%s],签约账号[%s]\n",__FILE__,__LINE__,cOpt,g_pub_tx.crd_no);
	**/
	vtcp_log("%s,%d,操作类型[%s],签约账号[%s]\n",__FILE__,__LINE__,cOpt,g_pub_tx.crd_no);
	
	if(strcmp(cOpt,"2") !=0 )  /*非解约暂不收费*/
	{
		vtcp_log("%s,%d,非解约暂不收费[%s]\n",__FILE__,__LINE__,cOpt);
		goto OkExit;
	}

	dSum=0.0;
	dGot=0.0;
	cnt=0;
	feetype = 0 ;    /* 0--预算 */
	counttype=0 ;   /* 1 --按日计费 */
	dAmt_should=dAmt_fact=0.0;

		
	g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&stMob_acct_type,"(ac_no='%s' or filler='%s') ",g_pub_tx.crd_no,g_pub_tx.crd_no);
	if(g_reply_int)
	{
	    vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	    strcpy(g_pub_tx.reply,"D104");
	    goto ErrExit ;
	}
      
    vtcp_log("[%s][%d]stMob_acct_type.server_type [%s],counttype=[%d]，stMob_acct_type.sfbz[%s]",__FILE__,__LINE__,stMob_acct_type.server_type,counttype,stMob_acct_type.sfbz);
   
	if(strcmp(stMob_acct_type.sfbz,"1") ==0 && memcmp(stMob_acct_type.mob_sts,"2",1))
	{
		g_reply_int=count_mob_fee(stMob_acct_type,feetype,counttype,dAmt_should) ;/*公函里面set_zd_double( "0410", fee);*/
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]计算手续费出错[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"B296");
			goto ErrExit ;
		}
		cnt++;
	}else if(!memcmp(stMob_acct_type.mob_sts,"2",1)){
		/*计算50天的应扣费金额*/
		g_reply_int=count_mob_fee_pre(stMob_acct_type,9,9,dAmt_should) ;
		if(g_reply_int)
		{
			vtcp_log("[%s][%d]计算手续费出错[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"B296");
			goto ErrExit ;
		}
		get_zd_double("0410",&dAmt_should);
	
	}else{
			vtcp_log("[%s][%d]该签约帐户免费ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
			strcpy(g_pub_tx.reply,"0001");

	}

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	Mob_acct_type_Clo_Sel();
	vtcp_log("[%s][%d]短信扣费处理失败! tx_code=[sp8889],prog_name=[sp8889]",__FILE__,__LINE__);
	set_zd_data("0120", g_pub_tx.reply);
	return g_reply_int;	
}


