/*************************************************
* �� �� ��:  sp9865.c
* ���������� ���˺��´��ſ�ƻ�
*
* ��    ��:  jack
* ������ڣ� 2003��1��17�� 
*
* �޸ļ�¼:
* ��   �� :
* �� �� ��:
* �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include <string.h>
#define EXTERN
#include "public.h"
#include "doc_dic_c.h"

int sp9865()
{
	struct doc_dic_c doc_dic;
    char filename[100];
    int ret;
	FILE *fp;
	int i=0;
	int j=0;
   
    get_zd_data("0630",g_pub_tx.ac_no);

	pub_base_strpack( g_pub_tx.ac_no );

	ret = Doc_dic_Dec_Sel( g_pub_tx.reply,"tab='%s' order by seqn" , 
		g_pub_tx.ac_no );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	while(1)
	{
		ret = Doc_dic_Fet_Sel ( &doc_dic , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		i+=5;
		j+=5;
		fprintf(fp,"%d|%s|%c|%s|%s|\n",
			i, doc_dic.tab, '0', doc_dic.fied,"=" );
		fprintf(fp,"%d|%s|%c|%s|%s|\n",
			j, doc_dic.tab, '1', doc_dic.fied," " );
	}
	Doc_dic_Clo_Sel ();

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
