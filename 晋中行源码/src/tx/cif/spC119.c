/*************************************************
* �� �� ��: spC119.c
* ������������˾������Ϣ¼��
* ��    ��: zj
* ������ڣ�2014��10��24��
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1117', '��˾������Ϣ¼��', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '4', '2');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1117', 0, 'C119', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('C119', '��˾������Ϣ¼��', 'spC119', '0', '0');
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
#include "cif_basic_inf_c.h"
#include "cif_copa_inf_c.h"
#include "cif_id_code_rel_c.h"

int spC119()
{
	int iRet=0;
	int ret=0;
	char cSc_bs[50];
	char cUp_br[50];
	char cif_no[9];
	struct cif_basic_inf_c sCif_basic_inf;
	struct cif_copa_inf_c sCif_copa_inf;
	struct cif_copa_inf_c sCif_copa_inf1;
	struct cif_id_code_rel_c sCif_id_code_rel;
	
	pub_base_sysinit();
	
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	memset(&sCif_copa_inf,0x00,sizeof(sCif_copa_inf));
	memset(&sCif_copa_inf1,0x00,sizeof(sCif_copa_inf1));
	memset(&sCif_id_code_rel,0x00,sizeof(sCif_id_code_rel));
	
	get_zd_data("0250",cSc_bs);
	get_zd_data("0260",sCif_copa_inf.con_en_name);	
	get_zd_data("0270",cUp_br);
	
	get_zd_long("0280",&sCif_copa_inf.cif_no);
	get_zd_data("0300",sCif_copa_inf.ag_cr_no);
	get_zd_data("0820",sCif_copa_inf.or_ag_type);
	get_zd_data("0480",sCif_copa_inf.or_ag_mst_type);
	get_zd_data("0520",sCif_copa_inf.eco_ind_type);
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

	strcpy(sCif_copa_inf.sc_bs,cSc_bs);
	strcpy(sCif_copa_inf.up_br_name,cUp_br);
	Cif_copa_inf_Debug(&sCif_copa_inf);
	
		/** ���ͻ����Ƿ���� **/
		ret = Cif_basic_inf_Sel( g_pub_tx.reply , &sCif_basic_inf ,
           					" cif_no=%ld " , sCif_copa_inf.cif_no );
  	if( ret==100 )
    {
       	sprintf(acErrMsg,"�޴˿ͻ��ţ�����![%s]",g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C007");
       	ERR_DEAL;
    }
    else if( ret )
    {
       	sprintf(acErrMsg,"��ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C006");
       	ERR_DEAL;
    }	
    
    /* ���ͻ����Ƿ��ǶԹ��ͻ��� */
		sprintf( cif_no , "%ld" , sCif_copa_inf.cif_no );
		if ( cif_no[0] != '5' )
		{
			sprintf(acErrMsg,"�ǶԹ��ͻ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C010");
			ERR_DEAL;
		}		
		sprintf(acErrMsg,"���ͻ����Ƿ��ǶԹ��ͻ��� PASS!");
		WRITEMSG
    
    strcpy(sCif_copa_inf.br_no,g_pub_tx.tx_br_no);
    strcpy(sCif_copa_inf.tel,g_pub_tx.tel);
    sCif_copa_inf.tx_date=g_pub_tx.tx_date;
    sCif_copa_inf.up_date=g_pub_tx.tx_date;
    sCif_copa_inf.cop_sts[0]='1';
    
    /*�������Ĺ�˾������Ϣ�����еĹ�˾������Ϣ�Ƿ��ظ�*/
		ret=Cif_copa_inf_Sel(g_pub_tx.reply,&sCif_copa_inf1,"cif_no=%ld",sCif_copa_inf.cif_no);
    if( ret==100 )
    {
       	/* �Ǽǹ�˾�ͻ���Ϣ�� */
   
				iRet = Cif_copa_inf_Ins(sCif_copa_inf);
				if(iRet)
				{
					sprintf(acErrMsg,"�Ǽǹ�˾������Ϣ��ʧ��![%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"�Ǽǹ�˾������Ϣ�� PASS!");
				WRITEMSG
    }
    else if(!ret)
    {
    		sprintf(acErrMsg,"�ÿͻ��ѵǼǹ�˾������Ϣ![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C087");
				goto ErrExit;
    }
    
    /* �Ǽǽ�����ˮ */
		if( pub_ins_trace_log() )
		{
				sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
				WRITEMSG
				goto ErrExit;
		}
		sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
		WRITEMSG
	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��˾������Ϣ¼��ɹ�!!�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"��˾������Ϣ¼��ʧ��!!�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;	
	
}
