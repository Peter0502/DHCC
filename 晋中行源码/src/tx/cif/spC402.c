/*************************************************
* �� �� ��: spC402.c
* ����������ɾ���ͻ���ͻ��������ϵ
* 
* ��    ��: andy
* ������ڣ�2004��2��5��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_mger_rel_c.h"

spC402()
{
	struct 	cif_mger_rel_c		sCifMgerRel;		/* �ͻ�-�ͻ������ϵ�� */
	char	comm[100];					/* ɾ�����¼��where���� */
	int 	ret;

    memset ( &sCifMgerRel, 0x00, sizeof( struct cif_mger_rel_c ) );
    memset( comm, 0x00, sizeof( comm ) );
        
    /* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
    get_zd_long( "0290", &sCifMgerRel.cif_no );		/* �ͻ����� */
    get_zd_data( "0920", sCifMgerRel.mang );    		/* ������� */
    get_zd_data( "0700", sCifMgerRel.mang_type );      	/* �ͻ��������� */

   	/* ���ͻ�-�ͻ������ϵ�����Ƿ������ؼ�¼ */
   	ret = Cif_mger_rel_Dec_Upd( g_pub_tx.reply, "cif_no=%ld and mang='%s' \
       				and mang_type='%s' and canl_date=%ld", sCifMgerRel.cif_no, \
				sCifMgerRel.mang, sCifMgerRel.mang_type, 0 );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Cif_mger_rel_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Cif_mger_rel_Fet_Upd( &sCifMgerRel, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ͻ�-�ͻ������ϵ���в�������ؼ�¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "C054" );
        	goto ErrExit;
    	}
	else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Cif_mger_rel_Dec_Upd��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   	}

	/* �޸Ŀͻ�-�ͻ������ϵ�� */
	sCifMgerRel.canl_date = g_pub_tx.tx_date;		/* ������� */

	ret = Cif_mger_rel_Upd_Upd( sCifMgerRel, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸Ŀͻ�-�ͻ������ϵ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Cif_mger_rel_Clo_Upd();

    	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���˿ͻ���Ϣ¼��ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"���˿ͻ���Ϣ¼��ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
