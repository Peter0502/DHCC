/*************************************************
* �� �� ��:  sp8133.c
* ���������� 
*
* ��    ��:  
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��: 20070417
* �� �� ��: gujy
* �޸�����: ����ת���Խ���Ϣ��ѯ
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
	int             ttlnum = 0;	/**��ȡ��������**/
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

	/**��ɲ�ѯ����**/
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
		sprintf(acErrMsg, "��������ȡ��![%c]", g_td_parm.td_type[0]);
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
			/**��ʾ�б���**
			***/
			/*fprintf(fp, "~ʼ����|ֹ����|����|$����|$˰��|$����|$��Ϣ|$��Ϣ˰|$�ϼ�|\n");*/
			fprintf(fp, "~����|ʼֹ����|����|$����|$����|$��ֵ|$�ϼ�|\n");
		}
	vtcp_log("%d@%s  ttlnum=[%d] CHANGECODE 00cceeffaa989aabbb filename{%s}\n", __LINE__, __FILE__, ttlnum, filename);
		/*if(0 != ilist_calculate_intst( &f_td_mst, fp, &ttlnum)){
			 sprintf(acErrMsg,"RET[%d]",ret); 
                	WRITEMSG 
                	goto ErrExit; 
		}*/
		{
/**------- hao -------- ���Ӷ����Զ�ת���Ϣ ---------**/

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
                                strcpy(acErrMsg, "����������ȡ��Ϣ����!!");
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
	/* ȡ�ø�������˰˰�� */
	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &tax_rate);	/* �˺���������reply */
	if (ret) {
		sprintf(acErrMsg, "ȡ��������˰�ʴ���[%d]!", ret);
		return 1;
	}
	/* ���˶��ڣ��������������汾ȡϢ  ����������������������Ҳ���� */
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
		/* ��Ϣ�մ��ڵ�ǰ���� */
		if (is_date > g_pub_tx.tx_date) {
			sprintf(acErrMsg, "��Ϣ���ڴ���![%d]", ret);
			WRITEMSG
				return (-1);
		}
		if (wd_td_mst.ac_sts[0] == '*') {
			sprintf(acErrMsg, "�˻�������");
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
			 * sprintf(acErrMsg,"���ҿ������Ǽǲ�����![%d]",ret);
			 * WRITEMSG return(ret); }
			 * wd_td_mst.bal=wd_mo_opn_cls.amt;
			 * ===================================================
			 * === 
			 */
			/* ������ʱ����������ڸ���ȥΪ����������Ϣ */
		}
		/** ȡ��Ʒ��Ϣ **/
		ret = Td_parm_Sel(g_pub_tx.reply, &wd_td_parm, "prdt_no='%s'", wd_td_mst.prdt_no);
		if (ret) {
			sprintf(acErrMsg, "���Ҷ����ʻ�����![%d]", ret);
			WRITEMSG
				return (ret);
		}
		/** ���ݶ��ڲ�Ʒȡ���� **/
		ret = pub_base_getllz(wd_td_parm.rate_no, wd_td_parm.cur_no,
				      wd_td_mst.ic_date, &td_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "ȡ���ʴ���![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (memcmp(wd_td_mst.prdt_no, "257", 3) == 0 || memcmp(wd_td_mst.prdt_no, "258", 3) == 0) {
			vtcp_log("Э����ȡ�ֻ��е�����[%ld]\n", wd_td_mst.ac_id);
			td_rate = wd_td_mst.rate;
		}
		/** ȡ���ڲ�Ʒ���� **/
		ret = pub_base_getllz("100", wd_td_parm.cur_no,
				      g_pub_tx.tx_date, &dd_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "ȡ���ʴ���![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (wd_td_mst.tfr_ind[0] == 'Y') {	/** �Զ�ת��� **/
			if (wd_td_mst.mtr_date < g_pub_tx.tx_date) {	/** �Ѿ��������յ� **/
				/** ���㵱�ڼ�����Ϣ����� **/
				if (wd_td_mst.ic_date < DATE061104) {	/* ����ǰ����ÿ��30���� */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						vtcp_log("FUN%s@L%d@F%s, wihleCnt- %d\n", __FUNCTION__, __LINE__, __FILE__, ++whileCnt);
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "1", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "�����������![%d]", ret);
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

						/** ���µ�����Ϣ������ **/
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
						sprintf(acErrMsg, "�����������![%d]", ret);
						WRITEMSG
							return ret;
					}
					sig_intst = (bal_tmp * dd_rate * days) / (360 * 100);
					sig_intst = pub_base_PubDround_1(sig_intst);
					tax_tmp = sig_intst * tax_rate;
					tax_tmp = pub_base_PubDround_1(tax_tmp);
					fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date, g_pub_tx.tx_date, days, dd_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
					*prln+=1;

				} else {	/* ���ߺ󿪻�ʵ�������� */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "0", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "�����������![%d]", ret);
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

						/** ���µ�����Ϣ������ **/
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
						sprintf(acErrMsg, "�����������![%d]", ret);
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
			} else {/** û���ڻ�յ��ڵ� **/
				/* ��Ϣ��С�������� */
				if (wd_td_mst.ic_date < DATE061104) {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "1", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "�����������![%d]", ret);
						WRITEMSG
							return ret;
					}
				} else {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "0", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "�����������![%d]", ret);
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
* ��������    pub_base_CalIntstTdZCZQ()
* �������ܣ�  ����������ȡ��Ϣ
* ����/ʱ�䣺 2003��12��28��
*
* ������
*     ���룺
*     ���: Ӧ����Ϣ          double
*           ʵ����Ϣ          double
*           ������Ϣ˰        double
*           ��ֵ��Ϣ          double
*           ��Ӧ��            char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int 
f_pub_base_CalIntstTdZCZQ(double *dealval, double *factval,
			double *val, double *keepval, char *reply, FILE *ifp)
{
	int             draw_flag;	/* ���ڱ�־ 
					 * 1-��ǰ֧ȡ,2-����֧ȡ,3-����֧ȡ */
	int             monterm, monterm1;
        long 		dayterm;	/* �� �մ��� */
	long             daytermval;	/* ���۸���������Ϣ���� */
	double          amtval;	/* ��������˰��� */
	double          sdsl;	/* ��������˰�� */
	double          rate_val;	/* ���� */
	double          lx;	/* ��Ϣ */
	long            date88, date89, date93, date9307, date99;
	long            date91, date97, date98, date00;
	char            llbh[3];/* ���ʱ�� */
	int             ret;
	double          a1, a2, a3, s1, s1_tmp, s2;	/* ����*���� */
	char            intst_term_type[2];	/* ������������ */


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
	(*keepval) = 0.00;	/* ��ֵ��Ϣ */
	(*dealval) = 0.00;	/* Ӧ����Ϣ */
	(*factval) = 0.00;	/* ʵ����Ϣ */
	(*val) = 0.00;		/* ������Ϣ˰ */

	/* �жϽ��������Ƿ��� */
	if (g_pub_tx.tx_date < g_td_mst.mtr_date) {
		draw_flag = 1;	/* ��ǰ */
	} else if (g_pub_tx.tx_date == g_td_mst.mtr_date) {
		draw_flag = 2;	/* ���� */
	} else {
		draw_flag = 3;	/* ���� */
	}

	/* ������� */
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
		/* ���󷵻� */
		sprintf(acErrMsg, "���ڲ�Ʒ���д������ʹ��� %c", g_td_parm.term_type);
		WRITEMSG
			strcpy(reply, "W108");
		return 1;
	}

	strcpy(intst_term_type, g_td_parm.intst_term_type);	/***��ʵ��������Ϣ***/

	if (intst_term_type[0] != '0' && intst_term_type[0] != '1') {
		sprintf(acErrMsg, "��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
			g_td_parm.intst_term_type);
		WRITEMSG
			MEMSET_DEBUG(reply, 0x0, sizeof(reply));
		strcpy(reply, "W107");
		return 1;
	}
	sprintf(acErrMsg, "��ʼ������Ϣmonterm=[%d],draw_flag=[%d]", monterm, draw_flag);
	WRITEMSG

		if (monterm <= 24) {

		sprintf(acErrMsg, "��ͨ��Ϣ����");
		WRITEMSG

		/* ��ͨ��Ϣ���� */
			if (g_td_mst.ic_date <= date93) {
			/* 93.03.01֮ǰ��������ȡ */

			sprintf(acErrMsg, "��ͨ��Ϣ����--930301֮ǰ��������ȡ");
			WRITEMSG

				switch (draw_flag) {
			case 1:

				/*
				 * ��ǰ֧ȡʵ��ͬ���ζ������� 1.���ݿ�������
				 * �������� �Ͳ�Ʒ �綨������
				 * 2.���������븳��Ӧ�����ʱ��,����ѯ������
				 * 3.������Ϣ 4.��������˰ 
				 */

				sprintf(acErrMsg, "��ͨ��Ϣ����--��ǰ֧ȡ");
				WRITEMSG

				/* ��ʵ��ʱ��ͬ���ζ������ʼ�Ϣ �綨���� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "�綨���޴���");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "�綨����:llbh=[%s]", llbh);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "������������!!");
					WRITEMSG
						strcpy(reply, "W102");
					return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

				/* llbh �������ʱ�źͽ������ڲ�ѯ���� */
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
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
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
					g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* ��������˰ */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
									   g_td_mst.ic_date), g_pub_tx.tx_date, intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* �����Ϣ˰����Ϣ */
					amtval = lx * daytermval / dayterm;

					sprintf(acErrMsg, "dayterm=[%ld],daytermval=[%ld],lx=[%lf],amtval=[%lf],flt_ratio=[%lf]", dayterm, daytermval, lx, amtval, g_td_mst.flt_ratio);
					WRITEMSG

					/* ȡ��Ϣ˰˰�� */
						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					sprintf(acErrMsg, "��Ϣ˰˰��sdsl=[%lf] ", sdsl);
					WRITEMSG

						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0.001)*/
				fprintf(ifp, "-��Ϣ˰|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					PubMax(date99, g_td_mst.ic_date) , g_pub_tx.tx_date, daytermval, 
						amtval, sdsl, val, g_pub_tx.tx_amt1+*dealval-*val);
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:
				/* ����֧ȡ��Ϣ���� */
			case 3:
				/*
				 * ����֧ȡ��Ϣ���� 1.�������ʱ�źͿ�������
				 * ��Ϣ���� �������ڲ�ѯ���ֶ����ʱ�
				 * ����������ʺʹ���֮�� 2.���㵽����Ϣ
				 * 3.�������,����������Ϣ 4.�����������˰ 
				 */

				sprintf(acErrMsg, "��ͨ��Ϣ����--���ڻ�����֧ȡ");
				WRITEMSG

				/* ���㵽������*����ʱ�� �ֶμ��� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, " llbh=[%s],cur_no=[%s],ic_date=[%ld],mtr_date=[%ld]",
					llbh, g_td_parm.cur_no, g_td_mst.ic_date, g_td_mst.mtr_date);
				WRITEMSG

					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
				sprintf(acErrMsg, " ���� : ����ֶ�����*���� = [%lf]", a1);
				WRITEMSG

				/* ������������*����ʱ�� */

				/*
				 * date93 ֮ǰ��ԭ���������ʼ��� date93
				 * ֮��֧ȡ�չ��ƹ���Ļ��ڴ������ʼ��� 
				 */

					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(g_td_mst.mtr_date, date93,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;	/* ����Ϣ */
				}

				/* ȡ��Ӧ���ڵ����� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a2 = dayterm * rate_val;	/* date93ǰ */
				if(a2 > 0.001)
				fprintf(ifp, "+93ǰ������Ϣ|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|------|\n", g_td_mst.mtr_date,
					date93, dayterm, g_pub_tx.tx_amt1, rate_val, a2);

				sprintf(acErrMsg, " ���� (date93ǰ) : ����ֶ�����*���� = [%lf]", a2);
				WRITEMSG


					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
									   date93), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;
				}

				/* �������ʱ�� ��������ȡ����ʱ���ƻ�����Ϣ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a3 = dayterm * rate_val;	/* date93�� */
				if(a3 > 0.001)
				fprintf(ifp, "+93��������Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------|\n", PubMax(g_td_mst.mtr_date,
					date93) , g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, a3);

				sprintf(acErrMsg, " ���� (date93��) : ����ֶ�����*���� = [%lf]", a3);
				WRITEMSG

					lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = pub_base_PubDround_1(lx);
				fprintf(ifp, "�ϼ�|%.08ld-%.08ld|X|XXX|XXX|XXX|%.2lf|\n", g_td_mst.ic_date,
					  g_pub_tx.tx_date, *dealval);

				/* ������Ϣ˰ */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.mtr_date), g_pub_tx.tx_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* Ӧ��˰��Ϣ */
					(*val) = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					/* ȡ��Ϣ˰��Ų�ȡ���� */
					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
					(*val) = pub_base_PubDround_1((*val) * sdsl);
					/*if(*val > 0.001)	*/
					fprintf(ifp, "-��Ϣ˰|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|-------|\n", 
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
		} else {	/* 93.03.01 ������������ȡ */

			sprintf(acErrMsg, "��ͨ��Ϣ����--930301֮������������ȡ");
			WRITEMSG

				switch (draw_flag) {
			case 1:
				/*
				 * ��ǰ֧ȡ
				 * ��֧ȡ�չ��ƹ���Ļ��ڴ��������ʼƸ���Ϣ
				 * 1.�����Ϣ���ڵ��������ڵ�������
				 * 2.ȡ��ǰ���ƹ���Ļ��ڴ���������
				 * 3.�������� 4.99��������Ҫ����������˰ 
				 */

				sprintf(acErrMsg, "��ͨ��Ϣ����--��ǰ֧ȡ");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "������������!!");
					WRITEMSG
						return 1;
				}
				/* ȡ�������� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);

				sprintf(acErrMsg, "���� : llbh=[%s],cur_no=[%s],date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				sprintf(acErrMsg, "��Ϣ : dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				lx = pub_base_PubDround_1(lx);
				(*dealval) = lx;
				if(*dealval > 0.001)
				fprintf(ifp, "+��ǰ֧ȡ��Ϣ|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
				if (floor(lx * 100) / 100 < 0.005) {
					lx = 0.00;
					(*dealval) = 0.00;
					(*factval) = 0.00;
					(*val) = 0.00;
				}
				/* �����������˰ */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.ic_date), g_pub_tx.tx_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					amtval = lx * daytermval / dayterm;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				if(*val > 0.001)
				fprintf(ifp, " - ��˰|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
							g_td_mst.ic_date), g_pub_tx.tx_date, dayterm, amtval, sdsl, *val,  
										pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)-(*val))) ;

				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:

				/*
				 * ����֧ȡ 1. ���㵽������ 2.
				 * ȡ��Ӧ��Ʒ�������� 3. ������Ϣ 4.
				 * 99������������˰ 
				 */
			case 3:
				/*
				 * ����֧ȡ 1.930301--930711 ��������Ϣ����
				 * 2.930711--������ ��930711ͬ�������ʼ���
				 * 3.������--������ �������ջ������ʼ���
				 * 4.����99��ĸ�������˰ 
				 *
				 */

				sprintf(acErrMsg, "��ͨ��Ϣ����-- ���ڻ�����֧ȡ");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG

					if (monterm <= 6) {
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
								    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* ȡ�ò�Ʒ�Ķ�Ӧ������ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[%ld],��������[%ld]",
						llbh, g_td_parm.cur_no, g_td_mst.ic_date, dayterm);
					WRITEMSG
						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "����:dayterm=[%ld],rate_val=[%lf],tx_amt=[%d],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = lx;
					sprintf(acErrMsg, "ע�⵽����Ϣ:[%lf]", lx);
					WRITEMSG
				if(*dealval > 0.001)
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
									g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
									pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));

					/* ������Ϣ */
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
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						vtcp_log("[%s][%d]���ں������[%ld]\n", __FILE__, __LINE__, dayterm);
					} else {
						dayterm = 0;

					}
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "����: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date, dayterm);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					sprintf(acErrMsg, "������Ϣ: dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],\
        flt_ratio=[%lf] lx[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio, lx);
					WRITEMSG
						(*dealval) = (*dealval) + lx;
				if(lx > 0)
				fprintf(ifp, "+������Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));
					sprintf(acErrMsg, "ע������Ϣ: dealval[[%lf]", *dealval);
					WRITEMSG
					/* �����������˰ */

						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						/* ���ʱ�ŴӲ�Ʒ��ȡ */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "���ڲ���Ӧ�ý���Ϣ˰�Ĳ���1:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval = rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.mtr_date), g_pub_tx.tx_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "��Ϣ˰:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "���ڲ���Ӧ�ý���Ϣ˰�Ĳ���2:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf], \
   flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;
						sprintf(acErrMsg, "���ڲ���Ӧ�ý�˰����Ϣ��:lx[%lf]", lx);
						WRITEMSG
							sprintf(acErrMsg, "����Ӧ��Ӧ��˰����Ϣ��amtval[%lf]", amtval);
						WRITEMSG
							amtval = amtval + lx;
						sprintf(acErrMsg, "�ܵ�Ӧ�ý�˰����Ϣ��amtval[%lf]", amtval);
						WRITEMSG
							ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
					if(*val > 0)		
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|--|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
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
					sprintf(acErrMsg, "��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				} else {	/* monterm >6 */

					sprintf(acErrMsg, "���ڴ���������");
					WRITEMSG
						ret = pub_base_CalIntstDays(g_td_mst.ic_date, date9307,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* ȡ930711ʱ�������� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
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
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, date9307,
						 dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

						ret = pub_base_CalIntstDays(PubMax(date9307,
					g_td_mst.ic_date), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/*
					 * ��930711�󿪻�,ȡ�ò�Ʒ�Ķ�Ӧ������
					 * 
					 * ����,ȡpubmax(930711,ic_date)����ͬ
					 * �������� 
					 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
								      PubMax(date9307, g_td_mst.ic_date), &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
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
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date9307,
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
					/* ������Ϣ���� */
						if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
						ret = pub_base_CalIntstDays(g_td_mst.mtr_date,
									    g_pub_tx.tx_date, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
					} else {
						dayterm = 0;
					}

					/* ȡ��������  */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "����: llbh=[%s],cur_no=[%s],date=[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ����[%s][%s][%ld][%.2lf]", llbh, g_td_parm.cur_no, g_pub_tx.tx_date, rate_val);
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
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
                      g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

					/* �����������˰ */
						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						/* ȡ��Ʒ������ʱ�� */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "��Ϣ˰: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "��Ϣdayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
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
								sprintf(acErrMsg, "������������!!");
								WRITEMSG
									return 1;
							}
						} else {
							daytermval = 0;

						}

						/* ȡ�������� */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "��Ϣ: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "��Ϣdayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
							daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval += rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0)	*/
				fprintf(ifp, "-��Ϣ˰|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
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
					sprintf(acErrMsg, "��Ϣ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				}
				break;
			default:
				/* ���󷵻� */
				return 1;
			}
		}
	} else {		/* monterm >24 */

		sprintf(acErrMsg, "��ֵ��Ϣ����");
		WRITEMSG

		/* ��ֵ��Ϣ���� */
			switch (draw_flag) {
		case 1:
			/* δ����֧ȡ */

			sprintf(acErrMsg, "δ����֧ȡ ");
			WRITEMSG

				if (g_td_mst.ic_date >= date93) {	/* date93֮�󿪻��� */

				sprintf(acErrMsg, "date93 ֮�󿪻���,���������ʼ�Ϣ ");
				WRITEMSG

				/* �����ڻ������ʼ��� */
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "������������!!");
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
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
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
				fprintf(ifp, "+��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
        g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* ������Ϣ˰ */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)	*/
				fprintf(ifp, "-��Ϣ˰|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(g_td_mst.ic_date,
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
			 /* end of date93֮�󿪻��� */ 
			else {	/* date93 ֮ǰ������ */

				sprintf(acErrMsg, "date93 ֮ǰ������,���������ʼ�Ϣ ");
				WRITEMSG

				/*
				 * ��ic_date,tx_date�ֶμ�Ϣ,��������,���ָߵ�
				 *  
				 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "�綨���޴���");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],ic_date=[%ld],tx_date=[%ld],\
        intst_term_type=[%d]", llbh, g_td_parm.cur_no, g_td_mst.ic_date,
					g_pub_tx.tx_date, intst_term_type);
				WRITEMSG

				/* rate_val = ���� * ����֮�� */
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "����ֶλ�������");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "�ֶλ���=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
					rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = lx;

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					/* �綨���� */
					/* �ӿ����յ�PubMin(������,date99) */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

					ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
								 g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99),
							   intst_term_type);
					if (ret != 0) {
						sprintf(acErrMsg, "�綨���޴���");
						WRITEMSG
							return 1;
					}
					/* rate_val = ���� * ����֮�� */
					ret = pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "����ֶλ�������");
						WRITEMSG
							return 1;
					}
					sprintf(acErrMsg, "lx=[%lf],�ֶλ���=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						lx, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = lx - rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)*/
				fprintf(ifp, "-��Ϣ˰|%.08ld-%.08ld|---|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99) ,  lx, sdsl, *val, 
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
			/* ����֧ȡ */
			/*
			 * 1. 88/09/10
			 * ��ǰ�����ı�ֵ����,��ֵ�ڴ�88/09/10�������� 2.
			 * 89/02/01
			 * �Ժ��ڵı�ֵ����,��88/09/10��ִ��89/02/01������
			 * �Ĵ��������� 3. 91/12/01--93/03/01
			 * ֮�����ı�ֵ����,��ֵ�ڴ�93/07/11 �������� 4.
			 * 93/03/01--93/07/11
			 * ֮������һ�������ϴ�����,�ӿ����� ��93/07/11
			 * ��93/07/11�����Ļ������ʼ�Ϣ,93/07/11��������
			 * ��93/07/11������ͬ���ζ������ʼ�Ϣ 5.
			 * 93/07/11(��)�������������ڰ��浥��ԭ�������ʼ�Ϣ 
			 */
		case 3:
			/* ����֧ȡ */

			sprintf(acErrMsg, "����������Ϣ����");
			WRITEMSG

				if (g_td_mst.ic_date < date88) {

				sprintf(acErrMsg, "----------->��Ϣ����С��88.09.10");
				WRITEMSG

					if (g_td_mst.mtr_date >= date89) {
					/* �ӿ����յ�date89�ֶμ�Ϣ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.opn_date, date89, intst_term_type,
								  Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
					ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* date89 ��Ӧ���ֵ����� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = a1 + dayterm * rate_val;	
				if(dayterm*rate_val > 0)
				  fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", date88, g_td_mst.mtr_date,
    				dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
         						 * (1 + g_td_mst.flt_ratio) / L360 / L100);

				} else {
					/* �ӿ����յ������շֶμ�Ϣ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.opn_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
				}

				/* ���㱣ֵ��Ϣ */
				/* ȡ�����յı�ֵ���� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, BAOZHI);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "������������!!");
					WRITEMSG
						return 1;
				}
				(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 / L360 / L100;
				if(*keepval >0)	
				fprintf(ifp, "+��ֵ��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date88, g_td_mst.mtr_date,
					 dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

			} else {/* 88.9.10�� */

				sprintf(acErrMsg, "----------->��Ϣ���ڴ���88.9.10");
				WRITEMSG
					if (g_td_mst.ic_date < date91) {

					sprintf(acErrMsg, "����������Ϣ����,88.9.10�󿪻���,date91ǰ����");
					WRITEMSG

						ret = pub_base_CalIntstDays(g_td_mst.ic_date,
									    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					/* ȡdate89���յ���Ӧ�Ĵ��ֵ����� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = rate_val * dayterm;
				if(a1>0)
				 fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------  |\n", g_td_mst.ic_date, g_td_mst.mtr_date,
 dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


					ret = pub_base_CalIntstDays(date99, g_td_mst.mtr_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					s1 += rate_val * (double) daytermval;


					/* ȡ�����յı�ֵ���� */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, BAOZHI);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					      g_td_mst.mtr_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
				if(*keepval > 0)
				fprintf(ifp, "+��ֵ��Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date99, g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

					s2 += rate_val * daytermval;

				}
				 /* end of date91 ǰ���� */ 
				else {	/* date91 �󿪻��� */
					if (g_td_mst.ic_date < date93) {

						sprintf(acErrMsg, "����������Ϣ����,date91�󿪻�,date93 ǰ����");
						WRITEMSG

						/*
						 * �Ӽ�Ϣ�յ������հ��ֶ����ʼ
						 * �Ϣ 
						 */
							MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
						if (ret != 0) {
							sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						/*
						 * ������
						 * PubMin(�����գ�date99) 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, PubMin(g_td_mst.mtr_date,
												   date99), intst_term_type, Lone, &s1_tmp, NULL);
						if (ret != 0) {
							sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						s1 += a1 - s1_tmp;

					} else {	/* 1993/03/01֮�� */

						sprintf(acErrMsg, "����������Ϣ����,date93 �󿪻�");
						WRITEMSG

							ret = pub_base_CalIntstDays(g_td_mst.ic_date,
										    date9307, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						/* ȡdate9307�������� */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "date93,date9307���ʱ�䰴���ڼ�Ϣ,llbh=[%s],cur_no=[%s],\
        date=9307", llbh, g_td_parm.cur_no);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307���ʱ�䰴���ڼ�Ϣ,date9307ʱ��rate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a1 = rate_val * dayterm;
						if (a1>0)
						 fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf| -------  |\n", g_td_mst.ic_date,
 date9307, dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


						sprintf(acErrMsg, "date93,date9307���ʱ�䰴���ڼ�Ϣ,����Ϊ=[%lf]", a1);
						WRITEMSG

							ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						date9307), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
							WRITEMSG
								return 1;
						}
						/*
						 * ȡPubMax(��������,date9307)
						 * �յ���Ӧ������ 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "date9307�󵽵����հ�date97ͬ�������ʼ�Ϣ,llbh=[%s], \
		cur_no=[%s],date=[%ld]", llbh, g_td_parm.cur_no,
							PubMax(g_td_mst.ic_date, date9307));
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
									      PubMax(g_td_mst.ic_date, date9307), &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307���ʱ�䰴���ڼ�Ϣ,date9307ʱ��rate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a2 = rate_val * dayterm;
								if(a2>0)
								fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.ic_date,
date9307), g_td_mst.mtr_date,
								  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
								 * (1 + g_td_mst.flt_ratio) / L360 / L100);

						sprintf(acErrMsg, "date9307�󵽵�����,���ʱ�����Ϊ=[%lf]", a2);
						WRITEMSG

							a1 = a1 + a2;

						ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						 date99), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "������������!!");
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
                    		sprintf(acErrMsg,"������������!!");
                    		WRITEMSG
                    		return 1;
                		}
  
                        * ȡ�����յı�ֵ���� *
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);
  
sprintf(acErrMsg,"�����յı�ֵ���� llbh=[%s],cur_no=[%s],mtr_date=[%ld]",
       llbh,g_td_parm.cur_no,g_td_mst.mtr_date);
WRITEMSG 
  
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }
  
sprintf(acErrMsg,"��ֵ���� rate_val=[%lf],dayterm=[%ld],tx_amt=[%lf],\
        flt_ratio=[%lf]",rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG 
  
                        (*keepval) = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
  
sprintf(acErrMsg, "��ֵ��Ϣ: keepval=[%lf]", *keepval);
WRITEMSG
						******/

					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "������������!!");
						WRITEMSG
							return 1;
					}
					s2 = 0.00;
					s2 = rate_val * dayterm;

				}

			}

			/* ������Ϣ */
			/* �����յ�date93���ֶ����ʼ�Ϣ */

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.rate_no);

			sprintf(acErrMsg, "�������� �����յ�date93,llbh=[%s],cur_no=[%s]",
				llbh, g_td_parm.cur_no);
			WRITEMSG
				if (g_td_mst.mtr_date < date93) {
				ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
							  g_td_mst.mtr_date, date93, intst_term_type, Lone, &a2, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "����ֶ�����*�������� [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
			} else {
				a2 = 0.00;
			}

			sprintf(acErrMsg, "�������� �����յ�date93,��ηֶλ���Ϊ: [%lf]", a2);
			WRITEMSG

			/* ȡ���׵��յĻ������� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

			sprintf(acErrMsg, "�������� date93��,llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
			WRITEMSG

				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
			if (ret != 0) {
				sprintf(acErrMsg, "��com_rateȡ����ֵ���� [%d]", ret);
				WRITEMSG
					strcpy(reply, "W110");
				return ret;
			}
			ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date93),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "������������!!");
				WRITEMSG
					return 1;
			}
			sprintf(acErrMsg, "�������� date93��  rate_val= [%lf],dayterm=[%ld]",
				rate_val, dayterm);
			WRITEMSG
				a3 = 0;
			a3 = rate_val * dayterm;
						if(rate_val > 0 && dayterm > 0)
						 fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.mtr_date, date93),
             g_pub_tx.tx_date,
  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


			sprintf(acErrMsg, "�������� date93����Ϣ����Ϊ: [%lf]", a3);
			WRITEMSG

				ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date99),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "������������!!");
				WRITEMSG
					return 1;
			}
			s1 += rate_val * dayterm;

			sprintf(acErrMsg, "������Ϣ����=[%lf],����9307ǰ����=[%lf],����9307�����=\
        [%lf],tx_amt=[%lf],flt_ratio=[%lf]", a1, a2, a3, g_pub_tx.tx_amt1,
				g_td_mst.flt_ratio);
			WRITEMSG
				lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			(*dealval) = lx;

			sprintf(acErrMsg, "--------->��ϢֵΪ:[%lf]", *dealval);
			WRITEMSG

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
				ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
				if (ret != 0 && ret == 1) {
					sprintf(acErrMsg, "���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",
						ret);
					WRITEMSG
						strcpy(reply, "W100");
					return 1;
				} else if (ret < 0) {
					sprintf(acErrMsg, "ȡ��Ϣ˰˰�ʴ��� %d", ret);
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
				fprintf(ifp, "-��Ϣ˰| ----  | -- |%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					 		 lx, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
			}
			(*dealval) = pub_base_PubDround_1(*dealval);
			(*val) = pub_base_PubDround_1(*val);
			(*keepval) = pub_base_PubDround_1(*keepval);

			(*factval) = (*dealval) - (*val);
			(*keepval) = (*keepval);	/* ˰��ֵ��Ϣ */

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
* ��������    f_pub_base_getsecrate()
* �������ܣ�  �������ʱ�� ���� ��Ϣ�� ������ ģʽ,����ֶ�����*����֮��
* ����/ʱ�䣺 2003��01��05��
*
* ������
*     ���룺 ���ʱ�� rate_no    char *
*            ����     cur_no     char *
*            ��Ϣ���� ic_date    long
*            �������� tx_date    long
*            ������������ intst_term_type char 
*                   0--��ʵ����������    1--��ÿ��30�����
*            ģʽ     mode       int  
*                   0--��������,���ָߵ� 1--���߾͸�,���Ͳ���
*     ���:
*            ����*���� rate_val  double *
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int f_pub_base_getsecrate(char *rate_no, char *cur_no, long ic_date,
        long tx_date, char intst_term_type[2], int mode, double *rate_val, FILE *ifp)
{
    struct com_rate_c com_rate_c;
    char rateno[4];
    int ret;
    double day_val; /* ����*����֮�� */
    double rate_val_last, llz; /* ��Ϣ��ǰһ������ֵ */
    long   beg_date;      /* ǰһ�����ʼ������ʼ���� */
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
            sprintf(acErrMsg,"������������!!");
            WRITEMSG
            return 1;
        }
        
        sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
        WRITEMSG
      
        day_val += dayterm * rate_val_last;

	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
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
        sprintf(acErrMsg,"������������!!");
        WRITEMSG
        return 1;
    }
    
    sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
    WRITEMSG

    day_val += rate_val_last * dayterm ;
    (*rate_val) = day_val ;
	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+�ֶ���Ϣ|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
		tx_date, dayterm, g_pub_tx.tx_amt1, rate_val_last,  day_val* floor(g_pub_tx.tx_amt1 * L100) / L100
          * (1 + g_td_mst.flt_ratio) / L360 / L100);

    Com_rate_Clo_Sel();

    return 0;
}

