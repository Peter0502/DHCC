/*************************************************
* 文 件 名:  spB911.c
* 功能描述： 完成贴现信息录入功能。
*
* 作    者:  jane
* 完成日期： 2003年4月5日
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

int spB911()
{
	struct	mo_discnt_c	sMoDiscnt,sMoDiscnt1;	/* 贴现登记薄 */
	struct	mdm_ac_rel_c	sMdmAcRel;			/* 介质与账户关系表 */
	struct	dd_mst_c	sDdMst;			/* 活期存款主文件 */
  struct	dd_mst_c	s_DdMst;			/* 活期存款主文件 */
	char	cOperCode[2];				/* 操作码 */
	char	cName[61];				/* 25域名字 */
	long	lPnDays=0;					/* 贴现天数 */
	double	rate_val;					/* 利率 */
	int	ret;
	char    pcTmp[64];
	char	payName[51];
	char  cPn_knd[2];
    	char            cTmpAcno[21];
    	struct in_mst_c sInMst; /***add by ligl 2010/5/5 23:14:10**/
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( payName, 0x00, sizeof( payName ) );
	memset( cName, 0x00, sizeof( cName ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cTmpAcno , 0 , sizeof(cTmpAcno));
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );/***add by ligl 2010/5/5 23:14:10**/
  memset( &s_DdMst, 0x00, sizeof( struct dd_mst_c ) );/***add by ligl 2010/5/5 23:14:10**/

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );				/* 操作码 */
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* 贴现协议编号 */
	    get_zd_data("0630" , cTmpAcno);
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	{
		/****TODO****/
		double a;
		memset(pcTmp,'\0',sizeof(pcTmp));
		get_zd_data("1002",pcTmp);
		vtcp_log("%s,%d,fd[1002]=%s\n",__FILE__,__LINE__,pcTmp);
		get_zd_double( "1002", &a );	/* 汇票金额 */
		vtcp_log("%s,%d,fd_db[1002]=%lf\n",__FILE__,__LINE__,a);
	}
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* 汇票金额 */
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%lf] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
	get_zd_double( "0840", &sMoDiscnt.pn_rate );	/* 贴现率 */
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
	sMoDiscnt.pn_date = g_pub_tx.tx_date;			/* 贴现日 */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* 贴现信息录入时交易日期 */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* 录入流水号 */
	get_zd_long( "0450", &sMoDiscnt.not_matr_date );/* 汇票到期日 */
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%lf] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
vtcp_log( "[%s][%d] sMoDiscnt.not_matr_date[%ld] ", __FILE__,__LINE__,sMoDiscnt.not_matr_date );
	get_zd_data( "0310", sMoDiscnt.rem_ac_no );		/* 出票人帐号 */
	get_zd_data( "0260", sMoDiscnt.remitter );		/* 出票人全称 */
	get_zd_data( "0810", sMoDiscnt.payer_brno_name );/* 付款行全称 */
	get_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* 收款人帐号 */
	get_zd_data( "0270", sMoDiscnt.payee );			/* 收款人全称 */
/* sMoDiscnt.payee_brno_name是30位长，而82域是50位
	get_zd_data( "0820", sMoDiscnt.payee_brno_name );收款人开户行全称 */

	get_zd_data( "0820", payName );/* 收款人开户行全称 */
	memcpy(sMoDiscnt.payee_brno_name, payName, sizeof(sMoDiscnt.payee_brno_name)-1);
	get_zd_data( "0900", sMoDiscnt.note_type );		/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt.pnote_no );		/* 票据号 */
	/***add by ligl 2010/5/12 17:07:05 dp***/
	get_zd_data( "0980", cPn_knd );		/* 票据号 */
	if(cPn_knd[0]=='E')
	get_zd_data( "0670", sMoDiscnt.ac_type );		/* 收款账户类型 */
	get_zd_data( "0830", sMoDiscnt.pn_ac_no );		/* 贴现申请人账号 */
	get_zd_data( "0250", cName );		/* 贴现申请人户名 */
	get_zd_data( "0680", sMoDiscnt.local_ind );		/* 异地同城标志 */
	get_zd_long( "0480", &sMoDiscnt.way_days );		/* 在途天数 */
	memcpy(sMoDiscnt.pn_name,cName,sizeof(sMoDiscnt.pn_name)-1);
vtcp_log( "[%s][%d]   sMoDiscnt.par_amt[%f] ",__FILE__,__LINE__, sMoDiscnt.par_amt );
vtcp_log( "[%s][%d]   sMoDiscnt.pn_rate[%lf] ",__FILE__,__LINE__, sMoDiscnt.pn_rate );
vtcp_log( "[%s][%d] sMoDiscnt.not_matr_date[%ld] ",__FILE__,__LINE__, sMoDiscnt.not_matr_date );
vtcp_log( "[%s][%d] lock sMoDiscnt.ac_type[%s] ",__FILE__,__LINE__, sMoDiscnt.ac_type );
vtcp_log( "[%s][%d] lock sMoDiscnt.pn_name[%s] ",__FILE__,__LINE__, sMoDiscnt.pn_name );

	if( cOperCode[0] == '0' )			/* 录入 */
	{
		/* 检查贴现登记薄中是否已经存在此贴现协议编号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'",\
				 sMoDiscnt.pact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "贴现登记薄中已经存在此贴现协议编号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B111" );
			goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
       	}
vtcp_log( "33333333333  sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "33333333333sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );

		/* 检查贴现登记薄中是否已经存在此票据类型和票据号 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' and pnote_no='%s'",\
				 sMoDiscnt.note_type, sMoDiscnt.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "贴现登记薄中已经存在此票据类型和票据号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B174" );
			goto ErrExit;
       	}else if( ret != 100 )
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
		if( sMoDiscnt1.pn_type[0] != '1' )
		{
			sprintf( acErrMsg, "非贴现类型!pn_type[%s]", sMoDiscnt1.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}
/* 检查此贴现协议状态 增加电票2010/5/5 21:26:10 */
        if (sMoDiscnt1.sts[0] != 'X') {
            sprintf(acErrMsg, "该笔贴现已做过其他处理,非录入状态!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B131");
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
    /***add by ligl 2010/5/5 22:52:45 出票人账号 还有票号**/
     memcpy(sMoDiscnt.rem_ac_no,sMoDiscnt1.rem_ac_no,sizeof(sMoDiscnt.rem_ac_no)-1);
     memcpy(sMoDiscnt.pnote_no,sMoDiscnt1.pnote_no,sizeof(sMoDiscnt.pnote_no)-1);
     memcpy(sMoDiscnt.po_knd,sMoDiscnt1.po_knd,sizeof(sMoDiscnt.po_knd)-1);
     /***end by ligl 2010/5/5 23:14:48**/
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
vtcp_log( "4444444444   sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "4444444444 sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );

	/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
	ret = pub_base_GetParm_Str( "TXCP", 1, sMoDiscnt.bkrl_code );
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
	}****/

	/* 如果贴现申请人账号为本行的,检查贴现申请人账号有效性：必须是活期、
		账户状态必须正常,户名与输入是否相符 */
	/* 根据客户帐号查询客户帐号ID和帐户序号 */
#if 0 /***modify by ligl 2010/5/5 23:14:10**/
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				sMoDiscnt.pn_ac_no );
	if( ret == 0 )
	{
		/* 检查输入收款账户类型与贴现申请人账号性质是否相符 */
		if( sMoDiscnt.ac_type[0] != '1' )
		{
			vtcp_log( "[%s][%d] lock sMoDiscnt.ac_type[%s] ",__FILE__,__LINE__, sMoDiscnt.ac_type );
			sprintf( acErrMsg, "收款账户类型应为本行!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B140" );
			goto ErrExit;
		}

		/* 根据客户帐号ID、帐户序号、户名、帐户状态为1正常
			查询活期存款主文件 */
		ret =Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld \
				and ac_seqn=%d and ac_sts='%s'",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1" );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "此客户帐号非活期或帐户状态不正常![%s]", sMoDiscnt.pn_ac_no );
       		WRITEMSG
       		strcpy( g_pub_tx.reply, "B124" );
			goto ErrExit;
   		}else if( ret )
   		{
   			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
           	WRITEMSG
           	goto ErrExit;
   		}
   	}
	else if( ret != 100 )
   	{
   		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
   	else
	{
		/* 检查输入收款账户类型与贴现申请人账号性质是否相符 */
		if( sMoDiscnt.ac_type[0] == '1' )
		{
			sprintf( acErrMsg, "收款账户类型应为本地或异地!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B141" );
			goto ErrExit;
		}
	}
vtcp_log( "5555555555   sMoDiscnt.par_amt[%f] ", sMoDiscnt.par_amt );
vtcp_log( "5555555555 sMoDiscnt.not_matr_date[%ld] ", sMoDiscnt.not_matr_date );
#endif/***end modify by ligl 2010/5/5 23:15:16**/

   /****是纸票的情况 modif by xyy 2010-4-29 17:13:23 NEWDP***/
   /***add by ligl 2010/5/5 23:14:10**/
    if(sMoDiscnt.po_knd[0]=='P')
    {
        /* 如果贴现申请人账号为本行的,检查贴现申请人账号有效性:必须是活期且账户状态必须正常,户名与输入是否相符*/
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoDiscnt.pn_ac_no);
        if (ret) {
            sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        else {
            /* 检查输入收款账户类型与贴现申请人账号性质是否相符 */
            vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
            if (sMoDiscnt.ac_type[0] != '1') {
                sprintf(acErrMsg, "收款账户类型应为本行!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "B140");
                goto ErrExit;
            }
            /* 根据客户帐号ID、帐户序号、户名、帐户状态为1正常 查询活期存款主文件 */
            ret = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
                    and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
            if (ret == 100) {
                sprintf(acErrMsg, "此客户帐号非活期或帐户状态不正常![%s]", sMoDiscnt.pn_ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply, "B124");
                goto ErrExit;
            }
            else if (ret) {
                sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            if(memcmp(sMoDiscnt.pn_name , sDdMst.name ,sizeof(sMoDiscnt.pn_name)-1)!=0)
            {
                sprintf(acErrMsg, "户名错误![%s][%s]", sMoDiscnt.pn_name,sDdMst.name);
                WRITEMSG
                strcpy(g_pub_tx.reply, "NXD5");
                goto ErrExit;
            }
        }
    }
    else if (sMoDiscnt.po_knd[0]=='E')/***电票业务也分两种情况***/
    {
	Mo_discnt_Debug(&sMoDiscnt);
    		get_zd_data( "0750", sMoDiscnt.pnote_no );		/* 票据号 */

        if(sMoDiscnt.ac_type[0]=='1')/***电票的话要检查入账账户,我行账户时***/
        {
             memset(&sMdmAcRel , 0 , sizeof(struct mdm_ac_rel_c));
             ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cTmpAcno);
             if (ret) {
                 sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             else {
                 /* 检查输入收款账户类型与贴现申请人账号性质是否相符 */
                 vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
                 
                 /* 根据客户帐号ID、帐户序号、户名、帐户状态为1正常 查询活期存款主文件 */
                 memset(&sDdMst , 0 , sizeof(struct dd_mst_c));
                 ret = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
                         and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                 if (ret) {
                     sprintf(acErrMsg, "此帐号非活期或帐户状态不正常![%s]", cTmpAcno);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "B124");
                     goto ErrExit;
                 }
             }
        }
        else
        {
             memset(&sMdmAcRel , 0 , sizeof(struct mdm_ac_rel_c));
             ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cTmpAcno);
             if (ret) {
                 sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             else {
                 /* 检查输入收款账户类型与贴现申请人账号性质是否相符 */
                 vtcp_log("[%s][%d] MoDiscnt.ac_type=[%s]\n",__FILE__,__LINE__,sMoDiscnt.ac_type);
                 
                 /* 根据客户帐号ID、帐户序号、户名、帐户状态为1正常 查询活期存款主文件 */
                 memset(&sInMst , 0 , sizeof(struct in_mst_c));
                 /*
                 ret = In_mst_Sel(g_pub_tx.reply, &sInMst, "ac_id=%ld \
                         and ac_seqn=%d and sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                         */
                 ret = Dd_mst_Sel(g_pub_tx.reply, &s_DdMst, "ac_id=%ld \
                         and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
                 if (ret) {
                     sprintf(acErrMsg, "此帐号非内部或帐户状态不正常![%s]", cTmpAcno);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "B124");
                     goto ErrExit;
                 }
            }
        }
    }
    /***end by ligl 2010/5/5 23:14:10**/
	/* 计算贴现利息：其中，贴现天数＝ 票据到期日 -贴现日期+ 在途天数,
		贴现利息 ＝ 汇票金额 * 贴现天数 * （月贴现率/30天/1000）） */
	ret = pub_base_CalDays( sMoDiscnt.pn_date, sMoDiscnt.not_matr_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "计算贴现天数错!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt.pn_int = sMoDiscnt.par_amt * (lPnDays + sMoDiscnt.way_days) * (sMoDiscnt.pn_rate/30/1000);
vtcp_log( "666666666   sMoDiscnt.pn_int[%f] ", sMoDiscnt.pn_int );
	if(sMoDiscnt.po_knd[0]=='E')
	get_zd_double("1001",&sMoDiscnt.pn_int);/***add by ligl 2010/5/5 23:14:10 以实际实付利息为准**/
	set_zd_double( "1001", sMoDiscnt.pn_int );		/* 贴现利息 */

	/* 登记贴现登记薄,状态为录入,贴现类型为"贴现" */
if(sMoDiscnt.po_knd[0]=='E')
	strcpy( sMoDiscnt.sts, "2" );				/* 标志：1录入 ***add by ligl 2010/5/5 23:14:10 该为审核**/
	else
	strcpy( sMoDiscnt.sts, "1" );				/* 标志：1录入 ***add by ligl 2010/5/5 23:14:10 该为审核**/
		strcpy( sMoDiscnt.pn_type, "1" );			/* 贴现类型: 1贴现 */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;		/* 贴现账户余额 */
	if( cOperCode[0] == '1'  )				/* 修改 */
	{
		pub_base_strpack( sMoDiscnt1.pact_no );
sprintf( acErrMsg, "__zxxxyu__sMoDiscnt.pact_no=[%s],sMoDiscnt1.pact_no=[%s]",\
		sMoDiscnt.pact_no, sMoDiscnt1.pact_no );
WRITEMSG
		if( strcmp( sMoDiscnt.pact_no, sMoDiscnt1.pact_no ) != 0 )
		{
			sprintf( acErrMsg, "贴现协议编号不允许修改!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B239" );
			goto ErrExit;
		}
	}

	Mo_discnt_Debug(&sMoDiscnt);
	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

vtcp_log( "666666666   sMoDiscnt.pn_int[] ");
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "贴现信息录入" );
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!!");
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
