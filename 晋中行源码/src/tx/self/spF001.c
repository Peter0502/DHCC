/*************************************************
* 文 件 名:  spF001.c
* 功能描述： ATM 密码修改
*
* 作    者:  rob
* 完成日期： 2003年2月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card_reg_c.h"
#include "chnl_self_trade_c.h"

int spF001()
{

	int ret,pass_cnt=0;
	int atm_qy_flag,atm_tx_flag,pos_qy_flag,pos_tx_flag;
	struct chnl_self_trade_c chnl_self_trade;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct card_reg_c    sCard_reg;
	char pcTmpBuf[512],fee_mode[2];
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));

	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	memset(&s_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&sCard_reg,0,sizeof(struct card_reg_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* 旧支取密码  */
	get_zd_data("0800",pcTmpBuf); /* 新支取密码 */
	memset(fee_mode,0,sizeof(fee_mode));
	get_zd_data("0690",fee_mode);
	memcpy(g_pub_tx.qry_pwd,pcTmpBuf,sizeof(g_pub_tx.qry_pwd)-1);
	sprintf(acErrMsg,"crd_no=[%s]!!",g_pub_tx.crd_no);

  	WRITEMSG

	/* 检查卡的合法性 */
	if (pub_base_CheckCrdNo())
	{
		sprintf(acErrMsg,"检查卡的合法性错误!!");
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("%s,%d,开始检查密码!",__FILE__,__LINE__);
        ret = pub_card_reg(0.00,"GMM",'0',&atm_qy_flag,&pos_qy_flag,&atm_tx_flag,&pos_tx_flag,0,3);
        if(ret)
        {
                vtcp_log("%s,%d,更新卡登记簿错误!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
        }	
        ret = Card_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.crd_no);    
        if(ret)
        {
                vtcp_log("%s,%d,帐号不存在!ret[%d]",__FILE__,__LINE__,ret);
                strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
        }
        ret = Card_reg_Fet_Upd(&sCard_reg,g_pub_tx.reply);      
        if(ret)
        {
                vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
        }
        ret = pub_base_GetParm_long("ATMMM",1,&pass_cnt); 
        if(ret)
        {
                pass_cnt = 10;
                vtcp_log("%s,%d,密码次数不存在!",__FILE__,__LINE__);
                vtcp_log("%s,%d,密码次数不存在,取默认次数[%d]!",__FILE__,__LINE__,pass_cnt);
        }
	/* 检查支取方式的合法性 */
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
		              g_pub_tx.draw_pwd, g_mdm_ac_rel.draw_pwd);
	if (ret != 0)
	{
		sprintf(acErrMsg,"卡的支取密码错误!!! [%s] ?= [%s]",
		    g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
		WRITEMSG 
                vtcp_log("%s,%d更新密码错误次数!",__FILE__,__LINE__);
                if(g_pub_tx.tx_date == sCard_reg.l_tx_date)
                {               
                        sCard_reg.add_pswd_cnt = sCard_reg.add_pswd_cnt + 1;    
                }else{
                        sCard_reg.add_pswd_cnt = 1;     
                }		
		sprintf(acErrMsg,"tx_date=[%ld],   ac_no=[%s]", g_pub_tx.tx_date,
				g_pub_tx.crd_no); 
		WRITEMSG 
                if(sCard_reg.add_pswd_cnt > pass_cnt && fee_mode[0]>'5')
                {
                        /* 修改介质账号对照表
                        ret  = sql_execute2("update mdm_ac_rel set coll_sts='2' where \ 
                        ac_no = '%s' ", g_pub_tx.crd_no); 
                        if (ret != 0) 
                        { 
                                sprintf(acErrMsg, "更新mdm_ac_rel表错误!! [%d]", ret); 
                                WRITEMSG 
                                strcpy(g_pub_tx.reply,"CU91"); 
                                Card_reg_Clo_Upd();
				goto ErrExit;
                        }  
                        sCard_reg.add_pswd_cnt = 0;
                         不设置吞卡标注提示超限*/ 

                        /* atm吞卡响应码 */ 
                        strcpy(g_pub_tx.reply,"CU38"); 
                }else{
                        strcpy(g_pub_tx.reply,"CU55");
                }       
                sCard_reg.l_tx_date= g_pub_tx.tx_date;
                ret = Card_reg_Upd_Upd(sCard_reg,g_pub_tx.reply);
                if(ret)
                {
                        /*当日交易限额*/
                        vtcp_log("%s,%d,更新密码错误次数!",__FILE__,__LINE__);
                        strcpy(g_pub_tx.reply,"CU14");
                        Card_reg_Clo_Upd();
			goto ErrExit;
                }
                Card_reg_Clo_Upd();
                ret = sql_commit(); 
                if (ret != 0) 
                { 
                        sprintf(acErrMsg,"提交事务错误!!"); 
                        WRITEMSG 
                } 
                ret = sql_begin(); 
                if (ret != 0) 
                { 
                        sprintf(acErrMsg,"开始事务错误!!"); 
                        WRITEMSG 
                } 
		goto ErrExit;
	}
        Card_reg_Clo_Upd();
        if(sCard_reg.add_pswd_cnt > pass_cnt && fee_mode[0]>'5')
        {
                vtcp_log("%s,%d,密码次数超限!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"CU38");
		goto ErrExit;
        }
                        
        ret = sql_execute2("update card_reg set add_pswd_cnt=0 where ac_no='%s'",g_pub_tx.crd_no);
        if(ret)
        {
                vtcp_log("%s,%d,更新更新卡登记簿失败![%d],ac_no[%s]",__FILE__,__LINE__,ret,g_pub_tx.crd_no);
                strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
        }
	/* 修改支取方式 */
	ret =Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"declare error !! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	} 

	ret = Mdm_ac_rel_Fet_Upd(&s_mdm_ac_rel,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Fetch error !! [%d]",ret);
		WRITEMSG
		Mdm_ac_rel_Clo_Upd();   
		strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	strcpy(s_mdm_ac_rel.draw_pwd,g_pub_tx.qry_pwd);
	pub_base_EnDes(g_pub_tx.tx_date,g_pub_tx.crd_no,s_mdm_ac_rel.draw_pwd);

	ret = Mdm_ac_rel_Upd_Upd(s_mdm_ac_rel,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Update error !![%d]",ret);
		WRITEMSG
		Mdm_ac_rel_Clo_Upd();   
		strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	Mdm_ac_rel_Clo_Upd();   

	/* 给自助设备交易日记结构赋值 */
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);

	strcpy(g_pub_tx.opn_br_no,s_mdm_ac_rel.opn_br_no);/*add by wyw 20080312*/   
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	strcpy(chnl_self_trade.ttyp,"1");
	/**strcpy(chnl_self_trade.sctp, "AT");**/

	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	/* 本交易对帐中心流水号 */
	get_zd_long("0520", &chnl_self_trade.stra);
	get_zd_long("0440", &chnl_self_trade.dz_date);
	get_zd_data("0240",chnl_self_trade.bbmk); /*用来存放业务类型*/
	strcpy(chnl_self_trade.brf_no,"ATMG");
	strcpy(chnl_self_trade.batch_flag,"0");
	strcpy(chnl_self_trade.stat,"0");
	strcpy(chnl_self_trade.info_source,"0");
	strcpy(chnl_self_trade.brf_dscp,"ATM修改密码");	
	/* 登记自助设备交易日记 */
	ret = pub_base_Insertselftrade(chnl_self_trade);
	if (ret != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT07");
		goto ErrExit;
	}

	/*  登记交易流水 */
	strcpy(g_pub_tx.brf,"ATM修改密码!!");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"密码修改开始写明细!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0 || strlen(g_pub_tx.reply)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

