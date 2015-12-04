/***************************************************************
* 文 件 名:   spH803.c
* 功能描述：  财税库明细核对请求   
*             处理多个包,前n-1个包只登记,不处理,第n个包再处理及划款
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
#include "com_branch_c.h"
#include "mq_acct_no_c.h"
#include "mq_tax_info_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "mq_tax_payment_mx_c.h"
#include "mq_batch_head_c.h"
#include "mq_batch_mx_c.h"
#include "rsp_code_c.h"

/* 财税库明细核对申请 */
int spH805( )
{
  FILE *fr, *fw ;
  int  ret = 0, i, j, k, num,n, pack_flag=0;
  char file_name[51];
  char tmp_item_no[6];             /* 扣税帐户科目 */
  char ret_result[6], ret_msg[61], rq_msg[7]; /* 返回结果代码及信息 */
  char f_name[31], buf[256], tmp_str[81];

  struct mq_acct_no_c         sMq_acct_no  ;        /*帐户对照表  */
  struct mq_tax_info_c        sMq_tax_info ;        /*个人信息表*/
  struct mq_tax_head_c        sMq_tax_head ;        /*扣税信息表头*/
  struct mq_tax_payment_c     sMq_tax_payment ;     /*扣税信息表体*/
  struct mq_tax_payment_mx_c  sMq_tax_payment_mx ;  /*税款条目信息表体*/
  struct mq_batch_head_c      sMq_batch_head ;      /*对帐信息表头*/
  struct mq_batch_mx_c        sMq_batch_mx ;        /*对帐信息表体*/
  struct dd_mst_c             sDd_mst ;             /*活期存款   */

  memset(&sMq_tax_head,0x0,sizeof(sMq_tax_head));
  memset(&sMq_tax_payment,0x0,sizeof(sMq_tax_payment));
  memset(&sMq_batch_head,0x0,sizeof(sMq_batch_head));
  memset(&sMq_batch_mx,0x0,sizeof(sMq_batch_mx));
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
  fprintf(fw,"begin 3111-扣税明细核对\n");
  fprintf(fw,"  交易<6805> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
  g_pub_tx.tx_time,g_pub_tx.trace_no);

  /*-------- 取输入变量  ---------*/
  get_zd_data  ("0330" , tmp_str               );      /* 发起节点代码 */
  get_zd_data  ("0620" , sMq_batch_head.msg_id );      /* 报文标识号 */

  fprintf(fw,"  节点代码  : <%s> ",  tmp_str) ; 
  fprintf(fw,"  报文标识  : <%s>\n", sMq_batch_head.msg_id);

  get_zd_data ("0964", f_name ); 
  sprintf(file_name,"/home/zzccb/txt/%s",f_name);
  fprintf(fw,"  传输文件名: <%s>\n", file_name);

  fr=fopen(file_name,"r");
  if(fr==NULL||strlen(file_name)<17){
    strcpy(g_pub_tx.reply,"D101");
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

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_batch_head.paybk_code , tmp_str); /* 付款行号 */
  set_zd_data("0600", tmp_str);
  fprintf(fw,"  付款行号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>12) j=12; tmp_str[j]=0;
  strcpy(sMq_batch_head.payee_bankno , tmp_str); /* 收款行号 */
  set_zd_data("0580", tmp_str);
  fprintf(fw,"  收款行号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>8) j=8; tmp_str[j]=0;
  sMq_batch_head.chk_date = atol(tmp_str);  /* 对帐日期 */
  sMq_batch_mx.chk_date   = atol(tmp_str);  /* 对帐日期 */
  set_zd_data("0860", tmp_str);
  fprintf(fw,"  对帐日期      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>4) j=4; tmp_str[j]=0;
  strcpy(sMq_batch_head.chk_ord , tmp_str );     /* 对帐批次 */
  strcpy(sMq_batch_mx.chk_ord   , tmp_str );     /* 对帐批次 */
  set_zd_data("0920", tmp_str);
  fprintf(fw,"  对帐批次      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
  if(j>1) j=1; tmp_str[j]=0;
  strcpy(sMq_batch_head.chk_type , tmp_str );     /* 对帐类型 0-日间 1-日切 */
  if(tmp_str[0]=='0') strcpy(rq_msg,"日间");
  else if(tmp_str[0]=='1') strcpy(rq_msg,"日切");
  else strcpy(rq_msg,"类型错");
  fprintf(fw,"  对帐类型      : <%s><%s>\n", tmp_str,rq_msg);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  总笔数        : <%s>\n", tmp_str);
  sMq_batch_head.all_num =  atoi(tmp_str) ; /* 总笔数 */

  /* 日间对帐且无记录,则返回 */
  if ( sMq_batch_head.all_num < 1 ){
    pack_flag = 1  ;
    goto OkExit;
  }

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.all_amt =  atof(tmp_str) ; /* 总金额  */
  fprintf(fw,"  总金额        : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.child_num =  atoi(tmp_str) ; /* 子包总数 */
  fprintf(fw,"  子包总数      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_batch_head.pack_ord =  atoi(tmp_str) ; /* 子包序号 */
  sMq_batch_mx.pack_ord   =  atoi(tmp_str) ; /* 子包序号 */
  fprintf(fw,"  子包序号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_num = atoi(tmp_str) ;        /* 本包笔数 */
  sMq_batch_head.pack_num =  atoi(tmp_str) ;
  fprintf(fw,"  本包笔数      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* 本包金额 */
  sMq_batch_head.pack_amt =  atof(tmp_str) ;
  fprintf(fw,"  本包金额      : <%s>\n", tmp_str);

  /* 插入记录到对帐表头中 */
  ret = Mq_batch_head_Ins ( sMq_batch_head , g_pub_tx.reply );
  if (ret != 0)
  {
	  strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"插入财税库对帐头表错![%s](sql=%d)",g_pub_tx.reply,ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"插入财税库对帐头表错![%s](sql=%d)",g_pub_tx.reply,ret);
      goto ErrExit;
  }

  /* 读文件第 2-n 行 */
  for(n=0; n < sMq_tax_head.all_num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"读文件%s基本信息失败!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"读文件(%s)基本信息失败", file_name);
      goto ErrExit;
    }

    sprintf(acErrMsg,"line3<%s>\n", buf); WRITEMSG
	fprintf(fw," %d=%s",n+1,buf);

    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>12) j=12; tmp_str[j]=0;
    fprintf(fw,"  付款行开户行号: <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>24) j=24; tmp_str[j]=0;
    strcpy(sMq_batch_mx.pay_acct, tmp_str);      /* 付款帐号     */
    pub_base_old_acno( sMq_batch_mx.pay_acct );  /* 对旧帐号的处理 */
    fprintf(fw,"  付款帐号      : <%s><%s>\n",tmp_str,sMq_batch_mx.pay_acct);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>12) j=12; tmp_str[j]=0;
    strcpy(sMq_batch_mx.taxorg_code, tmp_str);  /* 原征收机关代码  */
    fprintf(fw,"  原征收机关代码: <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    sMq_batch_mx.entrust_date = atol(tmp_str);  /* 原委托日期 */
    fprintf(fw,"  原委托日期    : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_batch_mx.pack_no, tmp_str);  /* 原包流水号 */
    fprintf(fw,"  原包流水号    : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; 
    if(j>8) j=8; tmp_str[j]=0;
    strcpy(sMq_batch_mx.tra_no, tmp_str);  /* 原交易流水号 */
    fprintf(fw,"  原交易流水号  : <%s>\n", tmp_str);

    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    sMq_batch_mx.tra_amt = atof(tmp_str);         /* 交易金额 */
    fprintf(fw,"  交易金额      : <%s>\n", tmp_str);

	strcpy(sMq_batch_mx.flag,"0");

    /* 插入记录到对帐明细中 */
    ret = Mq_batch_mx_Ins(sMq_batch_mx,g_pub_tx.reply);
    if (ret != 0)
    {
      strcpy(g_pub_tx.reply , "D101");
      sprintf(acErrMsg,"插入财税库对帐明细表错!(sql=%d)", ret);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"插入财税库对帐明细表错!(sql=%d)", ret);
      goto ErrExit;
	}
  }
  if(sMq_batch_head.child_num == sMq_batch_head.pack_ord ) pack_flag=1;
  else pack_flag=0;

  /* 子包总数等于子包序号,表示包已传完, 则进行处理,对帐及划款 */
  if(pack_flag == 1 ){
    ret = Mq_batch_mx_Dec_Upd( g_pub_tx.reply,
    "chk_date = %d and chk_ord = '%s' ",\
	 sMq_batch_head.chk_date, sMq_batch_head.chk_ord );
    if ( ret )
    {
      sprintf(acErrMsg,"执行Mq_batch_mx_Dec_Upd出错[%d]", ret );
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
        sprintf(acErrMsg,"执行Mq_batch_mx_Fet_Sel出错[%d]", ret );
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
	  else if(ret == 100){ /* 无记录,则要补扣 */
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
	/* 关闭游标 */
    Mq_batch_mx_Clo_Upd();  

	/* 通过大额支付提出 */

	/* ---------*/

  }

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库<%s>对帐成功![%s]",rq_msg,g_pub_tx.reply);
  WRITEMSG
  if(pack_flag == 1 ){
    strcpy(ret_result , "90000");
	sprintf(ret_msg , "(%s)对帐成功", rq_msg);
  }
  else{
	strcpy(tmp_str,"no");
    set_zd_data  ("0620" , tmp_str );      /* 报文标识号 */
    strcpy(ret_result , "no   ");
	sprintf(ret_msg , "(%s)对帐包接收成功", rq_msg);
  }

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  if(pack_flag==1) fprintf(fw,"  返回结果: 对帐(%s)\n", ret_msg);
  else  fprintf(fw,"  返回结果: 对帐包接收(%s)\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"财税库<%s>对帐失败![%s]", rq_msg,g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	if(pack_flag==1) sprintf(ret_msg , "%s 对帐错", rq_msg);
	else  sprintf(ret_msg , "%s对帐包接收错!", rq_msg);
  }
  else{
	if(pack_flag==1) sprintf(tmp_str ,"%s 对帐错", rq_msg);
	else  sprintf(tmp_str ,"%s对帐包接收错!", rq_msg);
	strcpy(ret_result, tmp_str);
  }

  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data("0620" , "  " );    /* 报文标识号 */

  set_zd_data(DC_REPLY,g_pub_tx.reply);

  if(pack_flag==1) fprintf(fw,"  返回结果: 对帐(%s)\n", ret_msg);
  else  fprintf(fw,"  返回结果: 对帐包接收(%s)\n", ret_msg);
  fprintf(fw,"end\n");
  fclose(fr);
  fclose(fw);

  return 1;
}
