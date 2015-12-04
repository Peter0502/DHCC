/*************************************************
* �� �� ��: spC201.c
* ����������¼��ͻ���ͻ����ϵ
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
#include "cif_basic_inf_c.h"

spC201()
{
	struct cif_cif_rel_c     sCifCifRel;		/* �ͻ�-�ͻ���ϵ�� */
	struct cif_basic_inf_c   sCifBasicInf;		/* �ͻ�������Ϣ�� */
	int ret;

        memset( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
        memset( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );
        
        /* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
        get_zd_long( "0290", &sCifCifRel.cif_no );        /* �ͻ�ID */
        get_zd_long( "0280", &sCifCifRel.rel_cid_no );    /* �����ͻ��� */
        get_zd_data( "0230", sCifCifRel.rel_code );       /* �ͻ���ϵ���� */

       	/* ���¼��Ŀͻ�ID�Ƿ��ظ� */
       	if( sCifCifRel.cif_no == sCifCifRel.rel_cid_no )
       	{
       		sprintf( acErrMsg, "¼��Ŀͻ�ID�ظ�!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C042" );
                goto ErrExit;
       	}
       
	/* ���ͻ�-�ͻ���ϵ�����Ƿ��Ѿ�������ؼ�¼ */
	ret = Cif_cif_rel_Sel( g_pub_tx.reply, &sCifCifRel, "( cif_no=%ld and rel_cid_no=%ld ) \
       				or ( cif_no=%ld and rel_cid_no=%ld) ", sCifCifRel.cif_no, \
       				sCifCifRel.rel_cid_no, sCifCifRel.rel_cid_no, sCifCifRel.cif_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "�ͻ�-�ͻ���ϵ�����Ѿ�������ؼ�¼!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C041" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "ִ��Cif_cif_rel_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

        /* �Ǽǿͻ���ͻ���ϵ�� */
	strcpy(	sCifCifRel.rel_sts, "1" );		/* ��ϵ״̬��1���� 2��� */
	sCifCifRel.crt_date = g_pub_tx.tx_date;		/* �������� */

        ret = Cif_cif_rel_Ins( sCifCifRel, g_pub_tx.reply ); 
        if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�Ǽǿͻ���ͻ���ϵ��ʧ��!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "C027" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Cif_cif_rel_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
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
