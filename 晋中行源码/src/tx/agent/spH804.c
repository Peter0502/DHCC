/***************************************************************
* 文 件 名:   spH804.c
* 功能描述：  财税库批量扣划止付
*
* 作    者:   mike        
* 完成日期：  2005年10月20日
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

/* 财税库批量扣划税止付 */
int spH804()
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
  fprintf(fw,"begin 1123-批量扣税止付\n");
  fprintf(fw,"  交易<6804> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* 报文标识号 */
  fprintf(fw,"  节点代码 : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  报文标识 : <%s>\n", sMq_tax_head.msg_id);

  get_zd_data  ("0380" , sMq_tax_head.taxorg_code);      /* 原征收机关代码 */
  fprintf(fw,"  原征收机关代码: <%s>\n", sMq_tax_head.taxorg_code);

  get_zd_data  ("0870" , tmp_str );      /* 止付申请序号   */
  fprintf(fw,"  止付申请序号  : <%s>\n", tmp_str);

  get_zd_long  ("0440" ,&sMq_tax_head.entrust_date);     /* 原委托日期     */
  fprintf(fw,"  原委托日期    : <%s>\n", sMq_tax_head.entrust_date);

  get_zd_data  ("0580" , sMq_tax_head.pack_no     );     /* 原包流水号     */
  fprintf(fw,"  原包流水号    : <%s>\n", sMq_tax_head.pack_no);

  get_zd_data  ("0860" , sMq_tax_payment.tra_no   );     /* 要止付交易流水 */
  fprintf(fw,"  止付交易流水  : <%s>\n", sMq_tax_payment.tra_no);

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[8]=0;
  set_zd_data ("0460", tmp_str); /* 止付日期 */


  ret = Mq_tax_head_Sel( g_pub_tx.reply, &sMq_tax_head, \
    "taxorg_code= '%s' and entrust_date = %ld and pack_no = '%s' ",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date, sMq_tax_head.pack_no );
  if ( ret )
  {
	sprintf(acErrMsg,"查找代缴税款资料错[%d]!!", ret); WRITEMSG
    sprintf(ret_msg ,"查找代缴税款资料错(%s)", ret);
    strcpy(ret_result,"3");
	goto ErrExit;
  }

  if(sMq_tax_head.sts[0]!='2'){
	if(sMq_tax_head.sts[0]=='1'){
	  sprintf(acErrMsg,"已止付");
	  sprintf(ret_msg ,"已止付");
	  strcpy(ret_result,"2");
	}
	else if(sMq_tax_head.sts[0]=='3'){
	  sprintf(acErrMsg,"已对帐");
	  sprintf(ret_msg ,"已对帐, 不能止付");
	  strcpy(ret_result,"2");
	}
	WRITEMSG
	goto ErrExit;
  }

  /* 修改实时扣税表头 */
  if(atoi(sMq_tax_payment.tra_no) < 1 ){
    ret = Mq_tax_head_Upt(sMq_tax_head,g_pub_tx.reply);
	strcpy(sMq_tax_head.sts,"1"); /* 置止付标志 */
    if (ret != 0)
    {
      sprintf(acErrMsg,"修改财税库税款头表出错![%s](sql=%d)",
	  g_pub_tx.reply,ret); WRITEMSG
      sprintf(ret_msg ,"修改财税库税款头表出错![%s](sql=%d)",
		g_pub_tx.reply,ret);
      strcpy(ret_result,"3");
      goto ErrExit;
    }
  }
  else{
    /* 查找明细 */
    ret = Mq_tax_payment_Sel( g_pub_tx.reply, &sMq_tax_payment, \
    "taxorg_code= '%s' and entrust_date = %d and tra_no = '%s' ",\
    sMq_tax_head.taxorg_code,sMq_tax_head.entrust_date,sMq_tax_payment.tra_no );
    if ( ret )
    {
 	  sprintf(acErrMsg,"查找代缴税款资料错[%d]!!", ret); WRITEMSG
      sprintf(ret_msg ,"查找代缴税款资料错(%s)", ret);
      strcpy(ret_result,"3");
	  goto ErrExit;
    }
    /* 修改扣税表明细, 置止付标志 */
    ret = Mq_tax_payment_Upt(sMq_tax_payment,g_pub_tx.reply);
	ret = sql_execute( "update mq_tax_payment set sts = '1', \
    where taxorg_code= '%s' and entrust_date = %d and tra_no = '%s' ",\
    sMq_tax_head.taxorg_code,sMq_tax_head.entrust_date,sMq_tax_payment.tra_no );
    if (ret != 0)
    {
      sprintf(acErrMsg,"修改财税库税款明细表出错![%s](sql=%d)",
	  g_pub_tx.reply, ret);
      sprintf(ret_msg ,"修改财税库税款明细表出错![%s](sql=%d)",
	  g_pub_tx.reply, ret);
      strcpy(ret_result,"3");
      WRITEMSG
      goto ErrExit;
    }
  }

OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库批量扣税冲正成功![%s]",g_pub_tx.reply);
  WRITEMSG
  strcpy(ret_result , "1");
  strcpy(ret_msg    , "批量扣税止付冲正成功");
  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* 报文标识号 */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);
  return 0;

ErrExit:
  strcpy(g_pub_tx.reply,"D101");
  sprintf(acErrMsg,"财税库批量扣税止付冲正失败![%s]",g_pub_tx.reply);
  WRITEMSG
  if(strlen(ret_result)<1){
	strcpy(ret_result , "3");
	sprintf(ret_msg , "批量扣税止付冲正错(%s)", g_pub_tx.reply);
  }
  /* 1-止付成功 2-止付失败,已扣款 3-止付失败,不存在或在途 */
  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );
  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fw);

  return 1;
}
