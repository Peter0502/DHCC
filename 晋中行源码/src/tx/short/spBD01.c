/*************************************************
* �� �� ��:    spBD01.c
* ����������   
*              �����ʺŻ����ʻ����
*
* ��    ��:    lance
* ������ڣ�   2003��03��14��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
update data_dic set data_leng=6 where data_code='1040';
update bitmap set leng=6 where bit_seq=104;
insert into tx_def values('9191','����ƾ֤','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','1','0');
insert into tx_flow_def values('9191',0,'BD01','#$');
insert into tx_sub_def values('BD01','����ƾ֤','spBD01','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
spBD01()  
{ 		
	int ret=0,ac_seqn;
	long lTx_date=0;
	long lTrace_no=0;
	char sPrint_num[7];
	struct trace_log_c g_trace_log;
	char file_name[101];		
	FILE *fp =NULL;
	pub_base_sysinit();
	/** ȡֵ����ֵ **/	
	get_zd_long("0450",&lTx_date);
	get_zd_long("0460",&lTrace_no);
	get_zd_data("1040",sPrint_num);
	/* �����ˮ�Ƿ����,��鲹���ļ��Ƿ���� */
	memset(&g_trace_log,0x00,sizeof(g_trace_log));
	ret=Trace_log_Sel(g_pub_tx.reply,&g_trace_log,"tx_date=%ld and trace_no=%ld and tel='%s'",lTx_date,lTrace_no,g_pub_tx.tel);
	if(ret)
	{
		vtcp_log("%s,%d,�ý�����ˮ������!",__FILE__,__LINE__);
		goto ErrExit;
	}	
	/* ����ļ��Ƿ���� */
	memset(file_name,0x00,sizeof(file_name));
	/**sprintf(file_name,"%s/txt/%ld-%ld-%s.txt",getenv("HOME"),lTx_date,lTrace_no,sPrint_num);*/
	sprintf(file_name,"%s/txt/%ld-%ld-%s.txt",getenv("HOME"),lTx_date,lTrace_no,g_trace_log.tel);
	fp=fopen(file_name,"r");	
	if(fp == NULL)
	{
		vtcp_log("%s,%d,���ļ�������![%s]",__FILE__,__LINE__,file_name);
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
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
