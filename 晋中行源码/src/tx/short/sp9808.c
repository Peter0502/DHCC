/*************************************************
* �� �� ��:    sp9808.c
* ����������   
*              ���ݴ�����ʾ�˺ź��˺����ȡ�������ļ�
*
* ��    ��:    lance
* ������ڣ�   2003��01��14��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9808()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	long beg_end_date;  /* ������- ������ */
	long beg_end_days;  /* ������- ������ */
	long max_exp_date;  /* ���չ������ */
	
	/** ���ݳ�ʼ�� **/
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0370" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
		
	get_zd_int( "0340" , &ac_seqn );	/* �����˻����s */
	
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ac_no,ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	/* ����������չ������ */
	beg_end_date = g_ln_mst.mtr_date - g_ln_mst.opn_date ;
	
	beg_end_days = pub_base_CalTrueDays( g_ln_mst.opn_date , g_ln_mst.mtr_date );
	
	if ( beg_end_date <= 10000 )
	{
		pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days , &max_exp_date );
	}else if ( beg_end_date <= 50000 )
		{
			pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days/2 , &max_exp_date );
		}else
			{
				max_exp_date = g_ln_mst.mtr_date + 30000;
			}
	sprintf(acErrMsg,"����������չ������ PASS!");
	WRITEMSG
	
	/* ��������� */
	if ( g_ln_mst.exp_ind[0]!='0' )
	{
		g_ln_mst.mtr_date = g_ln_mst.exp_mtr_date ;
	}	
	
		
	/* ������� */
	sprintf(acErrMsg,"�����������[%s]",g_ln_mst.name);
	WRITEMSG
	set_zd_double( "0400", g_ln_mst.amt_lmt );	/* �ƻ��ſ�� */	
	set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* �ۼƷ��Ŷ� */
	set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
	set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
	set_zd_long( "0440", g_ln_mst.mtr_date );	/* ��������� */		
	set_zd_long( "0530", max_exp_date );		/* ���չ������ */
	set_zd_data( "0700", g_ln_mst.five_sts );	/* �弶����״̬ */			
	set_zd_data("0370",ac_no);	/* ������ʺ� */
				
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
