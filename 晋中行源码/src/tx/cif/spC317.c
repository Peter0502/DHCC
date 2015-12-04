/*************************************************
* 文 件 名: spC317.c
* 功能描述：关联企业信息维护
*
* 作    者: zj
* 完成日期：2014年10月22日
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1317', '关联企业信息维护', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1317', 0, 'C317', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C317', '关联企业信息维护', 'spC317', '0', '0');
*
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*  
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "join_cop_inf_c.h"


int spC317()
{
		int iRet = 0;
		int ret = 0;
		char cFlag[2];
		char cTbwhere[50];
		
		struct join_cop_inf_c sJoin_cop_inf;
		struct join_cop_inf_c sJoin_cop_inf1;
		
		pub_base_sysinit();
		
		memset(cTbwhere,0x00,sizeof(cTbwhere));
		memset(cFlag,0x00,sizeof(cFlag));
		memset(&sJoin_cop_inf1,0x00,sizeof(sJoin_cop_inf1));
		memset(&sJoin_cop_inf,0x00,sizeof(sJoin_cop_inf));

			
		get_zd_data("0700",cFlag);
		get_zd_data("0260",sJoin_cop_inf.name);
		get_zd_long("0280",&sJoin_cop_inf.cif_no);
		get_zd_long("0290",&sJoin_cop_inf.cop_no);
		
		get_zd_data("0670",sJoin_cop_inf.type);
		get_zd_data("0690",sJoin_cop_inf.id_type);
		get_zd_data("0620",sJoin_cop_inf.id_no);
		
		get_zd_data("0370",sJoin_cop_inf.br_no);
		get_zd_data("0380",sJoin_cop_inf.ag_cr_no);

		switch(cFlag[0])
		{
			case '1':
						iRet = Join_cop_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%d and cop_no=%d ",\
						                              sJoin_cop_inf.cif_no,sJoin_cop_inf.cop_no);	
						if(iRet)
						{
							sprintf(acErrMsg,"DECLARE 关联企业信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Join_cop_inf_Fet_Upd(&sJoin_cop_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH 关联企业信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sJoin_cop_inf.up_date=g_pub_tx.tx_date;
						memcpy(sJoin_cop_inf.rowid, sJoin_cop_inf1.rowid,sizeof(sJoin_cop_inf1.rowid));
						iRet = Join_cop_inf_Upd_Upd(sJoin_cop_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  关联企业信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}	
						
						Join_cop_inf_Clo_Upd();
						break;
		
			case '2':
						sprintf(cTbwhere,"cif_no=%d and cop_no=%d ",\
						                  sJoin_cop_inf.cif_no,sJoin_cop_inf.cop_no);
						iRet = deldb("join_cop_inf",cTbwhere);
						if(iRet)
						{
							sprintf(acErrMsg,"删除关联企业信息表异常!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*登记交易流水*/
	g_pub_tx.ac_id = sJoin_cop_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"关联企业信息修改");
	else
	strcpy(g_pub_tx.brf,"关联企业信息删除");	
	sprintf(g_pub_tx.ac_no,"%d",sJoin_cop_inf.cop_no);
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"关联企业信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"关联企业信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}


