/*************************************************
* 文 件 名:  spB931.c
* 功能描述： 将本行的贴现票据贴现给他行，完成买断式转贴现信息录入（转出）功能。
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

int spB931()
{
	struct	mo_discnt_c	sMoDiscnt1, sMoDiscnt2;	/* 贴现登记薄 */
	long	lPnDays;							/* 贴现天数 */
	int		ret,num;
	char	cOperCode[2];				/* 操作码 */
	char	cPn_knd[2];				/* 电票标志 */
    char    cKinbr[BRNO_LEN+1];
    char    cZsq[2];         /* 追索权 0：无追索权 1：有追索权*/

	/* 数据初始化 */
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cKinbr , 0 , sizeof(cKinbr));
  memset(cZsq     , 0x00, sizeof(cZsq));
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );				/* 操作码 */
	strcpy( sMoDiscnt1.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	get_zd_double( "0840", &sMoDiscnt1.tpn_rate );	/* 转贴现率 */
	sMoDiscnt1.tpn_date = g_pub_tx.tx_date;			/* 转贴现日 */
	sMoDiscnt1.wrk_date = g_pub_tx.tx_date;			/* 转贴现信息录入时交易日期 */
	sMoDiscnt1.trace_no = g_pub_tx.trace_no;		/* 录入流水号 */
	get_zd_data( "0900", sMoDiscnt2.note_type );	/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt2.pnote_no );		/* 票据号 */
	get_zd_data( "0680", sMoDiscnt1.local_ind );	/* 异地同城标志 */
	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* 对方行名称 */
	get_zd_data( "0980", cPn_knd);	/* 电票标志 */
   get_zd_data("0380", sMoDiscnt1.tpact_no );
 	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* 对方行名称 */
     get_zd_data("0720", cZsq); 
 get_zd_data("0910" , cKinbr );
	if(cPn_knd[0]=='E')
	get_zd_data("1032",sMoDiscnt2.pnote_no);

	if( cOperCode[0] == '0' )			/* 录入 */
	{
		/* 根据票据类型和票据号查询贴现登记薄 */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' and pnote_no='%s' \
						and pn_type='4'", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "此票据已做转贴现转出!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B248" );
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
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' and pnote_no='%s' \
						and pn_type='4'", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "贴现登记薄中不存在此转贴现协议编号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B113" );
			goto ErrExit;
       	}
		else if( ret )
       	{
       		sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
vtcp_log( "pn_type[%s]", sMoDiscnt2.pn_type );
		if( sMoDiscnt2.pn_type[0] != '4' )
		{
			sprintf( acErrMsg, "非贴现类型!pn_type[%s]", sMoDiscnt2.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}

		/* 检查此贴现协议状态 */
		if( sMoDiscnt2.sts[0] == '2' )
		{
			sprintf( acErrMsg, "此贴现协议已审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B152" );
			goto ErrExit;
		}else if( sMoDiscnt2.sts[0] == '3' )
		{
			sprintf( acErrMsg, "此笔贴现已记帐!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B148" );
			goto ErrExit;
		}else if( sMoDiscnt2.sts[0] == '*' )
		{
			sprintf( acErrMsg, "此笔贴现已还款!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}

		/* 删除此贴现协议编号对应的纪录 */
		ret=sql_execute("DELETE from mo_discnt WHERE note_type='%s' and pnote_no='%s' and pn_type='4' ",sMoDiscnt2.note_type, sMoDiscnt2.pnote_no);
		if( ret ) 
		{
      		sprintf( acErrMsg, "执行sql_execute错![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}
	}

	/* 根据票据类型和票据号找出记录 */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s' and pn_type!='4' and br_no='%s' ",\
						 	sMoDiscnt2.note_type, sMoDiscnt2.pnote_no,g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt2, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "贴现登记薄中不存在此票据信息![%s],[%s]" ,sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( cOperCode[0] == '0' )			/* 录入 */
	{
sprintf( acErrMsg, "pn_type=[%s]", sMoDiscnt2.pn_type );
WRITEMSG
		/* 检查票据状态是否为正常，且贴现类型为贴现或者买断转入 */
		if( sMoDiscnt2.pn_type[0] != '1' && sMoDiscnt2.pn_type[0] != '2' )
		{
			sprintf( acErrMsg, "此票据非贴现或买断转入所得，不能做转贴现转出!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B154" );
			goto ErrExit;
		}

		if( sMoDiscnt2.pn_type[0] == '1' )	/* 贴现 */
		{
			if( sMoDiscnt2.sts[0] == '1' )
			{
				sprintf( acErrMsg, "此笔贴现未审核!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B147" );
				goto ErrExit;
			}
			else if( sMoDiscnt2.sts[0] == '2' )
			{
				sprintf( acErrMsg, "此笔贴现未记账!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B163" );
				goto ErrExit;
			}
		}
		else					/* 买断转入 */
		{
			if( sMoDiscnt2.sts[0] == '1' )
			{
				sprintf( acErrMsg, "此笔贴现未记账!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B163" );
				goto ErrExit;
			}
		}
		if( sMoDiscnt2.sts[0] == '*' )
		{
			sprintf( acErrMsg, "此笔贴现已做还款!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}
	}

	if( cOperCode[0] == '0' || cOperCode[0] == '1' )			/* 录入,修改 */
	{
		/* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
		ret = pub_base_GetParm_Str( "TXCP", 5, sMoDiscnt1.bkrl_code );
		if( ret )
		{
			sprintf( acErrMsg, "取贴现产品编号错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B126" );
			goto ErrExit;
		}

		/* 计算贴现利息：其中，贴现天数＝ 票据到期日 -转贴现日期+ 在途天数,
			贴现利息 ＝ 汇票金额 * 贴现天数 * （年转贴现率/360天/100）） */
		ret = pub_base_CalDays( sMoDiscnt1.tpn_date, sMoDiscnt2.not_matr_date, "0", &lPnDays );
		if( ret )
		{
			sprintf( acErrMsg, "计算贴现天数错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B127" );
			goto ErrExit;
		}

	sMoDiscnt1.tpn_int = sMoDiscnt2.par_amt * (lPnDays + sMoDiscnt1.way_days) * (sMoDiscnt1.tpn_rate/360/100);
		get_zd_double( "1001", &sMoDiscnt1.tpn_int );		/* 转贴现利息 */
		set_zd_double( "1001", sMoDiscnt1.tpn_int );		/* 转贴现利息 */

		/* 登记贴现登记薄，状态为录入 */
		strcpy( sMoDiscnt1.note_type, sMoDiscnt2.note_type );	/* 票据类型 */
		strcpy( sMoDiscnt1.pnote_no, sMoDiscnt2.pnote_no );		/* 票据号 */
		strcpy( sMoDiscnt1.sts, "1" );							/* 标志：1录入 */
		 sMoDiscnt1.po_knd[0] = sMoDiscnt2.po_knd[0] ;
        
        if(sMoDiscnt2.po_knd[0]=='E')
        {
            get_zd_data("0990",sMoDiscnt1.pn_type);              /* 贴现类型: 4买断转出 5:回购式转出 */
        }
        else if (sMoDiscnt2.po_knd[0]=='P')
        {
            strcpy(sMoDiscnt1.pn_type, "4");                     /* 贴现类型: 4买断转出 */
            if(memcmp(g_pub_tx.tx_br_no , cKinbr , BRNO_LEN)!=0)
            {
                sprintf(acErrMsg, "非本机构票据不能做此业务![%s][%s]", cKinbr,g_pub_tx.tx_br_no);
                WRITEMSG
                strcpy(g_pub_tx.reply , "Z10D");
                goto ErrExit;
            }
        }
        else
        {
            sprintf(acErrMsg, "票据类型错误![%s]", sMoDiscnt2.po_knd);
            WRITEMSG
            goto ErrExit;
        }
        	/*	strcpy( sMoDiscnt1.pn_type, "4" );						 贴现类型: 4买断转出 */
		sMoDiscnt1.tranx_bal = sMoDiscnt2.par_amt;				/* 金额 */

		ret = Mo_discnt_Ins( sMoDiscnt1, g_pub_tx.reply );
		if ( ret )
  		{
      		sprintf( acErrMsg, "执行Mo_discnt_Ins错误!ret=[%d]", ret );
       		WRITEMSG
       		goto ErrExit;
   		}
/* 修改贴现登记簿原贴现记录 */
     if(sMoDiscnt2.po_knd[0] =='P')
        {
            if(cZsq[0] == '0')  /**无追索权**/
            {
                strcpy(sMoDiscnt2.sts, "*");    /* 标志：* 销户 */
            }
            else if(cZsq[0] == '1')/**有追索权**/
            {
                strcpy(sMoDiscnt2.sts, "4");    /* 标志：4 有追索转出 */
            }
            else
            {
                sprintf(acErrMsg, "传入的追索权标志错误[%s]", cZsq);
                WRITEMSG
                goto ErrExit;
            }
        }
        else
        {
            vtcp_log("[%s][%d] NEW pn_type=[%s]",__FILE__,__LINE__,sMoDiscnt1.pn_type);
            if(sMoDiscnt1.pn_type[0] == '4')  /**买断式**/
            {
                strcpy(sMoDiscnt2.sts, "*");    /* 标志：* 销户 */
            }
            else if(sMoDiscnt1.pn_type[0] == '5')/**回购式**/
            {
                strcpy(sMoDiscnt2.sts, "4");    /* 标志：4 有追索转出 */
            }
            else
            {
                sprintf(acErrMsg, "传入的贴现类型错误[%s]", sMoDiscnt1.pn_type);
                WRITEMSG
                goto ErrExit;
            }
        }
		/* 修改贴现登记簿原贴现记录 *
		strcpy( sMoDiscnt2.sts, "*" );		* 标志：* 销户 */
   	}
	else			/** 删除　**/
	{
		strcpy( sMoDiscnt2.sts, "3" );		/* 标志：3 正常 */
	}

	ret = Mo_discnt_Upd_Upd( sMoDiscnt2, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改贴现登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/* 记流水日志 */
	/**----- xxx ---- @20030819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt2.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.note_type,sMoDiscnt2.note_type);
	memcpy(g_pub_tx.beg_note_no,sMoDiscnt2.pnote_no,16);
	strcpy(g_pub_tx.brf,"买断式转贴现转出录入");
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
