/*************************************************
* �� �� ��: spC315.c
* ������������˾�ɶ���Ϣά��
*
* ��    ��: zj
* ������ڣ�2014��10��21��
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1315', '��˾�ɶ���Ϣά��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1315', 0, 'C315', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C315', '��˾�ɶ���Ϣά��', 'spC315', '0', '0');
*
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_shareholder_inf_c.h"


int spC315()
{
		int iRet = 0;
		int ret = 0;
		char cFlag[2];
		char cTbwhere[50];
		
		struct cif_shareholder_inf_c sCif_shareholder_inf;
		struct cif_shareholder_inf_c sCif_shareholder_inf1;
		
		pub_base_sysinit();
		
		memset(cTbwhere,0x00,sizeof(cTbwhere));
		memset(cFlag,0x00,sizeof(cFlag));
		memset(&sCif_shareholder_inf1,0x00,sizeof(sCif_shareholder_inf1));
		memset(&sCif_shareholder_inf,0x00,sizeof(sCif_shareholder_inf));

			
		get_zd_data("0700",cFlag);
		get_zd_data("0260",sCif_shareholder_inf.name);
		get_zd_long("0280",&sCif_shareholder_inf.cif_no);
		get_zd_long("0290",&sCif_shareholder_inf.shareholder_no);
		get_zd_data("0370",sCif_shareholder_inf.br_no);
		get_zd_data("0380",sCif_shareholder_inf.ag_cr_no);
		get_zd_double("0400",&sCif_shareholder_inf.rate);
		get_zd_data("0670",sCif_shareholder_inf.type);
		get_zd_data("0680",sCif_shareholder_inf.sex);
		get_zd_data("0690",sCif_shareholder_inf.id_type);
		get_zd_data("0620",sCif_shareholder_inf.id_no);

		switch(cFlag[0])
		{
			case '1':
						iRet = Cif_shareholder_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%d and shareholder_no=%d ",\
						                              sCif_shareholder_inf.cif_no,sCif_shareholder_inf.shareholder_no);	
						if(iRet)
						{
							sprintf(acErrMsg,"DECLARE ��˾�ɶ���Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Cif_shareholder_inf_Fet_Upd(&sCif_shareholder_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH ��˾�ɶ���Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sCif_shareholder_inf.up_date=g_pub_tx.tx_date;
						memcpy(sCif_shareholder_inf.rowid, sCif_shareholder_inf1.rowid,sizeof(sCif_shareholder_inf1.rowid));
						iRet = Cif_shareholder_inf_Upd_Upd(sCif_shareholder_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  ��˾�ɶ���Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}	
						
						Cif_shareholder_inf_Clo_Upd();
						break;
		
			case '2':
						sprintf(cTbwhere,"cif_no=%d and shareholder_no=%d ",\
						                  sCif_shareholder_inf.cif_no,sCif_shareholder_inf.shareholder_no);
						iRet = deldb("cif_shareholder_inf",cTbwhere);
						if(iRet)
						{
							sprintf(acErrMsg,"ɾ����˾�ɶ���Ϣ���쳣!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sCif_shareholder_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"��˾�ɶ���Ϣ�޸�");
	else
	strcpy(g_pub_tx.brf,"��˾�ɶ���Ϣɾ��");	
	sprintf(g_pub_tx.ac_no,"%d",sCif_shareholder_inf.shareholder_no);
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��˾�ɶ���Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"��˾�ɶ���Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}
