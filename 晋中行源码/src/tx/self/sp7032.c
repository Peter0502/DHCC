/*************************************************
* �� �� ��:  sp7032.c
* ���������� ��С�����ʴ�ӡ
* ��    ��:  Cole
* ������ڣ� 2013��8��16��
* �ο����ݣ�����һ��spJ911��spJ912��д������
insert into tx_def values('7032','��С�����ʴ�ӡ','00000010000000000000100000000000000000000000000100000000001100000000000000000000000000000000000000000000000000000000000000000000','1','1','2');
insert into tx_flow_def values('7032','0','7032','#$');
insert into tx_sub_def values('7032','��С�����ʴ�ӡ','sp7032','0','0');
*************************************************/
#include <string.h>
#include "public.h"
sp7032()
{
	int ret=0;
	char cTel[5]={0};
	char cVtype[4]={""};
	char cVno[17]={0};
	char cNVno[17]={0};
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0070",cTel);
	get_zd_data("0230",cVtype);
	get_zd_data("0580",cVno);
	vtcp_log("[%s][%d]��ԱcTel = [%s],ƾ֤����cVtype = [%s],ƾ֤��cVno =[%s]",__FILE__,__LINE__,cTel,cVtype,cVno);
	memcpy(cNVno,cVno,sizeof(cNVno)-1);
	ret=pub_com_ChkNoteUse(cVtype);/*���ƾ֤�����Ƿ����*/
	if( ret )
	{
		vtcp_log("[%s][%d]ƾ֤����ͣ��״̬",__FILE__,__LINE__);
		goto ErrExit;
	} 
	/*ƾ֤����*/
	ret=pub_com_NoteUse(0,0,cVtype,cVno,cVno,cTel);
	if( ret )
	{
		vtcp_log("[%s][%d]��Աʹ��ƾ֤����",__FILE__,__LINE__);
		goto ErrExit;
	}
	/**�õ���һ��ƾ֤��**/
	ret=pub_com_GetNoteMin(cVtype,cNVno,cTel);
	if (ret)
	{
		vtcp_log("[%s][%d]û��ƾ֤��",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log("[%s][%d] cNVno=[%s]",__FILE__,__LINE__,cNVno);
	set_zd_data("0590",cNVno);
		
		
OkExit:
		strcpy( g_pub_tx.reply, "0000" );
		vtcp_log("[%s][%d] success !!: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 0;
ErrExit:
        memcpy(g_pub_tx.reply,"T063",4);
		vtcp_log("[%s][%d] fail !!: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;	
}
