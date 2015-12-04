/*************************************************************
* 文 件 名: gD034.c
* 功能描述：对公活期户利息入帐程序
*
* 作    者: jack
* 完成日期: 2003年3月11日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "intst_hst_c.h"

gD034()
{
	int ret = 0;
    struct intst_hst_c intst_hst_c;
    struct intst_hst_c o_intst_hst_c;
	double sum_intst=0.00;
	double sum_tax=0.00;
	long first=0;

    memset(&o_intst_hst_c, 0, sizeof(o_intst_hst_c));
    memset(&intst_hst_c, 0, sizeof(intst_hst_c));
    memset(&g_pub_tx, 0, sizeof(g_pub_tx));
	strcpy( g_pub_tx.prt_ind,"0" );

    ret=sql_begin(); /*打开事务*/
	MYSQLERR

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
	if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
		goto ErrExit;

	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.trace_cnt=0;

    ret = Intst_hst_Dec_Sel(g_pub_tx.reply,
		"ac_ind='0' and pt_ind='0'  order by br_no,cur_no,prdt_no");
	MYSQLERR

	sum_intst=0.00;
	sum_tax=0.00;

    while(1)
    {
        ret = Intst_hst_Fet_Sel(&intst_hst_c, g_pub_tx.reply);
        if (ret == 100) break;
		MYSQLERR

		if( !first )
			memcpy( &o_intst_hst_c,&intst_hst_c,sizeof(intst_hst_c) );
		first++;

		if( strcmp(o_intst_hst_c.br_no,intst_hst_c.br_no) ||
			strcmp(o_intst_hst_c.cur_no,intst_hst_c.cur_no) ||
			strcmp(o_intst_hst_c.prdt_no,intst_hst_c.prdt_no) )
		{
			if( intst_acct(o_intst_hst_c,sum_intst,sum_tax) )
				goto ErrExit;

			sum_intst=0.00;
			sum_tax=0.00;
		}

		strcpy(g_pub_tx.tx_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.opn_br_no,intst_hst_c.br_no);
		strcpy(g_pub_tx.cur_no,intst_hst_c.cur_no);
        memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
        g_pub_tx.ac_id = intst_hst_c.ac_id;
        g_pub_tx.ac_seqn = intst_hst_c.ac_seqn;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"1");
        strcpy(g_pub_tx.add_ind,"1");
        strcpy(g_pub_tx.ac_wrk_ind,"0200000J");
		/****************************^不计每户的流水**/
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=1005;
		strcpy(g_pub_tx.brf,"结息");
        g_pub_tx.tx_amt1 = intst_hst_c.fct_intst-intst_hst_c.intst_tax;

		sum_intst+=intst_hst_c.fct_intst;
		sum_tax+=intst_hst_c.intst_tax;

        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"向活期对公帐户存利息错误!!");
            WRITEMSG
            goto ErrExit;
        }
		memcpy( &o_intst_hst_c,&intst_hst_c,sizeof(intst_hst_c) );
    }
	Intst_hst_Clo_Sel();

		if( first )
		{
			if( intst_acct(o_intst_hst_c,sum_intst,sum_tax) )
				goto ErrExit;
		}
	/***修改计息记录***/
        /*** 更新利息登记簿 ***/
        sprintf(acErrMsg,"更新利息登记簿错误!!");
        WRITEMSG
    	ret = Intst_hst_Dec_Upd(g_pub_tx.reply,
			"ac_ind='0' and pt_ind='0'");
		MYSQLERR
		while(1)
		{
        	ret = Intst_hst_Fet_Upd(&intst_hst_c, g_pub_tx.reply);
        	if (ret == 100) break;
			MYSQLERR

        	strcpy(intst_hst_c.pt_ind,"1");
        	ret = Intst_hst_Upd_Upd(intst_hst_c,g_pub_tx.reply);
			MYSQLERR
		}
		Intst_hst_Clo_Upd();
	set_zd_data("101A","");
	set_zd_data("101B","");
	set_zd_double("1013",0.00);
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
int intst_acct( struct intst_hst_c o_intst,double sum_intst,double sum_tax )
{
	int ret=0;

	vtcp_log("汇总记帐[%s][%.2lf-%.2lf]",o_intst.prdt_no,sum_intst,sum_tax);

	strcpy(g_pub_tx.tx_br_no,o_intst.br_no);
	strcpy(g_pub_tx.opn_br_no,o_intst.br_no);
    strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
	g_pub_tx.svc_ind=1005;
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,o_intst.prdt_no );
	strcpy( g_pub_tx.ac_no,"" );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	strcpy( g_pub_tx.note_type,"" );
	strcpy( g_pub_tx.beg_note_no,"" );
	g_pub_tx.tx_amt1=sum_intst-sum_tax;
	strcpy( g_pub_tx.no_show,"0" );

		ret=pub_ins_trace_log();
		if( ret ) goto ErrExit;

	set_zd_data("101A",o_intst.cur_no);
	set_zd_data("101B","2");
	set_zd_double("1013",sum_intst-sum_tax);

        ret = pubf_acct_proc("D099");
		if( ret ) goto ErrExit;
	/*-----------------------------------------------------*/	
		strcpy(g_pub_tx.tx_br_no,o_intst.br_no);
		strcpy(g_pub_tx.opn_br_no,o_intst.br_no);
		vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------",g_pub_tx.ac_no);
		get_zd_data("0153",g_pub_tx.ac_no);
		vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------",g_pub_tx.ac_no);
       	g_pub_tx.ac_id = 0;
       	g_pub_tx.ac_seqn = 0;
        strcpy(g_pub_tx.ac_get_ind,"00");
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.add_ind,"0");		/** 不是减少，是借方 **/
    	strcpy(g_pub_tx.ac_wrk_ind,"0000000J");
        strcpy(g_pub_tx.hst_ind,"1");
        g_pub_tx.svc_ind=9001;
        g_pub_tx.tx_amt1 = sum_intst;
		strcpy(g_pub_tx.brf,"对公结息");

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
		/**-------对公利息税暂时不记---------------------------**/
	if( sum_tax>0.005 || sum_tax<-0.005 )
	{
		strcpy( g_pub_tx.reply,"AT03" );
		sprintf(acErrMsg,"SQLERR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
