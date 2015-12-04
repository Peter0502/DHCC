/*************************************************
* �� �� ��: spC502.c
* ��������: ��������Ϣά��
* ��    ��: andy
* ������ڣ�2005��2��5��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"

spC502()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* �ͻ���������ϵ */
	char	cOptcode[2];					/* ������ */
	char	cRmk[41];					/* �ϵ�ԭ�� */
	char	comm[100];					/* ɾ�����¼��where���� */
	int 	ret;
 	
	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
	memset( cOptcode, 0x00, sizeof( cOptcode ) );
	memset( cRmk, 0x00, sizeof( cRmk ) );
	memset( comm, 0x00, sizeof( comm ) );

        /* ���ݳ�ʼ�� */
        pub_base_sysinit();

        /* ��ǰ̨��Ļȡֵ */
        get_zd_data( "0680", cOptcode );  			/* ������: 1�޸� 2ɾ�� */
        get_zd_long( "0280", &sCifBlklstRel.cif_no );		/* �ͻ��� */
    	get_zd_data( "0670", sCifBlklstRel.blalist_origin );  	/* ��Ϣ��Դ */
    	get_zd_data( "0810", cRmk );             		/* �ϵ�ԭ�� */
       	
       	/* ������޸Ĳ��� */
       	if( cOptcode[0] == '1' )
       	{
       		/* ���ͻ���������ϵ�� */
		ret = Cif_blklst_rel_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and blalist_origin='%s'", \
						sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_blklst_rel_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ȡ����¼ */
		ret = Cif_blklst_rel_Fet_Upd( &sCifBlklstRel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "��ѯ�ͻ���������ϵ����޼�¼!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C059" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_blklst_rel_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* �޸Ŀͻ���������ϵ�� */
		strcpy( sCifBlklstRel.rmk, cRmk );
		
		ret = Cif_blklst_rel_Upd_Upd( sCifBlklstRel, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸Ŀͻ���������ϵ���[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C060" );
			goto ErrExit;	
		}
		
		Cif_blklst_rel_Clo_Upd();
	}
	else		/* �����ɾ������ */
        {
        	/* ׼��ɾ�����¼��where���� */
        	sprintf( comm, "cif_no=%ld and blalist_origin='%s' and rmk='%s'", \
        		sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin, cRmk );

        	ret = deldb( "cif_blklst_rel", comm );
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
