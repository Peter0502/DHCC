/*************************************************
* 文 件 名:  sp8133.c
* 功能描述： 
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期: 20070417
* 修 改 人: gujy
* 修改内容: 整整转存试结利息查询
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "com_rate_c.h"
#include "find_debug.h"
#define DATE061104 19000101

int  whileCnt;
sp8133()
{
	struct td_mst_c f_td_mst;
	int             ttlnum = 0;	/**读取的总条数**/
	int             i = 0;
	int             ret = 0;
	FILE           *fp;
	int             cnt, tmp_cnt;
	char            cTypename[21];
	char            f_acno0[25];
	char 		filename[100];
	memset(cTypename, 0x0, sizeof(cTypename));
	
	whileCnt = 0;

	memset(&g_pub_tx, 0, sizeof(g_pub_tx));
vtcp_log("FUN%s->%d@%s \n", __FUNCTION__, __LINE__, __FILE__);
	pub_base_sysinit();
vtcp_log("FUN%s->%d@%s \n", __FUNCTION__, __LINE__, __FILE__);

	get_zd_data("0380", f_acno0);
	pub_base_old_acno(f_acno0);
	get_zd_long("0510", &g_td_mst.ac_seqn);
vtcp_log("FUN%s->%d@%s g_td_mst.ac_seqn = %ld  \n", __FUNCTION__, __LINE__, __FILE__, g_td_mst.ac_seqn);

	/**组成查询条件**/
	pub_base_strpack(f_acno0);
	if (strlen(f_acno0)) {
		if (pub_base_acno_acid(&g_td_mst.ac_id, f_acno0)) {
			goto ErrExit;
		}
	}
vtcp_log("FUN%s->%d@%s \n", __FUNCTION__, __LINE__, __FILE__);
	ttlnum = 0;
	ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "1=1 and ac_seqn = %ld and ac_id = %ld", g_td_mst.ac_seqn, g_td_mst.ac_id);
	ERR_DEAL
	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'", g_td_mst.prdt_no);
	ERR_DEAL

	if('0' != g_td_parm.td_type[0]){
		sprintf(acErrMsg, "非整存整取户![%c]", g_td_parm.td_type[0]);
                        WRITEMSG
		goto ErrExit;
	}
		if (!ttlnum) {
			pub_base_AllDwnFilName(filename);
			fp = fopen(filename, "w");
			if (fp == NULL) {
				sprintf(acErrMsg, " open file [%s] error ", filename);
				WRITEMSG
					strcpy(g_pub_tx.reply, "S047");
				goto ErrExit;
			}
			/**显示列标题**
			***/
			/*fprintf(fp, "~始日期|止日期|天数|$利率|$税率|$本金|$利息|$利息税|$合计|\n");*/
			fprintf(fp, "~描述|始止日期|天数|$本金|$利率|$终值|$合计|\n");
		}
	vtcp_log("%d@%s  ttlnum=[%d] CHANGECODE 00cceeffaa989aabbb filename{%s}\n", __LINE__, __FILE__, ttlnum, filename);
		/*if(0 != ilist_calculate_intst( &f_td_mst, fp, &ttlnum)){
			 sprintf(acErrMsg,"RET[%d]",ret); 
                	WRITEMSG 
                	goto ErrExit; 
		}*/
		{
/**------- hao -------- 增加定期自动转存计息 ---------**/

			char            td_type;
			long            tmp_tx_date = 0;
			long            tmp_ic_date = 0;
			long            tmp_opn_date = 0;
			long            tmp_mtr_date = 0;
			double          tmp_dealval = 0.00;
			double          tmp_factval = 0.00;
			double          tmp_val = 0.00;
			double          factval = 0.00;
			double          val = 0.00;
			double          tmp_keepval = 0.00;
			double          tmp_amt = 0.00;
			double          dealval = 0.00;
			int             l_term = 0;
			char 		reply;

		g_pub_tx.tx_amt1 = g_td_mst.bal;
                tmp_tx_date = g_pub_tx.tx_date;
                tmp_ic_date = g_td_mst.ic_date;
                tmp_opn_date = g_td_mst.opn_date;
                tmp_mtr_date = g_td_mst.mtr_date;
                tmp_amt = g_pub_tx.tx_amt1;
                do {
                        if (g_td_mst.tfr_ind[0] == 'Y' && tmp_tx_date > g_td_mst.mtr_date)
                                g_pub_tx.tx_date = g_td_mst.mtr_date;
                        else
                                g_pub_tx.tx_date = tmp_tx_date;
                        tmp_dealval = 0.00;
                        tmp_factval = 0.00;
                        tmp_val = 0.00;
                        tmp_keepval = 0.00;
                        if (f_pub_base_CalIntstTdZCZQ(&tmp_dealval, &tmp_factval,
                                           &tmp_val, &tmp_keepval, reply, fp)) {
                                strcpy(acErrMsg, "计算整存整取利息出错!!");
                                WRITEMSG
                                        return 1;
                        }
                        vtcp_log("AFTER [%d--%d]", g_td_mst.ic_date, g_td_mst.mtr_date);
                        vtcp_log("deal[%.2lf] factval[%.2lf] val[%.2lf][%.2lf]",
                            tmp_dealval, tmp_factval, tmp_val, tmp_keepval);
                        /**dealval += tmp_dealval;
                        *factval += tmp_factval;
                        *val += tmp_val;
                        *keepval += tmp_keepval;*/

                        g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + (tmp_factval) + (tmp_keepval);

                        vtcp_log("after [%d--%d]", g_td_mst.ic_date, g_td_mst.mtr_date);
                        g_td_mst.ic_date = g_td_mst.mtr_date;
                        g_td_mst.opn_date = g_td_mst.mtr_date;
                        if (g_td_parm.term_type[0] == 'D') {
                                l_term = g_td_parm.term;
                                pub_base_deadlineD(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                        } else if (g_td_parm.term_type[0] == 'Y') {
                                l_term = g_td_parm.term * 12;
                                pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                        } else if (g_td_parm.term_type[0] == 'M') {
                                l_term = g_td_parm.term;
                                pub_base_deadlineM(g_td_mst.opn_date, l_term, &g_td_mst.mtr_date);
                        }
                } while (tmp_tx_date > g_pub_tx.tx_date);
                g_pub_tx.tx_date = tmp_tx_date;
                g_td_mst.ic_date = tmp_ic_date;
                g_td_mst.opn_date = tmp_opn_date;
                g_td_mst.mtr_date = tmp_mtr_date;
                g_pub_tx.tx_amt1 = tmp_amt;
                /**------------------------------------------------------**/
		}
		ttlnum = 1;

	vtcp_log("%d@%s  ttlnum=[%d]\n", __LINE__, __FILE__, ttlnum);

	if (!ttlnum) {
		strcpy(g_pub_tx.reply, "S049");
		goto ErrExit;
	} else {
		fclose(fp);

		set_zd_data(DC_FILE_SND, "1");
	}

GoodExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}



int 
ilist_calculate_intst(struct td_mst_c * i_td_mst, FILE * ifp, int *prln)
{
	int             ret = 0;
	int             days = 0;
	int             days_tmp = 0;
	int             l_term = 0;
	long            is_date = 0;
	long            is_date_tmp = 0;
	long            ie_date_tmp = 0;
	double          td_rate = 0.0;
	double          dd_rate = 0.0;
	double          tax_rate = 0.0;
	double          dg_tot_intst = 0.0, cx_tot_intst = 0;
	double          sig_intst = 0.0;
	double          tmp_intst = 0.0;
	double          bal_tmp = 0.0;
	double          tax_tmp = 0.0;
	struct td_mst_c wd_td_mst;
	struct td_parm_c wd_td_parm;
	char            tmp_cif_no[20];
	
	*prln = 0;
	/* 取得个人所得税税率 */
	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &tax_rate);	/* 此函数已置了reply */
	if (ret) {
		sprintf(acErrMsg, "取个人所得税率错误[%d]!", ret);
		return 1;
	}
	/* 个人定期，零整，教育，存本取息  在两个计提周期内销户的也得提 */
	memcpy(&wd_td_mst, i_td_mst, sizeof(wd_td_mst));
	{
		days = 0;
		days_tmp = 0;
		is_date = 0;
		l_term = 0;
		sig_intst = 0.0;
		bal_tmp = 0.0;
		dd_rate = 0.0;
		td_rate = 0.0;

		is_date = wd_td_mst.ic_date;
		/* 起息日大于当前日期 */
		if (is_date > g_pub_tx.tx_date) {
			sprintf(acErrMsg, "起息日期错误![%d]", ret);
			WRITEMSG
				return (-1);
		}
		if (wd_td_mst.ac_sts[0] == '*') {
			sprintf(acErrMsg, "账户已销户");
			WRITEMSG
				return (-1);
			/*
			 * ===================================================
			 * ======================================
			 * if(is_date>wd_td_mst.lst_date) continue;
			 * memset(&wd_mo_opn_cls,'\0',sizeof(wd_mo_opn_cls));
			 * ret =
			 * Mo_opn_cls_Sel(g_pub_tx.reply,&wd_mo_opn_cls,"ac_id
			 * ='%ld'",wd_td_mst.ac_id); if(ret) {
			 * sprintf(acErrMsg,"查找开销户登记簿错误![%d]",ret);
			 * WRITEMSG return(ret); }
			 * wd_td_mst.bal=wd_mo_opn_cls.amt;
			 * ===================================================
			 * === 
			 */
			/* 将销户时的销户金额在赋回去为了下面算利息 */
		}
		/** 取产品信息 **/
		ret = Td_parm_Sel(g_pub_tx.reply, &wd_td_parm, "prdt_no='%s'", wd_td_mst.prdt_no);
		if (ret) {
			sprintf(acErrMsg, "查找定期帐户错误![%d]", ret);
			WRITEMSG
				return (ret);
		}
		/** 根据定期产品取利率 **/
		ret = pub_base_getllz(wd_td_parm.rate_no, wd_td_parm.cur_no,
				      wd_td_mst.ic_date, &td_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "取利率错误![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (memcmp(wd_td_mst.prdt_no, "257", 3) == 0 || memcmp(wd_td_mst.prdt_no, "258", 3) == 0) {
			vtcp_log("协议存款取分户中的利率[%ld]\n", wd_td_mst.ac_id);
			td_rate = wd_td_mst.rate;
		}
		/** 取活期产品利率 **/
		ret = pub_base_getllz("100", wd_td_parm.cur_no,
				      g_pub_tx.tx_date, &dd_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "取利率错误![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (wd_td_mst.tfr_ind[0] == 'Y') {	/** 自动转存的 **/
			if (wd_td_mst.mtr_date < g_pub_tx.tx_date) {	/** 已经过到期日的 **/
				/** 计算当期计算利息的余额 **/
				if (wd_td_mst.ic_date < DATE061104) {	/* 上线前开户每月30天算 */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						vtcp_log("FUN%s@L%d@F%s, wihleCnt- %d\n", __FUNCTION__, __LINE__, __FILE__, ++whileCnt);
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "1", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "天数计算错误![%d]", ret);
							WRITEMSG
								return ret;
						}
						sig_intst = (bal_tmp * td_rate * days) / (360 * 100);
						sig_intst = pub_base_PubDround_1(sig_intst);

						tax_tmp = sig_intst * tax_rate;
						tax_tmp = pub_base_PubDround_1(tax_tmp);
						fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date_tmp, ie_date_tmp, days, td_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
						*prln+=1;
						bal_tmp += (sig_intst - tax_tmp);

						/** 重新调整起息到期日 **/
						is_date_tmp = ie_date_tmp;
						if (wd_td_parm.term_type[0] == 'D') {
							l_term = wd_td_parm.term;
							pub_base_deadlineD(is_date_tmp, l_term, &ie_date_tmp);
						} else if (wd_td_parm.term_type[0] == 'Y') {
							l_term = wd_td_parm.term * 12;
							pub_base_deadlineM(is_date_tmp, l_term, &ie_date_tmp);
						} else if (wd_td_parm.term_type[0] == 'M') {
							l_term = wd_td_parm.term;
							pub_base_deadlineM(is_date_tmp, l_term, &ie_date_tmp);
						}
					}
					ret = pub_base_CalDays(is_date_tmp, g_pub_tx.tx_date, "1", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "天数计算错误![%d]", ret);
						WRITEMSG
							return ret;
					}
					sig_intst = (bal_tmp * dd_rate * days) / (360 * 100);
					sig_intst = pub_base_PubDround_1(sig_intst);
					tax_tmp = sig_intst * tax_rate;
					tax_tmp = pub_base_PubDround_1(tax_tmp);
					fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date, g_pub_tx.tx_date, days, dd_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
					*prln+=1;

				} else {	/* 上线后开户实际天数算 */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "0", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "天数计算错误![%d]", ret);
							WRITEMSG
								return ret;
						}
						sig_intst = (bal_tmp * td_rate * days) / (360 * 100);
						sig_intst = pub_base_PubDround_1(sig_intst);

						tax_tmp = sig_intst * tax_rate;
						tax_tmp = pub_base_PubDround_1(tax_tmp);

						bal_tmp += (sig_intst - tax_tmp);
						fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date_tmp, ie_date_tmp, days, td_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
						*prln+=1;

						/** 重新调整起息到期日 **/
						is_date_tmp = ie_date_tmp;
						if (wd_td_parm.term_type[0] == 'D') {
							l_term = wd_td_parm.term;
							pub_base_deadlineD(is_date_tmp, l_term, &ie_date_tmp);
						} else if (wd_td_parm.term_type[0] == 'Y') {
							l_term = wd_td_parm.term * 12;
							pub_base_deadlineM(is_date_tmp, l_term, &ie_date_tmp);
						} else if (wd_td_parm.term_type[0] == 'M') {
							l_term = wd_td_parm.term;
							pub_base_deadlineM(is_date_tmp, l_term, &ie_date_tmp);
						}
					}
					ret = pub_base_CalDays(is_date_tmp, g_pub_tx.tx_date, "0", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "天数计算错误![%d]", ret);
						WRITEMSG
							return ret;
					}
					sig_intst = (bal_tmp * dd_rate * days) / (360 * 100);
					sig_intst = pub_base_PubDround_1(sig_intst);
					tax_tmp = sig_intst * tax_rate;
					tax_tmp = pub_base_PubDround_1(tax_tmp);
					fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date_tmp, ie_date_tmp, days, dd_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
					*prln+=1;
				}
			} else {/** 没到期或刚到期的 **/
				/* 起息日小于上线日 */
				if (wd_td_mst.ic_date < DATE061104) {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "1", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "天数计算错误![%d]", ret);
						WRITEMSG
							return ret;
					}
				} else {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "0", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "天数计算错误![%d]", ret);
						WRITEMSG
							return ret;
					}
				}
				sig_intst = (wd_td_mst.bal * td_rate * days) / (360 * 100);
				sig_intst = pub_base_PubDround_1(sig_intst);
				fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date_tmp, ie_date_tmp, days, td_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
				*prln+=1;
			}
		}
	}
	return 0;
}



/**********************************************************************
* 函数名：    pub_base_CalIntstTdZCZQ()
* 函数功能：  计算整存整取利息
* 作者/时间： 2003年12月28日
*
* 参数：
*     输入：
*     输出: 应付利息          double
*           实付利息          double
*           代扣利息税        double
*           保值利息          double
*           响应码            char(4)
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/
int 
f_pub_base_CalIntstTdZCZQ(double *dealval, double *factval,
			double *val, double *keepval, char *reply, FILE *ifp)
{
	int             draw_flag;	/* 到期标志 
					 * 1-提前支取,2-到期支取,3-逾期支取 */
	int             monterm, monterm1;
        long 		dayterm;	/* 月 日存期 */
	long             daytermval;	/* 代扣个人所得利息天数 */
	double          amtval;	/* 计算所得税金额 */
	double          sdsl;	/* 个人所得税率 */
	double          rate_val;	/* 利率 */
	double          lx;	/* 利息 */
	long            date88, date89, date93, date9307, date99;
	long            date91, date97, date98, date00;
	char            llbh[3];/* 利率编号 */
	int             ret;
	double          a1, a2, a3, s1, s1_tmp, s2;	/* 存期*利率 */
	char            intst_term_type[2];	/* 计算天数类型 */


	sprintf(acErrMsg, "g_td_parm.term_type=[%s],term=[%d],prdt_no=[%s], \
			tx_amt=[%lf]", g_td_parm.term_type, g_td_parm.term,
		g_td_parm.prdt_no, g_pub_tx.tx_amt1);
	WRITEMSG

		date88 = 19880910;
	date89 = 19890201;
	date91 = 19911201;
	date93 = 19930301;
	date9307 = 19930711;
	date97 = 19971023;
	date98 = 19981206;
	date99 = 19991101;
	date00 = 20000713;
	(*keepval) = 0.00;	/* 保值利息 */
	(*dealval) = 0.00;	/* 应付利息 */
	(*factval) = 0.00;	/* 实付利息 */
	(*val) = 0.00;		/* 代扣利息税 */

	/* 判断交易日期是否到期 */
	if (g_pub_tx.tx_date < g_td_mst.mtr_date) {
		draw_flag = 1;	/* 提前 */
	} else if (g_pub_tx.tx_date == g_td_mst.mtr_date) {
		draw_flag = 2;	/* 到期 */
	} else {
		draw_flag = 3;	/* 逾期 */
	}

	/* 计算存期 */
	switch (g_td_parm.term_type[0]) {
	case 'Y':
		monterm = g_td_parm.term * 12;
		break;
	case 'M':
		monterm = g_td_parm.term;
		break;
	case 'D':
		monterm = g_td_parm.term / 30;
		break;
	default:
		/* 错误返回 */
		sprintf(acErrMsg, "定期产品表中存期类型错误 %c", g_td_parm.term_type);
		WRITEMSG
			strcpy(reply, "W108");
		return 1;
	}

	strcpy(intst_term_type, g_td_parm.intst_term_type);	/***按实际天数计息***/

	if (intst_term_type[0] != '0' && intst_term_type[0] != '1') {
		sprintf(acErrMsg, "产品表中的计息天数类型错误 type=[%s]",
			g_td_parm.intst_term_type);
		WRITEMSG
			MEMSET_DEBUG(reply, 0x0, sizeof(reply));
		strcpy(reply, "W107");
		return 1;
	}
	sprintf(acErrMsg, "开始计算利息monterm=[%d],draw_flag=[%d]", monterm, draw_flag);
	WRITEMSG

		if (monterm <= 24) {

		sprintf(acErrMsg, "普通利息计算");
		WRITEMSG

		/* 普通利息计算 */
			if (g_td_mst.ic_date <= date93) {
			/* 93.03.01之前的整存整取 */

			sprintf(acErrMsg, "普通利息计算--930301之前的整存整取");
			WRITEMSG

				switch (draw_flag) {
			case 1:

				/*
				 * 提前支取实行同档次定期利率 1.根据开户日期
				 * 交易日期 和产品 界定期限码
				 * 2.根据期限码赋相应的利率编号,并查询出利率
				 * 3.计算利息 4.计算所得税 
				 */

				sprintf(acErrMsg, "普通利息计算--提前支取");
				WRITEMSG

				/* 按实存时间同档次定期利率计息 界定期限 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "界定期限错误");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "界定利率:llbh=[%s]", llbh);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "计算天数错误!!");
					WRITEMSG
						strcpy(reply, "W102");
					return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

				/* llbh 根据利率编号和交易日期查询利率 */
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				sprintf(acErrMsg, "dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
					dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = dayterm * rate_val * g_pub_tx.tx_amt1
					* (1 + g_td_mst.flt_ratio) / L360 / L100;

				(*dealval) = pub_base_PubDround_1(lx);
				if(*dealval > 0.001)
				fprintf(ifp, "+利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
					g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* 计算所得税 */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
									   g_td_mst.ic_date), g_pub_tx.tx_date, intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* 需扣利息税的利息 */
					amtval = lx * daytermval / dayterm;

					sprintf(acErrMsg, "dayterm=[%ld],daytermval=[%ld],lx=[%lf],amtval=[%lf],flt_ratio=[%lf]", dayterm, daytermval, lx, amtval, g_td_mst.flt_ratio);
					WRITEMSG

					/* 取利息税税率 */
						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "定期产品利息税税率不存在 %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "取利息税税率错误 %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					sprintf(acErrMsg, "利息税税率sdsl=[%lf] ", sdsl);
					WRITEMSG

						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0.001)*/
				fprintf(ifp, "-利息税|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					PubMax(date99, g_td_mst.ic_date) , g_pub_tx.tx_date, daytermval, 
						amtval, sdsl, val, g_pub_tx.tx_amt1+*dealval-*val);
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:
				/* 到期支取利息处理 */
			case 3:
				/*
				 * 逾期支取利息处理 1.根据利率编号和开户日期
				 * 起息日期 交易日期查询出分段利率表
				 * 并计算出利率和存期之积 2.计算到期利息
				 * 3.如果逾期,计算逾期利息 4.计算个人所得税 
				 */

				sprintf(acErrMsg, "普通利息计算--到期或逾期支取");
				WRITEMSG

				/* 计算到期利率*到期时间 分段计算 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, " llbh=[%s],cur_no=[%s],ic_date=[%ld],mtr_date=[%ld]",
					llbh, g_td_parm.cur_no, g_td_mst.ic_date, g_td_mst.mtr_date);
				WRITEMSG

					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
				sprintf(acErrMsg, " 到期 : 计算分段利率*天数 = [%lf]", a1);
				WRITEMSG

				/* 计算逾期利率*逾期时间 */

				/*
				 * date93 之前按原订存期利率计算 date93
				 * 之后按支取日挂牌公告的活期储蓄利率计算 
				 */

					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(g_td_mst.mtr_date, date93,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;	/* 不计息 */
				}

				/* 取相应存期的利率 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a2 = dayterm * rate_val;	/* date93前 */
				if(a2 > 0.001)
				fprintf(ifp, "+93前逾期利息|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|------|\n", g_td_mst.mtr_date,
					date93, dayterm, g_pub_tx.tx_amt1, rate_val, a2);

				sprintf(acErrMsg, " 逾期 (date93前) : 计算分段利率*天数 = [%lf]", a2);
				WRITEMSG


					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
									   date93), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;
				}

				/* 根据利率编号 利率类型取出当时挂牌活期利息 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a3 = dayterm * rate_val;	/* date93后 */
				if(a3 > 0.001)
				fprintf(ifp, "+93后逾期利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------|\n", PubMax(g_td_mst.mtr_date,
					date93) , g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, a3);

				sprintf(acErrMsg, " 逾期 (date93后) : 计算分段利率*天数 = [%lf]", a3);
				WRITEMSG

					lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = pub_base_PubDround_1(lx);
				fprintf(ifp, "合计|%.08ld-%.08ld|X|XXX|XXX|XXX|%.2lf|\n", g_td_mst.ic_date,
					  g_pub_tx.tx_date, *dealval);

				/* 计算利息税 */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.mtr_date), g_pub_tx.tx_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* 应缴税利息 */
					(*val) = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					/* 取利息税编号并取利率 */
					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "定期产品利息税税率不存在 %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "取利息税税率错误 %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
					(*val) = pub_base_PubDround_1((*val) * sdsl);
					/*if(*val > 0.001)	*/
					fprintf(ifp, "-利息税|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|-------|\n", 
					PubMax(date99, g_td_mst.mtr_date), 
						g_pub_tx.tx_date, dayterm, g_pub_intst.tax_intst, sdsl, *val);
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			default:
				break;
			}
		} else {	/* 93.03.01 后存入的整存整取 */

			sprintf(acErrMsg, "普通利息计算--930301之后存入的整存整取");
			WRITEMSG

				switch (draw_flag) {
			case 1:
				/*
				 * 提前支取
				 * 按支取日挂牌公告的活期储蓄存款利率计付利息
				 * 1.计算计息日期到交易日期的总天数
				 * 2.取当前挂牌公告的活期储蓄存款利率
				 * 3.计算利率 4.99年后个人需要交个人所得税 
				 */

				sprintf(acErrMsg, "普通利息计算--提前支取");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "计算天数错误!!");
					WRITEMSG
						return 1;
				}
				/* 取活期利率 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);

				sprintf(acErrMsg, "活期 : llbh=[%s],cur_no=[%s],date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				sprintf(acErrMsg, "利息 : dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				lx = pub_base_PubDround_1(lx);
				(*dealval) = lx;
				if(*dealval > 0.001)
				fprintf(ifp, "+提前支取利息|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
				if (floor(lx * 100) / 100 < 0.005) {
					lx = 0.00;
					(*dealval) = 0.00;
					(*factval) = 0.00;
					(*val) = 0.00;
				}
				/* 计算个人所得税 */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.ic_date), g_pub_tx.tx_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					amtval = lx * daytermval / dayterm;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "定期产品利息税税率不存在 %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "取利息税税率错误 %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				if(*val > 0.001)
				fprintf(ifp, " - 个税|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
							g_td_mst.ic_date), g_pub_tx.tx_date, dayterm, amtval, sdsl, *val,  
										pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)-(*val))) ;

				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:

				/*
				 * 到期支取 1. 计算到期天数 2.
				 * 取对应产品的日利率 3. 计算利息 4.
				 * 99后计算个人所得税 
				 */
			case 3:
				/*
				 * 逾期支取 1.930301--930711 按活期利息计算
				 * 2.930711--到期日 按930711同档次利率计算
				 * 3.到期日--交易日 按交易日活期利率计算
				 * 4.计算99后的个人所得税 
				 *
				 */

				sprintf(acErrMsg, "普通利息计算-- 到期或逾期支取");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG

					if (monterm <= 6) {
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
								    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* 取该产品的对应日利率 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[%ld],定期天数[%ld]",
						llbh, g_td_parm.cur_no, g_td_mst.ic_date, dayterm);
					WRITEMSG
						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "到期:dayterm=[%ld],rate_val=[%lf],tx_amt=[%d],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = lx;
					sprintf(acErrMsg, "注意到期利息:[%lf]", lx);
					WRITEMSG
				if(*dealval > 0.001)
				fprintf(ifp, "+利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
									g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
									pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));

					/* 逾期利息 */
						if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
						/*
						 *
						 * ret=pub_base_CalIntstDays(Pu
						 * bMax(g_td_mst.mtr_date,
						 * date99), g_pub_tx.tx_date,
						 * intst_term_type,
						 * &dayterm); 
						 */
						ret = pub_base_CalIntstDays(g_td_mst.mtr_date,
									    g_pub_tx.tx_date, intst_term_type,
								  &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						vtcp_log("[%s][%d]逾期后的天数[%ld]\n", __FILE__, __LINE__, dayterm);
					} else {
						dayterm = 0;

					}
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "逾期: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date, dayterm);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					sprintf(acErrMsg, "逾期利息: dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],\
        flt_ratio=[%lf] lx[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio, lx);
					WRITEMSG
						(*dealval) = (*dealval) + lx;
				if(lx > 0)
				fprintf(ifp, "+逾期利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));
					sprintf(acErrMsg, "注意总利息: dealval[[%lf]", *dealval);
					WRITEMSG
					/* 计算个人所得税 */

						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						/* 利率编号从产品里取 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "定期部分应该缴利息税的参数1:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval = rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.mtr_date), g_pub_tx.tx_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "利息税:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "逾期部分应该缴利息税的参数2:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf], \
   flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;
						sprintf(acErrMsg, "逾期部分应该缴税的利息是:lx[%lf]", lx);
						WRITEMSG
							sprintf(acErrMsg, "定期应该应缴税的利息是amtval[%lf]", amtval);
						WRITEMSG
							amtval = amtval + lx;
						sprintf(acErrMsg, "总的应该缴税的利息是amtval[%lf]", amtval);
						WRITEMSG
							ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "定期产品利息税税率不存在 %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "取利息税税率错误 %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
					if(*val > 0)		
				fprintf(ifp, "+利息|%.08ld-%.08ld|--|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
g_td_mst.ic_date), g_pub_tx.tx_date, amtval, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
					}
					(*dealval) = pub_base_PubDround_1(*dealval);
					(*factval) = (*dealval) - (*val);
					if (floor((*dealval) * 100) / 100 < 0.005) {
						lx = 0.00;
						(*dealval) = 0.00;
						(*factval) = 0.00;
						(*val) = 0.00;
					}
					sprintf(acErrMsg, "利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				} else {	/* monterm >6 */

					sprintf(acErrMsg, "存期大于六个月");
					WRITEMSG
						ret = pub_base_CalIntstDays(g_td_mst.ic_date, date9307,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* 取930711时活期利率 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = lx;
				if(*dealval > 0)
				fprintf(ifp, "+利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, date9307,
						 dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

						ret = pub_base_CalIntstDays(PubMax(date9307,
					g_td_mst.ic_date), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/*
					 * 在930711后开户,取该产品的对应日利率
					 * 
					 * 否则,取pubmax(930711,ic_date)当天同
					 * 档次利率 
					 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
								      PubMax(date9307, g_td_mst.ic_date), &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = (*dealval) + lx;
				if(lx > 0)
				fprintf(ifp, "+利息|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date9307,
g_td_mst.ic_date), g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, lx, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

					sprintf(acErrMsg, "1lx=[%lf]", dayterm * rate_val);
					WRITEMSG
						sprintf(acErrMsg, "2lx=[%lf]", floor(g_pub_tx.tx_amt1 * L100));
					WRITEMSG
						sprintf(acErrMsg, "3lx=[%lf]", dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100);
					WRITEMSG
						sprintf(acErrMsg, "4lx=[%lf]", dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio));
					WRITEMSG
						sprintf(acErrMsg, "4lx=[%lf]", dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio) / L360);
					WRITEMSG
						sprintf(acErrMsg, "4lx=[%lf]", dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio) / L360 / L100);
					WRITEMSG
						sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG
					/* 逾期利息计算 */
						if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
						ret = pub_base_CalIntstDays(g_td_mst.mtr_date,
									    g_pub_tx.tx_date, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
					} else {
						dayterm = 0;
					}

					/* 取活期利率  */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "逾期: llbh=[%s],cur_no=[%s],date=[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误[%s][%s][%ld][%.2lf]", llbh, g_td_parm.cur_no, g_pub_tx.tx_date, rate_val);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = (*dealval) + lx;
				if(lx > 0)
				fprintf(ifp, "+利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
                      g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

					/* 计算个人所得税 */
						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						/* 取产品表的利率编号 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "利息税: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "利息dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
							daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG
							amtval = rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;


						if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
							ret = pub_base_CalIntstDays(PubMax(date99,
											   g_td_mst.mtr_date), g_pub_tx.tx_date,
										    intst_term_type, &daytermval);
							if (ret != 0) {
								sprintf(acErrMsg, "计算天数错误!!");
								WRITEMSG
									return 1;
							}
						} else {
							daytermval = 0;

						}

						/* 取活期利率 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "利息: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "利息dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
							daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval += rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "定期产品利息税税率不存在 %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "取利息税税率错误 %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0)	*/
				fprintf(ifp, "-利息税|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
                       g_td_mst.ic_date), g_td_mst.mtr_date , dayterm, amtval, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
					}
					(*dealval) = pub_base_PubDround_1(*dealval);
					(*factval) = (*dealval) - (*val);
					if (floor((*dealval) * 100) / 100 < 0.005) {
						(*dealval) = 0.00;
						(*factval) = 0.00;
						(*val) = 0.00;
					}
					sprintf(acErrMsg, "利息 : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				}
				break;
			default:
				/* 错误返回 */
				return 1;
			}
		}
	} else {		/* monterm >24 */

		sprintf(acErrMsg, "保值利息计算");
		WRITEMSG

		/* 保值利息计算 */
			switch (draw_flag) {
		case 1:
			/* 未到期支取 */

			sprintf(acErrMsg, "未到期支取 ");
			WRITEMSG

				if (g_td_mst.ic_date >= date93) {	/* date93之后开户的 */

				sprintf(acErrMsg, "date93 之后开户的,按活期利率计息 ");
				WRITEMSG

				/* 按到期活期利率计算 */
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "计算天数错误!!");
					WRITEMSG
						return 1;
				}
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);

				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				sprintf(acErrMsg, "rate_val=[%lf],dayterm=[%ld],tx_amt=[%lf],flt_ratio=[%lf]",
					rate_val, dayterm, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;

				(*dealval) = pub_base_PubDround_1(lx);
				if(*dealval > 0)	
				fprintf(ifp, "+利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
        g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* 计算利息税 */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "定期产品利息税税率不存在 %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "取利息税税率错误 %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)	*/
				fprintf(ifp, "-利息税|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(g_td_mst.ic_date,
date99), g_pub_tx.tx_date,  dayterm, lx, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
				}
				if ((*dealval) < 0.005) {
					(*dealval) = 0.00;
					(*val) = 0.00;
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG
			}
			 /* end of date93之后开户的 */ 
			else {	/* date93 之前开户的 */

				sprintf(acErrMsg, "date93 之前开户的,按活期利率计息 ");
				WRITEMSG

				/*
				 * 从ic_date,tx_date分段计息,遇调即调,不分高低
				 *  
				 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "界定期限错误");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],ic_date=[%ld],tx_date=[%ld],\
        intst_term_type=[%d]", llbh, g_td_parm.cur_no, g_td_mst.ic_date,
					g_pub_tx.tx_date, intst_term_type);
				WRITEMSG

				/* rate_val = 利率 * 天数之和 */
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "计算分段积数错误");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "分段积数=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
					rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = lx;

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					/* 界定期限 */
					/* 从开户日到PubMin(到期日,date99) */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

					ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
								 g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99),
							   intst_term_type);
					if (ret != 0) {
						sprintf(acErrMsg, "界定期限错误");
						WRITEMSG
							return 1;
					}
					/* rate_val = 利率 * 天数之和 */
					ret = pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "计算分段积数错误");
						WRITEMSG
							return 1;
					}
					sprintf(acErrMsg, "lx=[%lf],分段积数=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						lx, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = lx - rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "定期产品利息税税率不存在 %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "取利息税税率错误 %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)*/
				fprintf(ifp, "-利息税|%.08ld-%.08ld|---|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99) ,  lx, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
				}
				if (floor((*dealval) * 100) / 100 < 0.005) {
					(*dealval) = 0.00;
					(*val) = 0.00;
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

			}
			break;
		case 2:
			/* 到期支取 */
			/*
			 * 1. 88/09/10
			 * 以前开户的保值储蓄,保值期从88/09/10至到期日 2.
			 * 89/02/01
			 * 以后到期的保值储蓄,从88/09/10起执行89/02/01调整后
			 * 的储蓄存款利率 3. 91/12/01--93/03/01
			 * 之间存入的保值储蓄,保值期从93/07/11 至到期日 4.
			 * 93/03/01--93/07/11
			 * 之间存入的一年期以上储蓄存款,从开户日 到93/07/11
			 * 按93/07/11公布的活期利率计息,93/07/11至到期日
			 * 按93/07/11公布的同档次定期利率计息 5.
			 * 93/07/11(含)起存入的三五年期按存单上原定的利率计息 
			 */
		case 3:
			/* 逾期支取 */

			sprintf(acErrMsg, "到期逾期利息计算");
			WRITEMSG

				if (g_td_mst.ic_date < date88) {

				sprintf(acErrMsg, "----------->起息日期小于88.09.10");
				WRITEMSG

					if (g_td_mst.mtr_date >= date89) {
					/* 从开户日到date89分段计息 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.opn_date, date89, intst_term_type,
								  Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
					ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* date89 相应储种的利率 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = a1 + dayterm * rate_val;	
				if(dayterm*rate_val > 0)
				  fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", date88, g_td_mst.mtr_date,
    				dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
         						 * (1 + g_td_mst.flt_ratio) / L360 / L100);

				} else {
					/* 从开户日到到期日分段计息 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.opn_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
				}

				/* 计算保值利息 */
				/* 取交易日的保值利率 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, BAOZHI);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "计算天数错误!!");
					WRITEMSG
						return 1;
				}
				(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 / L360 / L100;
				if(*keepval >0)	
				fprintf(ifp, "+保值利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date88, g_td_mst.mtr_date,
					 dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

			} else {/* 88.9.10后 */

				sprintf(acErrMsg, "----------->起息日期大于88.9.10");
				WRITEMSG
					if (g_td_mst.ic_date < date91) {

					sprintf(acErrMsg, "到期逾期利息计算,88.9.10后开户的,date91前开户");
					WRITEMSG

						ret = pub_base_CalIntstDays(g_td_mst.ic_date,
									    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					/* 取date89当日的相应的储种的利率 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = rate_val * dayterm;
				if(a1>0)
				 fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------  |\n", g_td_mst.ic_date, g_td_mst.mtr_date,
 dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


					ret = pub_base_CalIntstDays(date99, g_td_mst.mtr_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					s1 += rate_val * (double) daytermval;


					/* 取到期日的保值利率 */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, BAOZHI);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					      g_td_mst.mtr_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
				if(*keepval > 0)
				fprintf(ifp, "+保值利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date99, g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

					s2 += rate_val * daytermval;

				}
				 /* end of date91 前开户 */ 
				else {	/* date91 后开户的 */
					if (g_td_mst.ic_date < date93) {

						sprintf(acErrMsg, "到期逾期利息计算,date91后开户,date93 前开户");
						WRITEMSG

						/*
						 * 从计息日到到期日按分段利率�
						 * 葡� 
						 */
							MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
						if (ret != 0) {
							sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						/*
						 * 到期日
						 * PubMin(到期日，date99) 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, PubMin(g_td_mst.mtr_date,
												   date99), intst_term_type, Lone, &s1_tmp, NULL);
						if (ret != 0) {
							sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						s1 += a1 - s1_tmp;

					} else {	/* 1993/03/01之后 */

						sprintf(acErrMsg, "到期逾期利息计算,date93 后开户");
						WRITEMSG

							ret = pub_base_CalIntstDays(g_td_mst.ic_date,
										    date9307, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						/* 取date9307活期利率 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "date93,date9307这段时间按活期计息,llbh=[%s],cur_no=[%s],\
        date=9307", llbh, g_td_parm.cur_no);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307这段时间按活期计息,date9307时的rate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a1 = rate_val * dayterm;
						if (a1>0)
						 fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf| -------  |\n", g_td_mst.ic_date,
 date9307, dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


						sprintf(acErrMsg, "date93,date9307这段时间按活期计息,积数为=[%lf]", a1);
						WRITEMSG

							ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						date9307), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						/*
						 * 取PubMax(开户日期,date9307)
						 * 日的相应的利率 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "date9307后到到期日按date97同档次利率计息,llbh=[%s], \
		cur_no=[%s],date=[%ld]", llbh, g_td_parm.cur_no,
							PubMax(g_td_mst.ic_date, date9307));
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
									      PubMax(g_td_mst.ic_date, date9307), &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307这段时间按活期计息,date9307时的rate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a2 = rate_val * dayterm;
								if(a2>0)
								fprintf(ifp, "+分段利息|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.ic_date,
date9307), g_td_mst.mtr_date,
								  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
								 * (1 + g_td_mst.flt_ratio) / L360 / L100);

						sprintf(acErrMsg, "date9307后到到期日,这段时间积数为=[%lf]", a2);
						WRITEMSG

							a1 = a1 + a2;

						ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						 date99), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "计算天数错误!!");
							WRITEMSG
								return 1;
						}
						s1 = 0.00;
						s1 = daytermval * rate_val;
					}

					/******* delete by rob at 20030728 ***
						ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
	 						date9307),g_td_mst.mtr_date,
                            intst_term_type,&dayterm);
                		if (ret != 0)
                		{
                    		sprintf(acErrMsg,"计算天数错误!!");
                    		WRITEMSG
                    		return 1;
                		}
  
                        * 取到期日的保值利率 *
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);
  
sprintf(acErrMsg,"到期日的保值利率 llbh=[%s],cur_no=[%s],mtr_date=[%ld]",
       llbh,g_td_parm.cur_no,g_td_mst.mtr_date);
WRITEMSG 
  
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }
  
sprintf(acErrMsg,"保值利率 rate_val=[%lf],dayterm=[%ld],tx_amt=[%lf],\
        flt_ratio=[%lf]",rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG 
  
                        (*keepval) = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
  
sprintf(acErrMsg, "保值利息: keepval=[%lf]", *keepval);
WRITEMSG
						******/

					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "计算天数错误!!");
						WRITEMSG
							return 1;
					}
					s2 = 0.00;
					s2 = rate_val * dayterm;

				}

			}

			/* 逾期利息 */
			/* 到期日到date93按分段利率计息 */

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.rate_no);

			sprintf(acErrMsg, "逾期利率 到期日到date93,llbh=[%s],cur_no=[%s]",
				llbh, g_td_parm.cur_no);
			WRITEMSG
				if (g_td_mst.mtr_date < date93) {
				ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
							  g_td_mst.mtr_date, date93, intst_term_type, Lone, &a2, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "计算分段利率*天数错误 [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
			} else {
				a2 = 0.00;
			}

			sprintf(acErrMsg, "逾期利率 到期日到date93,这段分段积数为: [%lf]", a2);
			WRITEMSG

			/* 取交易当日的活期利率 */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

			sprintf(acErrMsg, "逾期利率 date93后,llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
			WRITEMSG

				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
			if (ret != 0) {
				sprintf(acErrMsg, "从com_rate取利率值错误 [%d]", ret);
				WRITEMSG
					strcpy(reply, "W110");
				return ret;
			}
			ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date93),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "计算天数错误!!");
				WRITEMSG
					return 1;
			}
			sprintf(acErrMsg, "逾期利率 date93后  rate_val= [%lf],dayterm=[%ld]",
				rate_val, dayterm);
			WRITEMSG
				a3 = 0;
			a3 = rate_val * dayterm;
						if(rate_val > 0 && dayterm > 0)
						 fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.mtr_date, date93),
             g_pub_tx.tx_date,
  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


			sprintf(acErrMsg, "逾期利率 date93后利息积数为: [%lf]", a3);
			WRITEMSG

				ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date99),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "计算天数错误!!");
				WRITEMSG
					return 1;
			}
			s1 += rate_val * dayterm;

			sprintf(acErrMsg, "到期利息积数=[%lf],逾期9307前积数=[%lf],逾期9307后积数=\
        [%lf],tx_amt=[%lf],flt_ratio=[%lf]", a1, a2, a3, g_pub_tx.tx_amt1,
				g_td_mst.flt_ratio);
			WRITEMSG
				lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			(*dealval) = lx;

			sprintf(acErrMsg, "--------->利息值为:[%lf]", *dealval);
			WRITEMSG

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
				ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
				if (ret != 0 && ret == 1) {
					sprintf(acErrMsg, "定期产品利息税税率不存在 %d",
						ret);
					WRITEMSG
						strcpy(reply, "W100");
					return 1;
				} else if (ret < 0) {
					sprintf(acErrMsg, "取利息税税率错误 %d", ret);
					WRITEMSG
						strcpy(reply, "W101");
					return 1;
				}
				lx = s1 * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio)
					/ L360 / L100;
				/* add by rob at 20030904 */
				g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
				(*val) = lx * sdsl;
				/*if(*val>0 )	*/
				fprintf(ifp, "-利息税| ----  | -- |%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					 		 lx, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
			}
			(*dealval) = pub_base_PubDround_1(*dealval);
			(*val) = pub_base_PubDround_1(*val);
			(*keepval) = pub_base_PubDround_1(*keepval);

			(*factval) = (*dealval) - (*val);
			(*keepval) = (*keepval);	/* 税后保值利息 */

			sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
				*dealval, *factval, *val, *keepval);
			WRITEMSG

				break;
		default:
			return 1;

		}
	}

	sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
		*dealval, *factval, *val, *keepval);

	return 0;
}



/**********************************************************************
* 函数名：    f_pub_base_getsecrate()
* 函数功能：  根据利率编号 币种 起息日 交易日 模式,计算分段利率*存期之和
* 作者/时间： 2003年01月05日
*
* 参数：
*     输入： 利率编号 rate_no    char *
*            币种     cur_no     char *
*            起息日期 ic_date    long
*            交易日期 tx_date    long
*            计算天数类型 intst_term_type char 
*                   0--按实际天数计算    1--按每月30天计算
*            模式     mode       int  
*                   0--遇调即调,不分高低 1--遇高就高,遇低不低
*     输出:
*            利率*存期 rate_val  double *
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/

int f_pub_base_getsecrate(char *rate_no, char *cur_no, long ic_date,
        long tx_date, char intst_term_type[2], int mode, double *rate_val, FILE *ifp)
{
    struct com_rate_c com_rate_c;
    char rateno[4];
    int ret;
    double day_val; /* 利率*天数之和 */
    double rate_val_last, llz; /* 调息后前一档利率值 */
    long   beg_date;      /* 前一档利率计算的起始日期 */
    long   dayterm;

    if (ic_date > tx_date)
    {
        return -1;
    }
    
    sprintf(acErrMsg,"ic_date = [%ld],tx_date=[%ld]",ic_date,tx_date);
    WRITEMSG
    
    ret = Com_rate_Sel(&g_pub_tx.reply, &com_rate_c, "rate_code = '%s' \
          and cur_no = '%s' and beg_date <= %ld and end_date > %ld ",
          rate_no, cur_no, ic_date, ic_date);
    if (ret == 100)
    {
        sprintf(acErrMsg, " NOTFOUND record in com_rate error!! [%d] ",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W109");
        return 100;
    }
    else if (ret != 0)
    {
        sprintf(acErrMsg, "SELECT com_rate error !! [%d] ", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W110");
        return ret;
    }
   
    day_val = 0.00;
    rate_val_last = com_rate_c.rate_val * com_rate_c.rate_type / L360;
    beg_date = ic_date ;

    ret = Com_rate_Dec_Sel(&g_pub_tx.reply,"rate_code = '%s' \
          and cur_no = '%s' and beg_date > %ld and beg_date < %ld \
          order by beg_date ", rate_no, cur_no, ic_date, tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg, "prepare error code [%d] ", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        return ret;
    }
    
    while(1)
    {
        ret = Com_rate_Fet_Sel(&com_rate_c, &g_pub_tx.reply);
        if ( ret != 0 && ret != 100)
        {
            sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "D103");
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        ret=pub_base_CalIntstDays(beg_date, com_rate_c.beg_date,
            intst_term_type,&dayterm);
        if (ret != 0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
            return 1;
        }
        
        sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
        WRITEMSG
      
        day_val += dayterm * rate_val_last;

	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
		com_rate_c.beg_date, dayterm, g_pub_tx.tx_amt1, rate_val_last,  day_val* floor(g_pub_tx.tx_amt1 * L100) / L100
          * (1 + g_td_mst.flt_ratio) / L360 / L100);

        
        if (com_rate_c.beg_date > beg_date)
        {
            beg_date = com_rate_c.beg_date;
        }

        llz = com_rate_c.rate_val * com_rate_c.rate_type / L360;
        
        if (mode != 0)
        {
            rate_val_last = (rate_val_last > llz) ? rate_val_last : llz ;
        }
        else
        {
            rate_val_last = llz ;
        }
    }
    
    ret=pub_base_CalIntstDays(beg_date, tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"计算天数错误!!");
        WRITEMSG
        return 1;
    }
    
    sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
    WRITEMSG

    day_val += rate_val_last * dayterm ;
    (*rate_val) = day_val ;
	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+分段利息|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
		tx_date, dayterm, g_pub_tx.tx_amt1, rate_val_last,  day_val* floor(g_pub_tx.tx_amt1 * L100) / L100
          * (1 + g_td_mst.flt_ratio) / L360 / L100);

    Com_rate_Clo_Sel();

    return 0;
}

