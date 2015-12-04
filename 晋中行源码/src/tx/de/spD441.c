/***************************************************************
* �� �� ��: spD441.c
* ������������������ҵ��������ǩԼ������ձ�
* ��    ��: Ѧ��
* ������ڣ�2011��4��8��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:

insert into tx_sub_def values('D441','������ǩԼ������ձ�','spD441','0','0');


**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "dd_mst_c.h"
#include "mo_prot_ac_rel_c.h"

spD441()
{
   int ret=0;
   char app_ac_no[21];

   struct dd_mst_c  sDd_mst;                /*�������Ż���*/
   struct mdm_ac_rel_c  sMdm_ac_rel;        /*�������˻���ϵ*/
   struct mo_prdt_prot_c  sMo_prdt_prot;    /*����Э��Ǽǲ�*/
   struct mo_prot_ac_rel_c  sMo_prot_ac_rel;/*����Э���˻����ձ�*/

   memset(&sDd_mst,         0x00 , sizeof(struct dd_mst_c));
   memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
   memset(&sMo_prdt_prot,   0x00 , sizeof(struct mo_prdt_prot_c));
   memset(&sMo_prot_ac_rel, 0x00 , sizeof(struct mo_prot_ac_rel_c));
   memset(app_ac_no,0x0,sizeof (app_ac_no));
  
   /*** ��ʼ��ȫ�ֱ��� ***/
   pub_base_sysinit();
   
   /*ȡֵ����ֵ*/
   get_zd_data("0300", g_pub_tx.ac_no);     /*���ʺ�*/
   get_zd_data("1039", app_ac_no);          /*���˺�*/
   
   pub_base_strpack(app_ac_no);
   pub_base_strpack(g_pub_tx.ac_no);
   pub_base_old_acno(g_pub_tx.ac_no); /* �Ծ��ʺŵĴ��� */
   
    /* �����ʺŲ�ѯ�˻���ź��˻�ID */
   ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
   if(ret)
    {
      sprintf(acErrMsg, "��ѯmdm_ac_rel����!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
   strcpy(sMo_prot_ac_rel.tx_br_no,g_pub_tx.tx_br_no);/*�Ǽǻ�������*/
   strcpy(sMo_prot_ac_rel.prot_type,"TK7");           /*Э������*/
   sMo_prot_ac_rel.main_ac_id=sMdm_ac_rel.ac_id;      /*���˺�*/
   sMo_prot_ac_rel.main_ac_seqn=sMdm_ac_rel.ac_seqn;  /*���˺����*/
    
   /* �����ʺŲ�ѯ�����˻���ź��˻�ID */
   memset(&sMdm_ac_rel,     0x00 , sizeof(struct mdm_ac_rel_c));
   ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",app_ac_no);
   if(ret)
    {
      sprintf(acErrMsg, "��ѯmdm_ac_rel����!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
   sMo_prot_ac_rel.app_ac_id=sMdm_ac_rel.ac_id;       /*���˺�*/
   sMo_prot_ac_rel.app_ac_seqn=sMdm_ac_rel.ac_seqn;   /*���˺����*/
   sMo_prot_ac_rel.tx_date=g_pub_tx.tx_date;          /*�Ǽ�����*/
   sMo_prot_ac_rel.trace_no=g_pub_tx.trace_no;        /*������ˮ��*/
   strcpy(sMo_prot_ac_rel.tel,g_pub_tx.tel);          /*�Ǽǹ�Ա*/
   strcpy(sMo_prot_ac_rel.sts,"1");                   /*״̬: 1��������*��ȡ��*/

   Mo_prot_ac_rel_Debug(&sMo_prot_ac_rel);

   ret = Mo_prot_ac_rel_Ins(sMo_prot_ac_rel,g_pub_tx.reply);
   if( ret )
    {  
      sprintf(acErrMsg, "����mo_prot_ac_rel����[%d]!!",ret);
      WRITEMSG
      strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
       
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "7����ǩԼ������ձ�ɹ�![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "7����ǩԼ������ձ�ʧ��![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
    
}
