/*************************************************
* 文 件 名:  spE403.c
* 功能描述： 上存记账
*
* 作    者:  mike
* 完成日期： 2004年3月05日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_mst_c.h"
#include "ass_mst_hst_c.h"
#include "ass_gl_c.h"
#include "ass_br_def_c.h"

int spE403()
{
    struct ass_br_def_c ass_br_def;
    struct ass_mst_c ass_mst;
    struct ass_gl_c ass_gl;
    struct ass_mst_hst_c ass_mst_hst;

    int ret;

    memset(&ass_br_def,0x0,sizeof(struct ass_br_def_c));
    memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
    memset(&ass_gl,0x0,sizeof(struct ass_gl_c));
    memset(&ass_mst_hst,0x0,sizeof(struct ass_mst_hst_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O100");
        goto ErrExit;
    }

    get_zd_data("0910",ass_br_def.sub_br_no); 
    get_zd_long("0470",&ass_mst.pact_no);
    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_double("0840",&ass_mst.rate);
    get_zd_double("0850",&ass_mst.over_rate);
    get_zd_long("0440",&ass_mst.end_date);  

    g_pub_tx.rate = ass_mst.rate;
    g_pub_tx.over_rate = ass_mst.over_rate;
    g_pub_tx.mach_date = ass_mst.end_date;
    g_pub_tx.open_date = g_pub_tx.tx_date;
    g_pub_tx.ac_id = ass_mst.pact_no;
    strcpy(g_pub_tx.ac_no,ass_br_def.sub_br_no);
		vtcp_log("ass_mst.rate=[%lf]\r\n,ass_mst.over_rate=[%lf]\r\n",ass_mst.rate,ass_mst.over_rate);
    /* 营业机构向考核机构的转化 */
    ret = Ass_br_def_Sel(g_pub_tx.reply,&ass_br_def,"sub_br_no='%s' and \
          beg_date <= %ld and end_date > %ld",ass_br_def.sub_br_no,
          g_pub_tx.tx_date,g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"营业机构转化考核机构错误!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O100");
        goto ErrExit;
    }

    /* 检查考核主文件中是否有相同的合同号 */
    ret = Ass_mst_Sel(g_pub_tx.reply,&ass_mst,"pact_no=%ld",ass_mst.pact_no);
    if (ret == 0)
    {
        sprintf(acErrMsg,"考核主文件中已经有该合同号!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O101");
        goto ErrExit;
    }
    else if (ret != 100)
    {
        sprintf(acErrMsg,"查询考核主文件出错!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O102");
        goto ErrExit;
    }

    /* 登记考核主文件 */
    strcpy(ass_mst.br_no,ass_br_def.br_no);
    strcpy(ass_mst.sub_br_no,ass_br_def.sub_br_no);
    strcpy(ass_mst.sign,"2");   
    ass_mst.bal = g_pub_tx.tx_amt1;
    ass_mst.ys_bal = 0.00;
    ass_mst.ic_date = g_pub_tx.tx_date;
    ass_mst.lst_date = g_pub_tx.tx_date;
    ass_mst.hst_cnt = 1;
    strcpy(ass_mst.brf,"上存记账");
    strcpy(ass_mst.sts,"1");

    ret = Ass_mst_Ins(ass_mst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记考核主文件错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O102");
        goto ErrExit;
    }
    sprintf(g_pub_tx.beg_note_no,"%d",ass_mst.pact_no);/* 取消时候用 */
    /* 登记考核明细 */
    strcpy(ass_mst_hst.br_no,ass_mst.br_no);
    ass_mst_hst.tx_date = g_pub_tx.tx_date;
    ass_mst_hst.trace_no = g_pub_tx.trace_no;
    strcpy(ass_mst_hst.sub_br_no,ass_mst.sub_br_no);
    ass_mst_hst.pact_no = ass_mst.pact_no;
    ass_mst_hst.tx_amt = g_pub_tx.tx_amt1;
    ass_mst_hst.bal = g_pub_tx.tx_amt1;
    ass_mst_hst.tx_time = g_pub_tx.tx_time;
    strcpy(ass_mst_hst.brf,"上存记帐");
    ass_mst_hst.hst_cnt = 1;
    strcpy(ass_mst_hst.tel,g_pub_tx.tel);
    strcpy(ass_mst_hst.chk,g_pub_tx.chk);
    strcpy(ass_mst_hst.auth,g_pub_tx.auth);
    
TRACE    
    ret = Ass_mst_hst_Ins(ass_mst_hst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记考核明晰错误!!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O103");
        goto ErrExit;
    }   
  
TRACE    
    /* 修改考核总帐  modified by liuxuan 2006-8-24
    strcpy(ass_gl.br_no,ass_mst_hst.br_no);
    ret = Ass_gl_Dec_Upd(g_pub_tx.reply,"br_no='%s'",ass_gl.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Ass_gl_Fet_Upd(&ass_gl,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        Ass_gl_Clo_Upd();
        goto ErrExit;
    }
    ass_gl.out_bal = ass_gl.out_bal + g_pub_tx.tx_amt1;
    ret = Ass_gl_Upd_Upd(ass_gl,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        Ass_gl_Clo_Upd();
        goto ErrExit;
    }
    Ass_gl_Clo_Upd();
	这个表用不到了 */

    /* 登记交易流水 */
    strcpy(g_pub_tx.brf,"上存记账");
    if (pub_ins_trace_log())/* 登记交易流水 pub_ins_trace_log1*/
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        strcpy( g_pub_tx.reply, "P126" );
        goto ErrExit;
    }

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
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

