/*****************************************************
* �� �� ��:  sp7025.c                              ***
* ���������� У����Ȩ��Ա                          ***
* ��    ��:  zhangtilei                            ***
* ������ڣ� 2012-10-23 14:50:10                   ***
*                                                  ***
* �޸ļ�¼��                                       ***
*   ��   ��:                                       ***
*   �� �� ��:                                      ***
*   �޸�����:                                      ***
insert into tx_def values ('7025', 'У����Ȩ��Ա', '10000000000000000000110001000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000', '0', '5', '2',NULL,NULL);
insert into tx_flow_def values ('7025', 0, '7025', '#$');
insert into tx_sub_def values ('7025', 'У����Ȩ��Ա', 'sp7025', '0', '0');
*****************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp7025()
{
   int    iRet = 0;
   char   cTelj[TEL_LENGTH+1]; /*���׹�Ա*/
   char   cTels[TEL_LENGTH+1]; /*��Ȩ��Ա*/
   char   cPwd[7];  /*����*/
    
   struct com_tel_c sCom_tel;
   struct com_tel_c sCom_tel1;
   
   memset(&sCom_tel, 0x00, sizeof(sCom_tel));
   memset(&sCom_tel1, 0x00, sizeof(sCom_tel1));
   memset(cTelj, 0x00, sizeof(cTelj));
   memset(cTels, 0x00, sizeof(cTels));
   memset(cPwd, 0x00, sizeof(cPwd));
   
   get_zd_data("0070",cTelj);
   get_zd_data("0920",cTels);
   get_zd_data("0790",cPwd);
   
   vtcp_log("%s,%d,0070=[%s]",__FILE__,__LINE__,cTelj);
   vtcp_log("%s,%d,0920=[%s]",__FILE__,__LINE__,cTels);
   vtcp_log("%s,%d,0790=[%s]",__FILE__,__LINE__,cPwd);
   /*��ʼ��ȫ�ֱ���*/
    iRet = pub_base_sysinit();
    if(iRet)
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
   
   /*��ѯ��Ȩ��Ա������Ϣ*/
   iRet=Com_tel_Sel(g_pub_tx.reply, &sCom_tel, "tel='%s'", cTels);
   if(iRet)
    {
       sprintf(acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]",iRet);
       WRITEMSG
       strcpy(g_pub_tx.reply, "S009");
       goto ErrExit;
    }
   if(strcmp(g_pub_tx.tx_br_no,sCom_tel.br_no))   /*���������Ƿ����*/
   {
      sprintf(acErrMsg,"��Ȩ��Ա�ͽ��׹�Ա������ͬ");
      WRITEMSG
      strcpy(g_pub_tx.reply,"S083");
      goto ErrExit;
   }
   pub_base_EnDes(g_pub_tx.tx_date, "",cPwd);   /*��Ȩ��Ա�����Ƿ���ȷ*/
   /*if (strcmp(cPwd, sCom_tel.pwd) != 0) */
   if ( memcmp(cPwd, sCom_tel.pwd,strlen(sCom_tel.pwd)) ) 
   {
      sprintf(acErrMsg, "�������벻��ȷ!tel=[%s]", cTels);
      WRITEMSG
      strcpy(g_pub_tx.reply, "M005");
      goto ErrExit;
   }
   /*�����Ȩ��Ա״̬*/
   /*if(sCom_tel.csts[0] != '0')                 
   {
      sprintf(acErrMsg, "��Ȩ��Աδǩ��!", cTels);
      WRITEMSG
      strcpy(g_pub_tx.reply, "S017");
      goto ErrExit;
   }*/
    
   memset(&sCom_tel1, 0x00, sizeof(sCom_tel1));
   iRet=Com_tel_Sel(g_pub_tx.reply, &sCom_tel1, "tel='%s'", cTelj);
   if(iRet)
   {
       printf(acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]",iRet);
       WRITEMSG
       strcpy(g_pub_tx.reply, "S009");
       goto ErrExit;
   }
   if(sCom_tel1.lvl[0] <= sCom_tel.lvl[0])            /*�����Ȩ��Ա����*/
   {
      vtcp_log("%s,%d,���׹�Ա����%s����С����Ȩ��Ա%s",__FILE__,__LINE__,sCom_tel1.lvl , sCom_tel.lvl);
      sprintf(acErrMsg,"����ƾ֤����Ա�����ܲ�ѯ���Լ�����ߵĹ�Ա��ƾ֤");
      WRITEMSG
      strcpy(g_pub_tx.reply,"Z10K");
      goto ErrExit;     
   }
OkExit:
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 0;
ErrExit:
  sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 1;
   
}

