/***************************************************************
* 文 件 名:   spH801.c
* 功能描述：  财税库实时扣划税款(转发3001)
*
* 作    者:   mike        
* 完成日期：  2004年10月05日
* 修改：按税款明细 20041116
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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

/* 财税库实时扣划税款 */
int spH801()
{
  FILE *fr, *fw ;
  int ret = 0, i, j, k, num;
  char file_name[51];
  char tmp_item_no[6]; /* 扣税帐户科目 */
  char ret_result[6], ret_msg[81]; /* 返回结果代码及信息 */
  char f_name[31], buf[256], tmp_str[81];

  struct mq_acct_no_c         sMq_acct_no  ;        /*帐户对照表  */
  struct mq_tax_head_c        sMq_tax_head ;        /*扣税信息表头*/
  struct mq_tax_info_c        sMq_tax_info ;        /*个人信息表*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*扣税信息表体*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*税款条目信息表体*/
  struct dd_mst_c             sDd_mst ;             /* 活期存款   */

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));
  memset(ret_result , 0x00, sizeof(ret_result));
  memset(ret_msg    , 0x00, sizeof(ret_msg   ));

  /*** 初始化全局变量 ***/
  memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
  memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
  pub_base_sysinit();

  /* 取日志文件名 */

  sprintf(tmp_str,"%d",g_pub_tx.tx_date); tmp_str[6]=0;
  sprintf(file_name,"/home/dhccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 3001-实时扣税请求\n");
  fprintf(fw,"  交易<6801> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* 报文标识号 */

  fprintf(fw,"  节点代码  : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  报文标识  : <%s>\n", sMq_tax_head.msg_id);

 
  memset(tmp_str,0x00, sizeof(tmp_str));
  get_zd_data ("0964", tmp_str ); 
  sprintf(file_name,"/home/dhccb/txt/%s",tmp_str);

  fprintf(fw,"  传输文件名: <%s>\n", file_name);

  fr=fopen(file_name,"r");
  if(fr==NULL){
    sprintf(acErrMsg,"打开文件%s失败!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"打开文件(%s)失败", file_name);
    goto ErrExit;
  }

  /* 读文件第 1 行 */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"读文件%s信息头失败!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"读文件(%s)信息头失败", file_name);
    goto ErrExit;
  }
sprintf(acErrMsg,"line1<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.taxorg_code , tmp_str); /* 征收机关代码 */
  strcpy(sMq_tax_payment.taxorg_code , tmp_str); /* 征收机关代码 */
  set_zd_data ("0380", tmp_str); 
  fprintf(fw,"  征收机关代码  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* 委托日期     */
  sMq_tax_payment.entrust_date = atol(tmp_str);  /* 委托日期     */
  set_zd_data ("0440", tmp_str); 
  fprintf(fw,"  委托日期      : <%s>\n", tmp_str);

  sprintf(tmp_str, "%ld", g_pub_tx.tx_date);
  set_zd_data ("0450", tmp_str); 

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.pack_no , tmp_str );     /* 包流水号     */
  strcpy(sMq_tax_payment.pack_no , tmp_str );   /* 流水号     */
  strcpy(sMq_tax_payment.tra_no , tmp_str );   /* 流水号     */
  set_zd_data ("0860", tmp_str); 
  fprintf(fw,"  流水号        : <%s>\n", tmp_str);

  sMq_tax_head.all_num = 1 ;                             /* 总笔数 */

  /* 读文件第 2 行 */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"读文件%s失败转帐信息!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"读文件(%s)转帐信息失败", file_name);
    goto ErrExit;
  }

sprintf(acErrMsg,"line2<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>1) j=1;  tmp_str[j]=0;
  strcpy(sMq_tax_head.handle_type, tmp_str);      /* 经收类别    */
  fprintf(fw,"  经收类别      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_bankno, tmp_str);     /* 收款行行号  */
  fprintf(fw,"  收款行行号    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payeeorg_code, tmp_str);    /* 收款单位开户行*/
  fprintf(fw,"  收款单位开户行: <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>32) j=32; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_acct, tmp_str);       /* 收款人帐号　*/
  fprintf(fw,"  收款人帐号    : <%s>\n", tmp_str);


  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_name, tmp_str);       /* 收款人名称  */
  fprintf(fw,"  收款人帐号    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy( sMq_tax_head.paybk_code, tmp_str);       /* 付款清算行号 */
  fprintf(fw,"  付款清算行号  : <%s>\n", tmp_str);

  sMq_tax_head.return_term = 0 ;                         /* 回执期限天数 */
  sMq_tax_head.succ_num = 1;
  strcpy(sMq_tax_head.result , "90000");                 /* 处理结果     */
  strcpy(sMq_tax_head.addword, "ok  ");                  /* 附言   */
  strcpy(sMq_tax_head.send_sts , "1" );                  /* 回执标志   */
  strcpy(sMq_tax_head.sts , "2" );                       /* 包状态标志   */
  sMq_tax_head.stop_num = 0;

sprintf(acErrMsg,"交流水号0860:<%s>", sMq_tax_payment.tra_no); WRITEMSG

  /* 判断收款帐号对应的内部帐号  */
  ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
    "payee_acct= '%s' ", sMq_tax_head.payee_acct );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"查找财政帐号与内部帐号对应错[%d]!", ret);
	WRITEMSG
    sprintf(ret_msg ,"查找财政帐号与内部帐号对应错(%d)", ret);
    strcpy(ret_result,"94054");
/* goto ErrExit; */
/* will delete */
strcpy(sMq_acct_no.ac_no,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));
  }

  /* 读文件第 3 行 */
  if(fgets ( buf , 255 , fr )<=0){
    sprintf(acErrMsg,"读文件%s基本信息失败!", file_name);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"读文件(%s)基本信息失败", file_name);
    goto ErrExit;
  }

sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>24) j=24; tmp_str[j]=0;
  strcpy(sMq_tax_payment.pay_acct, tmp_str);      /* 付款帐号     */

  pub_base_old_acno( sMq_tax_payment.pay_acct );  /* 对旧帐号的处理 */
  fprintf(fw,"  付款帐号      : <%s><%s>\n",tmp_str,sMq_tax_payment.pay_acct);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.handorg_name, tmp_str);  /* 缴款单位名称 */
  fprintf(fw,"  缴款单位名称  : <%s>\n",tmp_str);

/* will delete */
/*
strcpy(sMq_tax_payment.pay_acct,"1000010700010");
strcpy(sMq_tax_payment.handorg_name,"薛士忠");
*/

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_payment.tra_amt = atof(tmp_str);             /* 应扣金额 */
  sMq_tax_head.all_amt    = sMq_tax_payment.tra_amt;   /* 总金额 */
  sMq_tax_head.succ_amt   = sMq_tax_payment.tra_amt;   /* 成功金额     */
  fprintf(fw,"  应扣金额      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>18) j=18; tmp_str[j]=0;
  strcpy(sMq_tax_payment.taxvou_no, tmp_str);     /* 税票号码     */
  set_zd_data ("0370", tmp_str); 
  fprintf(fw,"  税票号码      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.protocol_no, tmp_str);   /* 协议书号     */
  fprintf(fw,"  协议书号      : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_payment.remark , tmp_str);       /* 备注         */
  fprintf(fw,"  备注          : <%s>\n",tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  num = atoi(tmp_str);
  sMq_tax_payment.tax_num = num; /* 税款条目 */
  fprintf(fw,"  税款条目: <%s>\n",tmp_str);

  strcpy(sMq_tax_payment_mx.taxorg_code, sMq_tax_payment.taxorg_code);
  strcpy(sMq_tax_payment_mx.tra_no, sMq_tax_payment.tra_no);
  sMq_tax_payment_mx.entrust_date = sMq_tax_payment.entrust_date;

  for(k=0; k<num; ++k){
    /* 读文件第 4 - 4+num 行 */
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"读文件税款条目%s失败(%d)!", file_name, k+1);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"读文件(%s)税款条目(%d)失败", file_name, k+1);
      goto ErrExit;
    }
    fprintf(fw,"    %d=%s", k+1,buf);
    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_tax_payment_mx.tax_num = atoi(tmp_str); /* 项目序号 */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>60) j=60; tmp_str[j]=0;
	strcpy(sMq_tax_payment_mx.tax_name, tmp_str);   /* 税种名称 */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>8) j=8; tmp_str[j]=0;
	sMq_tax_payment_mx.beg_date = atol(tmp_str); /* 起始日期 */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
	if(j>8) j=8; tmp_str[j]=0;
	sMq_tax_payment_mx.end_date = atol(tmp_str); /* 终止日期 */

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
	sMq_tax_payment_mx.tx_amt = atol(tmp_str);  /* 本项金额 */

    /* 插入记录到实时扣税条目中 */
    ret = Mq_tax_payment_mx_Ins(sMq_tax_payment_mx, g_pub_tx.reply);
    if (ret != 0)
    {
  	  strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"插入财税库税款明细条目错!(sql=%d)",ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"插入财税库税款明细条目错!(sql=%d)",ret);
      goto ErrExit;
    }
  }

  sMq_tax_payment.tax_date = g_pub_tx.tx_date ;          /* 扣税日期     */
  sMq_tax_payment.tx_amt   = sMq_tax_payment.tra_amt ;   /* 实扣金额     */
  sMq_tax_payment.seq_no   = g_pub_tx.trace_no ;         /* 扣款流水     */
  strcpy(sMq_tax_payment.result , "90000");              /* 处理结果     */
  strcpy(sMq_tax_payment.addword , "0000");              /* 附言   */

  sMq_tax_payment.chk_date  = 0 ;                 /* 对帐日期     */
  strcpy(sMq_tax_payment.chk_ord , "" );       /* 对帐批次     */
  strcpy(sMq_tax_payment.sts , "2" ); /* 标志0-未扣 1-撤或冲 2-已扣款 3-已对帐*/

  g_reply_int = Dd_mst_Sel(g_pub_tx.reply , &sDd_mst , \
   "ac_id = (select ac_id from mdm_ac_rel where ac_no='%s') ",\
   sMq_tax_payment.pay_acct);
  if(g_reply_int){
    sprintf(acErrMsg,"调用查询dd_mst失败!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
	memset(ret_msg, 0x00, sizeof(ret_msg));
	if(g_reply_int == 100){
      sprintf(ret_msg ,"无此付款帐号(%s)", sMq_tax_payment.pay_acct);
	}
	else{
      sprintf(ret_msg ,"查询付款帐号(%d)", g_reply_int);
	}
    goto ErrExit;
  }

  g_reply_int = diff_custname(sDd_mst.name, sMq_tax_payment.handorg_name) ;
  if(g_reply_int){
	strcpy(g_pub_tx.reply , "D101");
    strcpy (ret_result , "24002" ); 
    sprintf(ret_msg ,"帐号户名不符%s(%s,%s)",sMq_tax_payment.pay_acct, 
	  sDd_mst.name, sMq_tax_payment.handorg_name);
    sprintf(acErrMsg ,"帐号户名不符%s(%s,%s)",sMq_tax_payment.pay_acct, 
	  sDd_mst.name, sMq_tax_payment.handorg_name);
    WRITEMSG
    goto ErrExit;
  }
  if(sDd_mst.ac_sts[0]!='1' || sDd_mst.hold_sts[0]=='2' ||
  sDd_mst.hold_sts[0]=='3' ){
	strcpy(g_pub_tx.reply , "D101");
    strcpy (ret_result , "24002" ); 
    sprintf(ret_msg ,"付款帐户状态错");
    goto ErrExit;
  }

  /* 给记帐结构赋值, 调用公用记帐函数,取款 */
  strcpy(g_pub_tx.ac_no, sMq_tax_payment.pay_acct );  /* 帐号 */
  strcpy(g_pub_tx.cur_no, "01" );                     /* 货币 */
  g_pub_tx.ac_seqn = 1;
  pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn );
  strcpy(g_pub_tx.brf, "扣税款" ); /* 摘要 */
  strcpy(g_pub_tx.add_ind , "0" ); /* 增减标志 0-减少 1-增加 */
  strcpy(g_pub_tx.hst_ind , "1" ); /* 日间入明细 */
  strcpy(g_pub_tx.ct_ind  , "2" ); /* 现转标志 1-现金 2-转账 */
  strcpy(g_pub_tx.prt_ind , "0" ); /* 不打折标志 */
  g_pub_tx.tx_amt1  = sMq_tax_payment.tra_amt ;  /* 扣税金额     */

  /* 检查凭证、零不记流水、零不记明细、检查支取方式等 */
  strcpy(g_pub_tx.ac_wrk_ind,"0000000011"); 
  g_pub_tx.svc_ind=1001;
  
  g_reply_int = pub_acct_trance();
  if( g_reply_int )
  {
    sprintf(acErrMsg,"调用存取款主控失败!");
    WRITEMSG

    if(memcmp(g_pub_tx.reply,"P101",4)==0||memcmp(g_pub_tx.reply,"P102",4)==0){
      strcpy(ret_result , "24001" ); /* 帐号不存在 */
      sprintf(ret_msg ,"付款人帐号(%s)不存在", g_pub_tx.ac_no);
    }
    else if(memcmp(g_pub_tx.reply,"A022",4)==0){
      strcpy(ret_result , "24003" ); 
      sprintf(ret_msg ,"付款人帐户余额不足(%.2f),%d", sDd_mst.bal, sDd_mst.ac_id);
    }

    goto ErrExit;
  }

  /* 借贷标志: 1-借 2-贷 */
  set_zd_data("0700", "1");
  g_reply_int = pubf_acct_proc("H801");
  if( g_reply_int )
  {
    sprintf(acErrMsg,"调用会计记帐主控失败!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"调用会计记帐主控失败!");
    goto ErrExit;
  }


  /*** 初始化全局变量 ***/
  memset(&g_mdm_ac_rel , 0x00 , sizeof(struct mdm_ac_rel_c));
  memset(&g_dd_mst     , 0x00 , sizeof(struct dd_mst_c));
  memset(&g_dd_parm    , 0x00 , sizeof(struct dd_parm_c));
  pub_base_sysinit();

  /* 给记帐结构赋值, 调用公用记帐函数,存款 */
  strcpy(g_pub_tx.ac_no, sMq_acct_no.ac_no );         /* 内部帐号 */
  strcpy(g_pub_tx.cur_no, "01" );                     /* 货币     */
  g_pub_tx.ac_seqn = 1;
  pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn );

  strcpy( g_pub_tx.brf , "入税款" );
  strcpy(g_pub_tx.add_ind , "1" ); /* 增减标志 0-减少 1-增加 */
  strcpy(g_pub_tx.hst_ind , "1" ); /* 日间入明细 */
  strcpy(g_pub_tx.ct_ind  , "2" ); /* 转账 */
  strcpy(g_pub_tx.prt_ind , "0" ); /* 不打折标志*/
  g_pub_tx.tx_amt1  = sMq_tax_payment.tra_amt ;  /* 上税金额     */

  /**检查凭证、零不记流水、零不记明细、检查支取方式**/
  strcpy(g_pub_tx.ac_wrk_ind,"0000000011"); 
  g_pub_tx.svc_ind=1001;
  
  /* 写交易流水,修改主文件,写未登折明细等 */
  g_reply_int=pub_acct_trance();
  if( g_reply_int )
  {
    sprintf(acErrMsg,"调用存取款主控失败!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"调用存取款主控失败!");
    goto ErrExit;
  }

  /* 赋值, 供主控记帐调用 */
  /* 借贷标志: 1-借 2-贷 */
  set_zd_data("0700" , "2");
  g_reply_int = pubf_acct_proc("H801");
  if( g_reply_int )
  {
    sprintf(acErrMsg,"调用会计记帐主控失败!");
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"调用会计记帐主控失败!");
    goto ErrExit;
  }

  /* 插入记录到实时扣税表头中 */
  ret = Mq_tax_head_Ins(sMq_tax_head,g_pub_tx.reply);
  if (ret != 0)
  {
	strcpy(g_pub_tx.reply , "D101");
    sprintf(acErrMsg,"插入财税库税款头表错![%s](sql=%d)",g_pub_tx.reply,ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"插入财税库税款头表错![%s](sql=%d)",g_pub_tx.reply,ret);
    goto ErrExit;
  }

  /* 插入记录到实时扣税表明细中 */
  ret = Mq_tax_payment_Ins(sMq_tax_payment,g_pub_tx.reply);
  if (ret != 0)
  {
	strcpy(g_pub_tx.reply , "D101");
    sprintf(acErrMsg,"插入财税库税款明细表错!(sql=%d)", ret);
    WRITEMSG
    strcpy (ret_result , "99004" ); 
    sprintf(ret_msg ,"插入财税库税款明细表错!(sql=%d)", ret);
    goto ErrExit;
  }

OkExit:
  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库实时扣税成功![%s]",g_pub_tx.reply);
  WRITEMSG

  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "实时扣税成功");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  sprintf(acErrMsg, " ---[%s]", tmp_str);
  WRITEMSG
  set_zd_data("0620" , tmp_str );    /* 报文标识号 */

  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"财税库实时扣税失败![%s]",g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "实时扣税错(%s)", g_pub_tx.reply);
  }

  sprintf(acErrMsg,"mike:<%s><%s>",ret_result, ret_msg);
  WRITEMSG

  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  fprintf(fw,"  返回结果: <%s>\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);
  return 1;
}

/* 比较客户名称(去掉名字中的空格) */
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
