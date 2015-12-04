/*************************************************
* �� �� ��: spC317.c
* ����������������ҵ��Ϣά��
*
* ��    ��: zj
* ������ڣ�2014��10��22��
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1317', '������ҵ��Ϣά��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1317', 0, 'C317', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C317', '������ҵ��Ϣά��', 'spC317', '0', '0');
*
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
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
							sprintf(acErrMsg,"DECLARE ������ҵ��Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Join_cop_inf_Fet_Upd(&sJoin_cop_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH ������ҵ��Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sJoin_cop_inf.up_date=g_pub_tx.tx_date;
						memcpy(sJoin_cop_inf.rowid, sJoin_cop_inf1.rowid,sizeof(sJoin_cop_inf1.rowid));
						iRet = Join_cop_inf_Upd_Upd(sJoin_cop_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  ������ҵ��Ϣ��ʧ�ܣ�");
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
							sprintf(acErrMsg,"ɾ��������ҵ��Ϣ���쳣!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sJoin_cop_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"������ҵ��Ϣ�޸�");
	else
	strcpy(g_pub_tx.brf,"������ҵ��Ϣɾ��");	
	sprintf(g_pub_tx.ac_no,"%d",sJoin_cop_inf.cop_no);
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������ҵ��Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"������ҵ��Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}


