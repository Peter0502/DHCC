/*****************************************************
* �� �� ��:  sp9551.c
* ���������� ����֧��ϵͳ�к�(����) ��ѯ����ϵͳ���к�(������)������(������) 
* ��    ��:  ChenMing
* ������ڣ� 2006��8��7�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_orno_brno_c.h"

int sp9551()
{
	int ret=0;
	char or_br_no[13];
	struct hv_orno_brno_c shv_orno_brno;    
	
	memset(or_br_no,'\0',sizeof(or_br_no));
	memset(&shv_orno_brno,0x00,sizeof(shv_orno_brno));
 	vtcp_log("sp9551��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	
	get_zd_data("0320",or_br_no);
	or_br_no[12]='\0';  
	
	if( strlen(or_br_no)  == 0 ) 
	{
		sprintf(acErrMsg,"�кŴ���[%d]",ret);
   	vtcp_log("�кŴ��� ! [%s][%d]",__FILE__,__LINE__);
		
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_orno_brno_Sel(g_pub_tx.reply,&shv_orno_brno, "or_br_no='%s'",or_br_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴��к�");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��������[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	
 vtcp_log("[%s][%d]  br_name=[%s] br_no=[%s]",__FILE__,__LINE__,shv_orno_brno.br_name,shv_orno_brno.br_no);
 set_zd_data("0910",shv_orno_brno.br_no); 
 set_zd_data("0961",shv_orno_brno.br_name);


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

