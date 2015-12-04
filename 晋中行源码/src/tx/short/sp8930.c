/*************************************************
* 文 件 名:  sp8930.c
* 功能描述： 9630的反cp 机构号+终端号_tmp 机构号+终端号
*
* 作    者:  martin 
* 完成日期： 2008年8月5日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
  insert into tx_def values('8930','9630反cp','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values('8930','0','8930','#$');
	insert into tx_sub_def values('8930','9630反cp','sp8930','0','0');
*************************************************/
#include <string.h>
#include <errno.h>
#define EXTERN
#include "public.h"

sp8930()
{
	int             ret=0, i = 0, vcnt=0;
	char            cBr_no[6];
	char            cTtyno[5];
	char            tel[5], filename[300];
  char            execstr[256],filename1[300],filename2[300];

	
	/* 清空 */
	memset(cBr_no ,0x0,sizeof(cBr_no));
	memset(cTtyno ,0x0,sizeof(cTtyno));
	memset(filename		, 0x00, sizeof(filename));
	memset(filename1	, 0x00, sizeof(filename1));
	memset(filename2	, 0x00, sizeof(filename2));
	/* 前台取值 */
	pub_base_sysinit();
	
	get_zd_data("0030",cBr_no);
	get_zd_data("0100",cTtyno);
	
	memset( execstr, 0x0, sizeof(execstr));
	sprintf(filename1,"%s/%s%s",getenv("FILDIR"),cBr_no,cTtyno);			/*原文件名  */
	sprintf(filename2,"%s/%s%s_tmp",getenv("FILDIR"),cBr_no,cTtyno);			/*新文件名  */
	sprintf( execstr, "cp %s %s", filename2, filename1);
	ret = system(execstr);
	if ( ret )
	{
		sprintf(acErrMsg,"拷贝文件失败[%s][%s][%d][%s]", filename2, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;		
	}
	
  
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
