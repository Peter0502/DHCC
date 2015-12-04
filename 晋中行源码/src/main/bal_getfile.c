#include "find_debug.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <libgen.h>
#include <varargs.h>
#include"signal.h"
/****��һ���򻯵��ļ�get����****/
/****128λ�ļ���+8λ������ͷ,�������99M****/
#define GETFILE_PORT 8888
#define PACK_SIZE 1024
/****�����ײ㺯��****/
int setSignal(){
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGBUS, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

}
int writeLog( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
	va_list ap;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	char	log_time[9];
	long    logsize;
	struct stat lfsts;

	MEMSET_DEBUG(tmpstr,0x00,sizeof(tmpstr));
	MEMSET_DEBUG(tmpname,0x00,sizeof(tmpname));
	MEMSET_DEBUG(log_time,0x00,sizeof(log_time));

  /*  struct timeb Tp; */
	if ( getenv("DEBUG")==NULL || atoi(getenv("DEBUG"))==0 )
	    return ;

	va_start(ap);

	sprintf(tmpstr,"%s/log/getfile.log",getenv("HOME"));
	fp = fopen(tmpstr, "a");
	stat(tmpstr, &lfsts);

	if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
	{
	    logsize=500000;
	}else
	{
	    logsize=atol( getenv( "LOGSIZE" ) );
	}

	if ( lfsts.st_size >= logsize ) 
	{
	    fclose(fp);
		strcpy(tmpname, tmpstr);
		MEMCPY_DEBUG(tmpname+strlen(tmpname)-3, "old", 3);
		umask(0);
		rename(tmpstr, tmpname);
	    fp = fopen(tmpstr, "a");
	}

	if (fp)
	{
	    vfprintf(fp, fmtstr, ap);
	    fprintf(fp, "\n");
	    fclose(fp);
	}

	va_end(ap);


	if ( lfsts.st_size >= logsize ) 
	{
	    sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
	    system(comm_t);
	}

	return 0;
}
int iGetFileLength(char *pcFileName)
{
	struct stat  tFileStat;
	if( -1 == stat( pcFileName, &tFileStat ) )
	{
		writeLog("%d:ERROR(%d): stat - %s", __LINE__, errno, strerror(errno));
		return -1;
	}

	if( ( tFileStat.st_mode & S_IFMT ) != S_IFREG )
	{
		writeLog("%d:ERROR: st_mode = [%lX]", __LINE__, tFileStat.st_mode);
		return -1;
	}

	return(tFileStat.st_size);
}
char *zip_space(char *str){
	int i,j,len;
	i=j=0;
	while(str[i]!='\0'){
		if(str[i]==' '||str[i]== '\t'){
			i++;
		}else{
			str[j]=str[i];
			i++;
			j++;
		}		
	}
	while(j<i){
		str[j]='\0';
		j++;
	}
	return(str);	
}
/******��㺯��****/
int main(int argc ,char *argv[]){
	int i;
	int iPort=GETFILE_PORT;
	int iSocketId;
	int iWaitId;
	int iPid;
	setSignal();
	if(argc>1){
		i=1;
		while(i<argc){
			if(strcmp(argv[i],"-p")==0){
				iPort=atoi(argv[i+1]);
				i+=2;
				continue;
			}
			i++;
		}
	}
	iPid=fork();
	if(iPid>0 || iPid<0){
		exit(0);
	}
	
	iWaitId=Sock_bind("127.0.0.1",iPort);
	if(iWaitId<0){
		writeLog("%s,%d,Bind [%d]error,errno=[%d]\n",__FILE__,__LINE__,iPort,errno);
		printf("�󶨶˿�ʧ��,�ļ�������δ����!\n");	
		return(-1);
	}
	printf("�ļ������������ɹ�,��ʼ�ȴ�����...\n");
	while(1){
		iSocketId=Sock_accept(iWaitId);
		if(iSocketId<0){
			exit(1);
		}
		iPid=fork();
		if(iPid<0){
			writeLog("%s,%d,Fork error\n",__FILE__,__LINE__); 
			exit(1); 
		}else if(iPid==0){
			/***�ӽ���****/
			close(iWaitId);
			iChildWork(iSocketId);
			close(iSocketId);
			exit(0);
		}else{
			close(iSocketId);
			/***������****/
		}
	}	
}
int iChildWork(int iSocketId){
	char pcHead[128+8+1];
	char pcFileName[256+1];
	char pcLength[8+1];
	char pcBuffer[PACK_SIZE+1];
	int  iFileLength;
	int iSendLen;
	int iRc;
	FILE *fp;
	MEMSET_DEBUG(pcHead,'\0',sizeof(pcHead));
	MEMSET_DEBUG(pcFileName,'\0',sizeof(pcFileName));
	MEMSET_DEBUG(pcLength,'\0',sizeof(pcLength));
	MEMSET_DEBUG(pcBuffer,'\0',sizeof(pcBuffer));
	iRc=Sock_recv(iSocketId,pcHead,128+8);
	if(iRc<0){
		close(iSocketId);
		return(-1);
	}
	sprintf(pcFileName,"%s/",getenv("HOME"));
	strncpy(pcFileName+strlen(pcFileName),pcHead,128);
	zip_space(pcFileName);
	writeLog("�õ������ļ�[%s]",pcFileName);
	iFileLength=iGetFileLength(pcFileName);		
	sprintf(pcHead+128,"%08d",iFileLength);/****�����鱨��ͷ***/
	/****�����ļ�****/
	iSendLen=0;
	if(iFileLength==0){
		writeLog("%s,%d,Can not open file:[%s]",__FILE__,__LINE__,pcFileName);	
		close(iSocketId);
		return(-1);
	}
	fp=fopen(pcFileName,"rb");
	if(fp==NULL){
		writeLog("%s,%d,Can not open file:[%s]",__FILE__,__LINE__,pcFileName);	
		close(iSocketId);
		return(-1);
	}
	iRc=Sock_send(iSocketId,pcHead,128+8);
	if(iRc<0){
		close(iSocketId);
		return(-1);
	}
	while(1){
		iSendLen=fread(pcBuffer,1,PACK_SIZE,fp);
		if(iSendLen<=0){
			break;
		}
		iRc=Sock_send(iSocketId,pcBuffer,iSendLen);
		if(iRc<0){
			writeLog("����:[%d]�ֽ�ʧ��,errno=[%d]",iSendLen,errno);
			close(iSocketId);
			return(-1);
		}
		if(iSendLen<PACK_SIZE){
			break;
		}	
	}
	fclose(fp);
	return(0);
}
