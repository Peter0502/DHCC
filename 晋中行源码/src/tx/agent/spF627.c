/*************************************************
* 文 件 名: spF627.c
* 功能描述：代收代付数据清理
*
* 作    者: jack
* 完成日期: 2004年06月25日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ag_unitinfo_c.h"
#include "pay_agent_c.h"

spF627()
{ 	
	int ret=0;

	char cUnit_no[5];
	char cMsg[200];
	int i=0;
  int sum=0;
	struct ag_unitinfo_c sAg_unitinfo;
	struct pay_agent_c sPay_agent;
  double sumamt=0.00;
	memset(cUnit_no,0x0,sizeof(cUnit_no));
  memset(cMsg,0x0,sizeof(cMsg));
	get_zd_data("0920",cUnit_no);

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该单位编号对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*ret = sql_execute( "update ag_unitinfo set beg_date = 99999999, \
		end_date = 99999999 where unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "修改单位[%s]信息中批量日期出错[%d]", \
			cUnit_no, ret );
		WRITEMSG
		goto ErrExit;
	}
*/
  sum=sql_count("pay_agent","unit_no='%s'",cUnit_no);
  if(ret)
  {
  	sprintf( acErrMsg, "删除单位[%s]未缴记录失败!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
  }
  if(sum==0)
  {
  	sprintf( cMsg,"该单位没有可清理的数据!");
  }
  else
  {
  	sprintf( cMsg,"");
  }
  ret=sql_sum_double("pay_agent","tram" ,&sumamt,"unit_no='%s'",cUnit_no);
  if(ret)
  {
  		sprintf( acErrMsg, "删除单位[%s]未缴记录失败!!!", cUnit_no );
			WRITEMSG
			goto ErrExit;
  }
	ret = sql_execute( "delete from pay_agent where unit_no = '%s'", cUnit_no );
		/*and result = '1'", cUnit_no );*/	/*考虑是否全部删除*/
	if ( ret )
	{
		sprintf( acErrMsg, "删除单位[%s]未缴记录失败!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}
   set_zd_data("0130",cMsg);
   set_zd_int("0530",sum);
   set_zd_double("0400",sumamt);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->批量清理代收代付数据成功!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"----->批量清理代收代付数据失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
