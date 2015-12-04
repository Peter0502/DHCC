/**************************************************
�汾�ţ�    V1.0
���ڣ�      20111024
�������ߣ�   shangyongchao
�������ƣ�  spF249.c
�����ܣ�  ��ƽ̨�����˺š��ͻ�ƾ֤�š��ͻ�ƾ֤���࣬�Կͻ���ƾ֤���м��
            �ɹ����� 0000 ʧ�ܷ�����Ӧ������
�������ã�
  insert into tx_def values('6249','�ͻ�ƾ֤���','10000000000000000000000010000000000000000000000000000000000000000011100000000000000000000001000000000000000000000000000000000100','0','6','0','3',null);
  insert into tx_flow_def  values ('6249', 0, 'F249', '#$');
  insert into tx_sub_def  values ('F249', '�ͻ�ƾ֤���', 'spF249', '0', '0');
**************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"

int spF249()
{
    int ret = 0;
    char cNote_sts[2];/*ƾ֤״̬*/
    char cNote_type[4];/*ƾ֤����*/
    char cNote_no[17];   /*ƾ֤��*/
    char cAc_no[25];   /*�˺�*/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct dd_mst_c sDd_mst;
    
    memset(cNote_sts,0x00,sizeof(cNote_sts));
    memset(cNote_type,0x00,sizeof(cNote_type));
    memset(cNote_no,0x00,sizeof(cNote_no));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    
    /*��ʼ������*/
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    /*ƽ̨ȡֵ*/
    get_zd_data("0300",cAc_no);
    get_zd_data("0900",cNote_type);
    get_zd_data("0600",cNote_no);
    vtcp_log("[%s][%d],cAc_no = [%s],cNote_type =[%s],cNote_no=[%s]",__FILE__,__LINE__,cAc_no,cNote_type,cNote_no);
    
    pub_base_old_acno(cAc_no);  /** �¾��˺�ת�� **/
    zip_space(cAc_no);
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",cAc_no);
    if(ret)
    {
        sprintf(acErrMsg,"��ѯmdm_ac_relʧ�ܣ���");
        strcpy(g_pub_tx.reply,"W011");
        WRITEMSG
        goto ErrExit;
    }
    ret = pub_com_ChkAcCheq( sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn,cNote_type, cNote_no, cNote_no, cNote_sts );
    if(ret)
    {
        sprintf(acErrMsg,"��ѯ�ͻ�ƾ֤����");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N080");
        goto ErrExit;
    }
    vtcp_log("%s,%d,cNote_sts=[%s]",__FILE__,__LINE__,cNote_sts);
    /*����ƾ֤�Ĳ�ͬ״̬���ز�ͬ������*/
    if ( cNote_sts[0] == '0' )
    {
        sprintf( acErrMsg, "�ͻ��޴�ƾ֤!cNote_sts=[%s]", cNote_sts);
        WRITEMSG
        strcpy( g_pub_tx.reply, "N080" );
        goto ErrExit;
    }
    else if ( cNote_sts[0] == '2' )
    {
        sprintf( acErrMsg, "��ƾ֤�ѹ�ʧ!cNote_sts=[%s]", cNote_sts);
        WRITEMSG
        strcpy( g_pub_tx.reply, "N081" );
        goto ErrExit;
    } 
    else if ( cNote_sts[0] == '3' )
    {   
        sprintf( acErrMsg, "��ƾ֤�ѻ���!cNote_sts=[%s]", cNote_sts);
        WRITEMSG
        strcpy( g_pub_tx.reply, "N082" );
        goto ErrExit;
    } 
    else if ( cNote_sts[0] == '4' )
    {
        sprintf( acErrMsg, "��ƾ֤������!cNote_sts=[%s]", cNote_sts);
        WRITEMSG
        strcpy( g_pub_tx.reply, "N083" );
        goto ErrExit;
    }
     else if ( cNote_sts[0] == '5' )
    {
        sprintf( acErrMsg, "��ƾ֤��ʹ��!cNote_sts=[%s]", cNote_sts);
        WRITEMSG
        strcpy( g_pub_tx.reply, "N084" );
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
