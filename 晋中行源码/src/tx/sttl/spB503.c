/*************************************************
* 文 件 名:  spB503.c
* 功能描述： 同业拆出信息维护
*
* 作    者:  rob
* 完成日期： 2003年1月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"
#include "com_parm_c.h"

int spB503()
{
    struct mo_ibl_c sMo_ibl_c, sMo_ibl_c1;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
	struct com_parm_c sCom_parm;
    char flag[2];
    char ac_no[20];
    int ret;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&sMo_ibl_c1,0x0,sizeof(struct mo_ibl_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*合同号*/
    /* get_zd_data("0230",sMo_ibl_c.prdt_no);      *产品编号*/
    get_zd_data("0250",sMo_ibl_c.op_name);         /*拆借户名*/
    get_zd_data("0300",sMo_ibl_c.op_ac_no);     /*对方账号*/
    get_zd_double("0400",&sMo_ibl_c.amt);       /*拆借金额*/
    get_zd_double("0840",&sMo_ibl_c.rate);      /*拆借利率*/
    get_zd_double("0850",&sMo_ibl_c.over_rate); /*逾期利率*/
    get_zd_long("0440",&sMo_ibl_c.matr_date);   /*到期日期*/
	get_zd_long("0360", &sMo_ibl_c.term);       /* 拆借期限 */
    get_zd_data("0670",flag);
    get_zd_data("0310",sMo_ibl_c.kh_ac_no);  /*拆入方帐号(本行客户帐号)*/
	  get_zd_data("0260",sMo_ibl_c.kh_name);  /*拆入方户名(本行客户名)*/
	  get_zd_data("0520",sMo_ibl_c.op_br_no); /*外行行号*/ 

    strcpy(sMo_ibl_c.br_no, g_pub_tx.tx_br_no);
    sMo_ibl_c.no_pay_bal = sMo_ibl_c.amt; 
    sMo_ibl_c.wrk_date = g_pub_tx.tx_date;
    sMo_ibl_c.trance_no = g_pub_tx.trace_no;
    strcpy(sMo_ibl_c.sts,"1");
    strcpy(sMo_ibl_c.tel,g_pub_tx.tel);
    strcpy(sMo_ibl_c.chk,g_pub_tx.chk);
    strcpy(sMo_ibl_c.auth,g_pub_tx.auth);
    strcpy(sMo_ibl_c.sign,"2");

	/*** delete by rob at 20030706 *****
	if (strlen(ac_no) > 5)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询对方账号id错误!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B006");
        	goto ErrExit;
    	}

    	sMo_ibl_c.op_ac_id = sMdm_ac_rel_c.ac_id;
	}
	else
	{
		strcpy(sMo_ibl_c.acc_no, ac_no);
	}
	**********/
	/***     20060921 martin 从介质与账户关系表查出客户名称     
	if (strlen(ac_no) > 9)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询拆入方账号id错误!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B006");
        	goto ErrExit;
    	}
   	sMo_ibl_c.kh_ac_id = sMdm_ac_rel_c.ac_id;   
	}***/

	/* 查询拆出产品号码 */
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TYCC' \
			and parm_seqn = 1"); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "select com_parm error ! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
			
	pub_base_strpack(sCom_parm.val);
	strcpy(sMo_ibl_c.prdt_no, sCom_parm.val);
    
    switch( atoi(flag) )
    {
        case 0 :
            /* 录入 */
            sprintf(acErrMsg,"录入!!");
            WRITEMSG
            
            /* 检查该合同号是否已经存在 */
            ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c1,"pact_no='%s'",
                            sMo_ibl_c.pact_no);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"select mo_ibl error [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"该合同号已经存在!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B071");
                goto ErrExit;
            }

            ret = Mo_ibl_Ins(sMo_ibl_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向拆借登记簿插入数据错误",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "B007");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"同业拆出信息录入");
            sprintf(acErrMsg,"向拆借登记簿插入pass");
            WRITEMSG

            break;
        case 1 :
            /* 修改 */
            ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sign ='2'",
                  sMo_ibl_c.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            ret = Mo_ibl_Fet_Upd(&sMo_ibl_c1,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"没有该项合同或合同号输错!!ret=[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B003");
                goto ErrExit;
            }

            if (sMo_ibl_c1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许修改!!ret=[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
            
	    strcpy(sMo_ibl_c.rowid, sMo_ibl_c1.rowid);
            ret = Mo_ibl_Upd_Upd(sMo_ibl_c, g_pub_tx.reply);
            if ( ret != 0)
            {
                sprintf(acErrMsg,"修改拆借登记簿错误!! ret =[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B008");
                goto ErrExit;
            } 

            strcpy(g_pub_tx.brf,"同业拆借信息修改");
            Mo_ibl_Clo_Upd();
            sprintf(acErrMsg,"修改拆借登记簿pass!! ");
            WRITEMSG
            break;
        case 2 :
            /* 删除 */
            ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c1,"pact_no = '%s' and \
                  sign = '2'", sMo_ibl_c.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"没有该项合同或合同号输错!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B003");
                goto ErrExit;
            }

            if (sMo_ibl_c1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许删除!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            
            ret = sql_execute("delete from mo_ibl where pact_no = '%s' and \
                  sign ='2'", sMo_ibl_c.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"删除拆借登记簿中记录出错!! ret =[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B011");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"同业拆借信息删除");
            sprintf(acErrMsg,"删除拆借登记簿中记录pass!! ret =[%d]",ret);
            WRITEMSG

            break;
        default :
            sprintf(acErrMsg,"操作类型出错!! type=[%c] 0录入1修改2删除",flag);
            goto ErrExit;
            break;
    }
    
    strcpy(g_pub_tx.prdt_code,sMo_ibl_c.prdt_no);
    strcpy(g_pub_tx.ac_no,ac_no);
    g_pub_tx.tx_amt1 = sMo_ibl_c.amt;

	/******* delete by rob at 20030706
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
	*********/
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

