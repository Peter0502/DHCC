/*************************************************
* �� �� ��: spC501.c
* ��������: ��������Ϣ¼��
* ��    ��: andy
* ������ڣ�2005��01��25��
* �޸ļ�¼��
*     1. ��    ��:2005��2��5��
*        �� �� ��:andy
*        �޸�����:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"
#include "cif_basic_inf_c.h"

spC501()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* �ͻ���������ϵ */
	struct 	cif_basic_inf_c   	sCifBasicInf;		/* �ͻ�������Ϣ�� */
	int 	ret;
 	
	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
   	memset( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );

    /* ���ݳ�ʼ�� */
    pub_base_sysinit();

    /* ��ǰ̨��Ļȡֵ */
    get_zd_long( "0280", &sCifBlklstRel.cif_no );			/* �ͻ��� */
  	get_zd_data( "0670", sCifBlklstRel.blalist_origin );  	/* ��Ϣ��Դ */
  	get_zd_data( "0810", sCifBlklstRel.rmk );             	/* �ϵ�ԭ�� */

    /* ���ͻ����Ƿ��Ѿ����� */
    ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCifBasicInf, \
							"cif_no=%ld", sCifBlklstRel.cif_no );
    if( ret == 100 )
    {
		sprintf( acErrMsg, "�޴˿ͻ��ţ����ȵ��ͻ����ĵǼ�!![%ld]", \
				sCifBlklstRel.cif_no );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C007" );
		goto ErrExit;
    }
	else if( ret )
    {
       	sprintf( acErrMsg, "��ȡ�ͻ�������Ϣ�쳣![%d]", ret );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C006" );
       	goto ErrExit;
    }
        
    /* ���ͻ��Ƿ��Ѿ��Ϲ������� */
    ret = Cif_blklst_rel_Sel( g_pub_tx.reply, &sCifBlklstRel, \
							"cif_no=%ld and blalist_origin='%s'", \
    						sCifBlklstRel.cif_no, sCifBlklstRel.blalist_origin);             
    if( ret == 0 )
    {
		sprintf( acErrMsg, "�˿ͻ��Ѿ��ǼǺ�����![%ld]", sCifBlklstRel.cif_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "C030" );
		goto ErrExit;
	}
	else if( ret != 100 )
    {
       	sprintf( acErrMsg, "��ȡ�ͻ����������쳣![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }
       		
    /* �Ǽǿͻ���������ϵ�� */
    strcpy( sCifBlklstRel.relation_sts, "1" );		/* ��ϵ״̬��1���� 2��� */
    sCifBlklstRel.crt_date=g_pub_tx.tx_date;		/* �������� */

    ret =Cif_blklst_rel_Ins( sCifBlklstRel, g_pub_tx.reply );
    if( ret == -239 )
    {
       	sprintf( acErrMsg, "�Ǽǿͻ���������Ϣʧ��!���ظ���¼!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C031" );
       	goto ErrExit;
   	}
	else if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Cif_blklst_rel_Ins����!ret=[%d]", ret );
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

