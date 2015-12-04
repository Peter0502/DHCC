/***************************************************************
* 文 件 名:     spD021.c
* 功能描述：	财务划转交易
*
* 作    者:     LiuHuafeng
* 完成日期：    2006-10-26 16:00
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"
#include "com_item_add_c.h"
#include "zjgl_mst_c.h"

spD021()
{
	int	ret=0;
	int mcnt=0, zcnt=0;
	char  cBr_no[BRNO_LEN+1];
	char  cAcc_no[8];
	double dTxamt=0.0,dRelamt=0.0;
	struct com_item_c sCom_item;
	int iProcType=0; /* 0转出 1扣划 */
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&sCom_item, 0 , sizeof(sCom_item));
	memset(cBr_no, 0 , sizeof(cBr_no));
	memset(cAcc_no, 0 ,sizeof(cAcc_no));
	get_zd_data("0300",g_pub_tx.ac_no1);		/*** 母帐号 ***/
	pub_base_old_acno( g_pub_tx.ac_no1 );
	if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,BRNO_LEN) && memcmp(g_pub_tx.tx_br_no,CW_BR_NO, BRNO_LEN))
	{
		vtcp_log("%s %d 交易机构必须是清算中心或者是财务中心[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		strncpy(g_pub_tx.reply,"S043",4);
		goto ErrExit;
	}
	get_zd_data("0910",cBr_no);
	if(memcmp(cBr_no,g_pub_tx.tx_br_no,BRNO_LEN)==0 && memcmp(cBr_no,QS_BR_NO,BRNO_LEN)==0)
	{
		vtcp_log("%s %d 交易机构是清算中心[%s]但对方机构不能是清算机构[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,cBr_no);
		strncpy(g_pub_tx.reply,"S043",4);
		goto ErrExit;
	}
	get_zd_data("0640",cAcc_no);
	/* 得到科目基本信息 */
	{
		memset(&sCom_item, 0 , sizeof(sCom_item));
		ret = Com_item_Sel(g_pub_tx.reply,&sCom_item, "acc_no ='%s' ",cAcc_no);
		if ( ret==100 )
		{
			strncpy(g_pub_tx.reply,"S033",4);
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
		if(sCom_item.sub_acc_yn[0]=='Y')
		{
			strncpy(g_pub_tx.reply,"P301",4);
			sprintf(acErrMsg,"输入的科目不是最底层科目");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
		if(sCom_item.dc_ind[0]=='0')
		{
			strncpy(g_pub_tx.reply,"T059",4);
			sprintf(acErrMsg,"输入的科目是两性科目");
			WRITEMSG
			goto	ErrExit;
		}
	}
	if(memcmp(cBr_no,QS_BR_NO, BRNO_LEN)==0)
	{
		/* 表明是转出交易 */
		iProcType=0;/* 转出 */
	}
	else
	{
		double dFd42=0.0;
		get_zd_double("0420",&dFd42);
		dFd42=0-dFd42;
		set_zd_double("0420",dFd42);
		set_zd_double("0430",dFd42);/*用于划拨 add by LiuHuafeng 2006-12-26 10:48 */
		/* 根据科目的借贷方向判断是扣划还是划拨 */

		if(sCom_item.dc_ind[0]=='1')
			iProcType=1; /* 扣划 */
		else
			iProcType=2; /* 划拨 */

		vtcp_log("%s,%d ------------- look iProcType==[%d] ",__FILE__,__LINE__,iProcType);
	}
	/* 检查对方机构是否可以进行扣划处理 */
	{
		struct com_item_add_c sCom_item_add;
		memset(&sCom_item_add, 0 , sizeof(sCom_item_add));
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
		/*if(sCom_item_add.add_ind[0]=='0')  老刘说判断第三位*/
		if(sCom_item_add.add_ind[2]=='0')
		{
			strncpy(g_pub_tx.reply,"T058",4);
			sprintf(acErrMsg,"输入的科目不需要进行资金管理!!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
	}
	dTxamt=0.0,dRelamt=0.0;
	get_zd_double("0390",&dTxamt);
	vtcp_log("%s,%d dTxamt===[%f]",__FILE__,__LINE__,dTxamt);
	get_zd_double("0430",&dRelamt);
	/* 比较输入的两个金额 */
	{
		if(dTxamt-dRelamt<0)
		{
			strncpy(g_pub_tx.reply,"T058",4);
			sprintf(acErrMsg,"输入的扣划金额小额实际转出金额");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto	ErrExit;
		}
	}
/*
	dTxamt=0-dTxamt;
	set_zd_double("0390",dTxamt);
*/
	/* 查找郭总的表,判断相关业务数据-- 这部分由公共函数处理  */
	{
		double dTmpbal=0;
		struct zjgl_mst_c sZjgl_mst;
		memset(&sZjgl_mst, 0 , sizeof(sZjgl_mst));
		memcpy(sZjgl_mst.opn_br_no,cBr_no,BRNO_LEN);
		get_zd_data("0210",sZjgl_mst.cur_no);
		memcpy(sZjgl_mst.acc_no,cAcc_no,sizeof(sZjgl_mst.acc_no)-1);
		ret = Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst, 
			"opn_br_no='%s' and cur_no='%s' and acc_no ='%s' ", cBr_no,sZjgl_mst.cur_no,cAcc_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"输入的科目不需要进行资金管理!!");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"数据库操作失败[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("%s,%d into ipubf_acct_zjgl_trance",__FILE__,__LINE__);
		/*这部分由公共函数自动处理
		 *ret=ipubf_acct_zjgl_trance(cBr_no,sZjgl_mst.cur_no,cAcc_no,&dTxamt,&dTmpbal);
		 *if(ret)
		 *{
		 *	strncpy(g_pub_tx.reply,"T058",4);
		 *	vtcp_log("%s,%d 更新郭总资产管理登记簿失败",__FILE__,__LINE__);
		 *	sprintf(acErrMsg,"更新资产管理登记簿表失败[%d]",ret);
		 *	WRITEMSG
		 *	goto ErrExit;
		 *}
		 *vtcp_log("%s,%d 更新郭总资产管理登记簿成功",__FILE__,__LINE__);
		 *****************************************/
	}
	get_zd_data("0680",g_pub_tx.ct_ind);		/*** 现转标志 ***/
	get_zd_double("0390",&g_pub_tx.tx_amt1);

	sprintf(acErrMsg, "---ac_no=[%s], amt=[%lf], ct_ind=[%s]", 
		g_pub_tx.ac_no1, g_pub_tx.tx_amt1,g_pub_tx.ct_ind);
	WRITEMSG

OkExit:
	if(iProcType==1)
		strcpy(g_pub_tx.reply,"0001");
	else if(iProcType==2)
		strcpy(g_pub_tx.reply,"0002");
	else
		strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"更新资金管理登记簿成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"更新资金管理登记簿zjgl_mst失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
