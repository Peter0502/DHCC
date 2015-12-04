/*************************************************
* 文 件 名:  sp7032.c
* 功能描述： 大小额来帐打印
* 作    者:  Cole
* 完成日期： 2013年8月16日
* 参考内容：依据一代spJ911和spJ912编写，简化了
insert into tx_def values('7032','大小额来帐打印','00000010000000000000100000000000000000000000000100000000001100000000000000000000000000000000000000000000000000000000000000000000','1','1','2');
insert into tx_flow_def values('7032','0','7032','#$');
insert into tx_sub_def values('7032','大小额来帐打印','sp7032','0','0');
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
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0070",cTel);
	get_zd_data("0230",cVtype);
	get_zd_data("0580",cVno);
	vtcp_log("[%s][%d]柜员cTel = [%s],凭证种类cVtype = [%s],凭证号cVno =[%s]",__FILE__,__LINE__,cTel,cVtype,cVno);
	memcpy(cNVno,cVno,sizeof(cNVno)-1);
	ret=pub_com_ChkNoteUse(cVtype);/*检查凭证种类是否可用*/
	if( ret )
	{
		vtcp_log("[%s][%d]凭证处于停用状态",__FILE__,__LINE__);
		goto ErrExit;
	} 
	/*凭证销号*/
	ret=pub_com_NoteUse(0,0,cVtype,cVno,cVno,cTel);
	if( ret )
	{
		vtcp_log("[%s][%d]柜员使用凭证错误",__FILE__,__LINE__);
		goto ErrExit;
	}
	/**得到下一个凭证号**/
	ret=pub_com_GetNoteMin(cVtype,cNVno,cTel);
	if (ret)
	{
		vtcp_log("[%s][%d]没有凭证了",__FILE__,__LINE__);
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
