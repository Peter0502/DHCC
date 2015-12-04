/*************************************************
* 文 件 名:  spB703.c
* 功能描述： 证券回购借出还款记账
*
* 作    者:  
* 完成日期： 2003年1月29日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"

int spB703()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
    struct in_mst_c sIn_mst;
	struct com_parm_c sCom_parm;
	struct mo_city_c mo_city_c;
    char flag[2],zzlx[2];
	double lx=0.00;
	double tmpjs=0.00;
	long   tmp_date=0,tmp_trace_no=0;
	double tmp_amt=0.00;
    int ret;
  
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
    memset(flag,0x0,sizeof(flag));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);
    get_zd_data("0690",flag);
    get_zd_data("0720",zzlx);
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);

    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
      " pact_no='%s' and buy_type='1' and sts in ('2','3')",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询证券回购协议登记簿错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 检查证券回购借出账户余额和交易金额的大小 */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,g_pub_tx.tx_amt1) == -1)
    {
        sprintf(acErrMsg,"交易金额大于证券回购借出帐户余额!!");
        WRITEMSG
        goto ErrExit;
    }
    
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.ct_ind,flag);

    g_pub_tx.add_ind[0] = '1'; /** 贷(不是加) **/
    g_pub_tx.ac_id = 0;

    memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
    memset(g_pub_tx.name,0x0,sizeof(g_pub_tx.name));
    g_pub_tx.ac_id = sMo_stok_rebuy_prot.ac_id;
	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%ld' ",g_pub_tx.ac_id);
	if (ret)
	{
        sprintf(acErrMsg,"取内部主文件错误出错!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"W023");
        goto ErrExit;
	}
    if (pub_base_CompDblVal(g_in_mst.bal,g_pub_tx.tx_amt1) == 0)
    {
		g_pub_tx.svc_ind=9002;
        strcpy(sMo_stok_rebuy_prot.sts,"*");
    }
	else
		g_pub_tx.svc_ind=9001;

vtcp_log("[%s][%d]---------g_pub_tx.ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"对证券回购借出帐户进行帐务处理出错!!!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"pub_acct_trance() pass!!!!! bal=[%lf]",g_in_mst.bal);
    WRITEMSG
    set_zd_double("0390",g_in_mst.bal);

    sMo_stok_rebuy_prot.bal -=g_pub_tx.tx_amt1;
    /* 修改证券回购协议登记簿 */
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"br_no='%s' and pact_no='%s' and buy_type='1'",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"DECLARE for update error [%d] ",ret);
       	WRITEMSG
       	goto ErrExit;
    }   

    ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,g_pub_tx.reply);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"Fetch error [%s[[%s][%d] ",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no,ret);
       	WRITEMSG
       	Mo_stok_rebuy_prot_Clo_Upd();
       	goto ErrExit;
    }
	/* added by liuxuan 2006-12-1 */
	/*sMo_stok_rebuy_prot.sts[0]='*';*/
	/* added over */
    ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"Update error [%d]",ret);
       	WRITEMSG
       	Mo_stok_rebuy_prot_Clo_Upd();
       	goto ErrExit;
    }
    Mo_stok_rebuy_prot_Clo_Upd();

	{
		/*** 计算利息应该减少多少积数 ***/
		get_zd_double("0410",&lx);
    	if (pub_base_CompDblVal(lx,0.00) == 0)
			goto AAAAA;
		tmpjs=floor(lx*L360*L100/g_in_mst.rate);
		vtcp_log("intst_acm=[%lf],tmpjs=[%lf]",g_in_mst.intst_acm,tmpjs);
		g_in_mst.intst_acm=g_in_mst.intst_acm-tmpjs;
		vtcp_log("[%s][%d]下面调用tbl中的函数改in_mst的基数 [%d]",__FILE__,__LINE__,g_in_mst.ac_id);
		vtcp_log("intst_acm=[%lf]",g_in_mst.intst_acm);
    	if (pub_base_CompDblVal(g_in_mst.intst_acm,0.00) <= 0)
			g_in_mst.intst_acm=0.00;
/*	不管基数了
		ret=sql_execute("update in_mst set intst_acm=%lf where ac_id=%d",g_in_mst.intst_acm,g_in_mst.ac_id);
		if( ret )
		{
        	sprintf(acErrMsg,"取内部主文件错误出错!!!");
        	WRITEMSG
			strcpy(g_pub_tx.reply,"W023");
        	goto ErrExit;
		}
*/
	}
AAAAA:
vtcp_log("zzlx is:[%s]",zzlx);
	if ( zzlx[0]=='1' ) 			/*** 老的不用了同城 ***/
	{
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"2"); 	/* 提入 */
		strcpy(mo_city_c.cd_ind,"2");		/* 提入贷 */
		mo_city_c.bill_amt=g_pub_tx.tx_amt1+lx;
		strcpy(mo_city_c.sts,"P");
		strcpy(mo_city_c.bill_type,"1"); /* 票据种类：同城 */
		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		strcpy(mo_city_c.payee_ac_no,g_pub_tx.ac_no);
		strcpy(mo_city_c.payee_name,g_pub_tx.name);
		get_zd_data("0300",mo_city_c.payer_ac_no);
		get_zd_data("0270",mo_city_c.payer_name);
		ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"登记同城登记簿错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
	}
/* 0420 放本金和利息的合计 */
	set_zd_double("0420",g_pub_tx.tx_amt1+lx);
	double tmp_txamt=0;
	get_zd_double("1208",&tmp_txamt);
	vtcp_log("从0420域得到的金额是[%.2f]",tmp_txamt);

    if (flag[0] == '1')
    {
        /* 调用现金子交易 */
        goto OkExitCash;
    }
    else
    {
        /* 调用转帐子交易 */
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
        goto OkExit;
    }

OkExitCash:
    /* 调用现金子交易 */
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit:
    strcpy( g_pub_tx.reply, "0002" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
