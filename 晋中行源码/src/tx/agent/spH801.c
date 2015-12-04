/***************************************************************
* �� �� ��:   spH801.c
* ����������  ��˰��ʵʱ�ۻ�˰��(ת��3001)
*
* ��    ��:   mike        
* ������ڣ�  2004��10��05��
* �޸ģ���˰����ϸ 20041116
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define  EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "mq_acct_no_c.h"
#include "mq_tax_info_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "mq_tax_payment_mx_c.h"

/* ��˰��ʵʱ�ۻ�˰�� */
int spH801()
{
  FILE *fr, *fw ;
  int ret = 0, i, j, k, num;
  char file_name[51];
  char tmp_item_no[6]; /* ��˰�ʻ���Ŀ */
  char ret_result[6], ret_msg[81]; /* ���ؽ�����뼰��Ϣ */
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
  sprintf(file_name,"/home/dhccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 3001-ʵʱ��˰����\n");
  fprintf(fw,"  ����<6801> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- ȡ�������  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* ����ڵ���� */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* ���ı�ʶ�� */

  fprintf(fw,"  �ڵ����  : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  ���ı�ʶ  : <%s>\n", sMq_tax_head.msg_id);

 
  memset(tmp_str,0x00, sizeof(tmp_str));
  get_zd_data ("0964", tmp_str ); 
  sprintf(file_name,"/home/dhccb/txt/%s",tmp_str);

  fprintf(fw,"  �����ļ���: <%s>\n", file_name);

  fr=fopen(file_name,"r");
  if(fr==NULL){
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
  strcpy(sMq_tax_head.taxorg_code , tmp_str); /* ���ջ��ش��� */
  strcpy(sMq_tax_payment.taxorg_code , tmp_str); /* ���ջ��ش��� */
  set_zd_data ("0380", tmp_str); 
  fprintf(fw,"  ���ջ��ش���  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* ί������     */
  sMq_tax_payment.entrust_date = atol(tmp_str);  /* ί������     */
  set_zd_data ("0440", tmp_str); 
  fprintf(fw,"  ί������      : <%s>\n", tmp_str);

  sprintf(tmp_str, "%ld", g_pub_tx.tx_date);
  set_zd_data ("0450", tmp_str); 

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.pack_no , tmp_str );     /* ����ˮ��     */
  strcpy(sMq_tax_payment.pack_no , tmp_str );   /* ��ˮ��     */
  strcpy(sMq_tax_payment.tra_no , tmp_str );   /* ��ˮ��     */
  set_zd_data ("0860", tmp_str); 
  fprintf(fw,"  ��ˮ��        : <%s>\n", tmp_str);

  sMq_tax_head.all_num = 1 ;                             /* �ܱ��� */

  /* ���ļ��� 2 �� */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"���ļ�%sʧ��ת����Ϣ!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ļ�(%s)ת����Ϣʧ��", file_name);
    goto ErrExit;
  }

sprintf(acErrMsg,"line2<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>1) j=1;  tmp_str[j]=0;
  strcpy(sMq_tax_head.handle_type, tmp_str);      /* �������    */
  fprintf(fw,"  �������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_bankno, tmp_str);     /* �տ����к�  */
  fprintf(fw,"  �տ����к�    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payeeorg_code, tmp_str);    /* �տλ������*/
  fprintf(fw,"  �տλ������: <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>32) j=32; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_acct, tmp_str);       /* �տ����ʺš�*/
  fprintf(fw,"  �տ����ʺ�    : <%s>\n", tmp_str);


  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_name, tmp_str);       /* �տ�������  */
  fprintf(fw,"  �տ����ʺ�    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy( sMq_tax_head.paybk_code, tmp_str);       /* ���������к� */
  fprintf(fw,"  ���������к�  : <%s>\n", tmp_str);

  sMq_tax_head.return_term = 0 ;                         /* ��ִ�������� */
  sMq_tax_head.succ_num = 1;
  strcpy(sMq_tax_head.result , "90000");                 /* ������     */
  strcpy(sMq_tax_head.addword, "ok  ");                  /* ����   */
  strcpy(sMq_tax_head.send_sts , "1" );                  /* ��ִ��־   */
  strcpy(sMq_tax_head.sts , "2" );                       /* ��״̬��־   */
  sMq_tax_head.stop_num = 0;

sprintf(acErrMsg,"����ˮ��0860:<%s>", sMq_tax_payment.tra_no); WRITEMSG

  /* �ж��տ��ʺŶ�Ӧ���ڲ��ʺ�  */
  ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
    "payee_acct= '%s' ", sMq_tax_head.payee_acct );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"���Ҳ����ʺ����ڲ��ʺŶ�Ӧ��[%d]!", ret);
	WRITEMSG
    sprintf(ret_msg ,"���Ҳ����ʺ����ڲ��ʺŶ�Ӧ��(%d)", ret);
    strcpy(ret_result,"94054");
/* goto ErrExit; */
/* will delete */
strcpy(sMq_acct_no.ac_no,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));
  }

  /* ���ļ��� 3 �� */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"���ļ�%s������Ϣʧ��!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ļ�(%s)������Ϣʧ��", file_name);
    goto ErrExit;
  }

sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>24) j=24; tmp_str[j]=0;
  strcpy(sMq_tax_payment.pay_acct, tmp_str);      /* �����ʺ�     */

  pub_base_old_acno( sMq_tax_payment.pay_acct );  /* �Ծ��ʺŵĴ��� */
  fprintf(fw,"  �����ʺ�      : <%s><%s>\n",tmp_str,sMq_tax_payment.pay_acct);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.handorg_name, tmp_str);  /* �ɿλ���� */
  fprintf(fw,"  �ɿλ����  : <%s>\n",tmp_str);

/* will delete */
/*
strcpy(sMq_tax_payment.pay_acct,"1000010700010");
strcpy(sMq_tax_payment.handorg_name,"Ѧʿ��");
*/

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_payment.tra_amt = atof(tmp_str);             /* Ӧ�۽�� */
  sMq_tax_head.all_amt    = sMq_tax_payment.tra_amt;   /* �ܽ�� */
  sMq_tax_head.succ_amt   = sMq_tax_payment.tra_amt;   /* �ɹ����     */
  fprintf(fw,"  Ӧ�۽��      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>18) j=18; tmp_str[j]=0;
  strcpy(sMq_tax_payment.taxvou_no, tmp_str);     /* ˰Ʊ����     */
  set_zd_data ("0370", tmp_str); 
  fprintf(fw,"  ˰Ʊ����      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.protocol_no, tmp_str);   /* Э�����     */
  fprintf(fw,"  Э�����      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.remark , tmp_str);       /* ��ע         */
  fprintf(fw,"  ��ע          : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  num = atoi(tmp_str);
  sMq_tax_payment.tax_num = num; /* ˰����Ŀ */
  fprintf(fw,"  ˰����Ŀ: <%s>\n",tmp_str);

  strcpy(sMq_tax_payment_mx.taxorg_code, sMq_tax_payment.taxorg_code);
  strcpy(sMq_tax_payment_mx.tra_no, sMq_tax_payment.tra_no);
  sMq_tax_payment_mx.entrust_date = sMq_tax_payment.entrust_date;

  for(k=0; k<num; ++k){
    /* ���ļ��� 4 - 4+num �� */
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"���ļ�˰����Ŀ%sʧ��(%d)!", file_name, k+1);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"���ļ�(%s)˰����Ŀ(%d)ʧ��", file_name, k+1);
      goto ErrExit;
    }
    fprintf(fw,"    %d=%s", k+1,buf);
    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_tax_payment_mx.tax_num = atoi(tmp_str); /* ��Ŀ��� */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>60) j=60; tmp_str[j]=0;
	strcpy(sMq_tax_payment_mx.tax_name, tmp_str);   /* ˰������ */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>8) j=8; tmp_str[j]=0;
	sMq_tax_payment_mx.beg_date = atol(tmp_str); /* ��ʼ���� */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
	if(j>8) j=8; tmp_str[j]=0;
	sMq_tax_payment_mx.end_date = atol(tmp_str); /* ��ֹ���� */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
	sMq_tax_payment_mx.tx_amt = atol(tmp_str);  /* ������ */

    /* �����¼��ʵʱ��˰��Ŀ�� */
    ret = Mq_tax_payment_mx_Ins(sMq_tax_payment_mx, g_pub_tx.reply);
    if (ret != 0)
    {
  	  strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"�����˰��˰����ϸ��Ŀ��!(sql=%d)",ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"�����˰��˰����ϸ��Ŀ��!(sql=%d)",ret);
      goto ErrExit;
    }
  }

  sMq_tax_payment.tax_date = g_pub_tx.tx_date ;          /* ��˰����     */
  sMq_tax_payment.tx_amt   = sMq_tax_payment.tra_amt ;   /* ʵ�۽��     */
  sMq_tax_payment.seq_no   = g_pub_tx.trace_no ;         /* �ۿ���ˮ     */
  strcpy(sMq_tax_payment.result , "90000");              /* ������     */
  strcpy(sMq_tax_payment.addword , "0000");              /* ����   */

  sMq_tax_payment.chk_date  = 0 ;                 /* ��������     */
  strcpy(sMq_tax_payment.chk_ord , "" );       /* ��������     */
  strcpy(sMq_tax_payment.sts , "2" ); /* ��־0-δ�� 1-����� 2-�ѿۿ� 3-�Ѷ���*/

  g_reply_int = Dd_mst_Sel(g_pub_tx.reply , &sDd_mst , \
   "ac_id = (select ac_id from mdm_ac_rel where ac_no='%s') ",\
   sMq_tax_payment.pay_acct);
  if(g_reply_int){
    sprintf(acErrMsg,"���ò�ѯdd_mstʧ��!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
	memset(ret_msg, 0x00, sizeof(ret_msg));
	if(g_reply_int == 100){
      sprintf(ret_msg ,"�޴˸����ʺ�(%s)", sMq_tax_payment.pay_acct);
	}
	else{
      sprintf(ret_msg ,"��ѯ�����ʺ�(%d)", g_reply_int);
	}
    goto ErrExit;
  }

  g_reply_int = diff_custname(sDd_mst.name, sMq_tax_payment.handorg_name) ;
  if(g_reply_int){
	strcpy(g_pub_tx.reply , "D101");
    strcpy (ret_result , "24002" ); 
    sprintf(ret_msg ,"�ʺŻ�������%s(%s,%s)",sMq_tax_payment.pay_acct, 
	  sDd_mst.name, sMq_tax_payment.handorg_name);
    sprintf(acErrMsg ,"�ʺŻ�������%s(%s,%s)",sMq_tax_payment.pay_acct, 
	  sDd_mst.name, sMq_tax_payment.handorg_name);
    WRITEMSG
    goto ErrExit;
  }
  if(sDd_mst.ac_sts[0]!='1' || sDd_mst.hold_sts[0]=='2' ||
  sDd_mst.hold_sts[0]=='3' ){
	strcpy(g_pub_tx.reply , "D101");
    strcpy (ret_result , "24002" ); 
    sprintf(ret_msg ,"�����ʻ�״̬��");
    goto ErrExit;
  }

  /* �����ʽṹ��ֵ, ���ù��ü��ʺ���,ȡ�� */
  strcpy(g_pub_tx.ac_no, sMq_tax_payment.pay_acct );  /* �ʺ� */
  strcpy(g_pub_tx.cur_no, "01" );                     /* ���� */
  g_pub_tx.ac_seqn = 1;
  pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn );
  strcpy(g_pub_tx.brf, "��˰��" ); /* ժҪ */
  strcpy(g_pub_tx.add_ind , "0" ); /* ������־ 0-���� 1-���� */
  strcpy(g_pub_tx.hst_ind , "1" ); /* �ռ�����ϸ */
  strcpy(g_pub_tx.ct_ind  , "2" ); /* ��ת��־ 1-�ֽ� 2-ת�� */
  strcpy(g_pub_tx.prt_ind , "0" ); /* �����۱�־ */
  g_pub_tx.tx_amt1  = sMq_tax_payment.tra_amt ;  /* ��˰���     */

  /* ���ƾ֤���㲻����ˮ���㲻����ϸ�����֧ȡ��ʽ�� */
  strcpy(g_pub_tx.ac_wrk_ind,"0000000011"); 
  g_pub_tx.svc_ind=1001;
  
  g_reply_int = pub_acct_trance();
  if( g_reply_int )
  {
    sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
    WRITEMSG

    if(memcmp(g_pub_tx.reply,"P101",4)==0||memcmp(g_pub_tx.reply,"P102",4)==0){
      strcpy(ret_result , "24001" ); /* �ʺŲ����� */
      sprintf(ret_msg ,"�������ʺ�(%s)������", g_pub_tx.ac_no);
    }
    else if(memcmp(g_pub_tx.reply,"A022",4)==0){
      strcpy(ret_result , "24003" ); 
      sprintf(ret_msg ,"�������ʻ�����(%.2f),%d", sDd_mst.bal, sDd_mst.ac_id);
    }

    goto ErrExit;
  }

  /* �����־: 1-�� 2-�� */
  set_zd_data("0700", "1");
  g_reply_int = pubf_acct_proc("H801");
  if( g_reply_int )
  {
    sprintf(acErrMsg,"���û�Ƽ�������ʧ��!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���û�Ƽ�������ʧ��!");
    goto ErrExit;
  }


  /*** ��ʼ��ȫ�ֱ��� ***/
  memset(&g_mdm_ac_rel , 0x00 , sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst     , 0x00 , sizeof(struct dd_mst_c));
  memset(&g_dd_parm    , 0x00 , sizeof(struct dd_parm_c));
  pub_base_sysinit();

  /* �����ʽṹ��ֵ, ���ù��ü��ʺ���,��� */
  strcpy(g_pub_tx.ac_no, sMq_acct_no.ac_no );         /* �ڲ��ʺ� */
  strcpy(g_pub_tx.cur_no, "01" );                     /* ����     */
  g_pub_tx.ac_seqn = 1;
  pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn );

  strcpy( g_pub_tx.brf , "��˰��" );
  strcpy(g_pub_tx.add_ind , "1" ); /* ������־ 0-���� 1-���� */
  strcpy(g_pub_tx.hst_ind , "1" ); /* �ռ�����ϸ */
  strcpy(g_pub_tx.ct_ind  , "2" ); /* ת�� */
  strcpy(g_pub_tx.prt_ind , "0" ); /* �����۱�־*/
  g_pub_tx.tx_amt1  = sMq_tax_payment.tra_amt ;  /* ��˰���     */

  /**���ƾ֤���㲻����ˮ���㲻����ϸ�����֧ȡ��ʽ**/
  strcpy(g_pub_tx.ac_wrk_ind,"0000000011"); 
  g_pub_tx.svc_ind=1001;
  
  /* д������ˮ,�޸����ļ�,дδ������ϸ�� */
  g_reply_int=pub_acct_trance();
  if( g_reply_int )
  {
    sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���ô�ȡ������ʧ��!");
    goto ErrExit;
  }

  /* ��ֵ, �����ؼ��ʵ��� */
  /* �����־: 1-�� 2-�� */
  set_zd_data("0700" , "2");
  g_reply_int = pubf_acct_proc("H801");
  if( g_reply_int )
  {
    sprintf(acErrMsg,"���û�Ƽ�������ʧ��!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"���û�Ƽ�������ʧ��!");
    goto ErrExit;
  }

  /* �����¼��ʵʱ��˰��ͷ�� */
  ret = Mq_tax_head_Ins(sMq_tax_head,g_pub_tx.reply);
  if (ret != 0)
  {
	strcpy(g_pub_tx.reply , "D101");
    sprintf(acErrMsg,"�����˰��˰��ͷ���![%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"�����˰��˰��ͷ���![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  /* �����¼��ʵʱ��˰����ϸ�� */
  ret = Mq_tax_payment_Ins(sMq_tax_payment,g_pub_tx.reply);
  if (ret != 0)
  {
	strcpy(g_pub_tx.reply , "D101");
    sprintf(acErrMsg,"�����˰��˰����ϸ���!(sql=%d)", ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"�����˰��˰����ϸ���!(sql=%d)", ret);
    goto ErrExit;
  }

OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"��˰��ʵʱ��˰�ɹ�![%s]",g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "ʵʱ��˰�ɹ�");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  sprintf(acErrMsg, " ---[%s]", tmp_str);
  WRITEMSG
  set_zd_data("0620" , tmp_str );    /* ���ı�ʶ�� */

  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  ���ؽ��: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"��˰��ʵʱ��˰ʧ��![%s]",g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "ʵʱ��˰��(%s)", g_pub_tx.reply);
  }

  sprintf(acErrMsg,"mike:<%s><%s>",ret_result, ret_msg);
  WRITEMSG

  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  ���ؽ��: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);
  return 1;
}

/* �ȽϿͻ�����(ȥ�������еĿո�) */
int diff_custname(str_1, str_2)
char * str_1, *str_2;
{
  int i,j,k,len;
  char str_3[81], str_4[81];
  len=strlen(str_1);
  if(len>80) len=80;
  i=j=0;
  while(i<len){
    if(str_1[i]!=' ') str_3[j++]=str_1[i]; i++;
  }
  str_3[j]=0;
  len=strlen(str_2);
  if(len>80) len=80;
  i=k=0;
  while(i<len){
    if(str_2[i]!=' ') str_4[k++] = str_2[i]; i++;
  }
  str_4[k]=0;
  if( j != k ) return(1);
  if(strcmp(str_3, str_4) ==0 ) return(0) ;
  else return(1);
}
