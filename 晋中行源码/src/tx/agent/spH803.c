/***************************************************************
* 文 件 名:   spH803.c
* 功能描述：  财税库批量扣划申请   
*
* 作    者:   
* 完成日期：  2005年10月05日
* 修改：按税款明细 20051116
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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
char ret_result[6], ret_msg[61]; /* 返回结果代码及信息 */

/* 财税库批量扣划申请 */
int spH803()
{
  FILE *fr ;
  int ret = 0, i, j, k, num,n;
  char tmp_item_no[6];             /* 扣税帐户科目 */
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
  sprintf(file_name,"/home/zzccb/log/csk_%s.log",tmp_str);
  fw=fopen(file_name,"a");
  fprintf(fw,"begin 3102-批量扣税请求\n");
  fprintf(fw,"  交易<6803> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* 报文标识号 */

  fprintf(fw,"  节点代码  : <%s> ", sMq_tax_head.src); 
  fprintf(fw,"  报文标识  : <%s>\n", sMq_tax_head.msg_id);

  get_zd_data ("0964", f_name ); 
  sprintf(file_name,"/home/zzccb/txt/%s",f_name);

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
  strcpy(sMq_tax_payment_mx.taxorg_code , tmp_str); /* 征收机关代码 */
  set_zd_data ("0380", tmp_str); 
  fprintf(fw,"  征收机关代码  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* 委托日期     */
  sMq_tax_payment.entrust_date = atol(tmp_str);  /* 委托日期     */
  sMq_tax_payment_mx.entrust_date = atol(tmp_str);  /* 委托日期     */
  set_zd_data ("0440", tmp_str); 
  fprintf(fw,"  委托日期      : <%s>\n", tmp_str);

  sprintf(tmp_str, "%ld", g_pub_tx.tx_date);
  set_zd_data ("0450", tmp_str); 

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.pack_no , tmp_str );     /* 包流水号     */
  strcpy(sMq_tax_payment.pack_no , tmp_str );  /* 包流水号     */
  set_zd_data ("0860", tmp_str); 
  fprintf(fw,"  包流水号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_num = atoi(tmp_str) ;        /* 总笔数 */
  fprintf(fw,"  总笔数        : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* 总金额 */
  fprintf(fw,"  总金额        : <%s>\n", tmp_str);

  sMq_tax_head.succ_amt   = 0.0 ;   /* 成功金额  */
  sMq_tax_head.stop_num   = 0 ;     /* 止付笔数  */

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
  if(j>1) j=1; tmp_str[j]=0;
  strcpy(sMq_tax_head.handle_type, tmp_str); /* 经收类别    */
  fprintf(fw,"  经收类别      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_bankno, tmp_str); /* 收款行行号  */
  fprintf(fw,"  收款清算行号  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_tax_head.payeeorg_code, tmp_str); /* 收款单位开户行*/
  fprintf(fw,"  收款单位开户行: <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>32) j=32; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_acct, tmp_str); /* 收款人帐号　*/
  fprintf(fw,"  收款人帐号    : <%s>\n", tmp_str);


  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>60) j=60; tmp_str[j]=0;
  strcpy(sMq_tax_head.payee_name, tmp_str);  /* 收款人名称  */
  fprintf(fw,"  收款人名称    : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
  if(j>12) j=12; tmp_str[j]=0;
  strcpy( sMq_tax_head.paybk_code, tmp_str); /* 付款清算行号 */
  fprintf(fw,"  付款清算行号  : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.return_term = atoi(tmp_str) ; /* 回执期限天数 */
  fprintf(fw,"  回执期限天数  : <%s>\n", tmp_str);

  sMq_tax_head.succ_num = 0;
  strcpy(sMq_tax_head.result , "90000");     /* 处理结果  */
  strcpy(sMq_tax_head.addword ,"ok  ");      /* 附言代码  */
  strcpy(sMq_tax_head.send_sts , "1" );      /* 回执标志  */
  strcpy(sMq_tax_head.sts , "2" );           /* 包状态标志*/


  /* 判断收款帐号对应的内部帐号  */
  ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
    "payee_acct= '%s' ", sMq_tax_head.payee_acct );
  if ( ret )
  {
    strcpy(g_pub_tx.reply,"D101");
	sprintf(acErrMsg,"查找财政帐号(%s)与内部帐号对应错[%d]!",\
	sMq_tax_head.payee_acct,ret);
	WRITEMSG
    sprintf(ret_msg ,"查找财政帐号与内部帐号对应错(%d)", ret);
    strcpy(ret_result,"94054");
/* goto ErrExit; */
/* will delete */
strcpy(sMq_acct_no.ac_no,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));
  }

  /* 读文件第 3 行 */
  for(n=0; n < sMq_tax_head.all_num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"读文件%s基本信息失败!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"读文件(%s)基本信息失败", file_name);
      goto ErrExit;
    }

    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_tax_payment.tra_no , tmp_str);              /* 交易流水号  */
    fprintf(fw,"  交易流水号    : <%s>\n",tmp_str);

    sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>24) j=24; tmp_str[j]=0;
    strcpy(sMq_tax_payment.pay_acct, tmp_str);      /* 付款帐号     */
    pub_base_old_acno( sMq_tax_payment.pay_acct ); /* 对旧帐号的处理 */
    fprintf(fw,"  付款帐号      : <%s><%s>\n",tmp_str,sMq_tax_payment.pay_acct);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>60) j=60; tmp_str[j]=0;
    strcpy(sMq_tax_payment.handorg_name, tmp_str);  /* 缴款单位名称 */
    fprintf(fw,"  缴款单位名称  : <%s>\n",tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_tax_payment.tra_amt = atof(tmp_str);             /* 应扣金额 */
    fprintf(fw,"  扣款金额      : <%s>\n",tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++];
	if(j>18) j=18; tmp_str[j]=0;
    strcpy(sMq_tax_payment.taxvou_no, tmp_str);     /* 税票号码     */
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
    fprintf(fw,"  税款条目      : <%s>\n",tmp_str);

    strcpy(sMq_tax_payment_mx.tra_no, sMq_tax_payment.tra_no);

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

    sMq_tax_payment.tax_date = g_pub_tx.tx_date ;  /* 扣税日期     */
    sMq_tax_payment.tx_amt   = 0.0    ;            /* 实扣金额     */
    sMq_tax_payment.seq_no   = 0      ;            /* 扣款流水     */
    strcpy(sMq_tax_payment.result , "NNNNN");      /* 处理结果     */
    strcpy(sMq_tax_payment.addword , "");          /* 附言代码     */

    sMq_tax_payment.chk_date  = 0 ;                /* 对帐日期     */
    strcpy(sMq_tax_payment.chk_ord , "" );      /* 对帐批次     */
    strcpy(sMq_tax_payment.sts,"0" ); /*标志0-未扣1-撤或冲2-已扣款 3-已对帐*/

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

  ret = sql_commit();
  if(ret){
    sprintf(acErrMsg,"财税库批量请求提交失败(%d)",ret); WRITEMSG
	goto ErrExit;
  }
  sprintf(acErrMsg,"财税库批量请求提交成功"); WRITEMSG
  fprintf(fw,"  批量插入记录成功!\n");

  sql_begin();
  ret = spH803_kk() ;

  if(ret) goto ErrExit;

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库批量成功![%s]",g_pub_tx.reply);
  WRITEMSG
  strcpy(ret_result , "90000");
  strcpy(ret_msg    , "批量扣税成功");

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
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
  sprintf(acErrMsg,"财税库批量请求失败![%s]",g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "批量请求错(%s)", g_pub_tx.reply);
  }

  sprintf(acErrMsg,"sgq:<%s><%s>",ret_result, ret_msg);
  WRITEMSG

  set_zd_data  ("0620" , "no" );    /* 报文标识号 */
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


/* 批量扣税 */
int spH803_kk()
{
  int ret = 0;
  int ret1 = 0;
  char cReturn[51];
  char execstr[248];
  
  int iErr_flag = 0;   /*  0－处理正确；1－处理失败  */
  int iErr_flag1 = 0;  /*  0－处理正确；1－处理失败  */
  
  long cif_no = 0;
  char xs_acno[20];
  char jz_acno[20];
  int jz_ac_seq = 0;
  
  long lSnum = 0;    /*成功笔数*/
  long lFnum = 0;    /*失败笔数*/
  double dSamt = 0.00;  /*成功金额*/
  double dFamt = 0.00;  /*失败金额*/

  int i=0;

  char ret_result[6], ret_msg[61]; /* 返回结果代码及信息 */
  char tmp_str[81], f_name[51];
  

  struct com_branch_c         sCom_branch;
  struct mdm_ac_rel_c         sMdm_ac_rel;         
  struct mq_acct_no_c         sMq_acct_no  ;        /*帐户对照表  */
  struct mq_tax_head_c        sMq_tax_head ;        /*扣税信息表头*/
  struct mq_tax_info_c        sMq_tax_info ;        /*个人信息表*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*扣税信息表体*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*税款条目信息表体*/
  struct dd_mst_c             sDd_mst ;             /* 活期存款   */
  struct rsp_code_c           sRsp_code ;           /*  */

  /**------- 初始化全局变量 --------**/
  pub_base_sysinit();
  
  /*-------- 取输入变量  ---------*/
  memset( &sMq_tax_head, 0x0, sizeof(sMq_tax_head) );
  get_zd_data  ("0330" , sMq_tax_head.src        );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_tax_head.msg_id     );      /* 报文标识号 */

  sprintf(f_name,"/home/zzccb/tips_hz.txt");
  fw1 = fopen (f_name,"w");

  fprintf(fw,"  批量扣税开始\n");

  ret = Mq_tax_head_Sel( g_pub_tx.reply, &sMq_tax_head, \
    "src = '%s' and msg_id = '%s' ",\
	sMq_tax_head.src, sMq_tax_head.msg_id);
  if ( ret )
  {
    sprintf(acErrMsg,"查找 mq_tax_head 出错(%s,%s,%s)[%d]",
	sMq_tax_head.taxorg_code, sMq_tax_head.msg_id, sMq_tax_head.pack_no,ret );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
  }

  /*------------ 处理结果文件 -----------*/
  
  ret = Mq_tax_payment_Dec_Upd_hold( g_pub_tx.reply,
    "taxorg_code= '%s' and entrust_date = %d and pack_no='%s'",\
	sMq_tax_head.taxorg_code, sMq_tax_head.entrust_date,sMq_tax_head.pack_no);
  if ( ret )
  {
    sprintf(acErrMsg,"执行Mq_tax_payment_Dec_Upd_hold出错[%d]", ret );
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
      sprintf(acErrMsg,"执行Mq_tax_payment_Fet_Sel出错[%d]", ret );
      WRITEMSG
      goto ErrExit_kk;
    }
    
    /**------------ 每次循环都重新初始化全局变量 -------------**/
    memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
    pub_base_sysinit();    
  
    fprintf(fw,"    扣税:<%s><%s>",sMq_tax_payment.pay_acct,\
	sMq_tax_payment.tra_no);

    memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
    ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
        	"ac_no = '%s'", sMq_tax_payment.pay_acct );
    if ( ret1 )
    {
   	  sprintf( acErrMsg, "查找账号[%s]信息错", sMq_tax_payment.pay_acct );
   	  WRITEMSG
   	  iErr_flag = 1;
      strcpy(sMq_tax_payment.result , "24002");     /* 处理结果     */
      if(ret1 == 100) strcpy(sMq_tax_payment.addword,"帐号不存在"); /* 附言*/
      else strcpy(sMq_tax_payment.addword , "查找帐号错");  /* 附言  */
      goto TrLog;
    }
        
    memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
    ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
      	"ac_id = %ld", sMdm_ac_rel.ac_id );
    if ( ret1 )
    {
      sprintf( acErrMsg, "查找账号[%s]活期信息错", sMq_tax_payment.pay_acct );
      WRITEMSG
      iErr_flag = 1;
      strcpy(sMq_tax_payment.result , "24002");     /* 处理结果     */
      if(ret1 == 100) strcpy(sMq_tax_payment.addword,"帐号不存在"); /* 附言*/
      else strcpy(sMq_tax_payment.addword , "查找帐号错");  /* 附言  */
      goto TrLog;
    }
        
    pub_base_strpack( sDd_mst.ac_sts );
    if( sDd_mst.ac_sts[0] != '1'||sDd_mst.hold_sts[0]=='1')/*销户和完全冻结*/
    {
       	strcpy( cReturn, "帐户状态错" );
       	sprintf(acErrMsg,"帐户状态错");
       	WRITEMSG
      	iErr_flag = 1;
        sprintf(ret_msg ,"帐户状态错");
        strcpy(sMq_tax_payment.result , "24002");     /* 处理结果     */
        strcpy(sMq_tax_payment.addword , "帐户状态错");  /* 附言  */
       	goto TrLog;
    }
    ret1 = diff_custname(sDd_mst.name, sMq_tax_payment.handorg_name) ;
    if(ret1){
      strcpy(g_pub_tx.reply , "D101");
      strcpy (ret_result , "24002" ); 
      sprintf(ret_msg ,"帐号户名不符%s(%s,%s)",sMq_tax_payment.pay_acct, 
	    sDd_mst.name, sMq_tax_payment.handorg_name);
      sprintf(acErrMsg ,"帐号户名不符%s(%s,%s)",sMq_tax_payment.pay_acct, 
        sDd_mst.name, sMq_tax_payment.handorg_name);
      WRITEMSG
      strcpy(sMq_tax_payment.result , "24002");     /* 处理结果     */
      strcpy(sMq_tax_payment.addword , "帐号户名不符");  /* 附言  */
      iErr_flag = 1;
      goto TrLog;
    }
      
    sprintf(acErrMsg,"-----------开始活期存取款(%s)", sMq_tax_payment.pay_acct);
    WRITEMSG
  
    strcpy( g_pub_tx.ac_no, sMq_tax_payment.pay_acct );
    g_pub_tx.ac_seqn = 1;
    
    g_pub_tx.tx_amt1 = sMq_tax_payment.tra_amt ;
  
    strcpy(g_pub_tx.add_ind,"0");  /*- 扣款 -*/
      
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

    strcpy(g_pub_tx.brf, "扣税款");
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
      sprintf(acErrMsg,"调用存取款主控失败!");
      WRITEMSG
      iErr_flag = 1;
	  strcpy(sMq_tax_payment.result , "24001");
	  strcpy(sMq_tax_payment.addword, "调用存取款主控失败");
      goto TrLog;
    }
    
    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
    if ( ret )
    {
      sql_rollback();
      sql_begin();
      sprintf( cReturn, "%s", g_pub_tx.reply );
      sprintf(acErrMsg,"调用存取款主控失败!"); WRITEMSG
      iErr_flag = 1;
	  strcpy(sMq_tax_payment.result , "24001");
	  strcpy(sMq_tax_payment.addword, "调用存取款主控失败");
      goto TrLog;
    }

    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.tx_amt1 = 0.00;        /*发生额*/
    strcpy( g_pub_tx.ac_id_type,"0" ); /*账户类型设为活期*/

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
		fprintf(fw,"扣款提交失败!\n");
		fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
		fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
		fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
		fprintf(fw1,"%d|99999|修改记录失败(%d)|\n", g_pub_tx.tx_date, ret );
	  }
	  else{
        sql_commit();
		fprintf(fw,"扣款成功!\n");
		fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
		fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
		fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
		fprintf(fw1,"%d|90000|扣款成功|\n", g_pub_tx.tx_date );
	  }

     /* 写文本 */ 
    }
    else { /* 失败 */
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
        sprintf(acErrMsg,"修改 mq_tax_payment (%d)", ret); WRITEMSG
        sql_rollback();
	    fprintf(fw,"<修改记录> ");
	  }
      else{
		sql_commit();
	  }
	  fprintf(fw,"扣款失败!<%s>\n",sMq_tax_payment.addword);

	  fprintf(fw1,"%s|", sMq_tax_payment.tra_no);
	  fprintf(fw1,"%.2f|", sMq_tax_payment.tra_amt);
	  fprintf(fw1,"%s|", sMq_tax_payment.taxvou_no);
	  fprintf(fw1,"%d|%s|%s|\n", g_pub_tx.tx_date,
	  sMq_tax_payment.result, sMq_tax_payment.addword );
    }
	/* 修改记录 */
  	sql_begin();
  }
  Mq_tax_payment_Clo_Upd_hold();  

  /* 修改扣税头文件mq_tax_head */

  ret = sql_execute( "update mq_tax_head set succ_num = %d,\
	    succ_amt=%.2f, addword='扣款完成' where src='%s' and msg_id = '%s' \
		and pack_no='%s' ",lSnum, dSamt,\
		sMq_tax_head.src, sMq_tax_head.msg_id, sMq_tax_head.pack_no );
  if(ret){
    sprintf(acErrMsg,"修改 mq_tax_headt (%d)", ret); WRITEMSG
    sql_rollback();
    fprintf(fw,"    修改mq_tax_head记录错 <%d>\n", ret);
  }
  else  fprintf(fw,"    修改mq_tax_head记录成功!\n");

  if ( lSnum > 0 )
  {
    /* 判断收款帐号对应的内部帐号  */
strcpy(sMq_acct_no.ac_no,"5000038900010");
strcpy(sMq_tax_head.payee_acct,"5000038900010");
memset(ret_msg, 0x00, sizeof(ret_msg));

    ret = Mq_acct_no_Sel( g_pub_tx.reply, &sMq_acct_no , \
      "payee_acct= '%s' ", sMq_tax_head.payee_acct );
    if ( ret )
    {
      strcpy(g_pub_tx.reply,"D101");
	  sprintf(acErrMsg,"查找财政帐号与内部帐号对应错[%d]!", ret);
	  WRITEMSG
      sprintf(ret_msg ,"查找财政帐号与内部帐号对应错(%d)", ret);
      strcpy(ret_result,"94054");
	  goto ErrExit_kk;
    }
    pub_base_sysinit();
    
    sprintf(acErrMsg,"ssggqq开始单位账号存款(%s)",sMq_acct_no.ac_no); WRITEMSG
    g_pub_tx.ac_id = 0;   /*test clean */
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.beg_note_no,"");
          
    strcpy( g_pub_tx.ac_no, sMq_acct_no.ac_no );
    g_pub_tx.tx_amt1 = dSamt;
          
    if ( sMq_acct_no.ac_no[0] == '9' )  /*-- 内部账 --*/
    {
      strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
      strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
      g_pub_tx.svc_ind=9001;
      
      strcpy(g_pub_tx.add_ind,"1");	/*-- 代缴 --*/
        
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "A017" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      
      set_zd_data( "1214", "01" );
      set_zd_data( "1215", "2" );
      set_zd_double( "1218", dSamt );
      strcpy(g_pub_tx.brf, "代收税款");
    }
    else
    {
      strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为活期*/
      g_pub_tx.svc_ind=1001;

      strcpy(g_pub_tx.add_ind,"1");	/*-- 代缴 --*/
        
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "D099" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
        
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "1017", "0" );
          
      set_zd_data( "101A", "01" );
      set_zd_data( "101B", "2" );
      set_zd_double( "1013", dSamt );
       
      strcpy(g_pub_tx.brf, "代收税款");
        
      WRITEMSG
    }
    
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
    strcpy(g_pub_tx.hst_ind,"1");
    
    ret=pub_acct_trance();
    if( ret )
    {
      sprintf(acErrMsg,"调用存取款主控失败!");
      WRITEMSG
      goto ErrExit_kk;
    }
    
    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
    if ( ret )
    {
      sprintf(acErrMsg,"记单位会计账出错[%d]!",ret);
      WRITEMSG
	  fprintf(fw,"    记单位会计账出错<%s>[%d]!\n", g_pub_tx.ac_no,ret);
      goto ErrExit_kk;
    }
	fprintf(fw,"    记单位会计账成功<%s>!\n", g_pub_tx.ac_no,ret);
    
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
  fprintf(fw,"  批量扣税成功!\n");
  sprintf( acErrMsg, "批量扣税业务处理完毕！" ); WRITEMSG
  set_zd_data( DC_REPLY, g_pub_tx.reply );
  return 0;
    
ErrExit_kk:
  fclose(fw1);

  fw1 = fopen(file_name,"w");
  fclose(fw1);

  Mq_tax_payment_Clo_Sel();
  sprintf(acErrMsg,"日终批量扣税业务处理失败![%s]", g_pub_tx.reply );
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"  批量扣税失败\n");
  return 1;
}
