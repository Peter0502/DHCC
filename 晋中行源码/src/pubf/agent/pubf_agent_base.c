/***************************************************************
* 文 件 名: pubf_agent_base.c
* 功能描述: 代理业务公用函数
* 入口参数：
* 作    者: jack
* 完成日期： 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"
#include "pos_mobile_pay_c.h"

/***************************************************************
* 函 数 名: pubf_agent_perno
* 功能描述: 生成个人编号
* 入口参数：---------------联行号暂用1234567代替
* 作    者: jack
* 完成日期：2003年1月26日 
***************************************************************/
pub_agent_perno(char *Per_no)
{
	int ret = 0;
	int iPer_no = 0;
	char Per_tmp[6];

	memset(Per_tmp,0,sizeof(Per_tmp));

	ret = sql_count("ag_peracctinfo","1=1");
	if (ret == 0)
	{
		strcpy(Per_no,"123456700001");
	}
	if (ret > 0)
	{
		ret = 0;
		ret = sql_max_string( "ag_peracctinfo","per_no", Per_no, 12,"1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg,"取最大个人编号错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H002");
			goto ErrExit;
		}
		memcpy(Per_tmp,Per_no+7,5);
		iPer_no = atoi(Per_tmp);
		iPer_no = iPer_no + 1;

		memset(Per_tmp,0,sizeof(Per_tmp));
		memset(Per_no,0x0,sizeof(Per_no));
		sprintf(Per_tmp,"%05d",iPer_no);
		strcpy(Per_no,"1234567");
		strcat(Per_no,Per_tmp);

	sprintf(acErrMsg,"转化后的个人编号为![%s]",Per_no);
	WRITEMSG

	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成个人编号成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"生成个人编号失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

/***************************************************************
* 函 数 名: pubf_agent_unitno
* 功能描述: 生成单位编号
* 入口参数：---------------
* 作    者: jack
* 完成日期：2003年1月30日 
***************************************************************/
pub_agent_unitno(char *Unit_no)
{
	int ret = 0;
	int iUnit_no = 0;

	ret = sql_count("ag_unitinfo","1=1");
	if (ret == 0)
	{
		strcpy(Unit_no,"1001");
	}
	if (ret > 0)
	{
		ret = 0;
		ret = sql_max_string( "ag_unitinfo","unit_no", Unit_no, 4,"1=1");
		if (ret != 0)
		{
			sprintf(acErrMsg,"取最大单位编号错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H002");
			goto ErrExit;
		}
		iUnit_no = atoi(Unit_no);
		iUnit_no = iUnit_no + 1;

		memset(Unit_no,0x0,sizeof(Unit_no));
		sprintf(Unit_no,"%4d",iUnit_no);

	sprintf(acErrMsg,"转化后的单位编号为![%s]",Unit_no);
	WRITEMSG

	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成单位编号成功![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"生成单位编号失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}


/***************************************************************
* 函 数 名: pubf_mobile_pay()
* 功能描述: 判断是否可以移动支付
* 入口参数：
* 作    者: jack
* 完成日期：2003年09月02日 
***************************************************************/
int pub_mobile_pay( char *cIn_type, char *cTtyn, char *cMarch_no )
{
	int ret = 0;
	char cTrcode[5];

	struct pos_mobile_pay_c sPos_mobile_pay;

	memset( cTrcode, 0x0, sizeof(cTrcode) );
	get_zd_data( "0160", cTrcode );

	sprintf(acErrMsg, "------tty_no=[%s], march_no=[%s]", cMarch_no, cTtyn);
	WRITEMSG

	if ( strncmp( cIn_type, "012", 3 ) == 0 && \
		( strncmp( cTrcode, "6301", 4 ) == 0 || \
		strncmp( cTrcode, "6314", 4 ) == 0 ) ) 
	{
		ret = Pos_mobile_pay_Sel( g_pub_tx.reply, &sPos_mobile_pay, \
			"march_no = '%s' and tty_no = '%s'", \
			cMarch_no, cTtyn );
		if ( ret == 0 )
		{
			goto OkExit;
		}
		else
		{
			goto NoOkExit;
		}
	}
	else
	{
		goto NoOkExit;
	}

OkExit:
	sprintf( acErrMsg, "可以移动支付!" );
	WRITEMSG
	return 0;
NoOkExit:
	sprintf( acErrMsg, "不能移动支付!" );
	WRITEMSG
	return 1;
}
