/***************************************************************
* 文 件 名:   spH802.c
* 功能描述：  财税库实时扣划税冲正
*
* 作    者:   mike        
* 完成日期：  2004年10月07日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define  EXTERN
#include "public.h"
#include "mq_tax_info_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"

/* 财税库实时扣划税冲正 */
int spH802() 
{
  FILE *fw ;
  int ret = 0;
  char file_name[51], tmp_str[81];
  char tmp_item_no[6]; /* 扣税帐户科目 */
  char ret_result[6], ret_msg[61]; /* 返回结果代码及信息 */

  struct mq_tax_head_c    sMq_tax_head ;     /*扣税信息表头*/
  struct mq_tax_payment_c sMq_tax_payment ;  /*扣税信息表体*/

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));

  /*** 初始化全局变量 ***/
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
  memset(ret_result, 0x00, sizeof(ret_result));
  memset(ret_msg   , 0x00, sizeof(ret_msg   ));
  memset( &sMq_tax_head, 0x00, sizeof(sMq_tax_head) );

  pub_base_sysinit();

  /* 取日志文件名 */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 1021-实时扣税冲正\n");
  fprintf(fw,"  交易<6802> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* 报文标识号 */

  fprintf(fw,"  节点代码 : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  报文标识 : <%s>\n", sMq_tax_head.msg_id);

  get_zd_data  ("0380" , sMq_tax_head.taxorg_code);      /* 原征收机关代码 */
  fprintf(fw,"  原征收机关代码: <%s>\n", sMq_tax_head.taxorg_code);

  get_zd_data  ("0450" ,tmp_str);     /* 委托日期 */
  fprintf(fw,"  委托日期      : <%s>\n", tmp_str);

  get_zd_data  ("0870" ,tmp_str);     /* 冲正申请序号 */
  fprintf(fw,"  冲正申请序号  : <%s>\n", tmp_str);

  get_zd_long  ("0440" ,&sMq_tax_head.entrust_date);     /* 原委托日期 */
  fprintf(fw,"  原委托日期    : <%d>\n", sMq_tax_head.entrust_date);

  get_zd_data  ("0860" , sMq_tax_head.pack_no     );     /* 原包流水号 */
  fprintf(fw,"  原交易流水号  : <%s>\n", sMq_tax_head.pack_no);

  get_zd_data  ("0740" ,tmp_str);     /* 冲正原因 */
  fprintf(fw,"  冲正原因      : <%s>\n", tmp_str);

  ret = Mq_tax_head_Sel( g_pub_tx.reply, &sMq_tax_head, \
    "taxorg_code= '%s' and entrust_date = %d and pack_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"查找代缴税款资料错[%d]%d!!<%s><%d><%s>", ret, __LINE__,
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no);
	WRITEMSG
	if(ret==100) sprintf(ret_msg ,"无此扣税记录头");
	else sprintf(ret_msg ,"查找代缴税款资料头错(%d)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  if(sMq_tax_head.sts[0]!='2'){
	if(sMq_tax_head.sts[0]=='1'){
	  sprintf(acErrMsg,"已冲正");
	  sprintf(ret_msg ,"已冲正, 不能再次冲正!");
	  strcpy(ret_result,"94032");
	}
	else if(sMq_tax_head.sts[0]=='3'){
	  sprintf(acErrMsg,"已对帐");
	  sprintf(ret_msg ,"已对帐, 不能冲正");
	  strcpy(ret_result,"94032");
	}
	else{
	  sprintf(acErrMsg,"已对帐");
	  sprintf(ret_msg ,"状态标志错(%s), 不能冲正", sMq_tax_head.sts);
	  strcpy(ret_result,"94032");
	}
	WRITEMSG
    strcpy(g_pub_tx.reply,"D101");
	goto ErrExit;
  }

  /* 查找明细 */
  ret = Mq_tax_payment_Sel( g_pub_tx.reply, &sMq_tax_payment, \
    "taxorg_code= '%s' and entrust_date = %ld and tra_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"查找代缴税款资料错[%d]!!", ret);
	WRITEMSG
	if(ret==100) sprintf(ret_msg ,"无此扣税记录明细");
	else sprintf(ret_msg ,"查找代缴税款资料明细错(%d)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }


  ret = Mq_tax_head_Dec_Upd( g_pub_tx.reply, \
    "taxorg_code= '%s' and entrust_date = %ld and pack_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"定义代缴税款资料游标错[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"定义代缴税款资料游标错(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  ret = Mq_tax_head_Fet_Upd( &sMq_tax_head , g_pub_tx.reply);
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"打开代缴税款资料游标错[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"打开代缴税款资料游标错(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  /* 置撤消标志 */
  strcpy(sMq_tax_head.sts , "1");

  /* 修改实时扣税表头 */
  ret = Mq_tax_head_Upd_Upd( sMq_tax_head , g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"修改财税库税款头表出错![%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"修改财税库税款头表出错![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  ret = Mq_tax_payment_Dec_Upd( g_pub_tx.reply, \
    "taxorg_code= '%s' and entrust_date = %ld and tra_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"定义代缴税款资料游标错[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"定义代缴税款资料游标错(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  ret = Mq_tax_payment_Fet_Upd( &sMq_tax_payment , g_pub_tx.reply);
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"打开代缴税款资料游标错[%d]!!", ret);
	WRITEMSG
    sprintf(ret_msg ,"打开代缴税款资料游标错(%s)", ret);
    strcpy(ret_result,"94054");
	goto ErrExit;
  }

  /* 置撤消标志 */
  strcpy(sMq_tax_payment.sts , "1");

  /* 修改扣税表明细 */
  ret = Mq_tax_payment_Upd_Upd(sMq_tax_payment,g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"修改财税库税款明细表出错[%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"修改财税库税款明细表出错![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  /* 撤消流水 */
  ret = pub_acct_rollback (sMq_tax_payment.seq_no);
  if (ret != 0)
  {
    sprintf(acErrMsg,"撤消财税库税款明细表出错[%s](sql=%d)(%d)",\
	g_pub_tx.reply,ret, sMq_tax_head.succ_num);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"撤消财税库税款明细表出错![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }


OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"mike财税库实时扣税冲正成功![%s]",g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "实时扣税冲正成功");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data  ("0620" , tmp_str );    /* 报文标识号 */

  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);

  return 0;

ErrExit:
  sprintf(acErrMsg,"mike财税库实时扣税冲正失败![%s]",g_pub_tx.reply);
  WRITEMSG
  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "实时扣税冲正错(%s)", g_pub_tx.reply);
  }
  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);

  return 1;
}
