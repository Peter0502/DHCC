/*************************************************
* 文 件 名: spLT12.c
* 功能描述：   
*              旧贷款欠息归还交易 
*
* 作    者:    wanglei
* 完成日期：   2007年05月25日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "intrtmpreg_c.h"

		
spLT12()
{ 
	int    ret=0;
	int    iAc_seqn=0;
	int    iOwe_seqn=0;  /****还款序号****/
	char   cAc_no[20];
	char   cStat[2];     /****还款标志: 1 未还 2 部分还款 3 还清****/
	double dTx_amt=0.00,dOwe_bal=0.00;
	char   newAc_no[20];   /* add by yanqq 20111221 老帐号对应的新帐号 */

	static	struct intrtmpreg_c	intrtmpreg;

	memset (&intrtmpreg, 0x00, sizeof(struct intrtmpreg_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
	memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cStat,0x00,sizeof(cStat));
	memset(newAc_no,0x00,sizeof(newAc_no));   /* add by yanqq 20111221 老帐号对应的新帐号 */
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	vtcp_log("[%s][%d]>>>>>>>>",__FILE__,__LINE__);
	/** 取值、赋值 **/
	get_zd_data("0300",cAc_no);
	get_zd_data("0300",newAc_no);    /* add by yanqq 20111221 老帐号对应的新帐号 */
	vtcp_log("[%s][%d]输入帐号：ac_no===[%s],newAc_no=[%s]",__FILE__,__LINE__,cAc_no,newAc_no);
	get_zd_int("0340",&iAc_seqn);
	vtcp_log("[%s][%d]ac_seqn===[%d]",__FILE__,__LINE__,iAc_seqn);
	get_zd_int("0770",&iOwe_seqn);   /****欠款序号****/
	vtcp_log("[%s][%d]seq===[%d]",__FILE__,__LINE__,iOwe_seqn);
	get_zd_double("0400",&dTx_amt);  /****还欠息金额****/
	vtcp_log("[%s][%d]tx_amt===[%.2lf]",__FILE__,__LINE__,dTx_amt);

	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	vtcp_log("[%s][%d]ac_no===[%s]",__FILE__,__LINE__,cAc_no);
	vtcp_log("[%s][%d]ac_seqn===[%d]",__FILE__,__LINE__,iAc_seqn);
	vtcp_log("[%s][%d]tx_amt===[%.2lf]",__FILE__,__LINE__,dTx_amt);

	/* add beg by yanqq 20111221 老帐号新旧帐号转换 */
	pub_base_old_acno( newAc_no );           /****对老帐号处理****/
	vtcp_log("[%s][%d]新旧帐号转换：ac_no===[%s],newAc_no=[%s]",__FILE__,__LINE__,cAc_no,newAc_no);
	/* add end  by yanqq 20111221 */

	/* 取贷款信息 */
	/* ret = pub_ln_lndis_file(cAc_no,iAc_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); del by yanqq 20111221 对应的新帐号进行检查 */
	ret = pub_ln_lndis_file(newAc_no,iAc_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret )
		goto ErrExit;

	ret = Intrtmpreg_Sel(g_pub_tx.reply,&intrtmpreg,"actno = '%s' and actnoseq = %d",cAc_no,iOwe_seqn);
	if(ret == 100)
	{
		sprintf(acErrMsg,"老贷款欠息登记簿中无此笔记录![%s]",cAc_no); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"L410");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询老贷款欠息登记簿出错![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	dOwe_bal = intrtmpreg.avbal;  /****得到欠息金额****/

	/****若状态为 '3' 说明本条记录欠息已经还清****/
	if( intrtmpreg.stat[0] == '3')
	{
		sprintf(acErrMsg,"此期欠息已还清!"); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"L411");
		goto ErrExit;
	}


	/* 检查金额是否超限 */
	ret = pub_base_CompDblVal( dOwe_bal , dTx_amt );
	if( ret < 0 )
	{
		sprintf(acErrMsg,"还款金额大于欠款金额[%.2lf][%.2lf]",dOwe_bal,dTx_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B108");
		goto ErrExit;
	}
	else if(ret > 0)
	{
		cStat[0] = '2';
	}
	else
	{
		cStat[0] = '3';
	}

	/****更新欠息登记簿的金额和状态****/
	ret = Intrtmpreg_Dec_Upd(g_pub_tx.reply,"actno = '%s' and actnoseq = %d",cAc_no,iOwe_seqn);
	if(ret )
	{
		sprintf(acErrMsg,"查询老贷款欠息登记簿出错![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Intrtmpreg_Fet_Upd(&intrtmpreg,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"查询老贷款欠息登记簿出错![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	strcpy(intrtmpreg.actno,cAc_no);
	intrtmpreg.actnoseq = iOwe_seqn;
	strcpy(intrtmpreg.stat,cStat);
	intrtmpreg.avbal = dOwe_bal - dTx_amt;

	ret = Intrtmpreg_Upd_Upd(intrtmpreg,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"更新老贷款欠息登记簿出错![%d]",ret); 
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
	Intrtmpreg_Clo_Upd();


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"老贷款欠息还款成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"老贷款欠息还款失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

