/*****************************************************
* �� �� ��:  sp9598.c
* ���������� ����123������sql��䷵�ز�ѯ��� 
* ������ڣ� 2007-05-09 17:58
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��: SSH
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_c503r_c.h"
int sp9598()
{
	int ret=0,i,j,level;
	char sql_str[1024];
	char acno[20];
	char cFilename[128];
	FILE *fp;
	struct lv_c503r_c wd_lv_c503r;
	memset(sql_str, 0 , sizeof(sql_str));
	memset(acno, 0 , sizeof(acno));
	vtcp_log("sp9598 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("1230",sql_str);

	Gl_file_first=1;	
	pub_base_AllDwnFilName(cFilename);
	fp=fopen(cFilename,"wb");
	if(fp==NULL){
		vtcp_log( "%s,%dδ���ļ�",__FILE__,__LINE__ );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D104" );
		goto ErrExit;
	}
	ret=pubf_pubdb_comsel(fp,sql_str);
	if(ret){
		vtcp_log( "%s,%dδ�ҵ�ָ����¼",__FILE__,__LINE__ );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D104" );
		fclose(fp);
		goto ErrExit;
	}
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
