/*****************************************************
* �� �� ��:  sp9552.c
* ���������� ��������ϵͳ���к�(������) ��ѯ ֧��ϵͳ�к�(����)������ 
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
#include "hv_brno_orno_c.h"

int sp9552()
{
	int ret=0;
	char br_no[6];
	struct hv_brno_orno_c hv_brno_orno;    
	
	memset(br_no,'\0',sizeof(br_no));
	memset(&hv_brno_orno,0x00,sizeof(hv_brno_orno));
 	vtcp_log("sp9553��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	
	get_zd_data("0910",br_no);
	br_no[5]='\0';	
	
	if( strlen(br_no)  == 0 ) 
	{
		sprintf(acErrMsg,"�����Ŵ���[%d]",ret);
   	vtcp_log("�����Ŵ��� ! [%s][%d]",__FILE__,__LINE__);
		
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno, "br_no='%s'",br_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴˻�����");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	
 vtcp_log("[%s][%d]  or_br_no=[%s] or_br_name=[%s]",__FILE__,__LINE__,hv_brno_orno.or_br_no,hv_brno_orno.or_br_name);
 set_zd_data("0320",hv_brno_orno.or_br_no);   /* ע�⣺ǰ̨���պ�Ҫ��ȡǰ12λ*/
 set_zd_data("0961",hv_brno_orno.or_br_name);


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


