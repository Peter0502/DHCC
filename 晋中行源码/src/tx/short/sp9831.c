/*************************************************
* �� �� ��:    sp9831.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó����������
*
* ��    ��:    lance
* ������ڣ�   2003��01��25��
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
	
sp9831()  
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
	
	get_zd_data( "0310" , ac_no );	/* ����黹�˺� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0310",ac_no);	/* ������ʺ� */	
		
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

	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	set_zd_data( "0270", dd_mst_tmp.name );	/* ����黹�˺Ż��� */	
	set_zd_int( "0490", ac_seqn );	/* ����黹�˺���� */
		
		
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
   
