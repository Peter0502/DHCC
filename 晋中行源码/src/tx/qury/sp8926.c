/************************************************************
*文件名: sp8926.c
**程序功能:对com_item_add表进行修改和查询的操作
**作者:  Jarod
**编写日期: 2006-11-09
*************************************************************/

#define EXTERN
#include "public.h"
#include "com_item_add_c.h"

int sp8926()
{
	struct com_item_add_c sCom_item_add;
	char opr[2];
	char khbz;
	char dybz;
	char acc_no[8],acc_no1[8];
	int ret=0;
	
	memset(&khbz,0x00,sizeof(khbz));
	memset(&dybz,0x00,sizeof(dybz));	
	memset(&sCom_item_add,0x00,sizeof(struct com_item_add_c));
	memset(opr,0x00,sizeof(opr));
	memset(acc_no,0x00,sizeof(acc_no));
	memset(acc_no1,0x00,sizeof(acc_no1));

	pub_base_sysinit();
	
	get_zd_data("0640",acc_no);
	get_zd_data("0640",acc_no1);
	get_zd_data("0690",opr);
	
	vtcp_log("acc_no=[%s]",acc_no);
	if(opr[0] == '0')
	{
		ret=Com_item_add_Sel(g_pub_tx.reply,&sCom_item_add," acc_no='%s' ",acc_no);
		if(ret)
		{
			sprintf(acErrMsg,"查询科目附加表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
vtcp_log("取得的值: acc_no=[%s],khbz=[%c],dybz=[%c]  [%s]---[%d]",sCom_item_add.acc_no,sCom_item_add.add_ind[0],sCom_item_add.add_ind[1],__FILE__,__LINE__);
		set_zd_data("0670",&sCom_item_add.add_ind[0]);
		set_zd_data("0680",&sCom_item_add.add_ind[1]);
	}
	else if(opr[0] == '1')
	{
	vtcp_log("acc_no1=[%s]",acc_no1);
		get_zd_data("0670",&khbz);
		get_zd_data("0680",&dybz);
		ret=Com_item_add_Dec_Upd(g_pub_tx.reply,"acc_no='%s'",acc_no1);
		if(ret)
		{
			sprintf(acErrMsg,"声明科目附加表游标出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret=Com_item_add_Fet_Upd(&sCom_item_add,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"FETCH科目附加表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sCom_item_add.add_ind[0]=khbz;
		sCom_item_add.add_ind[1]=dybz;
		
		ret=Com_item_add_Upd_Upd(sCom_item_add,g_pub_tx.reply);	
		if(ret)
		{
			sprintf(acErrMsg,"update科目附加表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret=Com_item_add_Clo_Upd();
		if(ret)
		{
			sprintf(acErrMsg,"CLOSE科目附加表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		if( pub_ins_trace_log())
			goto ErrExit;
	} /*end of elseif */

	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"everyting is Ok.reply code is [%d] ",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before ERROR return.reply code is[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
