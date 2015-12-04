/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2006-11-15                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : cifserver.c                                              ***
 *** �������� : ������ӡ����ķ���˳���                                 ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
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
*  ���ܣ���ʼ�����ط���                                               *
*        1.ȡ��ϵͳ����                                               *
*  ��������                                                           *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nSysInit(){
	char pcFileName[256];
	int  iRc; 
	char buf[32];
	/****������־****/
	sprintf(pcFileName,"%s/log/cif.log",getenv("WORKDIR"));
	chgLog(LOG_CHG_LOG,pcFileName);
	memset(&stCifCfg,'\0',sizeof(stCifCfg));
	g_poCfg=&stCifCfg;
	sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Address",g_poCfg->cifip);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	memset(buf,'\0',sizeof(buf));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Port",buf);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	g_poCfg->cifport=atoi(buf);
	memset(buf,'\0',sizeof(buf));
	iRc=hv_readIni(pcFileName,"LOCAL","Port",buf);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	g_poCfg->localport=atoi(buf);
	iRc=hv_readIni(pcFileName,"DATABASE","Name",g_poCfg->dbname);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"DATABASE","User",g_poCfg->dbuser);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	iRc=hv_readIni(pcFileName,"DATABASE","Passwd",g_poCfg->dbpasswd);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,��ȡini�ļ�����!\n",__FILE__,__LINE__);
		return(-1);
	}
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"ϵͳ��ʼ�����!");
	return(0);	
}
/**********************************************************************
*  int  nDbConnect();                                                 *
*  ���ܣ��������ݿ�                                                   *
*  ��������                                                           *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nDbConnect(){
	return(db_open());
}
/**********************************************************************
*  int   nChildWork(int iSocketId)                                    *
*  ���ܣ��ӽ��̴���                                                   *
*  ����������socket������                                             *
*  ����ֵ��0/-1                                                       *
*                                                                     *
**********************************************************************/         
int nChildWork(int iSocketId){
	int iRc;
	int iPackLen;
	TCIFPACK pack_req;
	char pcYzjBuffer[10*1024];	/****��֮�����ݻ�����****/
	int  nYzjLength;
	memset(&g_pub_tx,'\0',sizeof(g_pub_tx));
	memset(pcYzjBuffer,'\0',sizeof(pcYzjBuffer));
	vInitCifPack(&pack_req);
	iRc=Cif_recv(iSocketId,pcYzjBuffer,8);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,�������ݴ���[%d]\n",__FILE__,__LINE__,errno);
		return(-1);
	}
	nYzjLength=atoi(pcYzjBuffer);
	iRc=Cif_recv(iSocketId,pcYzjBuffer+8,nYzjLength-8);
	if(iRc){
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"%s,%d,���ձ��ĳ���[%d]\n",__FILE__,__LINE__,errno);
		return(-1);
	}
	pcYzjBuffer[nYzjLength]='\0';
	/***BEGIN:20061124 add by wangyongwei ������!**/
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
  
	/****END:20061124 add by wangyongwei ������!***/
	/****Add by SSH,2007.5.16,����֮�ܸ�ʽת��Ϊ��׼cis��ʽ����****/
	vYzj2cif(pcYzjBuffer,&pack_req);
	/**************************************************************/
	iRc=nDealCifPack(&pack_req);
	if(iRc){
		/********ʧ��ֱ���ڴ˷���Ӧ��*********/
		/********���¼��㳤��,��Ϊ���ܱ��ı�**********/
		memcpy(pcYzjBuffer+79,"999",3);
		memcpy(pcYzjBuffer,"00000128",8);
		Cif_send(iSocketId,pcYzjBuffer,128);
		return(-1);
	}else{
		/******���¼��㳤��,��Ϊ���ܱ��ı� ******/
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
	printf("����Ӱ��ӿڿ�ʼ����...\n");
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"����Ӱ��ӿڿ�ʼ����...");
	while(1){
		nDataId=Cif_accept(nWaitId);
		if(nDataId<0){
			continue;
		}	
		errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"���յ�����...");
		/****ʹ�õ�����****/
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
