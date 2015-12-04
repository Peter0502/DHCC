/*************************************************
* �� �� ��: spL305.c
* ����������   
*              �����弶�϶�
*
* ��    ��:    lance
* ������ڣ�   2003��01��17��
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
	
	static char sts[2];
		
spL305()  
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
		
	/* ������״̬ 
	if ( strcmp( g_ln_mst.ac_sts,"1") )
	{
		sprintf( acErrMsg,"�ô����˻�״̬������");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L018" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"������״̬ PASS!");
	WRITEMSG */	
					
	/* ���´������ļ� */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd��!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "�������ļ����޴˼�¼" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd����!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    	/* �޸� */
	strcpy ( g_ln_mst.five_sts , sts );		/* �弶�϶���־ */
	
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "�޸Ĵ������ļ���,�����ظ���¼!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"���´������ļ� PASS!");
	WRITEMSG	
		
	/* Pub_tx�ṹ��ֵ */

	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3060;			/* �����弶�϶� */
	strcpy ( g_pub_tx.brf,"�����弶����" );	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����弶�϶��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����弶�϶�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{  
	int ret=0;  
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����ʺ� */
	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0700",sts);			/* �弶�϶���־ */
	sprintf(acErrMsg,"�弶�϶���־[%s]",sts);
	WRITEMSG
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}
						
	return 0;
}

