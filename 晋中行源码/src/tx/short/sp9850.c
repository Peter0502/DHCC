/*************************************************
* �� �� ��:  sp9850.c
* ���������� �ɿͻ����������Կͻ���������
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
#include "cif_mger_inf_c.h"

int sp9850()
{
	struct 	cif_mger_inf_c   sCifMgerInf;		/* �ͻ�������Ϣ�� */
	int	ret;

	get_zd_char( "0920", g_pub_tx.mdm_code );       /* ������� */

	/* ��龭������Ƿ���� */
       	ret = Cif_mger_inf_Sel( g_pub_tx.reply, &sCifMgerInf, "mang='%s'", g_pub_tx.mdm_code );
       	if( ret==100 )
       	{
                sprintf( acErrMsg, "�޴˾�����룬����!mang=[%s]", g_pub_tx.mdm_code );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C014" );
                goto ErrExit;
       	}else if( ret )
       	{
                sprintf( acErrMsg, "��ȡ�ͻ�������Ϣ�쳣![%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C015" );
                goto ErrExit;
        }
	
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0620", sCifMgerInf.mang_name );	/* �ͻ��������� */
	set_zd_data( "0700", sCifMgerInf.mang_type );	/* �ͻ��������� */
        sprintf( acErrMsg, "�ͻ���������![%s]", sCifMgerInf.mang_type );
        WRITEMSG		

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

