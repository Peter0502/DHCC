/***************************************************************
* �� �� ��:     spD666.c
* ����������	�����¼ӽ���
* ��    ��:     JYWe
* ������ڣ�    2006��7��17��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD666()
{
	char testch[21],str1[21];
	int i,j;
	char tmpname[100];
	FILE *fp;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	
	memset(testch,0,sizeof(testch));
	memset(str1,0,sizeof(str1));

	get_zd_data("0250",testch);

	j=strlen(testch);
	for(i=0;i<j;i++){
		str1[j-i-1]=testch[i];	
	}	
	set_zd_data("0260",str1);
	
/***
    pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}
	fprintf(fp,"TEST123|456|789|abc|defg|\n");

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
****/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����¼ӽ��׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����¼ӽ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
