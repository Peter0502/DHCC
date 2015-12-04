/*****************************************************
* �� �� ��:  sp9403.c
* ���������� С��������Ϣ��ѯ
* ��    ��:  chenhw
* ������ڣ� 2006-9-13 13:41 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_wbctl_c.h"
int sp9403()
{
	int ret=0,i;
	char cSts[2];
	char cPackday[9];
	char cPackid[9];
	
	struct lv_wbctl_c  g_lv_wbctl;
	memset(cSts,0,sizeof(cSts));
	memset(cPackday,'\0',sizeof(cPackday));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(&g_lv_wbctl,0x00,sizeof(struct lv_wbctl_c));
	
	vtcp_log("sp9403 ��ʼ��[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("0690",cSts);
	get_fd_data("0280",cPackday);
	get_fd_data("0290",cPackid);
	vtcp_log("[%s,%d]cPackday=[%s],packid=[%s]",__FILE__,__LINE__,cPackday,cPackid);
	
	g_lv_wbctl.pack_date = apatoi(cPackday,8);
	memcpy(g_lv_wbctl.packid,cPackid,sizeof(g_lv_wbctl.packid)-1);
	
	ret=Lv_wbctl_Sel(g_pub_tx.reply,&g_lv_wbctl,"pack_date='%s' and packid=%s",cPackday,cPackid);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"�޴�С����������");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"��ѯС���������׻�����Ϣ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("��ѯ�ɹ���  [%s][%d]",__FILE__,__LINE__); 
	set_zd_data("0520",g_lv_wbctl.cash_qs_no);
	set_zd_data("0690",g_lv_wbctl.stat);
	set_zd_long("0540",g_lv_wbctl.dtlcnt);
	set_zd_double("0430",g_lv_wbctl.totamt);
		
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
