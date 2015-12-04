/*****************************************************
* �� �� ��:  sp9553.c
* ���������� ����֧��ϵͳ�кţ����У��� ֧��ϵͳ����  hv_uniontab
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
#include "hv_uniontab_c.h"

int sp9553()
{
	int ret=0;
	char or_br_no[25];
	struct hv_uniontab_c hv_uniontab;    
	
	memset(or_br_no,'\0',sizeof(or_br_no));
	memset(&hv_uniontab,0x00,sizeof(hv_uniontab));
 	vtcp_log("sp9553��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	
	get_zd_data("0320",or_br_no);
   	pub_base_strpack(or_br_no);	
   	
	if( strlen(or_br_no)  == 0 ) 
	{
		sprintf(acErrMsg,"�кŴ���[%d]",ret);
   	vtcp_log("�кŴ��� ! [%s][%d]",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]  or_br_no=[%s]",__FILE__,__LINE__,or_br_no);	
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab, "or_br_no='%s'",or_br_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴��к�");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
		strcpy( g_pub_tx.reply, "P099" );
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��������[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	if(memcmp(hv_uniontab.sendco,"NULL",4)==0)/* ʧЧ���� martin*/
		{
			sprintf(acErrMsg,"��ѯ����ʧЧ[%d]",ret);
		  WRITEMSG
		  strcpy( g_pub_tx.reply, "P314" );
		  goto ErrExit;
		}
 vtcp_log("[%s][%d]  br_name_f=[%s]",__FILE__,__LINE__,hv_uniontab.br_name_f);

 set_zd_data("0961",hv_uniontab.br_name_f);  /* ����ȫ��: ֧��ϵͳ���� */
 /****add by xyy 2008-02-21 YHBP****/
 vtcp_log("[%s][%d]  qs_no=[%s]",__FILE__,__LINE__,hv_uniontab.qs_no);
 set_zd_data("0330",hv_uniontab.qs_no);
 /****end by xyy 2008-02-21***/
 /****add by wudawei 20131113 YHBP****/
 vtcp_log("[%s][%d]  br_sts=[%s]",__FILE__,__LINE__,hv_uniontab.br_sts);
 set_zd_data("0200",hv_uniontab.br_sts);
 /****end by wudawei 20131113***/

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

