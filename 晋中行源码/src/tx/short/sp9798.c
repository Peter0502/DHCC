/*************************************************
* �� �� ��: sp9798.c
* �������������ݿͻ����ʽ�����ʾ��ϸ_1108���׵Ŀ�ݲ�ѯ
* ��    ��: rob
* ������ڣ�2003��04��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9798()  
{ 		
	/** ���ݳ�ʼ�� **/
	struct dic_data_c        vdic_data; 
	int ret=0;
	char wherelist[40];
	char filename[100];
	char type[2],tmp[5];
	FILE *fp;
	int i=0;

	memset (&vdic_data, 0x00, sizeof(struct dic_data_c));
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "0670" , type );			/* ���ʽ���� */
	
	if(type[0]=='1')
		strcpy(wherelist,"life_de1");
	else if(type[0]=='2')
		strcpy(wherelist,"life_de2");
	else if(type[0]=='3')
		strcpy(wherelist,"life_de3");
	else if(type[0]=='4')
		strcpy(wherelist,"life_de4");
	else if(type[0]=='5')
		strcpy(wherelist,"life_de5");
	else if(type[0]=='6')
		strcpy(wherelist,"life_de6");
	else if(type[0]=='7')
		strcpy(wherelist,"life_de7");
	else if(type[0]=='8')
		strcpy(wherelist,"life_de8");

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

	/***ѡ���б�***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s'",wherelist);
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
	
		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )	set_zd_data("0220",vdic_data.pr_seq);
		fprintf( fp,"0220|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq );
		i++;
	}

vtcp_log("SUCCESSSUCCESSSUCCESS");
	Dic_data_Clo_Sel();

	if( i )
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
