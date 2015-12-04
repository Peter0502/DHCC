/*************************************************
* �� �� ��:  sp9922.c
* ���������� �������ʺŻ���������Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��6��15��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9922()
{
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	dd_mst_c	sDdMst;		/* ���ڴ�����ļ� */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0330", g_pub_tx.ac_no );	/* �����ʺ� */

	pub_base_old_acno( g_pub_tx.ac_no );                 /**�Ծ��ʺŵĴ���**/

	/* ��ѯ�ͻ��ʺ��Ƿ�Ϊ�����ʺ� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˿ͻ��ʺŲ�����![%s]", g_pub_tx.ac_no );
   		WRITEMSG
		set_zd_data( "0961", "���������˺�����,����������Э���Ż�����Ӧ��Ϣ!" );	/* ��ʾ��Ϣ */
TRACE
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* ���������ʺ�ID���ʻ���Ų�ѯ���ֵǼǱ� */
	ret =Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "ac_id=%ld and ac_seqn=%d", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		set_zd_data( "0961", "���������˺�����,����������Э���Ż�����Ӧ��Ϣ!" );	/* ��ʾ��Ϣ */
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* Ʊ������ */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* Ʊ�ݺ� */
	set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
 
        set_zd_double("0410",sMoDiscnt.par_amt);

	set_zd_data( "0250", sMoDiscnt.pn_name );	/* ת���������˻��� */
	set_zd_data( "0260", sMoDiscnt.pn_name );	/* ���������˻��� */
	set_zd_data( "0270", sMoDiscnt.remitter );	/* ��Ʊ���˻��� */
	set_zd_long( "0440", sMoDiscnt.pn_date );	/* ת�������� */
	set_zd_long( "0460", sMoDiscnt.return_date );  /* �ع����� */
	set_zd_double( "0840", sMoDiscnt.pn_rate );    /* ת������ */
	set_zd_double( "1001", sMoDiscnt.pn_int );      /* ������Ϣ */

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




