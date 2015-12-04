/***************************************************************
* �� �� ��:   spH803.c
* ����������  ��˰����������Ϣ���˶�����   
*
* ��    ��:   mike        
* ������ڣ�  2004��11��24��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define  EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "mq_acct_no_c.h"
#include "mq_tax_info_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "mq_tax_payment_mx_c.h"
#include "rsp_code_c.h"

/* ��˰����������Ϣ���˶����� */
int spH806()
{
  FILE *fr, *fw ;
  int ret = 0, i, j, k, num , n ;
  char file_name[51];
  char tmp_item_no[6];             /* ��˰�ʻ���Ŀ */
  char ret_result[6], ret_msg[61], rq_msg[7]; /* ���ؽ�����뼰��Ϣ */
  char f_name[31], buf[256], tmp_str[81];

  struct mq_acct_no_c         sMq_acct_no  ;        /*�ʻ����ձ�  */
  struct mq_tax_head_c        sMq_tax_head ;        /*��˰��Ϣ��ͷ*/
  struct mq_tax_info_c        sMq_tax_info ;        /*������Ϣ��*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*��˰��Ϣ����*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*˰����Ŀ��Ϣ����*/
  struct dd_mst_c             sDd_mst ;             /* ���ڴ��   */

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));
  memset(ret_result , 0x00, sizeof(ret_result));
  memset(ret_msg    , 0x00, sizeof(ret_msg   ));

  /*** ��ʼ��ȫ�ֱ��� ***/
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
  pub_base_sysinit();

  /* ȡ��־�ļ��� */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 3112-��������Ϣ���˶�\n");
  fprintf(fw,"  ����<6806> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- ȡ�������  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* ����ڵ���� */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* ���ı�ʶ�� */

  fprintf(fw,"  �ڵ����  : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  ���ı�ʶ  : <%s>\n", sMq_tax_head.msg_id);

  get_zd_data ("0964", f_name ); 
  sprintf(file_name,"/home/zzccb/txt/%s",f_name);

  fprintf(fw,"  �����ļ���: <%s>\n", file_name);

  fr=fopen(file_name,"r");
  if(fr==NULL||strlen(file_name)<17){
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"���ļ�%sʧ��!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ļ�(%s)ʧ��", file_name);
    goto ErrExit;
  }

  /* ���ļ��� 1 �� */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"���ļ�%s��Ϣͷʧ��!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ļ�(%s)��Ϣͷʧ��", file_name);
    goto ErrExit;
  }
sprintf(acErrMsg,"line1<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.taxorg_code , tmp_str); /* �����к� */
  set_zd_data("0600", tmp_str);
  fprintf(fw,"  �����к�      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* �������� */
  set_zd_data("0860", tmp_str);
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  /* ����ˮ�� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  ����ˮ��      : <%s>\n", tmp_str);

  /* �Ӱ����� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  �Ӱ�����      : <%s>\n", tmp_str);

  /* �Ӱ���� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  �������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  num = atoi(tmp_str) ;        /* �������� */
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* ������� */
  fprintf(fw,"  �������      : <%s>\n", tmp_str);

  /* ���Ͱ��� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  ���Ͱ���      : <%s>\n", tmp_str);

  /* ���հ��� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  ���հ���      : <%s>\n", tmp_str);

  /* ֹ������ */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  ֹ������      : <%s>\n", tmp_str);

  /* �������� */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  /* ���ļ��� 2-n �� */
  for(n=0; n < num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"���ļ�%s������Ϣʧ��!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"���ļ�(%s)������Ϣʧ��", file_name);
      goto ErrExit;
    }

    fprintf(fw,"--- ��¼��: %d --- \n", n+1);
    /* ԭ���ı�� */
    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ԭ���ı��    : <%s>\n", tmp_str);

    /* ԭ������� */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ԭ�������    : <%s>\n", tmp_str);

    /* ԭί������ */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ԭί������    : <%s>\n", tmp_str);

    /* ԭ����ˮ�� */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ԭ����ˮ��    : <%s>\n", tmp_str);

    /* �ܱ���     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  �ܱ���        : <%s>\n", tmp_str);

    /* �ܽ��     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  �ܽ��        : <%s>\n", tmp_str);

    /* �ɹ�����     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  �ɹ�����      : <%s>\n", tmp_str);

    /* �ɹ����     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  �ɹ����      : <%s>\n", tmp_str);

    /* ֹ������     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ֹ������      : <%s>\n", tmp_str);

    /* ������       */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  ������        : <%s>\n", tmp_str);
    fprintf(fw,"------------------ \n");
  }

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"��˰��<%s>���ʳɹ�![%s]",rq_msg,g_pub_tx.reply);
  WRITEMSG
  strcpy(ret_result , "90000");
  sprintf(ret_msg , "<%s>���ʳɹ�", rq_msg);

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fclose(fr);

  fprintf(fw,"  ���ؽ��: ���˶���ȷ\n");
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"��˰��<%s>����ʧ��![%s]", rq_msg,g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "(%s)���ʴ�(%s)", rq_msg,g_pub_tx.reply);
  }
  else{
	sprintf(tmp_str ,"(%s)����%s", rq_msg,ret_result);
	strcpy(ret_result, tmp_str);
  }

  sprintf(acErrMsg,"mike:(%s)(%s)",ret_result, ret_msg);
  WRITEMSG

  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"  ���ؽ��: ���˶Դ���(%s)\n", ret_msg);
  fclose(fr);
  fclose(fw);
  return 1;
}
