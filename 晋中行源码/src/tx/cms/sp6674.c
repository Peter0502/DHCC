/*************************************************
* 文 件 名:  sp6674.c
* 功能描述： 贷款信息查询
*
* 作    者:  李吉祥
* 完成日期： 20110721
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:
* 说明：tx_code='6674' sub_tx_code='6674'
        insert into tx_def values ('6674', '信贷贷款信息查询', '10000000000000000000000000000100000000111100000000000000000000000010000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
        insert into tx_flow_def values('6674','0','6674','#$');
        insert into tx_sub_def values ('6674', '信贷贷款信息查询', 'sp6674', '0', '0');
        输入：0300域获得账号 char24 
        输出：0390贷款余额 0400表内欠息 0410表外欠息 0420欠复利利息 0670 账户状态
*************************************************/
#define EXTERN
#include "public.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
int sp6674(){
	int ret=0;
	struct ln_mst_c m_ln_mst;
	struct mdm_ac_rel_c m_mdm_ac_rel;
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&m_ln_mst,0,sizeof(m_ln_mst));
	memset(&m_mdm_ac_rel,0,sizeof(m_mdm_ac_rel));
	
	get_zd_data("0300",m_mdm_ac_rel.ac_no);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&m_mdm_ac_rel,"ac_no='%s' ",m_mdm_ac_rel.ac_no);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*表示没有查到结果*/
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
			goto ErrExit;
		}
	}
	vtcp_log("[%s][%d] m_mdm_ac_rel.ac_id = [%d]\n",__FILE__,__LINE__,m_mdm_ac_rel.ac_id);
	ret=Ln_mst_Sel(g_pub_tx.reply,&m_ln_mst,"ac_id='%d' and ac_seqn='%d'",m_mdm_ac_rel.ac_id,m_mdm_ac_rel.ac_seqn);
	if(ret){
		if(ret==100){
			strcpy( g_pub_tx.reply, "XD00" );
			set_zd_data("0130","XD00");/*表示没有查到结果*/
			goto ErrExit;
		}
		else{
			set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
			goto ErrExit;
		}
	}
	set_zd_double("0390",m_ln_mst.bal);
	set_zd_double("0400",m_ln_mst.in_lo_intst);
	set_zd_double("0410",m_ln_mst.out_lo_intst);
	set_zd_double("0420",m_ln_mst.cmpd_lo_intst);
	set_zd_data("0670",m_ln_mst.ac_sts);/*add by gong 20110831 晋中信贷要求返回这个域*/
	vtcp_log("[%s][%d] m_ln_mst.ac_sts = [%s]\n",__FILE__,__LINE__,m_ln_mst.ac_sts);
	set_zd_data("0130","0000");
	
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
