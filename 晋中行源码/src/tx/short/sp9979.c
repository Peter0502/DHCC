/*************************************************
* �� �� ��:    sp9979.c
* ����������   ����ͬ���Ƿ����
*
* ��    ��:    mike
* ������ڣ�   2003��08��14��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

sp9979()
{ 		
	int		ret;
	struct	protect_note_c pro_note;

	/** ���ݳ�ʼ�� **/
	ret = 0;
	memset( &pro_note, 0x00, sizeof( struct protect_note_c ));

	get_zd_data("0330",g_pub_tx.ac_no);

	ret = Protect_note_Sel(g_pub_tx.reply, &pro_note,"pact_no='%s' and protect_sts<>'*' ",g_pub_tx.ac_no);
	if ( ret == 0  )
	{
		sprintf(acErrMsg,"ac_no to name erro [%s] ",g_pub_tx.ac_no,g_pub_tx.cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B071");
		goto ErrExit;
	}
	ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , 
			" pact_no='%s' and ac_sts!='9' " , g_pub_tx.ac_no );
	if( ret && ret!=100)
	{
		sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret == 0 )
	{
		sprintf(acErrMsg,"��ͬ���Ѵ���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L020");
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
