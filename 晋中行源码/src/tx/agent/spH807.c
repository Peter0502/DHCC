/***************************************************************
* 文 件 名:   spH807.c
* 功能描述：  财税库三方协议验证请求 
*
* 作    者:   mike        
* 完成日期：  2004年10月18日
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

/* 财税库三方协议验证请求 */
int spH807()
{
  FILE *fw , *fr;
  int  ret = 0 , i , j ;
  char file_name[51], tmp_str[61];
  char tmp_item_no[6]; /* 扣税帐户科目 */
  char ret_result[6], ret_msg[61]; /* 返回结果代码及信息 */
  char tmp_buf[512];

  struct mq_tax_info_c    sMq_tax_info ;     /*纳税人信息 */
  struct dd_mst_c         sDd_mst ;


  /*** 初始化全局变量 ***/
  memset(&sMq_tax_info,0x0,sizeof(sMq_tax_info));

  pub_base_sysinit();

  /* 取日志文件名 */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 9114-三方协议验证\n");
  fprintf(fw,"  交易<6807> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0380" , sMq_tax_info.taxorg_code);      /* 征收机关代码 */
  get_zd_data  ("0630" , sMq_tax_info.taxpay_code);      /* 纳税人编号   */
  get_zd_data  ("0590" , sMq_tax_info.payopbk_code);     /* 付款开户行号 */
  get_zd_data  ("0600" , sMq_tax_info.paybk_code);       /* 付款清算行号 */
  get_zd_data  ("0300" , sMq_tax_info.pay_acct  );       /* 付款帐号     */
  get_zd_data  ("0962" , sMq_tax_info.handorg_name);     /* 缴款单位名称 */
  get_zd_data  ("0963" , sMq_tax_info.protocol_no);      /* 协议书编号   */
  sMq_tax_info.tx_date = g_pub_tx.tx_date ;              /* 登记日期     */
  strcpy(sMq_tax_info.sts , "1" );                       /* 状态标志     */
  set_zd_data  ("0961" , sMq_tax_info.handorg_name);     /* 缴款单位名称 */

  fprintf(fw,"  征收机关代码 :<%s>\n",sMq_tax_info.taxorg_code);
  fprintf(fw,"  纳税人编号   :<%s>\n",sMq_tax_info.taxpay_code);
  fprintf(fw,"  付款开户行号 :<%s>\n",sMq_tax_info.payopbk_code);
  fprintf(fw,"  付款清算行号 :<%s>\n",sMq_tax_info.paybk_code);
  fprintf(fw,"  缴款单位名称 :<%s>\n",sMq_tax_info.handorg_name);
  fprintf(fw,"  付款帐号     :<%s>\n",sMq_tax_info.pay_acct);
  fprintf(fw,"  协议书编号   :<%s>\n",sMq_tax_info.protocol_no);

  pub_base_old_acno( sMq_tax_info.pay_acct ); /* 对旧帐号的处理 */

  ret = Dd_mst_Sel(g_pub_tx.reply , &sDd_mst , \
   "ac_id = (select ac_id from mdm_ac_rel where ac_no='%s') ",\
   sMq_tax_info.pay_acct);
  if(g_reply_int){
    sprintf(acErrMsg,"调用查询dd_mst失败!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"调用查询dd_mst失败");
    goto ErrExit;
  }

  /* set_zd_data("0580", sDd_mst.opn_br_no); */

  /* 插入记录到纳税人信息表中 */
  ret = Mq_tax_info_Ins(sMq_tax_info , g_pub_tx.reply);
  if (ret != 0)
  {
    strcpy(g_pub_tx.reply,"D101");
    sprintf(acErrMsg,"插入财税库纳税人信息表错!(sql=%d)",ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg,"插入财税库纳税人信息表错!(sql=%d)",ret);
    goto ErrExit;
  }


OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库三方协议登记成功![%s]" , g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "0");
  strcpy(ret_msg    , "财税库三方协议登记成功");
  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* 报文标识号 */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"返回<%s><%s>\n",ret_result,ret_msg);

  fclose(fw);

  return 0;

ErrExit:
  sprintf(acErrMsg,"财税库三方协议登记失败![%s]" , g_pub_tx.reply );
  WRITEMSG
  strcpy(ret_result , "1");
  if(strlen(ret_result)<1){
	sprintf(ret_msg , "财税库三方协议登记错(%s)", g_pub_tx.reply);
  }
  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0670", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"返回<%s><%s>\n",ret_result,ret_msg);
  fclose(fw);
  return 1;
}

