/**************************************************************
* 文 件 名: spB901.c
* 功能描述：同城交易提出,参照万能记帐交易
* 作    者: 
* 完成日期：2006年8月18日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "prdt_ac_id_c.h"
#include "accountreg_c.h"
#include "com_branch_c.h"

spB901()
{
	char flag[2],trance_type[2];
	char tx_br_no[6];
	char qs_br_no[6];
	char ctx_br_no[6];
	int ret=0;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	struct accountreg_c accountreg;
	struct  com_branch_c     com_branch;
	struct  com_parm_c     com_parm;
	struct  mdm_ac_rel_c     mdm_ac_rel;
	struct  in_mst_c     in_mst;
	

	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&com_parm,0x00,sizeof(struct com_parm_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&g_od_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct in_parm_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&accountreg,0x00,sizeof(struct accountreg_c));
	memset(&com_branch,0x00 , sizeof(struct  com_branch_c));
	memset(tx_br_no,0x00,sizeof(tx_br_no));
	memset(ctx_br_no,0x00,sizeof(ctx_br_no));
	memset(qs_br_no,0x00,sizeof(qs_br_no));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	
	get_zd_data("0680",trance_type);	/* 和前台对应 6表示输入是内部帐或科目  1对公户 */
	get_zd_data("0300",g_pub_tx.ac_no);

    pub_base_old_acno( g_pub_tx.ac_no );   /**对旧帐号的处理**/

	get_zd_data("0210",g_pub_tx.cur_no);
	/**get_zd_int("0340",&g_pub_tx.ac_seqn);**/
	if( trance_type[0]=='1' )
		pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn); /* (活期)币种转换成账户序号 */
	else
		g_pub_tx.ac_seqn=0;
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	get_zd_data("0660",flag);				/** 1-借；2-贷 **/
	if( flag[0]!='1' && flag[0]!='2' )
	{
		sprintf(acErrMsg,"借贷标志错误!! [%s]",flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P404");
		goto ErrExit;
	}
	/*get_zd_data("0880",g_pub_tx.brf); modified by liuxuan 20060817*/
	get_zd_data("0670",g_pub_tx.ct_ind); /* 1现金 2转帐 */
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	strcpy( g_pub_tx.ac_wrk_ind,"100" ); /*不核对凭证号，零金额不计流水、明细  有疑问 liuxuan*/
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	if( strlen(g_pub_tx.ac_no)==3 || strlen(g_pub_tx.ac_no)==5|| strlen(g_pub_tx.ac_no)==7 )
	{	/*** 科目记账 ***/
			get_zd_double("0400",&g_pub_tx.tx_amt1);
			if( flag[0]=='1' )
				strcpy(g_pub_tx.add_ind,"0");		/*** 借方 ***/
			else
				strcpy(g_pub_tx.add_ind,"1");		/*** 贷方 ***/
			g_pub_tx.svc_ind=9001;  				/*** 内部帐存取 ***/
			set_zd_double("1001",g_pub_tx.tx_amt1);
	}
	else 
	{
			/*** 取产品代码，及帐号类别 ***/
			ret=pub_base_ac_prdt(&sPrdt_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"取产品代码错误!");
				WRITEMSG
				goto ErrExit;
			}
			strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
			if( sPrdt_ac_id.ac_id_type[0]=='2' )
			{
			sprintf(acErrMsg,"定期帐号不能作此交易!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P403");
			goto ErrExit;
			}

			if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** 对公活期 ***/
			{
			get_zd_double("0400",&g_pub_tx.tx_amt1);
			if( trance_type[0]!='1' )
			{
				sprintf(acErrMsg,"活期账户，请选择活期交易类型!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P409");
				goto ErrExit;
			}
			ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",sPrdt_ac_id.prdt_no);
			if( ret )
			{
				sprintf(acErrMsg,"取活期产品定义表错误!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"W013");
				goto ErrExit;
			}
			if( g_dd_parm.cif_type[0]=='1' )
			{
				sprintf(acErrMsg,"请输入对公帐号!! [%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"O165");
				goto ErrExit;
			}

			if( flag[0]=='1' )	strcpy(g_pub_tx.add_ind,"0"); /*** 借方减少 ***/
			else strcpy(g_pub_tx.add_ind,"1");
			g_pub_tx.svc_ind=1001;  				/*活期存取*/

			set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='3' 
				|| sPrdt_ac_id.ac_id_type[0]=='4' )		/*** 贷款透支 ***/
			{
			if( flag[0]=='1' ) 
				strcpy(g_pub_tx.add_ind,"1");		/*** 贷款借方增加 ***/
			else
				strcpy(g_pub_tx.add_ind,"0");
			
			if( trance_type[0]=='2' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
				g_pub_tx.svc_ind=3010;  			/*贷款本金存取*/
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else if( trance_type[0]=='3' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt2);
				g_pub_tx.svc_ind=3020;  			/*贷款表内存取*/
				set_zd_double("1002",g_pub_tx.tx_amt2);
			}
			else if( trance_type[0]=='4' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt3);
				g_pub_tx.svc_ind=3030;  			/*贷款表外存取*/
				set_zd_double("1003",g_pub_tx.tx_amt3);
			}
			else if( trance_type[0]=='5' )
			{
				get_zd_double("0400",&g_pub_tx.tx_amt4);
				g_pub_tx.svc_ind=3040;  			/*贷款表外存取*/
				set_zd_double("1004",g_pub_tx.tx_amt4);
			}
			else
			{
				sprintf(acErrMsg,"贷款账户，请选择贷款交易类型!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P407");
				goto ErrExit;
			}
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** 内部帐 ***/
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
				if( flag[0]=='1' ) strcpy(g_pub_tx.add_ind,"0");
				else	strcpy(g_pub_tx.add_ind,"1");
				g_pub_tx.svc_ind=9001;  /*内部帐存取*/
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else
			{
				sprintf(acErrMsg,"帐号类型错误![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P410");
				goto ErrExit;
			}
	}

		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}
		ret=insert_accountreg(&accountreg);
		if( ret )
		{
			sprintf(acErrMsg,"同城登记薄处理失败!");
			WRITEMSG
			goto ErrExit;
		}
	/*set_zd_data("1204",g_pub_tx.cur_no);*/
	
	/*Add by hxc 091230  用于晋中商行市外支行采用其支行机构22301科目记账，其他支行采用清算行10001的22301科目记账*/
	get_zd_data("0020",tx_br_no);
	sprintf(acErrMsg,"[%s][%d]同城提出清算机构号[%s]",__FILE__,__LINE__,tx_br_no);
	WRITEMSG
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s' and city_no=1",tx_br_no);
    if(ret==0)
    {
        sprintf(acErrMsg,"[%s][%d]查找同城提出清算机构号[%s],属市外支行",__FILE__,__LINE__,com_branch.br_no);
        WRITEMSG
        set_zd_data("0020",tx_br_no);
    }
    else if ( ret == 100 || ret == 1403)
	{
    	sprintf(acErrMsg,"[%s][%d]查找同城提出清算机构号[%s],属市内支行",__FILE__,__LINE__,tx_br_no);
    	WRITEMSG
    	memset(qs_br_no,0x00,sizeof(qs_br_no));
    	memcpy(qs_br_no,"10001",5);
    	set_zd_data("0020",qs_br_no);
    	sprintf(acErrMsg,"[%s][%d]同城提出清算机构号[%s],清算机构[%s]",__FILE__,__LINE__,tx_br_no,qs_br_no);
    	WRITEMSG
	}
	else if ( ret )
	{
		strcpy( g_pub_tx.reply, "D103" );
    	sprintf(acErrMsg,"[%s][%d]查找同城提出清算机构号[%s]出错!\n",__FILE__,__LINE__,tx_br_no);
    	WRITEMSG
		goto ErrExit;
	}
		/*End by hxc 091230   用于晋中商行市外支行采用其支行机构22301科目记账，其他采用清算行10001的22301科目记账*/
    /**取对应的内部账户**/
    if(memcmp(tx_br_no,"11501", 5)!=0){
    	strcpy(ctx_br_no,"10001");
    	}else{
    		strcpy(ctx_br_no,"11501");
    		}
    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=9999",ctx_br_no);/********取出总行对应的内部账户和介休对应的内部账户***********/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]承兑付款临时帐户错误!cnt=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W025");
        WRITEMSG
        goto ErrExit;
    }
    /** 检查本票签发户是否存在 **/
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",com_parm.val);/******介质与账户关系表，这里是在找ac_id*************/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]检查22301分户失败!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id='%d'",mdm_ac_rel.ac_id);/********内部台账主文件****************/
    if(ret)
    {
        sprintf(acErrMsg,"[%s][%d]22301分户错误!sqlcode=[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"B117");
        WRITEMSG
        goto ErrExit;
    }
    if( in_mst.sts[0]!='1' && in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]",in_mst.ac_id,in_mst.sts);
        strcpy(g_pub_tx.reply,"P109");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("0310",com_parm.val);/**内部账户**/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"同城提出记帐成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"同城提出记帐失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int insert_accountreg(struct accountreg_c *accountreg)
{
	int  ret=0;
	char tmp_no[20];
	struct accountreg_c tmp_accountreg;
	memset(tmp_no,'\0',sizeof(tmp_no));
	get_zd_data("0020",accountreg->br_no);
	get_zd_data("0810",accountreg->br_name);
	get_zd_long("0040",&accountreg->trace_no);
	get_zd_long("0050",&accountreg->tx_date);
	get_zd_long("0060",&accountreg->tx_time);
	get_zd_data("0070",accountreg->tel);
	get_zd_data("0080",accountreg->chk);
	get_zd_data("0870",tmp_no);	/* 提出时放付款行交换号,提入时放收款行 */
	pub_base_strpack(tmp_no);
	get_zd_data("0690",accountreg->put_flag); /* 1提出  2提入 */
	if(accountreg->put_flag[0]=='1'){
		memcpy(accountreg->change_no,tmp_no,strlen(tmp_no));/* 付款行交换号 */
		strcpy(accountreg->pay_ac_no,g_pub_tx.ac_no);/* 付款帐号 */
		pub_base_strpack(accountreg->pay_ac_no);
		get_zd_data("0250",accountreg->pay_name);/* 付款帐号 */
		pub_base_strpack(accountreg->pay_name);
		get_zd_data("0961",accountreg->cash_ac_no); /* 收款人帐号 */
		pub_base_strpack(accountreg->cash_ac_no);
		vtcp_log("accountac_no![%s]",accountreg->cash_ac_no);
		get_zd_data("0962",accountreg->cash_name); /*  收款人名称 */
		pub_base_strpack(accountreg->cash_name);
		vtcp_log("accountac_name![%s]",accountreg->cash_name);
		get_zd_data("0963",accountreg->cash_chg_no); /* 提入行 */
		pub_base_strpack(accountreg->cash_chg_no);
		get_zd_data("0964",accountreg->cash_opn_name); /* 提入行名称 */
		pub_base_strpack(accountreg->cash_opn_name);
	}
	else{
		memcpy(accountreg->cash_chg_no,tmp_no,strlen(tmp_no));/* 收款行交换号 */
		strcpy(accountreg->cash_ac_no,g_pub_tx.ac_no);/* 收款帐号 */
		pub_base_strpack(accountreg->cash_ac_no);
		get_zd_data("0250",accountreg->cash_name);/* 收款名称 */
		pub_base_strpack(accountreg->cash_name);
		get_zd_data("0961",accountreg->pay_ac_no); /* 付款人帐号 */
		pub_base_strpack(accountreg->pay_ac_no);
		vtcp_log("accountac_no![%s]",accountreg->pay_ac_no);
		get_zd_data("0962",accountreg->pay_name); /*  付款人名称 */
		pub_base_strpack(accountreg->pay_name);
		vtcp_log("accountac_name![%s]",accountreg->pay_name);
		get_zd_data("0963",accountreg->change_no);	/* 付款行交换号 */
		pub_base_strpack(accountreg->change_no);
		get_zd_data("0964",accountreg->cash_opn_name); /* 付款行名称 */
		pub_base_strpack(accountreg->cash_opn_name);
	}
	get_zd_data("0700",accountreg->stat); /* 1登记  2记帐  8已打印  9取消 */
	get_zd_data("0890",accountreg->note_type);
	get_zd_data("0580",accountreg->note_no);
	accountreg->amt=g_pub_tx.tx_amt1;
	memset(&tmp_accountreg,'\0',sizeof(tmp_accountreg));
	memcpy(&tmp_accountreg,accountreg,sizeof(tmp_accountreg));
	ret = Accountreg_Ins(tmp_accountreg, g_pub_tx.reply );
	Accountreg_Debug(&tmp_accountreg);
	if ( ret )
		return(1);

	return 0;
ErrExit:
	return 1;
}
