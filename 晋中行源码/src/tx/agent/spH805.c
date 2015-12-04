/***************************************************************
* �� �� ��:   spH803.c
* ����������  ��˰����ϸ�˶�����   
*             ��������,ǰn-1����ֻ�Ǽ�,������,��n�����ٴ�������
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
#include "com_branch_c.h"
#include "mq_acct_no_c.h"
#include "mq_tax_info_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "mq_tax_payment_mx_c.h"
#include "mq_batch_head_c.h"
#include "mq_batch_mx_c.h"
#include "rsp_code_c.h"

/* ��˰����ϸ�˶����� */
int spH805( )
{
  FILE *fr, *fw ;
  int  ret = 0, i, j, k, num,n, pack_flag=0;
  char file_name[51];
  char tmp_item_no[6];             /* ��˰�ʻ���Ŀ */
  char ret_result[6], ret_msg[61], rq_msg[7]; /* ���ؽ�����뼰��Ϣ */
  char f_name[31], buf[256], tmp_str[81];

  struct mq_acct_no_c         sMq_acct_no  ;        /*�ʻ����ձ�  */
  struct mq_tax_info_c        sMq_tax_info ;        /*������Ϣ��*/
  struct mq_tax_head_c        sMq_tax_head ;        /*��˰��Ϣ��ͷ*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*��˰��Ϣ����*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*˰����Ŀ��Ϣ����*/
  struct mq_batch_head_c      sMq_batch_head ;      /*������Ϣ��ͷ*/
  struct mq_batch_mx_c        sMq_batch_mx ;        /*������Ϣ����*/
  struct dd_mst_c             sDd_mst ;             /*���ڴ��   */

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));
  memset(&sMq_batch_head,0x0,sizeof(sMq_batch_head));
  memset(&sMq_batch_mx,0x0,sizeof(sMq_batch_mx));
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
  fprintf(fw,"begin 3111-��˰��ϸ�˶�\n");
  fprintf(fw,"  ����<6805> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- ȡ�������  ---------*/
  get_zd_data  ("0330" , tmp_str               );      /* ����ڵ���� */
  get_zd_data  ("0620" , sMq_batch_head.msg_id );      /* ���ı�ʶ�� */

  fprintf(fw,"  �ڵ����  : <%s> ",  tmp_str) ; 
  fprintf(fw,"  ���ı�ʶ  : <%s>\n", sMq_batch_head.msg_id);

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

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_batch_head.paybk_code , tmp_str); /* �����к� */
  set_zd_data("0600", tmp_str);
  fprintf(fw,"  �����к�      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_batch_head.payee_bankno , tmp_str); /* �տ��к� */
  set_zd_data("0580", tmp_str);
  fprintf(fw,"  �տ��к�      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>8) j=8; tmp_str[j]=0;
  sMq_batch_head.chk_date = atol(tmp_str);  /* �������� */
  sMq_batch_mx.chk_date   = atol(tmp_str);  /* �������� */
  set_zd_data("0860", tmp_str);
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>4) j=4; tmp_str[j]=0;
  strcpy(sMq_batch_head.chk_ord , tmp_str );     /* �������� */
  strcpy(sMq_batch_mx.chk_ord   , tmp_str );     /* �������� */
  set_zd_data("0920", tmp_str);
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>1) j=1; tmp_str[j]=0;
  strcpy(sMq_batch_head.chk_type , tmp_str );     /* �������� 0-�ռ� 1-���� */
  if(tmp_str[0]=='0') strcpy(rq_msg,"�ռ�");
  else if(tmp_str[0]=='1') strcpy(rq_msg,"����");
  else strcpy(rq_msg,"���ʹ�");
  fprintf(fw,"  ��������      : <%s><%s>\n", tmp_str,rq_msg);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  �ܱ���        : <%s>\n", tmp_str);
  sMq_batch_head.all_num =  atoi(tmp_str) ; /* �ܱ��� */

  /* �ռ�������޼�¼,�򷵻� */
  if ( sMq_batch_head.all_num < 1 ){
    pack_flag = 1  ;
    goto OkExit;
  }

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.all_amt =  atof(tmp_str) ; /* �ܽ��  */
  fprintf(fw,"  �ܽ��        : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.child_num =  atoi(tmp_str) ; /* �Ӱ����� */
  fprintf(fw,"  �Ӱ�����      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.pack_ord =  atoi(tmp_str) ; /* �Ӱ���� */
  sMq_batch_mx.pack_ord   =  atoi(tmp_str) ; /* �Ӱ���� */
  fprintf(fw,"  �Ӱ����      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_num = atoi(tmp_str) ;        /* �������� */
  sMq_batch_head.pack_num =  atoi(tmp_str) ;
  fprintf(fw,"  ��������      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* ������� */
  sMq_batch_head.pack_amt =  atof(tmp_str) ;
  fprintf(fw,"  �������      : <%s>\n", tmp_str);

  /* �����¼�����ʱ�ͷ�� */
  ret = Mq_batch_head_Ins ( sMq_batch_head , g_pub_tx.reply );
  if (ret != 0)
  {
	  strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"�����˰�����ͷ���![%s](sql=%d)",g_pub_tx.reply,ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"�����˰�����ͷ���![%s](sql=%d)",g_pub_tx.reply,ret);
      goto ErrExit;
  }

  /* ���ļ��� 2-n �� */
  for(n=0; n < sMq_tax_head.all_num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"���ļ�%s������Ϣʧ��!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"���ļ�(%s)������Ϣʧ��", file_name);
      goto ErrExit;
    }

    sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG
	fprintf(fw," %d=%s",n+1,buf);

    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>12) j=12; tmp_str[j]=0;
    fprintf(fw,"  �����п����к�: <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>24) j=24; tmp_str[j]=0;
    strcpy(sMq_batch_mx.pay_acct, tmp_str);      /* �����ʺ�     */
    pub_base_old_acno( sMq_batch_mx.pay_acct );  /* �Ծ��ʺŵĴ��� */
    fprintf(fw,"  �����ʺ�      : <%s><%s>\n",tmp_str,sMq_batch_mx.pay_acct);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>12) j=12; tmp_str[j]=0;
    strcpy(sMq_batch_mx.taxorg_code, tmp_str);  /* ԭ���ջ��ش���  */
    fprintf(fw,"  ԭ���ջ��ش���: <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    sMq_batch_mx.entrust_date = atol(tmp_str);  /* ԭί������ */
    fprintf(fw,"  ԭί������    : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_batch_mx.pack_no, tmp_str);  /* ԭ����ˮ�� */
    fprintf(fw,"  ԭ����ˮ��    : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_batch_mx.tra_no, tmp_str);  /* ԭ������ˮ�� */
    fprintf(fw,"  ԭ������ˮ��  : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_batch_mx.tra_amt = atof(tmp_str);         /* ���׽�� */
    fprintf(fw,"  ���׽��      : <%s>\n", tmp_str);

	strcpy(sMq_batch_mx.flag,"0");

    /* �����¼��������ϸ�� */
    ret = Mq_batch_mx_Ins(sMq_batch_mx,g_pub_tx.reply);
    if (ret != 0)
    {
      strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"�����˰�������ϸ���!(sql=%d)", ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"�����˰�������ϸ���!(sql=%d)", ret);
      goto ErrExit;
	}
  }
  if(sMq_batch_head.child_num == sMq_batch_head.pack_ord ) pack_flag=1;
  else pack_flag=0;

  /* �Ӱ����������Ӱ����,��ʾ���Ѵ���, ����д���,���ʼ����� */
  if(pack_flag == 1 ){
    ret = Mq_batch_mx_Dec_Upd( g_pub_tx.reply,
    "chk_date = %d and chk_ord = '%s' ",\
	 sMq_batch_head.chk_date, sMq_batch_head.chk_ord );
    if ( ret )
    {
      sprintf(acErrMsg,"ִ��Mq_batch_mx_Dec_Upd����[%d]", ret );
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
	  goto ErrExit ;
    }

    while(1)
    {
      ret = Mq_batch_mx_Fet_Upd( &sMq_batch_mx , g_pub_tx.reply );
      if ( ret == 100 )
      break;
      else if ( ret )
      {
        sprintf(acErrMsg,"ִ��Mq_batch_mx_Fet_Sel����[%d]", ret );
        WRITEMSG
        goto ErrExit;
      }
	  
      ret = Mq_tax_payment_Sel( g_pub_tx.reply, &sMq_tax_payment, \
      "taxorg_code= '%s' and entrust_date = %d and tra_no = '%s' ",\
	   sMq_batch_mx.taxorg_code,sMq_batch_mx.entrust_date,sMq_batch_mx.tra_no );
fprintf(fw,"ret=%d %s %d %s\n", ret,sMq_batch_mx.taxorg_code,sMq_batch_mx.entrust_date,sMq_batch_mx.tra_no);
	  if(ret == 0 ) {
	    ret = sql_execute( "update mq_tax_payment set sts = '3', \
	          chk_date=%d ,chk_ord='%s' where taxorg_code='%s' and \
	          entrust_date=%d and tra_no='%s' ", sMq_batch_mx.chk_date,\
	          sMq_batch_mx.chk_ord , sMq_batch_mx.taxorg_code,\
	          sMq_batch_mx.chk_date, sMq_batch_mx.tra_no );
        strcpy(sMq_batch_mx.flag,"1"); 
	  }
	  else if(ret == 100){ /* �޼�¼,��Ҫ���� */
        strcpy(sMq_batch_mx.flag,"2"); 
	  }
      else if ( ret )
      {
        fprintf(fw,"ret=%d %s %d %s \n", ret,sMq_batch_mx.taxorg_code,
		sMq_batch_mx.chk_date,sMq_batch_mx.tra_no);
		goto ErrExit ;
      }
      ret = Mq_batch_mx_Upd_Upd( sMq_batch_mx , g_pub_tx.reply);
	  if(ret ){
        sprintf(acErrMsg,"Mq_batch_mx_Upd_Upd %s!", g_pub_tx.reply); WRITEMSG
        goto ErrExit;
      }

     fprintf(fw," <%s><%d><%s> ret=%d\n", sMq_batch_mx.taxorg_code,\
	 sMq_batch_mx.chk_date, sMq_batch_mx.tra_no, ret);

    }
	/* �ر��α� */
    Mq_batch_mx_Clo_Upd();  

	/* ͨ�����֧����� */

	/* ---------*/

  }

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"��˰��<%s>���ʳɹ�![%s]",rq_msg,g_pub_tx.reply);
  WRITEMSG
  if(pack_flag == 1 ){
    strcpy(ret_result , "90000");
	sprintf(ret_msg , "(%s)���ʳɹ�", rq_msg);
  }
  else{
	strcpy(tmp_str,"no");
    set_zd_data  ("0620" , tmp_str );      /* ���ı�ʶ�� */
    strcpy(ret_result , "no   ");
	sprintf(ret_msg , "(%s)���ʰ����ճɹ�", rq_msg);
  }

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  if(pack_flag==1) fprintf(fw,"  ���ؽ��: ����(%s)\n", ret_msg);
  else  fprintf(fw,"  ���ؽ��: ���ʰ�����(%s)\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"��˰��<%s>����ʧ��![%s]", rq_msg,g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	if(pack_flag==1) sprintf(ret_msg , "%s ���ʴ�", rq_msg);
	else  sprintf(ret_msg , "%s���ʰ����մ�!", rq_msg);
  }
  else{
	if(pack_flag==1) sprintf(tmp_str ,"%s ���ʴ�", rq_msg);
	else  sprintf(tmp_str ,"%s���ʰ����մ�!", rq_msg);
	strcpy(ret_result, tmp_str);
  }

  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  if(pack_flag==1) fprintf(fw,"  ���ؽ��: ����(%s)\n", ret_msg);
  else  fprintf(fw,"  ���ؽ��: ���ʰ�����(%s)\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 1;
}
