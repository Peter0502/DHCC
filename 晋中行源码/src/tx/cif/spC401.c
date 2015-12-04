/*************************************************
* �� �� ��: spC401.c
* ����������ά���ͻ���ͻ����ϵ
* 
* ��    ��: andy
* ������ڣ�2004��2��4��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_cif_rel_c.h"

spC401()
{
	struct 	cif_cif_rel_c     sCifCifRel;		/* �ͻ�-�ͻ���ϵ�� */
	char	cOptcode[2];				/* ������ */
	char	comm[100];				/* ɾ�����¼��where���� */
	char	cRelCode[4];				/* �ͻ���ϵ���� */
	int 	ret;

        memset( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
        memset( cOptcode, 0x00, sizeof( cOptcode ) );
        memset( comm, 0x00, sizeof( comm ) );
        memset( cRelCode, 0x00, sizeof( cRelCode ) );
        
        /* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
        get_zd_data( "0670", cOptcode );       			/* ������: 1�޸� 2ɾ�� */
        get_zd_long( "0290", &sCifCifRel.cif_no );		/* �ͻ�ID */
        get_zd_long( "0280", &sCifCifRel.rel_cid_no );    	/* �����ͻ��� */
        get_zd_data( "0230", cRelCode );      			/* �ͻ���ϵ���� */

       	/* ������޸Ĳ��� */
       	if( cOptcode[0] == '1' )
       	{
		/* ���ͻ�-�ͻ���ϵ�� */
		ret = Cif_cif_rel_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and rel_cid_no=%ld", \
					sCifCifRel.cif_no, sCifCifRel.rel_cid_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_cif_rel_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ȡ����¼ */
		ret = Cif_cif_rel_Fet_Upd( &sCifCifRel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "��ѯ�ͻ�-�ͻ���ϵ����޼�¼!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C046" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_cif_rel_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* �޸Ŀͻ���ͻ���ϵ�� */
		strcpy( sCifCifRel.rel_code, cRelCode );
		
		ret = Cif_cif_rel_Upd_Upd( sCifCifRel, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸Ŀͻ���ͻ���ϵ���[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C047" );
			goto ErrExit;	
		}
		
		Cif_cif_rel_Clo_Upd();
	}	
        else		/* �����ɾ������ */
        {
        	/* ׼��ɾ�����¼��where���� */
        	sprintf( comm, "cif_no=%ld and rel_cid_no=%ld and rel_code='%s'", \
        		sCifCifRel.cif_no, sCifCifRel.rel_cid_no, cRelCode );

        	ret = deldb( "cif_cif_rel", comm );
        	if( ret )
        	{
        		sprintf( acErrMsg, "ɾ�����¼��[%d]", ret );
			WRITEMSG
			goto ErrExit;
        	}
	}
	
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
