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

tx_main(int socket,char *addr)
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
	rc = TcpRcv(socket,ptpinfo.data, &len, rcvfiles);
	if(rc)
	{
		strcpy(RtCode,"S002");
		bal_return_error(&ptpinfo);
		vtcp_log("SysBegin() errorA %d!!\n",rc);
		goto ERREXIT;
	}
	ptpinfo.len=len;
	if(bal_open())
	{
		strcpy(RtCode,"S003");
		bal_return_error(&ptpinfo);
		vtcp_log("SysBegin() errorB %d!!\n",rc);
		goto ERREXIT;
	}

	_balmain(&ptpinfo);
	socket=ptpinfo.cd;

	if(bal_close())
	{
		vtcp_log("zyc aa[%s][%d]",__FILE__,__LINE__);
		strcpy(RtCode,"S004");
		bal_return_error(&ptpinfo);
		vtcp_log("SysBegin() errorC %d!!\n",rc);
		goto ERREXIT;
	}

/* 系统返回 */
	get_zd_data(DC_FILE_SND,curno);
	vtcp_log("zyc[%s]",curno);
	if(curno[0]=='1' || curno[0]=='2')
	{
		pub_base_GenDwnFilName(sndfiles);
		vtcp_log("zyc2[%s]",sndfiles);
	}
	else
	{
		strcpy(sndfiles,"");
	}
ERREXIT:
	rc = TcpSnd(socket, ptpinfo.data, ptpinfo.len, sndfiles);
	if(rc)
	{
		vtcp_log("ERROR SysEnd() error %d!!\n",rc);
		return rc;
	}

	return 0;
}


