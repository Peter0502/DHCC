/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2006-11-15                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : cifserver.c                                              ***
 *** 程序作用 : 接收验印结果的服务端程序                                 ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "public.h"
#include "cifserver.h"
#include "cifpack.h"
#include "errno.h"
#include "errlog.h"
TCIFCONFIG *g_poCfg;
TCIFCONFIG stCifCfg;
void setSignal();
void (*func)()=NULL;
/**********************************************************************
*  int nSysInit();                                                    *
*  功能：初始化本地服务                                               *
*        1.取得系统参数                                               *
*  参数：无                                                           *
*  返回值：0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nSysInit(){
	char pcFileName[256];
	int  iRc; 
	char buf[32];
	/****设置日志****/
	sprintf(pcFileName,"%s/log/cif.log",getenv("WORKDIR"));
	chgLog(LOG_CHG_LOG,pcFileName);
	memset(&stCifCfg,'\0',sizeof(stCifCfg));
	g_poCfg=&stCifCfg;
	sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Address",g_poCfg->cifip);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	memset(buf,'\0',sizeof(buf));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Port",buf);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	g_poCfg->cifport=atoi(buf);
	memset(buf,'\0',sizeof(buf));
	iRc=hv_readIni(pcFileName,"LOCAL","Port",buf);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	g_poCfg->localport=atoi(buf);
	iRc=hv_readIni(pcFileName,"DATABASE","Name",g_poCfg->dbname);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"DATABASE","User",g_poCfg->dbuser);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"DATABASE","Passwd",g_poCfg->dbpasswd);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,读取ini文件错误!\n",__FILE__,__LINE__);
		return(-1);
	}
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"系统初始化完成!");
	return(0);	
}
/**********************************************************************
*  int  nDbConnect();                                                 *
*  功能：连接数据库                                                   *
*  参数：无                                                           *
*  返回值：0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nDbConnect(){
	return(db_open());
}
/**********************************************************************
*  int   nChildWork(int iSocketId)                                    *
*  功能：子进程处理                                                   *
*  参数：数据socket描述符                                             *
*  返回值：0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nChildWork(int iSocketId){
	int iRc;
	int iPackLen;
	TCIFPACK pack_req;
	char pcYzjBuffer[10*1024];	/****银之杰数据缓冲区****/
	int  nYzjLength;
	memset(&g_pub_tx,'\0',sizeof(g_pub_tx));
	memset(pcYzjBuffer,'\0',sizeof(pcYzjBuffer));
	vInitCifPack(&pack_req);
	iRc=Cif_recv(iSocketId,pcYzjBuffer,8);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,接收数据错误[%d]\n",__FILE__,__LINE__,errno);
		return(-1);
	}
	nYzjLength=atoi(pcYzjBuffer);
	iRc=Cif_recv(iSocketId,pcYzjBuffer+8,nYzjLength-8);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,接收报文出错[%d]\n",__FILE__,__LINE__,errno);
		return(-1);
	}
	pcYzjBuffer[nYzjLength]='\0';
	/***BEGIN:20061124 add by wangyongwei 测试用!**/
	char pcFileName[256];
	memset(pcFileName,0,sizeof(pcFileName));
	FILE *fp;
	sprintf(pcFileName,"%s/etc/pack_req.dat",getenv("WORKDIR"));
	if((fp=fopen(pcFileName,"w"))==NULL) {
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"Cannot read file:%s\n",pcFileName);
		return(-1);
	}  
	fwrite(pcYzjBuffer,nYzjLength,1,fp);
	fclose(fp);
	/***del by martin 
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"[%s],[%s],[%d]\n",pcYzjBuffer,__FILE__,__LINE__);
	***/
  
	/****END:20061124 add by wangyongwei 测试用!***/
	/****Add by SSH,2007.5.16,把银之杰格式转换为标准cis格式报文****/
	vYzj2cif(pcYzjBuffer,&pack_req);
	/**************************************************************/
	iRc=nDealCifPack(&pack_req);
	if(iRc){
		/********失败直接在此返回应答*********/
		/********重新计算长度,因为可能被改变**********/
		memcpy(pcYzjBuffer+79,"999",3);
		memcpy(pcYzjBuffer,"00000128",8);
		Cif_send(iSocketId,pcYzjBuffer,128);
		return(-1);
	}else{
		/******重新计算长度,因为可能被改变 ******/
		memcpy(pcYzjBuffer+79,"000",3);
		memcpy(pcYzjBuffer,"00000128",8);
		iRc=Cif_send(iSocketId,pcYzjBuffer,128);
		if(iRc<0){
			return(-1);
		}else{
			return(0);
		}
	} 
	return(0);	
}
int main(){
	int nWaitId,nDataId;
	int nPid=0;
	int iRc=0;
	nWaitId=nDataId=0;
	setSignal();
	if(nSysInit()!=0){
		exit(1);
	}	
	if(nDbConnect()!=0){
		exit(1);
	}	
	nWaitId=Cif_bind("127.0.0.1",g_poCfg->localport);
	printf("行内影像接口开始工作...\n");
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"行内影像接口开始工作...");
	while(1){
		nDataId=Cif_accept(nWaitId);
		if(nDataId<0){
			continue;
		}	
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"接收到连接...");
		/****使用单进程****/
		iRc=nChildWork(nDataId);
		if(iRc){
			rollback_work();
		}else{
			commit_work();
		}
		close(nDataId);
	}
	return(0);
}
void setSignal(){
	signal(SIGINT , SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP , SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGBUS, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}
