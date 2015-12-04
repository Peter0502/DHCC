/***************************************************************************
 *** 程序作者 : Shi Shenghui  石升辉                                      **
 *** 日    期 : 2004-4-20                                                  **
 *** 所属模块 :                                                          ***
 *** 程序名称 : efjno.c                                                 ***
 *** 程序作用 : 流水号产生                                               ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
 #include"stdio.h"
 #include"stdlib.h"
 #include"errlog.h"
 #include"alloca.h"
 #include"errno.h"
 #include"ejfno.h"

#include "find_debug.h"
#include"string.h"
#include"unistd.h"
static int ejfno_fd;
static struct flock ejfno_lock;/*记录锁*/




int openEjfnoFile(){
	/*打开ejfno.dat文件，此函数执行在initEjfno之后，所以认为文件已经存在*/
	char pcFileName[128];
	strcpy(pcFileName,getenv("WORKDIR"));
	strcat(pcFileName,"/etc/ejfno.dat");
	ejfno_fd=open(pcFileName,O_RDWR);
	if(ejfno_fd<0 ){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not open [%s],errno=%d",__FILE__,__LINE__,pcFileName,errno);
		return(-1);
	}
	return(0);
}
int closeEjfnoFile(){
	close(ejfno_fd);
	ejfno_fd=-1;
}
int setDate(char *pcDate){
	/*初始化ejfno.dat的日期字段*/
	lseek(ejfno_fd,0,SEEK_SET);
	if(write(ejfno_fd,pcDate,8)<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not write ejfno.dat,Date:%.8s",__FILE__,__LINE__,pcDate);
		return(-1);
	}
	
	return(0);
}
char  *getDate(){
	/*初始化ejfno.dat的日期字段*/
	static char pcDate[9];
	MEMSET_DEBUG(pcDate,'\0',9);
	
	lseek(ejfno_fd,0,SEEK_SET);
	if(read(ejfno_fd,pcDate,8)<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not read ejfno.dat,Date:%.8s",__FILE__,__LINE__,pcDate);
		MEMSET_DEBUG(pcDate,'0',sizeof(pcDate)-1);
		return(pcDate);
	}
	return(pcDate);
}
int lock_area(int nOffset,int nLength){
	/*锁某一个区域*/
	ejfno_lock.l_start=nOffset;
	ejfno_lock.l_len=nLength;
	ejfno_lock.l_type=F_WRLCK;/*独占锁*/
	ejfno_lock.l_whence=0;
	return(fcntl(ejfno_fd,F_SETLKW,&ejfno_lock));
}
int lock_all(){
	return(lock_area(0,0));
}
int unlock(){
	ejfno_lock.l_type=F_UNLCK;
	return(fcntl(ejfno_fd,F_SETLKW,&ejfno_lock));
}
int findTlrno(INT32 nTlrno){
	/*查找指定的操作员记录,找到返回位置，否则返回负数，其绝对值为需插入位置
	//出错，返回0*/
	int nRecCnt;/*记录个数*/
	int nLeft,nRight,nCenter;/*二分查找的左右中位置*/
	int nFLen;/*文件长度*/
	INT32 nFTlrno;
	nFLen=lseek(ejfno_fd,0,SEEK_END);
	if((nFLen-8)%sizeof(TEJFNO)!=0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,File ejfno.dat length error,[%d-8] can not be divied by [%d]",__FILE__,__LINE__,nFLen,sizeof(TEJFNO));
		return(0);
	}
	nRecCnt=(nFLen-8)/sizeof(TEJFNO);
	nLeft=0;nRight=nRecCnt-1;
	if(nRecCnt==0){
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Can not find tlrno:%06d",__FILE__,__LINE__,nTlrno);
		return(-8);
	}
	while(nRight>=nLeft){
		nCenter=(nLeft+nRight)/2;
		lseek(ejfno_fd,8+sizeof(TEJFNO)*nCenter,SEEK_SET);
		read(ejfno_fd,&nFTlrno,sizeof(INT32));
		/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,File:%06d Para %06d,nCenter=%d",__FILE__,__LINE__,nFTlrno,nTlrno,nCenter);
		*/	
		if(nFTlrno==nTlrno){/*找到*/
			/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Find tlrno:%06d at %d",__FILE__,__LINE__,nTlrno,8+sizeof(TEJFNO)*nCenter);
			*/
			return(8+sizeof(TEJFNO)*nCenter);
		}else if(nFTlrno<nTlrno){
			nLeft=nCenter+1;
		}else{
			nRight=nCenter-1;
		}
	}
	/*没有找到*/
	if(nFTlrno<nTlrno){
		/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Can not find tlrno:%06d",__FILE__,__LINE__,nTlrno);
		*/
		return(-8-sizeof(TEJFNO)*(nCenter+1));
	}else{
		/*errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d,Can not find tlrno:%06d",__FILE__,__LINE__,nTlrno);
		*/
		return(-8-sizeof(TEJFNO)*nCenter);
	}
	
}
int insertTlrno(INT32 nTlrno){
	/*插入一条操作员记录
	//insert之前必须对全文件锁定*/
	int nInsertPos;
	int nFLen;
	char *mvBuf;/*移动缓冲区*/
	TEJFNO ejfno;
	nFLen=lseek(ejfno_fd,0,SEEK_END);
	nInsertPos=findTlrno(nTlrno);
	if(nInsertPos>0){
		/*记录已经存在*/
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d, Tlrno %06d exists,Can not insert again",__FILE__,__LINE__,nTlrno);
		return(-1);
	}else if(nInsertPos==0){
		return(-1);/*文件长度有错误*/
	}else{
		/*正常情况，可以插入*/
		initEjfnoRecord(nTlrno,&ejfno);
		nInsertPos=-nInsertPos;
		mvBuf=(char *)alloca(nFLen-nInsertPos);
		errLog(LOG_INFO,0,RPT_TO_TTY,0,0,"%s,%d, Begin moving,nFLen=%d,nInsertPos=%d",__FILE__,__LINE__,nFLen,nInsertPos);
		lseek(ejfno_fd,nInsertPos,SEEK_SET);
		read(ejfno_fd,mvBuf,nFLen-nInsertPos);
		lseek(ejfno_fd,nInsertPos,SEEK_SET);
		write(ejfno_fd,&ejfno,sizeof(TEJFNO));
		write(ejfno_fd,mvBuf,nFLen-nInsertPos);
		return(0);
	}
}
void initEjfnoRecord(INT32 nTlrno,TEJFNO *pEjfno){
	pEjfno->tlrno=nTlrno;
	pEjfno->ejfno_itp=ITP_SNO;
	pEjfno->ejfno_card=CARD_SNO;
	pEjfno->ejfno_agent=AGENT_SNO;
	pEjfno->ejfno_other1=OTHER1_SNO;
	pEjfno->ejfno_other2=OTHER2_SNO;
	pEjfno->itp_change='0';
	pEjfno->card_change='0';
	pEjfno->agent_change='0';
	pEjfno->other1_change='0';
	pEjfno->other2_change='0';

}
int initAllTlrno(){
	/*初始化所有操作员的流水号
	//操作之前要锁定整个文件*/
	int nFLen;
	int nRecCnt;
	int i;
	int nOffset;
	TEJFNO ejfno;
	INT32 nFTlrno;
	nFLen=lseek(ejfno_fd,0,SEEK_END);
	if((nFLen-8)%sizeof(TEJFNO)!=0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,File ejfno.dat length error,[%d-8] can not be divied by [%d]",__FILE__,__LINE__,nFLen,sizeof(TEJFNO));
		return(-1);
	}
	nRecCnt=(nFLen-8)/sizeof(TEJFNO);
	for(i=0;i<nRecCnt;i++){
		nOffset=8+sizeof(TEJFNO)*i;
		lseek(ejfno_fd,nOffset,SEEK_SET);
		read(ejfno_fd,&ejfno,sizeof(TEJFNO));
		initEjfnoRecord(ejfno.tlrno,&ejfno);
		lseek(ejfno_fd,nOffset,SEEK_SET);
		write(ejfno_fd,&ejfno,sizeof(TEJFNO));
	}
	return(0);
}
/*****************************************************
* 以上为支持函数，以下为输出函数			     *****
*****************************************************/
int initEjfnoFile(INT32 nTlrno,char *pcDate){/*初始化流水号文件，系统初始化时执行一次*/
	char pcFileName[128];
	strcpy(pcFileName,getenv("WORKDIR"));
	strcat(pcFileName,"/etc/ejfno.dat");
	ejfno_fd=open(pcFileName,O_RDWR);
	if(ejfno_fd<0 && errno!=ENOENT){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not open ejfno.dat",__FILE__,__LINE__);
		return(-1);
	}
	if(ejfno_fd<0){
		/*若文件不存在，则创建它*/
		ejfno_fd=open(pcFileName,O_CREAT|O_EXCL|O_RDWR);
		if(ejfno_fd<0  ){
			errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Can not create ejfno.dat",__FILE__,__LINE__);
			return(-1);
		}
		/*插入自己并初始化流水号*/
		lock_all();/*锁定整个文件*/
		if(setDate(pcDate)<0){/*插入日期字段*/
			unlock();
			closeEjfnoFile();
			return(-1);
		}
		if(insertTlrno(nTlrno)<0){
			unlock();
			closeEjfnoFile();
			return(-1);
		}
		unlock();
		closeEjfnoFile();
	}else{
		/*
		//流水号文件存在，首先判断日期是否为指定日期，若不是指定日期，需初始化
		//查找有无自己的记录，若有则置初值，如无责插入 
		//若是指定日期，也要查找有无自己的记录
		//因为必须保证下面操作的一致性，所以要首先锁定整个文件
		*/
		char *ptrTmpDate;
		lock_all();
		ptrTmpDate=getDate();
		
		if(memcmp(ptrTmpDate,pcDate,8)!=0 && memcmp(ptrTmpDate,"00000000",8)!=0){
			if(setDate(pcDate)<0){/*插入日期字段*/
				unlock();
				closeEjfnoFile();
				return(-1);
			}
			if(findTlrno(nTlrno)>0){
				if(initAllTlrno()<0){
					unlock();
					closeEjfnoFile();
					return(-1);
				}
			}else{
				if(insertTlrno(nTlrno)<0){
					unlock();
					closeEjfnoFile();
					return(-1);
				}
			}
			unlock();
			closeEjfnoFile();
		}else{
			if(findTlrno(nTlrno)<0){
				if(insertTlrno(nTlrno)<0){
					unlock();
					closeEjfnoFile();
					return(-1);
				}
			}
			unlock();
			closeEjfnoFile();
		}
	}
	return(0);
}
int fetchEjfno(INT32 nTlrno,int ejfType){
	/*取得某种交易的流水号，并且把当前流水号+1
	//如果出错，返回-1
	*/
	int nReadPos;
	int nEjfno;
	TEJFNO ejfno;
	if(openEjfnoFile()<0){
		return(-1);
	}
	nReadPos=findTlrno(nTlrno);
	if(nReadPos<=0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Try to fetch ejfno(%06d,%d),buf find tlrno error",__FILE__,__LINE__,nTlrno,ejfType);
		closeEjfnoFile();
		return(-1);
	}
	lock_area(nReadPos,sizeof(TEJFNO));/*在修改之前首先锁定*/
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	read(ejfno_fd,&ejfno,sizeof(TEJFNO));
	switch(ejfType){
	case EJF_ITP:
		nEjfno=ejfno.ejfno_itp;
		if(ejfno.itp_change=='0')
			ejfno.ejfno_itp++;
		ejfno.itp_change='1';
		break;
	case EJF_CARD:
		nEjfno=ejfno.ejfno_card;
		if(ejfno.card_change=='0')
			ejfno.ejfno_card++;
		ejfno.card_change='1';
		break;
	case EJF_AGENT:
		nEjfno=ejfno.ejfno_agent;
		if(ejfno.agent_change=='0')
			ejfno.ejfno_agent++;
		ejfno.agent_change='1';
		break;
	case EJF_OTHER1:
		nEjfno=ejfno.ejfno_other1;
		if(ejfno.other1_change=='0')
			ejfno.ejfno_other1++;
		ejfno.other1_change='1';
		break;
	case EJF_OTHER2:
		nEjfno=ejfno.ejfno_other2;
		if(ejfno.other2_change=='0')
			ejfno.ejfno_other2++;
		ejfno.other2_change='1';
		break;
	default:
		/*出错*/
		unlock();
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Try to fetch ejfno(%06d,%d>,buf tranaction type error",__FILE__,__LINE__,nTlrno,ejfType);
		closeEjfnoFile();
		return(-1);
	}
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	write(ejfno_fd,&ejfno,sizeof(TEJFNO));
	unlock();/*解除锁定*/
	closeEjfnoFile();
	return(nEjfno);
}
int commitEjfno(INT32 nTlrno){
	/*提交流水号的修改*/
	int nReadPos;
	int nEjfno;
	TEJFNO ejfno;
	if(openEjfnoFile()<0){
		return(-1);
	}
	nReadPos=findTlrno(nTlrno);
	if(nReadPos<=0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Try to commit ejfno(%06d),buf find tlrno error",__FILE__,__LINE__,nTlrno);
		closeEjfnoFile();
		return(-1);
	}
	lock_area(nReadPos,sizeof(TEJFNO));/*在修改之前首先锁定*/
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	read(ejfno_fd,&ejfno,sizeof(TEJFNO));
	
	if(ejfno.itp_change!='0'){
		ejfno.itp_change='0';
	}
	if(ejfno.card_change!='0'){
		ejfno.card_change='0';	
	}
	if(ejfno.agent_change!='0'){
		ejfno.agent_change='0';
	}	
	if(ejfno.other1_change!='0'){
		ejfno.other1_change='0';	
	}
	if(ejfno.other2_change!='0'){
		ejfno.other2_change='0';	
	}
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	write(ejfno_fd,&ejfno,sizeof(TEJFNO));
	unlock();/*解除锁定*/
	closeEjfnoFile();
	return(0);
}
int rollbackEjfno(INT32 nTlrno){
	/*回滚流水号的修改*/
	int nReadPos;
	int nEjfno;
	TEJFNO ejfno;
	if(openEjfnoFile()<0){
		return(-1);
	}
	nReadPos=findTlrno(nTlrno);
	if(nReadPos<=0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,Try to rollback ejfno(%06d),buf find tlrno error",__FILE__,__LINE__,nTlrno);
		closeEjfnoFile();
		return(-1);
	}
	lock_area(nReadPos,sizeof(TEJFNO));/*在修改之前首先锁定*/
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	read(ejfno_fd,&ejfno,sizeof(TEJFNO));
	
	if(ejfno.itp_change!='0'){
		ejfno.itp_change='0';
		ejfno.ejfno_itp--;
	}
	if(ejfno.card_change!='0'){
		ejfno.card_change='0';
		ejfno.ejfno_card--;	
	}
	if(ejfno.agent_change!='0'){
		ejfno.agent_change='0';
		ejfno.ejfno_agent--;
	}	
	if(ejfno.other1_change!='0'){
		ejfno.other1_change='0';
		ejfno.ejfno_other1--;
	}
	if(ejfno.other2_change!='0'){
		ejfno.other2_change='0';	
		ejfno.ejfno_other2--;
	}
	lseek(ejfno_fd,nReadPos,SEEK_SET);
	write(ejfno_fd,&ejfno,sizeof(TEJFNO));
	unlock();/*解除锁定*/
	closeEjfnoFile();
	return(0);
}
/****************************************
*下面是测试                             *
***************************************/
/*main(){
	int no;
	initEjfnoFile(10103,"20040421");
	//exit(0);
	no=fetchEjfno(10103,EJF_ITP);
	printf("no=%06d\n",no);
	no=fetchEjfno(102,EJF_CARD);
	printf("no=%06d\n",no);
	rollbackEjfno(10103);
}*/

		
		
	
	
		
		
		
	
		
