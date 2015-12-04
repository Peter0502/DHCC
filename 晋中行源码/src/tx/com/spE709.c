/*************************************************
* 文 件 名:  spE709.c
* 功能描述： 跨机构错帐处理交易
*
* 作    者:  wanglei
* 完成日期： 2007年6月24日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "com_branch_c.h"

int spE709()
{
	int	ret;
	char tmp_ind1[2],tmp_add[2];

	/* 初始化结构 */
	struct com_branch_c sCom_branch;

	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset(&sCom_branch,0,sizeof(sCom_branch) );
	pub_base_sysinit();

	get_zd_long("0440",&g_pub_tx.erro_date);	/*错帐日期*/
	get_zd_data("0910",g_pub_tx.opn_br_no);	    /*机构编码*/
	get_zd_data("0670",tmp_ind1);				/*处理方式1-充正2-补记*/
	get_zd_data("0680",tmp_add);				/*借贷类型1-借2-贷*/
vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	get_zd_data("0300",g_pub_tx.ac_no);			/*帐号*/
vtcp_log("[%s][%d]ACTNO=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*帐号序号*/
vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/*金额*/
vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	
	g_pub_tx.ct_ind[0] = '2';  /****此交易只需处理转帐的情况****/

	if( tmp_ind1[0]=='1' )
		strcpy(g_pub_tx.brf,"冲正");
	else if( tmp_ind1[0]=='2' )
		strcpy(g_pub_tx.brf,"补记");
	else
	{
    	sprintf(acErrMsg,"冲正补记处理方式出错[%s]!!",tmp_ind1);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"P442");
		goto ErrExit;
	}
	strcpy(g_pub_tx.hst_ind,"1");		/*日间入明细*/
	
	vtcp_log("[%s][%d] 机构号 = [%s]\n",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	ret = Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no = %s ",g_pub_tx.opn_br_no);
	if(ret == 100)
	{
    	sprintf(acErrMsg,"无此机构信息[%s]!!",g_pub_tx.opn_br_no);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"S019");
		goto ErrExit;
	}
	else if(ret != 0)
	{
    	sprintf(acErrMsg,"查询机构信息异常[%s]!!",g_pub_tx.opn_br_no);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
	if( tmp_add[0]=='1' )
		strcpy(g_pub_tx.add_ind,"0");
	else if( tmp_add[0]=='2' )
		strcpy(g_pub_tx.add_ind,"1");
	else
	{
    	sprintf(acErrMsg,"借贷标志错[%s]!!",tmp_ind1);
    	WRITEMSG
		strcpy(g_pub_tx.reply,"P404");
		goto ErrExit;
	}

vtcp_log("CTIND[%s]",g_pub_tx.ct_ind);
vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	ret=pub_acct_redo(tmp_ind1);
	if(ret)
	{
    	sprintf(acErrMsg,"调用冲正/补记函数出错[%s]!!",g_pub_tx.reply);
    	WRITEMSG
		goto ErrExit;
	}

vtcp_log("HAO---LOOK tx_mat1=[%lf]",g_pub_tx.tx_amt1);
	set_zd_data("0210",g_pub_tx.cur_no);
	set_zd_double("0410",g_pub_tx.tx_amt1);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
