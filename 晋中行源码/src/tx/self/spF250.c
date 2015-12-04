/*******************************************
�汾�ţ�    V1.0
���ڣ�      20111024
�������ߣ�   shangyongchao
�������ƣ�  spF250.c
�����ܣ�  ��ƽ̨�����˺š���ѯ���롢֧�������У�����ͣ��Կͻ��������֧ȡ��ʽ���м��
            �ɹ����� 0000 ʧ�ܷ�����Ӧ������
�������ã�
  insert into tx_def values('6250','�ͻ�������','10000000000000000000000010000000000000000000000000000000000000000011100000000000000000000001000000000000000000000000000000000100','0','6','0','3',null);
  insert into tx_flow_def  values ('6250', 0, 'F250', '#$');
  insert into tx_sub_def  values ('F250', '�ͻ�������', 'spF250', '0', '0');

����˵�����������ǰ���ƽ̨����������Ϊ������Ϊ8λ������ģ�
          ���Ϊ�����ܵ�6λ�������룬�뽫���ܵĲ���ɾ��������Ӧ������ǰ2λ�������ַ���������ֻ��ȡ��Ӧ������6λ

******************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"

int spF250()
{
    int ret = 0;
    long lKeylen = 0;/*��Կ����*/
    long lQrypwd = 0;/*��ѯ���볤��*/
    long lDrawpwd = 0;/*֧ȡ���볤��*/
    char cVal[17];/*���ʮ�����Ƶ���Կ*/
    char cTmp[17];
    char cQry_pwd[9]; /*��ѯ����*/
    char cDraw_pwd[9];/*֧ȡ����*/
    char cCmp_type[2]; /*У������1У��֧ȡ���� 2 У���ѯ���� 3ȫ��У��*/
    char cAc_no[25];   /*�˺�*/
    struct com_parm_c sCom_parm;
    
    memset(cQry_pwd,0x00,sizeof(cQry_pwd));
    memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
    memset(cCmp_type,0x00,sizeof(cCmp_type));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
    /*��ʼ������*/
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    /*��ƽ̨ȡֵ*/
    get_zd_data("0300",cAc_no);
    vtcp_log("[%s][%d],�˺�=��%s��",__FILE__,__LINE__,cAc_no);
    get_zd_data("0700",cCmp_type);
    vtcp_log("[%s][%d],У������=��%s��",__FILE__,__LINE__,cCmp_type);
    get_fd_data("0800",cTmp);
    vtcp_log("[%s][%d],80��ֵ=��%s��",__FILE__,__LINE__,cTmp);
    
    memcpy(cDraw_pwd,cTmp,8);
    /*vtcp_log("[%s][%d],֧ȡ����=��%s��",__FILE__,__LINE__,cDraw_pwd);*/
    memcpy(cQry_pwd,cTmp+8,8);
    /*vtcp_log("[%s][%d],��ѯ����=��%s��",__FILE__,__LINE__,cQry_pwd);*/
    
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
    if(ret)
    {
        sprintf(acErrMsg,"��ѯmdm_ac_relʧ�ܣ���");
        strcpy(g_pub_tx.reply,"W011");
        WRITEMSG
        goto ErrExit;
    }
    ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='PTMY' and parm_seqn=1");
    if( ret )
    {
         sprintf(acErrMsg,"��ѯ��Կʧ��!!!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "G001");
         goto ErrExit;
    }
    lKeylen = strlen(sCom_parm.val);
    vtcp_log("[%s][%d] lKeylen=[%ld]",__FILE__,__LINE__,lKeylen);
    lDrawpwd = strlen(cDraw_pwd);
   /* vtcp_log("[%s][%d] lDrawpwd=[%ld]",__FILE__,__LINE__,lDrawpwd);*/
    lQrypwd = strlen(cQry_pwd);
    /*vtcp_log("[%s][%d],lQrypwd=[%ld]",__FILE__,__LINE__,lQrypwd);*/
    vtcp_log("[%s][%d] val=[%s]",__FILE__,__LINE__,sCom_parm.val);
    /*תʮ������*/
    pubf_str_hex2str(sCom_parm.val,cVal,lKeylen/2);
    
    if(cCmp_type[0] == '1')
    {
       /* vtcp_log("[%s][%d],֧ȡ����У��=[%s]",__FILE__,__LINE__,cCmp_type);*/
        /*��3des���� ֧ȡ����*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cDraw_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"����ʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
        }
       /* vtcp_log("[%s][%d],������cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
        memcpy(g_pub_tx.draw_pwd,cDraw_pwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        
        /*vtcp_log("[%s][%d],g_pub_tx.draw_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd); */
        ret = pub_base_check_draw("001");
        if(ret)
        {
            sprintf(acErrMsg,"֧ȡ����������!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if(cCmp_type[0] == '2')
    {
        vtcp_log("[%s][%d],��������У��=[%s]",__FILE__,__LINE__,cCmp_type);
        /*��3des���� ��ѯ����*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cQry_pwd,cQry_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"����ʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*vtcp_log("[%s][%d],������cQry_pwd=[%s]",__FILE__,__LINE__,cQry_pwd);*/
        memcpy(g_pub_tx.qry_pwd,cQry_pwd+2,sizeof(g_pub_tx.qry_pwd)-1);
        /*vtcp_log("[%s][%d],g_pub_tx.qry_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.qry_pwd);*/
        ret = pub_base_check_draw("011");
        if(ret)
        {
            sprintf(acErrMsg,"��ѯ����������!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if(cCmp_type[0] == '3')
    {
        vtcp_log("[%s][%d],ȫ��У��=[%s]",__FILE__,__LINE__,cCmp_type);
        ret = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cDraw_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"����ʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
        }
        /*vtcp_log("[%s][%d],������cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
        memcpy(g_pub_tx.draw_pwd,cDraw_pwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        
        /*vtcp_log("[%s][%d],g_pub_tx.draw_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
        ret = pub_base_check_draw("001");
        if(ret)
        {
            sprintf(acErrMsg,"֧ȡ����������!");
            WRITEMSG
            goto ErrExit;
        }
        /*��3des���� ��ѯ����*/
        ret = pubf_3des_dec(cVal,lKeylen/2,cQry_pwd,cQry_pwd,8); 
        if( ret )
        {
             sprintf(acErrMsg,"����ʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*vtcp_log("[%s][%d],������cQry_pwd=[%s]",__FILE__,__LINE__,cQry_pwd);*/
        memcpy(g_pub_tx.qry_pwd,cQry_pwd+2,sizeof(g_pub_tx.qry_pwd)-1);
        /*vtcp_log("[%s][%d],g_pub_tx.qry_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.qry_pwd);*/
        ret = pub_base_check_draw("011");
        if(ret)
        {
            sprintf(acErrMsg,"��ѯ����������!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else
    {
        sprintf(acErrMsg,"У���־����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O006");
        goto ErrExit;
    }
    
  OkExit:
      strcpy( g_pub_tx.reply, "0000" );
      sprintf(acErrMsg,"success!!");
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      return 0;

  ErrExit:
      if (strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
      {
        strcpy(g_pub_tx.reply,"CU06");
      }
      sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      return 1;    
}