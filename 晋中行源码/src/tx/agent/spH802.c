/***************************************************************
* �� �� ��:   spH802.c
* ����������  ��˰��ʵʱ�ۻ�˰����
*
* ��    ��:   mike        
* ������ڣ�  2004��10��07��
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
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"

/* ��˰��ʵʱ�ۻ�˰���� */
int spH802() 
{
  FILE *fw ;
  int ret = 0;
  char file_name[51], tmp_str[81];
  char tmp_item_no[6]; /* ��˰�ʻ���Ŀ */
  char ret_result[6], ret_msg[61]; /* ���ؽ�����뼰��Ϣ */

  struct mq_tax_head_c    sMq_tax_head ;     /*��˰��Ϣ��ͷ*/
  struct mq_tax_payment_c sMq_tax_payment ;  /*��˰��Ϣ����*/

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));

  /*** ��ʼ��ȫ�ֱ��� ***/
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
  memset(ret_result, 0x00, sizeof(ret_result));
  memset(ret_msg   , 0x00, sizeof(ret_msg   ));
  memset( &sMq_tax_head, 0x00, sizeof(sMq_tax_head) );

  pub_base_sysinit();

  /* ȡ��־�ļ��� */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 1021-ʵʱ��˰����\n");
  fprintf(fw,"  ����<6802> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- ȡ�������  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* ����ڵ���� */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* ���ı�ʶ�� */

  fprintf(fw,"  �ڵ���� : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  ���ı�ʶ : <%s>\n", sMq_tax_head.msg_id);

  get_zd_data  ("0380" , sMq_tax_head.taxorg_code);      /* ԭ���ջ��ش��� */
  fprintf(fw,"  ԭ���ջ��ش���: <%s>\n", sMq_tax_head.taxorg_code);

  get_zd_data  ("0450" ,tmp_str);     /* ί������ */
  fprintf(fw,"  ί������      : <%s>\n", tmp_str);

  get_zd_data  ("0870" ,tmp_str);     /* ����������� */
  fprintf(fw,"  �����������  : <%s>\n", tmp_str);

  get_zd_long  ("0440" ,&sMq_tax_head.entrust_date);     /* ԭί������ */
  fprintf(fw,"  ԭί������    : <%d>\n", sMq_tax_head.entrust_date);

  get_zd_data  ("0860" , sMq_tax_head.pack_no     );     /* ԭ����ˮ�� */
  fprintf(fw,"  ԭ������ˮ��  : <%s>\n", sMq_tax_head.pack_no);

  get_zd_data  ("0740" ,tmp_str);     /* ����ԭ�� */
  fprintf(fw,"  ����ԭ��      : <%s>\n", tmp_str);

  ret = Mq_tax_head_Sel( g_pub_tx.reply, &sMq_tax_head, \
    "taxorg_code= '%s' and entrust_date = %d and pack_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"���Ҵ���˰�����ϴ�[%d]%d!!<%s><%d><%s>", ret, __LINE__,
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no);
	WRITEMSG
	if(ret==100) sprintf(ret_msg ,"�޴˿�˰��¼ͷ");
	else sprintf(ret_msg ,"���Ҵ���˰������ͷ��(%d)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  if(sMq_tax_head.sts[0]!='2'){
	if(sMq_tax_head.sts[0]=='1'){
	  sprintf(acErrMsg,"�ѳ���");
	  sprintf(ret_msg ,"�ѳ���, �����ٴγ���!");
	  strcpy(ret_result,"94032");
	}
	else if(sMq_tax_head.sts[0]=='3'){
	  sprintf(acErrMsg,"�Ѷ���");
	  sprintf(ret_msg ,"�Ѷ���, ���ܳ���");
	  strcpy(ret_result,"94032");
	}
	else{
	  sprintf(acErrMsg,"�Ѷ���");
	  sprintf(ret_msg ,"״̬��־��(%s), ���ܳ���", sMq_tax_head.sts);
	  strcpy(ret_result,"94032");
	}
	WRITEMSG
    strcpy(g_pub_tx.reply,"D101");
	goto ErrExit;
  }

  /* ������ϸ */
  ret = Mq_tax_payment_Sel( g_pub_tx.reply, &sMq_tax_payment, \
    "taxorg_code= '%s' and entrust_date = %ld and tra_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"���Ҵ���˰�����ϴ�[%d]!!", ret);
	WRITEMSG
	if(ret==100) sprintf(ret_msg ,"�޴˿�˰��¼��ϸ");
	else sprintf(ret_msg ,"���Ҵ���˰��������ϸ��(%d)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }


  ret = Mq_tax_head_Dec_Upd( g_pub_tx.reply, \
    "taxorg_code= '%s' and entrust_date = %ld and pack_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"�������˰�������α��[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"�������˰�������α��(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  ret = Mq_tax_head_Fet_Upd( &sMq_tax_head , g_pub_tx.reply);
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"�򿪴���˰�������α��[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"�򿪴���˰�������α��(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  /* �ó�����־ */
  strcpy(sMq_tax_head.sts , "1");

  /* �޸�ʵʱ��˰��ͷ */
  ret = Mq_tax_head_Upd_Upd( sMq_tax_head , g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"�޸Ĳ�˰��˰��ͷ�����![%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"�޸Ĳ�˰��˰��ͷ�����![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  ret = Mq_tax_payment_Dec_Upd( g_pub_tx.reply, \
    "taxorg_code= '%s' and entrust_date = %ld and tra_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"�������˰�������α��[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"�������˰�������α��(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  ret = Mq_tax_payment_Fet_Upd( &sMq_tax_payment , g_pub_tx.reply);
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"�򿪴���˰�������α��[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"�򿪴���˰�������α��(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  /* �ó�����־ */
  strcpy(sMq_tax_payment.sts , "1");

  /* �޸Ŀ�˰����ϸ */
  ret = Mq_tax_payment_Upd_Upd(sMq_tax_payment,g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"�޸Ĳ�˰��˰����ϸ�����[%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"�޸Ĳ�˰��˰����ϸ�����![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  /* ������ˮ */
  ret = pub_acct_rollback (sMq_tax_payment.seq_no);
  if (ret != 0)
  {
    sprintf(acErrMsg,"������˰��˰����ϸ�����[%s](sql=%d)(%d)",\
	g_pub_tx.reply,ret, sMq_tax_head.succ_num);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"������˰��˰����ϸ�����![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }


OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"mike��˰��ʵʱ��˰�����ɹ�![%s]",g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "ʵʱ��˰�����ɹ�");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data  ("0620" , tmp_str );    /* ���ı�ʶ�� */

  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  ���ؽ��: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);

  return 0;

ErrExit:
  sprintf(acErrMsg,"mike��˰��ʵʱ��˰����ʧ��![%s]",g_pub_tx.reply);
  WRITEMSG
  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "ʵʱ��˰������(%s)", g_pub_tx.reply);
  }
  set_zd_data("0620" , "  " );    /* ���ı�ʶ�� */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  ���ؽ��: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);

  return 1;
}
