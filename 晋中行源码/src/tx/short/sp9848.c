/*************************************************
* �� �� ��:  sp9848.c
* ���������� �ɿͻ�ID(0290)���Կͻ�����(0250)
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

int sp9848()
{
	struct 	cif_basic_inf_c   sCifBasicInf;		/* �ͻ�������Ϣ�� */
	int	ret;

	get_zd_long( "0290", &g_pub_tx.cif_no );       	/* �ͻ�ID */

	/* ���ͻ����Ƿ���� */
	if(g_pub_tx.cif_no)
	{
       	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCifBasicInf, \
							  "cif_no=%ld",g_pub_tx.cif_no );
       	if( ret==100 )
       	{
            sprintf(acErrMsg,"�޴˿ͻ��ţ�����!cif_no=[%ld]", \
					g_pub_tx.cif_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C007" );
            goto ErrExit;
       	}else if( ret )
       	{
            sprintf( acErrMsg, "��ȡ�ͻ�������Ϣ�쳣![%d]", ret );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C006" );
            goto ErrExit;
        }
	}
	else{
        sprintf(acErrMsg,"�ͻ���δ����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C075");
		goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0250", sCifBasicInf.name );

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

