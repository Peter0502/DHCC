/***************************************************************
* �� �� ��:   spH803.c
* ����������  ��˰�������ۻ�����   
*
* ��    ��:   
* ������ڣ�  2005��10��05��
* �޸ģ���˰����ϸ 20051116
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
char file_name[51];
FILE *fw, *fw1;
char ret_result[6], ret_msg[61]; /* ���ؽ�����뼰��Ϣ */

/* ��˰�������ۻ����� */
int spH803()
{
  FILE *fr ;
  int ret = 0, i, j, k, num,n;
  char tmp_item_no[6];             /* ��˰�ʻ���Ŀ */
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
  fprintf(fw,"begin 3102-������˰����\n");
  fprintf(fw,"  ����<6803> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
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
  strcpy(sMq_tax_payment_mx.taxorg_code , tmp_str); /* ���ջ��ش��� */
  set_zd_data ("0380", tmp_str); 
  fprintf(fw,"  ���ջ��ش���  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* ί������     */
  sMq_tax_payment.entrust_date = atol(tmp_str);  /* ί������     */
  sMq_tax_payment_mx.entrust_date = atol(tmp_str);  /* ί������     */
  set_zd_data ("0440", tmp_str); 
  fprintf(fw,"  ί������      : <%s>\n", tmp_str);

  sprintf(tmp_str, "%ld", g_pub_tx.tx_date);
  set_zd_data ("0450", tmp_str); 

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.pack_no , tmp_str );     /* ����ˮ��     */
  strcpy(sMq_tax_payment.pack_no , tmp_str );  /* ����ˮ��     */
  set_zd_data ("0860", tmp_str); 
  fprintf(fw,"  ����ˮ��      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_num = atoi(tmp_str) ;        /* �ܱ��� */
  fprintf(fw,"  �ܱ���        : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* �ܽ�� */
  fprintf(fw,"  �ܽ��        : <%s>\n", tmp_str);

  sMq_tax_head.succ_amt   = 0.0 ;   /* �ɹ����  */
  sMq_tax_head.stop_num   = 0 ;     /* ֹ������  */

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
  if(j>1) j=1; tmp_str[j]=0;
  strcpy(sMq_tax_head.handle_type, tmp_str); /* �������    */
  fprintf(fw,"  �������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_bankno, tmp_str); /* �տ����к�  */
  fprintf(fw,"  �տ������к�  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payeeorg_code, tmp_str); /* �տλ������*/
  fprintf(fw,"  �տλ������: <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>32) j=32; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_acct, tmp_str); /* �տ����ʺš�*/
  fprintf(fw,"  �տ����ʺ�    : <%s>\n", tmp_str);


  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_name, tmp_str);  /* �տ�������  */
  fprintf(fw,"  �տ�������    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy( sMq_tax_head.paybk_code, tmp_str); /* ���������к� */
  fprintf(fw,"  ���������к�  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.return_term = atoi(tmp_str) ; /* ��ִ�������� */
  fprintf(fw,"  ��ִ��������  : <%s>\n", tmp_str);

  sMq_tax_head.succ_num = 0;
  strcpy(sMq_tax_head.result , "90000");     /* ������  */
  strcpy(sMq_tax_head.addword ,"ok  ");      /* ���Դ���  */
  strcpy(sMq_tax_head.send_sts , "1" );      /* ��ִ��־  */
  strcpy(sMq_tax_head.sts , "2" );           /* ��״̬��־*/


  /* �ж��տ��ʺŶ�Ӧ���ڲ��ʺ�  */
  ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
    "payee_acct= '%s' ", sMq_tax_head.payee_acct );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"���Ҳ����ʺ�(%s)���ڲ��ʺŶ�Ӧ��[%d]!",\
	sMq_tax_head.payee_acct,ret);
	WRITEMSG
    sprintf(ret_msg ,"���Ҳ����ʺ����ڲ��ʺŶ�Ӧ��(%d)", ret);
    strcpy(ret_result,"94054");
/* goto ErrExit; */
/* will delete */
strcpy(sMq_acct_no.ac_no,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));
  }

  /* ���ļ��� 3 �� */
  for(n=0; n < sMq_tax_head.all_num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"���ļ�%s������Ϣʧ��!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"���ļ�(%s)������Ϣʧ��", file_name);
      goto ErrExit;
    }

    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_tax_payment.tra_no , tmp_str);              /* ������ˮ��  */
    fprintf(fw,"  ������ˮ��    : <%s>\n",tmp_str);

    sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>24) j=24; tmp_str[j]=0;
    strcpy(sMq_tax_payment.pay_acct, tmp_str);      /* �����ʺ�     */
    pub_base_old_acno( sMq_tax_payment.pay_acct ); /* �Ծ��ʺŵĴ��� */
    fprintf(fw,"  �����ʺ�      : <%s><%s>\n",tmp_str,sMq_tax_payment.pay_acct);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>60) j=60; tmp_str[j]=0;
    strcpy(sMq_tax_payment.handorg_name, tmp_str);  /* �ɿλ���� */
    fprintf(fw,"  �ɿλ����  : <%s>\n",tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_tax_payment.tra_amt = atof(tmp_str);             /* Ӧ�۽�� */
    fprintf(fw,"  �ۿ���      : <%s>\n",tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>18) j=18; tmp_str[j]=0;
    strcpy(sMq_tax_payment.taxvou_no, tmp_str);     /* ˰Ʊ����     */
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
    fprintf(fw,"  ˰����Ŀ      : <%s>\n",tmp_str);

    strcpy(sMq_tax_payment_mx.tra_no, sMq_tax_payment.tra_no);

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

    sMq_tax_payment.tax_date = g_pub_tx.tx_date ;  /* ��˰����     */
    sMq_tax_payment.tx_amt   = 0.0    ;            /* ʵ�۽��     */
    sMq_tax_payment.seq_no   = 0      ;            /* �ۿ���ˮ     */
    strcpy(sMq_tax_payment.result , "NNNNN");      /* ������     */
    strcpy(sMq_tax_payment.addword , "");          /* ���Դ���     */

    sMq_tax_payment.chk_date  = 0 ;                /* ��������     */
    strcpy(sMq_tax_payment.chk_ord , "" );      /* ��������     */
    strcpy(sMq_tax_payment.sts,"0" ); /*��־0-δ��1-�����2-�ѿۿ� 3-�Ѷ���*/

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

  ret = sql_commit();
  if(ret){
    sprintf(acErrMsg,"��˰�����������ύʧ��(%d)",ret); WRITEMSG
	goto ErrExit;
  }
  sprintf(acErrMsg,"��˰�����������ύ�ɹ�"); WRITEMSG
  fprintf(fw,"  ���������¼�ɹ�!\n");

  sql_begin();
  ret = spH803_kk() ;

  if(ret) goto ErrExit;

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"��˰�������ɹ�![%s]",g_pub_tx.reply);
  WRITEMSG
  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "������˰�ɹ�");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
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
  sprintf(acErrMsg,"��˰����������ʧ��![%s]",g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "���������(%s)", g_pub_tx.reply);
  }

  sprintf(acErrMsg,"sgq:<%s><%s>",ret_result, ret_msg);
  WRITEMSG

  set_zd_data  ("0620" , "no" );    /* ���ı�ʶ�� */
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


/* ������˰ */
int spH803_kk()
{
  int ret = 0;
  int ret1 = 0;
  char cReturn[51];
  char execstr[248];
  
  int iErr_flag = 0;   /*  0��������ȷ��1������ʧ��  */
  int iErr_flag1 = 0;  /*  0��������ȷ��1������ʧ��  */
  
  long cif_no = 0;
  char xs_acno[20];
  char jz_acno[20];
  int jz_ac_seq = 0;
  
  long lSnum = 0;    /*�ɹ�����*/
  long lFnum = 0;    /*ʧ�ܱ���*/
  double dSamt = 0.00;  /*�ɹ����*/
  double dFamt = 0.00;  /*ʧ�ܽ��*/

  int i=0;

  char ret_result[6], ret_msg[61]; /* ���ؽ�����뼰��Ϣ */
  char tmp_str[81], f_name[51];
  

  struct com_branch_c         sCom_branch;
  struct mdm_ac_rel_c         sMdm_ac_rel;         
  struct mq_acct_no_c         sMq_acct_no  ;        /*�ʻ����ձ�  */
  struct mq_tax_head_c        sMq_tax_head ;        /*��˰��Ϣ��ͷ*/
  struct mq_tax_info_c        sMq_tax_info ;        /*������Ϣ��*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*��˰��Ϣ����*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*˰����Ŀ��Ϣ����*/
  struct dd_mst_c             sDd_mst ;             /* ���ڴ��   */
  struct rsp_code_c           sRsp_code ;           /*  */

  /**------- ��ʼ��ȫ�ֱ��� --------**/
  pub_base_sysinit();
  
  /*-------- ȡ�������  ---------*/
  memset( &sMq_tax_head, 0x0, sizeof(sMq_tax_head) );
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* ����ڵ���� */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* ���ı�ʶ�� */

  sprintf(f_name,"/home/zzccb/tips_hz.txt");
  fw1 = fopen (f_name,"w");

  fprintf(fw,"  ������˰��ʼ\n");

  ret = Mq_tax_head_Sel( g_pub_tx.reply, &sMq_tax_head, \
    "src = '%s' and msg_id = '%s' ",\
	sMq_tax_head.src, sMq_tax_head.msg_id);
  if ( ret )
  {
    sprintf(acErrMsg,"���� mq_tax_head ����(%s,%s,%s)[%d]",
	sMq_tax_head.taxorg_code, sMq_tax_head.msg_id, sMq_tax_head.pack_no,ret );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
  }

  /*------------ �������ļ� -----------*/
  
  ret = Mq_tax_payment_Dec_Upd_hold( g_pub_tx.reply,
    "taxorg_code= '%s' and entrust_date = %d and pack_no='%s'",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date,sMq_tax_head.pack_no);
  if ( ret )
  {
    sprintf(acErrMsg,"ִ��Mq_tax_payment_Dec_Upd_hold����[%d]", ret );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
  }

  while(1)
  {
    
    iErr_flag = 0;
    
    memset( cReturn, 0x0, sizeof(cReturn) );
    
    memset( &sMq_tax_payment, 0x0, sizeof(sMq_tax_payment) );
    ret = Mq_tax_payment_Fet_Upd_hold( &sMq_tax_payment , g_pub_tx.reply );
fprintf(fw," sql=%d %s\n", ret, sMq_tax_payment.tra_no);
    if ( ret == 100 )
    {
      sql_begin();
      break;
    }
    else if ( ret )
    {
      sprintf(acErrMsg,"ִ��Mq_tax_payment_Fet_Sel����[%d]", ret );
      WRITEMSG
      goto ErrExit_kk;
    }
    
    /**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
    memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
    pub_base_sysinit();    
  
    fprintf(fw,"    ��˰:<%s><%s>",sMq_tax_payment.pay_acct,\
	sMq_tax_payment.tra_no);

    memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
    ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
        	"ac_no = '%s'", sMq_tax_payment.pay_acct );
    if ( ret1 )
    {
   	  sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sMq_tax_payment.pay_acct );
   	  WRITEMSG
   	  iErr_flag = 1;
      strcpy(sMq_tax_payment.result , "24002");     /* ������     */
      if(ret1 == 100) strcpy(sMq_tax_payment.addword,"�ʺŲ�����"); /* ����*/
      else strcpy(sMq_tax_payment.addword , "�����ʺŴ�");  /* ����  */
      goto TrLog;
    }
        
    memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
    ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
      	"ac_id = %ld", sMdm_ac_rel.ac_id );
    if ( ret1 )
    {
      sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", sMq_tax_payment.pay_acct );
      WRITEMSG
      iErr_flag = 1;
      strcpy(sMq_tax_payment.result , "24002");     /* ������     */
      if(ret1 == 100) strcpy(sMq_tax_payment.addword,"�ʺŲ�����"); /* ����*/
      else strcpy(sMq_tax_payment.addword , "�����ʺŴ�");  /* ����  */
      goto TrLog;
    }
        
    pub_base_strpack( sDd_mst.ac_sts );
    if( sDd_mst.ac_sts[0] != '1'||sDd_mst.hold_sts[0]=='1')/*��������ȫ����*/
    {
       	strcpy( cReturn, "�ʻ�״̬��" );
       	sprintf(acErrMsg,"�ʻ�״̬��");
       	WRITEMSG
      	iErr_flag = 1;
        sprintf(ret_msg ,"�ʻ�״̬��");
        strcpy(sMq_tax_payment.result , "24002");     /* ������     */
        strcpy(sMq_tax_payment.addword , "�ʻ�״̬��");  /* ����  */
       	goto TrLog;
    }
    ret1 = diff_custname(sDd_mst.name, sMq_tax_payment.handorg_name) ;
    if(ret1){
      strcpy(g_pub_tx.reply , "D101");
      strcpy (ret_result , "24002" ); 
      sprintf(ret_msg ,"�ʺŻ�������%s(%s,%s)",sMq_tax_payment.pay_acct, 
	    sDd_mst.name, sMq_tax_payment.handorg_name);
      sprintf(acErrMsg ,"�ʺŻ�������%s(%s,%s)",sMq_tax_payment.pay_acct, 
        sDd_mst.name, sMq_tax_payment.handorg_name);
      WRITEMSG
      strcpy(sMq_tax_payment.result , "24002");     /* ������     */
      strcpy(sMq_tax_payment.addword , "�ʺŻ�������");  /* ����  */
      iErr_flag = 1;
      goto TrLog;
    }
      
    sprintf(acErrMsg,"-----------��ʼ���ڴ�ȡ��(%s)", sMq_tax_payment.pay_acct);
    WRITEMSG
  
    strcpy( g_pub_tx.ac_no, sMq_tax_payment.pay_acct );
    g_pub_tx.ac_seqn = 1;
    
    g_pub_tx.tx_amt1 = sMq_tax_payment.tra_amt ;
  
    strcpy(g_pub_tx.add_ind,"0");  /*- �ۿ� -*/
      
    memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
    strcpy( g_pub_tx.sub_tx_code, "D003" );
    set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      
    strcpy( g_pub_tx.prt_ind, "0" );
    set_zd_data( "102O", "0" );
      
    set_zd_data( "102J", "01" );
    set_zd_data( "102K", "2" );
    set_zd_double( "102F", g_pub_tx.tx_amt1 );
    set_zd_double( "102P", 0.00 );
    set_zd_double( "102Q", 0.00 );

    strcpy(g_pub_tx.brf, "��˰��");
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
    strcpy(g_pub_tx.hst_ind,"1");
    g_pub_tx.svc_ind=1001;
  
    sprintf(acErrMsg,"BEG D099[%s][%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
    WRITEMSG
    
    sql_begin();
  
    ret = pub_acct_trance();
    if( ret != 0 )
    {
      sql_rollback();
      sql_begin();
      sprintf( cReturn, "%s", g_pub_tx.reply );
      sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
      WRITEMSG
      iErr_flag = 1;
	  strcpy(sMq_tax_payment.result , "24001");
	  strcpy(sMq_tax_payment.addword, "���ô�ȡ������ʧ��");
      goto TrLog;
    }
    
    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
    if ( ret )
    {
      sql_rollback();
      sql_begin();
      sprintf( cReturn, "%s", g_pub_tx.reply );
      sprintf(acErrMsg,"���ô�ȡ������ʧ��!"); WRITEMSG
      iErr_flag = 1;
	  strcpy(sMq_tax_payment.result , "24001");
	  strcpy(sMq_tax_payment.addword, "���ô�ȡ������ʧ��");
      goto TrLog;
    }

    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.tx_amt1 = 0.00;        /*������*/
    strcpy( g_pub_tx.ac_id_type,"0" ); /*�˻�������Ϊ����*/

TrLog:
fprintf(fw,"ok\n");
    if ( iErr_flag == 0)
    {
      lSnum = lSnum + 1;
      dSamt = dSamt + sMq_tax_payment.tra_amt;

	  sMq_tax_payment.tx_amt = sMq_tax_payment.tra_amt ;
	  sMq_tax_payment.seq_no = g_pub_tx.trace_no ;
	  strcpy(sMq_tax_payment.result , "90000");
	  strcpy(sMq_tax_payment.addword , "ok");
      strcpy(sMq_tax_payment.sts,"2");
      ret = Mq_tax_payment_Upd_Upd_hold( sMq_tax_payment , g_pub_tx.reply);
	  if(ret ){
        sprintf(acErrMsg,"Mq_tax_payment_Upd_Upd_hold %s!",
		g_pub_tx.reply); WRITEMSG
        sql_rollback();
        lSnum = lSnum - 1;
        dSamt = dSamt - sMq_tax_payment.tra_amt;
		fprintf(fw,"�ۿ��ύʧ��!\n");
		fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
		fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
		fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
		fprintf(fw1,"%d|99999|�޸ļ�¼ʧ��(%d)|\n", g_pub_tx.tx_date, ret );
	  }
	  else{
        sql_commit();
		fprintf(fw,"�ۿ�ɹ�!\n");
		fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
		fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
		fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
		fprintf(fw1,"%d|90000|�ۿ�ɹ�|\n", g_pub_tx.tx_date );
	  }

     /* д�ı� */ 
    }
    else { /* ʧ�� */
  	  memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
	  ret1 = Rsp_code_Sel( g_pub_tx.reply, &sRsp_code, "code = '%s'", g_pub_tx.reply );
	  if ( ret1 == 0 )
	  {
		pub_base_strpack( sRsp_code.memo );
		strcpy( sMq_tax_payment.addword, sRsp_code.memo );
	  }
	  ret = sql_execute( "update mq_tax_payment set sts = '2',\
	    result='%s', addword='%s' where taxorg_code='%s' and  entrust_date=%d \
		and tra_no='%s' ",sMq_tax_payment.result, sMq_tax_payment.addword,\
		sMq_tax_payment.taxorg_code, sMq_tax_payment.entrust_date,\
		sMq_tax_payment.tra_no );
      if(ret){
        sprintf(acErrMsg,"�޸� mq_tax_payment (%d)", ret); WRITEMSG
        sql_rollback();
	    fprintf(fw,"<�޸ļ�¼> ");
	  }
      else{
		sql_commit();
	  }
	  fprintf(fw,"�ۿ�ʧ��!<%s>\n",sMq_tax_payment.addword);

	  fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
	  fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
	  fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
	  fprintf(fw1,"%d|%s|%s|\n", g_pub_tx.tx_date,
	  sMq_tax_payment.result, sMq_tax_payment.addword );
    }
	/* �޸ļ�¼ */
  	sql_begin();
  }
  Mq_tax_payment_Clo_Upd_hold();  

  /* �޸Ŀ�˰ͷ�ļ�mq_tax_head */

  ret = sql_execute( "update mq_tax_head set succ_num = %d,\
	    succ_amt=%.2f, addword='�ۿ����' where src='%s' and msg_id = '%s' \
		and pack_no='%s' ",lSnum, dSamt,\
		sMq_tax_head.src, sMq_tax_head.msg_id, sMq_tax_head.pack_no );
  if(ret){
    sprintf(acErrMsg,"�޸� mq_tax_headt (%d)", ret); WRITEMSG
    sql_rollback();
    fprintf(fw,"    �޸�mq_tax_head��¼�� <%d>\n", ret);
  }
  else  fprintf(fw,"    �޸�mq_tax_head��¼�ɹ�!\n");

  if ( lSnum > 0 )
  {
    /* �ж��տ��ʺŶ�Ӧ���ڲ��ʺ�  */
strcpy(sMq_acct_no.ac_no,"5000038900010");
strcpy(sMq_tax_head.payee_acct,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));

    ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
      "payee_acct= '%s' ", sMq_tax_head.payee_acct );
    if ( ret )
    {
      strcpy(g_pub_tx.reply,"D101");
	  sprintf(acErrMsg,"���Ҳ����ʺ����ڲ��ʺŶ�Ӧ��[%d]!", ret);
	  WRITEMSG
      sprintf(ret_msg ,"���Ҳ����ʺ����ڲ��ʺŶ�Ӧ��(%d)", ret);
      strcpy(ret_result,"94054");
	  goto ErrExit_kk;
    }
    pub_base_sysinit();
    
    sprintf(acErrMsg,"ssggqq��ʼ��λ�˺Ŵ��(%s)",sMq_acct_no.ac_no); WRITEMSG
    g_pub_tx.ac_id = 0;   /*test clean */
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.beg_note_no,"");
          
    strcpy( g_pub_tx.ac_no, sMq_acct_no.ac_no );
    g_pub_tx.tx_amt1 = dSamt;
          
    if ( sMq_acct_no.ac_no[0] == '9' )  /*-- �ڲ��� --*/
    {
      strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
      strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
      g_pub_tx.svc_ind=9001;
      
      strcpy(g_pub_tx.add_ind,"1");	/*-- ���� --*/
        
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "A017" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      
      set_zd_data( "1214", "01" );
      set_zd_data( "1215", "2" );
      set_zd_double( "1218", dSamt );
      strcpy(g_pub_tx.brf, "����˰��");
    }
    else
    {
      strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
      g_pub_tx.svc_ind=1001;

      strcpy(g_pub_tx.add_ind,"1");	/*-- ���� --*/
        
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "D099" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
        
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "1017", "0" );
          
      set_zd_data( "101A", "01" );
      set_zd_data( "101B", "2" );
      set_zd_double( "1013", dSamt );
       
      strcpy(g_pub_tx.brf, "����˰��");
        
      WRITEMSG
    }
    
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
    strcpy(g_pub_tx.hst_ind,"1");
    
    ret=pub_acct_trance();
    if( ret )
    {
      sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
      WRITEMSG
      goto ErrExit_kk;
    }
    
    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
    if ( ret )
    {
      sprintf(acErrMsg,"�ǵ�λ����˳���[%d]!",ret);
      WRITEMSG
	  fprintf(fw,"    �ǵ�λ����˳���<%s>[%d]!\n", g_pub_tx.ac_no,ret);
      goto ErrExit_kk;
    }
	fprintf(fw,"    �ǵ�λ����˳ɹ�<%s>!\n", g_pub_tx.ac_no,ret);
    
  }
  
  set_zd_data( "0130", acErrMsg );  
        
  set_zd_data(DC_FILE_SND,"1");

OkExit_kk:
  fclose(fw1);
  pub_base_strpack(sMq_tax_head.addword);
  fw1 = fopen(file_name,"w");
  fprintf(fw1,"%s|", sMq_tax_head.paybk_code);
  fprintf(fw1,"%8d|", g_pub_tx.tx_date);
  fprintf(fw1,"%08d|", g_pub_tx.trace_no);
  fprintf(fw1,"%s|", sMq_tax_head.taxorg_code);
  fprintf(fw1,"%d|", sMq_tax_head.entrust_date);
  fprintf(fw1,"%s|", sMq_tax_head.pack_no);
  fprintf(fw1,"%d|", sMq_tax_head.all_num);
  fprintf(fw1,"%.2f|", sMq_tax_head.all_amt);
  fprintf(fw1,"%d|%.2f|", lSnum , dSamt );
  fprintf(fw1,"%s|%s|\n", sMq_tax_head.result, sMq_tax_head.addword );
  fclose(fw1);
  memset(tmp_str, 0x00, sizeof(tmp_str));
  sprintf(tmp_str,"cat %s >>%s",f_name,file_name);
  system(tmp_str);


  set_zd_data("0100", "tips");
  pub_base_AllDwnFilName( f_name );
  sprintf(tmp_str,"cp  %s %s",file_name, f_name);
  system(tmp_str);
  strcpy(tmp_str,f_name+16);
  fprintf(fw,"filename:%s-%s-%s\n",file_name,f_name, tmp_str);
  set_zd_data("0964", tmp_str);
  
  
  strcpy( g_pub_tx.reply, "0000" );
  fprintf(fw,"  ������˰�ɹ�!\n");
  sprintf( acErrMsg, "������˰ҵ������ϣ�" ); WRITEMSG
  set_zd_data( DC_REPLY, g_pub_tx.reply );
  return 0;
    
ErrExit_kk:
  fclose(fw1);

  fw1 = fopen(file_name,"w");
  fclose(fw1);

  Mq_tax_payment_Clo_Sel();
  sprintf(acErrMsg,"����������˰ҵ����ʧ��![%s]", g_pub_tx.reply );
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"  ������˰ʧ��\n");
  return 1;
}
