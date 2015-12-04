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
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
	rc = TcpRcv(socket,ptpinfo.data, &len, rcvfiles);
	if(rc)
	{
		vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		strcpy(RtCode,"S002");
		bal_return_error(&ptpinfo);
		vtcp_log("SysBegin() errorA %d!!\n",rc);
		goto ERREXIT;
	}
	vtcp_log("[%s][%d] wang-test==[%s]",__FILE__,__LINE__,ptpinfo.data);
	ptpinfo.len=len;
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
	if(bal_open())
	{
		vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		strcpy(RtCode,"S003");
		bal_return_error(&ptpinfo);
		vtcp_log("SysBegin() errorB %d!!\n",rc);
		goto ERREXIT;
	}
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);

	_balmain(&ptpinfo);
	socket=ptpinfo.cd;
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);

	if(bal_close())
	{
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		vtcp_log("zyc aa[%s][%d]",__FILE__,__LINE__);
		strcpy(RtCode,"S004");
		bal_return_error(&ptpinfo);
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		vtcp_log("SysBegin() errorC %d!!\n",rc);
		goto ERREXIT;
	}
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);

/* 系统返回 */
	get_zd_data(DC_FILE_SND,curno);
	vtcp_log("zyc[%s]",curno);
	if(curno[0]=='1' || curno[0]=='2')
	{
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		pub_base_GenDwnFilName(sndfiles);
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		vtcp_log("zyc2[%s]",sndfiles);
	}
	else
	{
	vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		strcpy(sndfiles,"");
	}
ERREXIT:
	rc = TcpSnd(socket, ptpinfo.data, ptpinfo.len, sndfiles);
	if(rc)
	{
		vtcp_log("[%s][%d] cl_addr==[%s]",__FILE__,__LINE__,cl_addr);
		vtcp_log("ERROR SysEnd() error %d!!\n",rc);
		return rc;
	}

	return 0;
}


