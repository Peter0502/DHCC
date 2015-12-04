/*************************************************
* 文 件 名: spC316.c
* 功能描述：公司高管信息维护
*
* 作    者: zj
* 完成日期：2014年10月21日
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1316', '公司高管信息维护', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1316', 0, 'C316', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C316', '公司高管信息维护', 'spC316', '0', '0');
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
#include "cif_manage_inf_c.h"


int spC316()
{
		int iRet = 0;
		int ret = 0;
		char cFlag[2];
		char cTbwhere[50];
		
		struct cif_manage_inf_c sCif_manage_inf;
		struct cif_manage_inf_c sCif_manage_inf1;
		
		pub_base_sysinit();
		
		memset(cTbwhere,0x00,sizeof(cTbwhere));
		memset(cFlag,0x00,sizeof(cFlag));
		memset(&sCif_manage_inf1,0x00,sizeof(sCif_manage_inf1));
		memset(&sCif_manage_inf,0x00,sizeof(sCif_manage_inf));

			
		get_zd_data("0700",cFlag);
		get_zd_data("0260",sCif_manage_inf.name);
		get_zd_long("0280",&sCif_manage_inf.cif_no);
		get_zd_long("0290",&sCif_manage_inf.manage_no);
		
		get_zd_data("0670",sCif_manage_inf.type);
		get_zd_data("0680",sCif_manage_inf.sex);
		get_zd_data("0690",sCif_manage_inf.id_type);
		get_zd_data("0620",sCif_manage_inf.id_no);

		switch(cFlag[0])
		{
			case '1':
						iRet = Cif_manage_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%d and manage_no=%d ",\
						                              sCif_manage_inf.cif_no,sCif_manage_inf.manage_no);	
						if(iRet)
						{
							sprintf(acErrMsg,"DECLARE 公司高管信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Cif_manage_inf_Fet_Upd(&sCif_manage_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH 公司高管信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sCif_manage_inf.up_date=g_pub_tx.tx_date;
						memcpy(sCif_manage_inf.rowid, sCif_manage_inf1.rowid,sizeof(sCif_manage_inf1.rowid));
						iRet = Cif_manage_inf_Upd_Upd(sCif_manage_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  公司高管信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}	
						
						Cif_manage_inf_Clo_Upd();
						break;
		
			case '2':
						sprintf(cTbwhere,"cif_no=%d and manage_no=%d ",\
						                  sCif_manage_inf.cif_no,sCif_manage_inf.manage_no);
						iRet = deldb("cif_manage_inf",cTbwhere);
						if(iRet)
						{
							sprintf(acErrMsg,"删除公司高管信息表异常!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*登记交易流水*/
	g_pub_tx.ac_id = sCif_manage_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"公司高管信息修改");
	else
	strcpy(g_pub_tx.brf,"公司高管信息删除");	
	sprintf(g_pub_tx.ac_no,"%d",sCif_manage_inf.manage_no);
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"公司高管信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"公司高管信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}

