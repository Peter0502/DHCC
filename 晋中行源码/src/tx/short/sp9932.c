/*************************************************
* �� �� ��:  sp9932.c
* ���������� ���ʺŻ��Կͻ���Ϣ_֪ͨ���Ǽ�
*
* ��    ��:  rob 
* ������ڣ� 2003��1��15�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9932()
{
	int	ret;
	
	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	get_zd_data( "0310", g_pub_tx.ac_no ); 		/* �ʺ� */
	pub_base_old_acno( g_pub_tx.ac_no );        /**�Ծ��ʺŵĴ���**/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						  g_pub_tx.ac_no );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "�����˻����ձ��в����ڸ��˻�!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "���ҽ����˻����ձ��쳣!ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W011" );
		goto ErrExit;
	}

	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		ret = Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
							 g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf( acErrMsg, "�ʻ�������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "���ҳ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D103" );
			goto ErrExit;
		}
		
		sprintf( acErrMsg, "��֪ͨ����ʺ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D213" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "���Ҵ���!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
					  g_td_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "���ڲ��������ڸò�Ʒ!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "���Ҵ���!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	if( strncmp(g_td_mst.prdt_no,"242",3) && \
		strncmp(g_td_mst.prdt_no,"243",3) && \
		strncmp(g_td_mst.prdt_no,"244",3) && \
		strncmp(g_td_mst.prdt_no,"281",3) && \
		strncmp(g_td_mst.prdt_no,"282",3) && \
		strncmp(g_td_mst.prdt_no,"283",3) )
	{
		sprintf( acErrMsg, "������֪ͨ����˺�!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O213" );
		goto ErrExit;
	}

	set_zd_data( "0810", g_td_parm.title );				/* ��Ʒ���� */
	set_zd_double( "0430", g_td_mst.bal );				/* �ʻ���� */

	set_zd_data( "0310", g_pub_tx.ac_no );			

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
