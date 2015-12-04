/*************************************************************
* 文 件 名: gD036.c
* 功能描述：内部帐户利息入帐程序
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

gD036()
{
	int ret = 0;
    struct intst_hst_c intst_hst_c;
    memset(&intst_hst_c, 0x0, sizeof(struct intst_hst_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
	/**----- xxx----- @20030702@ ----**/
	strcpy(g_pub_tx.tx_code,"Z036");
	strcpy(g_pub_tx.sub_tx_code,"Z036");
	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.trace_cnt=0;
	/**-------------------------------**/

    ret = Intst_hst_Dec_Upd(g_pub_tx.reply,"ac_ind='2' and pt_ind='0'");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Intst_hst_Fet_Upd(&intst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if (ret == 100)
        {
            break;
        }

		/**----- xxx ----- @20030702@ ----**/
		strcpy(g_pub_tx.tx_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.opn_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.cur_no,intst_hst_c.cur_no);
		/**-------------------------------**/
        memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
        g_pub_tx.ac_id = intst_hst_c.ac_id;
        g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.add_ind,"1");
        strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
		/**----- xxx ----- @20030702@ ----**
        strcpy(g_pub_tx.ac_id_type,"1");
        g_pub_tx.svc_ind=1001;
        strcpy(g_pub_tx.brf,"利息");
		**-------------------------------**/
        strcpy(g_pub_tx.ac_id_type,"9");
        g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.brf,"内部结息入帐");
        g_pub_tx.tx_amt1 = intst_hst_c.fct_intst;

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"向内部帐户存利息错误!!");
            WRITEMSG
            goto ErrExit;
        }
        /**------ xxx ------ @20030702@ ------ 进行会计记帐 ----**/
        set_zd_data("1214",g_pub_tx.cur_no);
        set_zd_data("1215",g_pub_tx.ct_ind);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A017");
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
		/**----------------------------------------------------**/

        /* 更新利息登记簿 */
        strcpy(intst_hst_c.pt_ind,"1");
        ret = Intst_hst_Upd_Upd(intst_hst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"更新利息登记簿错误!!");
            WRITEMSG
            goto ErrExit;
        }

		/**----- xxx ----- @20030702@ ---- 利息支出科目记账 -----*/
		get_zd_data("0153",g_pub_tx.ac_no);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.add_ind,"0");		/** 不是减少，是借方 **/
        strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=9001;
        g_pub_tx.tx_amt1 = intst_hst_c.fct_intst;
		strcpy(g_pub_tx.brf,"内部帐结息利息支出");

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"利息支出科目记账错误!!");
            WRITEMSG
            goto ErrExit;
        }

        set_zd_data("1204",g_pub_tx.cur_no);
        set_zd_data("1205",g_pub_tx.ct_ind);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        ret = pubf_acct_proc("A016");
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
		/**---------------------------------------------------**/
    }
    Eod_Dd_mst_Clo_Upd();
    set_zd_data("1214","");
    set_zd_data("1215","");
    set_zd_double("1218",0.00);
    set_zd_data("1204","");
    set_zd_data("1205","");
    set_zd_double("1208",0.00);

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"对公活期户利息入账程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"对公活期户利息入账程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
