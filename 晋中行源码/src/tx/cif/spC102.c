/*************************************************
* �� �� ��: spC102.c
* �������������˿ͻ���Ϣ¼��
* ��    ��: andy
* ������ڣ�2004��01��08��
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
#include "cif_per_inf_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_per_inf_c      cif_per_inf;
	
	char technical_post[4];
	char headship[4];
	char cif_no[9];
	
spC102()  
{ 	
	int ret=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
		
	/** �������� **/
	/* ���ͻ����Ƿ��Ѿ����� */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_per_inf.cif_no );
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
	sprintf(acErrMsg,"���ͻ����Ƿ��Ѿ����� PASS!");
	WRITEMSG

	/* ���ͻ����Ƿ��Ƕ�˽�ͻ��� */
	sprintf( cif_no , "%ld" , cif_per_inf.cif_no );
	if ( cif_no[0] != '1' )
		{
			sprintf(acErrMsg,"�Ƕ�˽�ͻ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C011");
			ERR_DEAL;
		}		
	sprintf(acErrMsg,"���ͻ����Ƿ��Ƕ�˽�ͻ��� PASS!");
	WRITEMSG
				
	/* �ǼǸ��˿ͻ���Ϣ�� */
	ret = Cif_per_inf_Ins( cif_per_inf , &g_pub_tx.reply );
	if( ret && ret!=-239 )
	{
		sprintf(acErrMsg,"�ǼǸ��˿ͻ���Ϣ��ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C008");
		ERR_DEAL;
	}else if( ret==-239 ){
		sprintf(acErrMsg,"�˿ͻ�������Ϣ�ѵǼ�![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C088");
		ERR_DEAL;
	}

	sprintf(acErrMsg,"�ǼǸ��˿ͻ���Ϣ�� PASS!");
	WRITEMSG
			
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
    sprintf(acErrMsg,"���˿ͻ���Ϣ¼��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���˿ͻ���Ϣ¼��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	get_zd_long("0280",&cif_per_inf.cif_no);	/* �ͻ��� */
	get_zd_data("0270",cif_per_inf.name);		/* �ͻ���� */
	get_zd_long("0440",&cif_per_inf.g_bir);		/* ������������ */
	get_zd_long("0450",&cif_per_inf.n_bir);		/* ũ���������� */
	get_zd_data("0670",cif_per_inf.sex);		/* �ͻ��Ա� */
	get_zd_data("0680",cif_per_inf.marriage);	/* ����״̬ */
	get_zd_long("0290",&cif_per_inf.mar_date);	/* ������� */
	get_zd_data("0710",cif_per_inf.edu);		/* ���ѧ�� */		
	get_zd_data("0700",cif_per_inf.religion);	/* �ڽ����� */		
	get_zd_data("0230",cif_per_inf.nationality);/* �������� */	
	get_zd_data("0240",cif_per_inf.country);	/* ��ס������ */
	get_zd_data("0210",cif_per_inf.folk);		/* ������� */		
	get_zd_data("0220",cif_per_inf.language);	/* ���Դ��� */	
	get_zd_data("0650",cif_per_inf.clerk_no);	/* ְԱ��� */				
	get_zd_data("0690",cif_per_inf.health);	   	/* ����״�� */
		
	get_zd_data("0900",technical_post);		/* ְ�� */
	sprintf(acErrMsg,"��Ļȡ�� [%s]",technical_post);
	WRITEMSG
	
	pub_base_strpack ( technical_post );
	sprintf(acErrMsg,"ѹ���� [%s]",technical_post);
	WRITEMSG
	
	strncpy( cif_per_inf.technical_post , technical_post , 3);
	sprintf(acErrMsg,"�޸ĳ���λ�� [%s]",technical_post);
	WRITEMSG
	
	get_zd_data("0890",headship);			/* ְ�� */	
	pub_base_strpack ( headship );
	strncpy( cif_per_inf.headship , headship , 3);	
	
	get_zd_data("0810",cif_per_inf.birth_adr);	/* ������ */
	get_zd_double("0400",&cif_per_inf.mon_income);	/* ������ */
	get_zd_double("0410",&cif_per_inf.h_mon_income);	/* ��ͥ������ */
	
	get_zd_long("0500",&cif_per_inf.home_cnt);	/* ������ͥ��Ա��Ŀ */	
	sprintf(acErrMsg,"������ͥ��Ա��Ŀ [%d]",cif_per_inf.home_cnt);
	WRITEMSG
		
	return 0;
}
