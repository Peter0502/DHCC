/*************************************************
* �� �� ��:    sp9806.c
* ����������   
*              ���ڸ�����ʾ�˺ű��ֵó����������
*
* ��    ��:    lance
* ������ڣ�   2003��01��13��
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
	
sp9806()  
{ 		
	int ret=0;
	char ac_no[21];
	char cur_no[3];
	int ac_seqn;
	
	/** ���ݳ�ʼ�� **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0380" , ac_no );	/* �����ʺ� */
	if ( strlen(ac_no)==0 )
	{	
	    goto OkExit;
	}
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	
	get_zd_data( "0210" , cur_no ); /* ���� */
vtcp_log( "zy1111ac_no[%s]cur_no[%s]",ac_no,cur_no );	
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
	
vtcp_log( "zy2222ac_no[%s]cur_no[%s]",ac_no,cur_no );	
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
	sprintf(acErrMsg,"���ݱ��֡���ʾ�˺�ȡ�������ļ�PASS   name[%s]", dd_mst_tmp.name);
	WRITEMSG

	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	set_zd_data( "0260", dd_mst_tmp.name );		/* �����˻����� */	
	set_zd_int( "0340", ac_seqn );			/* �����˻���� */
	set_zd_data( "0910", dd_mst_tmp.opn_br_no );	/* �������� */
	set_zd_data("0380",ac_no);	/* ������ʺ� */
		
vtcp_log( "xxxx__name[%s]br_no[%s]",dd_mst_tmp.name,dd_mst_tmp.opn_br_no );	
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
   
