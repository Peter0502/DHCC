/***************************************************************
* �� �� ��: spF063.c
* ���������������ն��޸�֧ȡ����
* ��    ��: jack
* ������ڣ�2003��6��9��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spF063()
{   
	int ret; 
	char cAcno[20];
	char cNpass[7];
	char cOpass[7];

	struct mdm_ac_rel_c sMdm_ac_rel;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cNpass, 0x0, sizeof(cNpass) );
	memset( cOpass, 0x0, sizeof(cOpass) );
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	get_zd_data( "0790", cOpass );
	get_zd_data( "0800", cNpass );

	/*�����ʻ�����*/
	ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply, "ac_no='%s'", cAcno );
	if ( ret )
	{
	 	sprintf( acErrMsg,"ִ��Mdm_ac_rel_Dec_Upd����!!!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	ret=Mdm_ac_rel_Fet_Upd( &sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
	 	sprintf( acErrMsg,"���ҽ����˺Ŷ�Ӧ��¼ʧ��!!!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	if( sMdm_ac_rel.draw_pwd_yn[0] != 'Y' )
	{
	 	sprintf( acErrMsg,"�ý��ʲ���������֧ȡ!!!" );
	  	WRITEMSG
		strcpy( g_pub_tx.reply, "P137" );
	    goto ErrExit;
	}

	get_zd_data("1027",g_pub_tx.draw_pwd); 			/* ֧ȡ���� */

	pub_base_EnDes( g_pub_tx.tx_date, "", cOpass );
	pub_base_EnDes( g_pub_tx.tx_date, "", cNpass );

	if ( strcmp( cOpass, sMdm_ac_rel.draw_pwd ) )
	{
	 	sprintf( acErrMsg,"ԭ�������벻��ȷ!!!" );
	  	WRITEMSG
		strcpy( g_pub_tx.reply, "M005" );
	    goto ErrExit;
	}

	memset( sMdm_ac_rel.draw_pwd, 0x0, sizeof(sMdm_ac_rel.draw_pwd) );
	strcpy( sMdm_ac_rel.draw_pwd, cNpass );

	ret=Mdm_ac_rel_Upd_Upd( sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
	 	sprintf( acErrMsg, "ִ��Mdm_ac_rel_Upd_Upd��[%d]", ret );
	  	WRITEMSG
	    goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf( acErrMsg,"�Ǽǽ�����ˮ����!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	set_zd_data( "0370", cAcno );

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�޸�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�޸�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
