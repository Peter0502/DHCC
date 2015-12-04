/***************************************************************
* 文 件 名:     pubf_acct_rol_reg.c
* 功能描述：
**标志用法：
*
* 作    者:
* 完成日期：    2003年11月27日
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
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "tx_dc_br_rel_c.h"
#include "mdm_unprt_hst_c.h"
#include "mo_city_c.h"
#include "mo_ibl_c.h"
#include "mo_ibl_ret_c.h"
#include "mo_invest_c.h"
#include "mo_righ_invest_c.h"
#include "mbfe_pub.h"
#include "mo_stok_rebuy_prot_c.h"
#include "cash_book_c.h"
#include "mo_discnt_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "mo_sg_cash_po_bill_c.h"
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"
#include "com_sys_parm_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "mo_bank_split_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_bail_rate_c.h"
#include "mo_po_co_c.h"
#include "protect_note_c.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include "accountreg_c.h"
#include "ass_mst_c.h"
#include "ass_mst_hst_c.h"
#include "mo_int_tax_hst_c.h"
#include "cashier_c.h"  /**YHBP**/
/**do_mode处理方式：1删2改
***reg_no登记簿编号
**/
int pub_acct_Rol_reg_sub( ptrace_log,do_mode,reg_no )
 struct trace_log_c *ptrace_log;
 char do_mode;
 int reg_no;
{
	int ret=0;

	sprintf( acErrMsg, "---------->开始处理登记记录!!!" );
	WRITEMSG

	if( do_mode=='0' )
	{
		sprintf( acErrMsg,"不处理" );
		WRITEMSG
		goto OkExit;
	}
	vtcp_log("%s,%d regno==%d",__FILE__,__LINE__,reg_no);
	switch( reg_no )
	{
		case 2:
			/*通知登记簿*/
			ret=Rol_mo_infrm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 3:
			/*挂失登记簿*/
			ret=Rol_mo_loss( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 4:
			/*久悬登记簿*/
			ret=Rol_mo_unchange( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 5:
			/*卡限额登记簿*/
			ret=Rol_card_consum_lmt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 6:
			/*贷款展期登记簿*/
			ret=Rol_ln_expan( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 7:
			/*划款协议*/
			ret=Rol_prdt_prot_parm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 8:
			/*划款协议登记簿*/
			ret=Rol_mo_prdt_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 9:
			/*划款协议账户对照表*/
			ret=Rol_mo_prot_ac_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 10:
			/*委托协议登记簿*/
			ret=Rol_mo_trust_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 11:
			/*透支还款计划表*/
			ret=Rol_od_pay_pln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 12:
			/*钱箱明细*/
			ret=Rol_cash_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 13:
			/*支票透支登记簿*/
			ret=Rol_mo_cheq_od( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 14:
			/*长短款登记簿*/
			ret=Rol_mo_rmny_smny( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 15:
			/*客户凭证台帐明细*/
			ret=Rol_note_cheq_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 16:
			/*客户凭证台帐*/
			ret=Rol_note_cheq_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 17:
			/*柜员凭证台账*/
			ret=Rol_note_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 18:
			/*凭证台帐明细*/
			ret=Rol_note_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 19:
			/*凭证最小号*/
			ret=Rol_note_min_no( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 20:
			/*分段积数*/
			ret=Rol_com_sect_acm( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 21:
			/*开销户登记薄*/
			TRACE
			ret=Rol_mo_opn_cls( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 22:
			/*捡拾登记薄*/
			ret=Rol_mo_coll( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 23:
			/*换证登记簿*/
			ret=Rol_mo_rpl_note( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 24:
			/*内部台账明细*/
			ret=Rol_in_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 25:
			/*贴现登记薄*/
			ret=Rol_mo_discnt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 26:
			/*贴现还款登记薄*/
			ret=Rol_mo_pay_discnt( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 27:
			/*拆借登记薄*/
			ret=Rol_mo_ibl( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 28:
			/*投资登记簿*/
			ret=Rol_mo_invest( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 29:
			/*权益性投资登记薄*/
			ret=Rol_mo_righ_invest( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 30:
			/*证券回购协议登记薄*/
			ret=Rol_mo_stok_rebuy_prot( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 31:
			/*保兑仓登记薄*/
			ret=Rol_mo_sg_cash_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 32:
			/*银行承兑汇票登记薄*/
			ret=Rol_mo_bank_acc_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 33:
			/*银行汇票查询查复登记*/
			ret=Rol_mo_po_qry( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 34:
			/*银行汇票登记薄*/
			ret=Rol_mo_bank_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 35:
			/*银行本票*/
			ret=Rol_mo_cashier( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 36:
			/*同城登记薄*/
			ret=Rol_mo_city( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 37:
			/*传真登记薄*/
			ret=Rol_mo_fax( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 38:
			/*传真银行代码表*/
			ret=Rol_mo_fax_code( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 39:
			/*委托清单*/
			ret=Rol_mo_trust_list( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 40:
			/*委托收款登记薄*/
			ret=Rol_mo_trust_pay( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 41:
			/*再贷款登记薄*/
			ret=Rol_mo_again_ln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 42:
			/*再贷款还款登记薄*/
			ret=Rol_mo_pay_again_ln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 43:
			/*（介质）积分主表*/
			ret=Rol_point_point_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 44:
			/*积分使用明细表*/
			ret=Rol_point_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 45:
			/*产品积分主表*/
			ret=Rol_prdt_point_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 46:
			/*产品积分使用明细表*/
			ret=Rol_prdt_point_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 47:
			/*考核总帐*/
			ret=Rol_ass_gl( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 48:
			/*考核主文件*/
			ret=Rol_ass_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 49:
			/*考核明细*/
			ret=Rol_ass_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 50:
			/*考核分段计息表*/
			ret=Rol_ass_subs_intst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 51:
			/*授权码管理表*/
			ret=Rol_auth_code( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 52:
			/*结息登记簿*/
			ret=Rol_intst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 53:
			/*活期存款明细*/
			ret=Rol_dd_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 54:
			/*贷款明细*/
			ret=Rol_ln_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 55:
			/*透支帐单文件*/
			ret=Rol_od_bill( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 56:
			/*透支明细*/
			ret=Rol_od_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 57:
			/*定期存款明细*/
			ret=Rol_td_mst_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 58:
			/*按揭贷款欠款表*/
			ret=Rol_ln_lo( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 59:
			/*贷款放款还款计划表*/
			ret=Rol_ln_pay_pln( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 60:
			/*贷款还款顺序表*/
			ret=Rol_ln_pay_seq( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 61:
			/*贷款登记簿*/
			ret=Rol_ln_reg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 62:
			/*介质与账户关系*/
			ret=Rol_mdm_ac_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 63:
			/*未打折明细表*/
			ret=Rol_mdm_unprt_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 64:
			/*账户变更历史*/
			ret=Rol_mo_ac_chg_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 65:
			/* 收费明细 */
			ret = Rol_com_chrg_hst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 66:
			/* 客户产品关系表 */
			ret = Rol_cif_prdt_rel( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 67:
			/*冻结登记簿*/
			ret=Rol_mo_hold( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 68:
			/*大额支付汇兑流水表*/
			ret=Rol_Mbfeghvsendjrn( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 69:
			/*大额支付来账清单*/
			ret=Rol_Mbfeghvrecvlist( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 70:
			/*现金收付登记簿*/
			ret=Rol_Cash_book( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 71: /*保兑仓开提货单登记薄*/
			ret=Rol_mo_sg_cash_po_bill( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 72: /*大额滞留提入*/
			ret=Rol_Mbfeghvsendlist( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 73: /*大额来账挂账业务提入*/
			ret=Rol_Mbfeghvrecvlist1( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 74: /*逾期汇票兑付*/
			ret=Rol_Mbfebillopnreg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 79: /*保证金注销*/
			ret=Rol_mo_sg_protect_note( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 80: /*子帐号存取消撤销*/
			ret=Rol_sub_dd_mst( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 81: /*承兑汇票拆分登记薄*/
			ret=Rol_mo_bank_split_po( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 82: /*质押存单录入*/
			ret=Rol_impa_dep_reg( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 83: /*保证金计息登记薄*/
			ret=Rol_mo_bail_rate( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 84: /*ＭＩＳ借据号对照表登记薄*/
			ret=Rol_mo_po_co( ptrace_log, do_mode );
			ERR_DEAL
			break;
		case 85:/* 大额登记簿 */
			/** add by LiuHuafeng 20061107 **/
			{
				char cCmtno[4];
				char cTxday[9];
				char cOrderno[9];
				char cWssrno[7];
				memset(cCmtno  ,0,sizeof(cCmtno)-1);
				memset(cTxday  ,0,sizeof(cTxday)-1);
				memset(cOrderno,0,sizeof(cOrderno)-1);
				memset(cWssrno ,0,sizeof(cWssrno)-1);
				sprintf(cWssrno,"%-6d",ptrace_log->trace_no);
				get_zd_data(DC_DATE,cTxday);
				zip_space(cWssrno);
				vtcp_log("%s,%d  wssrno=[%s]",__FILE__,__LINE__,cWssrno);
				ret=iFindSendRecord(cCmtno,cTxday,cOrderno,g_pub_tx.tx_br_no,cWssrno);
				if(ret)
				{
					set_zd_data("0130","该笔业务已经发送不可取消");
					strncpy(g_pub_tx.reply,"P088",4);
					ERR_DEAL
					goto ErrExit;
				}
			}
			/** end by LiuHuafeng 20061107 **/
			ret=Rol_Hv_zf(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P088",4);
			ERR_DEAL
			break;
		case 86:/* 汇票登记簿取消 */
			ret=Rol_Hv_Poinfo(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P085",4);
			ERR_DEAL;
			break;
		case 87:/* 小额包登记簿 */
			ret=Rol_Lv_Pkgreg(ptrace_log, do_mode );
			if(ret)
				strncpy(g_pub_tx.reply,"P085",4);
			ERR_DEAL;
			break;
		case 88:/* MBFE非支付类登记簿 */
			/**
			ret=Rol_mbfe_fzfreg(ptrace_log, do_mode );
			**/
			ERR_DEAL;
			break;
    case 89:/**大额错帐处理取消登记簿 **/
      ret=Rol_Hv_zf_Errdeal(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 90:/**小额错帐处理取消登记簿**/
      ret=Rol_Lv_pkgreg_Errdeal(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 91:/**销户交易取消分段利息税登记簿里的内容 */
      ret=Rol_Mo_int_tax_hst(ptrace_log, do_mode );
      if(ret)
        strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;
    case 92:/* 银行本票签发取消 add by xyy YHBP*/
      ret=Rol_Cashier(ptrace_log, do_mode );
      if(ret)
          strncpy(g_pub_tx.reply,"P085",4);
      ERR_DEAL;
      break;

    default:
	  break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/**
mo_hold            冻结登记簿
mo_infrm           通知登记簿
mo_loss            挂失登记簿
mo_unchange        久悬登记簿
card_consum_lmt    卡限额登记簿
ln_expand          贷款展期登记簿
prdt_prot_parm     划款协议
mo_prdt_prot       划款协议登记簿
mo_prot_ac_rel     划款协议账户对照表
mo_trust_prot      委托协议登记簿
od_pay_pln         透支还款计划表
cash_mst_hst       钱箱明细
mo_cheq_od         支票透支登记簿
mo_rmny_smny       长短款登记簿
note_cheq_hst      客户凭证台帐明细
note_cheq_mst      客户凭证台帐
note_mst           柜员凭证台账
note_mst_hst       凭证台帐明细
note_min_no        凭证最小号
com_sect_acm       分段积数
mo_opn_cls         开销户登记薄
mo_coll            捡拾登记薄
mo_rpl_note        换证登记簿
com_tel_connect    柜员交接登记薄
in_mst_hst         内部台账明细
mo_discnt          贴现登记薄
mo_pay_discnt      贴现还款登记薄
mo_ibl             拆借登记薄
mo_invest          投资登记簿
mo_righ_invest     权益性投资登记薄
mo_stok_rebuy_prot 证券回购协议登记薄
mo_sg_cash_po      保兑仓登记薄
mo_bank_acc_po     银行承兑汇票登记薄
mo_po_qry          银行汇票查询查复登记
mo_bank_po         银行汇票登记薄
mo_cashier         银行本票
mo_city            同城登记薄
mo_fax             传真登记薄
mo_fax_code        传真银行代码表
mo_trust_list      委托清单
mo_trust_pay       委托收款登记薄
mo_again_ln        再贷款登记薄
mo_pay_again_ln    再贷款还款登记薄
point_point_mst    （介质）积分主表
point_mst_hst      积分使用明细表：
prdt_point_mst     产品积分主表
prdt_point_hst     产品积分使用明细表：
ass_gl             考核总帐
ass_mst            考核主文件
ass_mst_hst        考核明细
ass_subs_intst     考核分段计息表
auth_code          授权码管理表
intst_hst          结息登记簿
dd_mst             活期存款主文件
dd_mst_hst         活期存款明细
ln_mst             贷款主文件
ln_mst_hst         贷款明细
od_mst             透支主文件
od_bill            透支帐单文件
od_mst_hst         透支明细
td_mst             定期存款主文件
td_mst_hst         定期存款明细
ln_lo              按揭贷款欠款表
ln_pay_pln         贷款放款还款计划表
ln_pay_seq         贷款还款顺序表
ln_reg             贷款登记簿
mdm_ac_rel         介质与账户关系
mdm_prdt_rel       介质产品关系表
mdm_unprt_hst      未打折明细表
mo_ac_chg_hst      账户变更历史
com_chrg_hst	   收费明细
cash_book		   现金收付登记簿
protect_note       保证金帐户
*/
/*通知登记簿*/
static int Rol_mo_infrm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*挂失登记簿*/
static int Rol_mo_loss( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*久悬登记簿*/
static int Rol_mo_unchange( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*卡限额登记簿*/
static int Rol_card_consum_lmt( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贷款展期登记簿*/
static int Rol_ln_expan( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*划款协议*/
static int Rol_prdt_prot_parm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*划款协议登记簿*/
static int Rol_mo_prdt_prot( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*划款协议账户对照表*/
static int Rol_mo_prot_ac_rel( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*委托协议登记簿*/
static int Rol_mo_trust_prot( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*透支还款计划表*/
static int Rol_od_pay_pln( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*钱箱明细*/
static int Rol_cash_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*支票透支登记簿*/
static int Rol_mo_cheq_od( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*长短款登记簿*/
static int Rol_mo_rmny_smny( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '1':
			ret=sql_execute( "delete from mo_rmny_smny where tx_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*客户凭证台帐明细*/
static int Rol_note_cheq_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*客户凭证台帐*/
static int Rol_note_cheq_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*柜员凭证台账*/
static int Rol_note_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*凭证台帐明细*/
static int Rol_note_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*凭证最小号*/
static int Rol_note_min_no( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
static int Rol_com_sect_acm( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*开销户登记薄*/
static int Rol_mo_opn_cls( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	sprintf(acErrMsg,"进入开销户登记簿[%c]",do_mode);
	WRITEMSG
	switch(do_mode)
	{
		case '1':  /**撤销开户的记录**/
			ret=sql_execute("delete from mo_opn_cls where ac_no='%s' and ac_seqn=%d and wrk_sts='1' ",
				ptrace_log->ac_no,ptrace_log->ac_seqn );
			ERR_DEAL
			break;
		case '2':  /**撤销销户的记录**/
			ret=sql_execute("delete from mo_opn_cls where ac_no='%s' and ac_seqn=%d and wrk_sts='2' ",
				ptrace_log->ac_no,ptrace_log->ac_seqn );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*捡拾登记薄*/
static int Rol_mo_coll( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '0':
			sprintf( acErrMsg,"不处理" );
			WRITEMSG
			break;
		default:
			ret=sql_execute( "delete from mo_coll where tx_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*换证登记簿*/
static int Rol_mo_rpl_note( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	switch(do_mode)
	{
		case '1':
			ret=sql_execute( "delete from mo_rpl_note where rpl_date=%d and trace_no=%d",
				ptrace_log->tx_date,ptrace_log->trace_no );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*内部台账明细*/
static int Rol_in_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贴现登记薄*/
static int Rol_mo_discnt( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mo_discnt_c	Tmp_mo_discnt;
	int	ret=0;
	int i=0;

	switch(do_mode)
	{
		case '1':		/** 录入撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"pact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"贴现登记薄非登记状态，请先撤销审批记账交易![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			/***
			if( Tmp_mo_discnt.pn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"非今日录入的贴现交易，不能撤销![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			****/
			ret=sql_execute( "delete from mo_discnt where pact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case '2':		/** 审批撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='2' )
			{
				sprintf(acErrMsg,"贴现登记薄非已经审批状态，请先撤销记账交易![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B279" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '3':		/** 贴现记账撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='3' )
			{
				sprintf(acErrMsg,"贴现登记薄非记账状态，不允许撤销![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"2");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '4':		/** 贴现还款撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			vtcp_log("%s %d 日志中的金额===[%f]",__FILE__,__LINE__,ptrace_log->amt);
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '5':		/** 转贴现记账撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='3' )
			{
				sprintf(acErrMsg,"贴现登记薄非记账状态，不允许撤销![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case '6':		/** 买断转贴现转出录入撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"note_type='%s' and pnote_no='%s' ",ptrace_log->note_type,ptrace_log->note_no);
			ERR_DEAL
			for( i=1;i<3;i++ )
			{
				ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
				ERR_DEAL
				pub_base_strpack(Tmp_mo_discnt.pact_no);
				pub_base_strpack(ptrace_log->ac_no);
				if( !strcmp(Tmp_mo_discnt.pact_no,ptrace_log->ac_no) )	/*** 原转入或贴现的记录 ***/
				{
					if( Tmp_mo_discnt.sts[0]!='*' )
					{
						sprintf(acErrMsg,"贴现登记薄非记账状态，不允许撤销![%s]",Tmp_mo_discnt.sts);
						WRITEMSG
						strcpy( g_pub_tx.reply,"B280" );
						goto ErrExit;
					}
					strcpy(Tmp_mo_discnt.sts,"3");
					ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
					ERR_DEAL
				}
				if( strlen(Tmp_mo_discnt.pact_no)==0 )
				{
					ret=Mo_discnt_Del_Upd(Tmp_mo_discnt,g_pub_tx.reply);
					ERR_DEAL
				}
			}
			Mo_discnt_Clo_Upd();
			break;
		case '7':		/** 买断转贴现转出记账撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='*' )
			{
				sprintf(acErrMsg,"贴现登记薄非记账状态，不允许撤销![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			Mo_discnt_Clo_Upd();
			break;
		case '8':		/** 回购式转贴现转出录入撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"tpact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"贴现登记薄非登记状态，请先撤销审批记账交易![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			if( Tmp_mo_discnt.tpn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"非今日录入的贴现交易，不能撤销![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			ret=sql_execute( "delete from mo_discnt where tpact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case '9':		/** 转贴现转出到期回购还款撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply," tpact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case 'A':		/** 回购式再贴现信息录入 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Sel(g_pub_tx.reply,&Tmp_mo_discnt,"rpact_no='%s' and sts!='*' ",ptrace_log->ac_no);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='1' )
			{
				sprintf(acErrMsg,"贴现登记薄非登记状态，请先撤销审批记账交易![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B277" );
				goto ErrExit;
			}
			if( Tmp_mo_discnt.rtpn_date!=g_pub_tx.tx_date )
			{
				sprintf(acErrMsg,"非今日录入的贴现交易，不能撤销![%d]",Tmp_mo_discnt.pn_date);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B278" );
				goto ErrExit;
			}
			ret=sql_execute( "delete from mo_discnt where rpact_no='%s' ",ptrace_log->ac_no );
			ERR_DEAL
			break;
		case 'B':		/** 再贴现回购撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply," rpact_no='%s' and sts='*' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			strcpy(Tmp_mo_discnt.sts,"3");
			Tmp_mo_discnt.tranx_bal+=ptrace_log->amt;
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		case 'C':		/** 买断式再贴现记账撤销 **/
			memset(&Tmp_mo_discnt,0x00,sizeof(struct mo_discnt_c));
		/*
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld and sts!='*' ",ptrace_log->tx_date,ptrace_log->trace_no);
		*/
			ret=Mo_discnt_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_discnt_Fet_Upd(&Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			if( Tmp_mo_discnt.sts[0]!='*' )
			{
				sprintf(acErrMsg,"贴现登记薄非销户状态，不允许撤销![%s]",Tmp_mo_discnt.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B280" );
				goto ErrExit;
			}
			strcpy(Tmp_mo_discnt.sts,"1");
			ret=Mo_discnt_Upd_Upd(Tmp_mo_discnt,g_pub_tx.reply);
			ERR_DEAL
			WRITEMSG
			Mo_discnt_Clo_Upd();
			WRITEMSG
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贴现还款登记薄*/
static int Rol_mo_pay_discnt( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*拆借登记薄*/
static int Rol_mo_ibl( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;
	 struct mo_ibl_c sMo_ibl;
	 struct mo_ibl_ret_c sMo_ibl_ret;

	 switch(do_mode)
	 {
		case '1': /* 同业拆入/拆出记账撤销 */
			memset(&sMo_ibl, 0x0, sizeof(struct mo_ibl_c));
			ret = Mo_ibl_Sel(g_pub_tx.reply, &sMo_ibl, "ac_id=%ld",
				ptrace_log->ac_id);
			ERR_DEAL

			if (pub_base_CompDblVal(sMo_ibl.amt, sMo_ibl.no_pay_bal) != 0)
			{
				sprintf(acErrMsg, "该笔拆入已经部分还款!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"S057");
				goto ErrExit;
			}

			if (sMo_ibl.sign[0] == '1')
			{
				ret = sql_count("mo_ibl_ret", "pact_no='%s'", sMo_ibl.pact_no);
				if (ret < 0)
				{
					sprintf(acErrMsg, "sql_count error !! [%d]", ret);
					WRITEMSG
					goto ErrExit;
				}
				if (ret > 0)
				{
					sprintf(acErrMsg, "资金营业部已经录入还款信息!!", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "S058");
					goto ErrExit;
				}
			}

			ret=sql_execute("update mo_ibl set sts='1',ac_id=0, opn_date=0 \
				where ac_id=%ld", ptrace_log->ac_id );
			ERR_DEAL
			break;
		case '2': /* 同业拆入还款 */
			memset(&sMo_ibl, 0x0, sizeof(struct mo_ibl_c));
			memset(&sMo_ibl_ret, 0x0, sizeof(struct mo_ibl_ret_c));

			ret = Mo_ibl_Sel(g_pub_tx.reply, &sMo_ibl, "ac_id=%ld",
				ptrace_log->ac_id);
			ERR_DEAL

			ret = Mo_ibl_ret_Dec_Upd(g_pub_tx.reply, "pact_no='%s' and \
				ret_amt=%lf and sts='2'", sMo_ibl.pact_no, ptrace_log->amt);
			ERR_DEAL

			ret = Mo_ibl_ret_Fet_Upd(&sMo_ibl_ret, g_pub_tx.reply);
			ERR_DEAL

			strcpy(sMo_ibl_ret.sts, "1");

			ret = Mo_ibl_ret_Upd_Upd(sMo_ibl_ret, g_pub_tx.reply);
			ERR_DEAL

			Mo_ibl_ret_Clo_Upd();

			ret=sql_execute("update mo_ibl set sts='2', \
				no_pay_bal=no_pay_bal+%lf where ac_id=%ld",
				ptrace_log->amt, ptrace_log->ac_id);
			ERR_DEAL
			break;
		case '3': /* 拆出回收 */
			ret=sql_execute("update mo_ibl set sts='2', \
				no_pay_bal=no_pay_bal+%lf where ac_id=%ld",
				ptrace_log->amt, ptrace_log->ac_id);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

/*投资登记簿*/
static int Rol_mo_invest( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_invest_c	sMo_invest;
	memset(&sMo_invest,0x00,sizeof(struct mo_invest_c));

	if( do_mode=='1' )		/*** 删除 ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** 修改 ***/
	{
		if( !strncmp("4602",ptrace_log->sub_tx_code,4) )	/** 债券投资记账 **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )	goto OkExit;
			ERR_DEAL
			if( pub_base_CompDblVal(sMo_invest.sol_amt,0.00)!=0
				|| sMo_invest.sts[0]!='2' )
			{
				vtcp_log("sts=[%s],[%d]",sMo_invest.sts,pub_base_CompDblVal(sMo_invest.sol_amt,0.00));
				sprintf(acErrMsg,"该笔债券投资买入的债券已经出售,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			strcpy(sMo_invest.sts,"1");
			sMo_invest.ac_id=0;
			sMo_invest.overflow_ac_id=0;
			sMo_invest.abate_ac_id=0;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->tx_code,"4603") )	/** 债券收息 **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.pre_intst = sMo_invest.pre_intst + ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"4604") )	/** 债券出售 **/
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto AAAAA;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			strcpy(sMo_invest.sts,"2");
			sMo_invest.par_bal = sMo_invest.par_bal + ptrace_log->amt;
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
			goto OkExit;

AAAAA:		ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"abate_ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto BBBBB;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.abate_amt = sMo_invest.abate_amt + ptrace_log->amt;
			strcpy(sMo_invest.sts,"2");
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
			goto OkExit;

BBBBB:		ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"overflow_ac_id=%ld ",ptrace_log->ac_id);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.overflow_amt = sMo_invest.overflow_amt + ptrace_log->amt;
			strcpy(sMo_invest.sts,"2");
			sMo_invest.trace_no = ptrace_log->trace_no;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"A016") && !strcmp(ptrace_log->tx_code,"4604") )
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.sol_amt = sMo_invest.sol_amt - ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
		if( !strcmp(ptrace_log->sub_tx_code,"A017") && !strcmp(ptrace_log->tx_code,"4604") )
		{
			ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
			ERR_DEAL
			ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_invest_Clo_Upd();
				goto OkExit;
			}
			ERR_DEAL
			if( sMo_invest.sts[0]!='2' && sMo_invest.sts[0]!='*' )
			{
				vtcp_log("sts=[%s]",sMo_invest.sts);
				sprintf(acErrMsg,"该笔债券投资台账状态非正常,无法撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P454" );
				goto ErrExit;
			}
			sMo_invest.pre_intst = sMo_invest.pre_intst + ptrace_log->amt;
			ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
			ERR_DEAL
			Mo_invest_Clo_Upd();
		}
	}
	else
	{
		sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S054" );
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*权益性投资登记薄*/
static int Rol_mo_righ_invest( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_righ_invest_c	sMo_righ_invest;
	memset(&sMo_righ_invest,0x00,sizeof(struct mo_righ_invest_c));

	if( do_mode=='1' )		/*** 删除 ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** 修改 ** 权益性投资记账 ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( pub_base_CompDblVal(sMo_righ_invest.amt,ptrace_log->amt)!=0 || sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s],[%lf][%lf]",sMo_righ_invest.sts,sMo_righ_invest.amt,ptrace_log->amt);
			sprintf(acErrMsg,"该笔股权投资已经出售,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		strcpy(sMo_righ_invest.sts,"1");
		sMo_righ_invest.ac_id=0;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='3' && !strcmp(ptrace_log->tx_code,"4607") )	/*** 修改 ** 权益性投资记账 ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"该笔股权投资已经出售,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.incm_bal=sMo_righ_invest.incm_bal-ptrace_log->amt;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='3' && !strcmp(ptrace_log->tx_code,"4608") )	/*** 修改 ** 权益性投资收入 ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"wrk_date=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"该笔股权投资已经出售,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.incm_bal=sMo_righ_invest.incm_bal-ptrace_log->amt;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
	else if( do_mode=='4' && !strcmp(ptrace_log->sub_tx_code,"4608") )	/*** 修改 ** 权益性投资出售 ***/
	{
		ret=Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"ac_id=%ld ",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if( sMo_righ_invest.sts[0]!='2' )
		{
			vtcp_log("sts=[%s]",sMo_righ_invest.sts);
			sprintf(acErrMsg,"该笔股权投资已经出售,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P469" );
			goto ErrExit;
		}
		sMo_righ_invest.amt=sMo_righ_invest.amt+ptrace_log->amt;
		sMo_righ_invest.wrk_date=ptrace_log->tx_date;
		sMo_righ_invest.trace_no=ptrace_log->trace_no;
		ret=Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
		ERR_DEAL
		Mo_righ_invest_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*证券回购协议登记薄*/
static int Rol_mo_stok_rebuy_prot( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_stok_rebuy_prot_c	sMo_stok_rebuy_prot;
	memset(&sMo_stok_rebuy_prot,0x00,sizeof(struct mo_stok_rebuy_prot_c));

	if( do_mode=='1' )		/*** 删除 ***/
	{
		;
	}
	else if( do_mode=='2' )	/*** 修改 ** 证券回购借出借入记账 ***/
	{
		ret=Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if(pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,ptrace_log->amt)!=0||sMo_stok_rebuy_prot.sts[0]!='2' )
		{
			vtcp_log("sts=[%s],[%lf][%lf]",sMo_stok_rebuy_prot.sts,sMo_stok_rebuy_prot.bal,ptrace_log->amt);
			sprintf(acErrMsg,"该笔证券回购台帐状态不正确,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P470" );
			goto ErrExit;
		}
		strcpy(sMo_stok_rebuy_prot.sts,"1");
		sMo_stok_rebuy_prot.ac_id=0;
		ret=Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
		ERR_DEAL
		Mo_stok_rebuy_prot_Clo_Upd();
	}
	else if( do_mode=='3' )	/*** 修改 ** 借出还款记账 ***/
	{
		ret=Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",ptrace_log->ac_id);
		ERR_DEAL
		ret=Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
		if( ret==100 )	goto OkExit;
		ERR_DEAL
		if(sMo_stok_rebuy_prot.sts[0]!='*' && sMo_stok_rebuy_prot.sts[0]!='3' )
		/* 3 的状态是逾期未收回 */
		{
			vtcp_log("sts=[%s]",sMo_stok_rebuy_prot.sts);
			sprintf(acErrMsg,"该笔证券回购台帐状态不正确,无法撤销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P470" );
			goto ErrExit;
		}
		strcpy(sMo_stok_rebuy_prot.sts,"2");
		sMo_stok_rebuy_prot.bal=sMo_stok_rebuy_prot.bal+ptrace_log->amt;
		ret=Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
		ERR_DEAL
		Mo_stok_rebuy_prot_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*保兑仓登记薄*/
static int Rol_mo_sg_cash_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_sg_cash_po_c	T_mo_sg_cash_po;
	memset(&T_mo_sg_cash_po,0x00,sizeof(struct mo_sg_cash_po_c));

	switch( do_mode )
	{
		case '1' :	/*** 保证金存款撤销 ***/
			ret=Mo_sg_cash_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_Fet_Upd(&T_mo_sg_cash_po,g_pub_tx.reply);
			if( ret==100 )
			{
				Mo_sg_cash_po_Clo_Upd();
				break;
			}
			ERR_DEAL

			if( ptrace_log->add_ind[0]=='1' )	/** 缴存保证金 **/
			{
				ret=Mo_sg_cash_po_Del_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else if( ptrace_log->note_type[0]=='4' )	/** 保兑仓 ***/
			{
				T_mo_sg_cash_po.safeg_sum=T_mo_sg_cash_po.safeg_sum-ptrace_log->amt;
				ret=Mo_sg_cash_po_Upd_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_sg_cash_po_Clo_Upd();
			break;
		case '2' :	/*** 开提货单撤销 ***/
			ret=Mo_sg_cash_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_Fet_Upd(&T_mo_sg_cash_po,g_pub_tx.reply);
			ERR_DEAL
			T_mo_sg_cash_po.pickup_sum=T_mo_sg_cash_po.pickup_sum-ptrace_log->amt;
			if( pub_base_CompDblVal(T_mo_sg_cash_po.pickup_sum,0.00)<0 )
			{
				sprintf(acErrMsg,"提货单登记薄撤销后金额为负，不能撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"B284" );
				goto ErrExit;
			}
			ret=Mo_sg_cash_po_Upd_Upd(T_mo_sg_cash_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_sg_cash_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*银行承兑汇票登记薄*/
static int Rol_mo_bank_acc_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_acc_po_c	T_mo_bank_acc_po;
	memset(&T_mo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));

	switch( do_mode )
	{
		case '1' :	/*** 保证金存款撤销 ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Debug(&T_mo_bank_acc_po);
			if( ptrace_log->add_ind[0]=='1' )	/** 缴存保证金 **/
			{
				vtcp_log("kkkkkkkkkkkkkkkkkkkkkkk\n");
				if( T_mo_bank_acc_po.acc_po_sts[0]!='A' )
				{
					sprintf(acErrMsg,"承兑汇票协议登记薄状态不对[%s]",T_mo_bank_acc_po.acc_po_sts);
					WRITEMSG
					strcpy( g_pub_tx.reply,"B281" );
					goto ErrExit;
				}
				ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else 								/** 补缴保证金 **/
			{
				T_mo_bank_acc_po.bail_amt=T_mo_bank_acc_po.bail_amt-ptrace_log->amt;
				ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_bank_acc_po_Clo_Upd();
			break;
		case '2' :	/*** 承兑协议登记撤销 ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='0' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态非审批状态[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			if( ptrace_log->add_ind[0]=='1' )
			{
				ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			else
			{
				strcpy(T_mo_bank_acc_po.acc_po_sts,"A");
				ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
				ERR_DEAL
			}
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '3' :	/*** 承兑协议审批撤销 ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态非审批状态[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_acc_po.acc_po_sts,"0");
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '4' :	/*** 承兑协议签发撤销 ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and acc_po_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			strcpy(T_mo_bank_acc_po.acc_po_sts,"1");
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		case '5' :	/*** 拆分票据管理撤销 ***/
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no='' and bill_ac_no='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_acc_po.acc_po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态不对[%s]",T_mo_bank_acc_po.acc_po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_acc_po_Del_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			break;
		case '6' :	/*** 保证金支取交易 ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' and acc_po_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_acc_po_Fet_Upd(&T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			T_mo_bank_acc_po.bail_amt=T_mo_bank_acc_po.bail_amt+ptrace_log->amt;
			ret=Mo_bank_acc_po_Upd_Upd(T_mo_bank_acc_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_acc_po_Clo_Upd();
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*银行汇票查询查复登记*/
static int Rol_mo_po_qry( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*银行汇票登记薄*/
static int Rol_mo_bank_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_po_c	T_mo_bank_po;
	memset(&T_mo_bank_po,0x00,sizeof(struct mo_bank_po_c));

	switch( do_mode )
	{
		case '1' :	/*** 承兑汇票签发撤销 ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态错误[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_po_Del_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			/*** 汇票状态该为作废状态 ***/
			break;
		case '2' :	/*** 承兑汇票托收撤销 ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='4' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态非托收状态[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"1");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		case '3' :	/*** 承兑汇票核销撤销 ***/
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='2' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态非核销状态[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"4");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		case '4' :	/*** 承兑汇票退票撤销 ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='3' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态非退票状态[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B302" );
				goto ErrExit;
			}
			strcpy(T_mo_bank_po.po_sts,"1");
			ret=Mo_bank_po_Upd_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			ret=Mo_bank_po_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and po_no='%s' ",ptrace_log->ac_no,ptrace_log->brf);
			ERR_DEAL
			ret=Mo_bank_po_Fet_Upd(&T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_po.po_sts[0]!='1' )
			{
				sprintf(acErrMsg,"承兑汇票协议登记薄状态错误[%s]",T_mo_bank_po.po_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B281" );
				goto ErrExit;
			}
			ret=Mo_bank_po_Del_Upd(T_mo_bank_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*银行本票*/
static int Rol_mo_cashier( struct trace_log_c *ptrace_log, char do_mode )
{
	pub_base_strpack(ptrace_log->ac_no);
	switch( do_mode )
	{
		case '1':	/*本票信息录入撤销*/
			g_reply_int=sql_execute("delete from mo_cashier where pact_no='%s' and cashier_sts='0'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		case '2':	/*本票签发撤销*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='0',cashier_no=NULL,sign_date=0,sign_trace_no=0,sign_tel=NULL,sign_br_no=NULL where cashier_no='%s'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		case '3':	/*本票结清撤销*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='1',use_ind='0',cash_amt=0,rem_amt=0,pay_ac_no=NULL,rec_ac_no=NULL,rem_ac_no=NULL,can_date=0,can_trace_no=0 where cashier_no='%s'",
				ptrace_log->note_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			if( ptrace_log->ct_ind[0] == '1' )    /*同城提入方式结清,需撤销同城登记簿*/
			{
				g_reply_int=sql_execute( "update mo_city set sts='C' where payer_ac_no='%s' and bill_amt='%lf' and sts='O'",
					ptrace_log->ac_no,ptrace_log->amt );
				if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			}
			break;
		case '4':	/*本票退回撤销*/
			g_reply_int=sql_execute("update mo_cashier set cashier_sts='1',use_ind='0',cash_amt=0,rem_amt=0,pay_ac_no=NULL,rec_ac_no=NULL,rem_ac_no=NULL,can_date=0,can_trace_no=0 where cashier_no='%s'",
				ptrace_log->ac_no);
			if(g_reply_int){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
			break;
		default :
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*同城登记薄 add by liuxuan 2006-10-12 */
static int Rol_mo_city( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct accountreg_c accountreg;
/*
	if(do_mode!='1'){
		vtcp_log("旧系统以前多个交易对同城登记簿处理,现在只有同城交易处理\n",__FILE__,__LINE__);
		return(0);
	}
*/
	if(memcmp(ptrace_log->sub_tx_code,"B901",4)!=0){
		vtcp_log("旧系统以前多个交易对同城登记簿处理,现在只有同城交易处理\n",__FILE__,__LINE__);
		return(0);
	}
		vtcp_log("进入对同城登记簿处理\n",__FILE__,__LINE__);
	memset(&accountreg, 0x0, sizeof(struct accountreg_c));
	vtcp_log("票据信息tx_date=%ld and trace_no=%ld and br_no=%s accountreg.stat[%s]",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat);
/**
	ret = Accountreg_Sel(g_pub_tx.reply, &accountreg, "tx_date=%ld and \
		trace_no=%ld and br_no='%s'",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no);
***/
	ret = Accountreg_Sel(g_pub_tx.reply, &accountreg, "tx_date=%ld and \
		trace_no=%ld ",ptrace_log->tx_date, ptrace_log->trace_no);
	vtcp_log("票据信息tx_date=%ld and trace_no=%ld and br_no=%s accountreg.stat[%s]",ptrace_log->tx_date, ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "同城登记簿中没有登记该笔业务!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "D122");
				goto ErrExit;
			}
			if (accountreg.stat[0] != '2' && accountreg.stat[0] != '3')
			{
				sprintf(acErrMsg, "该笔票据trace_no[%ld],br_no[%s]不是记帐或复核状态[%s]不能撤销!!,ptrace_log->trace_no,ptrace_log->tx_br_no,accountreg.stat");
				WRITEMSG
				strcpy(g_pub_tx.reply, "P004");
				goto ErrExit;
			}
			ret = sql_execute("update accountreg set stat='9' where trace_no = %ld and \
				tx_date=%ld ", \
				ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
OkExit:
	return 0;
ErrExit:
	return 1;
}
/* 以前的同城现在不用了所以改了个名 _ori */
static int Rol_mo_city_ori( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_city_c sMo_city;

	switch(do_mode)
	{
		case '1': /* 提出取款撤销 D096*/
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_date=%ld and \
				tx_no=%ld",ptrace_log->tx_date,ptrace_log->trace_no);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据![%ld]",ptrace_log->trace_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
				break;
			}
			if (sMo_city.sts[0] != '0')
			{
				sprintf(acErrMsg, "该笔票据状态不是提出登记状态不能撤销!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O244");
				goto ErrExit;
			}
			ret=sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '2': /* 手工录入撤销 D211*/
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_date=%ld and \
				tx_no=%ld",ptrace_log->tx_date, ptrace_log->trace_no);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}
			if (sMo_city.sts[0] != '1')
			{
				sprintf(acErrMsg, "该笔票据不是录入状态不能撤销!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O245");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='0',tx_no=%ld, \
				payee_ac_no='', bill_type='' where tx_no = %ld and \
				tx_date=%ld ", \
				ptrace_log->ac_id,ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '3': /* 手工提出打印撤销 D212 */
			ret = sql_count("mo_city", "bat_no=%ld and sts='3' and \
				in_out_ind='1' and trans_mode='1'", ptrace_log->ac_seqn);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "该批次号票据中心已经核销!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D230");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='1', pr_cnt=0 ,\
				bat_no=0 where bat_no=%ld and sts='2' and in_out_ind='1' and \
				trans_mode='1'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '4': /* 手工提出核销撤销 */
			/* 该交易不能撤销 */
			strcpy(g_pub_tx.reply, "S053");
			goto ErrExit;
			ret = sql_execute("update mo_city set sts='2'  where \
				bat_no=%ld and sts='3'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '5': /* 自动提出录入撤销 */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}
			if (sMo_city.sts[0] != '6')
			{
				sprintf(acErrMsg, "该笔票据已经打印不能撤销!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O246");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='0',tx_no=%ld, \
				payee_ac_no='', bill_type='',bat_no=0 where tx_no = %ld \
				and tx_date=%ld ",
				ptrace_log->ac_seqn,ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '6': /* 手工/自动提入录入撤销 */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.sts[0] != 'A' && sMo_city.sts[0] != 'H')
			{
				sprintf(acErrMsg, "该笔票据不是录入状态不能撤销!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O244");
				goto ErrExit;
			}

			ret = sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL
			break;
		case '7': /* 手工提入打印撤销 */
			ret = sql_count("mo_city", "bat_no>=%ld and bat_no<=%ld \
				and sts in ('C', 'O','Q') and in_out_ind='2' and \
				trans_mode='1'", ptrace_log->ac_seqn, ptrace_log->svc_ind);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "此次打印的票据有状态不是'B'的不能撤销!!\
					[%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "S053");
				goto ErrExit;
			}

			ret = sql_execute("update mo_city set sts='A', pr_cnt=0 ,\
				bat_no=0 where bat_no>=%ld  and bat_no <= %ld and \
				sts='B' and in_out_ind='2' and trans_mode='1'",
				ptrace_log->ac_seqn, ptrace_log->svc_ind);
			ERR_DEAL
			break;
		case '8': /* 手工提入核销 */
			ret = sql_count("mo_city", "trans_mode='1' and in_out_ind='2' \
				and bat_no=%ld and sts in ('O','Q')",
				ptrace_log->ac_seqn);
			if (ret < 0)
			{
				sprintf(acErrMsg, "统计错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			if (ret > 0)
			{
				sprintf(acErrMsg, "此次核销的票据有已经记帐或退票的不能撤销\
					[%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "S053");
				goto ErrExit;
			}
			ret = sql_execute("update mo_city set sts='B'  where \
				in_out_ind='2' and trans_mode='1' and bat_no=%ld and \
				sts='C'", ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		case '9': /* 网点提入记账 D251 D256(借方) */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				sts='O' and tx_date=%8.0lf", ptrace_log->ac_id,
				ptrace_log->amt);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询交易记录错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.trans_mode[0] == '1')
			{
				ret = sql_execute("update mo_city set sts='C' where \
					tx_no=%ld and sts='O' and tx_date=%8.0f ", \
					ptrace_log->ac_id,ptrace_log->amt);
				ERR_DEAL
			}
			else
			{
				ret = sql_execute("update mo_city set sts='I' where \
					tx_no=%ld and sts='O' and tx_date=%8.0f ", \
					ptrace_log->ac_id,ptrace_log->amt);
				ERR_DEAL
			}
			break;
		case 'A': /* 网点提入退票 */
			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				tx_date=%ld ", ptrace_log->trace_no, ptrace_log->tx_date);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询交易记录错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.sts[0] != '1' && sMo_city.sts[0] != '6')
			{
				sprintf(acErrMsg, "该退票不是录入状态不能撤销!! [%s]",
					sMo_city.sts);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D231");
				goto ErrExit;
			}

			ret  = sql_execute("delete from mo_city where tx_no=%ld and \
				tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
			ERR_DEAL

			memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
			ret = Mo_city_Sel(g_pub_tx.reply, &sMo_city, "tx_no=%ld and \
				sts='Q' and tx_date=%8.0lf ", ptrace_log->ac_id, \
				ptrace_log->amt);
			if (ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询交易记录错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "O242");
				goto ErrExit;
			}
			else if (ret == 100)
			{
				sprintf(acErrMsg, "没有该类票据!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "O243");
				goto ErrExit;
			}

			if (sMo_city.trans_mode[0] == '1')
			{
				ret = sql_execute("update mo_city set sts='C' where \
					tx_no=%ld and sts='Q' and tx_date=%8.0lf",
					ptrace_log->ac_id, ptrace_log->amt);
				ERR_DEAL
			}
			else
			{
				ret = sql_execute("update mo_city set sts='I' where \
					tx_no=%ld and sts='Q' and tx_date=%8.0lf",
					ptrace_log->ac_id, ptrace_log->amt);
				ERR_DEAL
			}
			break;
		case 'B':
			break;
		case 'C':	/*** 投资业务撤销 *** 无法判断是否清算 ***
					**** 不能删除只能修改状态 ***************/
			if( !strncmp(ptrace_log->tx_code,"46",2)
				|| !strncmp(ptrace_log->tx_code,"47",2) )
			{
				memset(&sMo_city, 0x0, sizeof(struct mo_city_c));
				ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and tx_no=%ld",
					ptrace_log->tx_date,ptrace_log->trace_no);
				if (ret)
				{
					sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O242");
					goto ErrExit;
				}
				ret = Mo_city_Fet_Upd(&sMo_city,g_pub_tx.reply);
				if (ret == 100)		break;
				if (ret)
				{
					sprintf(acErrMsg, "查询同城登记簿错误!! [%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O243");
					goto ErrExit;
				}
				strcpy(sMo_city.sts,"*");
				ret = Mo_city_Upd_Upd(sMo_city,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg, "没有该类票据![%ld]",ptrace_log->trace_no);
					WRITEMSG
					strcpy(g_pub_tx.reply, "O243");
					goto ErrExit;
					break;
				}
				Mo_city_Clo_Upd();
			}
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*传真登记薄*/
static int Rol_mo_fax( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*传真银行代码表*/
static int Rol_mo_fax_code( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*委托清单*/
static int Rol_mo_trust_list( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*委托收款登记薄*/
static int Rol_mo_trust_pay( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*再贷款登记薄*/
static int Rol_mo_again_ln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_again_ln_c	T_mo_again_ln;
	memset(&T_mo_again_ln,0x00,sizeof(struct mo_again_ln_c));

	switch( do_mode )
	{
		case '1' :	/*** 再贷款录入撤销 ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='0' )
			{
				sprintf(acErrMsg,"该笔再贷款非录入状态，请先撤销记账交易!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B285" );
				goto ErrExit;
			}
			ret=Mo_again_ln_Del_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '2' :	/*** 再贷款放款(记帐)撤销 ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' )
			{
				sprintf(acErrMsg,"该笔再贷款状态不正确!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"0");
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '3' :	/*** 再贷款还款录入撤销 ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' )
			{
				sprintf(acErrMsg,"该笔再贷款状态不正确!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"1");
			T_mo_again_ln.sum_amt=T_mo_again_ln.sum_amt-ptrace_log->amt;
			if( T_mo_again_ln.sum_amt<0.005 )
			{
				sprintf(acErrMsg,"再贷款登记薄撤销后金额为负，不能撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"B287" );
				goto ErrExit;
			}
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		case '4' :	/*** 再贷款还款记账撤销 ***/
			ret=Mo_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_again_ln_Fet_Upd(&T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_again_ln.sts[0]!='1' && T_mo_again_ln.sts[0]!='2' )
			{
				sprintf(acErrMsg,"该笔再贷款状态不正确!",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B286" );
				goto ErrExit;
			}
			strcpy(T_mo_again_ln.sts,"1");
			T_mo_again_ln.sum_amt -=ptrace_log->amt;
			T_mo_again_ln.cur_amt=0;
			ret=Mo_again_ln_Upd_Upd(T_mo_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_again_ln_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*再贷款还款登记薄*/
static int Rol_mo_pay_again_ln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_pay_again_ln_c	T_mo_pay_again_ln;
	char tmp_str[17];
	memset(&T_mo_pay_again_ln,0x00,sizeof(struct mo_pay_again_ln_c));

	switch( do_mode )
	{
		case '1' :	/*** 再贷款还款录入撤销 ***/
			ret=Mo_pay_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' and sts='0' and amt=%lf ",ptrace_log->ac_no,ptrace_log->amt);
			ERR_DEAL
			while(1)
			{
				ret=Mo_pay_again_ln_Fet_Upd(&T_mo_pay_again_ln,g_pub_tx.reply);
				ERR_DEAL
				if( T_mo_pay_again_ln.sts[0]!='0' )
				{
					sprintf(acErrMsg,"该笔再贷款还款记录非录入状态，请先撤销记账交易!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"B288" );
					goto ErrExit;
				}
				sprintf(tmp_str,"%.2f",T_mo_pay_again_ln.intst_amt);
				pub_base_strpack(ptrace_log->note_no);
				if( strcmp(ptrace_log->note_no,tmp_str) )
					continue;
				else
					break;
			}
			ret=Mo_pay_again_ln_Del_Upd(T_mo_pay_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_pay_again_ln_Clo_Upd();
			break;
		case '2' :	/*** 再贷款还款记账撤销 ***/
			ret=Mo_pay_again_ln_Dec_Upd(g_pub_tx.reply," pact_no='%s' and sts='1' and amt=%lf ",ptrace_log->ac_no,ptrace_log->amt);
			ERR_DEAL
			while(1)
			{
				ret=Mo_pay_again_ln_Fet_Upd(&T_mo_pay_again_ln,g_pub_tx.reply);
				ERR_DEAL
				if( T_mo_pay_again_ln.sts[0]!='1' )
				{
					sprintf(acErrMsg,"该笔再贷款还款记录状态不正确!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"B289" );
					goto ErrExit;
				}
				sprintf(tmp_str,"%.2f",T_mo_pay_again_ln.intst_amt);
				pub_base_strpack(ptrace_log->note_no);
				if( strcmp(ptrace_log->note_no,tmp_str) )
					continue;
				else
				{
					strcpy(T_mo_pay_again_ln.sts,"0");
					break;
				}
			}
			ret=Mo_pay_again_ln_Upd_Upd(T_mo_pay_again_ln,g_pub_tx.reply);
			ERR_DEAL
			Mo_pay_again_ln_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*（介质）积分主表*/
static int Rol_point_point_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*积分使用明细表*/
static int Rol_point_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*产品积分主表*/
static int Rol_prdt_point_mst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*产品积分使用明细表*/
static int Rol_prdt_point_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*考核总帐*/
static int Rol_ass_gl( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*考核主文件*/
static int Rol_ass_mst( struct trace_log_c *ptrace_log, char do_mode )
{
	struct ass_mst_c g_ass_mst;
	struct ass_mst_hst_c ass_mst_hst_g,tmp_ass_mst_hst;
	int ret=0;
    switch(do_mode)
    {
        case '1':			/* 记登记簿的:拆出和上存 */
			vtcp_log("[%s][%d]开始删除ass_mst记录[%s]\n",__FILE__,__LINE__,ptrace_log->note_no);
            ret=sql_execute("delete from ass_mst where pact_no=%s",
                ptrace_log->note_no);
            ERR_DEAL
            break;
		case '2':			/* 记帐类的:拆借还款和上存支取 */
	if(ptrace_log->trace_cnt!=1)	/* 第一笔是其他的不是否则会重复取消登记簿内容 */
		return(0);
	vtcp_log("开始取消ass_mst登记簿[%s][%d]\n",__FILE__,__LINE__);
	memset(&g_ass_mst,'\0',sizeof(g_ass_mst));
	memset(&ass_mst_hst_g,'\0',sizeof(ass_mst_hst_g));
	ret=Ass_mst_hst_Sel(g_pub_tx.reply,&ass_mst_hst_g,"tx_date=%ld and trace_no=%ld ", \
		ptrace_log->tx_date,ptrace_log->trace_no);
	ERR_DEAL

	ret=Ass_mst_Dec_Upd(g_pub_tx.reply,"pact_no=%ld",ass_mst_hst_g.pact_no);
	ERR_DEAL

	ret=Ass_mst_Fet_Upd(&g_ass_mst,g_pub_tx.reply);
	ERR_DEAL
	g_ass_mst.bal +=ass_mst_hst_g.tx_amt;
	g_ass_mst.intst_acm +=ass_mst_hst_g.intst_acm;
	g_ass_mst.intst=0;
	strcpy(g_ass_mst.sts,"1");

	ret=Ass_mst_Upd_Upd(g_ass_mst,g_pub_tx.reply);
	ERR_DEAL
	Ass_mst_Clo_Upd();

    /* 登记考核明细 */
	memset(&tmp_ass_mst_hst,'\0',sizeof(tmp_ass_mst_hst));
    strcpy(tmp_ass_mst_hst.br_no,g_ass_mst.br_no);
    tmp_ass_mst_hst.tx_date = g_pub_tx.tx_date;
    tmp_ass_mst_hst.trace_no = g_pub_tx.trace_no;
    strcpy(tmp_ass_mst_hst.sub_br_no,g_ass_mst.sub_br_no);
    tmp_ass_mst_hst.pact_no = g_ass_mst.pact_no;
    tmp_ass_mst_hst.tx_amt = 0 - ass_mst_hst_g.tx_amt;
    tmp_ass_mst_hst.bal = g_ass_mst.bal;
    tmp_ass_mst_hst.tx_time = g_pub_tx.tx_time;
    strcpy(tmp_ass_mst_hst.brf,"支取撤消");
    tmp_ass_mst_hst.hst_cnt++;
    strcpy(tmp_ass_mst_hst.tel,g_pub_tx.tel);
    strcpy(tmp_ass_mst_hst.chk,g_pub_tx.chk);
    strcpy(tmp_ass_mst_hst.auth,g_pub_tx.auth);

    ret = Ass_mst_hst_Ins(tmp_ass_mst_hst,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记考核明细错误!!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O103");
        goto ErrExit;
    }
	break;
    	default:
            sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
            WRITEMSG
            strcpy( g_pub_tx.reply,"S054" );
            goto ErrExit;
    }

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*考核明细*/
static int Rol_ass_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*考核分段计息表*/
static int Rol_ass_subs_intst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*授权码管理表*/
static int Rol_auth_code( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
static int Rol_intst_hst( struct trace_log_c *ptrace_log, char do_mode )
/*结息登记簿*/
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*活期存款明细*/
static int Rol_dd_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贷款明细*/
static int Rol_ln_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*透支帐单文件*/
static int Rol_od_bill( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*透支明细*/
static int Rol_od_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*定期存款明细*/
static int Rol_td_mst_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*按揭贷款欠款表*/
static int Rol_ln_lo( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贷款放款还款计划表*/
static int Rol_ln_pay_pln( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;

	sprintf(acErrMsg,"贷款放款还款计划表[%c]",do_mode);
	WRITEMSG
	switch(do_mode)
	{
		case '1':
			ret=sql_execute("delete from ln_pay_pln where wrk_date=%ld and pay_ind='0' and ac_id=%ld and ac_seqn=%d",
				ptrace_log->tx_date,ptrace_log->ac_id,ptrace_log->ac_seqn);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贷款还款顺序表*/
static int Rol_ln_pay_seq( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*贷款登记簿*/
static int Rol_ln_reg( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*介质与账户关系*/
static int Rol_mdm_ac_rel( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mdm_ac_rel_c vmdm_ac_rel;
	int ret=0;

	sprintf(acErrMsg,"MDM_AC_REL[%c]",do_mode);
	WRITEMSG

	switch(do_mode)
	{
		case '1': /**撤销开**/
			if ( strcmp(ptrace_log->tx_code, "2104") == 0 )
			{
				break;
			}

			ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'",
				ptrace_log->ac_no );
			ERR_DEAL
			ret=Mdm_ac_rel_Fet_Upd( &vmdm_ac_rel,g_pub_tx.reply );
			ERR_DEAL
			if( vmdm_ac_rel.note_sts[0]!='0' || vmdm_ac_rel.coll_sts[0]!='0' )
			{
				strcpy( g_pub_tx.reply,"L016" );
				goto ErrExit;
			}
			vmdm_ac_rel.note_sts[0]='5';
			ret=Mdm_ac_rel_Upd_Upd( vmdm_ac_rel,g_pub_tx.reply );
			ERR_DEAL
			Mdm_ac_rel_Clo_Upd( );
			break;
		case '2': /**撤销销**/
			ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply,"ac_no='%s'",
				ptrace_log->ac_no );
			ERR_DEAL
			ret=Mdm_ac_rel_Fet_Upd( &vmdm_ac_rel,g_pub_tx.reply );
			if( ret!=100 )
			{
				ERR_DEAL
			}
			if( ret==0 )
			{
				if( vmdm_ac_rel.note_sts[0]=='*' )
				{
					vmdm_ac_rel.note_sts[0]='0';
					ret=Mdm_ac_rel_Upd_Upd( vmdm_ac_rel,g_pub_tx.reply );
					ERR_DEAL
				}
			}
			Mdm_ac_rel_Clo_Upd( );
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*未打折明细表*/
static int Rol_mdm_unprt_hst( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret;
	struct mdm_unprt_hst_c mdm_unprt_hst_c;
	struct mdm_ac_rel_c mdm_ac_rel_c;
	struct pub_tx v_pub_tx;
	int i=0;
vtcp_log("ROLLBACK unprt_hst");

	switch(do_mode)
	{
		case '1':
			ret=Mdm_unprt_hst_Dec_Sel( g_pub_tx.reply,
			"ac_id=%ld and ac_seqn=%d and tx_date=%ld order by trace_no DESC",
				ptrace_log->ac_id, ptrace_log->ac_seqn,
				ptrace_log->tx_date );
			ERR_DEAL

			ret=Mdm_unprt_hst_Fet_Sel( &mdm_unprt_hst_c,g_pub_tx.reply );
			if( ret==100 )
			{
vtcp_log("WRITe 更正 hst");
					memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
					strcpy( g_pub_tx.brf,"更正" );
					pub_unprt_hst_ins(g_pub_tx.ac_id_type);
					memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
			}
			else if( ret )
			{
				ERR_DEAL
			}
			else
			{
				if( mdm_unprt_hst_c.trace_no==ptrace_log->trace_no )
				{
					ret=sql_execute("delete from mdm_unprt_hst where ac_id=%ld and ac_seqn=%d and tx_date=%ld and trace_no=%ld",
						ptrace_log->ac_id, ptrace_log->ac_seqn,
						ptrace_log->tx_date, ptrace_log->trace_no );
					ERR_DEAL
				}
				else
				{
vtcp_log("WRITE 更正 hst");
					memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
					strcpy( g_pub_tx.brf,"更正" );
					pub_unprt_hst_ins(g_pub_tx.ac_id_type);
					memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
				}
			}

			Mdm_unprt_hst_Clo_Sel( );
			break;
		case '2':
			memcpy( &v_pub_tx,&g_pub_tx,sizeof(v_pub_tx) );
			strcpy( g_pub_tx.brf,"更正" );
			pub_unprt_hst_ins(g_pub_tx.ac_id_type);
			memcpy( &g_pub_tx,&v_pub_tx,sizeof(v_pub_tx) );
			break;
		default:
			break;
	}
	Mdm_ac_rel_Clo_Upd( );
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*账户变更历史*/
static int Rol_mo_ac_chg_hst( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}

/* 收费明细 */
static int Rol_com_chrg_hst( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;

	 sprintf(acErrMsg,"撤销收费明细[%c]",do_mode);
	 WRITEMSG
	 switch(do_mode)
	 {
		case '1':
			ret=sql_execute("delete from com_chrg_hst where tx_date=%ld and \
					trace_no=%ld and ac_no='%s'", ptrace_log->tx_date,
					ptrace_log->trace_no,ptrace_log->ac_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/* 客户产品关系表 */
static int Rol_cif_prdt_rel( struct trace_log_c *ptrace_log, char do_mode )
{
	 int ret=0;

	 switch(do_mode)
	 {
		case '1':
			ret=sql_execute("delete from cif_prdt_rel where ac_id=%ld",
				ptrace_log->ac_id );
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
/*冻结登记簿*/
static int Rol_mo_hold( struct trace_log_c *ptrace_log, char do_mode )
{
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*大额支付汇兑流水表*/
static int Rol_Mbfeghvsendjrn( struct trace_log_c *ptrace_log, char do_mode )
{
	struct mbfeghvsendjrn_c	sMbfeghvsendjrn;
	int ret=0;
	memset(&sMbfeghvsendjrn,0x00,sizeof(struct mbfeghvsendjrn_c));
	if( do_mode=='1' )	/** 删除 **/
	{
		ret=Mbfeghvsendjrn_Dec_Sel(g_pub_tx.reply," consigndate=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		if( ret )
		{
			sprintf(acErrMsg,"取大额支付汇兑流水表错误ret=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P459" );
			goto ErrExit;
		}

		while(1)
		{
			ret=Mbfeghvsendjrn_Fet_Sel(&sMbfeghvsendjrn,g_pub_tx.reply);
			if( ret==100 )	break;
			if( ret )
			{
				sprintf(acErrMsg,"取大额支付汇兑流水表错误ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P459" );
				goto ErrExit;
			}
			if( sMbfeghvsendjrn.procstate[0]=='1' )
			{
				sprintf(acErrMsg,"该笔业务已经通过大额支付系统汇出不能撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P460" );
				goto ErrExit;
			}
		}
		Mbfeghvsendjrn_Clo_Sel();
		ret=sql_execute("delete from mbfeghvsendjrn where consigndate=%ld and trace_no=%ld ",ptrace_log->tx_date,ptrace_log->trace_no);
		if( ret )
		{
			sprintf(acErrMsg,"删除大额支付汇兑流水表错误!ret=[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P461" );
			goto ErrExit;
		}
	}
	else if( do_mode=='2' )	/** 修改 **/
	{
		;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*大额支付来账清单*/
static int Rol_Mbfeghvrecvlist( struct trace_log_c *ptrace_log, char do_mode )
{
	char tx_date[9];
	int ret=0;
	struct mbfeghvrecvlist_c	sMbfeghvrecvlist;
	memset(&sMbfeghvrecvlist,0x00,sizeof(struct mbfeghvrecvlist_c));
	if( do_mode=='1' )	/** 删除 **/
	{
		;
	}
	else if( do_mode=='2' )	/** 修改 **/
	{
			sprintf(tx_date,"%8ld",ptrace_log->tx_date);
			ret=Mbfeghvrecvlist_Dec_Upd(g_pub_tx.reply," incometime[1,8]='%s' and incomeflowno=%ld ",tx_date,ptrace_log->trace_no);
			if( ret )
			{
				sprintf(acErrMsg,"取大额支付来帐流水表错误ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P466" );
				goto ErrExit;
			}

			ret=Mbfeghvrecvlist_Fet_Upd(&sMbfeghvrecvlist,g_pub_tx.reply);
			if( ret==100 )
			{
				Mbfeghvsendjrn_Clo_Upd();
				return 0;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"取大额支付来帐流水表错误ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P466" );
				goto ErrExit;
			}
			if( sMbfeghvrecvlist.procstate[0]!='7' )
			{
				sprintf(acErrMsg,"大额支付来帐流水状态不正确，不能撤销!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"P467" );
				goto ErrExit;
			}
			strcpy(sMbfeghvrecvlist.incomeacno,"");
			strcpy(sMbfeghvrecvlist.incomebkcode,"");
			strcpy(sMbfeghvrecvlist.incometime,"");
			sMbfeghvrecvlist.incomeflowno=0;
			strcpy(sMbfeghvrecvlist.incometel,"");
			sMbfeghvrecvlist.procstate[0]='5';

			ret=Mbfeghvrecvlist_Upd_Upd(sMbfeghvrecvlist,g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"更新大额支付来帐流水表错误!ret=[%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P468" );
				goto ErrExit;
			}
			Mbfeghvsendjrn_Clo_Upd();
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}


/*现金收付登记簿 add by xxx*/
static int Rol_Cash_book( struct trace_log_c *ptrace_log, char do_mode )
{
	int		ret=0;
	struct	cash_book_c		sCashBook;
	struct	com_sys_parm_c		com_sys_parm;
	long 	l_trace_no=0;
	char add_ind[2];

sprintf( acErrMsg,"zyzyzyz ptrace_log->tx_date[%ld],ptrace_log->trace_no[%ld]",
	ptrace_log->tx_date, ptrace_log->trace_no );
WRITEMSG

	if( !strcmp(ptrace_log->sub_tx_code,"M202") )
	{
		if( ptrace_log->add_ind[0]=='1' )
			strcpy( add_ind,"0" );
		else
			strcpy( add_ind,"1" );
	}
	else
	{
		strcpy( add_ind,ptrace_log->add_ind );
	}
	l_trace_no=ptrace_log->trace_no;
vtcp_log("ZYZYZYZ[%c][%d] [%s]",do_mode,l_trace_no,add_ind);

	switch(do_mode)
	{
		case '1': /* 对内现金收付撤销 */
			memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
			if (add_ind[0] == '0')
			{
			}
			else if (add_ind[0] == '1')
			{
vtcp_log("Cash2: tx_date=%ld and wrk_no=%ld",ptrace_log->tx_date,l_trace_no );
			ret = Cash_book_Sel( g_pub_tx.reply, &sCashBook,
				"tx_date=%ld and wrk_no=%ld",ptrace_log->tx_date,l_trace_no );
			}
			if(ret != 0 && ret != 100)
			{
				sprintf(acErrMsg, "查询现金收付登记簿错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "M028");
				goto ErrExit;
			}
			else if(ret == 100)
			{
				goto OkExit;
				sprintf(acErrMsg, "没有该笔付出记录![%ld]",ptrace_log->trace_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "M027");
				goto ErrExit;
				break;
			}

			pub_base_strpack(ptrace_log->brf);
			if( !strcmp(ptrace_log->brf,"预开票") )
			{
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and trace_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Dec_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "现金收付登记簿中不存在此笔收入对应的付出记录!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}else if ( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Fet_Upd错!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}
				if( sCashBook.ind[0]!='2' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M034" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "*" );	/* 获取标志: 0 未取 1 已取 */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
				ret=sql_execute("delete from cash_book where trace_no=%ld and tx_date=%ld and ind='*'",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
			}
			else if( !strcmp(ptrace_log->brf,"核销预开票") )
			{
				/**取消当日付出**/
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and out_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				ERR_DEAL

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "现金收付登记簿中不存在此笔收入对应的付出记录!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}
				ERR_DEAL

				if( sCashBook.ind[0]!='0' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M031" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "*" );	/* 获取标志: 0 未取 1 已取 */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
				ret=sql_execute("delete from cash_book where out_no=%ld and tx_date=%ld and ind='*'",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
				/**取消昨日预开**/
				ret=Com_sys_parm_Sel( g_pub_tx.reply,&com_sys_parm,"1=1" );
				ERR_DEAL
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and out_no=%ld and ind='3'",
					com_sys_parm.lst_date, l_trace_no );
				ERR_DEAL

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "现金收付登记簿中不存在此笔收入对应的付出记录!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}
				ERR_DEAL

				if( sCashBook.ind[0]!='3' )
				{
					TRACE
					strcpy( g_pub_tx.reply, "M031" );
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "2" );	/* 获取标志: 0 未取 1 已取 */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
			}
			else if( add_ind[0] == '0' )			/* 现金付出时 */
			{
vtcp_log("Cash1: tx_date=%ld and trace_no=%ld",ptrace_log->tx_date,l_trace_no );
			ret = Cash_book_Sel( g_pub_tx.reply, &sCashBook,
				"tx_date=%ld and out_no=%ld",ptrace_log->tx_date,l_trace_no );
				if( sCashBook.ind[0] == '1' )
				{
					sprintf(acErrMsg, "该笔现金付出记录已作收入不能撤销!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "M029");
					goto ErrExit;
				}

				ret=sql_execute("delete from cash_book where out_no=%ld and tx_date=%ld ",ptrace_log->trace_no,ptrace_log->tx_date);
				ERR_DEAL
			}
			else										/* 现金收入时 */
			{
				ret = Cash_book_Dec_Upd( g_pub_tx.reply,
					"tx_date=%ld and wrk_no=%ld",
					ptrace_log->tx_date, l_trace_no );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Dec_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
				if ( ret == 100 )
				{
					sprintf( acErrMsg, "现金收付登记簿中不存在此笔收入对应的付出记录!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "M032" );
					goto ErrExit;
				}else if ( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Fet_Upd错!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}

				strcpy( sCashBook.ind, "0" );	/* 获取标志: 0 未取 1 已取 */

				ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Cash_book_Clo_Upd( );
			}
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/*保兑仓登记薄*/
static int Rol_mo_sg_cash_po_bill( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_sg_cash_po_bill_c	T_mo_sg_cash_po_bill;
	memset(&T_mo_sg_cash_po_bill,0x00,sizeof(struct mo_sg_cash_po_bill_c));

	switch( do_mode )
	{
		case '1' :	/*** 开提货单撤销 ***/
			ret=Mo_sg_cash_po_bill_Dec_Upd(g_pub_tx.reply," lading_bill='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Mo_sg_cash_po_bill_Fet_Upd(&T_mo_sg_cash_po_bill,g_pub_tx.reply);
			ERR_DEAL
			ret=Mo_sg_cash_po_bill_Del_Upd(T_mo_sg_cash_po_bill,g_pub_tx.reply);
			ERR_DEAL
			Mo_sg_cash_po_bill_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/* 担保保证金登记簿 */
Rol_mo_sg_protect_note(struct trace_log_c *ptrace_log, char do_mode)
{
	int	ret=0;
	struct	protect_note_c pro_note;
	memset( &pro_note,0x00,sizeof(struct protect_note_c));

sprintf(acErrMsg,"担保保证金交易[%s][%s][%c]",ptrace_log->tx_code,ptrace_log->note_no,do_mode);
WRITEMSG
	if ( strncmp("6321",ptrace_log->tx_code,4) &&
		strncmp("6322",ptrace_log->tx_code,4) &&
		strncmp("6323",ptrace_log->tx_code,4) &&
		strncmp("6325",ptrace_log->tx_code,4) &&
		strncmp("6326",ptrace_log->tx_code,4) )
	{
		sprintf(acErrMsg,"非但保保证金交易[%s]",ptrace_log->tx_code);
		WRITEMSG
		goto OkExit;
	}
	pub_base_strpack(ptrace_log->note_no);
	switch( do_mode )
	{
		case '1' :	/*** 开立保函撤销 ***/
			/**
			ret = sql_execute("UPDATE protect_note SET protect_sts='*' WHERE pact_no='%s'",ptrace_log->note_no);
			**/
			ret=Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->note_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			if( pro_note.protect_sts[0]!='0' )
			{
				sprintf(acErrMsg,"保函协议登记薄状态不对[%s]",pro_note.protect_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"B349" );
				goto ErrExit;
			}
			strcpy(pro_note.protect_sts,"A");
			ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
			ERR_DEAL
			Protect_note_Clo_Upd();
			break;
			ERR_DEAL
			break;
		case '2' :	/*** 保函到期与赔偿撤销 ***/
			ret = sql_execute("UPDATE protect_note SET protect_sts='0' WHERE pact_no='%s'",ptrace_log->note_no);
			ERR_DEAL
			break;
		case '3' :	/*** 保证金存款撤销 ***/
			ret=Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			if( ptrace_log->add_ind[0]=='1' )	/** 缴存保证金 **/
			{
				if( pro_note.protect_sts[0]!='A' )
				{
					sprintf(acErrMsg,"保函协议登记薄状态不对[%s]",pro_note.protect_sts);
					WRITEMSG
					strcpy( g_pub_tx.reply,"B349" );
					goto ErrExit;
				}
				ret=Protect_note_Del_Upd(pro_note,g_pub_tx.reply);
				ERR_DEAL
			}
			else 								/** 补缴保证金 **/
			{
				pro_note.bail_amt=pro_note.bail_amt-ptrace_log->amt;
				ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
				ERR_DEAL
			}
			Protect_note_Clo_Upd();
			break;
		case '6' :	/*** 保证金支取交易 ***/
			if( ptrace_log->no_show[0]!='1' )
				break;
			ret=Protect_note_Dec_Upd(g_pub_tx.reply," pact_no='%s' and protect_sts!='*'",ptrace_log->ac_no);
			ERR_DEAL
			ret=Protect_note_Fet_Upd(&pro_note,g_pub_tx.reply);
			ERR_DEAL
			pro_note.bail_amt=pro_note.bail_amt+ptrace_log->amt;
			ret=Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
			ERR_DEAL
			Protect_note_Clo_Upd();
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*大额滞留提入*/
Rol_Mbfeghvsendlist(struct trace_log_c *ptrace_log, char do_mode)
{
	g_reply_int = sql_execute("update mbfeghvsendlist set procstate='30',incomeacno='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"撤销交易更新大额往账登记簿记录错误![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}

/*大额来账挂账业务提入*/
Rol_Mbfeghvrecvlist1(struct trace_log_c *ptrace_log, char do_mode )
{
	g_reply_int = sql_execute("update mbfeghvrecvlist set procstate[1]='5',incomeacno='',incomebkcode='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"撤销交易更新大额来账登记簿记录错误![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/* add by LiuHuafeng 2006-10-4 10:16
 * 大额登记簿
 */
Rol_Hv_zf(struct trace_log_c *ptrace_log, char do_mode )
{
	int 	iRet=0;
	struct 	hv_zf_c sHv_zf;
	struct 	hv_zf_c sHv_zf_th;/***专为退汇使用的结构体****/
	char   	cWz_sts[2];	 /**由于对原交易处理在对退汇记录处理之后,需要对退汇记录的状态进行备份**/
	char 	cReply[5];

	memset(cReply, 0 , sizeof(cReply));
	memset(&sHv_zf, 0 , sizeof(sHv_zf));
	memset(&sHv_zf_th, 0 ,sizeof(sHv_zf_th));
	memset(cWz_sts, 0 , sizeof(cWz_sts));

	iRet=Hv_zf_Dec_Upd(cReply,"lw_ind=1 and ( (chk_trace_no=%d and chk='%s') or (trace_no=%d and tel='%s') ) and wt_date=%d",ptrace_log->trace_no,ptrace_log->tel,ptrace_log->trace_no,ptrace_log->tel,ptrace_log->tx_date);
	if(iRet)
	{
		vtcp_log("%s,%d 定义大额业务登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	iRet=Hv_zf_Fet_Upd( &sHv_zf, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d 没有找到要撤销的大额业务登记簿[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	/***准备四个字段,用来处理退汇交易时候锁定原支付表****/
	memcpy(sHv_zf_th.or_br_no,	sHv_zf.ac_br_no,	sizeof(sHv_zf.ac_br_no)-1);
	memcpy(sHv_zf_th.orderno,	sHv_zf.o_orderno,	sizeof(sHv_zf.o_orderno)-1);
	sHv_zf_th.wt_date	=	sHv_zf.o_wt_date;
	memcpy(sHv_zf_th.cmtno,		sHv_zf.o_cmtno,		sizeof(sHv_zf.o_cmtno)-1);
	/****备份该笔记录的状态,专为退汇时候处理原支付来帐交易***/
	cWz_sts[0]	=	sHv_zf.hv_sts[0];


	if(sHv_zf.hv_sts[0]!=SEND_ACCT_CHECK && sHv_zf.hv_sts[0]!=SEND_REGIST)
	{
		vtcp_log("%s,%d 大额状态=[%c]不可以取消",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
		goto ErrExit;
	}
	if(sHv_zf.hv_sts[0]==SEND_ACCT_CHECK)
	{
		if(ptrace_log->trace_no!=sHv_zf.chk_trace_no)
		{
			vtcp_log("%s,%d 大额状态=[%c]，请先撤销复合流水",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
			strcpy(g_pub_tx.reply,"P074");
			goto ErrExit;
		}
		if(memcmp(sHv_zf.cmtno,HV_HPYC,3))
		{
			sHv_zf.hv_sts[0]=SEND_REGIST;
		}
		else
		{
			sHv_zf.hv_sts[0]=SEND_REGIST;
		}
	}
	else
	{
		sHv_zf.hv_sts[0]=SEND_REVERSE_REG;
	}
	vtcp_log("%s,%d hvsts==[%c]",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
 	iRet=Hv_zf_Upd_Upd( sHv_zf, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P055");
		goto ErrExit;
	}

	Hv_zf_Clo_Upd();


	vtcp_log("[%s][%d]cmtno==[%.3s],hv_sts=[%c]\n",__FILE__,__LINE__,sHv_zf.cmtno,cWz_sts[0]);


	if(!memcmp(sHv_zf.cmtno,HV_TH,3) && cWz_sts[0]==SEND_ACCT_CHECK)
	{
		vtcp_log("[%s][%d]取消的是退汇的交易需要对原来帐的状态进行更新!\n",__FILE__,__LINE__);
		iRet = Hv_zf_Dec_Upd(cReply,"or_br_no='%.12s' and orderno= '%.8s' and cmtno='%.3s' and wt_date=%d ",
				sHv_zf_th.or_br_no,sHv_zf_th.orderno,sHv_zf_th.cmtno,sHv_zf_th.wt_date);
		if(iRet)
		{
				vtcp_log("%s,%d 定义大额业务登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}
		iRet=Hv_zf_Fet_Upd( &sHv_zf_th, cReply );
		if(iRet)
		{
				vtcp_log("%s,%d 没有找到要撤销的大额业务登记簿[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}

	/*	sHv_zf_th.hv_sts[0] = RECEIVE_ACCT_QS;**更新为来帐登记状态***/
		sHv_zf_th.hv_sts[0] = sHv_zf_th.hv_osts[0];/*恢复到原来的状态**/


		iRet=Hv_zf_Upd_Upd( sHv_zf_th, cReply);
		if(iRet)
		{
			vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
			goto ErrExit;
		}

	}

OkExit:
	return 0;
ErrExit:
	return -1;
}

/*
 * 汇票登记簿
 */
Rol_Hv_Poinfo(struct trace_log_c *ptrace_log, char do_mode )
{
	struct hv_poinfo_c sPoinfo;
	struct hv_brno_orno_c sHv_brno_orno;
	char cReply[5];
	char l_postat[1];

	int iRet=0;
	int iPo_no = 0;
	char cPo_no[9];
	memset(cPo_no,0x00,sizeof(cPo_no));
	iPo_no = atoi(ptrace_log->note_no);
	sprintf(cPo_no,"%08d",iPo_no);
	memset(&sHv_brno_orno, 0 , sizeof(sHv_brno_orno));
	memset(cReply, 0 , sizeof(cReply));
	memset(&sPoinfo, 0 , sizeof(sPoinfo));
	l_postat[0] = 0x00;



	if(ptrace_log->svc_ind==7021)
	{
		vtcp_log("[%s][%d凭证这块不要来修改登记表\n",__FILE__,__LINE__);
		goto OkExit;
	}
	/* 找到本交易机构对应的支付系统行号 */
	vtcp_log("[%s][%d]进入汇票取消程序!\n",__FILE__,__LINE__);
	iRet=Hv_brno_orno_Sel(cReply,&sHv_brno_orno,"br_no='%s'",ptrace_log->tx_br_no);
	if(iRet)
	{
		vtcp_log("%s,%d 得到交易机构对应的支付系统行号错误[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}


	iRet=Hv_poinfo_Dec_Upd(cReply,"pay_br_no='%s' and po_no='%s' ",sHv_brno_orno.or_br_no,cPo_no);
	if(iRet)
	{
		vtcp_log("%s,%d 定义汇票登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	iRet=Hv_poinfo_Fet_Upd( &sPoinfo, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d 没有找到要撤销的汇票信息[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	Hv_poinfo_Debug(&sPoinfo);

	vtcp_log("%s,%d 汇票状态=[%s]不可以撤销[%d]",__FILE__,__LINE__,sPoinfo.po_sts,iRet);
	/****营业员提出签发的汇票可以取消***/
	if(  sPoinfo.po_sts[0]==HV_HP_CASH
	 || sPoinfo.po_sts[0]==HV_HP_CASH || sPoinfo.po_sts[0]==HV_HP_JQ
	 || sPoinfo.po_sts[0]==HV_HP_PARTBACK || sPoinfo.po_sts[0]==HV_HP_FULLBACK
	 || sPoinfo.po_sts[0]==HV_HP_TOCUST || sPoinfo.po_sts[0]==HV_HP_UNSIGN
	 || sPoinfo.po_sts[0]==HV_HP_SEND || sPoinfo.po_sts[0]==HV_HP_LOSS
	 || sPoinfo.po_sts[0]==HV_HP_UNLOSS || sPoinfo.po_sts[0]==HV_HP_OVERDATE_UNTREAD
	 || sPoinfo.po_sts[0]==HV_HP_ASKRETURN || sPoinfo.po_sts[0]==HV_HP_CANCEL
	)
	{
		vtcp_log("%s,%d 汇票状态=[%c]不可以撤销[%d]",__FILE__,__LINE__,sPoinfo.po_sts[0],iRet);
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	vtcp_log("%s,%d 汇票状态=[%c]不可以撤销[%d]",__FILE__,__LINE__,sPoinfo.po_sts[0],iRet);

	/****add by liuyue 解决可以先取消登记 再取消移存的问题*****/
	vtcp_log("[%s][%d]ptrace_log->tx_code=[%s],Poinfo.po_sts[0]==[%c]\n",__FILE__,__LINE__,
		ptrace_log->tx_code,sPoinfo.po_sts[0]);
	if(!memcmp(ptrace_log->tx_code,"5805",4) && sPoinfo.po_sts[0]!=HV_HP_REG)
	{
		strcpy(g_pub_tx.reply,"P074");
		if(sPoinfo.po_sts[0]==HV_HP_MV_CENTER)
			set_zd_data("0130","先取消【资金移存记帐】流水->【签发流水】");
		else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST)
			set_zd_data("0130","先取消【未用解付】流水->【签发流水】");
		else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
			set_zd_data("0130","先取消【未用退回冲帐】流水->【签发流水】");
		else if(sPoinfo.po_sts[0]==HV_HP_SIGN)
			set_zd_data("0130","先取消【汇票签发】流水");
		goto ErrExit;
	}
	if(!memcmp(ptrace_log->tx_code,"5807",4) && sPoinfo.po_sts[0]!=HV_HP_SIGN)
	{
			strcpy(g_pub_tx.reply,"P074");
			if(sPoinfo.po_sts[0]==HV_HP_MV_CENTER)
			set_zd_data("0130","先取消【资金移存记帐】流水");
			else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST)
			set_zd_data("0130","先取消【未用解付】流水");
			else if(sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
			set_zd_data("0130","先取消【未用退回冲帐】");
			goto ErrExit;
	}

	if(sPoinfo.po_sts[0]==HV_HP_REG)
	{
		  l_postat[0] = sPoinfo.po_sts[0];
		  sPoinfo.po_sts[0]=HV_HP_UNREG;
		  sPoinfo.l_po_sts[0] = l_postat[0];

	}
	/***add by liuyue 20070109***/
	if(sPoinfo.po_sts[0]==HV_HP_SIGN)
	{
		  vtcp_log("[%s][%d]汇票状态是签发状态,po_sts=[%c],l_po_sts=[%c]\n",__FILE__,__LINE__,sPoinfo.po_sts[0],sPoinfo.l_po_sts[0]);
		  l_postat[0] = sPoinfo.po_sts[0];
		  sPoinfo.po_sts[0]=HV_HP_REG;
		  sPoinfo.l_po_sts[0] = l_postat[0];
		  vtcp_log("[%s][%d]汇票状态是签发状态,po_sts=[%c],l_po_sts=[%c]\n",__FILE__,__LINE__,sPoinfo.po_sts[0],sPoinfo.l_po_sts[0]);

	}
	if( sPoinfo.po_sts[0]==HV_HP_MV_CENTER || sPoinfo.po_sts[0]==HV_HP_UNUSED_TOCUST ||sPoinfo.po_sts[0]==HV_HP_UNUSED_FULLBACK)
	  {
	  	/**如果取消成功,将当前状态保存到l_po_sts，解决登记/签发取消时候再登记问题**/
		vtcp_log("[%s][%d]xiersPoinfo.l_posts=[%c] po_sts=[%c]\n", __FILE__,__LINE__,sPoinfo.l_po_sts[0],sPoinfo.po_sts[0]);
		/*
	  	l_postat[0] = sPoinfo.po_sts[0];
		sPoinfo.po_sts[0]=sPoinfo.l_po_sts[0];
		sPoinfo.l_po_sts[0] = l_postat[0];
		*/
		sPoinfo.l_po_sts[0] =sPoinfo.po_sts[0];
		sPoinfo.po_sts[0]=HV_HP_SIGN;
		vtcp_log("[%s][%d]xiersPoinfo.l_posts=[%c] po_sts=[%c]\n", __FILE__,__LINE__,sPoinfo.l_po_sts[0],sPoinfo.po_sts[0]);
	}
 	iRet=Hv_poinfo_Upd_Upd(sPoinfo, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]修改汇票状态成功了!\n",__FILE__,__LINE__);
	Hv_poinfo_Debug(&sPoinfo);
OkExit:
	vtcp_log("[%s][%d]汇票取消成功 登记表!\n",__FILE__,__LINE__);
	return 0;
ErrExit:
	return -1;
}
/* end by LiuHuafeng 2006-10-4 11:22 */

/*逾期汇票兑付*/
Rol_Mbfebillopnreg( struct trace_log_c *ptrace_log, char do_mode )
{
	g_reply_int = sql_execute("update mbfebillopnreg set useind='5',incomeacno='',incometime='',incomeflowno=0,incometel='' where incometime[1,8]='%ld' and incomeflowno=%ld", ptrace_log->tx_date, ptrace_log->trace_no );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"撤销交易更新大额来账登记簿记录错误![%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/***取消小额支付交易****/
Rol_Lv_Pkgreg(struct trace_log_c *ptrace_log, char do_mode )
{
	vtcp_log("[%s][%d]进入小额支付交易取消程序!\n",__FILE__,__LINE__);
	int 	iRet=0;
	struct 	lv_pkgreg_c sLv_pkgreg;
	struct 	lv_pkgreg_c sLv_pkgreg_th;/***专为退汇使用的结构体****/
	char   	cWz_sts[2];	 /**由于对原交易处理在对退汇记录处理之后,需要对退汇记录的状态进行备份**/
	char 	cReply[5];
	char    cWssrno[7];

	memset(cReply, 0 , sizeof(cReply));
	memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
	memset(&sLv_pkgreg_th, 0 ,sizeof(sLv_pkgreg_th));
	memset(cWz_sts, 0 , sizeof(cWz_sts));
	memset(cWssrno, 0 , sizeof(cWssrno));

	sprintf(cWssrno,"%d",ptrace_log->trace_no);

	iRet=Lv_pkgreg_Dec_Upd(cReply,"lw_ind=1 and ( (check_trace_no='%s' and check_tel='%s') or (input_trace_no='%s' and input_tel='%s') ) and wt_date=%d",cWssrno,ptrace_log->tel,cWssrno,ptrace_log->tel,ptrace_log->tx_date);
	if(iRet)
	{
		vtcp_log("%s,%d 定义大额业务登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}
	iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg, cReply );
	if(iRet)
	{
		vtcp_log("%s,%d 没有找到要撤销的大额业务登记簿[%d]",__FILE__,__LINE__,iRet);
		goto ErrExit;
	}

	/***准备四个字段,用来处理退汇交易时候锁定原支付表****/
	memcpy(sLv_pkgreg_th.or_br_no,	sLv_pkgreg.ac_br_no,	sizeof(sLv_pkgreg.ac_br_no)-1);
	memcpy(sLv_pkgreg_th.orderno,	sLv_pkgreg.o_orderno,	sizeof(sLv_pkgreg.o_orderno)-1);
	sLv_pkgreg_th.wt_date	=	sLv_pkgreg.o_wt_date;
	/****备份该笔记录的状态,专为退汇时候处理原支付来帐交易***/
	cWz_sts[0]	=	sLv_pkgreg.lv_sts[0];


	if(sLv_pkgreg.lv_sts[0]!=STAT_WZCHECK2 && sLv_pkgreg.lv_sts[0]!=STAT_WZLR)
	{
		vtcp_log("%s,%d 大额状态=[%c]不可以取消",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
		strcpy(g_pub_tx.reply,"P057");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]sLv_pkgreg.lv_sts[0]=[%c]=STAT_WZCHECK2=[%c]\n",__FILE__,__LINE__,
		sLv_pkgreg.lv_sts[0],STAT_WZCHECK2);

	/****组包后不可以做撤销交易  add at 20120105 begin****/
	if ( strlen(sLv_pkgreg.packid)!=0 )
	{
		vtcp_log("%s,%d 小额组包后不可以做取消:sLv_pkgreg.packid=[%s] ",__FILE__,__LINE__,sLv_pkgreg.packid);
		strcpy(g_pub_tx.reply,"P057");
		goto ErrExit;
	}
	/****组包后不可以做撤销交易  add at 20120105 end****/

	if(sLv_pkgreg.lv_sts[0]==STAT_WZCHECK2)
	{
		vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sLv_pkgreg.check_trace_no=[%ld]\n",
			__FILE__,__LINE__,ptrace_log->trace_no,apatoi(sLv_pkgreg.check_trace_no,sizeof(sLv_pkgreg.check_trace_no)-1));
		if(ptrace_log->trace_no!=apatoi(sLv_pkgreg.check_trace_no,sizeof(sLv_pkgreg.check_trace_no)-1))
		{
		vtcp_log("%s,%d 大额状态=[%c]，请先撤销复合流水",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
		strcpy(g_pub_tx.reply,"P074");
		goto ErrExit;
		}
		sLv_pkgreg.lv_sts[0]=STAT_WZLR;
	}
	else
	{
		sLv_pkgreg.lv_sts[0]=STAT_INVALID;
	}
	vtcp_log("%s,%d hvsts==[%c]",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
 	iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg, cReply);
	if(iRet)
	{
		vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
		strcpy(g_pub_tx.reply,"P055");
		goto ErrExit;
	}

	Lv_pkgreg_Clo_Upd();


	vtcp_log("[%s][%d]cmtno==[%.3s],lv_sts=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno,cWz_sts[0]);


	if(!memcmp(sLv_pkgreg.pkgno,PKGNO_DJTH,3) && cWz_sts[0]==STAT_WZCHECK2)
	{
		vtcp_log("[%s][%d]取消的是退汇的交易需要对原来帐的状态进行更新!\n",__FILE__,__LINE__);
		iRet = Lv_pkgreg_Dec_Upd(cReply,"or_br_no='%.12s' and orderno= '%.8s'  and wt_date=%d ",
				sLv_pkgreg_th.or_br_no,sLv_pkgreg_th.orderno,sLv_pkgreg_th.wt_date);
		if(iRet)
		{
				vtcp_log("%s,%d 定义大额业务登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}
		iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg_th, cReply );
		if(iRet)
		{
				vtcp_log("%s,%d 没有找到要撤销的大额业务登记簿[%d]",__FILE__,__LINE__,iRet);
				goto ErrExit;
		}

		/*sLv_pkgreg_th.lv_sts[0] = STAT_LZRECV2;*更新为来帐登记状态***/
		sLv_pkgreg_th.lv_sts[0] = sLv_pkgreg_th.lv_o_sts[0];


		iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg_th, cReply);
		if(iRet)
		{
			vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
			goto ErrExit;
		}

	}

OkExit:
	return 0;
ErrExit:
	return -1;
}
/*子帐号主文件存取销撤销*/
Rol_sub_dd_mst( struct trace_log_c *ptrace_log, char do_mode )
{
	int	ret=0;

	int iCount = 0;		/* 被撤销流水后的笔数 */
	double dAcbl = 0.00;	/* 更改后的余额 */
	double sRate=0.00;    /**活期利率**/
	double acm=0.00;      /**利息积数**/

	double dTram = 0.00;	/* 撤销流水对应金额 */
	char cAdd_ind[2];	/* 0－减;1－增 */
	char cSub_ac_no[25];    /* 暂存子账号用 chenchao 20110117 */

	struct  sub_dd_mst_c	T_sub_dd_mst;
	struct  sub_dd_mst_hst_c	T_sub_dd_mst_hst;
	struct  sub_dd_mst_hst_c  sHst;
	memset(&T_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(&T_sub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));
	memset(cAdd_ind,0,sizeof(cAdd_ind));
	/** add by chenchao 20110117 **/
	memset(cSub_ac_no,0,sizeof(cSub_ac_no));
	/* modify begin by wzs 20120514 reason:子账户序号扩成8位导致trace_log的ac_no不够用,子账号的序号在trace_log的ac_id中登记*/
	/* memcpy(cSub_ac_no,ptrace_log->ac_no,sizeof(cSub_ac_no)-1); */
	sprintf(cSub_ac_no,"%s-%ld",ptrace_log->ac_no,ptrace_log->ac_id);
	/* modify end by wzs 20120514 */
	memset(ptrace_log->ac_no,0,sizeof(ptrace_log->ac_no));
	pub_base_cut_str(cSub_ac_no,ptrace_log->ac_no,"-",1);
	vtcp_log(" %s, %d, ac_no=[%s]",__FILE__,__LINE__,ptrace_log->ac_no);

	switch( do_mode )
	{
	case '2' :	/*** 子帐号存取销 ***/
		if((ptrace_log->svc_ind==1201)&& (strcmp(ptrace_log->sub_tx_code,"ZHJX")==0))      /**销户结息 主账户入账 撤销**/
		{
			ret=Sub_dd_mst_Sel(g_pub_tx.reply,&T_sub_dd_mst,"ac_no='%s' and sub_ac_no='%s'",ptrace_log->ac_no,cSub_ac_no);
			if(ret)
			{
				vtcp_log(" %s, %d, 查询二级账户失败！",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			if(T_sub_dd_mst.intst_type[0]!='0' && T_sub_dd_mst.rate_type[0]=='0')
			{
				g_reply_int=pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date, &sRate);
				acm=ptrace_log->amt/sRate*L360*L100;
			}
			else if(T_sub_dd_mst.intst_type[0]!='0' && T_sub_dd_mst.rate_type[0]=='1')
			{
				sRate=T_sub_dd_mst.rate;
				acm=ptrace_log->amt/sRate*L360*L100;
			}
			else
			{
				acm=0.00;
			}
			ret=sql_execute("update sub_dd_mst set intst_acm=intst_acm+%lf where ac_no='%s' and sub_ac_no='%s'",acm,ptrace_log->ac_no,cSub_ac_no);
			if(ret)
			{
				sprintf(acErrMsg,"子帐号撤销销户后子账户积数修改错误!");
				WRITEMSG
				    strcpy(g_pub_tx.reply,"L310");
				goto ErrExit;
			}
		}
		ret=Sub_dd_mst_hst_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld and trace_cnt=%d and ac_no='%s' and tel='%s'",g_pub_tx.tx_date,ptrace_log->trace_no,ptrace_log->trace_cnt,ptrace_log->ac_no,g_pub_tx.tel);
		if(ret)
		{
			vtcp_log(" %s, %d, Sub_dd_mst_hst_Dec_Sel错误【%d】",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}
		while(1)
		{
			memset(&T_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
			memset(&T_sub_dd_mst_hst,0,sizeof(T_sub_dd_mst_hst));
			ret=Sub_dd_mst_hst_Fet_Upd(&T_sub_dd_mst_hst,g_pub_tx.reply);
			if(ret==100)
			{
				break;
			}
			else if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Fet_Upd错误【%d】",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D102");
				goto ErrExit;
			}
			ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%ld ",T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Dec_Upd错误【%d】",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}
			ret=Sub_dd_mst_Fet_Upd(&T_sub_dd_mst,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Dec_Upd错误【%d】",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"");
				goto ErrExit;
			}
			if ( ptrace_log->svc_ind==1201 && T_sub_dd_mst_hst.add_ind[0]=='1' )	/** 存款 **/
			{
				T_sub_dd_mst.bal-=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='1' )
				{
					sprintf(acErrMsg,"该子帐号状态不正确，不能撤销[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
			}
			else if ( ptrace_log->svc_ind==1201 && T_sub_dd_mst_hst.add_ind[0]=='0' )	/** 取款 **/
			{
				T_sub_dd_mst.bal+=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='1' )
				{
					sprintf(acErrMsg,"该子帐号状态不正确，不能撤销[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
			}
			else if ( ptrace_log->svc_ind==1202 )				/** 销户 **/
			{
				T_sub_dd_mst.bal+=T_sub_dd_mst_hst.tx_amt;
				if ( T_sub_dd_mst.ac_sts[0]!='*' )
				{
					sprintf(acErrMsg,"该子帐号状态不正确，不能撤销[%s]",T_sub_dd_mst.ac_sts);
					WRITEMSG
						strcpy( g_pub_tx.reply,"D252" );
					goto ErrExit;
				}
				strcpy(T_sub_dd_mst.ac_sts,"1");
			}
			else
			{
				sprintf(acErrMsg,"无次项操作撤销，不能撤销[%s]",T_sub_dd_mst.ac_sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D253" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(T_sub_dd_mst.bal,0.00)<0 )
			{
				sprintf(acErrMsg,"撤销后余额为负[%c]",do_mode);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P214" );
				goto ErrExit;
			}
			iCount=0;
			iCount = sql_count("sub_dd_mst_hst","tx_date=%ld and trace_no=%ld and ac_no='%s' and sub_ac_seqn=%ld",g_pub_tx.tx_date, T_sub_dd_mst_hst.trace_no,T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			if(iCount >0)
			{
				if(T_sub_dd_mst_hst.add_ind[0]=='1')
				{
					ret = sql_execute("update sub_dd_mst_hst set bal=bal-%lf \
						 where tx_date=%ld and trace_no>%ld and ac_no='%s'  \
						 and sub_ac_seqn=%d",T_sub_dd_mst_hst.tx_amt, \
						g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no, \
						T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
				}else if(T_sub_dd_mst_hst.add_ind[0]=='0')
				{
					ret = sql_execute("update sub_dd_mst_hst set bal=bal+%lf \
						 where tx_date=%ld and trace_no>%ld and ac_no='%s'  \
						 and sub_ac_seqn=%d",T_sub_dd_mst_hst.tx_amt, \
						g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no, \
						T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
				}else{
					vtcp_log("%s,%d,增减标志错误!",__FILE__,__LINE__);
					strcpy( g_pub_tx.reply,"H034" );
					goto ErrExit;

				}
				if(ret)
				{
						vtcp_log("%s,%d,更新记录错误!",__FILE__,__LINE__);
						strcpy( g_pub_tx.reply,"D013" );
						goto ErrExit;
				}
			}
			T_sub_dd_mst.hst_cnt--;
			ret=Sub_dd_mst_Upd_Upd(T_sub_dd_mst,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log(" %s, %d, Sub_dd_mst_Upd_Upd错误【%d】",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			Sub_dd_mst_Clo_Upd();
			/* 如果存在流水比撤销流水大的情况则需要撤销流水金额by Wang Yongwei 20101215*/
			/* delete begin by wzs 20120321 reason: DBS函数使用不对 */
			/* ret=Sub_dd_mst_hst_Del_Upd(g_pub_tx.reply); */
			/* delete end by wzs 20120321 */
			sql_execute("delete sub_dd_mst_hst where tx_date=%ld and trace_no=%ld and ac_no='%s' and sub_ac_seqn=%d and trace_cnt=%d", \
									g_pub_tx.tx_date,T_sub_dd_mst_hst.trace_no,T_sub_dd_mst_hst.ac_no,T_sub_dd_mst_hst.sub_ac_seqn,ptrace_log->trace_cnt);
			/****遂宁是删除原来的记录****/
			/******删除当前记录 by Wang Yongwei 20101215
			ret=sql_execute( "delete from sub_dd_mst_hst where tx_date=%ld and  trace_no=%ld and trace_cnt=%d and tel='%s' and ac_no='%s' and sub_ac_seqn=%d",g_pub_tx.tx_date,ptrace_log->trace_no,ptrace_log->trace_cnt,ptrace_log->tel,ptrace_log->ac_no,T_sub_dd_mst_hst.sub_ac_seqn);
			******/
			if ( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		Sub_dd_mst_hst_Clo_Upd();
		memcpy(ptrace_log->ac_no,cSub_ac_no,sizeof(ptrace_log->ac_no)-1);
		break;
	default:
		sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
		WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
		goto ErrExit;
		break;
	}
	OkExit:
		return 0;
	ErrExit:
		Sub_dd_mst_Clo_Upd();
		return -1;
}

/*承兑汇票拆分登记薄*/
static int Rol_mo_bank_split_po( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	struct mo_bank_split_po_c	T_mo_bank_split_po;
	memset(&T_mo_bank_split_po,0x00,sizeof(struct mo_bank_split_po_c));

	switch( do_mode )
	{
		case '1' :	/*** 拆分票据管理撤销 ***/
			ret=Mo_bank_split_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_split_po_Fet_Upd(&T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_split_po.sts[0]!='1' )
			{
				sprintf(acErrMsg,"非录入状态，不允许撤销录入[%s]",T_mo_bank_split_po.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D254" );
				goto ErrExit;
			}
			ret=Mo_bank_split_po_Del_Upd(T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_split_po_Clo_Upd();
			break;
		case '2' :	/*** 拆分票据管理收回撤销 ***/
			ret=Mo_bank_split_po_Dec_Upd(g_pub_tx.reply," pact_no='%s' ",ptrace_log->ac_no);
			ERR_DEAL
			ret=Mo_bank_split_po_Fet_Upd(&T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			if( T_mo_bank_split_po.sts[0]!='2' )
			{
				sprintf(acErrMsg,"非收回状态，不允许撤销收回[%s]",T_mo_bank_split_po.sts);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D255" );
				goto ErrExit;
			}
			ret=Mo_bank_split_po_Del_Upd(T_mo_bank_split_po,g_pub_tx.reply);
			ERR_DEAL
			Mo_bank_split_po_Clo_Upd();
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*承兑汇票质押存单登记薄*/
static int Rol_impa_dep_reg( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	switch( do_mode )
	{
		case '1' :	/*** 拆分票据管理撤销 ***/
			pub_base_strpack(ptrace_log->note_no);
			ret = sql_execute("delete  from impa_dep_reg WHERE pact_no='%s'",ptrace_log->note_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*保证金计息登记薄*/
static int Rol_mo_bail_rate( struct trace_log_c *ptrace_log, char do_mode )
{
	struct	mo_bail_rate_c	T_mo_bail_rate;
	struct	mo_bail_rate_c	T_mo_bail_rate1;
	char	flag[2];
	int	ret=0;

	strcpy(flag,"");
	switch( do_mode )
	{
		case '2' :	/*** 承兑保证金支取撤销 ***/
			if( ptrace_log->note_type[0]=='Y' )	/** 计息支取 **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='1' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='1' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->ac_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='1' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		case '3' :	/*** 保函保证金支取撤销 ***/
			if( ptrace_log->note_type[0]=='Y' )	/** 计息支取 **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->ac_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->ac_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		case '4' :	/*** 保函到期赔偿撤销 ***/
			if( ptrace_log->note_type[0]=='Y' )	/** 计息支取 **/
			{
				ret=Mo_bail_rate_Sel(g_pub_tx.reply,&T_mo_bail_rate,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->note_no);
				if( ret==100 )
				{
					strcpy(flag,"1");
				}
				else
				{
					ERR_DEAL
				}
vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and intst_date=%ld and end_trace_no=%ld and sts='*'",ptrace_log->note_no,ptrace_log->tx_date,ptrace_log->trace_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate,g_pub_tx.reply);
				ERR_DEAL
				if( flag[0]=='1' )
				{
					T_mo_bail_rate.intst_date=0;
					T_mo_bail_rate.end_trace_no=0;
					T_mo_bail_rate.intst_amt=0.00;
					strcpy(T_mo_bail_rate.sts,"1");
					ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				else
				{
					ret=Mo_bail_rate_Del_Upd(T_mo_bail_rate,g_pub_tx.reply);
					ERR_DEAL
				}
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL

vtcp_log("flag=[%s],pact_no=[%s]",flag,ptrace_log->ac_no);
				if( flag[0]=='1' )
					break;

				ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",ptrace_log->note_no);
				ERR_DEAL
				ret=Mo_bail_rate_Fet_Upd(&T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				T_mo_bail_rate1.amt=T_mo_bail_rate1.amt+T_mo_bail_rate.amt;
				ret=Mo_bail_rate_Upd_Upd(T_mo_bail_rate1,g_pub_tx.reply);
				ERR_DEAL
				Mo_bail_rate_Clo_Upd();
				ERR_DEAL
			}
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}

/*ＭＩＳ借据号对照表登记薄*/
static int Rol_mo_po_co( struct trace_log_c *ptrace_log, char do_mode )
{
	int ret=0;
	switch( do_mode )
	{
		case '1' :	/*** 承兑汇票录入撤销 ***/
			pub_base_strpack(ptrace_log->note_no);
			ret = sql_execute("delete  from mo_po_co WHERE pact_no='%s'",ptrace_log->ac_no);
			ERR_DEAL
			break;
		default:
			sprintf(acErrMsg,"撤销流程定义错误[%c]",do_mode);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S054" );
			goto ErrExit;
			break;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
int iFindSendRecord(char* cmtno,char * txday,char *orderno,char *brno,char *wssrno)
{
	FILE * fpr;
	char cFilename[513];
	char cBasename[15];
	char cRecordLine[MAXLINE+1];
	char *p;
	char cTxday[9];
	struct STR_BaseSend str_sendRecord;
	int iLine=0;
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(cTxday,txday,sizeof(cTxday)-1);
	memset(&str_sendRecord, 0 , sizeof(str_sendRecord));
	memset(cFilename, 0 ,sizeof(cFilename));
	memset(cBasename, 0 , sizeof(cBasename));
	memset(cRecordLine, 0 ,sizeof(cRecordLine));

	memcpy(cBasename,"sd",2);
	memcpy(cBasename+2,cTxday,8);
	memcpy(cBasename+10,".txt",4);
	sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
	vtcp_log("[%s][%d] r+ file=[%s] \n",__FILE__,__LINE__,cFilename);
	fpr=fopen(cFilename,"a+");
	if(fpr==NULL)
	{
		vtcp_log("[%s][%d] open [%s] error\n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	iLine=0;
	while(!feof(fpr))
	{
		memset(cRecordLine, 0 , sizeof(cRecordLine));
		memset(&str_sendRecord, 0 , sizeof(str_sendRecord));
		fgets(cRecordLine,MAXLINE+1,fpr);
		memcpy((char *)&str_sendRecord,cRecordLine,MAXLINE);
		vtcp_log("%s",(char*)&str_sendRecord);
		vtcp_log("wssrno=[%s]",wssrno);
		if(!memcmp(str_sendRecord.wssrno,wssrno,sizeof(str_sendRecord.wssrno)))
		{
			memcpy(orderno,str_sendRecord.orderno,sizeof(str_sendRecord.orderno));
			vtcp_log("[%s][%d] 该笔brno=[%s]wssrno[%s]txday[%s]cmtno[%s]orderno=%s \n业务已经发送过，不可以取消,line=[%d]\n",
				__FILE__,__LINE__,brno,wssrno,txday,cmtno,orderno,iLine);
			fclose(fpr);
			return 1;
		}
		/*
		vtcp_log("[%d][%s]\n",iLine,cRecordLine);
		*/
		iLine++;
		continue;
	}
	fclose(fpr);
	return 0;
}

/***小额错帐处理取消登记簿***/
Rol_Lv_pkgreg_Errdeal(struct trace_log_c *ptrace_log, char do_mode )
{
  vtcp_log("[%s][%d]进入小额错帐处理交易取消程序!\n",__FILE__,__LINE__);
  int   iRet=0;
  struct   lv_pkgreg_c sLv_pkgreg;
  char    cReply[5];
  char    cWssrno[7];

  memset(cReply, 0 , sizeof(cReply));
  memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
  memset(cWssrno, 0 , sizeof(cWssrno));

  sprintf(cWssrno,"%d",ptrace_log->trace_no);

  iRet=Lv_pkgreg_Dec_Upd(cReply,"lw_ind=2 and deal_tel ='%s' and deal_trace_no='%s' and filler like '%%%d%%' ",ptrace_log->tel,cWssrno,ptrace_log->tx_date);
  if(iRet)
  {
    vtcp_log("%s,%d 没有找到该笔流水对应的小额错帐处理业务[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }
  iRet=Lv_pkgreg_Fet_Upd( &sLv_pkgreg, cReply );
  if(iRet)
  {
    vtcp_log("%s,%d 没有找到该笔流水对应的小额错帐处理业务[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }

  vtcp_log("[%s][%d]sLv_pkgreg.lv_sts[0]=[%c]=STAT_LZRECV2=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0],STAT_LZRECV2);


  if(sLv_pkgreg.lv_sts[0]!=STAT_LZRECV2)
  {
    vtcp_log("%s,%d 该笔小额的状态=[%c]不可以取消",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
    strcpy(g_pub_tx.reply,"P057");
    goto ErrExit;
  }
  else
  {
    vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sLv_pkgreg.deal_trace_no=[%ld]\n",
      __FILE__,__LINE__,ptrace_log->trace_no,apatoi(sLv_pkgreg.deal_trace_no,sizeof(sLv_pkgreg.deal_trace_no)-1));
    if(ptrace_log->trace_no!=apatoi(sLv_pkgreg.deal_trace_no,sizeof(sLv_pkgreg.deal_trace_no)-1))
    {
        vtcp_log("%s,%d 该小额的状态=[%c]流水号错",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
        strcpy(g_pub_tx.reply,"P074");
        goto ErrExit;
    }
    sLv_pkgreg.lv_sts[0]=sLv_pkgreg.beg_sts[0];
  }
  vtcp_log("%s,%d lvsts==[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lv_sts[0]);
   iRet=Lv_pkgreg_Upd_Upd( sLv_pkgreg, cReply);
  if(iRet)
  {
    vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
    strcpy(g_pub_tx.reply,"P055");
    goto ErrExit;
  }
  Lv_pkgreg_Clo_Upd();
  vtcp_log("[%s][%d]cmtno==[%.3s],lv_sts=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno,sLv_pkgreg.lv_sts[0]);

OkExit:
  return 0;
ErrExit:
  return -1;
}
Rol_Hv_zf_Errdeal(struct trace_log_c *ptrace_log, char do_mode)
{
  vtcp_log("[%s][%d]进入大额错帐处理交易取消程序!\n",__FILE__,__LINE__);
  int   iRet=0;
  struct   hv_zf_c sHv_zf;
  char    cReply[5];
  char    cWssrno[7];

  memset(cReply, 0 , sizeof(cReply));
  memset(&sHv_zf, 0 , sizeof(sHv_zf));
  memset(cWssrno, 0 , sizeof(cWssrno));

  sprintf(cWssrno,"%d",ptrace_log->trace_no);

  iRet=Hv_zf_Dec_Upd(cReply,"lw_ind=2 and chk ='%s' and chk_trace_no='%s' and dealerr_date = '%d' ",ptrace_log->tel,cWssrno,ptrace_log->tx_date);
  if(iRet)
  {
    vtcp_log("%s,%d 没有找到该笔流水对应的小额错帐处理业务[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }
  iRet=Hv_zf_Fet_Upd( &sHv_zf, cReply );
  if(iRet)
  {
    vtcp_log("%s,%d 没有找到该笔流水对应的小额错帐处理业务[%d]",__FILE__,__LINE__,iRet);
    goto ErrExit;
  }

  vtcp_log("[%s][%d]sHv_zf.hv_sts[0]=[%c]=RECEIVE_ACCT_QS=[%c]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0],RECEIVE_ACCT_QS);


  if(sHv_zf.hv_sts[0]!=RECEIVE_ACCT_QS)
  {
    vtcp_log("%s,%d 该笔大额的状态=[%c]不可以取消",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
    strcpy(g_pub_tx.reply,"P057");
    goto ErrExit;
  }
  else
  {
    vtcp_log("[%s][%d]ptrace_log->trace_no=[%ld] sHv_zf.chk_trace_no=[%ld]\n",
      __FILE__,__LINE__,ptrace_log->trace_no,sHv_zf.chk_trace_no);
    if(ptrace_log->trace_no!=sHv_zf.chk_trace_no)
    {
        vtcp_log("%s,%d 该大额的状态=[%c]流水号错[%ld]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0],sHv_zf.chk_trace_no);
        strcpy(g_pub_tx.reply,"P074");
        goto ErrExit;
    }
    sHv_zf.hv_sts[0]=sHv_zf.beg_sts[0];
  }
  vtcp_log("%s,%d lvsts==[%c]\n",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
  iRet=Hv_zf_Upd_Upd( sHv_zf, cReply);
  if(iRet)
  {
    vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
    strcpy(g_pub_tx.reply,"P055");
    goto ErrExit;
  }
  Hv_zf_Clo_Upd();
  vtcp_log("[%s][%d]cmtno==[%.3s],hv_sts=[%c]\n",__FILE__,__LINE__,sHv_zf.cmtno,sHv_zf.hv_sts[0]);

OkExit:
  return 0;
ErrExit:
  return -1;
}
Rol_Mo_int_tax_hst(struct trace_log_c *ptrace_log, char do_mode)
{
	int   ret=0;
	struct	mo_int_tax_hst_c mo_int_tax_hst;
	char    cReply[5];

	vtcp_log("[%s][%d]取消利息税登记簿里的内容!\n",__FILE__,__LINE__);
	memset(cReply, 0 , sizeof(cReply));
	memset(&mo_int_tax_hst,'\0',sizeof(mo_int_tax_hst));
	ret=Mo_int_tax_hst_Dec_Upd(cReply,"trace_no='%d' and tx_date='%ld' ",ptrace_log->trace_no,ptrace_log->tx_date);
  if(ret && ret!=100)
  {
    vtcp_log("%s,%d 没有找到该笔流水对应的记录[%d]",__FILE__,__LINE__,ret);
    goto ErrExit;
  }
	while(1)
	{
		ret=Mo_int_tax_hst_Fet_Upd(&mo_int_tax_hst,g_pub_tx.reply);
		if(ret==100)
			break;
		Mo_int_tax_hst_Del_Upd(mo_int_tax_hst,g_pub_tx.reply);
	}
	Mo_int_tax_hst_Clo_Upd();

OkExit:
  return 0;
ErrExit:
  return -1;
}
/** 银行本票登记簿  YHBP **/
Rol_Cashier(struct trace_log_c *ptrace_log, char do_mode )
{
    int iRet=0;
    char cPono [13];
    char cTemp [21];
    char cReply[5 ];
    struct cashier_c sCashier;

    memset(cPono , 0 , sizeof(cPono));
    memset(cTemp , 0 , sizeof(cTemp));
    memset(cReply, 0 , sizeof(cReply));
    memset(&sCashier,0 ,sizeof(sCashier));

    memcpy(cPono,ptrace_log->note_no,sizeof(ptrace_log->note_no));
    vtcp_log("[%s][%d]cPono=[%s]\n",__FILE__,__LINE__,cPono);
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+8,cPono,sizeof(cPono)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);

    if(ptrace_log->svc_ind==7021)
    {
        vtcp_log("[%s][%d凭证这块不要来修改登记表\n",__FILE__,__LINE__);
        goto OkExit;
    }
    /* 找到本交易机构对应的支付系统行号 */
    vtcp_log("[%s][%d]进入银行本票取消程序!\n",__FILE__,__LINE__);

    iRet=Cashier_Dec_Upd(cReply," pono='%s' ",cTemp);/** 本票号码唯一，所以敢这么写，哈哈哈**/
    if(iRet)
    {
        vtcp_log("%s,%d 定义本票登记簿游标错误[%d]",__FILE__,__LINE__,iRet);
        goto ErrExit;
    }
    iRet=Cashier_Fet_Upd( &sCashier, cReply );
    if(iRet)
    {
        vtcp_log("%s,%d 没有找到要撤销的汇票信息[%d]",__FILE__,__LINE__,iRet);
        strcpy(g_pub_tx.reply,"P001");
        goto ErrExit;
    }
    Cashier_Debug(&sCashier);

    vtcp_log("[%s][%d]tx_code=[%s]stat[0]==[%c]\n",__FILE__,__LINE__,ptrace_log->tx_code,sCashier.stat[0]);
    if(!memcmp(ptrace_log->tx_code,"6343",4) )/**本票签发交易**/
    {
        if(sCashier.stat[0]!='1')/**如果不是签发状态**/
        {
             vtcp_log("%s,%d 汇票状态=[%c]不可以撤销",__FILE__,__LINE__,sCashier.stat[0]);
             strcpy(g_pub_tx.reply,"P000");
             goto ErrExit;
        }
        else
        {
             sCashier.stat[0]='7';    /**签发取消**/
             sCashier.lactcls[0]='1'; /**签发状态**/
        }
     }
     else if(!memcmp(ptrace_log->tx_code,"6341",4) || !memcmp(ptrace_log->tx_code,"6342",4))/**本票登记交易**/
     {
        if(sCashier.stat[0]!='0')
        {
            vtcp_log("%s,%d 汇票状态=[%c]不可以撤销",__FILE__,__LINE__,sCashier.stat[0]);
            strcpy(g_pub_tx.reply,"P000");
            goto ErrExit;
        }
        else
        {
            sCashier.stat[0]='8';    /**登记取消**/
            sCashier.lactcls[0]='0'; /**登记状态**/
        }
    }
    /******add by NEWBP3*****/
    else if (!memcmp(ptrace_log->tx_code,"6344",4))
	  {
	  	  if(sCashier.stat[0]!='2')/**如果不是兑付状态**/
	  	  {
	  	  	  vtcp_log("%s,%d 本票状态=[%c]不可以撤销",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	sCashier.stat[0]='1';    /**兑付取消 modified by robbit 20080405**/
	  	  	sCashier.lactcls[0]='0'; /**录入状态**/
	  	  }
	  }
	  else if (!memcmp(ptrace_log->tx_code,"6346",4))
	  {
	  	  if(sCashier.stat[0]!='6')/**如果不是退票状态**/
	  	  {
	  	  	  vtcp_log("%s,%d 本票状态=[%c]不可以撤销",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	  sCashier.stat[0]='1';    /**退票取消 modified by robbit 20080405**/
	  	  	  sCashier.lactcls[0]='0'; /**录入状态**/
	  	  }
	  }
	  else if (!memcmp(ptrace_log->tx_code,"6345",4))
	  {
	  	  if(sCashier.stat[0]!='3')/**如果不是解付状态**/
	  	  {
	  	  	  vtcp_log("%s,%d 本票状态=[%c]不可以撤销",__FILE__,__LINE__,sCashier.stat[0]);
	  	  	  strcpy(g_pub_tx.reply,"P000");
	  	  	  goto ErrExit;
	  	  }
	  	  else
	  	  {
	  	  	  sCashier.stat[0]='2';    /**退票取消 modified by robbit 20080405**/
	  	  	  sCashier.lactcls[0]='1'; /**录入状态**/
	  	  }
	  }
    /****end by NEWBP3***/
    else
    {
        vtcp_log("%s,%d 非银行本票业务[%s]",__FILE__,__LINE__,ptrace_log->tx_code);
        goto ErrExit;
    }
    iRet=Cashier_Upd_Upd(sCashier, cReply);
    if(iRet)
    {
        vtcp_log("%s,%d 更新大额登记簿错误[%d]",__FILE__,__LINE__,iRet);
        goto ErrExit;
    }
    vtcp_log("[%s][%d]修改汇票状态成功了!\n",__FILE__,__LINE__);
OkExit:
    vtcp_log("[%s][%d]汇票取消成功 登记表!\n",__FILE__,__LINE__);
    return 0;
ErrExit:
    return -1;
}
