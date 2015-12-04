 /***************************************************************************
 *** 程序作者 : Shi shenghui 石升辉                                       ***
 *** 日    期 : 2004-09-23                                                ***
 *** 所属模块 :                                                           ***
 *** 程序作用 : 网络传输库函数                                            ***
 ***                                                                      ***
 *** 使用注意 :  向平台发送晋中二级子账户当日交易明细文件                 ***
 *** insert into com_eod_exec values ('6006','R957','rpt957','向平台发送子账户当日明细','1111111111','0','0','00','00','2','Y','Y','N','2','Y');
 *****************************************************************************/
#include"stdlib.h"
#include"string.h"
#include"stdio.h"
#include"errno.h"
#include"sys/types.h"
#include"sys/select.h"
#include"sys/socket.h"
#include"sys/ioctl.h"
#include"sys/stat.h"
#include"netinet/in.h"
#include"netinet/tcp.h"
#include"arpa/inet.h"
#include"fcntl.h"
#include"unistd.h"
#include"netdb.h"
#include"sys/errno.h"
#ifdef SCO
#define socklen_t size_t
#endif
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "trace_log_bk_c.h"
static int netGetPeer(int iSocketID, char *pbyIPAddr );
static int FileSock_waitfor_read(int iSocketID,int waitseconds);
static int FileSock_waitfor_write(int iSocketID,int waitseconds);
#include"errlog.h"
#define WAITSECONDS 10          /****TODO:测试缩短时间****/

/**TESTADDR,TESTPORT 为测试地址,上线时请根据实际情况确定**/
#define TESTADDR    "172.168.78.150"
#define TESTPORT    11189 


extern char *get_env_info(char *infoname);
int rpt957()
{
    int iSockId,i=0;
    char buf[1024];
    char tmpstr[256];
    FILE *fp;
    FILE *fp_tmp;
    int  iLength;
    int  iRc;
    long tmp_date;
    char filename[64];
    char ptaddr[20];
    int ptport;
    int icount1=0;
		int icount2=0;
    memset(ptaddr,0,sizeof(ptaddr));

    struct com_sys_parm_c s_com_sys_parm;
    memset(&s_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));

    iRc = sql_begin ();  /*打开事务*/
    if(iRc)
    {
        sprintf(acErrMsg, "打开事务失败!!!iRc=[%d]",iRc);
        WRITEMSG
        goto ErrExit;
    }
    pub_base_sysinit();
    
#if 0 /* 平台测不到 暂时注掉可改为 0 */
    if(getenv("PTADDR") == NULL)
    {
        vtcp_log("%s,%d,getenv[%s]",__FILE__,__LINE__,(char*)getenv("PTADDR"));
        strcpy(g_pub_tx.reply,"9999");
        goto ErrExit;
    }
    vtcp_log("%s,%d,getenv[%s]",__FILE__,__LINE__,(char*)getenv("PTADDR"));
   
    memcpy(ptaddr,getenv("PTADDR"),sizeof(ptaddr)-1);
    if(getenv("PTPORT") ==NULL)
    {
        vtcp_log("%s,%d,getenv[%s]",__FILE__,__LINE__,(char*)getenv("PTPORT"));
        strcpy(g_pub_tx.reply,"9999");
        goto ErrExit;
    }
    ptport = atoi(getenv("PTPORT"));
    vtcp_log("%s,%d addr[%s],port[%d]",__FILE__,__LINE__,ptaddr,ptport);
#endif

    iRc=rpt957a();
    
    if(iRc)
    {
      vtcp_log("生成明细文件失败 [%d] [%s] [%d]",iRc,__FILE__,__LINE__);
      goto ErrExit;
    }

   iRc=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
   if(iRc)
		{
      vtcp_log("查询系统日期出错! [%d] [%s] [%d]",iRc,__FILE__,__LINE__);
      goto ErrExit;
		}

    tmp_date=s_com_sys_parm.lst_date;
    vtcp_log("得到的上日系统日期 tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);
		
    if( tmp_date%100==21 )
		{
			icount1=sql_count("trace_log_bk","tx_date=%ld and tx_code='Z037' and svc_ind='1201' and sts='0' and amt>0 ",tmp_date);    
			vtcp_log("房修金子账户结息金额大于0的记录数[%d]!\n",icount1);
			
			if( icount1 > 0 )
			{
	      for(i=0;i<3;)
	      {
	          iSockId= FileSock_connect(TESTADDR,TESTPORT);
	          if(iSockId<0)
						{
	              perror("Connect:");
	              vtcp_log("%s,%d,连接平台错误!\n",__FILE__,__LINE__);
	              strcpy(g_pub_tx.reply,"9999");
	              goto ErrExit;
	          }
	
	          sprintf(tmpstr,"00000074%ld%012ldJx12%ldPLZW_%ld_%08ld_SH.txt %-12s",tmp_date,g_pub_tx.trace_no,tmp_date,tmp_date,g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	          vtcp_log("%s,%d,tmpstr[%s],长度[%d]!\n",__FILE__,__LINE__,tmpstr,strlen(tmpstr));
	          iRc=FileSock_send(iSockId,tmpstr,strlen(tmpstr),-1);
	          if(iRc)
						{
	              vtcp_log("%s,%d,发送信息到平台错误!\n",__FILE__,__LINE__);
	              perror("Send:");
	              strcpy(g_pub_tx.reply,"9999");
	              goto ErrExit;
	          }
	          
	          sleep(10);
	          memset(buf,0,sizeof(buf));
	          iRc=FileSock_recv(iSockId,buf,8,-1);
	          iLength=atoi(buf);
	          iRc=FileSock_recv(iSockId,buf+8,iLength,-1);
	          if(iRc)
						{
	              perror("Send:");
	              vtcp_log("%s,%d,接收平台信息错误!\n",__FILE__,__LINE__);
	              strcpy(g_pub_tx.reply,"9999");
	              goto ErrExit;
	          }
	
	  	      vtcp_log("[%s][%d]buf=[%s]",__FILE__,__LINE__,buf+8);
	          if(memcmp(buf+8,"0000",4)==0)
	          {
	              vtcp_log("%s,%d,收到正确应答![%s]\n",__FILE__,__LINE__,buf);
	              FileSock_close(iSockId);
	              break;
	          }else
						{
	              FileSock_close(iSockId);
	              vtcp_log("%s,%d,收到错误应答,重新请求!i[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
	              i++;
	              continue;
	          }
	      }
	
				if(i>=3)
				{
	        vtcp_log("%s,%d,请求平台修改状态到日终时出错!i=[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
	        strcpy(g_pub_tx.reply, "9999");
	        goto ErrExit;
				}
				
				vtcp_log("%s,%d,接收数据长度[%d]\n",__FILE__,__LINE__,strlen(buf));
			}
			else if( icount1==0 )
			{
				vtcp_log("本月21日没有房修金子账户结息记录!\n");
			}
    }

    icount2=sql_count("Sub_dd_mst_hst","tx_date=%ld and tx_amt>0 ",tmp_date);    
		vtcp_log("房修金子账户[%ld]对账明细金额大于0的记录数[%d]!\n",tmp_date,icount2);

    if( icount2>0 )
		{
			for(i=0;i<3;)
			{
        iSockId= FileSock_connect(TESTADDR,TESTPORT);
        if(iSockId<0)
				{
            perror("Connect:");
            vtcp_log("%s,%d,连接平台错误!\n",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"9999");
            goto ErrExit;
        }

        vtcp_log("得到的上日系统日期tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);
				memset(tmpstr,0x00,sizeof(tmpstr));
        sprintf(tmpstr,"00000086%ld%012ldJ021%ldWXJJ_AC_MX_%ld_SH.txt    %-12s账户明细对账",tmp_date,g_pub_tx.trace_no,tmp_date,tmp_date,g_pub_tx.tx_br_no);
				vtcp_log("发送的日终报文长度为[%d],报文串为[%s]\n",strlen(tmpstr),tmpstr);

        iRc=FileSock_send(iSockId,tmpstr,strlen(tmpstr),-1);
        if(iRc)
				{
            vtcp_log("%s,%d,发送信息到平台错误!\n",__FILE__,__LINE__);
            perror("Send:");
            strcpy(g_pub_tx.reply,"9999");
            goto ErrExit;
        }
        
        sleep(10);
        memset(buf,0,sizeof(buf));
        iRc=FileSock_recv(iSockId,buf,8,-1);
        iLength=atoi(buf);
        iRc=FileSock_recv(iSockId,buf+8,iLength,-1);
        if(iRc)
				{
            perror("Send:");
            vtcp_log("%s,%d,接收平台信息错误!\n",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"9999");
            goto ErrExit;
        }

	      vtcp_log("[%s][%d]buf=[%s]",__FILE__,__LINE__,buf+8);
        if(memcmp(buf+8,"0000",4)==0)
        {
            vtcp_log("%s,%d,收到正确应答![%s]\n",__FILE__,__LINE__,buf);
            FileSock_close(iSockId);
            break;
        }else
				{
            FileSock_close(iSockId);
            vtcp_log("%s,%d,收到错误应答,重新请求!i[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
            i++;
            continue;
        }
        
			}

			if(i>=3)
			{
        vtcp_log("%s,%d,请求平台修改状态到日终时出错!i=[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
        strcpy(g_pub_tx.reply, "9999");
        goto ErrExit;
			}
		
			vtcp_log("%s,%d,接收数据长度[%d]\n",__FILE__,__LINE__,strlen(buf));
		}
		else
		{
			vtcp_log("本日没有房修金子账户对账明细对账记录!\n");
		}


OkExit:
    sql_commit();           /*--提交事务--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "成功接收平台应答信息[%s]", g_pub_tx.reply);
    set_zd_data(DC_GET_MSG,acErrMsg);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
    
ErrExit:
    sql_rollback();         /*--事务回滚--*/
    sprintf(acErrMsg, "平台处理请求失败!!", g_pub_tx.reply);
    set_zd_data(DC_GET_MSG,acErrMsg);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

int rpt957a()
{
  int ret=1;
  FILE *fp_tmp=NULL;
  long tmp_date;
  char filename[64];
  char filename_send[64];
  char tmpstr[20];
  char tmpcmd[128];
  struct sub_dd_mst_hst_c s_sub_dd_mst_hst;
  struct com_sys_parm_c s_com_sys_parm;
  struct trace_log_bk_c s_trace_log_bk;

  memset(filename,0x00,sizeof(filename));
  memset(filename_send,0x00,sizeof(filename_send));
  memset(tmpstr,0x00,sizeof(tmpstr));
  memset(tmpcmd,0x00,sizeof(tmpcmd));

  memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
  memset(&s_com_sys_parm,0x00,sizeof(struct com_sys_parm_c));
  memset(&s_trace_log_bk,0x00,sizeof(struct trace_log_bk_c));

  vtcp_log("---生成当日交易明细文件开始---");
  ret=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
  if(ret)
  {
    vtcp_log("查询系统日期出错! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

  tmp_date=s_com_sys_parm.lst_date;
  vtcp_log("得到的系统日期 tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);

  /* 结息日先生成计息的数据发给平台 */
  if(tmp_date%100==21)
  {
    sprintf(filename,"%s/txt/PLZW_%ld_%08ld_SH.txt",getenv("HOME"),tmp_date,g_pub_tx.trace_no);
    /**ftp 发送的文件名**/
    sprintf(filename_send,"PLZW_%ld_%08ld_SH.txt",tmp_date,g_pub_tx.trace_no);

    ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and tx_code='Z037' and svc_ind='1201' and sts='0' \
                          order by trace_no,trace_cnt",tmp_date);
    if(ret)
    {
      vtcp_log("提取数据声明异常.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }

    fp_tmp=fopen(filename,"w");
    if(fp_tmp == NULL)
    {
      vtcp_log("打开文件失败! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
      strcpy(g_pub_tx.reply,"L206");
      goto ErrExit;
    }

    while(1)
    {
      memset(tmpstr,0x00,sizeof(tmpstr));
      memset(&s_trace_log_bk,0x00,sizeof(struct trace_log_bk_c));
      ret=Trace_log_bk_Fet_Sel(&s_trace_log_bk,g_pub_tx.reply);
      if(ret==100)
      {
        vtcp_log("提取数据结束.[%s] [%d]",__FILE__,__LINE__);
        break;
      }
      else if(ret)
      {
        vtcp_log("提取数据异常.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
        goto ErrExit;
      }

      /* 零金额的就不发了 */
      if(s_trace_log_bk.amt==0.0)
      {
        continue;
      }

      strcpy(tmpstr,"1转账存款结息");
      fprintf(fp_tmp,"%s-%ld|%.2lf|%ld|313175000011|%s\n",s_trace_log_bk.ac_no,s_trace_log_bk.ac_id,s_trace_log_bk.amt,tmp_date,tmpstr);  /**机构号统一设定为313175000011，为此在程序中写死了，若变动需修改程序 by lzy 20140902**/
    } 
    fclose(fp_tmp);
    memset(tmpcmd,0x00,sizeof(tmpcmd)-1);
    /**
    sprintf(tmpcmd,"ftptopingtai.sh %s",filename_send);
    **/
    sprintf(tmpcmd,"ftptopingtai.sh %s %s %s",get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),filename_send);
    vtcp_log("命令为: [%s] [%s] [%d]",tmpcmd,__FILE__,__LINE__);
    ret=system(tmpcmd);
    if(ret)
    {
      vtcp_log("系统调用失败! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }
  }

  memset(filename,0x00,sizeof(filename));
  memset(filename_send,0x00,sizeof(filename_send));
  memset(tmpstr,0x00,sizeof(tmpstr));
  memset(tmpcmd,0x00,sizeof(tmpcmd));

  sprintf(filename,"%s/txt/WXJJ_AC_MX_%ld_SH.txt",getenv("HOME"),tmp_date);
  /*ftp发送到平台的文件名*/
  sprintf(filename_send,"WXJJ_AC_MX_%ld_SH.txt",tmp_date);

  ret=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date=%ld order by opn_br_no,ac_no,sub_ac_no,trace_no",tmp_date);
  if(ret)
  {
    vtcp_log("提取数据声明异常.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

  fp_tmp=fopen(filename,"w");
  if(fp_tmp == NULL)
  {
    vtcp_log("打开文件失败! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"L206");
    goto ErrExit;
  }

  while(1)
  {
    memset(tmpstr,0x00,sizeof(tmpstr));
    memset(&s_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
    ret=Sub_dd_mst_hst_Fet_Sel(&s_sub_dd_mst_hst,g_pub_tx.reply);
    if(ret==100)
    {
      vtcp_log("提取数据结束.[%s] [%d]",__FILE__,__LINE__);
      break;
    }
    else if(ret)
    {
      vtcp_log("提取数据异常.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }

    /* 零金额的就不发了 */
    if(s_sub_dd_mst_hst.tx_amt==0.0)
    {
      continue;
    }

    if(s_sub_dd_mst_hst.ct_ind[0]=='1' && s_sub_dd_mst_hst.add_ind[0]=='0')
    {
      strcpy(tmpstr,"2现金取款");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='1' && s_sub_dd_mst_hst.add_ind[0]=='1')
    {
      strcpy(tmpstr,"1现金存款");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='2' && s_sub_dd_mst_hst.add_ind[0]=='0')
    {
      strcpy(tmpstr,"2转账取款");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='2' && s_sub_dd_mst_hst.add_ind[0]=='1')
    {
      strcpy(tmpstr,"1转账存款");
    }

    fprintf(fp_tmp,"%s|%ld|%.2lf|%s\n",s_sub_dd_mst_hst.sub_ac_no,tmp_date,s_sub_dd_mst_hst.tx_amt,tmpstr);

  }
  fclose(fp_tmp);
  memset(tmpcmd,0x00,sizeof(tmpcmd));
  sprintf(tmpcmd,"ftptopingtai.sh %s %s %s",get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),filename_send);
  vtcp_log("命令为: [%s] [%s] [%d]",tmpcmd,__FILE__,__LINE__);
  ret=system(tmpcmd);
  if(ret)
  {
    vtcp_log("系统调用失败! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

OkExit:
  Sub_dd_mst_hst_Clo_Sel();
  fclose(fp_tmp);
  vtcp_log("---生成当日交易明细文件 成功 ---,[%s] [%d]",__FILE__,__LINE__);
  strcpy(g_pub_tx.reply,"0000");
  set_zd_data("0120",g_pub_tx.reply);
  return (0);

ErrExit:
  Sub_dd_mst_hst_Clo_Sel();
  if(fp_tmp != NULL) fclose(fp_tmp);
  vtcp_log("---生成当日交易明细文件 失败 !!! ---,[%s] [%d]",__FILE__,__LINE__);
  if( strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
  {
    strcpy(g_pub_tx.reply,"L207");
  }
  set_zd_data("0120",g_pub_tx.reply);
  return (1);
}

/**********************************************************************
*  int FileSock_bind(char *addr,int port);                            *
*  功能：初始化本地服务                                               *
*  参数：                                                             *
*      addr-本地地址                                                  *
*      port-本地端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/
int FileSock_bind(char *addr,int port){

        /*初始化linehandler的socket服务*/
        int iSocketID;
        struct sockaddr_in stSockAddr;
        struct linger TimeOut;
        int iLength;
        int iRc;
        iSocketID=socket(AF_INET, SOCK_STREAM, 0);
        if(iSocketID<0){
            vtcp_log("%s,%d,Creat socket error\n",__FILE__,__LINE__);
            return(-1);
        }
        iLength = sizeof(stSockAddr);
        stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(port);

        TimeOut.l_onoff = 1;
        TimeOut.l_linger = 10;
        setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
        setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
        /*setsockopt(iSocketID,SOL_SOCKET,SO_REUSEPORT,(char *)&TimeOut,sizeof(TimeOut));*/
        iRc = bind(iSocketID,(struct sockaddr*)(&stSockAddr),iLength);
        setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
        setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
        /*setsockopt(iSocketID,SOL_SOCKET,SO_REUSEPORT,(char *)&TimeOut,sizeof(TimeOut));*/
        if(iRc<0){
            vtcp_log("%s,%d,Bind socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        iRc = listen(iSocketID, 1);
        if(iRc<0){
            vtcp_log("%s,%d,Listen socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        vtcp_log("%s,%d,Bind over\n",__FILE__,__LINE__);
        return(iSocketID);
}

/**********************************************************************
*  int FileSock_accept(int socketid);                                       *
*  功能：接受远程连接                                                 *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/
int FileSock_accept(int socketid){
        int iAcceptedID;
        struct linger TimeOut;
        char   caIP[4*4];
        unsigned int iLength;
        struct sockaddr_in stSockAddr;
        do {
        iLength=sizeof(stSockAddr);
        iAcceptedID = accept(socketid,(struct sockaddr*)&stSockAddr,
                        (socklen_t *)&iLength);
        if (iAcceptedID < 0) {
                        vtcp_log("%s,%d,Accept err,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }else{
                        if (iAcceptedID == 0) continue;
                        vtcp_log("Accept,socketId=%d\n",iAcceptedID);
                        TimeOut.l_onoff = 1;
                        TimeOut.l_linger = 10;
                        setsockopt(iAcceptedID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));

                        setsockopt(iAcceptedID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
                        netGetPeer(iAcceptedID, caIP);
                        vtcp_log("%s,%d,Accept socket,remote addr: %s\n",__FILE__,__LINE__,caIP);
                        return(iAcceptedID);
                }
        }while(1);

}
/**********************************************************************
*  int FileSock_close(int socketid);                                        *
*  功能：关闭远程连接                                                 *
*  参数：连接id                                                       *
*  返回值：                                                           *
*                                                                     *
**********************************************************************/
int FileSock_close(int socketid){
        int iRc;
        if(socketid==0)
                return(0);
        iRc=close(socketid);
        return(0);
}

/**********************************************************************
*  int FileSock_send                                                        *
*  功能：发送数据                                                     *
*  参数：iSocketID-远端socket id                                      *
*        ht-发送数据的哈希表结构                                      *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/
int FileSock_send(int iSocketID,char *buf,int length,int timeout){
        int  nTotalLen;
        int  nSubLen;
        int  nShouldLen;
        int iRc;
        iRc=nTotalLen=nSubLen=nShouldLen=0;
        nShouldLen=length;
        while(nTotalLen<nShouldLen){
                if(timeout<=0){
                        iRc=FileSock_waitfor_write(iSocketID,WAITSECONDS);
                }else{
                        iRc=FileSock_waitfor_write(iSocketID,timeout);
                }
                if(iRc<=0){
                        vtcp_log("%s,%d,等待发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,发送数据错误,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nTotalLen+=nSubLen;
        }
        return(0);
}

/**********************************************************************
*  int FileSock_recv                                                        *
*  功能：接收远端数据                                                 *
*  参数：iSocketID-远端socket id                                      *
*        ht-接收哈希表结构                                            *
*  返回值:0/-1                                                        *
*                                                                     *
**********************************************************************/
int FileSock_recv(int iSocketID,char *buf,int length,int timeout){/***接收指定长度数据***/
        int  nTotalLen;
        int  nSubLen;
        int  nShouldLen;
        int iRc;
        iRc=nTotalLen=nSubLen=nShouldLen=0;
        nShouldLen=length;
        while(nTotalLen<nShouldLen){
                if(timeout<=0){
                        iRc=FileSock_waitfor_read(iSocketID,WAITSECONDS);
                }else{
                        iRc=FileSock_waitfor_read(iSocketID,timeout);
                }
                if(iRc<=0){
                        vtcp_log("%s,%d,等待接收数据错误,timeout=%d,errno=%d\n",__FILE__,__LINE__,timeout,errno);
                        return(-1);
                }
                nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,接收数据错误,errno=%d,len=%d\n",__FILE__,__LINE__,errno,nSubLen);
                        return(-1);
                }
                nTotalLen+=nSubLen;
        }
        return(0);
}

static int netGetPeer(int iSocketID, char *pbyIPAddr )
{
        struct sockaddr_in stSockAddr;
        unsigned int iLen;
        iLen = sizeof( stSockAddr );
        if (pbyIPAddr) {
                if (getpeername(iSocketID,(struct sockaddr *)&stSockAddr,(socklen_t *)&iLen))
                {
                        return(-1);
                }
                strcpy(pbyIPAddr,(char *)inet_ntoa(stSockAddr.sin_addr));
        }
        return(0);
}
/**********************************************************************
*  int FileSock_connect(char *addr,int port);                               *
*  功能：连接远程服务                                                 *
*  参数：                                                             *
*      addr-远程地址                                                  *
*      port-远程端口                                                  *
*  返回值：socket id/-1                                               *
*                                                                     *
**********************************************************************/
int FileSock_connect(char *addr,int port){
        struct hostent *hp;
        struct sockaddr_in stSockAddr;
        struct linger TimeOut;
        unsigned long lInterAddr;
        int  iRc,iSocketID;
        char caIP[4*4];
        char *caServerName;
        int iPort;
        iPort=port;
        caServerName=addr;
        iSocketID = socket(AF_INET, SOCK_STREAM, 0);
        if(iSocketID<0){
            vtcp_log("%s,%d,Creat socket error[%d]\n",__FILE__,__LINE__,errno);
            return(-1);
        }
        memset((char *)&stSockAddr,'\0',sizeof(stSockAddr));
        hp = gethostbyname(caServerName);
        if (hp != NULL){
                lInterAddr = *((unsigned long *)hp->h_addr);
        }else{
            if ((lInterAddr = inet_addr(caServerName)) == -1) {
                    vtcp_log("%s,%d,Get server address error[%d]\n",__FILE__,__LINE__,errno);
                        close(iSocketID);
                        return(-1);
            }
        }
        stSockAddr.sin_family = AF_INET;
        stSockAddr.sin_port = htons(iPort);
        /*Add by SSH,2004.7.27,长整形地址的最后4位必须为0，然而却有时不是*/
        if(sizeof(lInterAddr)>4){
                memcpy(((char *)&lInterAddr)+4,(char *)&lInterAddr,4);
                memset((char *)&lInterAddr,'\0',4);
        }
        stSockAddr.sin_addr.s_addr = lInterAddr;
        iRc = connect(iSocketID,(struct sockaddr*)(&stSockAddr),sizeof(stSockAddr));
        if(iRc){
                vtcp_log("%s,%d,Connect socket error,[%s],[%d],errno=%d\n",__FILE__,__LINE__,addr,port,errno);
                close(iSocketID);
                return(-1);
        }
        TimeOut.l_onoff = 1;
        TimeOut.l_linger = 10;
        setsockopt(iSocketID,SOL_SOCKET,SO_LINGER,(char *)&TimeOut,sizeof(TimeOut));
        setsockopt(iSocketID,SOL_SOCKET,SO_REUSEADDR,(char *)&TimeOut,sizeof(TimeOut));
        netGetPeer(iSocketID, caIP);
        return(iSocketID);

}
static int FileSock_waitfor_write(int iSocketID,int waitseconds){
        fd_set    rmask;
        struct    timeval time_out,*ptr;
        int iRc;
        FD_ZERO(&rmask);
        FD_SET((unsigned int)iSocketID,&rmask);
        if(waitseconds>=0){
                time_out.tv_sec  = waitseconds ;
                time_out.tv_usec = 0;
        }else{
                time_out.tv_sec  = 0 ;
                time_out.tv_usec = 200000;
        }
        ptr = &time_out;
        iRc=0;
        /*循环等待，直到socket空闲可写*/
        while(1){
                iRc = select(iSocketID+1,0,&rmask,0,ptr);
                if(iRc>0){
                        break;
                }else if(iRc==0){
                        /*超时*/
                        break;
                }else{
                        if(errno==EINTR)
                                continue;
                        else
                                break;
                }
        }
        return iRc;
}

static int FileSock_waitfor_read(int iSocketID,int waitseconds){
        fd_set    rmask;
        struct    timeval time_out,*ptr;
        int iRc;
        FD_ZERO(&rmask);
        FD_SET((unsigned int)iSocketID,&rmask);
        if(waitseconds>=0){
                time_out.tv_sec  = waitseconds ;
                time_out.tv_usec = 0;
        }else{
                time_out.tv_sec  = 0 ;
                time_out.tv_usec = 200000;
        }
        ptr = &time_out;
        iRc=0;
        /*循环等待，直到socket空闲可读*/
        while(1){
                iRc = select(iSocketID+1,&rmask,0,0,ptr);
                vtcp_log("%s,%d,循环等待，直到socket空闲可读,iRc=%d\n",__FILE__,__LINE__,iRc);
                if(iRc>0){
                        break;
                }else if(iRc==0){
                        /*超时*/
                        break;
                }else{
                        if(errno==EINTR)
                                continue;
                        else
                                break;
                }
        }

        return iRc;
}