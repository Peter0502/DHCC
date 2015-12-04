/*************************************************
* �� �� ��:    sp9745.c
* ����������   
*              ��ѯ�����˺�
*
* ��    ��:    rob
* ������ڣ�   2003��04��27��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

sp9745()
{ 		
	/** ���ݳ�ʼ�� **/
	struct	com_parm_c      sComParm;
	int	ret;
	int vcnt;
	char 	filename[100];
	FILE 	*fp;

	memset ( &sComParm, 0x00, sizeof( struct com_parm_c ) );

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

	ret = Com_parm_Dec_Sel( g_pub_tx.reply, "parm_code='CZZH'");
	if( ret ) 
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Com_parm_Fet_Sel( &sComParm, g_pub_tx.reply );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		/* ����һ�����������ļ�¼��Ϊ��ֵ����ǰ̨ */
		/***
		if( vcnt==0 )
		{
			sprintf(acErrMsg,"[%s]",sComParm.val);
			WRITEMSG
			set_zd_data("0720", sComParm.val);
		}
		****/
		vcnt++;
		pub_base_strpack(sComParm.parm_name);
		pub_base_strpack(sComParm.val);
		fprintf( fp, "0720|%s-%s|%d|\n", sComParm.parm_name,
			sComParm.val, sComParm.parm_seqn);
	}

	Com_parm_Clo_Sel();

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
