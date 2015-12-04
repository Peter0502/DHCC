/*************************************************************
* 文 件 名: gD035.c
* 功能描述：内部账计息
*
* 作    者: jack
* 完成日期: 2003年3月12日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "intst_hst_c.h"

gD035()
{
	int ret = 0;
	int	mouth = 0;
    struct in_mst_c in_mst_c;
    struct in_parm_c in_parm_c;
    struct intst_hst_c intst_hst_c;

    memset(&in_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&in_parm_c,0x0,sizeof(struct in_parm_c));
    memset(&intst_hst_c,0x0,sizeof(struct intst_hst_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
	mouth=g_pub_tx.tx_date%10000/100;
vtcp_log("mouth=[%d]",mouth);

    ret = Eod_In_mst_Dec_Upd(g_pub_tx.reply,"sts='1'");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
    while(1)
    {
        memset(&intst_hst_c,0x0,sizeof(struct intst_hst_c));
        ret = Eod_In_mst_Fet_Upd(&in_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        { break; }
		/**---------- hao ---- @20050701@ ------------**/
		if( in_mst_c.intst_knd[0]=='0' || in_mst_c.intst_knd[0]=='1' 
			|| in_mst_c.intst_knd[0]=='2' )
			continue;
		if( in_mst_c.intst_knd[0]=='4' && mouth!=3 && mouth!=6 
			&& mouth!=9 && mouth!=12 )
			continue;
		if( in_mst_c.intst_knd[0]=='5' && mouth!=12 )
			continue;
		/**-------------------------------------------**/

        ret = In_parm_Sel(g_pub_tx.reply,&in_parm_c,"prdt_no='%s'",
              in_mst_c.prdt_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询内部产品参数表错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        if (in_parm_c.intst_ind[0] == 'N')
        {
            break;
        }
        
        /* 计算积数 */
        if( pub_base_CalAcm(in_mst_c.lst_date,g_pub_tx.tx_date,
            in_parm_c.cal_day_type,in_parm_c.acm_type,
            in_mst_c.bal,0.00,&in_mst_c.intst_acm,
            in_parm_c.intst_knd,in_parm_c.intst_mon,in_parm_c.intst_date) )
        {
            sprintf(acErrMsg,"计算利息积数错误!");
            WRITEMSG
            goto ErrExit;
        }

        memcpy(&g_in_mst,&in_mst_c,sizeof(struct in_mst_c));
        memcpy(&g_in_parm,&in_parm_c,sizeof(struct in_parm_c));

        g_pub_tx.ac_id_type[0] = '9'; /* 内部 */
        intst_hst_c.bic_acm = in_mst_c.intst_acm;

sprintf(acErrMsg,"AAAAAAAA intst_acm = [%lf]  ",in_mst_c.intst_acm);
WRITEMSG

        sprintf(acErrMsg,"利息为 = [%lf] 利息税 = [%lf]",
                g_pub_intst.factval, g_pub_intst.val);
        WRITEMSG

        ret = pub_base_CalIntst(&g_pub_intst);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算利息错误!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A032");
            goto ErrExit;
        }

        sprintf(acErrMsg,"利息为 = [%lf] 利息税 = [%lf]",
                g_pub_intst.factval, g_pub_intst.val);
        WRITEMSG

        in_mst_c.intst_acm = 0.00;
        /* 计算利息积数 */
        if( pub_base_CalAcm(in_mst_c.lst_date,g_pub_tx.tx_date,
            in_parm_c.cal_day_type,in_parm_c.acm_type,
            0.00,in_mst_c.bal,&in_mst_c.intst_acm,
            in_parm_c.intst_knd,in_parm_c.intst_mon,in_parm_c.intst_date) )
        {
            sprintf(acErrMsg,"计算利息积数错误!");
            WRITEMSG
            goto ErrExit;
        }
        sprintf(acErrMsg,"计算利息前OK33 intst_acm=[%lf] ",in_mst_c.intst_acm);
        WRITEMSG

        sprintf(acErrMsg,"利息为 = [%lf] 利息税 = [%lf]",
                g_pub_intst.factval, g_pub_intst.val);
        WRITEMSG

		/*-- hao --- @20050701@ --- 党的生日献礼加班修改版 -----
        * 取下一天日期 *
        ret = pub_base_end_year(g_pub_tx.tx_date);
        if (ret == 0)
        {
            in_mst_c.ic_date = g_pub_tx.tx_date / 10000 + 101;
        }
        else
        {
            ret = pub_base_end_month(g_pub_tx.tx_date);
            if (ret == 0)
            {
                in_mst_c.ic_date = (g_pub_tx.tx_date / 100 + 1) + 1;
            }
            else
            {
                in_mst_c.ic_date = g_pub_tx.tx_date + 1;
            }
        }
		**---------------------------------------------------**/

        intst_hst_c.ic_date = in_mst_c.ic_date;
        intst_hst_c.intst_date=g_pub_tx.tx_date;
  		in_mst_c.ic_date=g_pub_tx.tx_date; 
        in_mst_c.lst_date = g_pub_tx.tx_date ;

        ret = Eod_In_mst_Upd_Upd(in_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"更新内部帐户主文件错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        strcpy(intst_hst_c.br_no,in_mst_c.opn_br_no);
        intst_hst_c.ac_id = in_mst_c.ac_id;
        intst_hst_c.ac_seqn = in_mst_c.ac_seqn;
        strcpy(intst_hst_c.cur_no,in_parm_c.cur_no);
        intst_hst_c.bic_bal = in_mst_c.bal;
        intst_hst_c.fct_intst = g_pub_intst.factval;
        intst_hst_c.aic_bal = in_mst_c.bal + g_pub_intst.factval;
        intst_hst_c.aic_acm = in_mst_c.intst_acm;
        strcpy(intst_hst_c.ac_ind,"2");
        strcpy(intst_hst_c.pt_ind,"0");

        if (g_pub_tx.tx_date > in_mst_c.end_date)
        {
            intst_hst_c.rate = in_mst_c.over_rate;
        }
        else
        {
            intst_hst_c.rate = in_mst_c.rate;
        }

        if (pub_base_CompDblVal(g_pub_intst.dealval,0.00) != 0)
        {
            ret = Intst_hst_Ins(intst_hst_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向结息登记簿中插入记录错误!![%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    Eod_In_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"内部账户计息成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"内部帐户计息失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
