/*************************************************
* �� �� ��:    sp9966.c
* ����������   ��ѯ���߼���ժҪѡ��
*
* ��    ��:    jane
* ������ڣ�   2003��07��18��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "note_parm_c.h"

sp9966()
{ 		
	/** ���ݳ�ʼ�� **/
	struct	dic_data_c      sDicData;		/* �����ֵ���Ŀ���� */
	struct	note_parm_c      sNoteParm;		/* ƾ֤������ */
	int	ret;
	int 	vcnt=0;
	int 	i=0;
	char 	filename[100];
	FILE 	*fp;

	memset ( &sDicData, 0x00, sizeof( struct dic_data_c ) );
	memset ( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );

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

	ret = Dic_data_Dec_Sel( g_pub_tx.reply, "pr_code='tran_memo'" );
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel( &sDicData, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(sDicData.pr_seq);
		pub_base_strpack(sDicData.content);
		if(i==0)
		{
			set_zd_data("0230",sDicData.pr_seq);
		}

		fprintf(fp,"0230|%-20s|%s|\n",sDicData.content,sDicData.pr_seq);
		i++;
		vcnt++;
	}

	Dic_data_Clo_Sel();

	ret = Note_parm_Dec_Sel( g_pub_tx.reply, "name NOT LIKE '%%�浥' and name NOT LIKE '%%����' and name NOT LIKE '%%��%%'order by note_type" );
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Note_parm_Fet_Sel( &sNoteParm, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret ) goto ErrExit;
		else
		{
			/* ���ƾ֤�Ƿ���ʹ��״̬ */
			if( sNoteParm.use_ind[0] == 'N' )
			{
				sprintf( acErrMsg,"ƾ֤����ͣ��״̬!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N025" );
				goto ErrExit;
			}

			/* ����һ�����������ļ�¼��Ϊ��ֵ����ǰ̨ */
			if( i==0 )
			{
				sprintf(acErrMsg,"[%s]",sNoteParm.note_type);
				WRITEMSG
				set_zd_data( "0890", sNoteParm.note_type );
			}
			fprintf( fp, "0890|%-20s|%s|\n", sNoteParm.name, sNoteParm.note_type );
			i++;
			vcnt++;
		}
	}

	Note_parm_Clo_Sel();

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
