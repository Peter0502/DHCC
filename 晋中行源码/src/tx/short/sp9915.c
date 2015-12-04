/*************************************************
* �� �� ��:    sp9915.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó����������,���жϿͻ����Ƿ����
*
* ��    ��:    lance
* ������ڣ�   2003��06��09��
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
	
sp9915()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	char cif_type_id[2];
	
	int ac_seqn;
	long cif_no_id;
	
	
	/** ���ݳ�ʼ�� **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0670" , g_pub_tx.id_type );	/* ֤������ */
	get_zd_data( "0620" , g_pub_tx.id_no );	/* ֤������ */
		
	get_zd_data( "0380" , ac_no );	/* �����ʺ� */
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}
        pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/	
	get_zd_data( "0210" , cur_no ); /* ���� */
	
	/* ����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ����� */
	ret = pub_cif_CifCheck( g_pub_tx.id_type , g_pub_tx.id_no , &cif_no_id , 
							cif_type_id  );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú���pub_cif_CifCheck����!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg, "ȡ���Ŀͻ���Ϊ[%ld] ", cif_no_id );
	WRITEMSG				
	sprintf(acErrMsg,"����֤�����ͺ�֤������ȡ�ÿͻ��ż��ͻ����� PASS!");
	WRITEMSG
	
	
	
	
	
	/* ���ݱ��������˻���� */
	ret = pub_base_CurToSeqn( cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"���ݱ��������˻���� PASS![%s]",cur_no);
	WRITEMSG
	
	/* ���ݱ��֡���ʾ�˺�ȡ�������ļ�*/
	ret = pub_base_CurGetddFile( ac_no , cur_no , &dd_mst_tmp );
	if( ret )
	{
		sprintf(acErrMsg,"���ø��ݱ��֡���ʾ�˺�ȡ�������ļ���������");
		WRITEMSG
		goto ErrExit;
	}
	
	sprintf(acErrMsg,"[%s]",dd_mst_tmp.name);
	WRITEMSG		  
	sprintf(acErrMsg,"���ݱ��֡���ʾ�˺�ȡ�������ļ�PASS");
	WRITEMSG
	
	/* �жϿͻ����Ƿ���ͬ ����뻹� ���ñȽ�*/
/**	if( cif_no_id!= dd_mst_tmp.cif_no)
	{
		sprintf(acErrMsg,"�����˻������ڴ������,����������!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "L147" );		
		goto ErrExit;	
	}*/	
	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	set_zd_data( "0260", dd_mst_tmp.name );		/* �����˻����� */	
	set_zd_int( "0340", ac_seqn );			/* �����˻���� */
	set_zd_data( "0910", dd_mst_tmp.opn_br_no );	/* �������� */
	set_zd_data("0380",ac_no);	/* ������ʺ� */
			
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
   
