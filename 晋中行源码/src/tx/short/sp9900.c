/*************************************************
* �� �� ��: sp9900.c
* ����������֧��ҵ����״̬ѡ����ϸ���
* ��    ��: jane
* ������ڣ�2003��7��5��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/

#define EXTERN
#include "public.h"

sp9900()  
{
	char filename[100],qrychoi[2];
	FILE *fp;

	get_zd_data("0670", qrychoi);
	
	pub_base_AllDwnFilName( filename );
	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/***ѡ���б�***/
	if (!strcmp(qrychoi,"1"))
	{
		fprintf( fp,"0680|������|0|\n" );
		fprintf( fp,"0680|δ��ִ|1|\n" );
		fprintf( fp,"0680|�Ŷ�|2|\n" );
		fprintf( fp,"0680|����|3|\n" );
		fprintf( fp,"0680|�ѳ���|*|\n" );
	}
	else if (!strcmp(qrychoi,"2"))
	{
		fprintf( fp,"0680|������|0|\n" );
		fprintf( fp,"0680|����|5|\n" );
		fprintf( fp,"0680|���˻�|6|\n" );
		fprintf( fp,"0680|������|7|\n" );
	}

	fclose(fp);
	set_zd_data( DC_FILE_SND,"2" );
	
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
