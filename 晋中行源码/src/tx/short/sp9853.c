/*************************************************
* �� �� ��:  sp9853.c
* ���������� �ɿͻ��ź���Ϣ��Դ�����ϵ�ԭ��(������)
*
* ��    ��:  jane
* ������ڣ� 2003��2��5��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_blklst_rel_c.h"

int sp9853()
{
	struct	cif_blklst_rel_c	sCifBlklstRel;		/* �ͻ���������ϵ */
	int	ret;

	memset( &sCifBlklstRel, 0x00, sizeof( struct cif_blklst_rel_c ) );
	
	get_zd_long( "0280", &g_pub_tx.cif_no );		/* �ͻ��� */
    	get_zd_data( "0670", g_pub_tx.ct_ind );  		/* ��Ϣ��Դ */

	/* ���ͻ���������ϵ�����Ƿ���� */
       	ret = Cif_blklst_rel_Sel( g_pub_tx.reply, &sCifBlklstRel, "cif_no=%ld and blalist_origin='%s'", \
        			g_pub_tx.cif_no, g_pub_tx.ct_ind );             
        if( ret == 100 )
        {
		sprintf( acErrMsg, "��ѯ�ͻ���������ϵ����޼�¼!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "C059" );
		goto ErrExit;
	}else if( ret )
        {
            	sprintf( acErrMsg, "��ȡ�ͻ����������쳣![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }
	
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0810", sCifBlklstRel.rmk );		/* �ϵ�ԭ�� */

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


