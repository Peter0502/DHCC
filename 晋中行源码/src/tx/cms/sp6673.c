/*************************************************
* 文 件 名: sp6673.c
* 功能描述：此交易完成信贷撤销功能
*
* 作    者: chenchao
* 完成日期：2011-7-20 9:50:43
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:

insert into tx_def values('6673','信贷撤销交易','10000000000000000000000000000000000000000000000000000000010000000010000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6673','0','6673','#$');
insert into tx_sub_def values ('6673','信贷撤销交易','sp6673','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_bank_acc_po_c.h"

int sp6673()
{
	int	iRet = 0;
	int	iCount = 0;
	double	dSumPoAmt=0.00;
	char	cCms_no[30];	/** 信贷授权编号 **/
	

	struct cms_rollback_c sCms_rollback;
	struct mo_bank_acc_po_c sMo_bank_acc_po;
	
	memset(cCms_no,0x00,sizeof(cCms_no));
	memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
	memset(&sMo_bank_acc_po,0x00,sizeof( sMo_bank_acc_po));
	
	pub_base_sysinit();
	get_zd_data("0580",cCms_no);
	pub_base_strpack(cCms_no);
	vtcp_log("[%s][%d]进入信贷撤销交易.......撤销编号为[%s]",__FILE__,__LINE__,cCms_no);
	iRet = Cms_rollback_Dec_Upd( g_pub_tx.reply, " cms_sq_no = '%s' ", cCms_no );
	if(iRet)
	{
		vtcp_log("[%s][%d]定义游标出错!!!",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "D101" );
		set_zd_data(DC_GET_MSG,"定义游标出错!!!");
		goto ErrExit;
	}
	while(1)
	{
		memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
		iRet = Cms_rollback_Fet_Upd(&sCms_rollback, g_pub_tx.reply );
		if( iRet==100 )
		{
			if(iCount)
			{
				break;
			}else
			{
				vtcp_log("[%s][%d]未找到要撤销的记录.请检查",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "XD00" );
				set_zd_data(DC_GET_MSG,"没有找到该条记录!!!");
				goto ErrExit;
			}
		}else if(iRet)
		{
			vtcp_log("[%s][%d]未找到要撤销的记录.请检查",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "AT06" );
			set_zd_data(DC_GET_MSG,"查找记录失败!!!");
			goto ErrExit;
		}
		if(sCms_rollback.sts[0] == '1')
		{
			vtcp_log("[%s][%d]该授权号已撤销",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "AT06" );
			set_zd_data(DC_GET_MSG,"该授权号已撤销!!!");
			goto ErrExit;
		}
		iRet = sql_count(sCms_rollback.beg_table,"%s",sCms_rollback.beg_sql);
		if(iRet > 1)
		{
			vtcp_log("[%s][%d]查找条数为[%d],撤销失败,查找原因!!!",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "H007" );
			set_zd_data(DC_GET_MSG,"撤销失败!!!");
			goto ErrExit;
		}
		if(sCms_rollback.do_type[0] == 'I')	/** 信贷原始为插入操作,则撤销为删除操作  **/
		{
			vtcp_log("[%s][%d]要执行的撤销语句为[Delete * from %s where %s]",__FILE__,__LINE__,sCms_rollback.beg_table , sCms_rollback.beg_sql);
			iRet = sql_execute2("Delete from %s where %s ",sCms_rollback.beg_table , sCms_rollback.beg_sql);
			if(iRet)
			{
				vtcp_log("[%s][%d]删除表记录失败",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "H007" );
				set_zd_data(DC_GET_MSG,"删除表记录失败");
				goto ErrExit;
			}
		}else if(sCms_rollback.do_type[0] == 'U')	/** 信贷原始为更新操作,则撤销为更新操作  **/
		{
			vtcp_log("[%s][%d]要执行的撤销语句为[Update %s set %s where %s ]",__FILE__,__LINE__,sCms_rollback.beg_table , sCms_rollback.beg_sql);
			iRet = sql_execute2("Update %s set %s where %s ",sCms_rollback.beg_table, sCms_rollback.rolbk_sql,sCms_rollback.beg_sql);
			if(iRet)
			{
				vtcp_log("[%s][%d]更新表记录失败",__FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "D105" );
				set_zd_data(DC_GET_MSG,"更新表记录失败");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]beg_table=[%s]",__FILE__,__LINE__,sCms_rollback.beg_table);
			/** 处理承兑汇票撤销后核心不能同步状态和协议金额的问题 ***/
			if(memcmp(sCms_rollback.beg_table,"mo_bank_acc_po",14)==0)
			{
				char cTemp[61];
				memset(cTemp,0x00,sizeof(cTemp));
				memcpy(cTemp,sCms_rollback.beg_sql,27);
				iRet = sql_sum_double( "mo_bank_po","po_amt", &dSumPoAmt, "%s and po_sts='1'",cTemp);
				if( iRet )
				{
					sprintf( acErrMsg, "执行sql_sum_double错误![%d]", iRet );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}
				iRet = Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"%s and acc_po_sts in ('A','B','0','1','2')",sCms_rollback.beg_sql);
				if( iRet )
				{
					sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错误![%d]", iRet );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"执行数据库查询失败！");
					goto ErrExit;
				}
				if(pub_base_CompDblVal(sMo_bank_acc_po.sign_amt,dSumPoAmt) ==0 )
				{
					iRet = sql_execute2("Update mo_bank_acc_po set acc_po_sts='2' where %s and acc_po_sts in ('A','B','0','1')",sCms_rollback.beg_sql);
					if(iRet)
					{
						vtcp_log("[%s][%d]更新表记录失败",__FILE__,__LINE__);
						strcpy( g_pub_tx.reply, "D105" );
						set_zd_data(DC_GET_MSG,"更新表记录失败");
						goto ErrExit;
					}
				}
			}
		}
		sCms_rollback.sts[0] = '2';/** 置为已撤销 **/
		iRet = Cms_rollback_Upd_Upd(sCms_rollback,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("[%s][%d]更新失败[%d]!!!",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "H007" );
			set_zd_data(DC_GET_MSG,"更新撤销登记薄失败!!!");
			goto ErrExit;
		}
		iCount ++;
	}
	Cms_rollback_Clo_Upd();
	vtcp_log("[%s][%d]撤销处理成功.....",__FILE__,__LINE__);

OkExit:
	sprintf( acErrMsg, "撤销成功!!!" );
	WRITEMSG
	set_zd_data("0670","0");	/**撤销成功 **/
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "撤销失败reply[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data("0670","1");	/**撤销失败 **/
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
