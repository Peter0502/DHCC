/*************************************************
* 文 件 名: spC318.c
* 功能描述：公司机构信息维护
*
* 作    者: zj
* 完成日期：2014年10月29日
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1318', '公司机构信息维护', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1318', 0, 'C318', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C318', '公司机构信息维护', 'spC318', '0', '0');
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
#include "cif_copa_inf_c.h"


int spC318()
{
		int iRet = 0;
		int ret = 0;
		char cFlag[2];
		char cTbwhere[50];
		
		struct cif_copa_inf_c sCif_copa_inf;
		struct cif_copa_inf_c sCif_copa_inf1;
		
		pub_base_sysinit();
		
		memset(cTbwhere,0x00,sizeof(cTbwhere));
		memset(cFlag,0x00,sizeof(cFlag));
		memset(&sCif_copa_inf,0x00,sizeof(sCif_copa_inf));
		memset(&sCif_copa_inf,0x00,sizeof(sCif_copa_inf1));

			
		get_zd_data("0700",cFlag);
		get_zd_data("0250",sCif_copa_inf.sc_bs);
		get_zd_data("0260",sCif_copa_inf.con_en_name);	
		get_zd_data("0270",sCif_copa_inf.up_br_name);
		get_zd_long("0280",&sCif_copa_inf.cif_no);
		get_zd_data("0300",sCif_copa_inf.ag_cr_no);
		get_zd_data("0820",sCif_copa_inf.or_ag_type);
		get_zd_data("0480",sCif_copa_inf.or_ag_mst_type);
		get_zd_data("0580",sCif_copa_inf.eco_ind_type);
		get_zd_data("0500",sCif_copa_inf.eco_type);
		get_zd_data("0810",sCif_copa_inf.ent_tel);
		get_zd_data("0670",sCif_copa_inf.up_reg_type);
		get_zd_data("0310",sCif_copa_inf.up_reg_no);
		get_zd_data("0600",sCif_copa_inf.up_ag_zz_no);
		get_zd_data("0830",sCif_copa_inf.up_ag_cr_no);
		get_zd_data("0620",sCif_copa_inf.check_no);
		get_zd_data("0630",sCif_copa_inf.reg_ad_area_no);
		get_zd_long("0440",&sCif_copa_inf.beg_date);
		get_zd_long("0450",&sCif_copa_inf.end_date);
		get_zd_data("0710",sCif_copa_inf.cop_sts);

		switch(cFlag[0])
		{
			case '1':
						iRet = Cif_copa_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%d ",\
						                              sCif_copa_inf.cif_no);	
						if(iRet)
						{
							sprintf(acErrMsg,"DECLARE 公司机构信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Cif_copa_inf_Fet_Upd(&sCif_copa_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH 公司机构信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sCif_copa_inf.up_date=g_pub_tx.tx_date;
						memcpy(sCif_copa_inf.rowid, sCif_copa_inf1.rowid,sizeof(sCif_copa_inf1.rowid));
						iRet = Cif_copa_inf_Upd_Upd(sCif_copa_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  公司机构信息表失败！");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}	
						
						Cif_copa_inf_Clo_Upd();
						break;
		
			case '2':
						sprintf(cTbwhere,"cif_no=%d and ",\
						                  sCif_copa_inf.cif_no);
						iRet = deldb("cif_copa_inf",cTbwhere);
						if(iRet)
						{
							sprintf(acErrMsg,"删除公司机构信息表异常!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*登记交易流水*/
	g_pub_tx.ac_id = sCif_copa_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"公司机构信息修改");
	else
	strcpy(g_pub_tx.brf,"公司机构信息删除");	
	
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"公司机构信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"公司机构信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}



