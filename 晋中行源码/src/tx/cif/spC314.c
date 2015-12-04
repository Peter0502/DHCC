/*************************************************
* �� �� ��: spC314.c
* �����������ͻ���ͥ��Ϣά��
*
* ��    ��: lwb
* ������ڣ�2014��10��20��
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1314', '�ͻ���ͥ��Ϣά��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1314', 0, 'C314', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C314', '�ͻ���ͥ��Ϣά��', 'spC314', '0', '0');
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
#include "cif_family_inf_c.h"


int spC314()
{
		int iRet = 0;
		int ret = 0;
		char cFlag[2];
		char cTbwhere[50];
		
		struct cif_family_inf_c sCif_family_inf;
		struct cif_family_inf_c sCif_family_inf1;
		
		pub_base_sysinit();
		
		memset(cTbwhere,0x00,sizeof(cTbwhere));
		memset(cFlag,0x00,sizeof(cFlag));
		memset(&sCif_family_inf1,0x00,sizeof(sCif_family_inf1));
		memset(&sCif_family_inf,0x00,sizeof(sCif_family_inf));

			
		get_zd_data("0700",cFlag);
		get_zd_data("0260",sCif_family_inf.name);
		get_zd_long("0280",&sCif_family_inf.cif_no);
		get_zd_long("0290",&sCif_family_inf.manage_no);
		get_zd_long("0340",&sCif_family_inf.family_no);
		get_zd_data("0260",sCif_family_inf.name);
		get_zd_data("0670",sCif_family_inf.relation);
		get_zd_data("0680",sCif_family_inf.sex);
		get_zd_data("0690",sCif_family_inf.id_type);
		get_zd_data("0620",sCif_family_inf.id_no);

		switch(cFlag[0])
		{
			case '1':
						iRet = Cif_family_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%d and manage_no=%d and family_no=%d ",\
						                              sCif_family_inf.cif_no,sCif_family_inf.manage_no,sCif_family_inf.family_no);	
						if(iRet)
						{
							sprintf(acErrMsg,"DECLARE �ͻ���ͥ��Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}
						
						iRet = Cif_family_inf_Fet_Upd(&sCif_family_inf1,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg," FETCH �ͻ���ͥ��Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;									
						}
						sCif_family_inf.up_date=g_pub_tx.tx_date;
						memcpy(sCif_family_inf.rowid, sCif_family_inf1.rowid,sizeof(sCif_family_inf1.rowid));
						iRet = Cif_family_inf_Upd_Upd(sCif_family_inf,g_pub_tx.reply);
						if(iRet)
						{
							sprintf(acErrMsg,"UPDATE  �ͻ���ͥ��Ϣ��ʧ�ܣ�");
							WRITEMSG
							strcpy(g_pub_tx.reply,"C006");
							goto ErrExit;	
						}	
						
						Cif_family_inf_Clo_Upd();
						break;
		
			case '2':
						sprintf(cTbwhere,"cif_no=%d and manage_no=%d and family_no=%d ",\
						                  sCif_family_inf.cif_no,sCif_family_inf.manage_no,sCif_family_inf.family_no);
						iRet = deldb("cif_family_inf",cTbwhere);
						if(iRet)
						{
							sprintf(acErrMsg,"ɾ���ͻ���ͥ��Ϣ���쳣!![%s]",ret);
					 		WRITEMSG
							strcpy(g_pub_tx.reply,"C053");
					 		goto ErrExit;	
							
						}
						break;
						
			default:
						break;
		
		}
		
		/*�Ǽǽ�����ˮ*/
	g_pub_tx.ac_id = sCif_family_inf.cif_no;
	if(cFlag[0]=='1')
	strcpy(g_pub_tx.brf,"�ͻ���ͥ��Ϣ�޸�");
	else
	strcpy(g_pub_tx.brf,"�ͻ���ͥ��Ϣɾ��");	
	sprintf(g_pub_tx.ac_no,"%d",sCif_family_inf.manage_no);
	g_pub_tx.ac_seqn = sCif_family_inf.family_no;
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }

		
		
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ���ͥ��Ϣά���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"�ͻ���ͥ��Ϣά��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}