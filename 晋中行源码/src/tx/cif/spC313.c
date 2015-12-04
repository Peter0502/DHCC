/*************************************************
* �� �� ��: spC313.c
* ���������������ͻ���Ϣά��
* ��    ��: andy
* ������ڣ�2004��06��03��
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
#include "cif_magn_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_magn_inf_c      cif_magn_inf; 

spC313()  
{ 	
	int ret=0;
	long    lCif_no=0;
	char    cLicense[21];
	char    cQualife[21];
	char    cName[21];
	char    cId_no[21];
	char    cCredit_no[17];
	char    cOwn_type[2];
	char    cEnt_type[2];
	char    cCredit[2];
	char    cSupervior[51];
	char    cDirector[51];
	char    cFinancer[51];
	char    cBank[51];
	double  dOpn_amt=0.0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	memset(cLicense,0x00,sizeof(cLicense));
	memset(cQualife,0x00,sizeof(cQualife));
	memset(cName,0x00,sizeof(cName));
	memset(cId_no,0x00,sizeof(cId_no));
	memset(cCredit_no,0x00,sizeof(cCredit_no));
	memset(cOwn_type,0x00,sizeof(cOwn_type));
	memset(cEnt_type,0x00,sizeof(cEnt_type));
	memset(cCredit,0x00,sizeof(cCredit));
	memset(cSupervior,0x00,sizeof(cSupervior));
	memset(cDirector,0x00,sizeof(cDirector));
	memset(cFinancer,0x00,sizeof(cFinancer));
	memset(cBank,0x00,sizeof(cBank));
	memset(&cif_magn_inf, 0x00, sizeof(struct cif_magn_inf_c));

	/** ȡֵ����ֵ **/
	get_zd_long("0280",&lCif_no);		/* �ͻ��� */
	get_zd_data("0620",cLicense);		/* ����֤�� */		
	get_zd_data("0330",cQualife);	/* ����֤�� */	
	get_zd_data("0810",cName);/* ��ҵ�������� */				
	get_zd_data("0820",cId_no);	   	/* �������֤���� */
	get_zd_data("0580",cCredit_no);		/* ����� */
	get_zd_data("0710",cOwn_type);	/* ��������ʽ */
	get_zd_data("0670",cEnt_type);		/* ��ҵ���� */
	get_zd_data("0640",cCredit);				/* ������ */
	get_zd_data("0250",cSupervior);	/* �ϼ���λ */	
	get_zd_data("0260",cDirector);	/* ���ܵ�λ */	
	get_zd_data("0270",cFinancer);	/* �����ʽ������λ */	
	get_zd_data("0830",cBank);	/* �����˺ſ����� */	
	get_zd_double("0410",&dOpn_amt);	/* �����ʽ� */	
	
	
	sprintf(acErrMsg,"����֤��[%s]",cif_magn_inf.units_license);
	WRITEMSG
	sprintf(acErrMsg,"��Ļȡ�� [%s]",cCredit);
	WRITEMSG
	
	pub_base_strpack ( cCredit );
	sprintf(acErrMsg,"ѹ���� [%s]",cCredit);
	WRITEMSG

		
	/** �������� **/
	
	vtcp_log("[%s][%d]����֤��1[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",lCif_no);
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE Cif_magn_inf_rel�α��쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	
	vtcp_log("[%s][%d]����֤��2[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Fet_Upd(&cif_magn_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"FETCH Cif_magn_inf�α��쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	memcpy(cif_magn_inf.units_license,cLicense,sizeof(cif_magn_inf.units_license));		/* ����֤�� */		
	memcpy(cif_magn_inf.ap_qualife,cQualife,sizeof(cif_magn_inf.ap_qualife));	/* ����֤�� */	
	memcpy(cif_magn_inf.artificial_person,cName,sizeof(cif_magn_inf.artificial_person));/* ��ҵ�������� */				
	memcpy(cif_magn_inf.ap_id,cId_no,sizeof(cif_magn_inf.ap_id));	   	/* �������֤���� */
	memcpy(cif_magn_inf.credit_no,cCredit_no,sizeof(cif_magn_inf.credit_no));		/* ����� */
	memcpy(cif_magn_inf.own_attri,cOwn_type,sizeof(cif_magn_inf.own_attri));	/* ��������ʽ */
	memcpy(cif_magn_inf.ent_attri,cEnt_type,sizeof(cif_magn_inf.ent_attri));		/* ��ҵ���� */
	memcpy(cif_magn_inf.credit,cCredit,sizeof(cif_magn_inf.credit));				/* ������ */
	memcpy(cif_magn_inf.supervior,cSupervior,sizeof(cif_magn_inf.supervior));	/* �ϼ���λ */	
	memcpy(cif_magn_inf.director,cDirector,sizeof(cif_magn_inf.director));	/* ���ܵ�λ */	
	memcpy(cif_magn_inf.financer,cFinancer,sizeof(cif_magn_inf.financer));	/* �����ʽ������λ */	
	memcpy(cif_magn_inf.opn_bank,cBank,sizeof(cif_magn_inf.opn_bank));	/* �����˺ſ����� */	
	cif_magn_inf.opn_amt=dOpn_amt;	/* �����ʽ� */	
	vtcp_log("[%s][%d]����֤��3[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Upd_Upd(cif_magn_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"UPDATE Cif_magn_inf�α��쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	Cif_magn_inf_Clo_Upd( );	
	
	vtcp_log("[%s][%d]����֤��4[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
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
    sprintf(acErrMsg,"�����ͻ���Ϣά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ͻ���Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
