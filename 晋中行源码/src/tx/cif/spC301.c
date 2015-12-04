/*************************************************
* �� �� ��: spC301.c
* �����������ͻ�ʶ��ID״̬ά��
* ��    ��: andy
* ������ڣ�2004��01��10��
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
#include "cif_inf_chg_log_c.h"

static struct cif_basic_inf_c    cif_basic_inf_t;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_inf_chg_log_c  cif_inf_chg_log;
	
char beg_sts[2];
		
spC301()  
{ 	
	int ret=0;
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ���ݳ�ʼ�� **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_basic_inf_t, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));	
		
	/** ȡֵ����ֵ **/
	get_zd_long("0280",&cif_basic_inf_t.cif_no);	/* �ͻ��� */
	get_zd_data("0700",cif_basic_inf_t.sts);		/* �ͻ�״̬ */
	get_zd_data("0810",cif_inf_chg_log.memo);		/* ����˵�� */
		
	/** �������� **/
	sprintf(acErrMsg,"cif_basic_inf_t.cif_no=[%ld]",cif_basic_inf_t.cif_no);
	WRITEMSG

	/* ���ͻ����Ƿ��Ѿ����ڣ���ȡ����Ϣ */
	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
							 "cif_no=%ld",cif_basic_inf_t.cif_no);
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
	
	/* ���ͻ�״̬�Ƿ�ı� */
/**	if (!strcmp (cif_basic_inf.sts , cif_basic_inf_t.sts ))
	{
		sprintf(acErrMsg,"δ�����κ��޸ģ�������ύ![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C012"); 
		ERR_DEAL;
	}

	strcpy(beg_sts,cif_basic_inf.sts);	**/		/* �����޸�ǰ״̬ */
				
	/* �޸Ŀͻ�������Ϣ�� */
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		
	ret = Cif_basic_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",
	                            cif_basic_inf_t.cif_no);
	if( ret )
	{
		sprintf( acErrMsg,"ȡ�ͻ�������Ϣ�������쳣,�α�DEC����[%s]",ret);
		WRITEMSG
		goto ErrExit;
	}
		
	ret = Cif_basic_inf_Fet_Upd ( &cif_basic_inf, g_pub_tx.reply);
	if(ret) 
	{
		sprintf( acErrMsg,"ȡ�ͻ�������Ϣ�������쳣,�α�FET���� [%s]",ret);
		WRITEMSG
		goto ErrExit;
	}			
	
	strcpy(cif_basic_inf.sts,cif_basic_inf_t.sts);

	ret = Cif_basic_inf_Upd_Upd( cif_basic_inf, g_pub_tx.reply);
	if(ret)
	{
		sprintf( acErrMsg,"ִ��Cif_basic_inf_Upd_Upd�� [%s]",ret);
		WRITEMSG
		goto ErrExit;
	} 
			
	Cif_basic_inf_Clo_Upd( );

	/* �Ǽǿͻ���Ϣ�޸���־ */
	cif_inf_chg_log.cif_no=cif_basic_inf_t.cif_no;	 /* �ͻ����� */	
	memcpy(cif_inf_chg_log.chg_tablename,"cif_basic_inf",sizeof(cif_inf_chg_log.chg_tablename));  	 /* ���޸Ŀͻ���Ϣ���� */
	memcpy (cif_inf_chg_log.chg_columnname,"sts",sizeof(cif_inf_chg_log.chg_columnname));	 /* �޸Ŀͻ���Ϣ���� */	
	cif_inf_chg_log.chg_date=g_pub_tx.tx_date; 	 	 /* �޸����� */	
	cif_inf_chg_log.chg_time=g_pub_tx.tx_time; 	 	 /* �޸�ʱ�� */	
	memcpy(cif_inf_chg_log.bef_content,beg_sts,sizeof(cif_inf_chg_log.bef_content));	 /* �޸�ǰ���� */	
	memcpy(cif_inf_chg_log.aft_content,cif_basic_inf_t.sts,sizeof(cif_inf_chg_log.aft_content));		 /* �޸ĺ����� */ 
	memcpy(cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(cif_inf_chg_log.clerk_no)); 	 	 /* ְԱ��� */	
	ret = Cif_inf_chg_log_Ins(cif_inf_chg_log);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǿͻ���Ϣ�޸���־ʧ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C013");
		goto ErrExit;
	}
			
	/* �Ǽǽ�����ˮ */
       strcpy(g_pub_tx.brf,"�ͻ�ʶ��ID״̬ά��");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�ͻ�ʶ��ID״̬ά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"�ͻ�ʶ��ID״̬ά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
