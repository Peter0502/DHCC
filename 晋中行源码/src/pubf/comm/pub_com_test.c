#include "find_debug.h" 
/*************************************************
* �� �� ��:  pub_com_test.c
* ���������� ���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
* ��  ��:    Ѧ��
* ������ڣ� 2011-8-9 
*
* �����б�
*         1��pub_base_sysinit();��ʼ������
*         2��pub_base_check_draw();������뺯��
*         3��pubf_card_chk_msr();��鿨�ŵ���Ϣ����
*         4��pub_com_ChkNoteUse();��Աƾ֤��麯��
*         5��pub_com_ChkAcCheq();�ͻ�ƾ֤��麯��
*������
*     ���룺
*     char * cCtrlcmd; ���Ʊ�־
*          ����־(0�����1���)��
*          [0]:�������־0����1��
*          [1]:�ŵ���Ϣ����־0����1��
*          [2]:֤������־0����1��
*          [3]:��������־0���鲻��
*          [4]:�ʺż���־0���鲻��
*          [5]:�տ��˹��ʱ�־0�ʺű���������黧��1�ʺŻ������붼���2�ʺű��������������������3�ʺŻ�����һ��������,9ǿ�ƹ���
*          [6]:ͨ��ͨ�ұ�־0����1������
*          [7]:ƾ֤����־0����1���Աƾ֤2��ͻ�ƾ֤
*     char *cCert_no; ֤������(1λ)+����(19λ) ֤�����ͣ�1�����֤��2�����ڲ���
*          3�����գ�4������֤��5������֤��6��ʿ��֤��7���۰ľ�������ͨ��֤
*          8����ҵ����֤��9����Ӫ���֤��A: Ӫҵִ�գ�B: ��ҵ����֤�飻
*          C�����̺�׼�ţ�D���������Թ�����E����ʱ���֤��F������˾���֤��
*          G������֤��H������֤������˽����F��̨��ͬ������ͨ��֤
*     char *cAc_name;����
*     char *cAc_no;�˺�
*     char *cNote_type;ƾ֤����
*     char *cNote_no;ƾ֤����
*     char *cDc_ind;�����־ 1��2��
*     �����
*     char *cGz_flag;���˱�־
*     char *sts;ƾ֤״̬
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "svrpub_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "com_item_c.h"
#include "com_parm_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "mo_loss_c.h"
char * get_bin_data(char *,char *);
int pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no, cNote_type,cNote_no,cDc_ind,cGz_flag)
    char *cCtrlcmd;
    char *cCert_no;
    char *cAc_name;
    char *cAc_no;
    char *cNote_type;
    char *cNote_no;
    char *cDc_ind;
    char *cGz_flag;
{
    int iRet;
    int lKeylen=0;
    int iPwdlen=0;
    char  cName_tmp[61];/** �ȶ���NAME **/
    char  cVal[17];   /*ʮ������*/
    char  cPwd[17];
    char  cDraw_pwd[17];
    char sts[51];/*ƾ֤״̬*/
    struct  dd_mst_c    sDd_mst;
    struct  dd_parm_c   sDd_parm;
    struct  com_item_c  sCom_item;
    struct  prdt_ac_id_c sPrdt_ac_id;
    struct  in_mst_c     sIn_mst;
    struct  mo_loss_c    sMo_loss;
    struct  com_parm_c   sCom_parm;
    /*��ʼ������*/
   
    memset(sts,0x00,sizeof(sts));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sMo_loss,0x00,sizeof(sMo_loss));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(cVal,0x00,sizeof(cVal));
    memset(cPwd,0x00,sizeof(cPwd));
    memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
    
    /*��������*/
    get_bin_data("0800",cDraw_pwd);
    /**20111113  CD
    get_fd_data("0800", cDraw_pwd);
    **/
    /*vtcp_log("[%s],[%d],cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cNote_type=[%s],cNote_no=[%s],cDc_ind=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cNote_type,cNote_no,cDc_ind);
    pub_base_old_acno(cAc_no);  /** �¾��˺�ת�� **/
    memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
    zip_space(g_pub_tx.ac_no);
    if( strlen ( cAc_no ) >10) /*** ���������,ֻ�����˺� ***/
    {
      iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
      if (iRet)
      {
          vtcp_log("%s,%d,ac_no[%s]����mdm_ac_rel����!",__FILE__,__LINE__,g_pub_tx.ac_no);
          strcpy(g_pub_tx.reply,"C114");
          goto ErrExit;
      }
      if(g_mdm_ac_rel.note_sts[0] != '0')
      {
          vtcp_log("%s,%d,ac_no[%s]����״̬������!",__FILE__,__LINE__,g_pub_tx.ac_no);
          if(g_mdm_ac_rel.note_sts[0] == '1') /*��ʧ*/
          {
              iRet = Mo_loss_Sel(g_pub_tx.reply,&sMo_loss,"ac_no ='%s' and unloss_ind ='0'",g_mdm_ac_rel.ac_no);
              if(iRet)
              {
                  vtcp_log("[%s][%d],��ѯ��ʧ�Ǽǲ�����",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"P189");
                  goto ErrExit;
              }
              if(sMo_loss.loss_ind[0] == '1')
              {
                 strcpy(g_pub_tx.reply,"P501");
              }
              else if(sMo_loss.loss_ind[0] == '2')
              {
                 strcpy(g_pub_tx.reply,"P502");
              }
              else if(sMo_loss.loss_ind[0] == '3')
              {
                 strcpy(g_pub_tx.reply,"P503");
              }
              else if(sMo_loss.loss_ind[0] == '4')
              {
                 strcpy(g_pub_tx.reply,"P504");
              }
              else
              { 
                    strcpy(g_pub_tx.reply,"P170");
              }              
          }
          else if(g_mdm_ac_rel.note_sts[0] == '2')/*��ʧ��֤*/
          {
              strcpy(g_pub_tx.reply,"P171");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '*')/*����*/
          {
              strcpy(g_pub_tx.reply,"P172");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '3')/*������֤*/
          {
              strcpy(g_pub_tx.reply,"M022");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '5')/*����*/
          {
              strcpy(g_pub_tx.reply,"M023");
          }
          else /*����*/
          {
              strcpy(g_pub_tx.reply,"D140");
          }
          goto ErrExit;
      }
      vtcp_log("%s,%d,ac_no[%s]����״̬����!",__FILE__,__LINE__,g_pub_tx.ac_no);
      iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
      if (iRet)
      {
          vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_rel����!",__FILE__,__LINE__,cAc_no);
          strcpy(g_pub_tx.reply,"W013");
          goto ErrExit;
      }
      if(sPrdt_ac_id.ac_id_type[0] == '1')
      {
          iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
          if (iRet)
          {
              vtcp_log("%s,%d,ac_no[%s]����dd_mst����!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"P102");
              goto ErrExit;
          }
          if(sDd_mst.ac_sts[0] != '1')
          {
              vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"O085");
              goto ErrExit;
          }
          memcpy(cName_tmp,sDd_mst.name,sizeof(cName_tmp)-1);
          iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
          if (iRet)
          {  
              vtcp_log("%s,%d,prdt_no[%s]��ѯdd_parm����!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"W015");
              goto ErrExit;
          }
      }
      else if(sPrdt_ac_id.ac_id_type[0] == '9')
      {
          iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
          if (iRet)
          {
              vtcp_log("%s,%d,ac_no[%s]����in_mst����!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"A029");
              goto ErrExit;
          }
          if(sIn_mst.sts[0] != '1')
          {
              vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"O085");
              goto ErrExit;
          }
          memcpy(cName_tmp,sIn_mst.name,sizeof(cName_tmp)-1);
					zip_space(cName_tmp);
					
      }
       if ( cCtrlcmd[3] == '1' )    /** ��黧����Ϣ **/
       {
           zip_space(cAc_name);
           vtcp_log("%s,%d ����=[%s]",__FILE__,__LINE__,cAc_name);
           if( cDc_ind[0] == '2')
           {
              if(cCtrlcmd[5] == '1' && memcmp (cName_tmp, cAc_name,sizeof(cName_tmp)-1))
              {
                vtcp_log("%s,%d,cName_tmp=[%s]",__FILE__,__LINE__,cName_tmp);
                 sprintf(acErrMsg,"��������!!!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "P042");
                 goto ErrExit;
              }
              if( cCtrlcmd[5] == '9' || ((cCtrlcmd[5] == '2' || cCtrlcmd[5] == '3' ) && memcmp (cName_tmp , cAc_name,sizeof(cName_tmp)-1)))
              {
                 cGz_flag[0] = '2';
              }
                 vtcp_log("%s,%d,cGz_flag[0]=[%s]",__FILE__,__LINE__,cGz_flag); 
           }
       }
    }
    else
    {    /**** �����ǿ�Ŀ  **/
         iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
         if(iRet)
         {
             sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B172");
             goto ErrExit;
         }   
         if ( cCtrlcmd[3] == '1' )    /** ��黧����Ϣ **/
         {
             zip_space(cAc_name);
             vtcp_log("%s,%d ����=%s",__FILE__,__LINE__,cAc_name);
             if( cDc_ind[0] == '2')
             {
                 if(cCtrlcmd[5] == '1' && memcmp (sCom_item.acc_name , cAc_name,sizeof(sCom_item.acc_name ) ))
                 {
                     sprintf(acErrMsg,"��Ŀ��������!!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "P042");
                     goto ErrExit;
                 }
                 if( cCtrlcmd[5] == '9' || ((cCtrlcmd[5] == '2' || cCtrlcmd[5] == '3' ) && memcmp (sCom_item.acc_name  , cAc_name,sizeof(sCom_item.acc_name ))))
                 {
                     cGz_flag[0] = '2';
                 } 
                 vtcp_log("%s,%d,22cGz_flag[0]=%s",__FILE__,__LINE__,cGz_flag);
             }
         }
    }    
    /**����ʼ����Ҫ�ء�**/
    if ( cCtrlcmd[0] == '1' ) /* �ж������Ƿ���ȷ */
    {   
        /*ADD BEGIN BY CHENGBO 20111020 14:47 ���ܴ���*/
        iRet = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='PTMY' and parm_seqn=1");
        if( iRet )
        {
             sprintf(acErrMsg,"��ѯ��Կʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "G001");
             goto ErrExit;
        }
        lKeylen=strlen(sCom_parm.val);
        vtcp_log("[%s][%d] lKeylen=[%d]",__FILE__,__LINE__,lKeylen);
        /*iPwdlen=strlen(cDraw_pwd);*/
        iPwdlen=8;
        vtcp_log("[%s][%d] iPwdlen=[%d]",__FILE__,__LINE__,iPwdlen);
        vtcp_log("[%s][%d] val=[%s],draw_pwd=[%s]",__FILE__,__LINE__,sCom_parm.val,cDraw_pwd);
        vtcp_log("[%s][%d] draw_pwd[0]=[%x]",__FILE__,__LINE__,cDraw_pwd[0]);
        vtcp_log("[%s][%d] draw_pwd[1]=[%x]",__FILE__,__LINE__,cDraw_pwd[1]);
        vtcp_log("[%s][%d] draw_pwd[2]=[%x]",__FILE__,__LINE__,cDraw_pwd[2]);
        vtcp_log("[%s][%d] draw_pwd[3]=[%x]",__FILE__,__LINE__,cDraw_pwd[3]);
        vtcp_log("[%s][%d] draw_pwd[4]=[%x]",__FILE__,__LINE__,cDraw_pwd[4]);
        vtcp_log("[%s][%d] draw_pwd[5]=[%x]",__FILE__,__LINE__,cDraw_pwd[5]);
        vtcp_log("[%s][%d] draw_pwd[6]=[%x]",__FILE__,__LINE__,cDraw_pwd[6]);
        vtcp_log("[%s][%d] draw_pwd[7]=[%x]",__FILE__,__LINE__,cDraw_pwd[7]);
        /*תʮ������*/
        pubf_str_hex2str(sCom_parm.val,cVal,lKeylen/2);
        
        /*��3des����*/
        iRet = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cPwd,8); 
        if( iRet )
        {
             sprintf(acErrMsg,"����ʧ��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*ADD END BY CHENGBO 20111020 14:47 ���ܴ���*/
        vtcp_log("[%s][%d],draw_pwd=[%s]",__FILE__,__LINE__,cPwd);
        memcpy(g_pub_tx.draw_pwd,cPwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        iRet = pub_base_check_draw("001");
        if(iRet)
        {
            sprintf(acErrMsg,"֧ȡ����������!");
            if(memcmp(g_pub_tx.reply,"M036",4))
            {
                sprintf(g_pub_tx.reply,"M005");
            }
            goto ErrExit;

        }
        else
        {
            vtcp_log("%s,%d,����������У������ˡ�����",__FILE__,__LINE__);
        }
    }
    if ( cCtrlcmd[1] == '1' ) /** ���ŵ���Ϣ **/
    {
        iRet = pubf_card_chk_msr(3,cAc_no);
        if ( iRet )
        {
          sprintf(acErrMsg,"���ŵ���Ϣʧ��!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "CU05");
          goto ErrExit;
        }
    }
    if ( cCtrlcmd[2] == '1' ) /** ���֤����Ϣ **/
    {
        if ( cCert_no[0] != g_mdm_ac_rel.id_type[0] )
        {
            sprintf(acErrMsg,"֤�����Ͳ���!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "P444");
            goto ErrExit;
        }
        else
        {
          vtcp_log("%s,%d,���֤������ͨ����",__FILE__,__LINE__);
        }
        iRet=memcmp (g_mdm_ac_rel.id_no , cCert_no+1,strlen(g_mdm_ac_rel.id_no) );
        vtcp_log("%s,%d,g_mdm_ac_rel.id_no=[%s],cCert_no+1=[%s]",__FILE__,__LINE__,g_mdm_ac_rel.id_no,cCert_no+1);
        if(iRet!=0)
        {
            /**modify by lw 20110917 ���������������֤����λ**/
            if(cCert_no[0] == '1' && strlen(cCert_no) == 7 && nCheckID(g_mdm_ac_rel.id_no,cCert_no+1) == 0)
            {
                vtcp_log("[%s][%d]�������֤�ź���λ��֤ͨ����",__FILE__,__LINE__);
            }
            else
            {
             sprintf(acErrMsg,"֤�����벻��!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "P445");
             goto ErrExit;
            }
        }
        else
        {
            vtcp_log("%s,%d,���֤������ͨ����",__FILE__,__LINE__);
        }
    }
    /** if ( cCtrlcmd[4] == '1' )  ����˺���Ϣ 
    {
      if ( memcmp (g_mdm_ac_rel.ac_no , cAc_no,sizeof(g_mdm_ac_rel.ac_no) ))
      {
        sprintf(acErrMsg,"�˺Ų���!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "YYL4");
        goto ErrExit;
      }else{
      if ( memcmp (sCom_item.acc_no , cAc_no,sizeof(sCom_item.acc_no) ))
      {
        sprintf(acErrMsg,"��Ŀ�˺Ų���!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "YYL4");
        goto ErrExit;
       }
      }
    }**/
    if ( cCtrlcmd[6] == '0') /** ���ͨ��ͨ�ұ�־ **/
    {
    	/*Ӧ�������ͨ�� Modefied by YangMeng 2014/7/11 9:23:25*/
        if (sDd_parm.thr_dpst_ind[0] == 'N' && memcmp(sDd_parm.prdt_no,"138",3))
        {
            sprintf(acErrMsg,"ͨ���־����ȷ������");
            strcpy(g_pub_tx.reply,"A102");
            WRITEMSG
            goto ErrExit;
        }
        if ( sDd_parm.thr_draw_ind[0] =='N')
        {
            sprintf(acErrMsg,"ͨ�ұ�־����ȷ������");
            strcpy(g_pub_tx.reply,"A102");
            WRITEMSG
            goto ErrExit;
        }
    }
    /*ƾ֤����־0����1���Աƾ֤2��ͻ�ƾ֤*/
    if ( cCtrlcmd[7] == '1' )
    {
        iRet = pub_com_ChkNoteUse(cNote_type);
        if (iRet)
        {
            sprintf(acErrMsg,"����Աƾ֤����");
            WRITEMSG
            strcpy(g_pub_tx.reply, "M020");
            goto ErrExit;
        }
    }
    else if( cCtrlcmd[7] == '2' )
    {
      iRet = pub_com_ChkAcCheq( g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,cNote_type, cNote_no, cNote_no, sts );
      if ( iRet )
      {
        sprintf(acErrMsg,"��ѯ�ͻ�ƾ֤����");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N080");
        goto ErrExit;
      }
      vtcp_log("%s,%d,sts[0]=%s,iRET=%d,g_mdm_ac_rel.ac_id=%ld,",__FILE__,__LINE__,sts,iRet,g_mdm_ac_rel.ac_id);
      if ( sts[0] == '0' )
      {
        sprintf( acErrMsg, "�ͻ��޴�ƾ֤!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] ƾ֤����=[%s] ƾ֤����=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N080" );
        goto ErrExit;
      }
      else if ( sts[0] == '2' )
      {
        sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] ƾ֤����=[%s] ƾ֤����=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N081" );
        goto ErrExit;
      } else if ( sts[0] == '3' )
      {   
        sprintf( acErrMsg, "��ƾ֤�ѻ���!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] ƾ֤����=[%s] ƾ֤����=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N082" );
        goto ErrExit;
      } else if ( sts[0] == '4' )
      {
        sprintf( acErrMsg, "��ƾ֤������!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] ƾ֤����=[%s] ƾ֤����=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N083" );
        goto ErrExit;
      } else if ( sts[0] == '5' )
      {
        sprintf( acErrMsg, "��ƾ֤��ʹ��!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] ƾ֤����=[%s] ƾ֤����=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N084" );
        goto ErrExit;
      }
    }

OKExit:
    return 0;
ErrExit:
    return 1;
}
/**
������:
��  ��: luowei
��  ��: 2011-09-17
��  ��: ��֤���֤����λ
��  ��:
        id_no ���֤����
        no    ���֤�ź���λ
����ֵ: 0/1
**/
int nCheckID(const char *id_no, const char *no)
{
	char sNo[7];
	int  len=0;
	
	memset(sNo,'0',sizeof(sNo));
	sNo[sizeof(sNo)-1]='\0';
	len=strlen(sNo)-1;
	
	for(int i=strlen(id_no); i>=0; i--)
	{
		if(id_no[i]>='0' && id_no[i]<='9')
		{
			sNo[len]=id_no[i];
			len--;
			if(len<0)
			{
				break;
			}
		}	
	}
	vtcp_log("[%s][%d]��������ʽȡ�����֤[%s]����λ[%s]",__FILE__,__LINE__,id_no,sNo);
	if(memcmp(sNo,no,strlen(sNo))!=0)
	{
		vtcp_log("[%s][%d]���֤����λ[%s][%s]",__FILE__,__LINE__,sNo,no);
		return(1);
	}
	return(0);
}



/**********************************************************************
* �� �� ����  get_bin_data
* �������ܣ�  ȡ��ָ�����ݱ������������
* ��    �ߣ�  
* ���ʱ�䣺  2003��12��16��
*
* ��    ����
*     ��  �룺sjbm:    ���ݱ���
*      
*     ��  ����data:    ���ص�����
*
*     ����ֵ��ȡ�õ�����
*             NULLʧ��
*
* �޸���ʷ��
*                   
********************************************************************/

char *get_bin_data( char *sjbm, char *data )
{
	char	tmpdata[1024];
	int i;

	memset(tmpdata, 0 , sizeof(tmpdata));
	data[0] = 0;
	vtcp_log("[%s][%d]get_bin_data*****************sjbm=[%s]",__FILE__,__LINE__,sjbm);
	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
	{
		vtcp_log("%s,%d i==%d ",__FILE__,__LINE__,i);TRACE;
		return NULL;
	}
	pub_base_Nctoc((char*)Pu_Val[i].val,tmpdata,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);  
	
	memcpy(data,tmpdata,Pu_Val[i].length);

	return data;
}
