 /***************************************************************************
 *** �������� : Shi shenghui ʯ����                                       ***
 *** ��    �� : 2004-09-23                                                ***
 *** ����ģ�� :                                                           ***
 *** �������� : ���紫��⺯��                                            ***
 ***                                                                      ***
 *** ʹ��ע�� :  ��ƽ̨���ͽ��ж������˻����ս�����ϸ�ļ�                 ***
 *** insert into com_eod_exec values ('6006','R957','rpt957','��ƽ̨�������˻�������ϸ','1111111111','0','0','00','00','2','Y','Y','N','2','Y');
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
#define WAITSECONDS 10          /****TODO:��������ʱ��****/

/**TESTADDR,TESTPORT Ϊ���Ե�ַ,����ʱ�����ʵ�����ȷ��**/
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

    iRc = sql_begin ();  /*������*/
    if(iRc)
    {
        sprintf(acErrMsg, "������ʧ��!!!iRc=[%d]",iRc);
        WRITEMSG
        goto ErrExit;
    }
    pub_base_sysinit();
    
#if 0 /* ƽ̨�ⲻ�� ��ʱע���ɸ�Ϊ 0 */
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
      vtcp_log("������ϸ�ļ�ʧ�� [%d] [%s] [%d]",iRc,__FILE__,__LINE__);
      goto ErrExit;
    }

   iRc=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
   if(iRc)
		{
      vtcp_log("��ѯϵͳ���ڳ���! [%d] [%s] [%d]",iRc,__FILE__,__LINE__);
      goto ErrExit;
		}

    tmp_date=s_com_sys_parm.lst_date;
    vtcp_log("�õ�������ϵͳ���� tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);
		
    if( tmp_date%100==21 )
		{
			icount1=sql_count("trace_log_bk","tx_date=%ld and tx_code='Z037' and svc_ind='1201' and sts='0' and amt>0 ",tmp_date);    
			vtcp_log("���޽����˻���Ϣ������0�ļ�¼��[%d]!\n",icount1);
			
			if( icount1 > 0 )
			{
	      for(i=0;i<3;)
	      {
	          iSockId= FileSock_connect(TESTADDR,TESTPORT);
	          if(iSockId<0)
						{
	              perror("Connect:");
	              vtcp_log("%s,%d,����ƽ̨����!\n",__FILE__,__LINE__);
	              strcpy(g_pub_tx.reply,"9999");
	              goto ErrExit;
	          }
	
	          sprintf(tmpstr,"00000074%ld%012ldJx12%ldPLZW_%ld_%08ld_SH.txt %-12s",tmp_date,g_pub_tx.trace_no,tmp_date,tmp_date,g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	          vtcp_log("%s,%d,tmpstr[%s],����[%d]!\n",__FILE__,__LINE__,tmpstr,strlen(tmpstr));
	          iRc=FileSock_send(iSockId,tmpstr,strlen(tmpstr),-1);
	          if(iRc)
						{
	              vtcp_log("%s,%d,������Ϣ��ƽ̨����!\n",__FILE__,__LINE__);
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
	              vtcp_log("%s,%d,����ƽ̨��Ϣ����!\n",__FILE__,__LINE__);
	              strcpy(g_pub_tx.reply,"9999");
	              goto ErrExit;
	          }
	
	  	      vtcp_log("[%s][%d]buf=[%s]",__FILE__,__LINE__,buf+8);
	          if(memcmp(buf+8,"0000",4)==0)
	          {
	              vtcp_log("%s,%d,�յ���ȷӦ��![%s]\n",__FILE__,__LINE__,buf);
	              FileSock_close(iSockId);
	              break;
	          }else
						{
	              FileSock_close(iSockId);
	              vtcp_log("%s,%d,�յ�����Ӧ��,��������!i[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
	              i++;
	              continue;
	          }
	      }
	
				if(i>=3)
				{
	        vtcp_log("%s,%d,����ƽ̨�޸�״̬������ʱ����!i=[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
	        strcpy(g_pub_tx.reply, "9999");
	        goto ErrExit;
				}
				
				vtcp_log("%s,%d,�������ݳ���[%d]\n",__FILE__,__LINE__,strlen(buf));
			}
			else if( icount1==0 )
			{
				vtcp_log("����21��û�з��޽����˻���Ϣ��¼!\n");
			}
    }

    icount2=sql_count("Sub_dd_mst_hst","tx_date=%ld and tx_amt>0 ",tmp_date);    
		vtcp_log("���޽����˻�[%ld]������ϸ������0�ļ�¼��[%d]!\n",tmp_date,icount2);

    if( icount2>0 )
		{
			for(i=0;i<3;)
			{
        iSockId= FileSock_connect(TESTADDR,TESTPORT);
        if(iSockId<0)
				{
            perror("Connect:");
            vtcp_log("%s,%d,����ƽ̨����!\n",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"9999");
            goto ErrExit;
        }

        vtcp_log("�õ�������ϵͳ����tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);
				memset(tmpstr,0x00,sizeof(tmpstr));
        sprintf(tmpstr,"00000086%ld%012ldJ021%ldWXJJ_AC_MX_%ld_SH.txt    %-12s�˻���ϸ����",tmp_date,g_pub_tx.trace_no,tmp_date,tmp_date,g_pub_tx.tx_br_no);
				vtcp_log("���͵����ձ��ĳ���Ϊ[%d],���Ĵ�Ϊ[%s]\n",strlen(tmpstr),tmpstr);

        iRc=FileSock_send(iSockId,tmpstr,strlen(tmpstr),-1);
        if(iRc)
				{
            vtcp_log("%s,%d,������Ϣ��ƽ̨����!\n",__FILE__,__LINE__);
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
            vtcp_log("%s,%d,����ƽ̨��Ϣ����!\n",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"9999");
            goto ErrExit;
        }

	      vtcp_log("[%s][%d]buf=[%s]",__FILE__,__LINE__,buf+8);
        if(memcmp(buf+8,"0000",4)==0)
        {
            vtcp_log("%s,%d,�յ���ȷӦ��![%s]\n",__FILE__,__LINE__,buf);
            FileSock_close(iSockId);
            break;
        }else
				{
            FileSock_close(iSockId);
            vtcp_log("%s,%d,�յ�����Ӧ��,��������!i[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
            i++;
            continue;
        }
        
			}

			if(i>=3)
			{
        vtcp_log("%s,%d,����ƽ̨�޸�״̬������ʱ����!i=[%d],buf[%s]\n",__FILE__,__LINE__,i,buf);
        strcpy(g_pub_tx.reply, "9999");
        goto ErrExit;
			}
		
			vtcp_log("%s,%d,�������ݳ���[%d]\n",__FILE__,__LINE__,strlen(buf));
		}
		else
		{
			vtcp_log("����û�з��޽����˻�������ϸ���˼�¼!\n");
		}


OkExit:
    sql_commit();           /*--�ύ����--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "�ɹ�����ƽ̨Ӧ����Ϣ[%s]", g_pub_tx.reply);
    set_zd_data(DC_GET_MSG,acErrMsg);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
    
ErrExit:
    sql_rollback();         /*--����ع�--*/
    sprintf(acErrMsg, "ƽ̨��������ʧ��!!", g_pub_tx.reply);
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

  vtcp_log("---���ɵ��ս�����ϸ�ļ���ʼ---");
  ret=Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
  if(ret)
  {
    vtcp_log("��ѯϵͳ���ڳ���! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

  tmp_date=s_com_sys_parm.lst_date;
  vtcp_log("�õ���ϵͳ���� tmp_date:[%ld] [%s] [%d]",tmp_date,__FILE__,__LINE__);

  /* ��Ϣ�������ɼ�Ϣ�����ݷ���ƽ̨ */
  if(tmp_date%100==21)
  {
    sprintf(filename,"%s/txt/PLZW_%ld_%08ld_SH.txt",getenv("HOME"),tmp_date,g_pub_tx.trace_no);
    /**ftp ���͵��ļ���**/
    sprintf(filename_send,"PLZW_%ld_%08ld_SH.txt",tmp_date,g_pub_tx.trace_no);

    ret=Trace_log_bk_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and tx_code='Z037' and svc_ind='1201' and sts='0' \
                          order by trace_no,trace_cnt",tmp_date);
    if(ret)
    {
      vtcp_log("��ȡ���������쳣.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }

    fp_tmp=fopen(filename,"w");
    if(fp_tmp == NULL)
    {
      vtcp_log("���ļ�ʧ��! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
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
        vtcp_log("��ȡ���ݽ���.[%s] [%d]",__FILE__,__LINE__);
        break;
      }
      else if(ret)
      {
        vtcp_log("��ȡ�����쳣.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
        goto ErrExit;
      }

      /* ����ľͲ����� */
      if(s_trace_log_bk.amt==0.0)
      {
        continue;
      }

      strcpy(tmpstr,"1ת�˴���Ϣ");
      fprintf(fp_tmp,"%s-%ld|%.2lf|%ld|313175000011|%s\n",s_trace_log_bk.ac_no,s_trace_log_bk.ac_id,s_trace_log_bk.amt,tmp_date,tmpstr);  /**������ͳһ�趨Ϊ313175000011��Ϊ���ڳ�����д���ˣ����䶯���޸ĳ��� by lzy 20140902**/
    } 
    fclose(fp_tmp);
    memset(tmpcmd,0x00,sizeof(tmpcmd)-1);
    /**
    sprintf(tmpcmd,"ftptopingtai.sh %s",filename_send);
    **/
    sprintf(tmpcmd,"ftptopingtai.sh %s %s %s",get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),filename_send);
    vtcp_log("����Ϊ: [%s] [%s] [%d]",tmpcmd,__FILE__,__LINE__);
    ret=system(tmpcmd);
    if(ret)
    {
      vtcp_log("ϵͳ����ʧ��! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }
  }

  memset(filename,0x00,sizeof(filename));
  memset(filename_send,0x00,sizeof(filename_send));
  memset(tmpstr,0x00,sizeof(tmpstr));
  memset(tmpcmd,0x00,sizeof(tmpcmd));

  sprintf(filename,"%s/txt/WXJJ_AC_MX_%ld_SH.txt",getenv("HOME"),tmp_date);
  /*ftp���͵�ƽ̨���ļ���*/
  sprintf(filename_send,"WXJJ_AC_MX_%ld_SH.txt",tmp_date);

  ret=Sub_dd_mst_hst_Dec_Sel(g_pub_tx.reply,"tx_date=%ld order by opn_br_no,ac_no,sub_ac_no,trace_no",tmp_date);
  if(ret)
  {
    vtcp_log("��ȡ���������쳣.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

  fp_tmp=fopen(filename,"w");
  if(fp_tmp == NULL)
  {
    vtcp_log("���ļ�ʧ��! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
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
      vtcp_log("��ȡ���ݽ���.[%s] [%d]",__FILE__,__LINE__);
      break;
    }
    else if(ret)
    {
      vtcp_log("��ȡ�����쳣.[%d] [%s] [%d]",ret,__FILE__,__LINE__);
      goto ErrExit;
    }

    /* ����ľͲ����� */
    if(s_sub_dd_mst_hst.tx_amt==0.0)
    {
      continue;
    }

    if(s_sub_dd_mst_hst.ct_ind[0]=='1' && s_sub_dd_mst_hst.add_ind[0]=='0')
    {
      strcpy(tmpstr,"2�ֽ�ȡ��");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='1' && s_sub_dd_mst_hst.add_ind[0]=='1')
    {
      strcpy(tmpstr,"1�ֽ���");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='2' && s_sub_dd_mst_hst.add_ind[0]=='0')
    {
      strcpy(tmpstr,"2ת��ȡ��");
    }
    else if(s_sub_dd_mst_hst.ct_ind[0]=='2' && s_sub_dd_mst_hst.add_ind[0]=='1')
    {
      strcpy(tmpstr,"1ת�˴��");
    }

    fprintf(fp_tmp,"%s|%ld|%.2lf|%s\n",s_sub_dd_mst_hst.sub_ac_no,tmp_date,s_sub_dd_mst_hst.tx_amt,tmpstr);

  }
  fclose(fp_tmp);
  memset(tmpcmd,0x00,sizeof(tmpcmd));
  sprintf(tmpcmd,"ftptopingtai.sh %s %s %s",get_env_info("SW_FTP_USER"),get_env_info("SW_FTP_PWD"),filename_send);
  vtcp_log("����Ϊ: [%s] [%s] [%d]",tmpcmd,__FILE__,__LINE__);
  ret=system(tmpcmd);
  if(ret)
  {
    vtcp_log("ϵͳ����ʧ��! [%d] [%s] [%d]",ret,__FILE__,__LINE__);
    goto ErrExit;
  }

OkExit:
  Sub_dd_mst_hst_Clo_Sel();
  fclose(fp_tmp);
  vtcp_log("---���ɵ��ս�����ϸ�ļ� �ɹ� ---,[%s] [%d]",__FILE__,__LINE__);
  strcpy(g_pub_tx.reply,"0000");
  set_zd_data("0120",g_pub_tx.reply);
  return (0);

ErrExit:
  Sub_dd_mst_hst_Clo_Sel();
  if(fp_tmp != NULL) fclose(fp_tmp);
  vtcp_log("---���ɵ��ս�����ϸ�ļ� ʧ�� !!! ---,[%s] [%d]",__FILE__,__LINE__);
  if( strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
  {
    strcpy(g_pub_tx.reply,"L207");
  }
  set_zd_data("0120",g_pub_tx.reply);
  return (1);
}

/**********************************************************************
*  int FileSock_bind(char *addr,int port);                            *
*  ���ܣ���ʼ�����ط���                                               *
*  ������                                                             *
*      addr-���ص�ַ                                                  *
*      port-���ض˿�                                                  *
*  ����ֵ��socket id/-1                                               *
*                                                                     *
**********************************************************************/
int FileSock_bind(char *addr,int port){

        /*��ʼ��linehandler��socket����*/
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
*  ���ܣ�����Զ������                                                 *
*  ����ֵ��socket id/-1                                               *
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
*  ���ܣ��ر�Զ������                                                 *
*  ����������id                                                       *
*  ����ֵ��                                                           *
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
*  ���ܣ���������                                                     *
*  ������iSocketID-Զ��socket id                                      *
*        ht-�������ݵĹ�ϣ��ṹ                                      *
*  ����ֵ:0/-1                                                        *
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
                        vtcp_log("%s,%d,�ȴ��������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nSubLen=send(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,�������ݴ���,errno=%d\n",__FILE__,__LINE__,errno);
                        return(-1);
                }
                nTotalLen+=nSubLen;
        }
        return(0);
}

/**********************************************************************
*  int FileSock_recv                                                        *
*  ���ܣ�����Զ������                                                 *
*  ������iSocketID-Զ��socket id                                      *
*        ht-���չ�ϣ��ṹ                                            *
*  ����ֵ:0/-1                                                        *
*                                                                     *
**********************************************************************/
int FileSock_recv(int iSocketID,char *buf,int length,int timeout){/***����ָ����������***/
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
                        vtcp_log("%s,%d,�ȴ��������ݴ���,timeout=%d,errno=%d\n",__FILE__,__LINE__,timeout,errno);
                        return(-1);
                }
                nSubLen=recv(iSocketID,buf+nTotalLen,nShouldLen-nTotalLen,0);
                if(nSubLen<=0){
                        vtcp_log("%s,%d,�������ݴ���,errno=%d,len=%d\n",__FILE__,__LINE__,errno,nSubLen);
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
*  ���ܣ�����Զ�̷���                                                 *
*  ������                                                             *
*      addr-Զ�̵�ַ                                                  *
*      port-Զ�̶˿�                                                  *
*  ����ֵ��socket id/-1                                               *
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
        /*Add by SSH,2004.7.27,�����ε�ַ�����4λ����Ϊ0��Ȼ��ȴ��ʱ����*/
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
        /*ѭ���ȴ���ֱ��socket���п�д*/
        while(1){
                iRc = select(iSocketID+1,0,&rmask,0,ptr);
                if(iRc>0){
                        break;
                }else if(iRc==0){
                        /*��ʱ*/
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
        /*ѭ���ȴ���ֱ��socket���пɶ�*/
        while(1){
                iRc = select(iSocketID+1,&rmask,0,0,ptr);
                vtcp_log("%s,%d,ѭ���ȴ���ֱ��socket���пɶ�,iRc=%d\n",__FILE__,__LINE__,iRc);
                if(iRc>0){
                        break;
                }else if(iRc==0){
                        /*��ʱ*/
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