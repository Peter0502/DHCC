/*****************************************************
* �� �� ��:  sp9599.c
* ���������� ����123������sql��䷵�ز�ѯ��� 
* ������ڣ� 2007-04-18 17:58
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��: SSH
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_c503r_c.h"
int sp9599()
{
	int ret=0,i,j,level;
	char sql_str[1024];
	char cFilename[128];
	FILE *fp;
	struct lv_c503r_c wd_lv_c503r;
	memset(sql_str, 0 , sizeof(sql_str));
	vtcp_log("sp9599 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("1230",sql_str);
	Gl_file_first=1;	
	pub_base_AllDwnFilName(cFilename);
	pub_comm_chan_pack(sql_str);
	ret=Lv_c503r_Dec_Sel(g_pub_tx.reply,sql_str);
	if(ret){
		vtcp_log( "%s,%dδ�ҵ�ָ����¼",__FILE__,__LINE__ );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D104" );
		goto ErrExit;
	}
	fp=fopen(cFilename,"wb");
	if(fp==NULL){
		vtcp_log( "%s,%dδ���ļ�",__FILE__,__LINE__ );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D104" );
		goto ErrExit;
	}
	while(1){
		memset(&wd_lv_c503r, 0 , sizeof(wd_lv_c503r));
		ret=Lv_c503r_Fet_Sel(&wd_lv_c503r,g_pub_tx.reply);
		if(ret && ret!=100){
			vtcp_log( "%s,%d���ݿ����[%d]",__FILE__,__LINE__,ret );
			WRITEMSG
			strcpy( g_pub_tx.reply,"D104" );
			goto ErrExit;
		}else if(ret==100){
			break;
		}
		fprintf(fp,"%s|����Ȩ��|%s\n",wd_lv_c503r.bakcode,wd_lv_c503r.sendgrant);
		fprintf(fp,"%s|����Ȩ��|%s\n",wd_lv_c503r.bakcode,wd_lv_c503r.recvgrant);
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
