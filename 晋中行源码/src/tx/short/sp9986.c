/*************************************************
* 文 件 名:  sp9986.c
* 功能描述： 收费检查
*
* 作    者:  jack
* 完成日期： 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_def_c.h"
#include "tx_def_c.h"
#include "auth_code_c.h"
#include "com_tel_c.h"


int sp9986()
{
	char vact_flag[11]; /*流程标志*/
	char vlvl_flag[11]; /*归员级别标志*/
	char txcode[5];
	char auth[5];
	struct tx_def_c tx_def_v;
	struct com_tel_c vcom_tel;
	int ret=0;
	double amt;
	char mo[41];
	char passd[21];
	char vttl[21];
	char vtx_code[5];

	memset( vact_flag,0,sizeof(vact_flag) );
	memset( vlvl_flag,0,sizeof(vlvl_flag) );
  
  get_zd_data(DC_REPLY,txcode);
	vtcp_log("%d<> %s FD12-->TX_CODE=[%s]", __LINE__, __FILE__, txcode);
	pub_base_sysinit();

	get_zd_data(DC_REPLY,txcode);
	vtcp_log("%d<> %s FD12-->TX_CODE=[%s]", __LINE__, __FILE__, txcode);
	get_zd_data( DC_TX_CODE,vtx_code );
	vtcp_log("%d<> %s FD16->VTX_CODE=[%s]", __LINE__, __FILE__, vtx_code);
	set_zd_data( DC_TX_CODE,txcode );
	vtcp_log("%d<> %s reset FD16 with FD12 TX_CODE=[%s]", __LINE__, __FILE__, txcode);
	get_zd_data(DC_AUTH,auth);

	/*goto OkExit;*/

	ret=Tx_def_Sel( g_pub_tx.reply,&tx_def_v,"tx_code='%s'",txcode );
vtcp_log("%d<> %s --- >TX_CODE=[%s]", __LINE__, __FILE__, txcode);
	if( ret )
	{
		set_zd_data( DC_TX_CODE,vtx_code );
		goto ErrExit;
	}	
	if( tx_def_v.tx_type[0]=='4' )
	{
		sprintf(acErrMsg,"快捷查询不必检查[%s]",txcode);
		WRITEMSG
		set_zd_data( DC_TX_CODE,vtx_code );
		goto OkExit;
	}

	sprintf(acErrMsg,"交易[%s]",txcode);
	WRITEMSG
	pub_comm_chrg_chk();
	set_zd_data( DC_TX_CODE,vtx_code );
	return 0;

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"CHRG 不需要 return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"CHRG CHK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
