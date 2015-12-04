/*****************************************************
* 文 件 名:  sp9580.c
* 功能描述： 查找com_item com_item_add 返回科目相关信息
* 作    者:  LiuHuafeng 2006-10-25 17:58
* 完成日期： 2006-10-25 17:58
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "com_item_c.h"
#include "com_item_add_c.h"
int sp9580()
{
	int ret=0,i;
	struct com_item_c  sCom_item;
	struct com_item_add_c sCom_item_add;
	char cAcc_no[8];
	char cTmpbuf[51];

	memset(&sCom_item, 0 , sizeof(sCom_item));
	memset(&sCom_item_add, 0 , sizeof(sCom_item_add));
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	vtcp_log("sp9580 开始！[%s][%d]",__FILE__,__LINE__);
	pub_base_sysinit();
	get_zd_data("0640",cAcc_no);
	
	/******获取三个索引的值*******/
	ret = Com_item_Sel(g_pub_tx.reply,&sCom_item, "acc_no ='%s' ",cAcc_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"输入的科目不存在!!");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"数据库操作失败[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_item_add_Sel(g_pub_tx.reply,&sCom_item_add, "acc_no ='%s' ",cAcc_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"输入的科目不需要进行资金管理!!");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"数据库操作失败[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	cTmpbuf[0]=sCom_item.sub_acc_yn[0];
	cTmpbuf[1]=sCom_item.acc_lvl[0];
	cTmpbuf[2]=sCom_item.dc_ind[0];
	cTmpbuf[3]=sCom_item.roll_ind[0];
	cTmpbuf[4]=sCom_item.acc_knd[0];
	cTmpbuf[5]=sCom_item.fore_ind[0];
	cTmpbuf[6]=sCom_item.equa_ind[0];
	cTmpbuf[7]=sCom_item.amt_dc_ind[0];
	cTmpbuf[8]=sCom_item.od_ind[0];
	cTmpbuf[9]=sCom_item.in_ind[0];
	cTmpbuf[10]=sCom_item.scope[0];
	memcpy(cTmpbuf+11,sCom_item_add.add_ind,20);
	
	pub_base_full_space(cTmpbuf,sizeof(cTmpbuf)-1);
	set_zd_data("0641",sCom_item.acc_hrt);
	set_zd_data("0830",sCom_item.acc_name);
	set_zd_data("0810",cTmpbuf); 
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
