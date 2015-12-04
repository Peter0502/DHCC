/* *****************************************************************************/
/*  Function Name: iGetAreaTax()                                               */
/*  Description  : 遇利息税调整倒算利息税(从交易日倒算到99年)                  */
/*  param        :                                                             */
/* *****************************************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "find_debug.h"
#include "com_intst_tax_rate_c.h"
#include "mo_int_tax_hst_c.h"
#include "td_mst_hst_c.h"
#include "com_sys_parm_c.h"
long ori_tx_date;
int iGetAreaTax(double *val,double *factval,double *dealval,char *intst_term_type,int acttype,double ll_ddval,double ll_hhval,char *edu_ind)
{
	struct com_intst_tax_rate_c g_com_intst_tax_rate;
	struct mo_int_tax_hst_c     g_mo_int_tax_hst;
	struct td_mst_hst_c         g_td_mst_hst;
	int i=0,ret=0,hst_cnt=0;
        long daytermval=0/* 计息的天数 */,tot_cnt=0/*总明细笔数*/;
	long hq_dayterm=0,dq_dayterm=0;	/* 分段的活期和定期的天数 */
	long date99=19991101,hst_tx_date=0;/* 明细记录的交易日期 */
	char   llbh[3]; /* 利率编号 */ 	char jsts[10];/* 计算天数的方式,0实际  1 30天 */
	double rate_val=0,hq_amtval=0,dq_amtval=0,lxs=0,dq_lx=0,hq_lx=0,dq_acm=0,hq_acm=0; 
	struct com_sys_parm_c tmp_com_sys_parm;

	memset(jsts,'\0',sizeof(jsts)); strcpy(jsts,intst_term_type);
	memset(&g_mo_int_tax_hst,'\0',sizeof(g_mo_int_tax_hst));

	vtcp_log("注意: 进入重新计算利息税函数[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("注意: 传进来的应付利息[%.2f],实付利息[%.2f],利息税[%.2f],计算天数的方式[%s],定期利率[%.5f],活期利率[%.5f],教育证明[%s]\n",*dealval,*factval,*val,jsts,ll_ddval,ll_hhval,edu_ind);
	/* 对于自动转存户 pub_tx.tx_date 会变,所以交易日期只能从数据库里取 */
	memset(&tmp_com_sys_parm,'\0',sizeof(tmp_com_sys_parm));
	if( pub_base_GetSysparm( &tmp_com_sys_parm)){
		vtcp_log("[%s][%d]取公共参数表中时间出错\n",__FILE__,__LINE__);
		return (-1);
	}
	ori_tx_date=tmp_com_sys_parm.sys_date;	

	ret = Com_intst_tax_rate_Dec_Sel(g_pub_tx.reply,"cur_no = '%s' and end_date > %ld \
		order by beg_date ", g_pub_tx.cur_no,g_td_mst.ic_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "prepare com_intst err [%d] ", ret);
		WRITEMSG
		return ret;
	}
switch(acttype){
		case 0:		/* 整整 */
	/* 未到期那么按活期算应缴税的利息 */
	if(g_td_mst.mtr_date>g_pub_tx.tx_date){
		ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"计算天数错误!!");
			WRITEMSG
			return 1;
		}

		/* 取活期利率 */
		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,HQLLBH);
sprintf(acErrMsg,"活期 : 利率编号=[%s],币种=[%s],交易日期=[%ld]",
        llbh,g_pub_tx.cur_no,g_pub_tx.tx_date);
WRITEMSG

		ret = pub_base_getllz(llbh,g_pub_tx.cur_no,
		g_pub_tx.tx_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W110");
			return ret;
		}

		hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
		hq_lx = pub_base_PubDround_1(hq_lx) ;

		if (floor(hq_lx * 100) / 100 < 0.005 )
		{
			hq_lx = 0.00;
			(*val) = 0.00;
		}
sprintf(acErrMsg,"缴税天数=[%d],计息利率=[%lf],本金=[%lf],浮动比=[%lf]",hq_dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

		/* 计算个人所得税 */
		if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
			vtcp_log("[%s][%d]取分段税率表税率",__FILE__,__LINE__);
			while(1){
				memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
				ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
				if(ret==100)
					break;
				if (ret)
				{
					sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
					WRITEMSG
					return ret;
				}

				ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); /* 活期 */
			   if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				vtcp_log("[%s][%d]分段天数[%d]\n",__FILE__,__LINE__,daytermval);
				if(hq_dayterm>0){	
					vtcp_log("分母不能为0 [%d]\n",hq_dayterm);
					hq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
				}

				/* 准备明细 */
				g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
				g_mo_int_tax_hst.rate=rate_val * (1 + g_td_mst.flt_ratio) ;/* 利率 */
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;/* 利息 */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;	/* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
			Com_intst_tax_rate_Clo_Sel();

				lxs= hq_lx * hq_amtval;
			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		}
	}
	else{	/* 到期或逾期 */
		ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_td_mst.mtr_date,intst_term_type,&dq_dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"计算定期天数错误!!");
			WRITEMSG
			return 1;
		}

		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,g_td_parm.rate_no);

		ret = pub_base_getllz(llbh,g_td_parm.cur_no,g_td_mst.ic_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W110");
			return ret;
		}
		double tmp_rate_val; tmp_rate_val=rate_val;
		dq_lx = dq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 
			* (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"应缴税的定期利息[%.2f] : 利率编号=[%s],币种=[%s],计息天数=[%ld],利率[%.5f]",
        dq_lx,llbh,g_pub_tx.cur_no,dq_dayterm,rate_val);
WRITEMSG
		
		/* 应缴税活期利息计算 */
		hq_dayterm=0;
		if (g_td_parm.ovrd_intst_ind[0] == 'Y'){
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.mtr_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"计算活期天数错误!!");
				WRITEMSG
				return 1;
			}
		}
		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,HQLLBH);

		ret = pub_base_getllz(llbh,g_pub_tx.cur_no,g_pub_tx.tx_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W110");
			return ret;
		}

		hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;

sprintf(acErrMsg,"应缴税的活期利息[%.2f] : 本金[%.2f],计息天数=[%ld],利率[%.5f]",
        hq_lx,g_pub_tx.tx_amt1,hq_dayterm,rate_val);
WRITEMSG

		/* 计算个人所得税 */
		if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
			vtcp_log("取分段税率表税率");
			while(1){
				memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
				ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
				if(ret==100)
					break;
				if (ret)
				{
					sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
					WRITEMSG
					return ret;
				}
				if(g_td_mst.mtr_date > g_com_intst_tax_rate.beg_date){
					ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
					dq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / dq_dayterm;

					vtcp_log("[%s][%d]定期分段天数[%d],税率[%.5f],定期所占的百分比[%.5f]\n",__FILE__,__LINE__,daytermval,g_com_intst_tax_rate.tax_rate_val,dq_amtval);

					/* 准备明细 */
					g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
					g_mo_int_tax_hst.end_date=PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date);
					g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
					g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
					g_mo_int_tax_hst.rate=tmp_rate_val * (1 + g_td_mst.flt_ratio) ;/*利率 */
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
				if (g_td_parm.ovrd_intst_ind[0] == 'Y'){	/* 逾期部分计算利息 */
					if(g_td_mst.mtr_date < g_com_intst_tax_rate.end_date){
						ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
						if(hq_dayterm>0){	/* 分母不能为0 */
							vtcp_log("分母不能为0[%d]\n",hq_dayterm);
							hq_amtval +=g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
						}

						vtcp_log("[%s][%d]活期分段天数[%d]\n",__FILE__,__LINE__,daytermval);

						/* 准备明细 */
						g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date);
						g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
						g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
						g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
						g_mo_int_tax_hst.rate=rate_val*(1+g_td_mst.flt_ratio);/*利率 */
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
						if(ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
				if (ret != 0){
					sprintf(acErrMsg,"计算分段天数错误!!");
					WRITEMSG
					return 1;
				}
			}
			Com_intst_tax_rate_Clo_Sel();

				lxs= dq_lx * dq_amtval + hq_lx * hq_amtval;
vtcp_log("[%s][%d]定期利息[%.2f],百分比[%.5f],活期利息[%.2f],百分比[%.5f]\n",__FILE__,__LINE__,dq_lx,dq_amtval,hq_lx,hq_amtval);
			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		}
	}
	vtcp_log("[%s][%d]分段算后应缴税利息[%.2f],税金[%.2f]\n",__FILE__,__LINE__,dq_lx+hq_lx,*val);
	return(0);

	case 1:		/* 零整 */
		while(1){
			memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
			ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
			if(ret==100)
				break;
			if (ret)
			{
				sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
				WRITEMSG
				return ret;
			}
			vtcp_log("[%s][%d]用分段起[%ld],止[%ld]查零整明细\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date < %ld order by tx_date ",g_td_mst.ac_id,g_td_mst.ac_seqn,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "prepare td_mst_hst err [%d] ", ret);
				WRITEMSG
				return ret;
			}
			hst_cnt=1; dq_acm=0;hq_acm=0;hst_tx_date=0;
			while(1){
				memset(&g_td_mst_hst,'\0',sizeof(g_td_mst_hst));
				ret = Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
				if (ret && ret!=100)
				{
					sprintf(acErrMsg, "fet td_mst_hst err [%d] ", ret);
					WRITEMSG
					return ret;
				}
				if(ret==100){
					if(tot_cnt==0){
						vtcp_log("[%s][%d]这个户[%ld]没有明细!!!没法分段算税,按外面函数算的吧\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if(hst_cnt==1){
					vtcp_log("每次分段的第一笔的积数＝上次的余额*天数");
					if(g_td_mst.ic_date==g_td_mst_hst.tx_date){
						vtcp_log("此户的第一笔明细[%ld],用if外的发生额\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
					vtcp_log("[%s][%d]临界点的起[%ld],天数[%d]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0){
						vtcp_log("[%d]销户那笔记录[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else 
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]分段起[%ld],天数[%d],积数[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++; tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if(g_pub_tx.tx_date>=g_td_mst.mtr_date){
				vtcp_log("开始计算定期部分利息税[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"应缴税的定期税[%.2f] : 币种=[%s],积数=[%.2f],利率[%.5f]",
			lxs,g_pub_tx.cur_no,dq_acm,ll_ddval);
WRITEMSG
				/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=dq_acm;
                g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
                g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
                g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
                if(ret=insert_hst(&g_mo_int_tax_hst))
                    return(ret);

				if(g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("最后一笔交易完后又做了利息税的调整,帐户从调整到到期日间的利息税\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
				lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("调整后的利息税[%.2f],天数[%d]\n",lxs,daytermval);
					/* 准备明细 */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else{
				vtcp_log("[%s][%d]提前支取按活期算息\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if(hst_cnt==1 && tot_cnt>0){
					vtcp_log("[%s][%d]提前支取,历史明细里没有这段税率区间[%ld][%ld]的记录,用余额算",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("天数[%d],本金[%.2f],积数[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"提前支付活期利息税[%.2f] : 提前支取活期积数[%.2f],天数=[%ld],利率[%.5f]",
			lxs,dq_acm,daytermval,ll_hhval);
WRITEMSG
					/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
				/* 到期后活期部分又分段 */
				/* 因为计算天数函数会判断起止日所以不会重复计算逾期部分的活期利息 */
	  		if(g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]到期后的活期部分还可以分段\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]到期日－分段止日[%ld]的天数[%d],活期积数[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

				lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"整个利息税[%.2f] : 逾期部分积数[%.2f],天数=[%ld],利率[%.5f]",
			lxs,hq_acm,daytermval,ll_hhval);
WRITEMSG
					/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
		}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
vtcp_log("[%s][%d]分段算后税金[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	case 6:		/* 定活两便 */
		vtcp_log("取分段税率表税率");
			while(1){
				memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
				ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
				if(ret==100)
					break;
				if (ret)
				{
					sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
					WRITEMSG
					return ret;
				}
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 

				lxs +=  daytermval * ll_ddval * floor(g_pub_tx.tx_amt1*L100)/L100  
					* (1 + g_td_mst.flt_ratio) / L360 / L100 * g_com_intst_tax_rate.tax_rate_val;

				vtcp_log("[%s][%d]分段天数[%d]\n",__FILE__,__LINE__,daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算分段天数错误!!");
					WRITEMSG
					return 1;
				}
				/* 准备明细 */
				g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
				g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);/*利率 */
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
			Com_intst_tax_rate_Clo_Sel();

			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		return(0);
	case 7:		/* 教育储蓄 */
		while(1){
			memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
			ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
			if(ret==100)
				break;
			if (ret)
			{
				sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
				WRITEMSG
				return ret;
			}
		if(g_pub_tx.tx_date>g_td_mst.mtr_date && edu_ind[0]=='Y'){
				vtcp_log("[%s][%d]逾期且有教育证明那么只收逾期部分利息税\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				lxs +=g_td_mst.bal * daytermval * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"应缴的税[%.2f] :本金=[%.2f],利率[%.5f],天数[%d]",
			lxs,g_td_mst.bal,ll_hhval,daytermval);
WRITEMSG
				/* 准备明细 */
				g_mo_int_tax_hst.beg_date=PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /*利息*/
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
		}
		else
		{
			vtcp_log("没到期或者没教育证明那么算法同零整,按明细分段算\n");
			vtcp_log("[%s][%d]用分段起[%ld],止[%ld]查教育明细\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date < %ld order by tx_date ",g_td_mst.ac_id,g_td_mst.ac_seqn,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "prepare td_mst_hst err [%d] ", ret);
				WRITEMSG
				return ret;
			}
			hst_cnt=1; dq_acm=0;hq_acm=0;hst_tx_date=0;
			while(1){
				memset(&g_td_mst_hst,'\0',sizeof(g_td_mst_hst));
				ret = Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
				if (ret && ret!=100)
				{
					sprintf(acErrMsg, "fet td_mst_hst err [%d] ", ret);
					WRITEMSG
					return ret;
				}
				if(ret==100){
					if(tot_cnt==0){
						vtcp_log("[%s][%d]这个户[%ld]没有明细!!!没法分段算税,按外面函数算的吧\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if(hst_cnt==1){
					vtcp_log("每次分段的第一笔的积数＝上次的余额*天数");
					if(g_td_mst.ic_date==g_td_mst_hst.tx_date){
						vtcp_log("此户的第一笔明细[%ld],用if外的发生额\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
					vtcp_log("[%s][%d]临界点的起[%ld],天数[%d]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0){
						vtcp_log("[%d]销户那笔记录[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]分段起[%ld],天数[%d],积数[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++;tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if(g_pub_tx.tx_date>=g_td_mst.mtr_date){
				vtcp_log("开始计算定期部分利息税[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"应缴的定期税[%.2f] : 积数=[%.2f],利率[%.5f]",
			lxs,dq_acm,ll_ddval);
WRITEMSG
				/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=dq_acm;
                g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
                g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
                g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
                if(ret=insert_hst(&g_mo_int_tax_hst))
                    return(ret);

				if(g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("最后一笔交易完后又做了利息税的调整,帐户从调整到到期日间的利息税\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
				lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("调整后的利息税[%.2f],天数[%d]\n",lxs,daytermval);
					/* 准备明细 */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else{
				vtcp_log("[%s][%d]提前支取按活期算息\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if(hst_cnt==1 && tot_cnt>0){
					vtcp_log("[%s][%d]提前支取,历史明细里没有这段税率区间[%ld][%ld]的记录,用余额算",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0){
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
						return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("天数[%d],本金[%.2f],积数[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"提前支付活期利息税[%.2f] : 提前支取活期积数[%.2f],天数=[%ld],利率[%.5f]",
			lxs,dq_acm,daytermval,ll_hhval);
WRITEMSG
				/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
				/* 到期后活期部分又分段 */
				/* 因为计算天数函数会判断起止日所以不会重复计算逾期部分的活期利息 */
	  		if(g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]到期后的活期部分还可以分段\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
					return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]分段起－分段止日[%ld]的天数[%d],活期积数[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

					lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"整个利息税[%.2f] : 逾期部分积数[%.2f],天数=[%ld],利率[%.5f]",
			lxs,hq_acm,daytermval,ll_hhval);
WRITEMSG
				/* 准备明细 */
				g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
		}
	}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
vtcp_log("[%s][%d]分段算后税金[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
}

}
insert_hst(struct mo_int_tax_hst_c *g_mo_int_tax_hst){
	int ret=0;
	struct mo_int_tax_hst_c tmp_mo_int_tax_hst;
	memset(&tmp_mo_int_tax_hst,'\0',sizeof(tmp_mo_int_tax_hst));

	strcpy(g_mo_int_tax_hst->ac_no,g_pub_tx.ac_no);
	g_mo_int_tax_hst->ac_id=g_pub_tx.ac_id;
	g_mo_int_tax_hst->ac_seqn=g_pub_tx.ac_seqn;
	strcpy(g_mo_int_tax_hst->tx_code,g_pub_tx.tx_code);
	strcpy(g_mo_int_tax_hst->tx_br_no,g_pub_tx.tx_br_no);
	g_mo_int_tax_hst->tx_date=ori_tx_date;
	g_mo_int_tax_hst->tx_time=g_pub_tx.tx_time;
	g_mo_int_tax_hst->trace_no=g_pub_tx.trace_no;
	g_mo_int_tax_hst->trace_cnt=g_pub_tx.trace_cnt;
	strcpy(g_mo_int_tax_hst->tel,g_pub_tx.tel);
	memcpy(&tmp_mo_int_tax_hst,g_mo_int_tax_hst,sizeof(tmp_mo_int_tax_hst));
	ret=Mo_int_tax_hst_Ins(tmp_mo_int_tax_hst,g_pub_tx.reply);
	if(ret){
		vtcp_log("插入分段利息税历史表错[%d]\n",ret);
		Mo_int_tax_hst_Debug(g_mo_int_tax_hst);
		return(ret);
	}
	return(0);
}

int iGetAreaTax_ZCZQ_TK(long real_tx_date,double *val,double *factval,double *dealval,char *intst_term_type,int acttype,double ll_ddval,double ll_hhval,char *edu_ind,int ss_flag)
{
	vtcp_log("注意: 进入重新计算利息税函数[%s][%d]intst_term_type===[%s]\n",__FILE__,__LINE__,intst_term_type);

	struct com_intst_tax_rate_c g_com_intst_tax_rate;
	struct mo_int_tax_hst_c     g_mo_int_tax_hst;
	struct td_mst_hst_c         g_td_mst_hst;
	int i=0,ret=0,hst_cnt=0;
	long daytermval=0/* 计息的天数 */,tot_cnt=0/*总明细笔数*/;
	long hq_dayterm=0,dq_dayterm=0;	/* 分段的活期和定期的天数 */
	long date99=19991101,hst_tx_date=0;/* 明细记录的交易日期 */
	char   llbh[3]; /* 利率编号 */ 	char jsts[10];/* 计算天数的方式,0实际  1 30天 */
	double rate_val=0,hq_amtval=0,dq_amtval=0,lxs=0,dq_lx=0,hq_lx=0,dq_acm=0,hq_acm=0; 
	struct com_sys_parm_c tmp_com_sys_parm;
 /* intst_term_type[0]='0';*/
	memset(&g_com_intst_tax_rate,'\0',sizeof(struct com_intst_tax_rate_c));
	memset(jsts,'\0',sizeof(jsts)); strcpy(jsts,intst_term_type);
	memset(&g_mo_int_tax_hst,'\0',sizeof(struct mo_int_tax_hst_c));
	memset(&g_td_mst_hst,'\0',sizeof(struct td_mst_hst_c));

	vtcp_log("注意: 进入重新计算利息税函数[%s][%d]aaaaaa%s\n",__FILE__,__LINE__,intst_term_type);
	vtcp_log("注意: 传进来的实付利息[%.2f]\n",*factval);
	vtcp_log("注意: 传进来的教育证明[%s]\n",edu_ind);
	vtcp_log("注意: 传进来的利息税[%.2f]\n",*val);
	vtcp_log("注意: 进入重新计算利息税函数[%s][%d]aaaaaa%s\n",__FILE__,__LINE__,intst_term_type);
	vtcp_log("注意: 传进来的计算天数的方式[%s]\n",jsts);
	vtcp_log("注意: 传进来的定期利率[%.5f]\n",ll_ddval);
	vtcp_log("注意: 传进来的活期利率[%.5f]\n",ll_hhval);
	vtcp_log("注意: 传进来的应付利息[%.2f]\n",*dealval);
	/* 对于自动转存户 pub_tx.tx_date 会变,所以交易日期只能从数据库里取 */
	memset(&tmp_com_sys_parm,'\0',sizeof(tmp_com_sys_parm));
	if ( pub_base_GetSysparm( &tmp_com_sys_parm))
	{
		vtcp_log("[%s][%d]取公共参数表中时间出错\n",__FILE__,__LINE__);
		return (-1);
	}
	ori_tx_date=tmp_com_sys_parm.sys_date;	

	ret = Com_intst_tax_rate_Dec_Sel(g_pub_tx.reply,"cur_no = '%s' and end_date > %ld \
		order by beg_date ", g_pub_tx.cur_no,g_td_mst.ic_date);
	if (ret != 0)
	{
		sprintf(acErrMsg, "prepare com_intst err [%d] ", ret);
		WRITEMSG
			return ret;
	}
					vtcp_log("[%s][%d]取分段税率表税率[%ld]",__FILE__,__LINE__,g_td_mst.ic_date);

	switch(acttype)
	{
	case 0:		/* 整整 */
		/* 未到期那么按活期算应缴税的利息 */
		if (g_td_mst.mtr_date>g_pub_tx.tx_date)
		{
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"计算天数错误!!");
				WRITEMSG
					return 1;
			}
			/* 取活期利率 */
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,HQLLBH);
			sprintf(acErrMsg,"活期 : 利率编号=[%s],币种=[%s],交易日期=[%ld]",
				llbh,g_pub_tx.cur_no,g_pub_tx.tx_date);
			WRITEMSG

				ret = pub_base_getllz(llbh,g_pub_tx.cur_no,
				real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
				WRITEMSG
					strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			hq_lx = pub_base_PubDround_1(hq_lx) ;

			if (floor(hq_lx * 100) / 100 < 0.005 )
			{
				hq_lx = 0.00;
				(*val) = 0.00;
			}
			sprintf(acErrMsg,"缴税天数=[%ld],计息利率=[%lf],本金=[%lf],浮动比=[%lf]",hq_dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
			WRITEMSG

			/* 计算个人所得税 */
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
			{
				vtcp_log("[%s][%d]取分段税率表税率",__FILE__,__LINE__);
				while (1)
				{
					memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
					ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
					if (ret==100)
						break;
					if (ret)
					{
						sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
						WRITEMSG
							return ret;
					}
					ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); /* 活期 */
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					vtcp_log("[%s][%d]分段天数[%ld]\n",__FILE__,__LINE__,daytermval);
					if (hq_dayterm>0)
					{	
						vtcp_log("分母不能为0 [%ld]\n",hq_dayterm);
						hq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
					}
					/* 准备明细 */
					g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
					g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
					g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
					g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
					g_mo_int_tax_hst.rate=rate_val * (1 + g_td_mst.flt_ratio) ;/* 利率 */
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;/* 利息 */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;	/* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
				Com_intst_tax_rate_Clo_Sel();

				lxs= hq_lx * hq_amtval;
				(*val) = pub_base_PubDround_1(lxs);
				(*factval) = (*dealval) -(*val);
			}
		}
		else
		{	/* 到期或逾期 */
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_td_mst.mtr_date,intst_term_type,&dq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"计算定期天数错误!!");
				WRITEMSG
					return 1;
			}
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,g_td_parm.rate_no);

			ret = pub_base_getllz(llbh,g_td_parm.cur_no,real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
				WRITEMSG
					strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			double tmp_rate_val; tmp_rate_val=rate_val;
			dq_lx = dq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			sprintf(acErrMsg,"应缴税的定期利息[%.2f] : 利率编号=[%s],币种=[%s],计息天数=[%ld],利率[%.5f]",
				dq_lx,llbh,g_pub_tx.cur_no,dq_dayterm,rate_val);
			WRITEMSG

			/* 应缴税活期利息计算 */
				hq_dayterm=0;
			if (g_td_parm.ovrd_intst_ind[0] == 'Y')
			{
				ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.mtr_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
				if (ret != 0)
				{
					sprintf(acErrMsg,"计算活期天数错误!!");
					WRITEMSG
						return 1;
				}
			}
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,HQLLBH);

			ret = pub_base_getllz(llbh,g_pub_tx.cur_no,real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
				WRITEMSG
					strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;

			sprintf(acErrMsg,"应缴税的活期利息[%.2f] : 本金[%.2f],计息天数=[%ld],利率[%.5f]",
				hq_lx,g_pub_tx.tx_amt1,hq_dayterm,rate_val);
			WRITEMSG

			/* 计算个人所得税 */
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
			{
				vtcp_log("取分段税率表税率");
				while (1)
				{
					memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
					ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
					if (ret==100)
						break;
					if (ret)
					{
						sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
						WRITEMSG
							return ret;
					}
					if (g_td_mst.mtr_date > g_com_intst_tax_rate.beg_date)
					{
						ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
						dq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / dq_dayterm;

						vtcp_log("[%s][%d]定期分段天数[%ld],税率[%.5f],定期所占的百分比[%.5f]\n",__FILE__,__LINE__,daytermval,g_com_intst_tax_rate.tax_rate_val,dq_amtval);

						/* 准备明细 */
						g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
						g_mo_int_tax_hst.end_date=PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date);
						g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
						g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
						g_mo_int_tax_hst.rate=tmp_rate_val * (1 + g_td_mst.flt_ratio) ;/*利率 */
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
						if(ss_flag != 1)
						{
							if (ret=insert_hst(&g_mo_int_tax_hst))
								return(ret);
						}
					}
					if (g_td_parm.ovrd_intst_ind[0] == 'Y')
					{	/* 逾期部分计算利息 */
						if (g_td_mst.mtr_date < g_com_intst_tax_rate.end_date)
						{
							ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
							if (hq_dayterm>0)
							{	/* 分母不能为0 */
								vtcp_log("分母不能为0[%ld]\n",hq_dayterm);
								hq_amtval +=g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
							}
							vtcp_log("[%s][%d]活期分段天数[%ld]\n",__FILE__,__LINE__,daytermval);

							/* 准备明细 */
							g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date);
							g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
							g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
							g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
							g_mo_int_tax_hst.rate=rate_val*(1+g_td_mst.flt_ratio);/*利率 */
							g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
							g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
							g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
							if(ss_flag != 1)
							{
								if (ret=insert_hst(&g_mo_int_tax_hst))
									return(ret);
							}
						}
					}
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算分段天数错误!!");
						WRITEMSG
							return 1;
					}
				}
				Com_intst_tax_rate_Clo_Sel();

				lxs= dq_lx * dq_amtval + hq_lx * hq_amtval;
				vtcp_log("[%s][%d]定期利息[%.2f],百分比[%.5f],活期利息[%.2f],百分比[%.5f]\n",__FILE__,__LINE__,dq_lx,dq_amtval,hq_lx,hq_amtval);
				(*val) = pub_base_PubDround_1(lxs);
				(*factval) = (*dealval) -(*val);
			}
		}
		vtcp_log("[%s][%d]分段算后应缴税利息[%.2f],税金[%.2f]\n",__FILE__,__LINE__,dq_lx+hq_lx,*val);
		return(0);

	case 1:		/* 零整 */
		while (1)
		{
			memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
			ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
			if (ret==100)
				break;
			if (ret)
			{
				sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
				WRITEMSG
					return ret;
			}
			vtcp_log("[%s][%d]用分段起[%ld],止[%ld]查零整明细\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date < %ld order by tx_date ",g_td_mst.ac_id,g_td_mst.ac_seqn,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
			if (ret != 0)
			{
				sprintf(acErrMsg, "prepare td_mst_hst err [%d] ", ret);
				WRITEMSG
					return ret;
			}
			hst_cnt=1; dq_acm=0;hq_acm=0;hst_tx_date=0;
			while (1)
			{
				memset(&g_td_mst_hst,'\0',sizeof(g_td_mst_hst));
				ret = Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
				if (ret && ret!=100)
				{
					sprintf(acErrMsg, "fet td_mst_hst err [%d] ", ret);
					WRITEMSG
						return ret;
				}
				if (ret==100)
				{
					if (tot_cnt==0)
					{
						vtcp_log("[%s][%d]这个户[%ld]没有明细!!!没法分段算税,按外面函数算的吧\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if (hst_cnt==1)
				{
					vtcp_log("每次分段的第一笔的积数＝上次的余额*天数");
					if (g_td_mst.ic_date==g_td_mst_hst.tx_date)
					{
						vtcp_log("此户的第一笔明细[%ld],用if外的发生额\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else
					{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					vtcp_log("[%s][%d]临界点的起[%ld],天数[%ld]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if ( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
						pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0)
					{
						vtcp_log("[%d]销户那笔记录[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else 
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
						return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]分段起[%ld],天数[%ld],积数[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++; tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if (g_pub_tx.tx_date>=g_td_mst.mtr_date)
			{
				vtcp_log("开始计算定期部分利息税[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"应缴税的定期税[%.2f] : 币种=[%s],积数=[%.2f],利率[%.5f]",
					lxs,g_pub_tx.cur_no,dq_acm,ll_ddval);
				WRITEMSG
				/* 准备明细 */
					g_mo_int_tax_hst.intst_acm=dq_acm;
				g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}

				if (g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("最后一笔交易完后又做了利息税的调整,帐户从调整到到期日间的利息税\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("调整后的利息税[%.2f],天数[%ld]\n",lxs,daytermval);
					/* 准备明细 */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
			}
			else
			{
				vtcp_log("[%s][%d]提前支取按活期算息\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if (hst_cnt==1 && tot_cnt>0)
				{
					vtcp_log("[%s][%d]提前支取,历史明细里没有这段税率区间[%ld][%ld]的记录,用余额算",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("天数[%d],本金[%.2f],积数[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"提前支付活期利息税[%.2f] : 提前支取活期积数[%.2f],天数=[%ld],利率[%.5f]",
					lxs,dq_acm,daytermval,ll_hhval);
				WRITEMSG
				/* 准备明细 */
					g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			/* 到期后活期部分又分段 */
			/* 因为计算天数函数会判断起止日所以不会重复计算逾期部分的活期利息 */
			if (g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]到期后的活期部分还可以分段\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
						return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]到期日－分段止日[%ld]的天数[%ld],活期积数[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

				lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"整个利息税[%.2f] : 逾期部分积数[%.2f],天数=[%ld],利率[%.5f]",
					lxs,hq_acm,daytermval,ll_hhval);
				WRITEMSG
				/* 准备明细 */
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ss_flag != 1)
				{
				if (ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
				}
			}
		}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
		vtcp_log("[%s][%d]分段算后税金[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	case 6:		/* 定活两便 */
		vtcp_log("取分段税率表税率");
		while (1)
		{
			memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
			ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
			if (ret==100)
				break;
			if (ret)
			{
				sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
				WRITEMSG
					return ret;
			}
			ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 

			lxs +=  daytermval * ll_ddval * floor(g_pub_tx.tx_amt1*L100)/L100  
				* (1 + g_td_mst.flt_ratio) / L360 / L100 * g_com_intst_tax_rate.tax_rate_val;

			vtcp_log("[%s][%d]分段天数[%ld]\n",__FILE__,__LINE__,daytermval);
			if (ret != 0)
			{
				sprintf(acErrMsg,"计算分段天数错误!!");
				WRITEMSG
					return 1;
			}
			/* 准备明细 */
			g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
			g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
			g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
			g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
			g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);/*利率 */
			g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
			g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
			g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
			if(ss_flag != 1)
			{
				if (ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
		}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
		return(0);
	case 7:		/* 教育储蓄 */
		while (1)
		{
			memset(&g_com_intst_tax_rate,'\0',sizeof(g_com_intst_tax_rate));
			ret = Com_intst_tax_rate_Fet_Sel(&g_com_intst_tax_rate,g_pub_tx.reply);
			if (ret==100)
				break;
			if (ret)
			{
				sprintf(acErrMsg, "fetch com_intst err [%d] ", ret);
				WRITEMSG
					return ret;
			}
			if (g_pub_tx.tx_date>g_td_mst.mtr_date && edu_ind[0]=='Y')
			{
				vtcp_log("[%s][%d]逾期且有教育证明那么只收逾期部分利息税\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"计算天数错误!!");
					WRITEMSG
						return 1;
				}
				lxs +=g_td_mst.bal * daytermval * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"应缴的税[%.2f] :本金=[%.2f],利率[%.5f],天数[%d]",
					lxs,g_td_mst.bal,ll_hhval,daytermval);
				WRITEMSG
				/* 准备明细 */
					g_mo_int_tax_hst.beg_date=PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* 积数 */
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /*利息*/
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else
			{
				vtcp_log("没到期或者没教育证明那么算法同零整,按明细分段算\n");
				vtcp_log("[%s][%d]用分段起[%ld],止[%ld]查教育明细\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
				ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and tx_date>=%ld and tx_date < %ld order by tx_date ",g_td_mst.ac_id,g_td_mst.ac_seqn,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
				if (ret != 0)
				{
					sprintf(acErrMsg, "prepare td_mst_hst err [%d] ", ret);
					WRITEMSG
						return ret;
				}
				hst_cnt=1; dq_acm=0;hq_acm=0;hst_tx_date=0;
				while (1)
				{
					memset(&g_td_mst_hst,'\0',sizeof(g_td_mst_hst));
					ret = Td_mst_hst_Fet_Sel(&g_td_mst_hst,g_pub_tx.reply);
					if (ret && ret!=100)
					{
						sprintf(acErrMsg, "fet td_mst_hst err [%d] ", ret);
						WRITEMSG
							return ret;
					}
					if (ret==100)
					{
						if (tot_cnt==0)
						{
							vtcp_log("[%s][%d]这个户[%ld]没有明细!!!没法分段算税,按外面函数算的吧\n",__FILE__,__LINE__,g_td_mst.ac_id);
							return(0);
						}
						break;
					}
					hst_tx_date=g_td_mst_hst.tx_date;
					if (hst_cnt==1)
					{
						vtcp_log("每次分段的第一笔的积数＝上次的余额*天数");
						if (g_td_mst.ic_date==g_td_mst_hst.tx_date)
						{
							vtcp_log("此户的第一笔明细[%ld],用if外的发生额\n",g_td_mst_hst.tx_date);
							daytermval=0;
						}
						else
						{
							ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
						}
						if (ret != 0)
						{
							sprintf(acErrMsg,"计算天数错误!!");
							WRITEMSG
								return 1;
						}
						vtcp_log("[%s][%d]临界点的起[%ld],天数[%ld]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
						if ( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0)
						{
							vtcp_log("[%d]销户那笔记录[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
							dq_acm += g_td_mst_hst.tx_amt * daytermval;
						}
						else
							dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
					}
					ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					dq_acm += g_td_mst_hst.tx_amt * daytermval;
					vtcp_log("[%s][%d]分段起[%ld],天数[%ld],积数[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
					hst_cnt++;tot_cnt++;
				}
				Td_mst_hst_Clo_Sel();

				if (g_pub_tx.tx_date>=g_td_mst.mtr_date)
				{
					vtcp_log("开始计算定期部分利息税[%s][%d]\n",__FILE__,__LINE__);

					lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"应缴的定期税[%.2f] : 积数=[%.2f],利率[%.5f]",
						lxs,dq_acm,ll_ddval);
					WRITEMSG
					/* 准备明细 */
						g_mo_int_tax_hst.intst_acm=dq_acm;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}

					if (g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
					{
						vtcp_log("最后一笔交易完后又做了利息税的调整,帐户从调整到到期日间的利息税\n");

						ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
						if (ret != 0)
						{
							sprintf(acErrMsg,"计算天数错误!!");
							WRITEMSG
								return 1;
						}
						lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
						vtcp_log("调整后的利息税[%.2f],天数[%ld]\n",lxs,daytermval);
						/* 准备明细 */
						g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
						g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
						g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
						if(ss_flag != 1)
						{
							if (ret=insert_hst(&g_mo_int_tax_hst))
								return(ret);
						}
					}
				}
				else
				{
					vtcp_log("[%s][%d]提前支取按活期算息\n",__FILE__,__LINE__);
					long tmp_days=0; double tmp_acm=0;
					if (hst_cnt==1 && tot_cnt>0)
					{
						vtcp_log("[%s][%d]提前支取,历史明细里没有这段税率区间[%ld][%ld]的记录,用余额算",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
						if (ret != 0)
						{
							sprintf(acErrMsg,"计算天数错误!!");
							WRITEMSG
								return 1;
						}
						tmp_acm=g_td_mst.bal * tmp_days;
						vtcp_log("天数[%d],本金[%.2f],积数[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
					}
					lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"提前支付活期利息税[%.2f] : 提前支取活期积数[%.2f],天数=[%ld],利率[%.5f]",
						lxs,dq_acm,daytermval,ll_hhval);
					WRITEMSG
					/* 准备明细 */
						g_mo_int_tax_hst.intst_acm=hq_acm;
					g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
				/* 到期后活期部分又分段 */
				/* 因为计算天数函数会判断起止日所以不会重复计算逾期部分的活期利息 */
				if (g_pub_tx.tx_date>=g_td_mst.mtr_date )
				{
					vtcp_log("[%s][%d]到期后的活期部分还可以分段\n",__FILE__,__LINE__);
					ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"计算天数错误!!");
						WRITEMSG
							return 1;
					}
					hq_acm += g_td_mst.bal * daytermval;
					vtcp_log("[%s][%d]分段起－分段止日[%ld]的天数[%ld],活期积数[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

					lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"整个利息税[%.2f] : 逾期部分积数[%.2f],天数=[%ld],利率[%.5f]",
						lxs,hq_acm,daytermval,ll_hhval);
					WRITEMSG
					/* 准备明细 */
						g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* 利息 */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* 利息税 */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* 税率 */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
			}
		}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
		vtcp_log("[%s][%d]分段算后税金[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	}
}
