/*************************************************
* 文 件 名:  spB921.c
* 功能描述： 从他行转入票据贴现,完成买断式转贴现信息录入(转入)功能。
*
* 作    者:  jane
* 完成日期： 2003年4月14日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int spB921()
{
	struct	mo_discnt_c	sMoDiscnt;	/* 贴现登记薄 */
	struct	mo_discnt_c	sMoDiscnt1;	/* 贴现登记薄 */
	long	lPnDays=0;				/* 贴现天数 */
	double	rate_val=0.00;			/* 利率 */
	int		ret=0;
	char	cOperCode[2];				/* 操作码 */
	char	payerName[51];
	char	payeeName[51];
    char            cKinbr[BRNO_LEN+1];
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( payerName, 0x00, sizeof( payerName ) );
	memset( payeeName, 0x00, sizeof( payeeName ) );
    memset(cKinbr   ,   0 , sizeof(cKinbr));

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );				/* 操作码 */
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* 转贴现协议编号 */
	if(memcmp(g_pub_tx.tx_code,"4956",4)!=0)
 strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
 else
 {   get_zd_data("0910" , cKinbr);
    strcpy(sMoDiscnt.br_no, cKinbr);    /* 机构编码 */
  }
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* 汇票金额 */
	get_zd_double( "0840", &sMoDiscnt.pn_rate );	/* 转贴现率 */
	sMoDiscnt.pn_date = g_pub_tx.tx_date;			/* 转贴现日 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 转贴现信息录入时交易日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 录入流水号 */
	get_zd_long( "0450", &sMoDiscnt.not_matr_date );/* 汇票到期日 */
	get_zd_data( "0310", sMoDiscnt.rem_ac_no );		/* 出票人帐号 */
	get_zd_data( "0260", sMoDiscnt.remitter );		/* 出票人全称 */

/*长度问题	get_zd_data( "0810", sMoDiscnt.payer_brno_name ); 付款行全称 */
	get_zd_data( "0810", payerName );/* 付款行全称 */
	memcpy(sMoDiscnt.payer_brno_name, payerName, sizeof(sMoDiscnt.payer_brno_name)-1);

	get_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	get_zd_data( "0270", sMoDiscnt.payee );			/* 收款人全称 */

/* 长度问题	get_zd_data( "0820", sMoDiscnt.payee_brno_name ); 收款人开户行全称 */
	get_zd_data( "0820", payeeName );/* 收款人开户行全称 */
	memcpy(sMoDiscnt.payee_brno_name, payeeName, sizeof(sMoDiscnt.payee_brno_name)-1);

	get_zd_data( "0900", sMoDiscnt.note_type );		/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt.pnote_no );		/* 票据号 */
	get_zd_data( "0680", sMoDiscnt.local_ind );		/* 异地同城标志 */
	get_zd_long( "0480", &sMoDiscnt.way_days );		/* 在途天数 */
	get_zd_data( "0250", sMoDiscnt.pn_name );		/* 申请人名称 */

	if( cOperCode[0] == '0' )			/* 录入 */
	{
		/* 检查贴现登记薄中是否已经存在此转贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "贴现登记薄中已经存在此转贴现协议编号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B132" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
 		}

		/* 检查贴现登记薄中是否已经存在此票据类型和票据号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' and pnote_no='%s' and sts<>'*'",\
			 sMoDiscnt.note_type, sMoDiscnt.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "贴现登记薄中已经存在此票据类型和票据号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B174" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
   		}
   	}
	else if( cOperCode[0] == '1' || cOperCode[0] == '2' )			/* 修改,删除 */
	{
		vtcp_log( "pact_no[%s]", sMoDiscnt.pact_no );
		/* 检查贴现登记薄中是否存在此贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "pact_no='%s'",\
							 sMoDiscnt.pact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "此贴现协议编号不存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B113" );
			goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
vtcp_log( "pn_type[%s]", sMoDiscnt1.pn_type );
/**dp add by ligl 2010/5/12 17:24:36***/
		if( sMoDiscnt1.pn_type[0] != '2' && sMoDiscnt1.pn_type[0] != '3')
		{
			sprintf( acErrMsg, "非贴现类型!pn_type[%s]", sMoDiscnt1.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}

		/* 检查此贴现协议状态 */
		if( sMoDiscnt1.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此贴现协议已审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B152" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '3' )
		{
			sprintf( acErrMsg, "此笔贴现已记帐!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B148" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '*' )
		{
			sprintf( acErrMsg, "此笔贴现已还款!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}
		 sMoDiscnt.po_knd[0] = sMoDiscnt1.po_knd[0];
		 if(sMoDiscnt.po_knd[0]=='E') /**从新取值**/
		 get_zd_data( "1032", sMoDiscnt.pnote_no );		/* 票据号 */
		/**modify by ligl 2010/5/5 23:13:35**/
if( cOperCode[0] == '2' )		/*** 删除 ***/
		{
		/* 删除此贴现协议编号对应的纪录 */
		ret = sql_execute( "DELETE from mo_discnt WHERE pact_no='%s'",\ 
					 		sMoDiscnt.pact_no );
		if( ret ) 
		{
      		sprintf( acErrMsg, "执行sql_execute错![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}

		if( cOperCode[0] == '2' )		/*** 删除 ***/
		{
      		sprintf( acErrMsg, "删除成功" );
			WRITEMSG
			goto OkExit;
		}
	}
	/**end by ligl 2010/5/5 23:13:46***/

		/* 删除此贴现协议编号对应的纪录 *
		ret = sql_execute( "DELETE from mo_discnt WHERE pact_no='%s'",\ 
					 		sMoDiscnt.pact_no );
		if( ret ) 
		{
      		sprintf( acErrMsg, "执行sql_execute错![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}

		if( cOperCode[0] == '2' )		*** 删除 ***
		{
      		sprintf( acErrMsg, "删除成功" );
			WRITEMSG
			goto OkExit;
		}
		****/
	}

	/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	ret = pub_base_GetParm_Str( "TXCP", 2, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "取贴现产品编号错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}
	
	/**** 检查所输贴现利率是否符合规定  2003年8月17日资金营运部提出对利率不做控制，所以封掉  by jane *
	* 根据贷款产品编号取贷款参数 *
	ret = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此贴现产品编号不存在!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B240" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "执行Ln_parm_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}
       	
	* 取利率编号对应的利率 *
	ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
				g_pub_tx.tx_date,&rate_val);
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"转换前利率[%lf]",rate_val);
	WRITEMSG
	
	rate_val = rate_val/1.2;* 转换成月利率 * 
	sprintf(acErrMsg,"转换结束后利率[%lf]",rate_val);
	WRITEMSG

	if( g_ln_parm.get_rate_type[0]=='0' ) * 挂牌利率 *
	{
		* 比较这两个值是否相等：前一个是屏幕取值，后一个是利率表中的固定利率值 *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate, rate_val ) != 0 )
		{
			sprintf( acErrMsg, "所输贴现利率与规定利率值不符!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B241" );
			goto ErrExit;
		}	
	}else		* 手输利率检查上浮下浮比率 *
	{
		* 正常利率检查 *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
				rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
		{
vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,sMoDiscnt.bkrl_code );
			sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
				rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
				,g_ln_parm.max_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
sprintf(acErrMsg,"最高利率正确时[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
WRITEMSG

		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
		{
			sprintf(acErrMsg,"最低利率错误时[%lf][%lf][%d] ", rate_val,
	 			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
sprintf(acErrMsg,"最低利率正确时[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
WRITEMSG	
	}
	sprintf(acErrMsg,"利率[%lf]", sMoDiscnt.pn_rate);
	WRITEMSG****/
	
	/* 计算贴现利息：其中，贴现天数＝ 票据到期日 -转贴现日期+ 在途天数,
		贴现利息 ＝ 汇票金额 * 贴现天数 * （月转贴现率/30天/1000）） */
	ret = pub_base_CalDays( sMoDiscnt.pn_date, sMoDiscnt.not_matr_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}
	
	sprintf(acErrMsg,"利率[%lf]天数[%ld]", sMoDiscnt.pn_rate,lPnDays);
	WRITEMSG
	
	sMoDiscnt.pn_int = sMoDiscnt.par_amt * (lPnDays + sMoDiscnt.way_days) * (sMoDiscnt.pn_rate/30/1000);
	
	sprintf(acErrMsg,"利率[%lf]天数[%ld]利息[%lf]", sMoDiscnt.pn_rate,lPnDays,sMoDiscnt.pn_int);
	WRITEMSG
		 if(sMoDiscnt.po_knd[0]=='E') /**从新取值**/
		get_zd_double("1001",&sMoDiscnt.pn_int);	

	set_zd_double( "1001", sMoDiscnt.pn_int );		/* 贴现利息 */
 if(sMoDiscnt.po_knd[0]=='P')
    {
        strcpy(sMoDiscnt.pn_type, "2");    /* 贴现类型: 2买断转入 */
    }
    else if(sMoDiscnt.po_knd[0]=='E')
    {
        get_zd_data("0990",sMoDiscnt.pn_type);
    }
    else
    {
        strcpy(sMoDiscnt.po_knd,"P");
        strcpy(sMoDiscnt.pn_type, "2");
vtcp_log( "[%s][%d]>>>po_knd===[%s]",__FILE__,__LINE__, sMoDiscnt.po_knd );
vtcp_log( "[%s][%d]>>>pn_type===[%s]",__FILE__,__LINE__, sMoDiscnt.pn_type );
        /*sprintf(acErrMsg,"票据类型错误=[%s]",sMoDiscnt.po_knd);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B112" );
        goto ErrExit;*/
    }
	/* 登记贴现登记薄，状态为录入 */
	strcpy( sMoDiscnt.sts, "2" );				/* 标志：1录入 */
/*	strcpy( sMoDiscnt.pn_type, "2" );			 贴现类型: 2买断转入 */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;	/* 金额 */
vtcp_log( "pact_no=[%s]", sMoDiscnt.pact_no );
/***modify by ligl 2010/5/5 23:13:15***/
if(cOperCode[0] == '1')/**修改**/
    {
        memset(&sMoDiscnt1, 0x00, sizeof(struct mo_discnt_c));
        ret = Mo_discnt_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sMoDiscnt.pact_no);
        if(ret)
        {
            sprintf(acErrMsg, "此贴现协议编号不存在!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B113");
            goto ErrExit;
        }
        ret = Mo_discnt_Fet_Upd(&sMoDiscnt1,g_pub_tx.reply);
        if(ret)
        {
            sprintf(acErrMsg, "此贴现协议编号不存在!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B113");
            goto ErrExit;
        }
        strcpy(sMoDiscnt1.br_no, cKinbr);
        sMoDiscnt1.par_amt = sMoDiscnt.par_amt;
        sMoDiscnt1.pn_date = g_pub_tx.tx_date;
        sMoDiscnt1.pn_rate = sMoDiscnt.pn_rate;
        sMoDiscnt1.wrk_date = g_pub_tx.tx_date;
        sMoDiscnt1.trace_no = g_pub_tx.trace_no;
        sMoDiscnt1.not_matr_date = sMoDiscnt.not_matr_date;
        strcpy(sMoDiscnt1.note_type,sMoDiscnt.note_type);
        strcpy(sMoDiscnt1.pact_no,sMoDiscnt.pact_no);
        strcpy(sMoDiscnt1.pnote_no,sMoDiscnt.pnote_no);
        strcpy(sMoDiscnt1.local_ind,sMoDiscnt.local_ind);
        strcpy(sMoDiscnt1.pn_name,sMoDiscnt.pn_name);
        strcpy(sMoDiscnt1.bkrl_code,sMoDiscnt.bkrl_code);
        sMoDiscnt1.way_days =sMoDiscnt.way_days;
        /*sMoDiscnt1.ic_date =sMoDiscnt.ic_date;*/
        sMoDiscnt1.pn_int=sMoDiscnt.pn_int;
        strcpy(sMoDiscnt1.sts, "2");     /* 标志：1录入 */
        strcpy(sMoDiscnt1.pn_type, sMoDiscnt.pn_type); /* 贴现类型: 2买断转入 3:回购式*/
        sMoDiscnt1.tranx_bal = sMoDiscnt1.par_amt;  /* 金额 */

        ret = Mo_discnt_Upd_Upd(sMoDiscnt1,g_pub_tx.reply);
        if(ret)
        {    
            sprintf(acErrMsg, "此贴现协议编号不存在!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B113");
            goto ErrExit;
        }
        Mo_discnt_Clo_Upd();
    }
    else
    {
        /** 录入的时候 **/
        ret = Mo_discnt_Ins(sMoDiscnt, g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
    }
/*****end by ligl 2010/5/5 23:13:04******/
/***
	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
  	{
      	sprintf( acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret );
      	WRITEMSG
       	goto ErrExit;
   	}
****/
	/* 记流水日志 */
	/**--- xxx ---@20030818@ ---- 将合同号付给帐号撤销时用 ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
/*
	strcpy( g_pub_tx.brf, "买断式转贴现转入信息录入" );
*/
	strcpy( g_pub_tx.brf, "买断式转贴现转入录入" );
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
