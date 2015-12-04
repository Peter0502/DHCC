/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2008-09-09                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : mobmqlib.c                                               ***
 *** �������� : ����ʵʱ���ͳ��������Ϣ���к�����                       ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
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
#define MOB_MAX_LEN 128
#define IPC_QU_THRESHOLD   80
struct st_MSGBUF {
    long mtype;
    char data[MOB_MAX_LEN - sizeof(long)];
};

static int nMobMqId=-1;
/**************************************************************************
������:nMobMqOpen
����  :�����������Ӷ�����Ϣ����
����  :
����ֵ:
      -1         ʧ��
      0          �ɹ� 
**************************************************************************/ 
int nMobMqOpen()
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
            vtcp_log("������Ϣ����[%d]ʧ��,������[%d]",nMqKey,errno);
            return(-1);
        }
        vtcp_log(" ��Ϣ����[%d]������,��Ҫ���� ",nMqKey );
    }else{
        vtcp_log(" ��Ϣ����[%d]�Ѿ����ڣ�������Ϣ���гɹ�! ",nMqKey );
        nMobMqId=nMqId;
        return(0);
    }
    /****��Ϣ���в�����,��Ҫ����****/
    nMqId = msgget((key_t)nMqKey, IPC_CREAT|0666);
    if (nMqId < 0 ) {
        vtcp_log("������Ϣ����[%d]ʧ��,������[%d] nMqId==[%d]",nMqKey,errno,nMqId);
        return(-1);
    }
    vtcp_log("������Ϣ����[%d][0x%x]�ɹ�",nMqKey,nMqKey);
    nMobMqId=nMqId;
    return(0);     
}
/**************************************************************************
������:nMobMqInit
����  :��ʼ��(���)��Ϣ����       
����  :
       ��
����ֵ:
      0          ����
      -1         ʧ��
**************************************************************************/ 
int nMobMqInit(){
    int  iRc;
    struct st_MSGBUF stMsgQuData;
    /* ��ʼ��(���)��Ϣ���� */    
    memset (&stMsgQuData,'\0', sizeof(stMsgQuData));
    while ((iRc = msgrcv(nMobMqId, &stMsgQuData, MOB_MAX_LEN,0,IPC_NOWAIT))>0)
    {
        vtcp_log("[%s][%d] ",__FILE__,__LINE__);
    }
        
    vtcp_log("%s,%d,��ʼ��(���)��Ϣ����[%d]! ",__FILE__,__LINE__,errno);
    return(0);
}
/**************************************************************************
������:nMobMqCheckSize
����  :�����Ϣ���пռ��Ƿ�д�뱾��Ϣ
����  :
       nLength   д�볤��   
����ֵ:
      0          �ռ��㹻
      -1         �ռ䲻��
**************************************************************************/ 
int nMobMqCheckSize(int nLength){
    int iRc;
    int iWaterLev;
    struct msqid_ds stMsgQuDs;
    int nMqId=nMobMqId;
    iRc = msgctl(nMqId, IPC_STAT, &stMsgQuDs);
    if (iRc < 0) 
    {
        vtcp_log("%s,%d, iRc==[%d]������! errno==[%d]",__FILE__,__LINE__, iRc, errno);
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
������:nMobMqPut
����  :����Ϣд�������Ϣ����
����  :
       pcBuffer  ��Ϣ������
       nLength   д�볤��   
       nMtype    ��Ϣ��mtype(�ݹ̶�Ϊ1)
����ֵ:
      0          ����
      -1         ʧ��
**************************************************************************/ 
int nMobMqPut(char *pcBuffer,int nLength,int nMtype){
    int iRc;
    struct st_MSGBUF st;
    int nMqId;    
    nMqId = nMobMqId;
    if(nMqId<0)
    {
         vtcp_log("%s,%d,����Ϣ���� ����! ",__FILE__,__LINE__);
         return -1;    
    }
    
    /* ����Ϣд����Ϣ���� */
    if(nLength>sizeof(st.data)){
        vtcp_log("%s,%d,������Ϣ����[%d]����!",__FILE__,__LINE__,nLength);
        return(-1);
    }
    if(nMobMqCheckSize(nLength)<0){
        vtcp_log("%s,%d,��Ϣ����[%d]�ռ䲻��!",__FILE__,__LINE__,nMqId);
        return(-1);
    }
    st.mtype=nMtype;
    memcpy(st.data, pcBuffer, nLength);    
    iRc=msgsnd(nMqId, (char *)(&st), nLength, 0);
    if(iRc){
        vtcp_log("%s,%d,д����Ϣ���д���,������[%d]",__FILE__,__LINE__,errno);
        return(-1);
    }
    vtcp_log("%s,%d,д����Ϣ����[%d]�ɹ� ",__FILE__,__LINE__, nMqId);
    return(0);
}
/**************************************************************************
������:nMobMqGet
����  :����Ϣ���ж�����(������)
����  :
       pcBuffer  ��Ϣ������
       pnLength  ��Ϣ���ݳ���ָ�� 
       nMtype    ��Ϣ��mtype
����ֵ:
      1          ������
      0          ������
      -1         ʧ��
**************************************************************************/
int nMobMqGet(char *pcBuffer,int *pnLength, int nMtype){
    int iRc;
    struct st_MSGBUF st;
    int nMqId=nMobMqId;    
    if(nMqId<0)
    {
         vtcp_log("%s,%d,����Ϣ���� ����! ",__FILE__,__LINE__);
         return -1;    
    }

    /****����Ϣ���ж�����(������)****/
    iRc=msgrcv(nMqId,&st,MOB_MAX_LEN, nMtype, IPC_NOWAIT);
    if(iRc==-1){
        if(errno==ENOMSG||errno==EINTR){
            return(1);
        }else{
            vtcp_log("%s,%d,����Ϣ����[%d]����,errno=%d",__FILE__,__LINE__,nMqId,errno);
            return(-1);
        }
    }
    memcpy(pcBuffer,st.data,iRc);
    *pnLength=iRc;
    vtcp_log("%s,%d,  ����Ϣ�����ж�ȡ�ɹ� mtype===[%d] length==[%d] ",__FILE__, __LINE__, st.mtype, iRc);        
    
    return(0);
}

