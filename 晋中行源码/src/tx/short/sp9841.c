	/*************************************************
* 文 件 名:  sp9841.c
* 功能描述： 查询单位编号信息
*
* 作    者:  jack
* 完成日期： 2003年1月30日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"
#include "ag_qs_gd_rel_c.h"

int sp9841()
{
	struct ag_unitinfo_c sUnitInfo;	/*单位信息表*/
	struct ag_qs_gd_rel_c sAg_qs_gd_rel;/* 结算账户与过渡账户关联表 */
	int ret = 0;
	char cUnit_no[5];
	char cFlag[2];

	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cFlag,0x0,sizeof(cFlag));

	pub_base_sysinit();
	/*get_zd_data( "0360",cUnit_no);*/
	get_zd_data( "0920", cUnit_no);
	get_zd_data( "0700", cFlag);

	if ( cFlag[0] == '0' )
	{
		goto OkExit;
	}

	memset( &sUnitInfo, 0x00, sizeof(sUnitInfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sUnitInfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该单位编号对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
/*
 ret = pub_ins_trace_log();
    if ( ret )
    {
        sprintf( acErrMsg, "记流水日志错" );
        WRITEMSG
        goto ErrExit;
    }
*/
	/**根据财务部要求加上非本机构的代发单位 直接返回错误 不让继续执行操作 add by wudawei 20130424 **/
	if ( strcmp( g_pub_tx.tx_br_no, sUnitInfo.opn_br_no ) )
	{
		sprintf( acErrMsg, "非代理单位建立机构,请用[%s]机构柜员做此交易", sUnitInfo.opn_br_no );
		WRITEMSG
		sprintf( acErrMsg, "请用[%s]机构柜员做此交易", sUnitInfo.opn_br_no );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( DC_REPLY, g_pub_tx.reply );
   		return 1;
	}
	/** add end 20130424 **/
	/** 为网银添加了ag_qs_gd_rel表（结算账户和过渡账户关联表） 查询单位资料时 将结算账户回显 add by wudawei 20130820 **/
	memset( &sAg_qs_gd_rel, 0x00, sizeof(sAg_qs_gd_rel) );
	ret = Ag_qs_gd_rel_Sel( g_pub_tx.reply , &sAg_qs_gd_rel ,\
			 "unit_no = '%s' ",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到结算账户和过渡账户关联对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行sAg_qs_gd_rel查询错误!ret=[%d]", ret );
		WRITEMSG
		
	}
	else if( ret==0 )
	{
		set_zd_data( "0310", sAg_qs_gd_rel.qs_ac_no );
	}
	/** add end 20130820 **/
	set_zd_data( "0250", sUnitInfo.unit_name );
	set_zd_data( "0710", sUnitInfo.opr_attri );
	set_zd_data( "0210", sUnitInfo.pay_type );
	set_zd_data( "0300", sUnitInfo.bank_acct_no );
	set_zd_data( "0720", sUnitInfo.auto_flag );
	set_zd_data( "0670", sUnitInfo.clr_flag );
	set_zd_data( "0350", sUnitInfo.up_unit );
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
