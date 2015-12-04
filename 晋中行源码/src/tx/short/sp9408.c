/*****************************************************
* 文 件 名:  sp9408.c
* 功能描述： 交易流水传票打印
* 作    者:  xyy
* 完成日期： 2006-10-27 15:16
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "tx_def_c.h"
#include "com_branch_c.h"
#define MAX_CNT 20
int sp9408()
{
  int ret=0,i;
  int iCnt=0;
  char cTrace_no[10];
  char cTxday[9];
  char cKinbr[BRNO_LEN+1];
  char cQsbrno[BRNO_LEN+1];
  char cBuff[20000];
  char cTxcode[5];
  char cTmp[31];
  char cbuff[51];
  char cFilename[100];
  char ctrace_no[7];  /* 流水号   */           
  vtcp_log("sp9408 开始！[%s][%d]",__FILE__,__LINE__);
  FILE *fp;
  struct dc_log_c     dc_log;
  struct com_branch_c com_branch;
  struct trace_log_c  trace_log;
  struct tx_def_c     tx_def;
  vtcp_log("sp9408 开始！[%s][%d]",__FILE__,__LINE__);
  struct TOS
  {
      int  cnt;
      char tx_name[20];   /* 交易名称 */
      char tx_br_no[BRNO_LEN];   /* 机构号   */
      char tx_br_name[30];/* 机构名称 */
      long trace_no;      /* 流水号   */           
      char tel[4];        /* 操作员   */  
      struct{
          char   dc_ind[3]; /*借贷标志*/
          char   acc_hrt[6];/*科目号*/
          double txamt;    /* 交易金额 */
      }record[MAX_CNT];
  }tos;
  vtcp_log("sp9408 开始！[%s][%d]",__FILE__,__LINE__);
  memset(ctrace_no, 0 ,sizeof(ctrace_no));
  memset(cQsbrno  , 0 ,sizeof(cQsbrno));
  memset(cbuff    , 0 ,sizeof(cbuff));
  memset(cTmp     , 0 ,sizeof(cTmp));
  memset(cTxcode  , 0 ,sizeof(cTxcode));
  memset(cTrace_no, 0 ,sizeof(cTrace_no));
  memset(cBuff    , 0 ,sizeof(cBuff));
  memset(cFilename, 0 ,sizeof(cFilename));
  memset(cKinbr   , 0 ,sizeof(cKinbr));
  memset(cTxday   , 0 ,sizeof(cTxday));
  
  memset(&tos         ,0x00,sizeof(tos));
  memset(&dc_log      ,0x00,sizeof(struct dc_log_c));
  memset(&com_branch  ,0x00,sizeof(struct com_branch_c));
  memset(&trace_log   ,0x00,sizeof(struct trace_log_c));
  memset(&tx_def      ,0x00,sizeof(struct tx_def_c));
  vtcp_log("sp9408 开始！[%s][%d]",__FILE__,__LINE__);
  	
    
  /*** 初始化全局变量 ***/
  pub_base_sysinit();
  get_fd_data("0030",cQsbrno);
  get_fd_data("0910",cKinbr);
  get_fd_data("0470",cTxday);
  get_fd_data("0790",ctrace_no);
  ret=pub_base_AllDwnFilName(cFilename);
  if(ret)
  {
      vtcp_log("%s,%d 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
  }
  fp=fopen(cFilename,"w");
  if(fp==NULL)
  {
      vtcp_log("%s,%d 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
  }
  /**这里开始写文件**/
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"~交易名称",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*交易名称*/
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"机构号",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.8s|", cTmp);/*机构号*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"机构名称",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*机构名称*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"日期",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.10s|", cTmp);/*日期*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"流水号",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.8s|", cTmp);/*流水号*/
  
  memset(cTmp,0,sizeof(cTmp));
  memcpy(cTmp,"操作员",sizeof(cTmp)-1);
  zip_space(cTmp);
  fprintf(fp, "%.8s|\n",cTmp);/*操作员*/

  sprintf(cbuff," sts='0' and tx_date='%s' ",cTxday);
  zip_space(cKinbr);      
  if(strlen(cKinbr)!=0 && memcmp(cKinbr,"00000",BRNO_LEN))      
  {
     sprintf(cbuff+strlen(cbuff),"and tx_opn_br_no = '%s' ",cKinbr);
  }

  vtcp_log("[%s][%d]trace_no==[%s]",__FILE__,__LINE__,ctrace_no);
  zip_space(ctrace_no);      
  sprintf(ctrace_no,"%ld",atol(ctrace_no));
  vtcp_log("[%s][%d]trace_no==[%s]",__FILE__,__LINE__,ctrace_no);
  if(atol(ctrace_no)!=0)      
  {
     sprintf(cbuff+strlen(cbuff)," and trace_no = '%s' ",ctrace_no);
  }

  strcat(cbuff," order by trace_no");

  vtcp_log("[%s][%d]cbuff==[%s]",__FILE__,__LINE__,cbuff);
/*
  distinct_dec_sel( "dc_log", "trace_no	"," sts='0' and tx_date='%s' order by trace_no",cTxday);
*/
  distinct_dec_sel( "dc_log", "trace_no	","%s",cbuff);
  while(1)
  {
  	  long iTrace_no=0;
  	  long iWssrno=0;
      g_reply_int = distinct_fet_sel(cTrace_no);
      iTrace_no = atol(cTrace_no);
      vtcp_log("sp9408 开始！[%s][%d]iTrace_no===========[%ld]",__FILE__,__LINE__,iTrace_no);
      if(g_reply_int==100) break;
      if(g_reply_int!=0)
      {
           vtcp_log("[%s][%d] error fetch trace_no sqlcode=[%d]",__FILE__,__LINE__,g_reply_int);
           break;
      }
      ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date='%s' and trace_no='%ld' order by  trace_no",cTxday,iTrace_no);
      if ( ret )
      {
	    	sprintf(acErrMsg,"没有[%s]机构[%s]的传票信息sqlcode=[%d]", cKinbr,cTxday,ret);
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "T060" );
	  	    goto	ErrExit;
      }
      while(1)
      {  
      	  if(iTrace_no==iWssrno)
      	  {
      	  	break;
      	  }
	  	    memset(cTxcode     ,0,sizeof(cTxcode));
	  	    memset(&trace_log,0,sizeof(trace_log));
	  	    ret = Trace_log_Fet_Sel(&trace_log,g_pub_tx.reply);
	  	    if( ret == 100 ) break;
	  	    if ( ret )
	        {
	          	sprintf(acErrMsg,"查询[%s]机构的流水出错ret=[%d]",cKinbr,ret);
	      	    WRITEMSG
	      	    strcpy( g_pub_tx.reply, "T061" );
	      	    goto	ErrExit;
	        }
	        memcpy(cTxcode,trace_log.tx_code,sizeof(cTxcode)-1);
          ret = Dc_log_Dec_Sel(g_pub_tx.reply,"tx_date='%s' and trace_no='%ld' and sts='0' order by amt desc",cTxday,iTrace_no);
          i=0;
          while(1)
          {
            ret = Dc_log_Fet_Sel(&dc_log,g_pub_tx.reply);
            if(ret==100) break;
            if(ret)
            {
                sprintf(acErrMsg, "统计dc_log表错误! [%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            if(dc_log.dc_ind[0]=='1')
            {
                memcpy(tos.record[i].dc_ind,"借:",sizeof(tos.record[i].dc_ind));
            }    
            else
            {
                memcpy(tos.record[i].dc_ind,"贷:",sizeof(tos.record[i].dc_ind));
            }
            zip_space(dc_log.acc_hrt);
            if(strlen(dc_log.acc_hrt)==0)
            {
            	memcpy(tos.record[i].acc_hrt,"      ",sizeof(tos.record[i].acc_hrt));
            }
            else
            {
                memcpy(tos.record[i].acc_hrt,dc_log.acc_hrt,sizeof(tos.record[i].acc_hrt));
            }
            tos.record[i].txamt = dc_log.amt;
            if(i==0)
            {
            	 char cBrno[BRNO_LEN+1];
            	 char cTel[5];
            	 memset(cTel,0,sizeof(cTel));
               memset(cBrno,0,sizeof(cBrno));
               
               memcpy(cBrno,dc_log.tx_opn_br_no,sizeof(cBrno)-1);
               memcpy(cTel,dc_log.tel,sizeof(cTel)-1);
               memcpy(tos.tel,cTel,sizeof(cTel)-1);
               memcpy(tos.tx_br_no,cBrno,sizeof(cBrno)-1);
               tos.trace_no = dc_log.trace_no;
               ret = Com_branch_Sel(g_pub_tx.reply, &com_branch, "br_no='%s'",cBrno);
               if (ret != 0)
               {
               	   sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
               	   WRITEMSG
               	   goto ErrExit;
               }
               pub_base_strpack( com_branch.br_name ); 
               memcpy(tos.tx_br_name,com_branch.br_name,sizeof(tos.tx_br_name));
               
			   pub_base_strpack(cTxcode);
			   if(strlen(cTxcode)>0)
			   {
					   ret = Tx_def_Sel(g_pub_tx.reply, &tx_def, "tx_code='%s'",cTxcode);
					   if (ret != 0)
					   {
						  sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
						  WRITEMSG
						  goto ErrExit;
					   }
					   pub_base_strpack( tx_def.tx_name ); 
					   memcpy(tos.tx_name,tx_def.tx_name,sizeof(tos.tx_name));
			   }else{
					   pub_base_strpack( trace_log.brf); 
					   memcpy(tos.tx_name,trace_log.brf,sizeof(tos.tx_name));
			   }
            }
            i++;
            iWssrno = iTrace_no;
          }
          Dc_log_Clo_Sel();
          /**这里开始写文件**/
          tos.cnt = i;
          fprintf(fp, "%d|", tos.cnt);       /* 循环的次数 */
          fprintf(fp, "%s|", tos.tx_name);   /* 交易名称 */
          fprintf(fp, "%.5s|", tos.tx_br_no);  /* 机构号   */
          fprintf(fp, "%.30s|", tos.tx_br_name);/* 机构名称 */
          fprintf(fp, "%s|", cTxday);        /* 日期     */
          fprintf(fp, "%ld|", tos.trace_no);  /* 流水号   */           
          fprintf(fp, "%.4s|", tos.tel);       /* 操作员   */
          int j=0;
          for(j=0;j<i;j++)
          {
          	fprintf(fp, "%.3s|", tos.record[j].dc_ind);/*借贷标志*/
          	fprintf(fp, "%.6s|", tos.record[j].acc_hrt);/*科目号*/
          	fprintf(fp, "%.2f|",tos.record[j].txamt);    /* 交易金额 */
          }
          fprintf(fp, "|\n");       /* 结束一行 */  
       }
       Trace_log_Clo_Sel();
  }
  distinct_clo_sel();
  fclose(fp);
  set_zd_data( DC_FILE_SND,"2" );
  
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
