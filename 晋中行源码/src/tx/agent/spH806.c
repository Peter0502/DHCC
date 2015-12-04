/***************************************************************
* 文 件 名:   spH803.c
* 功能描述：  财税库与银行信息包核对请求   
*
* 作    者:   mike        
* 完成日期：  2004年11月24日
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

/* 财税库与银行信息包核对申请 */
int spH806()
{
  FILE *fr, *fw ;
  int ret = 0, i, j, k, num , n ;
  char file_name[51];
  char tmp_item_no[6];             /* 扣税帐户科目 */
  char ret_result[6], ret_msg[61], rq_msg[7]; /* 返回结果代码及信息 */
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
  fprintf(fw,"begin 3112-与银行信息包核对\n");
  fprintf(fw,"  交易<6806> <%d-%d> <%d>\n",g_pub_tx.tx_date,\
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

  i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  strcpy(sMq_tax_head.taxorg_code , tmp_str); /* 付款行号 */
  set_zd_data("0600", tmp_str);
  fprintf(fw,"  付款行号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.entrust_date = atol(tmp_str);  /* 对帐日期 */
  set_zd_data("0860", tmp_str);
  fprintf(fw,"  对帐日期      : <%s>\n", tmp_str);

  /* 包流水号 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  包流水号      : <%s>\n", tmp_str);

  /* 子包总数 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  子包总数      : <%s>\n", tmp_str);

  /* 子包序号 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  本包序号      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  num = atoi(tmp_str) ;        /* 本包笔数 */
  fprintf(fw,"  本包笔数      : <%s>\n", tmp_str);

  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  sMq_tax_head.all_amt = atof(tmp_str);        /* 本包金额 */
  fprintf(fw,"  本包金额      : <%s>\n", tmp_str);

  /* 发送包数 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  发送包数      : <%s>\n", tmp_str);

  /* 接收包数 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  接收包数      : <%s>\n", tmp_str);

  /* 止付包数 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  止付包数      : <%s>\n", tmp_str);

  /* 对帐类型 */
  i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
  fprintf(fw,"  对帐类型      : <%s>\n", tmp_str);

  /* 读文件第 2-n 行 */
  for(n=0; n < num; ++n ){
    if(fgets ( buf , 255 , fr )<=0){
      sprintf(acErrMsg,"读文件%s基本信息失败!", file_name);
      WRITEMSG
      strcpy (ret_result , "99004" ); 
      sprintf(ret_msg ,"读文件(%s)基本信息失败", file_name);
      goto ErrExit;
    }

    fprintf(fw,"--- 记录号: %d --- \n", n+1);
    /* 原报文编号 */
    i=0; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  原报文编号    : <%s>\n", tmp_str);

    /* 原发起机构 */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  原发起机构    : <%s>\n", tmp_str);

    /* 原委托日期 */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  原委托日期    : <%s>\n", tmp_str);

    /* 原包流水号 */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  原包流水号    : <%s>\n", tmp_str);

    /* 总笔数     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  总笔数        : <%s>\n", tmp_str);

    /* 总金额     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  总金额        : <%s>\n", tmp_str);

    /* 成功笔数     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  成功笔数      : <%s>\n", tmp_str);

    /* 成功金额     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  成功金额      : <%s>\n", tmp_str);

    /* 止付笔数     */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  止付笔数      : <%s>\n", tmp_str);

    /* 包类型       */
    i++; j=0; while(buf[i]!='|') tmp_str[j++]=buf[i++]; tmp_str[j]=0;
    fprintf(fw,"  包类型        : <%s>\n", tmp_str);
    fprintf(fw,"------------------ \n");
  }

OkExit:

  strcpy(g_pub_tx.reply,"0000");
  sprintf(acErrMsg,"财税库<%s>对帐成功![%s]",rq_msg,g_pub_tx.reply);
  WRITEMSG
  strcpy(ret_result , "90000");
  sprintf(ret_msg , "<%s>对帐成功", rq_msg);

  memset(tmp_str,0x00, sizeof(tmp_str));
  sprintf(tmp_str,"7861%8d%08d", g_pub_tx.tx_date,g_pub_tx.trace_no);
  set_zd_data("0620" , tmp_str );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fclose(fr);

  fprintf(fw,"  返回结果: 包核对正确\n");
  fclose(fw);

  return 0;
ErrExit:
  sprintf(acErrMsg,"财税库<%s>对帐失败![%s]", rq_msg,g_pub_tx.reply);
  WRITEMSG

  if(strlen(ret_result)<4){
	strcpy(ret_result , "99090");
	sprintf(ret_msg , "(%s)对帐错(%s)", rq_msg,g_pub_tx.reply);
  }
  else{
	sprintf(tmp_str ,"(%s)对帐%s", rq_msg,ret_result);
	strcpy(ret_result, tmp_str);
  }

  sprintf(acErrMsg,"mike:(%s)(%s)",ret_result, ret_msg);
  WRITEMSG

  set_zd_data("0620" , "  " );    /* 报文标识号 */
  set_zd_data("0640", ret_result );
  set_zd_data("0740", ret_msg    );

  set_zd_data("0620" , "  " );    /* 报文标识号 */

  set_zd_data(DC_REPLY,g_pub_tx.reply);
  fprintf(fw,"  返回结果: 包核对错误(%s)\n", ret_msg);
  fclose(fr);
  fclose(fw);
  return 1;
}
