/***************************************************************
* �� �� ��:   spH807.c
* ����������  ��˰������Э����֤���� 
*
* ��    ��:   mike        
* ������ڣ�  2004��10��18��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define  EXTERN
#include "public.h"
#include "mq_tax_info_c.h"

/* ��˰������Э����֤���� */
int spH807()
{
  FILE *fw , *fr;
  int  ret = 0 , i , j ;
  char file_name[51], tmp_str[61];
  char tmp_item_no[6]; /* ��˰�ʻ���Ŀ */
  char ret_result[6], ret_msg[61]; /* ���ؽ�����뼰��Ϣ */
  char tmp_buf[512];

  struct mq_tax_info_c    sMq_tax_info ;     /*��˰����Ϣ */
  struct dd_mst_c         sDd_mst ;


  /*** ��ʼ��ȫ�ֱ��� ***/
  memset(&sMq_tax_info,0x0,sizeof(sMq_tax_info));

  pub_base_sysinit();

  /* ȡ��־�ļ��� */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 9114-����Э����֤\n");
  fprintf(fw,"  ����<6807> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- ȡ�������  ---------*/
  get_zd_data  ("0380" , sMq_tax_info.taxorg_code);      /* ���ջ��ش��� */
  get_zd_data  ("0630" , sMq_tax_info.taxpay_code);      /* ��˰�˱��   */
  get_zd_data  ("0590" , sMq_tax_info.payopbk_code);     /* ������к� */
  get_zd_data  ("0600" , sMq_tax_info.paybk_code);       /* ���������к� */
  get_zd_data  ("0300" , sMq_tax_info.pay_acct  );       /* �����ʺ�     */
  get_zd_data  ("0962" , sMq_tax_info.handorg_name);     /* �ɿλ���� */
  get_zd_data  ("0963" , sMq_tax_info.protocol_no);      /* Э������   */
  sMq_tax_info.tx_date = g_pub_tx.tx_date ;              /* �Ǽ�����     */
  strcpy(sMq_tax_info.sts , "1" );                       /* ״̬��־     */
  set_zd_data  ("0961" , sMq_tax_info.handorg_name);     /* �ɿλ���� */

  fprintf(fw,"  ���ջ��ش��� :<%s>\n",sMq_tax_info.taxorg_code);
  fprintf(fw,"  ��˰�˱��   :<%s>\n",sMq_tax_info.taxpay_code);
  fprintf(fw,"  ������к� :<%s>\n",sMq_tax_info.payopbk_code);
  fprintf(fw,"  ���������к� :<%s>\n",sMq_tax_info.paybk_code);
  fprintf(fw,"  �ɿλ���� :<%s>\n",sMq_tax_info.handorg_name);
  fprintf(fw,"  �����ʺ�     :<%s>\n",sMq_tax_info.pay_acct);
  fprintf(fw,"  Э������   :<%s>\n",sMq_tax_info.protocol_no);

  pub_base_old_acno( sMq_tax_info.pay_acct ); /* �Ծ��ʺŵĴ��� */

  ret = Dd_mst_Sel(g_pub_tx.reply , &sDd_mst , \
   "ac_id = (select ac_id from mdm_ac_rel where ac_no='%s') ",\
   sMq_tax_info.pay_acct);
  if(g_reply_int){
    sprintf(acErrMsg,"���ò�ѯdd_mstʧ��!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ò�ѯdd_mstʧ��");
    goto ErrExit;
  }

  /* set_zd_data("0580", sDd_mst.opn_br_no); */

  /* �����¼����˰����Ϣ���� */
  ret = Mq_tax_info_Ins(sMq_tax_info , g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"�����˰����˰����Ϣ���!(sql=%d)",ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"�����˰����˰����Ϣ���!(sql=%d)",ret);
    goto ErrExit;
  }


OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"��˰������Э��Ǽǳɹ�![%s]" , g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "0");
  strcpy(ret_msg    , "��˰������Э��Ǽǳɹ�");
  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* ���ı�ʶ�� */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"����<%s><%s>\n",ret_result,ret_msg);

  fclose(fw);

  return 0;

ErrExit:
  sprintf(acErrMsg,"��˰������Э��Ǽ�ʧ��![%s]" , g_pub_tx.reply );
  WRITEMSG
  strcpy(ret_result , "1");
  if(strlen(ret_result)<1){
	sprintf(ret_msg , "��˰������Э��ǼǴ�(%s)", g_pub_tx.reply);
  }
  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"����<%s><%s>\n",ret_result,ret_msg);
  fclose(fw);
  return 1;
}

