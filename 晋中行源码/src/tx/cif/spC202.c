/*************************************************
* �� �� ��: spC202.c
* �����������ͻ�����ָ��
*              
*
* ��    ��: andy
* ������ڣ�2004��01��10��
* �޸ļ�¼��   
*     1. ��    ��: 2004��2��4��
*        �� �� ��: andy
*        �޸�����:
*************************************************/
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "cif_mger_rel_c.h"

spC202()  
{
	struct 	cif_mger_rel_c		sCifMgerRel;		/* �ͻ�-�ͻ������ϵ�� */
	int ret=0;

	memset ( &sCifMgerRel, 0x00, sizeof( struct cif_mger_rel_c ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_long( "0290", &sCifMgerRel.cif_no );		/* �ͻ��� */
	get_zd_data( "0920", sCifMgerRel.mang );		/* �ͻ�������� */
	get_zd_data( "0700", sCifMgerRel.mang_type );	/* �ͻ��������� */

	/* ���ͻ�-�ͻ������ϵ�����Ƿ��Ѿ�������ؼ�¼ */
	ret = Cif_mger_rel_Sel( g_pub_tx.reply, &sCifMgerRel, \
					"cif_no=%ld and mang='%s' and mang_type='%s' and \
					 canl_date=0 ", \
					sCifMgerRel.cif_no, sCifMgerRel.mang,sCifMgerRel.mang_type);
	if( ret == 0 )
	{
		sprintf( acErrMsg, "�ͻ�-�ͻ������ϵ�����Ѿ�������ؼ�¼!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C043" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "ִ��Cif_mger_rel_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* �Ǽǿͻ�-�ͻ������ϵ�� */
	sCifMgerRel.crt_date = g_pub_tx.tx_date ;	 /* �������� */
			
	ret = Cif_mger_rel_Ins( sCifMgerRel, g_pub_tx.reply );
	if( ret == -239 )
    {
       	sprintf( acErrMsg, "�Ǽǿͻ�-�ͻ������ϵ��ʧ��!���ظ���¼!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C016" );
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
    sprintf(acErrMsg,"�����ͻ�ʶ��ID�ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ͻ�ʶ��IDʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
