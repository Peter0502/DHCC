/*************************************************
* 文 件 名: sp9879.c
* 功能描述：根据产品号查询产品
*
* 作    者: jack
* 完成日期：2003年03月30日
*
* 修改记录： 
* 日    期: 1、20030923
* 修 改 人: 1、jack
* 修改内容: 1、由开活期产品改为活期定期都可以开
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9879()
{
	struct dd_parm_c sDd_parm;	/*活期产品参数表*/
	struct td_parm_c sTd_parm;	/*定期产品参数表*/
	int ret = 0;
	int ret1 = 0;
	char cPrdt_no[4];

	memset(cPrdt_no,0,sizeof(cPrdt_no));

	pub_base_sysinit();
	get_zd_data( "0230", cPrdt_no );

	memset( &sDd_parm, 0x0, sizeof(sDd_parm) );		/*活期产品*/
	ret = Dd_parm_Sel( g_pub_tx.reply , &sDd_parm ,\
			 "prdt_no = '%s'",cPrdt_no);
	if( ret==100 )
	{
		memset( &sTd_parm, 0x0, sizeof(sTd_parm) );	/*定期产品*/
		ret1 = Td_parm_Sel( g_pub_tx.reply , &sTd_parm ,\
				 "prdt_no = '%s'",cPrdt_no);
		if( ret1 == 100 )
		{
			sprintf( acErrMsg, "没有查询到该产品编号对应记录!\
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H014");
			goto	ErrExit;
		}
		else if( ret1 )
		{
			sprintf( acErrMsg, "执行Dd_parm错误!ret=[%d]", ret );
			WRITEMSG
			goto	ErrExit;
		}
        	
		if ( sTd_parm.cif_type[0] != '1' )
		{
			sprintf( acErrMsg, "非个人客户产品不能在此批量开户 \
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H056");
			goto	ErrExit;
		}
        	
		set_zd_data( "0270",sTd_parm.title);
	}
	else if ( ret == 0 )
	{
		if ( sDd_parm.cif_type[0] != '1' )
		{
			sprintf( acErrMsg, "非个人客户产品不能在此批量开户 \
				prdt_no=[%s]", cPrdt_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H056");
			goto	ErrExit;
		}

		set_zd_data( "0270",sDd_parm.title);
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Dd_parm错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	

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
