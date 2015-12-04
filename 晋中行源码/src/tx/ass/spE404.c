/*************************************************
* 文 件 名:  spE404.c
* 功能描述： 上存支取  
*            由清算中心做,如果改成支行做的话需要改走帐部分的opn_br_no
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
#include "com_rate_c.h"

int spE404()
{
    struct ass_br_def_c ass_br_def;
    struct ass_mst_c ass_mst;
    struct ass_gl_c ass_gl;
    struct ass_mst_hst_c ass_mst_hst;
    struct com_rate_c com_rate;

    int ret;
    double intst,over_intst;
    double bf_rate =0.0;
    long dayterm;
		int day1=0;
    int day2=0;
    char tmpname[101];
    FILE *fp;
    memset(tmpname,0x0,sizeof(tmpname));
    memset(&ass_br_def,0x0,sizeof(struct ass_br_def_c));
    memset(&ass_mst,0x0,sizeof(struct ass_mst_c));
    memset(&ass_gl,0x0,sizeof(struct ass_gl_c));
    memset(&ass_mst_hst,0x0,sizeof(struct ass_mst_hst_c));
    memset(&com_rate,0x0,sizeof(struct com_rate_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O100");
        goto ErrExit;
    }

    get_zd_data("0910",ass_br_def.sub_br_no); 
    get_zd_long("0470",&ass_mst.pact_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);

    g_pub_tx.ac_id = ass_mst.pact_no;
    strcpy(g_pub_tx.ac_no,ass_br_def.sub_br_no);

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

    /* 检查考核主文件中是否有该合同号 */
    ret = Ass_mst_Dec_Upd(g_pub_tx.reply,"pact_no=%ld",ass_mst.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        goto ErrExit;
    }
    ret = Ass_mst_Fet_Upd(&ass_mst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        Ass_mst_Clo_Upd();
        goto ErrExit;
    }

    /******
    ret = Ass_mst_Sel(g_pub_tx.reply,&ass_mst,"pact_no=%ld",ass_mst.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询考核主文件错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        goto ErrExit;
    }
    ******/

    /* 检查该合同号的状态 */
    if (ass_mst.sts[0] != '1')
    {
        sprintf(acErrMsg,"该笔上存已经支取完!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O109");
        Ass_mst_Clo_Upd();
        goto ErrExit;
    }

    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,ass_mst.bal) == 1)
    {
        sprintf(acErrMsg,"取款金额大于上存金额!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O110");
        Ass_mst_Clo_Upd();
        goto ErrExit;
    }

    /* 计算利息 */
    /* 此处错误
    ret=pub_base_CalIntstDays(ass_mst.ic_date,g_pub_tx.tx_date, "0",&dayterm);  计算实际天数
     */
	/****如果提前支取,那么利率取备付金利率 modify at 20090319 martin add date****/
	ret = Com_rate_Sel(g_pub_tx.reply,&com_rate,"cur_no = '01' and rate_code ='ZJ3' and beg_date <= %ld and end_date > %ld ", g_pub_tx.tx_date,g_pub_tx.tx_date);
	if(ret != 0)
	{
        	sprintf(acErrMsg,"取利率值出错!!!");
	        WRITEMSG
		strcpy(g_pub_tx.reply,"W110");
		goto ErrExit;
	}
	bf_rate = com_rate.rate_val;
	/****end at 20090319****/

	/****modify at 20071214****/
	if(g_pub_tx.tx_date < ass_mst.end_date)
	{
		ret=pub_base_CalIntstDays(ass_mst.ic_date,g_pub_tx.tx_date, "0",&dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"计算天数错误!!");
			WRITEMSG
			Ass_mst_Clo_Upd();
			strcpy(g_pub_tx.reply,"O108");
			return 1;
		}
	}
	else
	{
		ret=pub_base_CalIntstDays(ass_mst.ic_date,PubMin(g_pub_tx.tx_date, ass_mst.end_date), "0",&dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"计算天数错误!!");
			WRITEMSG
			Ass_mst_Clo_Upd();
			strcpy(g_pub_tx.reply,"O108");
			return 1;
		}
	}
	/****end at 20071214****/
	/* 旧数据有积数,还过一次后把积数清0 */
	double tmp_intst_acm=0;
	tmp_intst_acm=ass_mst.intst_acm;
	/****modify at 20071212****/
	if(g_pub_tx.tx_date < ass_mst.end_date)
	{
		intst=(ass_mst.intst_acm + dayterm *g_pub_tx.tx_amt1) * bf_rate / 100 / 30 / 12; /****因取年利率,故再除以12****/
	}
	else
	{
		intst=(ass_mst.intst_acm + dayterm *g_pub_tx.tx_amt1) * ass_mst.rate / 1000 / 30;
	}
	/****end at 20071212****/
   	ass_mst.intst_acm=0;
	day1=dayterm;

    /* 逾期 */
	dayterm=0;
	if(ass_mst.end_date){
			ret=pub_base_CalIntstDays(ass_mst.end_date,g_pub_tx.tx_date,"0",&dayterm); /*实际天数 */
			if (ret != 0)
			{
				sprintf(acErrMsg,"计算天数错误!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O108");
				Ass_mst_Clo_Upd();
				return 1;
			}
	}
    over_intst=dayterm * ass_mst.over_rate * g_pub_tx.tx_amt1 / 1000 / 30;
    day2=dayterm;

    /* 登记考核主文件 */
    ass_mst.ys_bal = ass_mst.bal;
    ass_mst.bal = ass_mst.bal - g_pub_tx.tx_amt1;
    ass_mst.lst_date = g_pub_tx.tx_date;
    ass_mst.hst_cnt ++;
    ass_mst.intst = ass_mst.intst + intst + over_intst;
    if (pub_base_CompDblVal(ass_mst.bal,0.00) == 0)
    {
        ass_mst.cls_date = g_pub_tx.tx_date;
        strcpy(ass_mst.sts,"*");
    }
    /* 修改考核主文件 */
    ret = Ass_mst_Upd_Upd(ass_mst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改考核主文件错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O107");
        Ass_mst_Clo_Upd();
        goto ErrExit;
    }
    Ass_mst_Clo_Upd();

    /* 登记考核明细 */
    strcpy(ass_mst_hst.br_no,ass_mst.br_no);
    ass_mst_hst.tx_date = g_pub_tx.tx_date;
    ass_mst_hst.trace_no = g_pub_tx.trace_no;
    strcpy(ass_mst_hst.sub_br_no,ass_mst.sub_br_no);
    ass_mst_hst.pact_no = ass_mst.pact_no;
    ass_mst_hst.tx_amt = g_pub_tx.tx_amt1;
    ass_mst_hst.bal = ass_mst.bal;
    ass_mst_hst.tx_time = g_pub_tx.tx_time;
    strcpy(ass_mst_hst.brf,"上存支取");
    ass_mst_hst.hst_cnt++;
    ass_mst_hst.intst_acm = tmp_intst_acm;/* 保留这个积数取消时候用 */
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
    strcpy(g_pub_tx.brf,"上存取款");
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
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
    ass_gl.out_bal = ass_gl.out_bal - g_pub_tx.tx_amt1;
    ass_gl.out_intst_bal = ass_gl.out_intst_bal + intst + over_intst;
    ret = Ass_gl_Upd_Upd(ass_gl,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        Ass_gl_Clo_Upd();
        goto ErrExit;
    }
    Ass_gl_Clo_Upd();
	 这个表用不到了目前*/

	 	g_pub_tx.tx_amt1=intst + over_intst; 
	 /*借方记帐 清算利息支出*/
	 strcpy(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no); /**发生行**/
		strcpy(g_pub_tx.ac_no,"52203");	/**科目号**/
		strcpy(g_pub_tx.add_ind,"0");
		strcpy(g_pub_tx.brf,"上存取款");	
		ret=pub_acct_trance();/*调用存取款主控*/
		if(ret){
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret = pubf_acct_proc("A016");
    if (ret != 0)
    {
      	sprintf(acErrMsg,"会计记帐不成功!!");
      	WRITEMSG
      	goto ErrExit;
    }

 	/*赋空值*
    	set_zd_data("1204","" );
	set_zd_data("1205","" );
      	set_zd_double("1208",0.00 );
	*/

	 /*贷方记帐 支行利息收入*/
		strcpy(g_pub_tx.opn_br_no,ass_br_def.sub_br_no);	
		strcpy(g_pub_tx.ac_no,"50203");	/**科目号**/
		strcpy(g_pub_tx.add_ind,"1");/*借贷标志*/
		strcpy(g_pub_tx.brf,"上存取款");
		ret=pub_acct_trance();/*调用存取款主控*/
		if(ret) 
		{
			goto ErrExit;
		}
		set_zd_data("1214","01");
		set_zd_data("1215","2");	
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret = pubf_acct_proc("A017");/*调用登记会计流水 贷方*/
    if(ret!=0)
    {
      	sprintf(acErrMsg,"会计记帐不成功!!");
      	WRITEMSG
      	goto ErrExit;
    }

		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");

		/****modify at 20071212****/
		if(g_pub_tx.tx_date < ass_mst.end_date)
		{
			fprintf(fp,"LXQD%ld|%ld|%.2lf|%.2lf|%.4f|%.4f|%ld|%ld|%s|\n",ass_mst.ic_date,ass_mst.end_date,intst,over_intst,bf_rate/12*10,ass_mst.over_rate,day1,day2,ass_mst.sub_br_no);
		}
		else
		{
		  fprintf(fp,"LXQD%ld|%ld|%.2lf|%.2lf|%.4f|%.4f|%ld|%ld|%s|\n",ass_mst.ic_date,ass_mst.end_date,intst,over_intst,ass_mst.rate,ass_mst.over_rate,day1,day2,ass_mst.sub_br_no);
		}
		/****end at 20071212****/
		fclose(fp);
		set_zd_data(DC_FILE_SND,"1");
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                strcpy(g_pub_tx.reply,"D103");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

