/*************************************************
* �� �� ��: spC316.c
* ������������˾�߹���Ϣά��
*
* ��    ��: zj
* ������ڣ�2014��10��21��
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1316', '��˾�߹���Ϣά��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1316', 0, 'C316', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C316', '��˾�߹���Ϣά��', 'spC316', '0', '0');
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
							sprintf(acErrMsg,"DECLARE ��˾�߹���Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Cif_manage_inf_Fet_Upd(&sCif_manage_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH ��˾�߹���Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sCif_manage_inf.up_date=g_pub_tx.tx_date;
						memcpy(sCif_manage_inf.rowid, sCif_manage_inf1.rowid,sizeof(sCif_manage_inf1.rowid));
						iRet = Cif_manage_inf_Upd_Upd(sCif_manage_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  ��˾�߹���Ϣ��ʧ�ܣ�");
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
							sprintf(acErrMsg,"ɾ����˾�߹���Ϣ���쳣!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sCif_manage_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"��˾�߹���Ϣ�޸�");
	else
	strcpy(g_pub_tx.brf,"��˾�߹���Ϣɾ��");	
	sprintf(g_pub_tx.ac_no,"%d",sCif_manage_inf.manage_no);
	
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��˾�߹���Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"��˾�߹���Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}

