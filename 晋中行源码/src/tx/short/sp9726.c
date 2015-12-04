/*************************************************
* �� �� ��: sp9726.c
* ��������������ƾ֤���������ת��־
*
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "note_parm_c.h"

sp9726()  
{ 		
	int ret=0,i,vcnt;
	char filename[50];
	FILE *fp;

	struct note_parm_c g_note_parm;
	struct dic_data_c g_dic_data;

	memset(&g_note_parm,0x00,sizeof(struct note_parm_c));
	memset(&g_dic_data,0x00,sizeof(struct dic_data_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1023",g_note_parm.note_type);			/*ƾ֤����*/

	/*** ��ȡ�������� ***/
	ret=Note_parm_Sel(&g_pub_tx.reply,&g_note_parm,"note_type='%s'", \
						g_note_parm.note_type);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"�ý������Բ�����![%s]",g_note_parm.note_type);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"P132");
		 goto ErrExit;
	}
	else if( ret )
	{
	 	 sprintf(acErrMsg,"ȡ���������쳣![%d]",g_reply_int);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"P133");
	 	 goto ErrExit;
	}

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

	/***��ת�����б�***/
	if(g_note_parm.ct_ind[0]=='C')
	{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='ct_cash'");
		if( ret ) goto ErrExit;
	}
	else if(g_note_parm.ct_ind[0]=='T')
	{
		ret=Dic_data_Dec_Sel(g_pub_tx.reply,"pr_code='ct_tran' \
			order by pr_seq");
		if( ret ) goto ErrExit;
	}
	else if(g_note_parm.ct_ind[0]=='O')
	{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='ct_cash' and \
			pr_code='ct_tran' order by pr_seq");
		if( ret ) goto ErrExit;
	}
	else 
	{
		sprintf( acErrMsg,"ƾ֤��ת�������岻�Ϸ�[%s]",g_pub_tx.ct_ind );
		WRITEMSG
		strcpy( g_pub_tx.reply,"N074");
		goto ErrExit;
	}

	i=0;
	while(1)
	{
		ret = Dic_data_Fet_Sel( &g_dic_data, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("102K",g_dic_data.pr_seq);
		}

		pub_base_strpack(g_dic_data.pr_seq);
		pub_base_strpack(g_dic_data.content);
		fprintf( fp,"102K|%s|%s|\n",g_dic_data.content,g_dic_data.pr_seq );

		i++;
		vcnt++;
	}
	Dic_data_Clo_Sel();

	/***ժҪ�����б�***/
	if(g_dic_data.pr_seq[0]=='1')
	{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='cash_imemo'");
		if( ret ) goto ErrExit;
	}
	else if(g_dic_data.pr_seq[0]=='2'||g_dic_data.pr_seq[0]=='3'|| \
			g_dic_data.pr_seq[0]=='4')
	{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
		if( ret ) goto ErrExit;
	}

	i=0;
	while(1)
	{
		ret = Dic_data_Fet_Sel( &g_dic_data, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("102L",g_dic_data.pr_seq);
		}

		pub_base_strpack(g_dic_data.pr_seq);
		pub_base_strpack(g_dic_data.content);
		fprintf( fp,"102L|%s|%s|\n",g_dic_data.content,g_dic_data.pr_seq );

		i++;
		vcnt++;
	}
	Dic_data_Clo_Sel();
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
