/*************************************************
* 文 件 名:  sp6671.c
* 功能描述： 客户存款查询
*
* 作    者:  李吉祥
* 完成日期： 20110720
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:
* 说明：tx_code='6671' sub_tx_code='6671'
        insert into tx_def values ('6671', '信贷客户存款查询', '10000000000000000000000000010000000000100000000000010000000000000000000000000000000000000000000000000000000000000000000000100000', '0', '4', '0');
        insert into tx_flow_def values('6671','0','6671','#$');
        insert into tx_sub_def values ('6671', '信贷客户存款查询', 'sp6671', '0', '0');
        输入：0280域获得客户号(long)，0670域获得要查询余额类型 '0'账面余额，'1'可用余额。0900域（char）查询类型:"0"活期，"1"定期，"2"保证金，"3"委托存款
        输出：0280域客户号保持不变 0520域(int)存款笔数 0390域（double）存款总额 
              1230域 最长975位 将每一组账号与余额按照 账号1|余额1|账号2|余额2|…… 的方式组包发送。
*************************************************/
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "td_mst_c.h"
#include "in_mst_c.h"
#include "cif_basic_inf_c.h"
int sp6671(){
	int ret=0;
	char flag[2]={'a','\0'};/*余额类型。*/
	char flg[2];   /*90域控制标识"0"活期，"1"定期，"2"保证金 ，"3"委托存款20110830*/
	int count=0;/*统计活期存款笔数*/
	int tdcount=0;/*统计定期存款笔数*/
	double sum=0.0;/*总账户余额*/
	char msg[1000];/*发往信贷的账号余额信息*/
	struct dd_mst_c m_dd_mst;/*m_ 开头表示My 私有变量*/
	struct mdm_ac_rel_c m_mdm_ac_rel;
	struct td_mst_c	 m_td_mst;
	struct in_mst_c	 m_in_mst;
	struct cif_basic_inf_c sCif_basic_inf;
	memset(msg,'\0',sizeof(msg));
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_dd_mst,0,sizeof(m_dd_mst));
	memset(&sCif_basic_inf,0,sizeof(sCif_basic_inf));
	memset(&m_td_mst,0,sizeof(m_td_mst));
	memset(&m_in_mst,0,sizeof(m_in_mst));
	memset(&m_mdm_ac_rel,0,sizeof(m_mdm_ac_rel));
	/*char filename[100];
	FILE *fp=NULL;
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );测试用*/
	
	get_zd_long("0280",&m_dd_mst.cif_no);
	get_zd_data("0670",flag);
	get_zd_data("0900",flg);
	vtcp_log("[%s][%d] 前台传过来的flg===[%s]\n",__FILE__,__LINE__,flg);
	sprintf( acErrMsg, "line 48 m_dd_mst.cif_no[%ld] flag[%s]",m_dd_mst.cif_no,flag );
	WRITEMSG
	
	/*m_dd_mst.cif_no=10128189;
	flag='0';测试用*/
	if ( flg[0]=='0')
	{
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
			goto ErrExit;
		}
		int j=0;/*控制sprintf用*/
		double m_bal=0.0;/*存储余额用*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*表示没有查到结果*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*表示没有查到结果 理论上不会存在有ac_id但是没有ac_no的情况*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*账面余额*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*账面余额-冻结金额-预授权金额*/
				}
				else{
					sprintf(acErrMsg,"flag标志错误，执行失败");
					WRITEMSG
					set_zd_data("0130","ERROR[flag标志错误]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();
	}else if (flg[0]=='1'){
		ret=Td_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
			goto ErrExit;
		}
		int j=0;/*控制sprintf用*/
		double m_bal=0.0;/*存储余额用*/
		while(1){
			ret=Td_mst_Fet_Sel(&m_td_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!tdcount){
						set_zd_data("0130","XD00");/*表示没有查到结果*/
					}
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_td_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*表示没有查到结果 理论上不会存在有ac_id但是没有ac_no的情况*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_td_mst.bal;/*账面余额*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_td_mst.bal-m_td_mst.hold_amt-m_td_mst.ctl_amt;/*账面余额-冻结金额-预授权金额*/
				}
				else{
					sprintf(acErrMsg,"flag标志错误，执行失败");
					WRITEMSG
					set_zd_data("0130","ERROR[flag标志错误]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			tdcount+=1;
			sum+=m_bal;
		}
		Td_mst_Clo_Sel();
	}else if (flg[0]=='2'){
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' and prdt_no in ('131','142') ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","没有查找到符合条件的数据");/*表示程序执行失败*/
			goto ErrExit;
		}
		int j=0;/*控制sprintf用*/
		double m_bal=0.0;/*存储余额用*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*表示没有查到结果*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*表示没有查到结果 理论上不会存在有ac_id但是没有ac_no的情况*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*账面余额*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*账面余额-冻结金额-预授权金额*/
				}
				else{
					sprintf(acErrMsg,"flag标志错误，执行失败");
					WRITEMSG
					set_zd_data("0130","ERROR[flag标志错误]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();
	 
	}else if (flg[0]=='3'){
		vtcp_log("[%s][%d]取得flag为3的值",__FILE__,__LINE__);
		ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld and ac_sts='1' and prdt_no in ('133','161','162','163','164','165','166','167','168','169','170') ",m_dd_mst.cif_no);
		if(ret){
			sprintf( acErrMsg, "sql error" );
			WRITEMSG
			set_zd_data("0130","没有查找到符合条件的数据");/*表示程序执行失败*/
			goto ErrExit;
		}
		int j=0;/*控制sprintf用*/
		double m_bal=0.0;/*存储余额用*/
		while(1){
			ret=Dd_mst_Fet_Sel(&m_dd_mst,g_pub_tx.reply);
			if(ret){
				if(ret==100){
					if(!count){
						set_zd_data("0130","XD00");/*表示没有查到结果*/
					}					
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_id=%d ",m_dd_mst.ac_id);
			if(ret){
				if(ret==100){				
					set_zd_data("0130","XD00");/*表示没有查到结果 理论上不会存在有ac_id但是没有ac_no的情况*/							
					break;
				}
				else{
					sprintf(acErrMsg,"sql error");
					WRITEMSG
					set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			if(flag[0]=='0'){
				m_bal=m_dd_mst.bal;/*账面余额*/
			}
			else{
				if(flag[0]=='1'){
					m_bal=m_dd_mst.bal-m_dd_mst.hold_amt-m_dd_mst.ctl_amt;/*账面余额-冻结金额-预授权金额*/
				}
				else{
					sprintf(acErrMsg,"flag标志错误，执行失败");
					WRITEMSG
					set_zd_data("0130","ERROR[flag标志错误]");/*表示程序执行失败*/
					Dd_mst_Clo_Sel();
					goto ErrExit;
				}
			}
			j+=sprintf(msg+j,"%s|%.2lf|",m_mdm_ac_rel.ac_no,m_bal);
			count+=1;
			sum+=m_bal;
		}
		Dd_mst_Clo_Sel();

	}
	if(count+tdcount){
	  set_zd_data("1230",msg);/*由于1230域有900多位，因此假定不会存在溢出，故不去判断msg的长度（j的值）溢出则怪老天吧。*/
	  set_zd_double("0390",sum);
	  set_zd_long("0520",count+tdcount);	
	  set_zd_data("0130","0000");
  }else{
  	strcpy( g_pub_tx.reply, "XD00" );
  	set_zd_data("0130","XD00");
  	goto ErrExit;
  }
	/*fprintf(fp,"MSG[%s]。",msg);
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");测试用*/
	
OKExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
