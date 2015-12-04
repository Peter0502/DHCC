/***************************************************************
* 文 件 名:     pubf_acct_redo.c	
* 功能描述：	(冲正补记公用函数)
*              1、检查账号、账号ID和账号序号的合法性；
*              2、检查产品参数表，检查账号主文件，修改主文件全局变量；
*              3、登记明细帐；
*              4、登记交易流水；
**标志用法：
**需要核对凭证号** 	g_pub_tx.ac_wrk_ind[0]='1'
**					g_pub_tx.hst_ind[0]=='1'日间入明细
**					g_pub_tx.prt_ind[0]!='1'是否打折
*
* 入口参数:		err_date 错帐日期
*				C_B_flag 冲正补记标志1-充2-补
*
* 作    者:		
* 完成日期：    2003年03月16日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "com_sect_acm_c.h"
#include "tx_dc_br_rel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

struct mdm_attr_c v_mdm_attr;
struct com_sect_acm_c com_sect_acm_c;

pub_acct_redo(char C_B_flag[2])
{
	int ret=0;
	int	ass_ind=0;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c mdm_attr_c;
	struct note_parm_c note_parm_c;
    int factday,lastday;

	/**初始化主文件内容**/
	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
	memset(&com_sect_acm_c,0x00,sizeof(struct com_sect_acm_c));

	/*冲正用负数记账*/
	if( C_B_flag[0]=='1' )	g_pub_tx.tx_amt1=-g_pub_tx.tx_amt1;

	/*如果科目的情况*/
	pub_base_strpack(g_pub_tx.ac_no);
	ret=strlen(g_pub_tx.ac_no);
	if(ret==3||ret==5||ret==7)
	{
		ret=chongzheng_cash();
		ERR_DEAL
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
		ret=Chg_acchrt_mst(&ass_ind);
		ERR_DEAL
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
	/****冲补科目帐时，也记入流水 modify by wanglei 20070518****/
	/********* 
		if( ass_ind!=1 )
		{
		}
	*********/ 
		vtcp_log("CT_IND[%s]",g_pub_tx.ct_ind);
			if( pub_ins_trace_log1() )
			{
				sprintf(acErrMsg,"登记交易流水出错!");
				WRITEMSG
				goto ErrExit;
			}
	}
	else
	{
		ret=pub_sub_ac1();
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"帐号不存在!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		ERR_DEAL
			sprintf(acErrMsg,"初始化主文件内容PASS!");
			WRITEMSG

		/***检查处理分户帐***/
		ret=pub_sub_mst1();
		ERR_DEAL

		/***补登折处理***/
vtcp_log("-===================g_pub_tx.ac_id_type=[%s]",g_pub_tx.ac_id_type);
		ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
		ERR_DEAL

		/***登记交易流水***/
		if( g_pub_tx.ac_id_type[0]!='3' && g_pub_tx.hst_ind[0]!='0' )
		{
			/***交易特别处理***/
			ret=pub_sub_spe_do1();
			ERR_DEAL

			if( pub_ins_trace_log1() )
			{
				sprintf(acErrMsg,"登记交易流水出错!");
				WRITEMSG
				goto ErrExit;
			}
		}
		sprintf(acErrMsg,"登记交易流水PASS!");
		WRITEMSG
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
	sprintf(acErrMsg,"冲正补记记账处理成功!");
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"冲正补记处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

int Chg_dd_mst1()
{
	int ret;
	double acm;		/*分段利息积数*/

	/*** 活期主文件检查 ***/
	ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**读取产品参数**/
	ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
						" prdt_no='%s' ",g_dd_mst.prdt_no );
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);

	strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;
	
	/*结算户可以转帐,非结算户不可转帐*/
	/**------- xxx ------ 冲正不考虑**
	if( g_pub_tx.ct_ind[0]=='2' && g_dd_mst.sttl_type[0]=='S' )
	{
		sprintf(acErrMsg,"非结算户不允许转账!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P105");
		goto ErrExit;
	}
	**------------------------------**/

	if( g_dd_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"账号状态非正常状态!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	if( g_dd_mst.hold_sts[0]=='1' )
	{
		sprintf(acErrMsg,"该账号已被完全冻结!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P110");
		goto ErrExit;
	}

	/*冲正/撤销记借贷方向*/
	if( g_pub_tx.add_ind[0]=='0' )			/*记借,相当于取款,帐户余额减少*/
	{
		if( g_dd_parm.draw_ind[0]=='N' )
		{
			sprintf(acErrMsg,"不可续取[%s]!",g_dd_parm.draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A023");
			goto ErrExit;
		}
		if( g_dd_parm.thr_draw_ind[0]=='N' 
			&& strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
		{
			sprintf(acErrMsg,"不可通兑[%s]!",g_dd_parm.thr_draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A011");
			goto ErrExit;
		}

		/*** 调用算利息积数的函数 ***/
		/***if( g_dd_parm.acm_calc_type[0]!='3')	*非倒算*/
		{
			/**增加原来的积数处理 add by ligl 2010/7/16 11:22:04**/
			if ( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,g_dd_mst.bal,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
					goto ErrExit;
			}
			/**end**/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}
	
			/*计算分段利息积数*/
			/**------- why ----???? -------------**
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type,
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_dd_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**----------------------------------**/
		}

		g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
		}

		if( g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"该账号已被只进不出冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
		if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt )
		{
			sprintf(acErrMsg,"该账号已被部分冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
		if(g_dd_mst.od_ind[0] != 'Y')
		{
			if( g_dd_mst.bal<g_dd_mst.ctl_amt+g_dd_mst.hold_amt )
			{
			sprintf(acErrMsg,"该账号余额低于控制金额!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A030");
			goto ErrExit;
			}
		}
	}
	else if( g_pub_tx.add_ind[0]=='1' )		/*记贷,帐户余额增加*/
	{
		if( g_dd_parm.dpst_ind[0]=='N' )
		{
			sprintf(acErrMsg,"不可续存[%s]!",g_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			goto ErrExit;
		}
		if( g_dd_parm.thr_dpst_ind[0]=='N' 
			&& strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
		{
			sprintf(acErrMsg,"不可通存[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			goto ErrExit;
		}

		/**if( g_dd_parm.acm_calc_type[0]!='3' )			*非倒算户*/
		{
			/*** 调用算利息积数的函数 ***/
			/**增加原来的积数处理 add by ligl 2010/7/16 11:22:04**/
			if ( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,g_dd_mst.bal,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
					goto ErrExit;
			}
			/**end**/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			/*计算分段利息积数*
			**------------ why --------????? --------**
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, 
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_dd_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------**/
		}

		g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
		g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
		}
	}
	else
	{
		sprintf(acErrMsg,"借贷标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_dd_mst.lst_date=g_pub_tx.tx_date;

	if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.min_bal)<0 
		&& g_dd_mst.ac_sts[0]=='1' )
	{
		sprintf(acErrMsg,"存款余额[%.2lf]低于下限[%.2lf]",
			g_dd_mst.bal,g_dd_parm.min_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}
	if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_bal)>0 
		&& pub_base_CompDblVal(g_dd_parm.max_bal,0.00) )
	{
		sprintf(acErrMsg,"存款余额[%.2lf]高于上限[%.2lf]",
			g_dd_mst.bal,g_dd_parm.max_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A008" );
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
	{
		ret=pub_ins_dd_hst1();
		ERR_DEAL
	}
	/*****add zgf 20130725****/

		pub_mob_sendmail( g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );

	/***add end zgf*****/
	ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Dd_mst_Clo_Upd();

			sprintf(acErrMsg,"处理活期分户帐PASS!");
			WRITEMSG
OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_td_mst1()
{
	int ret;
    long l_date;
    long num;
	double acm;

	/*** 调用定期产品参数各项检查函数 ***/

	/*** 定期主文件检查 ***/
	ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	if( g_td_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"账号状态非正常状态!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	if( g_td_mst.hold_sts[0]=='1' )
	{
		sprintf(acErrMsg,"该账号已被完全冻结!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P110");
		goto ErrExit;
	}

    /* 给定期产品结构赋值 */
    ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm," prdt_no='%s' ",
                      g_td_mst.prdt_no);
	ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;

	/*** 调用计算利息积数的函数 ***/
    if ( g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' )
        /* 定额续存 定额续取 */
    {
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}
    }

    if ( g_pub_tx.add_ind[0]=='1') 		/*取款*/
	{
		if( g_pub_tx.ct_ind[0]=='1' && (g_td_parm.cif_type[0]=='1' || \
			g_td_parm.cif_type[0] =='2') )
		{
			sprintf(acErrMsg,"对公定期不允许取现!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P113");
			goto ErrExit;
		}

        if (g_pub_tx.ct_ind[0]=='1' && g_td_parm.cash_ind[0] == 'N')
        {
            sprintf(acErrMsg,"该产品不允许提现!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P223");
            goto ErrExit;
        }

		if( g_pub_tx.ct_ind[0]=='1' && g_td_mst.odtt_ind[0]=='1' )
		{
			sprintf(acErrMsg,"汇户不允许取现!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P108");
			goto ErrExit;
		}

        if( g_td_parm.thr_draw_ind[0]=='N' \
              && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
        {
            sprintf(acErrMsg,"不可通兑[%s]!",g_td_parm.thr_draw_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A011");
            goto ErrExit;
        }

        /* 检查帐户余额 */
        if (pub_base_CompDblVal(g_td_mst.bal,g_pub_tx.tx_amt1) == -1)
        {
            sprintf(acErrMsg,"取款额大于帐户余额!!bal=[%lf] amt1=[%lf]",
                g_td_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P214");
            goto ErrExit;
        }

        if ((g_td_parm.td_type[0] != '3' && g_pub_tx.svc_ind==1001) \
            || g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004)
        {
		    g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
		    g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;
        }

	    if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
	    {
	        g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
	    }

		if( g_td_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"该账号已被只进不出冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}

		if( g_td_mst.hold_sts[0]=='3' && g_td_mst.bal<g_td_mst.hold_amt )
		{
			sprintf(acErrMsg,"该账号已被部分冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}
    
        /* 检查是否可以做取款交易 */
        if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '6')
        /* 定额续存 定活两便 */
        {
           sprintf(acErrMsg,"该产品不允许做取款交易!!");
           WRITEMSG
           strcpy(g_pub_tx.reply,"P219");
           goto ErrExit;
        }

        /* 如果是检查是否允许部提 */
        if (g_td_parm.td_type[0] == '0')
        /* 普通整存整取 */
        {
        	/********
			if (g_pub_tx.tx_date >= g_td_mst.mtr_date)
   			{
            	sprintf(acErrMsg,"该账户已经到期,不允许做部提!!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"A034");
				goto ErrExit;
 			}
          	********/
            if (g_td_parm.wdr_ind[0] == 'N')
            {
                sprintf(acErrMsg,"普通整存整取不允许部提!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P215");
                goto ErrExit;
            }

            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1; /* 部提次数 */

            if (g_td_parm.wdr_lmt_cnt != 0 \
                  && g_td_mst.tx_cnt > g_td_parm.wdr_lmt_cnt)
            {
                sprintf(acErrMsg,"部提次数超过规定的次数!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P216");
                goto ErrExit;
            }
        }

        /* 检查最低取款金额 */
        if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
            g_td_parm.draw_min_amt) == -1)
        {
            sprintf(acErrMsg,"取款金额小于最低取款金额!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P217");
            goto ErrExit;
        }

        if (g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0] == '2')
        /* 定额续取 定额取息 */
        {
		   if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 )
			{
			    sprintf(acErrMsg,"取款金额错!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P115");
				goto ErrExit;
			}

       		/*存本取息每次取利息不用修改明细余额*/
            if (g_td_parm.td_type[0] == '3')
            {
                if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
   				{
      				g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
   				}
            }

			g_td_mst.tx_cnt++;
			if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
			{
				sprintf(acErrMsg,"取款超过总次数!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P117");
				goto ErrExit;
			}
            /**********
            if (g_td_mst.ttl_cnt == g_td_mst.tx_cnt)
            {
                sprintf(acErrMsg,"本次是最后一次交易应做销户交易!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P229");
                goto ErrExit;
            }
            ***********/
            switch(g_td_parm.intvl_type[0])
            {
                case 'Y':
                        break;
                case 'Q':
                        break;
                case 'M':
                        num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                		ret = pub_base_deadlineM(g_td_mst.ic_date,num,
                        						 &l_date);
                		if (ret != 0)
                        {
                             sprintf(acErrMsg,"计算间隔取款时间错误!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"P228");
                             goto ErrExit;
                        }  
                        if (g_pub_tx.tx_date < l_date)
                        {
                             sprintf(acErrMsg,"还没有到每次取款的时间!!");
                             WRITEMSG
                             strcpy(g_pub_tx.reply,"P218");
                             goto ErrExit;
                        }
                         break;
                case 'D':
                        num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                        ret = pub_base_deadlineD(g_td_mst.ic_date,num,
                   					             &l_date);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"计算间隔取款时间错误!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P228");
                            goto ErrExit;
                        }
                        if (g_pub_tx.tx_date < l_date)
                        {
                            sprintf(acErrMsg,"还没有到每次取款的时间!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P218");
                            goto ErrExit;
                        }
                        break;
                 default:
                        break;
              }/*switch结束*/
           }

           /* 计算利息 */
           if (g_td_parm.td_type[0] != '2' && g_td_parm.td_type[0] != '3')
           /* 定额续取 存本取息 不用计算利息 */
           {
               ret = pub_base_CalIntst(&g_pub_intst);
               if (ret != 0)
               {
                   sprintf(acErrMsg,"计算存本取息利息错误!!");
                   WRITEMSG
                   strcpy(g_pub_tx.reply,"P220");
                   goto ErrExit;
               }
               sprintf(acErrMsg,"取款利息为:,[%lf]",g_pub_intst.factval);
               WRITEMSG
           }
			
		/*计算取款积数*/
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

		/*计算分段利息积数*/
		if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
			g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
			0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
			g_dd_parm.intst_mon,g_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算分段利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

		/*登记分段积数表*/
		com_sect_acm_c.ac_id=g_pub_tx.ac_id;
		com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
		com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*错帐日期*/
		com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*发生日期*/
		com_sect_acm_c.rate=g_dd_mst.rate/12;				/*月利率*/
		com_sect_acm_c.intst_acm=acm;						/*利息积数*/
		strcpy(com_sect_acm_c.sts,"0");						/*状态*/
		ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
		if(ret==-239)
		{
			sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D108" );
			goto ErrExit;;
		}
		ERR_DEAL
	}
	else if( g_pub_tx.add_ind[0]=='2')						/*记贷,存款*/
	{
        if (g_td_parm.td_type[0] != '1')
        {
            sprintf(acErrMsg,"该产品的定期存款类型不允许做存款交易!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P225");
            goto ErrExit;
        }
        if (g_td_parm.dpst_ind[0] == 'N')
        {
            sprintf(acErrMsg,"该产品不允许续存!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P225");
            goto ErrExit;
        }
        if ( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
              && g_td_parm.thr_dpst_ind[0] == 'N')
        {
            sprintf(acErrMsg,"该产品不允许通存!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P226");
            goto ErrExit;
        }

		g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
		g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
		{
			g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
			g_td_mst.hst_cnt++;
		}

		if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 )
		{
			sprintf(acErrMsg,"存款金额错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P118");
			goto ErrExit;
		}

        if (pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_parm.dpst_min_amt) == -1)
        {
            sprintf(acErrMsg,"存款金额小于最低续存金额!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P227");
            goto ErrExit;
        }

		/*计算存款积数*/
		if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
			g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
			g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
			g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

		/*计算分段利息积数*/
		if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
			g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
			0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
			g_dd_parm.intst_mon,g_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算分段利息积数错误!");
			WRITEMSG
			goto ErrExit;
		}

		/*登记分段积数表*/
		com_sect_acm_c.ac_id=g_pub_tx.ac_id;
		com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
		com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*错帐日期*/
		com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*发生日期*/
		com_sect_acm_c.rate=g_dd_mst.rate/12;				/*月利率*/
		com_sect_acm_c.intst_acm=acm;						/*利息积数*/
		strcpy(com_sect_acm_c.sts,"0");						/*状态*/
		ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
		if(ret==-239)
		{
			sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "D108" );
			goto ErrExit;;
		}
		ERR_DEAL
	}
	else
	{
		sprintf(acErrMsg,"借贷标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_td_mst.lst_date=g_pub_tx.tx_date;

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_bal)<0 
		&& g_td_mst.ac_sts[0]=='1' )
	{
		sprintf(acErrMsg,"存款余额[%.2lf]低于下限[%.2lf]",
			g_td_mst.bal,g_td_parm.min_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}
	if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_bal)>0 
		&& pub_base_CompDblVal(g_td_parm.max_bal,0.00) )
	{
		sprintf(acErrMsg,"存款余额[%.2lf]高于上限[%.2lf]",
			g_td_mst.bal,g_td_parm.max_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A008" );
		goto ErrExit;
	}
    if (g_pub_tx.hst_ind[0]== '1') /*日间入明细*/
    {
		strcpy(g_pub_tx.name,g_td_mst.name);
		g_pub_tx.cif_no=g_td_mst.cif_no;
		ret=pub_ins_td_hst();
		ERR_DEAL
    }
	ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Td_mst_Clo_Upd();
			sprintf(acErrMsg,"处理定期分户帐PASS!");
			WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_ln_mst1()
{
	int ret;
	int do_flag=0;
	double acm;

	/*** 调用贷款产品参数各项检查函数 ***/

	if( g_pub_tx.ac_get_ind[1]!='1' ) /**未读取ln_mst**/
	{
		ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		ERR_DEAL

		ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
			WRITEMSG
			if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}

	ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
	ERR_DEAL

	strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );
	strcpy( g_pub_tx.opn_br_no,g_ln_mst.opn_br_no );
	strcpy( g_pub_tx.name,g_ln_mst.name );
	g_pub_tx.cif_no=g_ln_mst.cif_no;

	/**账户状态检查**/
	if( g_ln_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"贷款账号已销户![%s]",g_ln_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P120");
		goto ErrExit;
	}
	if( g_ln_mst.ac_sts[0]=='9' )
	{
		sprintf(acErrMsg,"贷款账号已撤销![%s]",g_ln_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L036");
		goto ErrExit;
	}

	/*** 调用计算利息积数的函数 ***/
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2",g_ln_mst.bal,g_pub_tx.tx_amt1 , 
							&g_ln_mst.intst_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2",g_ln_mst.in_lo_intst,g_pub_tx.tx_amt2, 
							&g_ln_mst.in_lo_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2" , g_ln_mst.out_lo_intst , g_pub_tx.tx_amt3,
							&g_ln_mst.out_lo_acm ,"0",0,0 );
		ERR_DEAL
	ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
							"2" , g_ln_mst.cmpd_lo_intst , g_pub_tx.tx_amt4,
							&g_ln_mst.cmpd_lo_acm ,"0",0,0 );
		ERR_DEAL
	g_ln_mst.lst_date=g_pub_tx.tx_date;

	/**如果是积数结息，清理积数，注：分段积数由各程序自己清**/

	if( g_pub_tx.add_ind[0]=='0' ) 				/*记借*/
	{
		if( g_pub_tx.svc_ind==3010 ) 			/*贷款本金增加，即放款*/
		{
			if( g_ln_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg, "状态不对[%s]",
					g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
				WRITEMSG
				strcpy( g_pub_tx.reply,"L067" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
				g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
			{
				sprintf(acErrMsg,
					"超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
					g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P122" );
				goto ErrExit;
			}
			g_ln_mst.bal+=g_pub_tx.tx_amt1;
			g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
			ret=pub_ins_ln_hst( 0 );
			ERR_DEAL
			do_flag=1;
		}

			/*计算分段利息积数*/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			/*登记分段积数表*/
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*错帐日期*/
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*发生日期*/
			com_sect_acm_c.rate=g_ln_mst.rate/12;				/*月利率*/
			com_sect_acm_c.intst_acm=acm;						/*利息积数*/
			strcpy(com_sect_acm_c.sts,"0");						/*状态*/
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			strcpy(g_pub_tx.add_ind,"1");
	}
	else if( g_pub_tx.add_ind[0]=='1' ) 		/*记贷,帐户余额减少*/
	{
		if( g_pub_tx.svc_ind==3010 ) /*贷款本金减少，即还本*/
		{
			g_ln_mst.bal-=g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 )
			{
				sprintf(acErrMsg,"还本金额过大[%.2lf]>[%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
				WRITEMSG
				strcpy( g_pub_tx.reply,"L039" );
				goto ErrExit;
			}
			ret=pub_ins_ln_hst( 0 );
			ERR_DEAL
			do_flag=1;
		}

			/*计算分段利息积数*/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			/*登记分段积数表*/
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			/*错帐日期*/
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			/*发生日期*/
			com_sect_acm_c.rate=g_ln_mst.rate/12;				/*月利率*/
			com_sect_acm_c.intst_acm=acm;						/*利息积数*/
			strcpy(com_sect_acm_c.sts,"0");						/*状态*/
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			strcpy(g_pub_tx.add_ind,"0");
	}
	else
	{
		sprintf(acErrMsg,"借贷标志错误[%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	if( do_flag==0 )
	{
		sprintf(acErrMsg,"标志错误[%s][%d]",g_pub_tx.add_ind,g_pub_tx.svc_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
	{
		g_ln_mst.hst_bal=g_ln_mst.hst_bal-g_pub_tx.tx_amt1;
		ret=pub_ins_ln_hst1( 1 );
		ERR_DEAL
	}

	ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	Ln_mst_Clo_Upd();
			sprintf(acErrMsg,"处理贷款分户帐PASS!");
			WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int Chg_in_mst1()
{
	struct com_item_c v_com_item;
	int ret;
	double acm;
	/*** 调用内部产品参数各项检查函数 ***/

	/*** 内部主文件检查 ***/
	ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**读取产品参数**/
	ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",g_in_mst.prdt_no);
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_in_parm.dc_code); 			/*会计代码*/

	/**读取科目定义**/
	ret = Com_item_Sel( g_pub_tx.reply,&v_com_item,
						"acc_hrt='%s'",g_in_parm.acc_hrt );
	ERR_DEAL
	if( g_in_mst.sts[0]!='1' && g_in_mst.sts[0]!='2' )
	{
		sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]", \
				g_in_mst.ac_id,g_in_mst.sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	g_in_mst.lst_date=g_pub_tx.tx_date;

	if( g_pub_tx.add_ind[0]=='0' )					/*借*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"该账户已经反向冻结!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"科目发生额方向贷方");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A025");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='2' )
		{
			sprintf(acErrMsg,"产品发生额方向贷方");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A027");
			goto ErrExit;
		}

		if( v_com_item.dc_ind[0]=='2' )					/*科目为贷方向*/
		{
			/*** 调用计算利息积数函数 ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

			/*计算分段利息积数*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------------------**/
		}
		else											/*科目为借方向*/
		{
			/*** 调用计算利息积数函数 ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;

			/*计算分段利息积数*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**--------------------------------**/
			strcpy(g_pub_tx.add_ind,"1");
		}
	}
	else if ( g_pub_tx.add_ind[0]=='1' )	/*贷*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"该账户已经反向冻结!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"科目发生额方向借方");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A024");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='1' )
		{
			sprintf(acErrMsg,"产品发生额方向借方");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A026");
			goto ErrExit;
		}
		if( v_com_item.dc_ind[0]=='2' )					/*科目为贷方向*/
		{
			/*** 调用计算利息积数函数 ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_pub_tx.tx_amt1,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;

			/*计算分段利息积数*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, 
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**---------------------------------------**/
		}
		else											/*科目为借方向*/
		{
			/*** 调用计算利息积数函数 ***/
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				-g_pub_tx.tx_amt1,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"计算利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

			/*计算分段利息积数*
			if( pub_base_CalAcm(g_pub_tx.erro_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				0.00,-g_pub_tx.tx_amt1,&acm,g_dd_mst.intst_type, \
				g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"计算分段利息积数错误!");
				WRITEMSG
				goto ErrExit;
			}

			*登记分段积数表*
			com_sect_acm_c.ac_id=g_pub_tx.ac_id;
			com_sect_acm_c.ac_seqn=g_pub_tx.ac_seqn;
			com_sect_acm_c.beg_date=g_pub_tx.erro_date;			*错帐日期*
			com_sect_acm_c.end_date=g_pub_tx.tx_date;			*发生日期*
			com_sect_acm_c.rate=g_in_mst.rate/12;				*月利率*
			com_sect_acm_c.intst_acm=acm;						*利息积数*
			strcpy(com_sect_acm_c.sts,"0");						*状态*
			ret=Com_sect_acm_Ins(com_sect_acm_c,g_pub_tx.reply);
			if(ret==-239)
			{
				sprintf(acErrMsg,"登记分段积数表出错!![%d]",ret);
				WRITEMSG
				strcpy ( g_pub_tx.reply , "D108" );
				goto ErrExit;;
			}
			ERR_DEAL
			**-------------------------------------------**/
			strcpy(g_pub_tx.add_ind,"0");
		}
	}
	else
	{
		sprintf(acErrMsg,"借贷标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( pub_base_CompDblVal(g_in_mst.bal,0.00)<0 
		&& g_in_mst.od_ind[0]=='N')
	{
		sprintf(acErrMsg,"该账户余额不允许透支![%.2f][%.2f]",
				g_in_mst.bal,g_pub_tx.tx_amt1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P124");
		goto ErrExit;
	}

	if( g_in_mst.od_ind[0]=='Y' 
		&& pub_base_CompDblVal(g_in_parm.od_amt,0.00)
		&& pub_base_CompDblVal(-g_in_mst.bal,g_in_parm.od_amt)>0 )
	{
		sprintf(acErrMsg,"透支超限![%.2f][%.2f]",
				g_in_mst.bal,g_in_parm.od_amt );
		WRITEMSG
		strcpy(g_pub_tx.reply,"T004");
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*日间入明细*/
	{
		ret=pub_ins_in_hst();
		ERR_DEAL
	}

	ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	In_mst_Clo_Upd();

			sprintf(acErrMsg,"处理内部分户帐PASS!");
			WRITEMSG
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*科目记账*  旧的实时改总帐**/
int Chg_acchrt_mst1_old()
{
	int ret=0,i=0; 
	double amt;
	char wherelist[200];

	struct gl_mst_c g_gl_mst;
	struct com_item_c g_com_item;
	struct com_item_c f_com_item;
	struct tx_dc_br_rel_c g_tx_dc_br_rel;

	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&g_com_item,0x00,sizeof(struct com_item_c));
	memset(&f_com_item,0x00,sizeof(struct com_item_c));

	pub_base_strpack(g_pub_tx.cur_no);
	if( !strlen(g_pub_tx.cur_no) ) /**默认为人民币**/
	strcpy( g_pub_tx.cur_no,"01" );

	/*检查科目类型*/
	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该科目号不存在[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P100");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查询公共科目表异常[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	/*科目类型必须为无帐户类型*/
	if(g_com_item.acc_knd[0]!='3')
	{
		sprintf(acErrMsg,"该科目类型不是无帐户科目!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O073");
		goto ErrExit;
	}

	/**必须无下级科目**/
	if(g_com_item.sub_acc_yn[0]=='Y')
	{
		sprintf(acErrMsg,"该科目还有下级科目!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O073");
		goto ErrExit;
	}

	/**取得对应的会计机构**/
	ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&g_tx_dc_br_rel,"tx_br_no='%s'",
						 g_pub_tx.tx_br_no );
	if( ret )
	{
		sprintf(acErrMsg,"交易机构无对应会计机构[%s]",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S029");
		goto ErrExit;
	}

	memset( wherelist,0,sizeof(wherelist) );
	memcpy( &f_com_item,&g_com_item,sizeof(g_com_item) );
	pub_base_strpack( f_com_item.acc_hrt );
	pub_base_strpack( f_com_item.up_acc_hrt );

	strcpy( g_pub_tx.dc_code,"ACHR" );
	set_zd_data("0152",f_com_item.acc_hrt );

	sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
			 g_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no,f_com_item.acc_hrt );
	/**取得上级科目**/
	for ( i=0;i<2;i++ )
	{
	 	if( !strcmp(f_com_item.acc_hrt,f_com_item.up_acc_hrt) )
	 	break;

		ret=Com_item_Sel(g_pub_tx.reply,&f_com_item,"acc_hrt='%s'",
			 			 f_com_item.up_acc_hrt);
		if( ret )
		{
			sprintf(acErrMsg,"该科目控制字不存在[%s]",f_com_item.up_acc_hrt);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"P102");
		    goto ErrExit;
	    }
	  	pub_base_strpack( f_com_item.acc_hrt );
	  	pub_base_strpack( f_com_item.up_acc_hrt );
	   	strcat( wherelist,",'" );
	    strcat( wherelist,f_com_item.acc_hrt );
		strcat( wherelist,"'" );
	}

	 strcat( wherelist,")" );

	 sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
	 WRITEMSG

	/*修改总帐*/
	ret=Gl_mst_Dec_Upd(g_pub_tx.reply,wherelist);
	if( ret )
	{
		sprintf(acErrMsg,"查询总帐异常!![%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	i=0;
	while(1)
	{
		ret=Gl_mst_Fet_Upd(&g_gl_mst,g_pub_tx.reply);
		if( ret==100 )break;
		else if(ret)
		{
			sprintf(acErrMsg,"查询总帐异常!![%s]",g_pub_tx.ac_no);
			WRITEMSG
			goto ErrExit;
		}
		i++;

		switch(g_pub_tx.add_ind[0])								/*交易借贷类型*/
		{	
			case '0':											/*借*/
					if(g_com_item.amt_dc_ind[0]=='2')
					{
						sprintf(acErrMsg,"该科目只允许记贷!![%s]",g_pub_tx.ac_no);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P102");
						goto ErrExit;
					}

					if(g_com_item.dc_ind[0]=='1')				/*借处理*/
						g_gl_mst.dr_bal+=g_pub_tx.tx_amt1;		/*当前借方余额*/
					else if(g_com_item.dc_ind[0]=='2')			/*贷处理*/
						g_gl_mst.cr_bal-=g_pub_tx.tx_amt1;		/*当前贷方余额*/
					else if(g_com_item.dc_ind[0]=='0')			/*双向处理*/
					{
						amt=g_gl_mst.dr_bal-g_gl_mst.cr_bal;
						amt+=g_pub_tx.tx_amt1;
						ret=pub_base_CompDblVal(amt,0.00);
						if(ret==1)
						{
							g_gl_mst.dr_bal=amt;		/*当前借方余额*/
							g_gl_mst.cr_bal=0.00;		/*当前贷方余额*/
						}
						else if(ret==-1)
						{
							g_gl_mst.cr_bal=-amt;		/*当前贷方余额*/
							g_gl_mst.dr_bal=0.00;		/*当前借方余额*/
						}
						else
						{
							g_gl_mst.dr_bal=0.00;       /*当前借方余额*/
							g_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
						}
					}
					g_gl_mst.rdd_cnt+=1;		/*本日借笔数*/
					g_gl_mst.rdd_amt+=amt;		/*本日借发生额*/
					g_gl_mst.tddr_cnt+=1;		/*本旬借笔数*/
					g_gl_mst.tddr_amt+=amt;		/*本旬借发生额*/
					g_gl_mst.mdr_cnt+=1;		/*本月借笔数*/
					g_gl_mst.mdr_amt+=amt;		/*本月借发生额*/
					g_gl_mst.qdr_cnt+=1;		/*本季借笔数*/
					g_gl_mst.qdr_amt+=amt;		/*本季借发生额*/
					g_gl_mst.ydr_cnt+=1;		/*本年借笔数*/
					g_gl_mst.ydr_amt+=amt;		/*本年借发生额*/
					break;
			case '1':												/*交易记贷*/
					if(g_com_item.amt_dc_ind[0]=='1')
					{
						sprintf(acErrMsg,"该科目只允许记借!![%s]",g_pub_tx.ac_no);
						WRITEMSG
						strcpy(g_pub_tx.reply,"P102");
						goto ErrExit;
					}

					if(g_com_item.dc_ind[0]=='1')				/*借处理*/
						g_gl_mst.cr_bal-=g_pub_tx.tx_amt1;		/*当前贷方余额*/
					else if(g_gl_mst.dc_ind[0]=='2')			/*贷处理*/
						g_gl_mst.dr_bal+=g_pub_tx.tx_amt1;		/*当前贷方余额*/
					else if(g_com_item.dc_ind[0]=='0')			/*双向处理*/
					{
						amt=g_gl_mst.dr_bal-g_gl_mst.cr_bal;
						amt=amt+g_pub_tx.tx_amt1;
						ret=pub_base_CompDblVal(amt,0.00);
						if(ret==1)
						{
							g_gl_mst.dr_bal=amt;		/*当前借方余额*/
							g_gl_mst.cr_bal=0.00;		/*当前贷方余额*/
						}
						else if(ret==-1)
						{
							g_gl_mst.cr_bal=-amt;		/*当前贷方余额*/
							g_gl_mst.dr_bal=0.00;		/*当前借方余额*/
						}
						else
						{
							g_gl_mst.dr_bal=0.00;       /*当前借方余额*/
							g_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
						}
					}	
					g_gl_mst.rcd_cnt+=1;					/*本日贷笔数*/
					g_gl_mst.rcd_amt+=g_pub_tx.tx_amt1;		/*本日贷发生额*/
					g_gl_mst.tdcr_cnt+=1;					/*本旬贷笔数*/
					g_gl_mst.tdcr_amt+=g_pub_tx.tx_amt1;	/*本旬贷发生额*/
					g_gl_mst.mcr_cnt+=1;					/*本月贷笔数*/
					g_gl_mst.mcr_amt+=g_pub_tx.tx_amt1;		/*本月贷发生额*/
					g_gl_mst.qcr_cnt+=1;					/*本季贷笔数*/
					g_gl_mst.qcr_amt+=g_pub_tx.tx_amt1;		/*本季贷发生额*/
					g_gl_mst.ycr_cnt+=1;					/*本年贷笔数*/
					g_gl_mst.ycr_amt+=g_pub_tx.tx_amt1;		/*本年贷发生额*/
					break;
			default:
					sprintf(acErrMsg,"存取标志错误=[%s]!",g_pub_tx.add_ind);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P106");
					goto ErrExit;
					break;
		}
	}

	ret=Gl_mst_Upd_Upd(g_gl_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Update 总帐出错!![%s]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	Gl_mst_Clo_Upd();

	if( i==0 )
	{
		sprintf(acErrMsg,"总帐中不存在该科目g_gl_mst.acc_hrt=[%s]", \
				g_gl_mst.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O075");
		goto ErrExit;
	}

		sprintf(acErrMsg,"处理科目总帐PASS!");
		WRITEMSG

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_ins_ln_trace_log1( double amt )
{
	double v_amt;
	int ret=0;

	v_amt=g_pub_tx.tx_amt1;
	g_pub_tx.tx_amt1=amt;
	ret=pub_ins_trace_log();
	ERR_DEAL
	g_pub_tx.tx_amt1=v_amt;

OkExit:
	return 0;
ErrExit:
	return 1;
}
int pub_ins_trace_log1()
{
	struct trace_log_c	trace_log_c;
	int ret;

	memset(&trace_log_c,0x00,sizeof(struct trace_log_c));
	
	trace_log_c.trace_no=g_pub_tx.trace_no;
	trace_log_c.tx_date=g_pub_tx.tx_date;
	trace_log_c.tx_time=g_pub_tx.tx_time;

	strcpy(trace_log_c.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(trace_log_c.opn_br_no,g_pub_tx.opn_br_no);

	strcpy(trace_log_c.tx_code,g_pub_tx.tx_code);
    strcpy(trace_log_c.sub_tx_code,g_pub_tx.sub_tx_code);
	trace_log_c.svc_ind=g_pub_tx.svc_ind;

	strcpy(trace_log_c.cur_no,g_pub_tx.cur_no);
	strcpy(trace_log_c.ct_ind,g_pub_tx.ct_ind);
	strcpy(trace_log_c.ac_no,g_pub_tx.ac_no);
	trace_log_c.ac_seqn=g_pub_tx.ac_seqn;
	trace_log_c.ac_id=g_pub_tx.ac_id;

	strcpy(trace_log_c.note_type,g_pub_tx.note_type);
	strcpy(trace_log_c.note_no,g_pub_tx.beg_note_no);
	strcpy(trace_log_c.saving_notarize,"0");

	if( g_pub_tx.add_ind[0]=='1' )
		strcpy(trace_log_c.add_ind,g_pub_tx.add_ind);
	else
		strcpy(trace_log_c.add_ind,"0");
	trace_log_c.amt=g_pub_tx.tx_amt1;

	strcpy(trace_log_c.brf,g_pub_tx.brf);
	strcpy(trace_log_c.hst_ind,g_pub_tx.hst_ind);
	strcpy(trace_log_c.tel,g_pub_tx.tel);
	strcpy(trace_log_c.chk,g_pub_tx.chk);
	strcpy(trace_log_c.aut,g_pub_tx.auth);
	strcpy(trace_log_c.sts,"0");
	strcpy(trace_log_c.no_show,"0");

	if( pub_base_CompDblVal(trace_log_c.amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[1]=='1' )
	{
			get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
		g_pub_tx.trace_cnt++;
		trace_log_c.trace_cnt=g_pub_tx.trace_cnt;
			set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
		ret=Trace_log_Ins(trace_log_c,g_pub_tx.reply);
		if( ret )
		{
			vtcp_log("=====[%.2lf]",trace_log_c.amt);
			sprintf(acErrMsg,"登记交易流水错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P126");
			goto ErrExit;
		}
	}

	return 0;
ErrExit:
	return 1;
}

int pub_ins_dd_hst1()
{
	int ret;

	struct dd_mst_hst_c	dd_mst_hst_c;
	memset(&dd_mst_hst_c,0x00,sizeof(struct dd_mst_hst_c));

	dd_mst_hst_c.trace_no=g_pub_tx.trace_no;
	dd_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(dd_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(dd_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	dd_mst_hst_c.ac_id=g_pub_tx.ac_id;
	dd_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(dd_mst_hst_c.tx_code,g_pub_tx.tx_code);	
	strcpy(dd_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);	

	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' 
		|| g_pub_tx.add_ind[0]=='3' )
		strcpy(dd_mst_hst_c.add_ind,"0");	
	else
		strcpy(dd_mst_hst_c.add_ind,"1");	
    strcpy(dd_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	dd_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	dd_mst_hst_c.bal=g_dd_mst.hst_bal;
	dd_mst_hst_c.acbo_bal=g_dd_mst.acbo_bal;
	dd_mst_hst_c.intst_acm=g_dd_mst.intst_acm;
	dd_mst_hst_c.tx_date=g_pub_tx.tx_date;
	dd_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(dd_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strncpy(dd_mst_hst_c.brf,g_pub_tx.brf,10);	
	strcpy(dd_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(dd_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(dd_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(dd_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_dd_mst.hst_cnt++;
		dd_mst_hst_c.hst_cnt=g_dd_mst.hst_cnt;
		ret=Dd_mst_hst_Ins(dd_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记活期明细帐错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P127");
			goto ErrExit;
		}
	}

OkExit:	
	return 0;
ErrExit:
	return 1;
}

int pub_ins_td_hst1()
{
	int ret;
	struct td_mst_hst_c	td_mst_hst_c;
	memset(&td_mst_hst_c,0x00,sizeof(struct td_mst_hst_c));

	td_mst_hst_c.trace_no=g_pub_tx.trace_no;
	td_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(td_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(td_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	td_mst_hst_c.ac_id=g_pub_tx.ac_id;
	td_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	strcpy(td_mst_hst_c.tx_code,g_pub_tx.tx_code);	
	strcpy(td_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);	
TRACE
	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
		strcpy(td_mst_hst_c.add_ind,"0");	
	else
		strcpy(td_mst_hst_c.add_ind,"1");	
    strcpy(td_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	td_mst_hst_c.bal=g_td_mst.hst_bal;
	td_mst_hst_c.acbo_bal=g_td_mst.acbo_bal;
	td_mst_hst_c.intst_acm=g_td_mst.intst_acm;
	td_mst_hst_c.tx_date=g_pub_tx.tx_date;
	td_mst_hst_c.tx_time=g_pub_tx.tx_time;
TRACE
	strcpy(td_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strcpy(td_mst_hst_c.brf,g_pub_tx.brf);	
	strcpy(td_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(td_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(td_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(td_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_td_mst.hst_cnt++;
		td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
		ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记定期明细帐错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P128");
			goto ErrExit;
		}
	}

    sprintf(acErrMsg,"登记定期帐户明细成功!!!");
    WRITEMSG
	return 0;
ErrExit:
    sprintf(acErrMsg,"登记定期帐户明细失败!!!");
    WRITEMSG
	return 1;
}
/* mode 1本金 2表内 3表外 4复利 */
int pub_ins_ln_hst1( int mode )
{
	struct ln_mst_hst_c	ln_mst_hst_c;
	int ret=0;

	memset(&ln_mst_hst_c,0x00,sizeof(ln_mst_hst_c));

	ln_mst_hst_c.trace_no=g_pub_tx.trace_no;

	strcpy(ln_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(ln_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
	ln_mst_hst_c.ac_id=g_pub_tx.ac_id;
	ln_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;

	strcpy(ln_mst_hst_c.tx_code,g_pub_tx.tx_code);
	strcpy(ln_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);
	strcpy(ln_mst_hst_c.add_ind,g_pub_tx.add_ind);
    strcpy(ln_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
	ln_mst_hst_c.tx_date=g_pub_tx.tx_date;
	ln_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(ln_mst_hst_c.note_type,g_pub_tx.note_type);
	strcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no);

	strncpy(ln_mst_hst_c.brf,g_pub_tx.brf,10);
	strcpy(ln_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(ln_mst_hst_c.chk,g_pub_tx.chk);
	strcpy(ln_mst_hst_c.auth,g_pub_tx.auth);

	if( mode==0 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"0");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
		ln_mst_hst_c.bal=g_ln_mst.bal;
		ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
	}
	else if( mode==1 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"1");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
		ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
	}
	else if( mode==2 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"2");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
		ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
	}
	else if( mode==3 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"3");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
		ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
	}
	else if( mode==4 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"4");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
		ln_mst_hst_c.bal=g_ln_mst.bal;
		ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
	}
	else if( mode==5 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"5");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
		ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
	}
	else if( mode==6 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"6");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
		ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
	}
	else if( mode==7 )
	{
		strcpy(ln_mst_hst_c.ln_tx_type,"7");
		ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
		ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
		ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
	}
	else
	{
		sprintf(acErrMsg,"登记贷款明细帐mode错误[%d]!",mode);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P129");
		goto ErrExit;
	}

	ln_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;

	if ( g_pub_tx.hst_ind[0]!='1' ) goto GoodExit;

	if( ( pub_base_CompDblVal(ln_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_ln_mst.hst_cnt++;
		ln_mst_hst_c.hst_cnt=g_ln_mst.hst_cnt;

		ret=Ln_mst_hst_Ins(ln_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记贷款明细帐错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P129");
			goto ErrExit;
		}
	}

	ret=pub_ins_ln_trace_log(ln_mst_hst_c.tx_amt);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

int pub_ins_in_hst1()
{
	int ret;
	struct in_mst_hst_c	in_mst_hst_c;
	memset(&in_mst_hst_c,0x00,sizeof(struct in_mst_hst_c));

	in_mst_hst_c.trace_no=g_pub_tx.trace_no;
	in_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(in_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);	
	strcpy(in_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);	
	in_mst_hst_c.tx_date=g_pub_tx.tx_date;
	in_mst_hst_c.tx_time=g_pub_tx.tx_time;
	in_mst_hst_c.ac_id=g_pub_tx.ac_id;
	in_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
		strcpy(in_mst_hst_c.add_ind,"0");	
	else
		strcpy(in_mst_hst_c.add_ind,"1");	
	in_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
	in_mst_hst_c.bal=g_in_mst.hst_bal;
	in_mst_hst_c.intst_acm=g_in_mst.intst_acm;
	strcpy(in_mst_hst_c.note_type,g_pub_tx.note_type);	
	strcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no);	
	strncpy(in_mst_hst_c.brf,g_pub_tx.brf,10);	
	strcpy(in_mst_hst_c.tel,g_pub_tx.tel);	
	strcpy(in_mst_hst_c.chk,g_pub_tx.chk);	
	strcpy(in_mst_hst_c.auth,g_pub_tx.auth);	

	if( ( pub_base_CompDblVal(in_mst_hst_c.tx_amt,0.00) 
		|| g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
	{
		g_in_mst.hst_cnt++;
		in_mst_hst_c.hst_cnt=g_in_mst.hst_cnt;

		ret=In_mst_hst_Ins(in_mst_hst_c,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"登记内部明细帐错误[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P128");
			goto ErrExit;
		}
	}
	
	return 0;
ErrExit:
	return 1;
}

static pub_sub_ac1()
{
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id_c;
	char v_ac_get_ind[2];
	int ret=0;

	v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_strpack(g_pub_tx.crd_no);

	/*输入卡号的情况*/
	if( strlen(g_pub_tx.crd_no) && ! strlen(g_pub_tx.ac_no) )
	{
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
	}

	/*已经读取了介质账号*/
	if( g_pub_tx.ac_get_ind[0]=='1' ) 				
	{
		if( g_mdm_ac_rel.ac_id<=0 ) 
		{
			sprintf(acErrMsg,"标识ac_get_ind 错误[%s][%ld]",
					g_pub_tx.ac_get_ind,g_mdm_ac_rel.ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
		strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
	}
	else		/*未读取介质帐号*/
	{
		/*** 检查账号、账号ID和账号序号 ***/
		if( g_pub_tx.ac_no[0]!=NULL )
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
								"ac_no='%s' ",g_pub_tx.ac_no);
			if( ret )
			{
				sprintf(acErrMsg,"显示账号不存在[%s][%s]", \
						g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind);
				WRITEMSG
				if( ret==100 ) 
				{
					if( g_pub_tx.ac_wrk_ind[6]=='1' )
					{
						strcpy(g_pub_tx.reply,"P102");		/*允许它行帐号*/
						goto ErrExit2;
					}	
					strcpy(g_pub_tx.reply,"P102");
				}
				goto ErrExit;
			}
			if( g_pub_tx.ac_id && g_pub_tx.ac_id!=g_mdm_ac_rel.ac_id )
			{
				sprintf(acErrMsg,"账号[%s]和账号ID[%ld]不是一个户",
						g_pub_tx.ac_no,g_pub_tx.ac_id);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
			strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
			g_pub_tx.ac_get_ind[0]='1';
		}
		else if( g_pub_tx.ac_id )
		{
			/**没法读取mdm_ac_rel,找最贴近的一个**/
			g_pub_tx.ac_get_ind[0]='0';
			ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_pub_tx.ac_id);
			ERR_DEAL
			while(1)
			{
				ret=Mdm_ac_rel_Fet_Sel( &v_mdm_ac_rel,g_pub_tx.reply );
				if( ret==100 ) break;
				ERR_DEAL

				if( v_mdm_ac_rel.ac_seqn==g_pub_tx.ac_seqn )
				{
					memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
					g_pub_tx.ac_get_ind[0]='1';
					break;
				}
				else if( v_mdm_ac_rel.ac_seqn==ALL_SEQN && 
					(v_mdm_ac_rel.note_sts[0]=='0' || 
					v_mdm_ac_rel.note_sts[0]=='1') 
					&& g_mdm_ac_rel.ac_id!=g_pub_tx.ac_id )
				{
					g_pub_tx.ac_get_ind[0]='1';
					memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
				}
			}
			if( g_pub_tx.ac_get_ind[0]=='0' )
			{
				sprintf(acErrMsg,"mdm_ac_rel中无匹配纪录acid=[%d]ac_seqn=[%d]",
						g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn );
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
			strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
		}
		else
		{
			sprintf(acErrMsg,"账号[%s]和账号ID[%ld]都为空",
					g_pub_tx.ac_no,g_pub_tx.ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P101");
			goto ErrExit;
		}
	}
	strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

	/**检查已经输入的序号与找到的比较一下**/
	if( g_pub_tx.ac_seqn>=0 )
	{
		if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && 
			g_mdm_ac_rel.ac_seqn!=g_pub_tx.ac_seqn )
		{
			sprintf(acErrMsg,"mdm_ac_rel序号不同acid=[%d]seqn=[%d]in[%d]",
					g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.ac_seqn );
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}
	else if( g_pub_tx.ac_seqn==0 )
	{
		if( g_mdm_ac_rel.ac_seqn==ALL_SEQN )
		{
			if( !strncmp(g_mdm_ac_rel.mdm_code,"0016",4) )
			  	g_pub_tx.ac_seqn = 200;
			else
			{
				ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
				if( ret ) goto ErrExit;
			}
		}
	    else
		{
			g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
		}
	}

	/**介质状态检查**/
	if( g_pub_tx.ac_get_ind[0]=='1' )
	{
		strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);

		if( g_mdm_ac_rel.note_sts[0]=='1' )
		{
			sprintf(acErrMsg,"该介质处于挂失状态,请作解挂处理!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M009");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='2' )
		{
			sprintf(acErrMsg,"该介质处于挂失换证状态,请先作挂失换证!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M010");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='3' )
		{
			sprintf(acErrMsg,"该介质已换证!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M011");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='4' )
		{
			sprintf(acErrMsg,"该账户已作部提!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M017");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='5' )
		{
			sprintf(acErrMsg,"该账户已作撤销!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M019");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]=='*' )
		{
			sprintf(acErrMsg,"该介质已作销户处理!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M012");
			goto ErrExit;
		}
		else if( g_mdm_ac_rel.note_sts[0]!='0' )
		{
			sprintf(acErrMsg,"该介质状态不正常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"M013");
			goto ErrExit;
		}
	}

	/*** 取账户类型,产品编码 如果已经确定了就不必了***/
	if( g_pub_tx.ac_id_type[0]=='0' || g_pub_tx.ac_id_type[0]==NULL )
	{
		if( g_pub_tx.ac_seqn>0 )
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c, \
								"ac_id=%ld and ac_seqn=%d ", \
								g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		else
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
								"ac_id=%ld ",g_pub_tx.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在![%ld][%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
		strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
	}
	else
	{
		/**已经确定类型,由类型取账户序号**/
		if( g_pub_tx.ac_seqn<=0 )
		{
			ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
								"ac_id=%ld and ac_id_type='%c' ",
								g_pub_tx.ac_id,g_pub_tx.ac_id_type[0]);
			if( ret )
			{
				sprintf(acErrMsg,"账号不存在![%ld][%d]",
					g_pub_tx.ac_id,g_pub_tx.ac_seqn);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
			strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
		}
	}

	if( g_pub_tx.ac_id_type[0]=='9' ) /**内部帐，不区分序号**/
	{
		if( g_pub_tx.ac_seqn==0 )
			g_pub_tx.ac_seqn=SINGLE_SEQN;
	}

	g_pub_tx.ac_get_ind[0]=v_ac_get_ind[0];

GoodExit:
	return 0;
ErrExit:
	return 1;
ErrExit2:
	return 100;
}

pub_sub_mst1()
{
	int ret=0 ;

	/*** 检查处理分户帐,明细账 ***/
	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':	
				ret=Chg_dd_mst1();
				ERR_DEAL
				break;
		case '2':	
				/**---- xxx---- 定期不允许充正不记 ----**
				ret=Chg_td_mst1();
				ERR_DEAL
				**-------------------------------------**/
				sprintf(acErrMsg,"定期不允许冲正补记![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P451");
				goto ErrExit;
		case '3':	
				/**----- xxx ---- 贷款用贷款的冲正补记交易 ----**
				ret=Chg_ln_mst1();
				ERR_DEAL
				**---------------------------------------------**/
				sprintf(acErrMsg,"请用贷款的冲正补记![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P452");
				goto ErrExit;
				break;
		case '9':	
				ret=Chg_in_mst1();
				ERR_DEAL
				break;
		default:	
				sprintf(acErrMsg,"无此账号类型![%s]",g_pub_tx.ac_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
		}	

	return 0;
ErrExit:
	return 1;
}
pub_sub_spe_do1()
{
	int ret;

	/*** 检查是否有理财产品 ****/
	if( g_pub_tx.add_ind[0]=='2' )
	{
		ret=sql_count("mo_prdt_prot"," main_ac_id=%ld and sts='1' ", \
						g_pub_tx.ac_id,g_pub_tx.ac_id);
		if( ret!=0 )
		{
			sprintf(acErrMsg,"该户有理财或协定产品不能销户![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P182");
			goto ErrExit;
       }
	}

	if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='3' )
	{
		ret=pub_base_InsMo_opn_cls(g_pub_tx.name,"","2");
		if( ret )
		{
			sprintf(acErrMsg,"登记开销户登记簿出错!");
			WRITEMSG
			goto ErrExit;
		}
	}

	return 0;
ErrExit:
	return 1;
}
int chongzheng_cash()
{
	int ret;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;

	if(strcmp(g_pub_tx.ac_no,"10101")!=0)
		return(0);

	vtcp_log("%s%d 现金冲正更改柜员cash_mst \n",__FILE__,__LINE__);
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));

	if( !strlen(g_pub_tx.cur_no) ) /**默认为人民币**/
		strcpy( g_pub_tx.cur_no,"01" );

	/*** 根据交易机构号和柜员编码取现金账号ID ***/
	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	/* 检查币种是否存在，是否启用 */
	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查币种错！" );
		WRITEMSG
		goto ErrExit;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"现金账号未启用或状态不正常!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit;
	}
	vtcp_log("add_ind is [%s],amt is[%.2f]\n",g_pub_tx.add_ind,g_pub_tx.tx_amt1);
	/*	hst 的付出、收入标志和101科目的借贷相反,所以重新给hst的add_ind 赋值 */
	if( g_pub_tx.add_ind[0]=='0' ){
		cash_mst_c.bal=cash_mst_c.bal+g_pub_tx.tx_amt1;
		strcpy(cash_mst_hst_c.add_ind,"1");
	}
	else if( g_pub_tx.add_ind[0]=='1' ){
		cash_mst_c.bal=cash_mst_c.bal-g_pub_tx.tx_amt1;
		strcpy(cash_mst_hst_c.add_ind,"0");
	}
	else
	{
		sprintf(acErrMsg,"增减标志错误![%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( cash_mst_c.bal<=-0.005)
	{
		sprintf(acErrMsg,"现金不足!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P152");
		goto ErrExit;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Cash_mst_Clo_Upd();

	/*** 登记钱箱明细 ***/
	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,cash_mst_c.ac_no);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,"冲正补计");
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记钱箱明细出错![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}
	TRACE

OkExit:
	return 0;
ErrExit:
	return 1;
}
