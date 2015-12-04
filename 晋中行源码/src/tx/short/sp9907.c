/*************************************************
* �� �� ��:    sp9907.c
* ����������   
*              ���ڴ浥������ʾ�˺ŵó����������
*
* ��    ��:    jane
* ������ڣ�   2003��05��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9907()  
{ 		
	struct	mdm_ac_rel_c 	sMdmAcRel;	/* �����˺Ŷ��ձ� */
	struct	td_mst_c 	sTdMst;		/* �������ļ� */
	int ret=0;
	char ac_no[20];
	
	/** ���ݳ�ʼ�� **/
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0320" , ac_no );	/* ��Ѻ�ʺ� */

	/* ������Ѻ�˺Ų�ѯ�����˺Ŷ��ձ����ڴ�����ļ� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
						ac_no);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��Ѻ�˺Ų�����![%s]", ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B183" );
		goto ErrExit;
	}

	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Td_mst_Sel( g_pub_tx.reply, &sTdMst, "ac_id=%ld and ac_seqn=%d",\
					sMdmAcRel.ac_id, 0 );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��Ѻ�˺Ų��Ƕ��ڴ浥��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B184" );
		goto ErrExit;
	}

	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Td_mst_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( sTdMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "����Ѻ�˺�״̬������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B185" );
		goto ErrExit;
	}

	/* ������� */
	set_zd_data( "0820", sTdMst.name );	/* �ͻ����� */	
	set_zd_double( "1004", sTdMst.bal );	/* ��Ѻ�浥��� */
	
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
   
