/**********************************************************************
    程序名:tradsvr.ec
-----------------------------------------------------------------------
    本程序为系统进入退出程序。
-----------------------------------------------------------------------
    编制人：zyc                             编制时间：
**********************************************************************/
# include "bal.h"
# include "tcpsvr.h"
#include "pubf_data_code.h"
extern char		cl_addr[16];

cstx_main(int socket,char *addr)
{
    int			rc,i;
	long		date,date1;
	char		rcvfiles[100];
	char		sndfiles[100];
	char		fna[40];
	char		curno[2];
	char		tty[4];
	int			len;
	int			wlen;
	TPSVCINFO	ptpinfo;

	/* 系统开始 */
	pub_base_strpack( addr );
	strncpy( cl_addr , addr ,15); 
	cl_addr[strlen(cl_addr)]='\0';
	ptpinfo.cd=socket;
	ptpinfo.len=len;

	if(bal_open())
	{
		strcpy(ptpinfo.data,"S003");
		printf("SysBegin() errorB %d!!\n",rc);
		goto ERREXIT;
	}

	_balmain(&ptpinfo);
	socket=ptpinfo.cd;

	if(bal_close())
	{
		vtcp_log("zyc aa[%s][%d]",__FILE__,__LINE__);
		strcpy(ptpinfo.data,"S004");
		printf("SysBegin() errorC %d!!\n",rc);
		goto ERREXIT;
	}

ERREXIT:
    return 0;
}
