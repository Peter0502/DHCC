/*************************************************
* �� �� ��:    sp9910.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó���������ź����
*
* ��    ��:    jane
* ������ڣ�   2003��05��25��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9910()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	int ac_seqn;
	
	/** ���ݳ�ʼ�� **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0320" , ac_no );	/* ��֤���ʺ� */
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}

	pub_base_old_acno( ac_no );   	/* �Ծ��ʺŵĴ��� */

	get_zd_data( "0210" , cur_no ); /* ���� */
	
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

/**
	if( strcmp( dd_mst_tmp.prdt_no, "131" ) !=0 )
	{
		sprintf( acErrMsg, "�����˺ŷǱ�֤���˺�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B202" );
		goto ErrExit;
	}
**/

	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	set_zd_data( "0820", dd_mst_tmp.name );	/* ��֤���˻�ȫ�� */
	set_zd_double( "1004", dd_mst_tmp.bal );/* ��֤���˻���� */
	set_zd_data( "0230", dd_mst_tmp.prdt_no );/* ��Ʒ��� */
	set_zd_int( "0340", ac_seqn );		/* �����˻���� */
	set_zd_data("0320",ac_no);		/* ������ʺ� */
		
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
   
