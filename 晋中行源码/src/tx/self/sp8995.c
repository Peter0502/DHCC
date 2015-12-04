/***************************************************************************
 *** 程序作者 : wy                                                       ***
 *** 日    期 : 2013-12-17                                               ***
 *** 所属模块 : 二代支付                                                 ***
 *** 程序名称 : sp89955555.c                                                 ***
 *** 程序作用 : 二代支付信息补登记trace_log                              ***
 *** 使用注意 :                                                          ***
 insert into tx_def  values ('8995', '二代支付信息补登记', '10010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0', '3', null);
 insert into tx_flow_def values('8995',0,'8995','#$')
 insert into tx_sub_def  values ('8995', '二代支付信息补登记', 'sp8995', '0', '0');
 ***************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "trace_log_c.h"
#include "mdm_ac_rel_c.h" 
#include "dd_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "find_debug.h"
#include "dd_parm_c.h"
#include "mo_prot_ac_rel_c.h"
int sp8995() 
{ 
	int ret = 0;
	struct trace_log_c  sTrace_log;
	struct mdm_ac_rel_c sMdm_ac_rel;
	
	memset(&sTrace_log,0x00,sizeof(struct trace_log_c));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	
	if( pub_base_sysinit() )
	{
		vtcp_log("[%s][%d]初始化公用结构错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
  	vtcp_log("%s,%d,二代支付信息补登记开始trace_log.....",__FILE__,__LINE__);
	vtcp_log("[%s][%d] 柜员号==[%s]", __FILE__, __LINE__, g_pub_tx.tel);	
  
	get_zd_data("0380",g_pub_tx.ac_no);
	pub_base_strpack(g_pub_tx.ac_no);
	if(strlen(g_pub_tx.ac_no)!= 0)
	{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if ( 100 == ret ) 
		{
			vtcp_log("[%s] [%d]行\n", __FILE__, __LINE__);
			sprintf(acErrMsg, "无此帐/卡号，请检查!ac_no=[%s]", g_pub_tx.ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		 else if ( ret ) 
		{
			vtcp_log("[%s] [%d]行\n", __FILE__, __LINE__);
			sprintf(acErrMsg, "读取介质账户对照表异常![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "AT06");
			goto ErrExit;
		}
		
		if(sMdm_ac_rel.ac_no[0] == '9') /*内部帐号*/
		{
			struct in_mst_c sIn_mst = {0};
		  ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id = '%ld'",sMdm_ac_rel.ac_id);
		  if(ret) 
		  {
				vtcp_log("%s,%d,In_mst_Sel error![%d]",__FILE__,__LINE__,ret);
				strcpy( g_pub_tx.reply, "B114" );
				goto ErrExit;
		  }
		  strcpy(sTrace_log.prdt_no,sIn_mst.prdt_no);
		}
		else
		{
			struct dd_mst_c sDd_mst = {0};
		  ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = '%ld'",sMdm_ac_rel.ac_id);
		  if(ret) 
		  {
				vtcp_log("%s,%d,Dd_mst_Sel error![%d]",__FILE__,__LINE__,ret);
				strcpy( g_pub_tx.reply, "B114" );
				goto ErrExit;
		  }
		  strcpy(sTrace_log.prdt_no,sDd_mst.prdt_no);
		}
	}	
	
  if(g_pub_tx.trace_no == 0)/**录入,生成流水号 **/
  {
  	ret=pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply);
		if (ret) 
		{
			sprintf(acErrMsg, "调用pub_base_PubQlsh得到新流水号错误![%s][%d]", __FILE__, __LINE__);
			WRITEMSG
			goto ErrExit;
		}
  }
	vtcp_log("[%s][%d] 流水号==[%ld]", __FILE__, __LINE__, g_pub_tx.trace_no);
  set_zd_long(DC_TRACE_NO , g_pub_tx.trace_no);
  sTrace_log.trace_no=g_pub_tx.trace_no;
  sTrace_log.tx_date=g_pub_tx.tx_date;
  sTrace_log.tx_time=g_pub_tx.tx_time;
  strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
  strcpy(sTrace_log.opn_br_no,g_pub_tx.opn_br_no);
  strcpy(sTrace_log.tel,g_pub_tx.tel);
  strcpy(sTrace_log.chk,g_pub_tx.chk);
  strcpy(sTrace_log.aut,g_pub_tx.auth);
  strcpy(sTrace_log.tx_code,g_pub_tx.tx_code);
  strcpy(sTrace_log.sub_tx_code,g_pub_tx.sub_tx_code);  
  sTrace_log.svc_ind=g_pub_tx.svc_ind;
  sTrace_log.ct_ind[0]=g_pub_tx.ct_ind[0]; 
  strcpy(sTrace_log.cur_no,"01");
  strcpy(sTrace_log.ac_no,sMdm_ac_rel.ac_no);
  sTrace_log.ac_seqn=sMdm_ac_rel.ac_seqn;
  sTrace_log.ac_id=sMdm_ac_rel.ac_id;
  strcpy(sTrace_log.note_type,g_pub_tx.note_type);
  strcpy(sTrace_log.note_no,g_pub_tx.beg_note_no);
  strcpy(sTrace_log.add_ind,g_pub_tx.add_ind);
  sTrace_log.amt=0.00;
  vtcp_log("%s,%d,输入的摘要为[%s],log.brf[%s]",__FILE__,__LINE__,g_pub_tx.brf,sTrace_log.brf);
  strcpy(sTrace_log.hst_ind,g_pub_tx.hst_ind);
  strcpy(sTrace_log.saving_notarize,"0");  
  strcpy(sTrace_log.sts,"0");
  sTrace_log.no_show[0]='0';  
  g_pub_tx.trace_cnt++;
  sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
  set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
  
  get_zd_data("0810",g_pub_tx.brf);
	strcpy(sTrace_log.brf,g_pub_tx.brf);
  ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
  vtcp_log("[%s][%d]in Trace_log_Ins  === ret = [%d]", __FILE__, __LINE__, ret);
  if ( ret )
  {
  	Trace_log_Debug(&sTrace_log);
  	vtcp_log("=====[%ld]",sTrace_log.trace_no);
  	sprintf(acErrMsg,"登记交易流水错误[%d]!",ret);
  	WRITEMSG
  	strcpy(g_pub_tx.reply,"P126");
  	goto ErrExit;
  }
  
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"登记trace_log成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"登记trace_log失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

