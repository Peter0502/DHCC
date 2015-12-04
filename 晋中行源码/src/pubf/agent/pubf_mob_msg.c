/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2008-09-09                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : mobmqlib.c                                               ***
 *** 程序作用 : 短信实时发送程序相关消息队列函数库                       ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <varargs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sys/shm.h"
#include "sys/msg.h"
#include "sys/types.h"
#include"pub_tx.h"

#define MOB_MAX_LEN 128
#define IPC_QU_THRESHOLD   80
struct st_MSGBUF {
    long mtype;
    char data[MOB_MAX_LEN - sizeof(long)];
};
extern struct pub_tx g_pub_tx;              /*通用记帐结构*/

static int nMobMqId=-1;
/**************************************************************************
函数名:nMobMqOpen
功能  :创建或者连接短信消息队列
参数  :
返回值:
      -1         失败
      0          成功 
**************************************************************************/ 
int nMobMqOpenbatch()
{
    int iRc;
    int nMqKey;
    struct msqid_ds stMsgQuDs;
    int nMqId;    
    if(getenv("MOBKEY")!=NULL){
        nMqKey=atoi(getenv("MOBKEY"));
    }else{
        nMqKey=0x20080;
    }
    nMqId = msgget((key_t)nMqKey, 0);
    if(nMqId<0 ){
        if(errno!=ENOENT){
            vtcp_log("连接消息队列[%d]失败,错误码[%d]",nMqKey,errno);
            return(-1);
        }
        vtcp_log(" 消息队列[%d]不存在,需要创建 ",nMqKey );
    }else{
        vtcp_log(" 消息队列[%d]已经存在，连接消息队列成功! ",nMqKey );
        nMobMqId=nMqId;
        return(0);
    }
    /****消息队列不存在,需要创建****/
    nMqId = msgget((key_t)nMqKey, IPC_CREAT|0666);
    if (nMqId < 0 ) {
        vtcp_log("创建消息队列[%d]失败,错误码[%d] nMqId==[%d]",nMqKey,errno,nMqId);
        return(-1);
    }
    vtcp_log("创建消息队列[%d][0x%x]成功",nMqKey,nMqKey);
    nMobMqId=nMqId;
    return(0);     
}
/**************************************************************************
函数名:nMobMqInit
功能  :初始化(清空)消息队列       
参数  :
       无
返回值:
      0          正常
      -1         失败
**************************************************************************/ 
int nMobMqInitbatch(){
    int  iRc;
    struct st_MSGBUF stMsgQuData;
    /* 初始化(清空)消息队列 */    
    memset (&stMsgQuData,'\0', sizeof(stMsgQuData));
    while ((iRc = msgrcv(nMobMqId, &stMsgQuData, MOB_MAX_LEN,0,IPC_NOWAIT))>0)
    {
        vtcp_log("[%s][%d] ",__FILE__,__LINE__);
    }
        
    vtcp_log("%s,%d,初始化(清空)消息队列[%d]! ",__FILE__,__LINE__,errno);
    return(0);
}
/**************************************************************************
函数名:nMobMqCheckSize
功能  :检查消息队列空间是否够写入本消息
参数  :
       nLength   写入长度   
返回值:
      0          空间足够
      -1         空间不够
**************************************************************************/ 
int nMobMqCheckSizebatch(int nLength){
    int iRc;
    int iWaterLev;
    struct msqid_ds stMsgQuDs;
    int nMqId=nMobMqId;
    iRc = msgctl(nMqId, IPC_STAT, &stMsgQuDs);
    if (iRc < 0) 
    {
        vtcp_log("%s,%d, iRc==[%d]，错误! errno==[%d]",__FILE__,__LINE__, iRc, errno);
        return(-1);
    }
    iWaterLev=((((stMsgQuDs.msg_cbytes+nLength)*1000)/stMsgQuDs.msg_qbytes+5)/10);
    if (iWaterLev >= IPC_QU_THRESHOLD) {
        vtcp_log("[%s][%d] ",__FILE__,__LINE__);
        return(-1);
    } else {
        vtcp_log("[%s][%d] ",__FILE__,__LINE__);
        return(0);
    }
    return(0);
}
/**************************************************************************
函数名:nMobMqPut
功能  :将消息写入短信消息队列
参数  :
       pcBuffer  消息缓冲区
       nLength   写入长度   
       nMtype    消息的mtype(暂固定为1)
返回值:
      0          正常
      -1         失败
**************************************************************************/ 
int nMobMqPutbatch(char *pcBuffer,int nLength,int nMtype){
    int iRc;
    struct st_MSGBUF st;
    int nMqId;    
    nMqId = nMobMqId;
    if(nMqId<0)
    {
         vtcp_log("%s,%d,打开消息队列 错误! ",__FILE__,__LINE__);
         return -1;    
    }
    
    /* 将消息写入消息队列 */
    if(nLength>sizeof(st.data)){
        vtcp_log("%s,%d,发送消息长度[%d]超长!",__FILE__,__LINE__,nLength);
        return(-1);
    }
    if(nMobMqCheckSizebatch(nLength)<0){
        vtcp_log("%s,%d,消息队列[%d]空间不够!",__FILE__,__LINE__,nMqId);
        return(-1);
    }
    st.mtype=nMtype;
    memcpy(st.data, pcBuffer, nLength);    
    iRc=msgsnd(nMqId, (char *)(&st), nLength, 0);
    if(iRc){
        vtcp_log("%s,%d,写入消息队列错误,错误码[%d]",__FILE__,__LINE__,errno);
        return(-1);
    }
    vtcp_log("%s,%d,写入消息队列[%d]成功 ",__FILE__,__LINE__, nMqId);
    return(0);
}
/**************************************************************************
函数名:nMobMqGet
功能  :从消息队列读数据(非阻塞)
参数  :
       pcBuffer  消息缓冲区
       pnLength  消息数据长度指针 
       nMtype    消息的mtype
返回值:
      1          无数据
      0          有数据
      -1         失败
**************************************************************************/
int nMobMqGetbatch(char *pcBuffer,int *pnLength, int nMtype){
    int iRc;
    struct st_MSGBUF st;
    int nMqId=nMobMqId;    
    if(nMqId<0)
    {
         vtcp_log("%s,%d,打开消息队列 错误! ",__FILE__,__LINE__);
         return -1;    
    }

    /****从消息队列读数据(非阻塞)****/
    iRc=msgrcv(nMqId,&st,MOB_MAX_LEN, nMtype, IPC_NOWAIT);
    if(iRc==-1){
        if(errno==ENOMSG||errno==EINTR){
            /**TODO:测试没有问题后把这条日志语句删除**
            vtcp_log("%s,%d,读消息队列[%d]无数据",__FILE__,__LINE__,nMqId);            
            */
            return(1);
        }else{
            vtcp_log("%s,%d,读消息队列[%d]错误,errno=%d",__FILE__,__LINE__,nMqId,errno);
            return(-1);
        }
    }
    memcpy(pcBuffer,st.data,iRc);
    *pnLength=iRc;
    vtcp_log("%s,%d,  从消息队列中读取成功 mtype===[%d] length==[%d] ",__FILE__, __LINE__, st.mtype, iRc);        
    
    return(0);
}
int mobputmsg()
{
		/****Add by SSH,20080909,通知mobsend发送短消息****/
	vtcp_log("取到流水号:[%d]\n",g_pub_tx.trace_no);
		char buf[128];
		/****TODO:
			首先测试每次都连接消息队列,这样队列出错时不用重新启动服务
		   如果效率受到明显影响则应在初始化时连接
		****/
		memset(buf,'\0',sizeof(buf));
		sprintf(buf,"%d|%d",g_pub_tx.tx_date,g_pub_tx.trace_no);
		if(nMobMqOpenbatch()<0){
			vtcp_log("连接短信队列失败,但程序继续![%d]\n",errno);
			return(0);
		}	
		vtcp_log("连接短信队列open,程序继续!\n");
		if(nMobMqPutbatch(buf,strlen(buf),1)<0){
			vtcp_log("发送数据到短信队列失败,但程序继续![%d]\n",errno);
			return(0);
		}
		vtcp_log("发送数据到短信队列成功\n");
		
	return(0);
}