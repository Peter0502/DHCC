/*************************************************
* �� �� ��:  sp9852.c
* ���������� �ɿͻ��Ż��Կͻ���ϵ���뼰�����ͻ�����
*
* ��    ��:  jane
* ������ڣ� 2003��2��4��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_cif_rel_c.h"

int sp9852()
{
	struct 	cif_basic_inf_c   	sCifBasicInf;		/* �ͻ�������Ϣ�� */
	struct 	cif_cif_rel_c   	sCifCifRel;		/* �ͻ�-�ͻ���ϵ�� */
	int	ret;

	memset ( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );
	memset ( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
	
	get_zd_long( "0290", &g_pub_tx.cif_no );       	/* �ͻ��� */
	get_zd_long( "0280", &g_pub_tx.ac_id );       	/* �����ͻ��� */

	/* ���ͻ����Ƿ���� */
       	ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCifBasicInf, "cif_no=%ld", g_pub_tx.ac_id );
       	if( ret==100 )
       	{
                sprintf( acErrMsg,"�޴˿ͻ��ţ�����!cif_no=[%ld]", g_pub_tx.ac_id );
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
	
	/* ���ͻ���ϵ�Ƿ���� */
       	ret = Cif_cif_rel_Sel( g_pub_tx.reply, &sCifCifRel, "cif_no=%ld and rel_cid_no=%ld", \
       				g_pub_tx.cif_no, g_pub_tx.ac_id );
       	if( ret==100 )
       	{
                sprintf( acErrMsg, "�ͻ�-�ͻ���ϵ��������ؼ�¼!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C046" );
                goto ErrExit;
       	}else if( ret )
       	{
                sprintf( acErrMsg, "��ȡ�ͻ�-�ͻ���ϵ���쳣![%d]", ret );
                WRITEMSG
                goto ErrExit;
        }
	
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0260", sCifBasicInf.name );		/* �����ͻ����� */
	set_zd_data( "0230", sCifCifRel.rel_code );		/* �ͻ���ϵ���� */

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

