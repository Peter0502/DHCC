/*************************************************
* �� �� ��:  sp9813.c
* ���������� ������ʾ�˺�ȡ�ÿͻ�����
*
* ��    ��:  lance
* ������ڣ� 2003��1��17�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9813()
{			
	int ret=0;
	long cif_no;
	char cif_type[2];
	
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	/* ������ʾ�˺Ų�ѯ�������˻���ϵ��Ϣ */ 
	get_zd_data( "0300", g_pub_tx.ac_no );       /* �ʺ� */

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'",\
				g_pub_tx. ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�������˻���ϵ��Ϣ���޴˿��Ż��˺ţ�����!\
			[%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"��ȡ�������˻���ϵ��Ϣ�쳣![%s]",\
				g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		}
	sprintf(acErrMsg,"������ʾ�˺Ų�ѯ�����״̬ PASS!");
	WRITEMSG  

	/* ���ú��� */
	
	ret = pub_cif_CifCheck ( g_mdm_ac_rel.id_type ,g_mdm_ac_rel.id_no ,\
				&cif_no , cif_type );
	if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_cif_CifCheck����!");
			WRITEMSG
			goto ErrExit;	
		}

	set_zd_data( "0690", cif_type );      /* �ͻ����� */

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
