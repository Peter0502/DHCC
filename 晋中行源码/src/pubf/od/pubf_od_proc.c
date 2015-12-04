/*************************************************
* 文 件 名: pubf_od_proc.c
* 功能描述：   
*              pub_od_proc      透支核心记账模块
*
* 作    者:    power
* 完成日期：   2004年3月15日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "svrpub_c.h"
#include "od_dd_rel_c.h"
#include "od_bill_c.h"
#include "od_mst_hst_c.h"
#include "od_drt_def_c.h"
#include "mo_def_rate_c.h"

#define DEF_PRDT_NO "4ZZ"

	static	struct od_parm_c od_parm;
	static	char def_prdt_no[4];
	static	double dr_amt,cr_amt,intst_amt;
	static	double hst_bal;
/**********************************************************************
* 函 数 名：  pubf_od_proc
* 函数功能：  透支产品变更，将账户对应得产品编号改变
* 作    者：  power
* 完成时间：  2004年03月14日
*
* 参    数：
*     输  入：ac_id     integer   账户ID   
*             ac_seqn   smallint  账户序号
*             od_ind    char(2)   透支标志
*             add_ind   char(2)   增减标志
*             dd_prdt   char(4)   活期产品编号
*             amt       double    透支金额/还款金额
*      
*     输  出:           char(4)   响应码
*             amt       double    透支时返回实际透支金额，还款时返回
*                                 还款后剩余金额
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
pubf_od_proc(ac_id,ac_seqn,od_ind,add_ind,dd_prdt_no,amt)
long	ac_id;
int		ac_seqn;
char	*od_ind;
char	*add_ind;
char	*dd_prdt_no;
double	*amt;
{
	struct od_mst_c od_mst;
	struct od_drt_def_c drt_def;
	double amt1;
	double amt2;
	double unit_amt;
	int unit;
	long pay_date;
	double drt_ratio;
	double drt_intst_amt;
	char brf[21];

	memset ( &od_mst , 0x00 , sizeof ( struct od_mst_c ) );
	memset ( &od_parm , 0x00 , sizeof ( struct od_parm_c ) );
	memset ( &drt_def , 0x00 , sizeof ( struct od_drt_def_c ) );
	amt1=0.00;
	amt2=0.00;
	pay_date=0;
	drt_ratio=0.00;
	unit_amt=0.00;
	dr_amt=0.00;
	cr_amt=0.00;
	intst_amt=0.00;
	hst_bal=0.00;

	if ( Od_mst_Dec_Upd(RtCode,"ac_id=%d and ac_seqn=%d",ac_id,ac_seqn) )
	{
		sprintf(acErrMsg,"定义透支主文件update游标错[%d][%d]",ac_id,ac_seqn);
		WRITEMSG
		strcpy (RtCode,"T007");
		return 1;
	}

	RespCode = Od_mst_Fet_Upd( &od_mst , RtCode ) ;
	if ( RespCode < 0 )
	{
		Od_mst_Clo_Upd();
		sprintf(acErrMsg,"取得透支主文件update游标错[%d][%d]",ac_id,ac_seqn);
		WRITEMSG
		strcpy (RtCode,"T008");
		return 1;
	}
	else if ( RespCode )
	{
		Od_mst_Clo_Upd();
		if ( od_ind[0]=='Y' || add_ind[0]=='0')
		{
			sprintf(acErrMsg,"透支主文件无此账户信息[%d][%d]",ac_id,ac_seqn);
			WRITEMSG
			strcpy (RtCode,"T009");
			return 1;
		}
		else
		{
			od_mst.ac_id=ac_id;
			od_mst.ac_seqn=ac_seqn;
			if ( pubf_od_opn_init ( &od_mst , dd_prdt_no ) )
			{
				sprintf(acErrMsg,"生成透支账户错误[%d][%d]",ac_id,ac_seqn);
				WRITEMSG
				return 1;
			}
			if ( od_mst.max_bal < od_mst.bal+*amt )
			{
				sprintf(acErrMsg,"生成透支账户错误[%d][%d]",ac_id,ac_seqn);
				WRITEMSG
				strcpy ( RtCode , "T017" );
				return 1;
			}
			od_mst.bal+=*amt;
			dr_amt+=*amt;
			hst_bal=0.00;
			if ( g_pub_tx.ct_ind[0]=='1' )
			{
				od_mst.cash_bal+=*amt;
			}
			if ( dr_amt >0.005 || dr_amt <-0.005 )
			{
				strcpy ( brf , "透支" );
				if ( pubf_od_hst(&od_mst,dr_amt,add_ind,brf) )
				{
					sprintf(acErrMsg,"登记透支明细错误[%d][%d]",ac_id,ac_seqn);
					WRITEMSG
					return 1;
				}
			}
			od_mst.hst_bal=od_mst.bal;
			if ( Od_mst_Ins ( od_mst , RtCode ) )
			{
				sprintf(acErrMsg,"生成透支账户错误[%d][%d]",ac_id,ac_seqn);
				WRITEMSG
				strcpy ( RtCode , "T016" );
				return 1;
			}
		}
	}
	else
	{
		hst_bal=od_mst.hst_bal;
		RespCode = Od_parm_Sel(RtCode,&od_parm,
			"prdt_no='%s'",od_mst.prdt_no);
		if ( RespCode <0 )
		{
			sprintf(acErrMsg,"取透支产品参数错[%s]",def_prdt_no);
			WRITEMSG
			strcpy (RtCode,"T010");
			return 1;
		}
		else if ( RespCode )
		{
			sprintf(acErrMsg,"无此透支产品错[%s]",def_prdt_no);
			WRITEMSG
			strcpy (RtCode,"T001");
			return 1;
		}
		/*XXX
		RespCode=Od_drt_def_Sel(RtCode,&drt_def,
			"prdt_no='%s' and tx_code='%s'",
			od_mst.prdt_no,g_pub_tx.tx_code);
		*/
		RespCode=Od_drt_def_Sel(RtCode,&drt_def,
			"prdt_code='%s' and tx_code='%s'",
			od_mst.prdt_no,g_pub_tx.tx_code);
		if ( RespCode <0 )
		{
			sprintf(acErrMsg,"取透支免息定义错[%s][%s]",
				def_prdt_no,g_pub_tx.tx_code);
			WRITEMSG
			strcpy (RtCode,"T020");
			return 1;
		}
		else if ( RespCode )
		{
			drt_def.drt_ratio=0.00;
		}

		drt_ratio=drt_def.drt_ratio;
		RespCode=pubf_od_pay_date(od_parm.pay_date,
			od_parm.pay_intvl,od_parm.pay_drt_days,&pay_date);
		if ( RespCode )
		{
			Od_mst_Clo_Upd();
			sprintf(acErrMsg,"帐单归还终止日期错误[%d]",RespCode);
			WRITEMSG
			return 1;
		}
		if ( add_ind[0]=='0' )
		{
			amt1=*amt;
			RespCode=pubf_od_bill_pay (od_mst.ac_id,od_mst.ac_seqn,&amt1);
			if ( RespCode )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"归还欠款帐单错误[%d]",RespCode);
				WRITEMSG
				return 1;
			}
			od_mst.bal-=(*amt-amt1);
			if ( od_mst.bal < -0.005 )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"账单金额与透支账户不符[%lf]",od_mst.bal);
				WRITEMSG
				strcpy ( RtCode , "T027" );
				return 1;
			}
			if ( amt1 >0.005 || od_mst.bal<0.005 )
			{
				if ( od_mst.bal != od_mst.cur_bal )
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"账单金额与透支账户不符[%lf]",od_mst.bal);
					WRITEMSG
					strcpy ( RtCode , "T027" );
					return 1;
				}
				amt2=amt1>=od_mst.bal?od_mst.bal:amt1;
				RespCode=pubf_cal_od_acm1(od_mst.lst_date,
					od_mst.cur_bal,&od_mst.intst_acm);
				if ( RespCode )
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"计算利息积数错误[%d]",RespCode);
					WRITEMSG
					strcpy (RtCode,"T015");
					return 1;
				}
				RespCode=pubf_cal_od_acm1(od_mst.lst_date,
					od_mst.drt_bal,&od_mst.drt_intst_acm);
				if ( RespCode )
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"计算利息积数错误[%d]",RespCode);
					WRITEMSG
					strcpy (RtCode,"T015");
					return 1;
				}
				if ( od_parm.rate_mode[0]!='F' )
				{
					if(pubf_cal_od_intst(od_mst.cur_bal,od_mst.intst_acm,
						&od_mst.intst_bal,&od_mst.intst_amt,od_mst.lst_date))
					{
						Od_mst_Clo_Upd();
						sprintf(acErrMsg,"计算透支利息错");
						WRITEMSG
						strcpy (RtCode,"T023");
						return 1;
					}
					od_mst.intst_acm=0.00;
					if(pubf_cal_od_intst(od_mst.drt_bal,
						od_mst.drt_intst_acm,&od_mst.drt_intst_bal,
						&drt_intst_amt,od_mst.lst_date))
					{
						Od_mst_Clo_Upd();
						sprintf(acErrMsg,"计算透支利息错");
						WRITEMSG
						strcpy (RtCode,"T023");
						return 1;
					}
					od_mst.drt_intst_acm=0.00;
				}
				if ( amt2 <= od_mst.bal )
				{
					amt1=0.00;
					od_mst.bal-=amt2;
					od_mst.cur_bal-=amt2;
					if ( od_mst.bal<od_mst.drt_bal )
					{
						od_mst.drt_bal=od_mst.bal;
					}
					cr_amt+=amt2;
					if ( od_mst.cash_bal > 0.00 )
					{
						od_mst.cash_bal-=amt2;
						if ( od_mst.cash_bal < 0.005 )
						{
							od_mst.cash_bal=0.00;
						}
					}
				}
				else
				{
					if ( od_parm.rate_mode[0]=='F' )
					{
						if(pubf_cal_od_intst(od_mst.bal,od_mst.intst_acm,
							&od_mst.intst_bal,&od_mst.intst_amt,
							od_mst.lst_date))
						{
							Od_mst_Clo_Upd();
							sprintf(acErrMsg,"计算透支利息错");
							WRITEMSG
							strcpy (RtCode,"T023");
							return 1;
						}
						od_mst.intst_acm=0.00;
						if(pubf_cal_od_intst(od_mst.drt_bal,
							od_mst.drt_intst_acm,&od_mst.drt_intst_bal,
							&drt_intst_amt,od_mst.lst_date))
						{
							Od_mst_Clo_Upd();
							sprintf(acErrMsg,"计算透支利息错");
							WRITEMSG
							strcpy (RtCode,"T023");
							return 1;
						}
						od_mst.drt_intst_acm=0.00;
					}
					amt1-=od_mst.bal;
					cr_amt+=od_mst.bal;
					od_mst.bal=0.00;
					od_mst.cur_bal=0.00;
					od_mst.drt_bal=0.00;
					if ( amt1 <= od_mst.intst_bal )
					{
						intst_amt+=amt1;
						od_mst.intst_bal-=amt1;
						amt1=0.00;
					}
					else
					{
						amt1-=od_mst.intst_bal;
						intst_amt+=od_mst.intst_bal;
						od_mst.intst_bal=0.00;
					}
				}
			}
			*amt=amt1;
			if ( cr_amt >0.005 || cr_amt <-0.005 )
			{
				strcpy ( brf , "透支还款" );
				if ( pubf_od_hst(&od_mst,cr_amt,add_ind,brf) )
				{
					sprintf(acErrMsg,"登记透支明细错误[%d][%d]",ac_id,ac_seqn);
					WRITEMSG
					return 1;
				}
			}
			if ( intst_amt >0.005 || intst_amt <-0.005 )
			{
				strcpy ( brf , "透支还息" );
				if ( pubf_od_hst(&od_mst,intst_amt,add_ind,brf) )
				{
					sprintf(acErrMsg,"登记透支明细错误[%d][%d]",ac_id,ac_seqn);
					WRITEMSG
					return 1;
				}
			}
		}
		else if ( add_ind[0]=='1' )
		{
			if ( od_mst.max_bal < od_mst.bal+*amt )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"账户透支[%lf][%lf][%lf",
					od_mst.max_bal,od_mst.bal,*amt);
				WRITEMSG
				strcpy ( RtCode , "T017" );
				return 1;
			}
			if ( g_pub_tx.ct_ind[0]=='1' )
			{
				if ( od_mst.cash_max_bal<od_mst.cash_bal+*amt )
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"账户现金透支[%lf][%lf][%lf",
						od_mst.cash_max_bal,od_mst.cash_bal,*amt);
					strcpy ( RtCode , "T018" );
					return 1;
				}
			}
			RespCode=pubf_cal_od_acm1(od_mst.lst_date,
				od_mst.cur_bal,&od_mst.intst_acm);
			if ( RespCode )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"计算利息积数错误[%d]",RespCode);
				WRITEMSG
				strcpy (RtCode,"T015");
				return 1;
			}
			RespCode=pubf_cal_od_acm1(od_mst.lst_date,
				od_mst.drt_bal,&od_mst.drt_intst_acm);
			if ( RespCode )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"计算利息积数错误[%d]",RespCode);
				WRITEMSG
				strcpy (RtCode,"T015");
				return 1;
			}
			/**XXXX
			RespCode=pubf_od_cal_acm1(od_mst.lst_date,
				od_mst.drt_bal,&od_mst.drt_intst_acm);
			if ( RespCode )
			{
				Od_mst_Clo_Upd();
				sprintf(acErrMsg,"计算利息积数错误[%d]",RespCode);
				WRITEMSG
				strcpy (RtCode,"T015");
				return 1;
			}
			**/
			if ( od_parm.rate_mode[0]!='F' )
			{
				if(pubf_cal_od_intst(od_mst.cur_bal,od_mst.intst_acm,
					&od_mst.intst_bal,&od_mst.intst_amt,od_mst.lst_date))
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"计算透支利息错");
					WRITEMSG
					strcpy (RtCode,"T023");
					return 1;
				}
				od_mst.intst_acm=0.00;
				if(pubf_cal_od_intst(od_mst.drt_bal,
					od_mst.drt_intst_acm,&od_mst.drt_intst_bal,
					&drt_intst_amt,od_mst.lst_date))
				{
					Od_mst_Clo_Upd();
					sprintf(acErrMsg,"计算透支利息错");
					WRITEMSG
					strcpy (RtCode,"T023");
					return 1;
				}
				od_mst.drt_intst_acm=0.00;
			}
			od_mst.bal+=*amt;
			od_mst.cur_bal+=*amt;
			od_mst.drt_bal+=*amt*drt_ratio;
			dr_amt+=*amt;
			if ( g_pub_tx.ct_ind[0]=='1' )
			{
				od_mst.cash_bal+=*amt;
			}
			if ( dr_amt >0.005 || dr_amt <-0.005 )
			{
				strcpy ( brf , "透支" );
				if ( pubf_od_hst(&od_mst,dr_amt,add_ind,brf) )
				{
					sprintf(acErrMsg,"登记透支明细错误[%d][%d]",ac_id,ac_seqn);
					WRITEMSG
					return 1;
				}
			}
		}
		else
		{
			Od_mst_Clo_Upd();
			sprintf(acErrMsg,"程序调用异常[%s]",add_ind);
			WRITEMSG
			strcpy (RtCode,"T014");
			return 1;
		}
		od_mst.hst_bal=od_mst.bal;
		od_mst.lst_date=g_pub_tx.tx_date;
		if ( Od_mst_Upd_Upd ( od_mst , RtCode ) )
		{
			Od_mst_Clo_Upd();
			sprintf(acErrMsg,"修改透支账户错误[%d][%d]",ac_id,ac_seqn);
			WRITEMSG
			strcpy ( RtCode , "T016" );
			return 1;
		}
		Od_mst_Clo_Upd();
	}
	set_zd_double(OD_DR_AMT,dr_amt);
	set_zd_double(OD_CR_AMT,cr_amt);
	set_zd_double(OD_INTST_AMT,intst_amt);

	return 0;
}
pubf_od_opn_init(od_mst,dd_prdt_no)
struct od_mst_c *od_mst;
char *dd_prdt_no;
{
	struct od_dd_rel_c od_dd_rel;

	strcpy ( def_prdt_no , OD_PRDT_CODE );

	RespCode = Od_dd_rel_Sel(RtCode,&od_dd_rel,
		"dd_prdt_no='%s' and od_prdt_no='%s'",
		dd_prdt_no,def_prdt_no);
	if ( RespCode < 0 )
	{
		sprintf(acErrMsg,"取活期产品与透支产品关系表错[%s][%s]",
			dd_prdt_no,def_prdt_no);
		WRITEMSG
		strcpy (RtCode,"T012");
		return 1;
	}
	else if ( RespCode )
	{
		sprintf(acErrMsg,"此产品不能开立此透支产品[%s][%s]",
			dd_prdt_no,def_prdt_no);
		WRITEMSG
		strcpy (RtCode,"T013");
		return 1;
	}

	RespCode = Od_parm_Sel(RtCode,&od_parm,"prdt_no='%s'",def_prdt_no);
	if ( RespCode <0 )
	{
		sprintf(acErrMsg,"取透支产品参数错[%s]",def_prdt_no);
		WRITEMSG
		strcpy (RtCode,"T010");
		return 1;
	}
	else if ( RespCode )
	{
		sprintf(acErrMsg,"无此透支产品错[%s]",def_prdt_no);
		WRITEMSG
		strcpy (RtCode,"T001");
		return 1;
	}
	if(g_pub_tx.tx_date<od_parm.beg_date||g_pub_tx.tx_date >od_parm.end_date)
	{
		sprintf(acErrMsg,"无此透支产品错[%s]",def_prdt_no);
		WRITEMSG
		strcpy (RtCode,"T011");
		return 1;
	}
	strcpy ( od_mst->opn_br_no , g_pub_tx.opn_br_no );
	strcpy ( od_mst->prdt_no , od_parm.prdt_no );
	od_mst->bal=0.00;
	od_mst->cash_bal=0.00;
	od_mst->ys_bal=0.00;
	od_mst->hst_bal=0.00;
	od_mst->intst_acm=0.00;
	od_mst->drt_intst_acm=0.00;
	od_mst->max_bal=od_parm.max_amt;
	od_mst->cash_max_bal=od_parm.cash_amt;
	od_mst->min_pay_amt=od_parm.min_pay_amt;
	od_mst->min_pay_ratio=od_parm.min_pay_ratio;
	od_mst->lst_date=g_pub_tx.tx_date;
	od_mst->hst_cnt=0;
	od_mst->hst_pg=0;
	strcpy ( od_mst->ac_sts , "1" );
	od_mst->pay_ac_id=0;
	od_mst->pay_ac_seqn=0;
	strcpy ( od_mst->mac , "" );
	if ( od_parm.opn_lmt )
	{
		if ( pubf_opn_lmt ( od_parm.opn_lmt ) )
		{
			return 1;
		}
	}

	return 0;
}
pubf_opn_lmt ( int opn_lmt )
{
	return 0;
}
pubf_od_bill_pay(long ac_id,int ac_seqn,double *amt)
{
	struct od_bill_c od_bill;
	double drt_intst_amt;
	double intst_bal;

	memset ( &od_bill , 0x00 , sizeof(struct od_bill_c));

	if(Od_bill_Dec_Upd(RtCode,"ac_id=%d and ac_seqn=%d and pay_sts='N'",
		ac_id,ac_seqn))
	{
		sprintf(acErrMsg,"定义透支帐单游标错");
		WRITEMSG
		strcpy (RtCode,"T019");
		return 1;
	}

	while(1)
	{
		RespCode=Od_bill_Fet_Upd ( &od_bill , RtCode ) ;
		if ( RespCode <0 )
		{
			Od_bill_Clo_Upd();
			sprintf(acErrMsg,"取得透支帐单游标错");
			WRITEMSG
			strcpy (RtCode,"T021");
			return 1;
		}
		else if ( RespCode )
		{
			break;
		}
		drt_intst_amt=0.00;
		if(pubf_cal_od_acm1(od_bill.lst_date,od_bill.bal,
			&od_bill.intst_acm))
		{
			Od_bill_Clo_Upd();
			sprintf(acErrMsg,"计算利息积数错误");
			WRITEMSG
			strcpy (RtCode,"T015");
			return 1;
		}
		if ( g_pub_tx.tx_date <= od_bill.pay_date )
		{
			if(pubf_cal_od_acm1(od_bill.lst_date,od_bill.drt_bal,
				&od_bill.drt_intst_acm))
			{
				Od_bill_Clo_Upd();
				sprintf(acErrMsg,"计算利息积数错误");
				WRITEMSG
				strcpy (RtCode,"T015");
				return 1;
			}
		}
		if ( *amt > od_bill.bal )
		{
			if(pubf_cal_od_intst(od_bill.bal,od_bill.intst_acm,
				&od_bill.intst_bal,&od_bill.intst_amt,od_bill.lst_date))
			{
				Od_bill_Clo_Upd();
				sprintf(acErrMsg,"计算透支利息错");
				WRITEMSG
				strcpy (RtCode,"T023");
				return 1;
			}
			if(pubf_cal_od_intst(od_bill.bal,od_bill.drt_intst_acm,
				&od_bill.drt_intst_bal,&drt_intst_amt,od_bill.lst_date))
			{
				Od_bill_Clo_Upd();
				sprintf(acErrMsg,"计算透支利息错");
				WRITEMSG
				strcpy (RtCode,"T023");
				return 1;
			}
			if ( od_bill.pay_date >= g_pub_tx.tx_date )
			{
				if ( od_bill.intst_bal > od_bill.drt_intst_bal )
				{
					intst_bal=od_bill.intst_bal-od_bill.drt_intst_bal;
				}
				else
				{
					intst_bal=0.00;
				}
			}
			else
			{
				intst_bal=od_bill.intst_bal;
			}
			od_bill.intst_acm=0.00;
			od_bill.drt_intst_acm=0.00;
			*amt-=od_bill.bal;
			cr_amt+=od_bill.bal;
			od_bill.bal=0.00;
			od_bill.lst_date=g_pub_tx.tx_date;
			if ( *amt >= intst_bal )
			{
				*amt-=intst_bal;
				intst_amt+=intst_bal;
				od_bill.intst_bal=0.00;
				od_bill.pay_sts[0]='Y';
				if ( Od_bill_Upd_Upd ( od_bill, RtCode ) )
				{
					Od_bill_Clo_Upd();
					sprintf(acErrMsg,"更新透支账单文件错");
					WRITEMSG
					strcpy (RtCode,"T022");
					return 1;
				}
			}
			else
			{
				od_bill.intst_bal-=*amt;
				intst_amt+=*amt;
				*amt=0.00;
				if ( Od_bill_Upd_Upd ( od_bill, RtCode ) )
				{
					Od_bill_Clo_Upd();
					sprintf(acErrMsg,"更新透支账单文件错");
					WRITEMSG
					strcpy (RtCode,"T022");
					return 1;
				}
				break;
			}
		}
		else
		{
			if ( od_parm.rate_mode[0]!='F' )
			{
				if(pubf_cal_od_intst(od_bill.bal,od_bill.intst_acm,
					&od_bill.intst_bal,&od_bill.intst_amt,od_bill.lst_date))
				{
					Od_bill_Clo_Upd();
					sprintf(acErrMsg,"计算透支利息错");
					WRITEMSG
					strcpy (RtCode,"T023");
					return 1;
				}
				od_bill.intst_acm=0.00;
				if(pubf_cal_od_intst(od_bill.bal,od_bill.drt_intst_acm,
					&od_bill.drt_intst_bal,&drt_intst_amt,od_bill.lst_date))
				{
					Od_bill_Clo_Upd();
					sprintf(acErrMsg,"计算透支利息错");
					WRITEMSG
					strcpy (RtCode,"T023");
					return 1;
				}
				od_bill.drt_intst_acm=0.00;
			}
			od_bill.lst_date=g_pub_tx.tx_date;
			od_bill.bal-=*amt;
			cr_amt+=*amt;
			*amt=0.00;
			if ( Od_bill_Upd_Upd ( od_bill, RtCode ) )
			{
				Od_bill_Clo_Upd();
				sprintf(acErrMsg,"更新透支账单文件错");
				WRITEMSG
				strcpy (RtCode,"T022");
				return 1;
			}
			break;
		}
	}

	Od_bill_Clo_Upd();

	return 0;
}
pubf_cal_od_acm1(long lst_date,double bal,double *acm)
{
	long days;

	RespCode = pub_base_CalDays(lst_date,g_pub_tx.tx_date,"0",&days );
	if ( RespCode )
	{
		sprintf(acErrMsg,"调用计算天数函数错误!");
		WRITEMSG
		return 1;
	}
	*acm+=bal*days;

	return 0;
}
pubf_cal_od_acm(long pay_date,double amt,double *acm)
{
	long days;

	RespCode = pub_base_CalDays(g_pub_tx.tx_date,pay_date,"0",&days );
	if ( RespCode )
	{
		sprintf(acErrMsg,"调用计算天数函数错误!");
		WRITEMSG
		return 1;
	}
	*acm+=amt*days;

	return 0;
}

pubf_od_pay_date(pay_day,pay_intvl,pay_drt_days,pay_date)
long pay_day;
long pay_intvl;
long pay_drt_days;
long *pay_date;
{
	long year,day;
	long tmp_date;
	long tmp_date1;

	day=g_pub_tx.tx_date%100;
	year=g_pub_tx.tx_date/100;
	tmp_date=year*100+pay_day;
	if ( day>=pay_day )
	{
		if ( pub_base_deadlineM ( tmp_date , 1 , &tmp_date1 ) )
		{
			sprintf(acErrMsg,"调用pub_base_deadlineM错误!");
			WRITEMSG
			return 1;
		}
		tmp_date=tmp_date1;
	}

	if ( pay_intvl >0 )
	{
		if ( pub_base_deadlineM ( tmp_date , pay_intvl , &tmp_date1 ) )
		{
			sprintf(acErrMsg,"调用pub_base_deadlineM错误!");
			WRITEMSG
			return 1;
		}
		tmp_date=tmp_date1;
	}
	*pay_date=tmp_date;

	return 0;
}

pubf_cal_od_intst(bal,intst_acm,intst_bal,intst_amt,lst_date)
double bal;
double intst_acm;
double *intst_bal;
double *intst_amt;
long lst_date;
{
	double rate_val;
	struct mo_def_rate_c def_rate;
	double tmp_bal;
	int flag;
	long days;

	memset(&def_rate,0x00,sizeof(struct mo_def_rate_c));

	RespCode = pub_base_CalDays(lst_date,g_pub_tx.tx_date,"0",&days );
	if ( RespCode )
	{
		sprintf(acErrMsg,"调用计算天数函数错误!");
		WRITEMSG
		return 1;
	}
	switch(od_parm.rate_mode[0])
	{
		case 'F':
			if (pub_base_getllz(od_parm.rate_code,od_parm.cur_no,
				g_pub_tx.tx_date,&rate_val))
			{
				sprintf(acErrMsg,"调用pub_base_getllz错误!");
				WRITEMSG
				strcpy ( RtCode , g_pub_tx.reply );
				return 1;
			}
			*intst_bal+=pub_base_PubDround_1(intst_acm*rate_val);
			*intst_amt+=pub_base_PubDround_1(intst_acm*rate_val);
			break;
		case 'S':
			if(Mo_def_rate_Dec_Sel(RtCode,"chrg_code='%s' order by min_amt",
				od_parm.rate_code_def))
			{
				sprintf(acErrMsg,"定义mo_def_rate错误!");
				WRITEMSG
				strcpy ( RtCode , "T026" );
				return 1;
			}
			flag=0;
			while(1)
			{
				RespCode=Mo_def_rate_Fet_Sel(&def_rate,RtCode);
				if ( RespCode <0 )
				{
					Mo_def_rate_Clo_Sel();
					sprintf(acErrMsg,"取mo_def_rate错误!");
					WRITEMSG
					strcpy ( RtCode , "T025" );
					return 1;
				}
				else
				{
					Mo_def_rate_Clo_Sel();
					if ( flag==0 )
					{
						sprintf(acErrMsg,"取mo_def_rate错误!");
						WRITEMSG
						strcpy ( RtCode , "T024" );
						return 1;
					}
					break;
				}
				flag=1;
				if (pub_base_getllz(def_rate.chrg_rate_no,od_parm.cur_no,
					g_pub_tx.tx_date,&rate_val))
				{
					sprintf(acErrMsg,"调用pub_base_getllz错误!");
					WRITEMSG
					strcpy ( RtCode , g_pub_tx.reply );
					return 1;
				}
				tmp_bal=bal<=def_rate.max_amt?bal:def_rate.max_amt;
				*intst_bal+=pub_base_PubDround_1(tmp_bal*days*rate_val);
				*intst_amt+=pub_base_PubDround_1(tmp_bal*days*rate_val);
				if ( bal <= def_rate.max_amt )
				{
					Mo_def_rate_Clo_Sel();
					break;
				}
			}
			break;
		case 'T':
			RespCode= Mo_def_rate_Sel (RtCode,&def_rate,
				"chrg_code='%s' and min_amt<=%lf and max_amt>=%lf",
				od_parm.rate_code_def,bal,bal);
			if ( RespCode < 0 )
			{
				sprintf(acErrMsg,"取mo_def_rate错误!");
				WRITEMSG
				strcpy ( RtCode , "T025" );
				return 1;
			}
			else if ( RespCode )
			{
				sprintf(acErrMsg,"取mo_def_rate错误!");
				WRITEMSG
				strcpy ( RtCode , "T024" );
				return 1;
			}
			if (pub_base_getllz(def_rate.chrg_rate_no,od_parm.cur_no,
				g_pub_tx.tx_date,&rate_val))
			{
				sprintf(acErrMsg,"调用pub_base_getllz错误!");
				WRITEMSG
				strcpy ( RtCode , g_pub_tx.reply );
				return 1;
			}
			*intst_bal+=pub_base_PubDround_1(bal*days*rate_val);
			*intst_amt+=pub_base_PubDround_1(bal*days*rate_val);
			break;
	}

	return 0;
}
pubf_od_hst(od_mst,amt,add_ind,brf)
struct od_mst_c *od_mst;
double amt;
char *add_ind;
char *brf;
{
	struct od_mst_hst_c od_hst;

	od_hst.trace_no = g_pub_tx.trace_no ;
	od_hst.trace_cnt = g_pub_tx.trace_cnt ;
	strcpy ( od_hst.opn_br_no , od_mst->opn_br_no );
	od_hst.ac_id=od_mst->ac_id;
	od_hst.ac_seqn=od_mst->ac_seqn;
	strcpy ( od_hst.tx_code , g_pub_tx.tx_code );
	strcpy ( od_hst.sub_tx_code , g_pub_tx.sub_tx_code );
	strcpy ( od_hst.add_ind , add_ind );
	strcpy ( od_hst.ct_ind , g_pub_tx.ct_ind );
	od_hst.tx_bal=amt;
	od_hst.bal=od_mst->bal;
	od_hst.intst_acm=od_mst->intst_acm;
	od_hst.tx_date=g_pub_tx.tx_date;
	od_hst.tx_time=g_pub_tx.tx_time;
	od_hst.hst_cnt=od_mst->hst_cnt++;
	strcpy ( od_hst.brf , brf );
	strcpy ( od_hst.tel , g_pub_tx.tel );
	strcpy ( od_hst.chk , g_pub_tx.chk );
	strcpy ( od_hst.auth , g_pub_tx.auth );

	if ( Od_mst_hst_Ins ( od_hst , RtCode ) )
	{
		sprintf(acErrMsg,"写透支历史文件错误!");
		WRITEMSG
		strcpy ( RtCode , "T028" );
		return 1;
	}

	return 0;
}
