/****************************************************
*程序名称：spB624.c
*程序功能：实现对承兑垫款五级分类状态的调整
*作者    ：lwb
*时间    ：20150518
*insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('5624', '承兑垫款五级状态调整', '10000000000000000000000000010000000000000000001000000000000001100000000000000000000000001000000000000000000000000000000000000000', '1', '4', '2');
*insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('5624', 0, 'B624', '#$');
*insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('B624', '承兑垫款五级状态调整', 'spB624', '0', '0');
*****************************************************/
#include <stdio.h>
#include <string.h>
#include "public.h"
#define EXTERN
#include "mo_bank_acc_po_c.h"
#include "ln_mst_c.h"

int spB624()
{
		char cPact_no[20];
		char cFive_sts[2];
		char cAc_sts[2];
		int  iRet=0;
			
		struct mo_bank_acc_po_c sMo_bank_acc_po;
		struct mdm_ac_rel_c sMdm_ac_rel;
		struct ln_mst_c sLn_mst;
		
		memset(cPact_no,0x00,sizeof(cPact_no));
		memset(cFive_sts,0x00,sizeof(cFive_sts));
		memset(cAc_sts,0x00,sizeof(cAc_sts));
		memset(&sMo_bank_acc_po,0x00,sizeof(sMo_bank_acc_po));
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		
		pub_base_sysinit();
		
		get_zd_data("0630",cPact_no);
		get_zd_data("0680",cFive_sts);
		get_zd_data("0690",cAc_sts);
		
		iRet = Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"pact_no = '%s' ",cPact_no);
		if(iRet&&iRet!=100)
		{
			vtcp_log("查询Mo_bank_acc_po错 ");	
			strcpy(g_pub_tx.reply,"B334");
			goto ErrExit;
		}
		else if(iRet == 100 )
		{
			vtcp_log("无此承兑协议编号 [%s]",cPact_no);	
			strcpy(g_pub_tx.reply,"B199");
			goto ErrExit;
		}
		
		if(sMo_bank_acc_po.acc_po_ind[0]=='0')
		{
			sprintf(acErrMsg,"此承兑协议未到期处理");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto ErrExit;	
		}
		if(strlen(sMo_bank_acc_po.over_ac_no)==0)
		{
			sprintf(acErrMsg,"此承兑协议未生成垫款");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto ErrExit;	
		}
		
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",sMo_bank_acc_po.over_ac_no);
		if(iRet&&iRet!=100)
		{
			vtcp_log("查询Mdm_ac_rel错 ");	
			strcpy(g_pub_tx.reply,"C117");
			goto ErrExit;
		}
		else if(iRet==100)
		{
			vtcp_log("查询Mdm_ac_rel无此记录[%s] ",sMo_bank_acc_po.over_ac_no);	
			strcpy(g_pub_tx.reply,"C117");
			goto ErrExit;	
		}
	
		iRet = Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id = %ld ",sMdm_ac_rel.ac_id);
		if(iRet)
		{
			vtcp_log("查询ln_mst错误[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L021");
			goto ErrExit;			
		}
		
		iRet = Ln_mst_Fet_Upd(&sLn_mst,g_pub_tx.reply);
		if(iRet == 100)
		{
			vtcp_log("查询ln_mst无此记录[%ld] ",sLn_mst.ac_id);	
			strcpy(g_pub_tx.reply,"L034");
			goto ErrExit;			
		}
		else if(iRet)
		{
			vtcp_log("查询ln_mst错误[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L021");
			goto ErrExit;	
		}
		vtcp_log("状态%s",sLn_mst.five_sts);
		sLn_mst.five_sts[0]=cFive_sts[0];
		vtcp_log("状态%s",sLn_mst.five_sts);
		iRet = Ln_mst_Upd_Upd(sLn_mst,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("修改ln_mst错误[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L073");
			goto ErrExit;		
		}
		
		iRet = Ln_mst_Clo_Upd();
		
        /* Pub_tx结构赋值 */
	strcpy(g_pub_tx.ac_no,sMo_bank_acc_po.over_ac_no);
	g_pub_tx.ac_seqn=0;
       strcpy ( g_pub_tx.add_ind , "1" );              /* 增减:0减1加2销户 */
        strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
        strcpy ( g_pub_tx.ct_ind , "2" );               /* 现转:1现2转 */
       /* 是否打印存折(有折无折标志):1是0否 */
        strcpy ( g_pub_tx.prt_ind , "0" );
       /* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
        strcpy ( g_pub_tx.hst_ind , "1" );
       g_pub_tx.svc_ind = 3060;                        /* 贷款五级认定 */
       strcpy ( g_pub_tx.brf,"承兑垫款五级状态调整" );
        sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
        WRITEMSG

        sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
        WRITEMSG

       /*** 交易记帐处理 ***/
        if( pub_acct_trance() )
        {
                sprintf(acErrMsg,"交易记帐处理错误!");
                WRITEMSG	
		goto ErrExit;
	}


OkExit:
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
		return 0;
ErrExit:
		sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
		return 1;
	
}
