/***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                      ***
 *** 日    期 : 2008-09-09                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : mobdata.c                                                ***
 *** 程序作用 : 短信实时发送程序组包函数库                               ***
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
#include <sys/time.h>
#include <unistd.h>
#include <iconv.h>
#include "mob_sendmail_c.h"
#include "mobile_time_c.h"
#include "com_sys_parm_c.h"
#define  SEND_TYPE_NOW        "00"        /****发送类型:及时发送****/
#define  CHANNEL_TYPE_AC    "CB"        /****渠道:核心****/
#define  FEE_TYPE_CZ        "00"        /****计费类型:免费****/
#define  INTERACTION_CZ        "D"            /****交互类型:下行短信****/
#define  SERVICE_TYPE_COMM    "00"        /****业务类型:通用****/
#define TRACE vtcp_log("%s,%d,Gethere\n",__FILE__,__LINE__);
/**#define  ACCOUNT	"wuzihaoma" **/
#define  ACCOUNT1	"test-dhcc1"
#define  ACCOUNT2	"test-dhcc2"
#define  PASSWORD  "123.com"
extern int Sock_connect(char *addr,int port);
int md5(char*desc,char*md);
extern char * pub_base_daynumLAT(char *date, int num);
int changeTime(long date,long time,char *standTime);
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t *inlen,char *outbuf,size_t *outlen);
/*************************************************************************
函数名:nMobSendRecord
功能  :发送一条指定记录到短信平台
参数  :
       addr      平台地址
       port      平台端口
       st        mob_sendmail记录
返回值:
      -1         失败
      0          成功 
**************************************************************************/ 
int nMobSendRecord(char *addr,int port,struct mob_sendmail_c *st){
    int iRc;
    int len=0;
    char sReply[33];
    char res[1024*10];
    char req[1024*10];
    char body[1024];
    char acno[24];
    char tmp[10000];
    char passwd[100];
    char standTime[40];
    char errcode[5];
    char tmpmsg[10000];
    char des[2000];
    char tmpdes[100];
    int nSocketId;
    char *p=NULL;
    char *Des=NULL;
    int i=0;
    long date;
    char *pDate;
    char sDate[8];
		struct com_sys_parm_c sCom_sys_parm;
		struct mobile_time_c sMobile_time;
    size_t inlen,outlen;
    memset(tmpdes,0x00,sizeof(tmpdes));
    memset(des,0x00,sizeof(des));
		memset(req,0x00,sizeof(req));
		memset(res,0x00,sizeof(res));
		memset(body,0x00,sizeof(body));
		memset(acno,0x00,sizeof(acno));
		memset(tmp,0x00,sizeof(tmp));
		memset(passwd,0x00,sizeof(passwd));
		memset(errcode,0x00,sizeof(errcode));
		memset(tmpmsg,0x00,sizeof(tmpmsg));
    /****组包****/
		sprintf(acno,"%s",st->ac_no);
		if ( acno[0] == '5' )
		{
			sprintf(body,"account=%s",ACCOUNT2);
		}
		else
		{
			sprintf(body,"account=%s",ACCOUNT1);
		}
    strcpy(tmp,PASSWORD);
    md5(tmp,passwd);
    memset(tmp,0x00,sizeof(tmp));
    memset(&sCom_sys_parm,0x0,sizeof(sCom_sys_parm));
    memset(&sMobile_time,0x0,sizeof(sMobile_time));
    memset(sReply,0x00,sizeof(sReply));
		if ( acno[0] == '5' )
		{
			sprintf(tmp,"&password=%s&smsType=4&message=",passwd);
		}
		else
		{
			sprintf(tmp,"&password=%s&smsType=4&message=",passwd);
		}
    strcat(body,tmp);
    /*memset(tmp,0x00,sizeof(tmp));
    sprintf(tmp,"&%s",st->mobile);
    strcat(body,tmp);
    memset(tmp,0x00,sizeof(tmp));
    sprintf(tmp,"&%s",st->message);*/
    if(strlen(st->mobile) != 11)
    {
    	vtcp_log("[%s][%d]手机号码长度错误![%d]\n",__FILE__,__LINE__,strlen(st->mobile));
    	return(-1);
    }
    inlen=strlen(st->message);
    outlen=inlen*2;
    memset(tmpmsg,0x00,outlen+1);
    code_convert("GBK","UTF-8",st->message,&inlen,tmpmsg,&outlen);
    
  	if(strcmp(st->timeflag,"1") == 0||strcmp(st->tel,"9999") == 0)
  	{
  		memset(standTime,0x00,sizeof(standTime));
  		iRc= Com_sys_parm_Sel(sReply,&sCom_sys_parm,"1=1");
			if(iRc)
			{
				vtcp_log("[%s][%d]得到系统日期错![%d]\n",__FILE__,__LINE__,iRc);
				return(-1);
			}
			vtcp_log("[%s][%d]得到的系统日期！！！！！！！！！![%ld]\n",__FILE__,__LINE__,sCom_sys_parm.sys_date);
			sprintf(sDate,"%ld",sCom_sys_parm.sys_date);
			/*memcpy(standTime,pub_base_daynumLAT(sDate,1),sizeof(standTime));*/
			iRc = Mobile_time_Sel(sReply,&sMobile_time,"1=1");
			if(iRc)
			{
				vtcp_log("[%s][%d]得到短信发送时间错![%d]\n",__FILE__,__LINE__,iRc);
				return(-1);
			}
  		/*strcat(standTime,sMobile_time.time);*/
  		changeTime(pub_base_daynumLAT(sDate,1),atol(sMobile_time.time),standTime);
  		vtcp_log("[%s][%d]传出的系统日期！！！！！！！！![%s]\n",__FILE__,__LINE__,standTime);
    	memset(tmp,0x00,sizeof(tmp));
    	sprintf(tmp,"<com.ctc.ema.server.jwsserver.sms.MtMessage><content>%s</content><phoneNumber>%s</phoneNumber><sendTime>%s</sendTime></com.ctc.ema.server.jwsserver.sms.MtMessage>",tmpmsg,st->mobile,standTime);
 	  }
 	  else
   	{
    	memset(tmp,0x00,sizeof(tmp));
    	sprintf(tmp,"<com.ctc.ema.server.jwsserver.sms.MtMessage><content>%s</content><phoneNumber>%s</phoneNumber></com.ctc.ema.server.jwsserver.sms.MtMessage>",tmpmsg,st->mobile);
  	}
  	
    strcat(body,tmp);
    len=strlen(body);
    body[len]='\0';
    sprintf(req,"POST /ema/httpSms/sendSms HTTP/1.1\r\nHost: 172.168.99.124:8080\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n%s",len,body);
    nMobMsgDebug(req);
    /****发送****/
    nSocketId=Sock_connect(addr,port);
    if(nSocketId<0){
        vtcp_log("%s,%d,连接短信平台失败![%d]\n",__FILE__,__LINE__,errno);
        return(-1);
    }    
    iRc=Sock_send(nSocketId,req,strlen(req));
    if(iRc){
        vtcp_log("%s,%d,发送数据到短信平台失败![%d]\n",__FILE__,__LINE__,errno);
        Sock_close(nSocketId);
        return(-1);
    }
    
    iRc=Sock_recv(nSocketId,res,1000);
    if(iRc){    	
        vtcp_log("%s,%d,接收数据从短信平台失败![%d]\n",__FILE__,__LINE__,errno);
        Sock_close(nSocketId);
        return(-1);
    }
    Sock_close(nSocketId);
    nMobMsgDebug(res);
    p=strtok(res,"<");
	  while((p=strtok(NULL,"<"))!=NULL)
		{
			/* memset(des,0x00,sizeof(des));
			if(memcmp("statDes",p,7)==0)
				{
				strcpy(des,p+8);
				Des=strtok(des,"<");
				strcpy(tmpdes,Des);
			}*/
			if(memcmp("subStat",p,7)==0)
				{
				memcpy(errcode,p+10,3);
			}
			memset(des,0x00,sizeof(des));
			p=strtok(NULL,"<");
			p=strtok(NULL,"<");
			Des=p;
			Des=strtok(NULL,"<");
			if(memcmp("subStatDes",p,10)==0)
			{
				p+=11;
				while(p!=Des) 
				{	
						des[i++] = *(p++);
				}
				des[i+1] = '\0';
			break;
			}
		}
		memset(tmpdes,0x00,sizeof(tmpdes));
		inlen=strlen(des);
		outlen=inlen*2;
		code_convert("UTF-8","GBK",des,&inlen,tmpdes,&outlen);
    if(memcmp(errcode,"000",3)==0){
        vtcp_log("发送数据到短信平台且应答成功![%s]\n",tmpdes);
        return(0);
    }else{
        vtcp_log("发送数据到短信平台但应答失败errcode=[%.3s][%s]!\n",errcode,tmpdes);
        return(-1);
    }
    return(0);
}
/******************************************************************
函数名:nMobSendOne
功能  :发送一条指定记录到短信平台
参数  :
       addr      平台地址
       port      平台端口
       date      日期
       traceno   流水
返回值:
      -1         失败
      0          成功 
********************************************************************/ 
int nMobSendOne(char *addr,int port,int date,int traceno){
    struct mob_sendmail_c mob_sendmail;
    int iRc;
    char buf[33];
    

    if(sql_begin()<0){
        vtcp_log("%s,%d,开始事务失败!\n",__FILE__,__LINE__);
        return(-1);
    }

    memset(&mob_sendmail,'\0',sizeof(mob_sendmail));
    iRc=Mob_sendmail_Dec_Upd(buf,"tx_date=%d and trace_no=%d and proc_flag='0'",date,traceno);
    if(iRc){
        vtcp_log("%s,%d,HERE游标定义错误!\n",__FILE__,__LINE__);
        sql_rollback();
        return(-1);
    }
    iRc=Mob_sendmail_Fet_Upd(&mob_sendmail,buf);
    if(iRc && iRc!=100){
        vtcp_log("%s,%d,FETCH游标错误!\n",__FILE__,__LINE__);
        Mob_sendmail_Clo_Upd();
        sql_rollback();
        return(-1);
    }else if(iRc==100){
        vtcp_log("%s,%d,指定短信记录不存在!\n",__FILE__,__LINE__);
        Mob_sendmail_Clo_Upd();
        sql_commit();
        return(0);
    }
    /*************houyi后台转发到平台一次就不再发送
    if(nMobSendRecord(addr,port,&mob_sendmail)<0){
        sql_rollback();
        Mob_sendmail_Clo_Upd();
        return(-1);
    }
    mob_sendmail.proc_flag[0]='1';
    ******************************************/
    /*****侯毅20100528******/
    if(nMobSendRecord(addr,port,&mob_sendmail)<0)
    {
    	mob_sendmail.proc_flag[0]='2';
    }
    else
    {
	    mob_sendmail.proc_flag[0]='1';
    }
    /******侯毅结束******/
    iRc=Mob_sendmail_Upd_Upd(mob_sendmail,buf);
    if(iRc){
        vtcp_log("%s,%d,更新数据库失败!\n",__FILE__,__LINE__);
        Mob_sendmail_Clo_Upd();
        sql_rollback();
        return(-1);
    }
    Mob_sendmail_Clo_Upd();
    sql_commit();
    return(0);
}
/*************************************************************************
函数名:nMobSendAll
功能  :发送所有未发送记录到短信平台
参数  :
       addr      平台地址
       port      平台端口
       date      日期
       traceno   流水
返回值:
      -1         失败
      0          成功 
**************************************************************************/ 
int nMobSendAll(char *addr,int port,int date,int traceno){
    struct mob_sendmail_c mob_sendmail;
    int iRc;
    char buf[33];


    if(sql_begin()<0){
       vtcp_log("%s,%d,开始事务失败!\n",__FILE__,__LINE__);
       return(-1);
    }

    while(1){
        memset(&mob_sendmail,'\0',sizeof(mob_sendmail));
        iRc=Mob_sendmail_Dec_Upd(buf,"proc_flag='0'");
        if(iRc){
            vtcp_log("%s,%d,游标定义错误!\n",__FILE__,__LINE__);
            sql_rollback();
            return(-1);
        }
        iRc=Mob_sendmail_Fet_Upd(&mob_sendmail,buf);
        if(iRc && iRc!=100){
            vtcp_log("%s,%d,FETCH游标错误!\n",__FILE__,__LINE__);
            Mob_sendmail_Clo_Upd();
            sql_rollback();
            return(-1);
        }else if(iRc==100){
            vtcp_log("%s,%d,所有短信发送完毕!\n",__FILE__,__LINE__);
            Mob_sendmail_Clo_Upd();
            sql_commit();
            return(0);
        }
        /*************houyi后台把短信只发往平台一次，异常时只状态2
        if(nMobSendRecord(addr,port,&mob_sendmail)<0){
            vtcp_log("[%s][%d] 注意要执行sql_rollback",__FILE__,__LINE__);
            Mob_sendmail_Clo_Upd();
            sql_rollback();
            return(-1);
        }
        mob_sendmail.proc_flag[0]='1';
        **********************************************/
        /**侯毅20100528**/
        if(nMobSendRecord(addr,port,&mob_sendmail)<0)
        {
	        mob_sendmail.proc_flag[0]='2';
        }
        else
        {
	        mob_sendmail.proc_flag[0]='1';
        }
        /**侯毅20100528结束**/
        iRc=Mob_sendmail_Upd_Upd(mob_sendmail,buf);
        if(iRc){
            vtcp_log("%s,%d,更新数据库失败!\n",__FILE__,__LINE__);
            Mob_sendmail_Clo_Upd();
            sql_rollback();
            return(-1);
        }
        Mob_sendmail_Clo_Upd();
        sql_commit();
    	if(sql_begin()<0){
       	vtcp_log("%s,%d,开始事务失败!\n",__FILE__,__LINE__);
       	return(-1);
    	}
    }
   
    return(0);
}
int nMobMsgDebug( char *buf){
	vtcp_log("package:[%s]\n",buf);
	return(0);
}
int md5(char*desc,char*md)
{
char msg[50];
char tmp[50];
char out[50];
memset(out,0x00,sizeof(out));
memset(tmp,0x00,sizeof(tmp));
memset(msg,0x00,sizeof(msg));
strcpy(tmp,desc);
MD5ENC(tmp,out,strlen(tmp));
sprintf(msg,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],out[8],out[9],out[10],out[11],out[12],out[13],out[14],out[15]);
strcpy(md,msg); 
return 0;
}
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t *inlen,char *outbuf,size_t *outlen)
{
 iconv_t cd;
 int rc;
 char **pin = &inbuf;
 char **pout = &outbuf;
        cd = iconv_open(to_charset,from_charset);
        if (cd==0) return -1;
                memset(outbuf,0,*outlen);
        if (iconv(cd,pin,inlen,pout,outlen)==-1)
        {
                return -1;
        }
        iconv_close(cd);
        return 0;
}
int changeTime(long date,long time,char *standTime)
{
	char tmp[40];
	char Date[20];
	char Time[20];
	char hour[3];
	char min[3];
	char sec[3];
	char year[5];
	char mon[3];
	char day[3];
	memset(tmp,0x00,sizeof(tmp));
	memset(Date,0x00,sizeof(Date));
	memset(Time,0x00,sizeof(Time));
	memset(hour,0x00,sizeof(hour));
	memset(min,0x00,sizeof(min));
	memset(sec,0x00,sizeof(sec));
	memset(year,0x00,sizeof(year));
	memset(mon,0x00,sizeof(mon));
	memset(day,0x00,sizeof(day));
	sprintf(Date,"%ld",date);
	zip_space(Date);
	memcpy(year,Date,4);
	memcpy(mon,Date+4,2);
	memcpy(day,Date+6,2);
	sprintf(Time,"%ld",time);
	zip_space(Time);
	if(strlen(Time)==6)
		{
	memcpy(hour,Time,2);
	memcpy(min,Time+2,2);
	memcpy(sec,Time+4,2);
		}
		else
			{
	memcpy(hour,Time,1);
	memcpy(min,Time+1,2);
	memcpy(sec,Time+3,2);
			}
	sprintf(tmp,"%s-%02s-%02s %02s:%02s:%02s",year,mon,day,hour,min,sec);
	strcpy(standTime,tmp);
	return 0;
}
