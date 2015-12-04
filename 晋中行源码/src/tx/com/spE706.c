/*************************************************
* 文 件 名:  spE706.c
* 功能描述： 重新打印凭
*
* 作    者:  andy
* 完成日期： 2004年7月20日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "trace_log_c.h"

int spE706()
{
	int	ret;
	struct com_tel_c g_com_tel;
	struct trace_log_c trace_log_c;
	long vlsh=0;
	int first=0;
	FILE *fp;
	char filename[81];

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));

	/* 初始化结构 */
	pub_base_sysinit();

	get_zd_long("0440",&vlsh);		

	ret = Trace_log_Dec_Sel( g_pub_tx.reply , "trace_no=%d" ,vlsh  );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	first=0;
	while(1)
	{
		ret = Trace_log_Fet_Sel( &trace_log_c , g_pub_tx.reply );
		if( ret==100 ) break;
		if (ret)
		{
			WRITEMSG
			goto ErrExit;
		}

		if( trace_log_c.sts[0]=='1' )
		{
			strcpy( g_pub_tx.reply,"S079" );
			goto ErrExit;
		}
		if( first==0 )
		{
			sprintf(filename,"%s/log/5706.log",getenv("HOME"));

			fp=fopen(filename,"a");
			if( fp==NULL )
			{
				;
			}
			else
			{
				fprintf(fp,"[%s][%s] [%07d] RE PRINT[%s]\n",
					g_pub_tx.tx_br_no, g_pub_tx.tel, vlsh, trace_log_c.tx_code);
				fclose(fp);
			}
			first=1;
		}
	}
	Trace_log_Clo_Sel();

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
