/*************************************************
* �� �� ��:    sp9968.c
* ����������   
*              ��ѯ���ڲ�Ʒ
*
* ��    ��:    rob
* ������ڣ�   2003��07��20��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"

sp9968()
{ 		
	/** ���ݳ�ʼ�� **/
	int	ret;
	int vcnt;
	char 	filename[100];
	FILE 	*fp;

	struct td_parm_c sTd_parm;

	memset(&sTd_parm, 0x0, sizeof(struct td_parm_c));


    vcnt = 0;
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	ret = Td_parm_Dec_Sel(g_pub_tx.reply, "1=1 order by prdt_no");
	if (ret != 0)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR !! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Td_parm_Fet_Sel(&sTd_parm, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg, "FETCH FOR SELECT ERROR !! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		fprintf( fp, "0230|%s|%s|\n", sTd_parm.title, sTd_parm.prdt_no );
		vcnt++;
	}
	Td_parm_Clo_Sel();

	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

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
