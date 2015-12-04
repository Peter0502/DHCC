/*************************************************
* 文 件 名:    sp5666.c
* 功能描述:   
*              取得柜面通清算帐户
*
* 作    者:    wanglei
* 完成日期:    2010年06月29日
* 交易定义:
	insert into tx_def values ('5666','取得柜面通清算帐户','10000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values ('5666',0,'5666','#$');
	insert into tx_sub_def values ('5666','取得柜面通清算帐户','sp5666','0','0');
* 修改记录:    
*     1. 日    期: 
*        修 改 人: 
*        修改内容: 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

	
sp5666()  
{ 		
	/** 数据初始化 **/
	struct com_parm_c      com_parm; 

	int  ret =0;	
	char cCode[4];
	char cSeqn[3];
	int  iSeqn=0;

	memset (&com_parm, 0x00, sizeof(struct com_parm_c));
	memset (cCode, 0x00, sizeof(cCode));
	memset (cSeqn, 0x00, sizeof(cSeqn));
	
	/** 取值、赋值 **/	
	get_zd_data( "0210" , cSeqn );  /* 参数序号 */
	get_zd_data( "0230" , cCode );  /* 参数代码 */
	iSeqn = atoi(cSeqn);
	vtcp_log("[%s][%d] cSEQN=[%s]",__FILE__,__LINE__,cSeqn);
	vtcp_log("[%s][%d] iSEQN=[%d]",__FILE__,__LINE__,iSeqn);
	vtcp_log("[%s][%d] CODE=[%s]",__FILE__,__LINE__,cCode);
	
					  
	/* 根据客户号查询客户名称 */			  
	ret = Com_parm_Sel( g_pub_tx.reply , &com_parm , " parm_code= '%s'  and parm_seqn = %d " , cCode,iSeqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"公共参数表中无此记录![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"G001");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取公共参数表异常![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O005");
		goto ErrExit;
	}

	set_zd_data("0370",com_parm.val);/**客户姓名**/

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
