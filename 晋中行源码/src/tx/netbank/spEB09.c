/*************************************************
* �� �� ��:  spEB09.c
* ���������� ����������˵�ǩԼ/��Լ
*
* ��    ��:  huzhaohua
* ������ڣ� 2013��7��31��
*
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "netbank_yqdz_c.h"
#include "mdm_ac_rel_c.h"

char *mmstrtok(char *instr, char *outstr, char *delimiter);
int spEB09()
{

    int ret;
    char sFlag[2];
    char sTemp[512];
    char sAc_no[13+1];
    char *s_sp=NULL;
    struct netbank_yqdz_c netbank_yqdz;
    struct com_sys_parm_c com_sys_parm;
    struct dd_mst_c dd_mst;
    struct mdm_ac_rel_c mdm_ac_rel;
    int iNum=0;
    
    memset(sFlag,0x0,sizeof(sFlag));
    memset(sTemp,0x0,sizeof(sTemp));
    memset(sAc_no,0x0,sizeof(sAc_no));
    memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
    memset(&com_sys_parm,0x0,sizeof(struct com_sys_parm_c));
    memset(&dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    get_zd_data("0670",sFlag); /* �������� */
    ret= Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
    if(ret)
    {
    vtcp_log("[%s][%d]�õ�ϵͳ���ڴ�![%d]\n",__FILE__,__LINE__,ret);
    goto ErrExit;
    }
    get_zd_data("0950",sTemp); /* �˻����� */
    get_zd_int("0480",&iNum);
    vtcp_log("[%s][%d]��������[%s],�˻�����[%s]���˺Ÿ���[%d],�������ڣ�[%d]\n",__FILE__,__LINE__,sFlag,sTemp,iNum,com_sys_parm.sys_date);
    s_sp=mmstrtok(sTemp, sAc_no, "|");/*ǩԼ�˺�*/
  for(;iNum>0;iNum--)
  {
    memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
    memset(sAc_no,0x0,sizeof(sAc_no));
    s_sp=mmstrtok(s_sp, sAc_no, "|");/*ǩԼ�˺�*/
    vtcp_log("[%s][%d]����������˻�[%s]\n",__FILE__,__LINE__,sAc_no);
    ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",sAc_no);
    if(ret)
    {
    vtcp_log("[%s][%d]�˺Ų�����![%d]\n",__FILE__,__LINE__,ret);
    strcpy(g_pub_tx.reply,"M003");
    goto ErrExit;
    }
    
    ret= Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id='%ld'",mdm_ac_rel.ac_id);
    if(ret)
    {
    vtcp_log("[%s][%d]�ͻ�id������![%d]\n",__FILE__,__LINE__,ret);
    strcpy(g_pub_tx.reply,"M003");
    goto ErrExit;
    }
    
    netbank_yqdz.ac_id=mdm_ac_rel.ac_id;
    ret= Netbank_yqdz_Sel(g_pub_tx.reply,&netbank_yqdz,"ac_id='%ld'",mdm_ac_rel.ac_id);
    if(ret==100)
    {
     if(strcmp(sFlag,"1") == 0)
     {
     vtcp_log("[%s][%d]���û���δǩԼ�����ܽ�Լ![%d]\n",__FILE__,__LINE__,ret);
     strcpy(g_pub_tx.reply,"EB01");
     goto ErrExit;
     }
     else if(strcmp(sFlag,"0") == 0)
     {
      /**����ǩԼ**/
      strcpy(netbank_yqdz.sts,"0");
      netbank_yqdz.sign_date=com_sys_parm.sys_date;
      ret = Netbank_yqdz_Ins(netbank_yqdz, g_pub_tx.reply);
      if (ret != 0)
      {
       sprintf(acErrMsg,"���������ǩԼ������¼����!![%d]",ret);
       WRITEMSG
       goto ErrExit;
      }
     vtcp_log("[%s][%d]�Ǽ��������ǩԼ��ɹ�!\n",__FILE__,__LINE__);
     }
     else
     {
     strcpy(g_pub_tx.reply,"H037");
     vtcp_log("[%s][%d]�������ʹ���!\n",__FILE__,__LINE__);
     goto ErrExit;
     }
    }
    else
    {
    if(strcmp(netbank_yqdz.sts,"0") == 0 && strcmp(sFlag,"0") == 0)
      {
       strcpy(g_pub_tx.reply,"EB02");
       vtcp_log("[%s][%d]�Ѿ�ǩԼ�û�����������ǩԼ����![%d]\n",__FILE__,__LINE__,ret);
       goto ErrExit;
      }
     else if(strcmp(netbank_yqdz.sts,"1") == 0 && strcmp(sFlag,"1") == 0)
      {
       strcpy(g_pub_tx.reply,"EB03");
       vtcp_log("[%s][%d]���û��Ѿ���Լ������������Լ![%d]\n",__FILE__,__LINE__,ret);
       goto ErrExit;
      }
     else if(strcmp(netbank_yqdz.sts,"0") == 0 && strcmp(sFlag,"1") == 0)
      {
      /**���Ѿ�ǩԼ�û�����Լ����**/
       ret = Netbank_yqdz_Dec_Upd(g_pub_tx.reply," ac_id=%ld ", mdm_ac_rel.ac_id);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Declare error [%d]",ret);
           WRITEMSG
           goto ErrExit;
       }
       memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
       ret = Netbank_yqdz_Fet_Upd(&netbank_yqdz,g_pub_tx.reply);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Fetch error [%d] ",ret);
           WRITEMSG
           Netbank_yqdz_Clo_Upd();
           goto ErrExit;
       }
       
       strcpy(netbank_yqdz.sts,"1");
       netbank_yqdz.cancel_date=com_sys_parm.sys_date;
       ret = Netbank_yqdz_Upd_Upd(netbank_yqdz, g_pub_tx.reply);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Update error [%d] ",ret);
           WRITEMSG
           Netbank_yqdz_Clo_Upd();
           goto ErrExit;
       }
       Netbank_yqdz_Clo_Upd();
      }
     else if(strcmp(netbank_yqdz.sts,"1") == 0 && strcmp(sFlag,"0") == 0)
      {
      /**���Ѿ���Լ�û�������ǩԼ����**/
       ret = Netbank_yqdz_Dec_Upd(g_pub_tx.reply," ac_id=%ld ", mdm_ac_rel.ac_id);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Declare error [%d]",ret);
           WRITEMSG
           goto ErrExit;
       }
       memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
       ret = Netbank_yqdz_Fet_Upd(&netbank_yqdz,g_pub_tx.reply);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Fetch error [%d] ",ret);
           WRITEMSG
           Netbank_yqdz_Clo_Upd();
           goto ErrExit;
       }
       
       strcpy(netbank_yqdz.sts,"0");
       netbank_yqdz.sign_date=com_sys_parm.sys_date;
       ret = Netbank_yqdz_Upd_Upd(netbank_yqdz, g_pub_tx.reply);
       if (ret != 0)
       {
           sprintf(acErrMsg,"Update error [%d] ",ret);
           WRITEMSG
           Netbank_yqdz_Clo_Upd();
           goto ErrExit;
       }
       Netbank_yqdz_Clo_Upd();
      }
     if(strcmp(sFlag,"0") != 0 && strcmp(sFlag,"1") != 0)
      {
      strcpy(g_pub_tx.reply,"H037");
      vtcp_log("[%s][%d]�������ʹ���!\n",__FILE__,__LINE__);
      goto ErrExit;
      }
    }
  }

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

/**********************************
��instr�н�ȡ��һ��delimiter֮ǰ�����ݷŵ�outstr�У����ص�һ��delimiter֮���λ��
**************************************/
char *mmstrtok(char *instr, char *outstr, char *delimiter)
{
  char *tmpstr;

  if(memcmp(instr, delimiter, strlen(instr))==0){
    return(NULL);
  }
  if(instr==NULL || strlen(instr)==0){return(NULL);}
  tmpstr=strstr(instr,delimiter);
  if(tmpstr!=NULL){
    memcpy(outstr, instr, strlen(instr)-strlen(tmpstr));
    return(strstr(instr,delimiter)+strlen(delimiter));
  }else{
    memcpy(outstr, instr, strlen(instr));
    return(NULL);
  }
}

