/*************************************************
*2204��ƾ֤��������
* �� �� ��:    sp9720.c
* ����������   
* ��    ��:    rob
* ������ڣ�   2003��01��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9720()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct dic_data_c        g_dic_data; 
	int ret=0;
	int vcnt=0;
	char wherelist[40];
	char filename[100];
	FILE *fp;
	int i=0;

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

	/***ƾ֤�б�***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='com_de' order by pr_seq");
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Dic_data_Fet_Sel( &g_dic_data, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1014",g_dic_data.pr_seq);
		}

		pub_base_strpack(g_dic_data.pr_seq);
		pub_base_strpack(g_dic_data.content);
		fprintf( fp,"1014|%s|%s|\n",g_dic_data.content,g_dic_data.pr_seq );

		i++;
		vcnt++;
	}
	Dic_data_Clo_Sel();

	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* ������� */

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
