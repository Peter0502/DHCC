/*************************************************
* �� �� ��:    sp9937.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó����������
*
* ��    ��:    lance
* ������ڣ�   2003��06��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.      :wq

*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9937()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	char cif_type_id[2];
	char name[51];
	char sts[2];
	
	int ac_seqn;
	long cif_no_id;
	long cif_no;
	long save_ac_id;
	
	
	/** ���ݳ�ʼ�� **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/			
	get_zd_data( "0320" , ac_no );	/* ����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	
	/***
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}
	***/
	
	get_zd_data( "0250" , name );	 /* ����� */
	get_zd_data( "0670" , sts ); 	 /* �����˱�־ */
			
	get_zd_data( "0210" , cur_no );  /* ���� */
	
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
	
	pub_base_strpack( ac_no );
	if( strlen( ac_no ) )
	{
		ret = pub_ln_AcCheck( ac_no,ac_seqn,&cif_no,&save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}

	}	
	sprintf(acErrMsg,"[%s]",dd_mst_tmp.name);
	WRITEMSG
	pub_base_strpack( name );
	pub_base_strpack( dd_mst_tmp.name );	
	/*if( sts[0]=='N' )
	{
		if( strcmp( name , dd_mst_tmp.name ) )
		{
			sprintf(acErrMsg,"����˺Ż�����������һ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply , "L189");
			goto ErrExit;	
		}	
	}	*/  
	sprintf(acErrMsg,"���ݱ��֡���ʾ�˺�ȡ�������ļ�PASS");
	WRITEMSG
	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	set_zd_data( "0820", dd_mst_tmp.name );		/* �����˻����� */	
	set_zd_int( "0360", ac_seqn );			/* �����˻���� */
	set_zd_data( "0910", dd_mst_tmp.opn_br_no );	/* �������� */
	set_zd_data("0320",ac_no);	/* ������ʺ� */	
		
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
   
