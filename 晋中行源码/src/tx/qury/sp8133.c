/*************************************************
* ÎÄ ¼ş Ãû:  sp8133.c
* ¹¦ÄÜÃèÊö£º 
*
* ×÷    Õß:  
* Íê³ÉÈÕÆÚ£º 
*
* ĞŞ¸Ä¼ÇÂ¼£º
* ÈÕ   ÆÚ: 20070417
* ĞŞ ¸Ä ÈË: gujy
* ĞŞ¸ÄÄÚÈİ: ÕûÕû×ª´æÊÔ½áÀûÏ¢²éÑ¯
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
	int             ttlnum = 0;	/**¶ÁÈ¡µÄ×ÜÌõÊı**/
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

	/**×é³É²éÑ¯Ìõ¼ş**/
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
		sprintf(acErrMsg, "·ÇÕû´æÕûÈ¡»§![%c]", g_td_parm.td_type[0]);
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
			/**ÏÔÊ¾ÁĞ±êÌâ**
			***/
			/*fprintf(fp, "~Ê¼ÈÕÆÚ|Ö¹ÈÕÆÚ|ÌìÊı|$ÀûÂÊ|$Ë°ÂÊ|$±¾½ğ|$ÀûÏ¢|$ÀûÏ¢Ë°|$ºÏ¼Æ|\n");*/
			fprintf(fp, "~ÃèÊö|Ê¼Ö¹ÈÕÆÚ|ÌìÊı|$±¾½ğ|$ÀûÂÊ|$ÖÕÖµ|$ºÏ¼Æ|\n");
		}
	vtcp_log("%d@%s  ttlnum=[%d] CHANGECODE 00cceeffaa989aabbb filename{%s}\n", __LINE__, __FILE__, ttlnum, filename);
		/*if(0 != ilist_calculate_intst( &f_td_mst, fp, &ttlnum)){
			 sprintf(acErrMsg,"RET[%d]",ret); 
                	WRITEMSG 
                	goto ErrExit; 
		}*/
		{
/**------- hao -------- Ôö¼Ó¶¨ÆÚ×Ô¶¯×ª´æ¼ÆÏ¢ ---------**/

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
                                strcpy(acErrMsg, "¼ÆËãÕû´æÕûÈ¡ÀûÏ¢³ö´í!!");
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
	/* È¡µÃ¸öÈËËùµÃË°Ë°ÂÊ */
	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &tax_rate);	/* ´Ëº¯ÊıÒÑÖÃÁËreply */
	if (ret) {
		sprintf(acErrMsg, "È¡¸öÈËËùµÃË°ÂÊ´íÎó[%d]!", ret);
		return 1;
	}
	/* ¸öÈË¶¨ÆÚ£¬ÁãÕû£¬½ÌÓı£¬´æ±¾È¡Ï¢  ÔÚÁ½¸ö¼ÆÌáÖÜÆÚÄÚÏú»§µÄÒ²µÃÌá */
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
		/* ÆğÏ¢ÈÕ´óÓÚµ±Ç°ÈÕÆÚ */
		if (is_date > g_pub_tx.tx_date) {
			sprintf(acErrMsg, "ÆğÏ¢ÈÕÆÚ´íÎó![%d]", ret);
			WRITEMSG
				return (-1);
		}
		if (wd_td_mst.ac_sts[0] == '*') {
			sprintf(acErrMsg, "ÕË»§ÒÑÏú»§");
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
			 * sprintf(acErrMsg,"²éÕÒ¿ªÏú»§µÇ¼Ç²¾´íÎó![%d]",ret);
			 * WRITEMSG return(ret); }
			 * wd_td_mst.bal=wd_mo_opn_cls.amt;
			 * ===================================================
			 * === 
			 */
			/* ½«Ïú»§Ê±µÄÏú»§½ğ¶îÔÚ¸³»ØÈ¥ÎªÁËÏÂÃæËãÀûÏ¢ */
		}
		/** È¡²úÆ·ĞÅÏ¢ **/
		ret = Td_parm_Sel(g_pub_tx.reply, &wd_td_parm, "prdt_no='%s'", wd_td_mst.prdt_no);
		if (ret) {
			sprintf(acErrMsg, "²éÕÒ¶¨ÆÚÕÊ»§´íÎó![%d]", ret);
			WRITEMSG
				return (ret);
		}
		/** ¸ù¾İ¶¨ÆÚ²úÆ·È¡ÀûÂÊ **/
		ret = pub_base_getllz(wd_td_parm.rate_no, wd_td_parm.cur_no,
				      wd_td_mst.ic_date, &td_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "È¡ÀûÂÊ´íÎó![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (memcmp(wd_td_mst.prdt_no, "257", 3) == 0 || memcmp(wd_td_mst.prdt_no, "258", 3) == 0) {
			vtcp_log("Ğ­Òé´æ¿îÈ¡·Ö»§ÖĞµÄÀûÂÊ[%ld]\n", wd_td_mst.ac_id);
			td_rate = wd_td_mst.rate;
		}
		/** È¡»îÆÚ²úÆ·ÀûÂÊ **/
		ret = pub_base_getllz("100", wd_td_parm.cur_no,
				      g_pub_tx.tx_date, &dd_rate);
		if (ret != 0) {
			sprintf(acErrMsg, "È¡ÀûÂÊ´íÎó![%d]", ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "W110");
			return ret;
		}
		if (wd_td_mst.tfr_ind[0] == 'Y') {	/** ×Ô¶¯×ª´æµÄ **/
			if (wd_td_mst.mtr_date < g_pub_tx.tx_date) {	/** ÒÑ¾­¹ıµ½ÆÚÈÕµÄ **/
				/** ¼ÆËãµ±ÆÚ¼ÆËãÀûÏ¢µÄÓà¶î **/
				if (wd_td_mst.ic_date < DATE061104) {	/* ÉÏÏßÇ°¿ª»§Ã¿ÔÂ30ÌìËã */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						vtcp_log("FUN%s@L%d@F%s, wihleCnt- %d\n", __FUNCTION__, __LINE__, __FILE__, ++whileCnt);
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "1", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
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

						/** ÖØĞÂµ÷ÕûÆğÏ¢µ½ÆÚÈÕ **/
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
						sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
						WRITEMSG
							return ret;
					}
					sig_intst = (bal_tmp * dd_rate * days) / (360 * 100);
					sig_intst = pub_base_PubDround_1(sig_intst);
					tax_tmp = sig_intst * tax_rate;
					tax_tmp = pub_base_PubDround_1(tax_tmp);
					fprintf(ifp, "%08ld|%08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n", is_date, g_pub_tx.tx_date, days, dd_rate, tax_rate, bal_tmp, sig_intst, tax_tmp, bal_tmp+sig_intst-tax_tmp);
					*prln+=1;

				} else {	/* ÉÏÏßºó¿ª»§Êµ¼ÊÌìÊıËã */
					is_date_tmp = wd_td_mst.ic_date;
					ie_date_tmp = wd_td_mst.mtr_date;
					bal_tmp = wd_td_mst.bal;
					while (ie_date_tmp < g_pub_tx.tx_date) {
						ret = pub_base_CalDays(is_date_tmp, ie_date_tmp, "0", &days);
						if (ret != 0) {
							sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
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

						/** ÖØĞÂµ÷ÕûÆğÏ¢µ½ÆÚÈÕ **/
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
						sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
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
			} else {/** Ã»µ½ÆÚ»ò¸Õµ½ÆÚµÄ **/
				/* ÆğÏ¢ÈÕĞ¡ÓÚÉÏÏßÈÕ */
				if (wd_td_mst.ic_date < DATE061104) {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "1", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
						WRITEMSG
							return ret;
					}
				} else {
					ret = pub_base_CalDays(is_date, g_pub_tx.tx_date, "0", &days);
					if (ret != 0) {
						sprintf(acErrMsg, "ÌìÊı¼ÆËã´íÎó![%d]", ret);
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
* º¯ÊıÃû£º    pub_base_CalIntstTdZCZQ()
* º¯Êı¹¦ÄÜ£º  ¼ÆËãÕû´æÕûÈ¡ÀûÏ¢
* ×÷Õß/Ê±¼ä£º 2003Äê12ÔÂ28ÈÕ
*
* ²ÎÊı£º
*     ÊäÈë£º
*     Êä³ö: Ó¦¸¶ÀûÏ¢          double
*           Êµ¸¶ÀûÏ¢          double
*           ´ú¿ÛÀûÏ¢Ë°        double
*           ±£ÖµÀûÏ¢          double
*           ÏìÓ¦Âë            char(4)
* ·µ »Ø Öµ: 0 ³É¹¦
*
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int 
f_pub_base_CalIntstTdZCZQ(double *dealval, double *factval,
			double *val, double *keepval, char *reply, FILE *ifp)
{
	int             draw_flag;	/* µ½ÆÚ±êÖ¾ 
					 * 1-ÌáÇ°Ö§È¡,2-µ½ÆÚÖ§È¡,3-ÓâÆÚÖ§È¡ */
	int             monterm, monterm1;
        long 		dayterm;	/* ÔÂ ÈÕ´æÆÚ */
	long             daytermval;	/* ´ú¿Û¸öÈËËùµÃÀûÏ¢ÌìÊı */
	double          amtval;	/* ¼ÆËãËùµÃË°½ğ¶î */
	double          sdsl;	/* ¸öÈËËùµÃË°ÂÊ */
	double          rate_val;	/* ÀûÂÊ */
	double          lx;	/* ÀûÏ¢ */
	long            date88, date89, date93, date9307, date99;
	long            date91, date97, date98, date00;
	char            llbh[3];/* ÀûÂÊ±àºÅ */
	int             ret;
	double          a1, a2, a3, s1, s1_tmp, s2;	/* ´æÆÚ*ÀûÂÊ */
	char            intst_term_type[2];	/* ¼ÆËãÌìÊıÀàĞÍ */


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
	(*keepval) = 0.00;	/* ±£ÖµÀûÏ¢ */
	(*dealval) = 0.00;	/* Ó¦¸¶ÀûÏ¢ */
	(*factval) = 0.00;	/* Êµ¸¶ÀûÏ¢ */
	(*val) = 0.00;		/* ´ú¿ÛÀûÏ¢Ë° */

	/* ÅĞ¶Ï½»Ò×ÈÕÆÚÊÇ·ñµ½ÆÚ */
	if (g_pub_tx.tx_date < g_td_mst.mtr_date) {
		draw_flag = 1;	/* ÌáÇ° */
	} else if (g_pub_tx.tx_date == g_td_mst.mtr_date) {
		draw_flag = 2;	/* µ½ÆÚ */
	} else {
		draw_flag = 3;	/* ÓâÆÚ */
	}

	/* ¼ÆËã´æÆÚ */
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
		/* ´íÎó·µ»Ø */
		sprintf(acErrMsg, "¶¨ÆÚ²úÆ·±íÖĞ´æÆÚÀàĞÍ´íÎó %c", g_td_parm.term_type);
		WRITEMSG
			strcpy(reply, "W108");
		return 1;
	}

	strcpy(intst_term_type, g_td_parm.intst_term_type);	/***°´Êµ¼ÊÌìÊı¼ÆÏ¢***/

	if (intst_term_type[0] != '0' && intst_term_type[0] != '1') {
		sprintf(acErrMsg, "²úÆ·±íÖĞµÄ¼ÆÏ¢ÌìÊıÀàĞÍ´íÎó type=[%s]",
			g_td_parm.intst_term_type);
		WRITEMSG
			MEMSET_DEBUG(reply, 0x0, sizeof(reply));
		strcpy(reply, "W107");
		return 1;
	}
	sprintf(acErrMsg, "¿ªÊ¼¼ÆËãÀûÏ¢monterm=[%d],draw_flag=[%d]", monterm, draw_flag);
	WRITEMSG

		if (monterm <= 24) {

		sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã");
		WRITEMSG

		/* ÆÕÍ¨ÀûÏ¢¼ÆËã */
			if (g_td_mst.ic_date <= date93) {
			/* 93.03.01Ö®Ç°µÄÕû´æÕûÈ¡ */

			sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã--930301Ö®Ç°µÄÕû´æÕûÈ¡");
			WRITEMSG

				switch (draw_flag) {
			case 1:

				/*
				 * ÌáÇ°Ö§È¡ÊµĞĞÍ¬µµ´Î¶¨ÆÚÀûÂÊ 1.¸ù¾İ¿ª»§ÈÕÆÚ
				 * ½»Ò×ÈÕÆÚ ºÍ²úÆ· ½ç¶¨ÆÚÏŞÂë
				 * 2.¸ù¾İÆÚÏŞÂë¸³ÏàÓ¦µÄÀûÂÊ±àºÅ,²¢²éÑ¯³öÀûÂÊ
				 * 3.¼ÆËãÀûÏ¢ 4.¼ÆËãËùµÃË° 
				 */

				sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã--ÌáÇ°Ö§È¡");
				WRITEMSG

				/* °´Êµ´æÊ±¼äÍ¬µµ´Î¶¨ÆÚÀûÂÊ¼ÆÏ¢ ½ç¶¨ÆÚÏŞ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "½ç¶¨ÆÚÏŞ´íÎó");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "½ç¶¨ÀûÂÊ:llbh=[%s]", llbh);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
					WRITEMSG
						strcpy(reply, "W102");
					return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

				/* llbh ¸ù¾İÀûÂÊ±àºÅºÍ½»Ò×ÈÕÆÚ²éÑ¯ÀûÂÊ */
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
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
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
					g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* ¼ÆËãËùµÃË° */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
									   g_td_mst.ic_date), g_pub_tx.tx_date, intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* Ğè¿ÛÀûÏ¢Ë°µÄÀûÏ¢ */
					amtval = lx * daytermval / dayterm;

					sprintf(acErrMsg, "dayterm=[%ld],daytermval=[%ld],lx=[%lf],amtval=[%lf],flt_ratio=[%lf]", dayterm, daytermval, lx, amtval, g_td_mst.flt_ratio);
					WRITEMSG

					/* È¡ÀûÏ¢Ë°Ë°ÂÊ */
						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					sprintf(acErrMsg, "ÀûÏ¢Ë°Ë°ÂÊsdsl=[%lf] ", sdsl);
					WRITEMSG

						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0.001)*/
				fprintf(ifp, "-ÀûÏ¢Ë°|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					PubMax(date99, g_td_mst.ic_date) , g_pub_tx.tx_date, daytermval, 
						amtval, sdsl, val, g_pub_tx.tx_amt1+*dealval-*val);
				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:
				/* µ½ÆÚÖ§È¡ÀûÏ¢´¦Àí */
			case 3:
				/*
				 * ÓâÆÚÖ§È¡ÀûÏ¢´¦Àí 1.¸ù¾İÀûÂÊ±àºÅºÍ¿ª»§ÈÕÆÚ
				 * ÆğÏ¢ÈÕÆÚ ½»Ò×ÈÕÆÚ²éÑ¯³ö·Ö¶ÎÀûÂÊ±í
				 * ²¢¼ÆËã³öÀûÂÊºÍ´æÆÚÖ®»ı 2.¼ÆËãµ½ÆÚÀûÏ¢
				 * 3.Èç¹ûÓâÆÚ,¼ÆËãÓâÆÚÀûÏ¢ 4.¼ÆËã¸öÈËËùµÃË° 
				 */

				sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã--µ½ÆÚ»òÓâÆÚÖ§È¡");
				WRITEMSG

				/* ¼ÆËãµ½ÆÚÀûÂÊ*µ½ÆÚÊ±¼ä ·Ö¶Î¼ÆËã */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, " llbh=[%s],cur_no=[%s],ic_date=[%ld],mtr_date=[%ld]",
					llbh, g_td_parm.cur_no, g_td_mst.ic_date, g_td_mst.mtr_date);
				WRITEMSG

					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
				sprintf(acErrMsg, " µ½ÆÚ : ¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı = [%lf]", a1);
				WRITEMSG

				/* ¼ÆËãÓâÆÚÀûÂÊ*ÓâÆÚÊ±¼ä */

				/*
				 * date93 Ö®Ç°°´Ô­¶©´æÆÚÀûÂÊ¼ÆËã date93
				 * Ö®ºó°´Ö§È¡ÈÕ¹ÒÅÆ¹«¸æµÄ»îÆÚ´¢ĞîÀûÂÊ¼ÆËã 
				 */

					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(g_td_mst.mtr_date, date93,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;	/* ²»¼ÆÏ¢ */
				}

				/* È¡ÏàÓ¦´æÆÚµÄÀûÂÊ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, g_td_parm.rate_no);

				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a2 = dayterm * rate_val;	/* date93Ç° */
				if(a2 > 0.001)
				fprintf(ifp, "+93Ç°ÓâÆÚÀûÏ¢|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|------|\n", g_td_mst.mtr_date,
					date93, dayterm, g_pub_tx.tx_amt1, rate_val, a2);

				sprintf(acErrMsg, " ÓâÆÚ (date93Ç°) : ¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı = [%lf]", a2);
				WRITEMSG


					if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,
									   date93), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
				} else {
					dayterm = 0;
				}

				/* ¸ù¾İÀûÂÊ±àºÅ ÀûÂÊÀàĞÍÈ¡³öµ±Ê±¹ÒÅÆ»îÆÚÀûÏ¢ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				a3 = dayterm * rate_val;	/* date93ºó */
				if(a3 > 0.001)
				fprintf(ifp, "+93ºóÓâÆÚÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------|\n", PubMax(g_td_mst.mtr_date,
					date93) , g_pub_tx.tx_date, dayterm, g_pub_tx.tx_amt1, rate_val, a3);

				sprintf(acErrMsg, " ÓâÆÚ (date93ºó) : ¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı = [%lf]", a3);
				WRITEMSG

					lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = pub_base_PubDround_1(lx);
				fprintf(ifp, "ºÏ¼Æ|%.08ld-%.08ld|X|XXX|XXX|XXX|%.2lf|\n", g_td_mst.ic_date,
					  g_pub_tx.tx_date, *dealval);

				/* ¼ÆËãÀûÏ¢Ë° */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.mtr_date), g_pub_tx.tx_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* Ó¦½ÉË°ÀûÏ¢ */
					(*val) = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					/* È¡ÀûÏ¢Ë°±àºÅ²¢È¡ÀûÂÊ */
					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
					(*val) = pub_base_PubDround_1((*val) * sdsl);
					/*if(*val > 0.001)	*/
					fprintf(ifp, "-ÀûÏ¢Ë°|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|-------|\n", 
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
		} else {	/* 93.03.01 ºó´æÈëµÄÕû´æÕûÈ¡ */

			sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã--930301Ö®ºó´æÈëµÄÕû´æÕûÈ¡");
			WRITEMSG

				switch (draw_flag) {
			case 1:
				/*
				 * ÌáÇ°Ö§È¡
				 * °´Ö§È¡ÈÕ¹ÒÅÆ¹«¸æµÄ»îÆÚ´¢Ğî´æ¿îÀûÂÊ¼Æ¸¶ÀûÏ¢
				 * 1.¼ÆËã¼ÆÏ¢ÈÕÆÚµ½½»Ò×ÈÕÆÚµÄ×ÜÌìÊı
				 * 2.È¡µ±Ç°¹ÒÅÆ¹«¸æµÄ»îÆÚ´¢Ğî´æ¿îÀûÂÊ
				 * 3.¼ÆËãÀûÂÊ 4.99Äêºó¸öÈËĞèÒª½»¸öÈËËùµÃË° 
				 */

				sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã--ÌáÇ°Ö§È¡");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
					WRITEMSG
						return 1;
				}
				/* È¡»îÆÚÀûÂÊ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);

				sprintf(acErrMsg, "»îÆÚ : llbh=[%s],cur_no=[%s],date=[%ld]",
				  llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
				WRITEMSG

					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				sprintf(acErrMsg, "ÀûÏ¢ : dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				lx = pub_base_PubDround_1(lx);
				(*dealval) = lx;
				if(*dealval > 0.001)
				fprintf(ifp, "+ÌáÇ°Ö§È¡ÀûÏ¢|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
				if (floor(lx * 100) / 100 < 0.005) {
					lx = 0.00;
					(*dealval) = 0.00;
					(*factval) = 0.00;
					(*val) = 0.00;
				}
				/* ¼ÆËã¸öÈËËùµÃË° */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(date99,
					g_td_mst.ic_date), g_pub_tx.tx_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					amtval = lx * daytermval / dayterm;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if ((ret != 0) && (ret == 1)) {
						sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d", ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d ", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
					(*val) = amtval * sdsl;
					(*val) = pub_base_PubDround_1(*val);
				if(*val > 0.001)
				fprintf(ifp, " - ¸öË°|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
							g_td_mst.ic_date), g_pub_tx.tx_date, dayterm, amtval, sdsl, *val,  
										pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)-(*val))) ;

				}
				(*factval) = (*dealval) - (*val);

				sprintf(acErrMsg, "ÀûÏ¢ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
				WRITEMSG

					break;
			case 2:

				/*
				 * µ½ÆÚÖ§È¡ 1. ¼ÆËãµ½ÆÚÌìÊı 2.
				 * È¡¶ÔÓ¦²úÆ·µÄÈÕÀûÂÊ 3. ¼ÆËãÀûÏ¢ 4.
				 * 99ºó¼ÆËã¸öÈËËùµÃË° 
				 */
			case 3:
				/*
				 * ÓâÆÚÖ§È¡ 1.930301--930711 °´»îÆÚÀûÏ¢¼ÆËã
				 * 2.930711--µ½ÆÚÈÕ °´930711Í¬µµ´ÎÀûÂÊ¼ÆËã
				 * 3.µ½ÆÚÈÕ--½»Ò×ÈÕ °´½»Ò×ÈÕ»îÆÚÀûÂÊ¼ÆËã
				 * 4.¼ÆËã99ºóµÄ¸öÈËËùµÃË° 
				 *
				 */

				sprintf(acErrMsg, "ÆÕÍ¨ÀûÏ¢¼ÆËã-- µ½ÆÚ»òÓâÆÚÖ§È¡");
				WRITEMSG

					sprintf(acErrMsg, "ic_date=[%ld],tx_date=[%ld]", g_td_mst.ic_date, g_pub_tx.tx_date);
				WRITEMSG

					if (monterm <= 6) {
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
								    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* È¡¸Ã²úÆ·µÄ¶ÔÓ¦ÈÕÀûÂÊ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[%ld],¶¨ÆÚÌìÊı[%ld]",
						llbh, g_td_parm.cur_no, g_td_mst.ic_date, dayterm);
					WRITEMSG
						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_td_mst.ic_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					sprintf(acErrMsg, "µ½ÆÚ:dayterm=[%ld],rate_val=[%lf],tx_amt=[%d],flt_ratio=[%lf]",
						dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					(*dealval) = lx;
					sprintf(acErrMsg, "×¢Òâµ½ÆÚÀûÏ¢:[%lf]", lx);
					WRITEMSG
				if(*dealval > 0.001)
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
									g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
									pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));

					/* ÓâÆÚÀûÏ¢ */
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
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						vtcp_log("[%s][%d]ÓâÆÚºóµÄÌìÊı[%ld]\n", __FILE__, __LINE__, dayterm);
					} else {
						dayterm = 0;

					}
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "ÓâÆÚ: llbh=[%s],cur_no=[%s],date=[%ld],dayterm[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date, dayterm);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					lx = rate_val * dayterm * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;
					sprintf(acErrMsg, "ÓâÆÚÀûÏ¢: dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],\
        flt_ratio=[%lf] lx[%lf]", dayterm, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio, lx);
					WRITEMSG
						(*dealval) = (*dealval) + lx;
				if(lx > 0)
				fprintf(ifp, "+ÓâÆÚÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, pub_base_PubDround_1(g_pub_tx.tx_amt1+(*dealval)));
					sprintf(acErrMsg, "×¢Òâ×ÜÀûÏ¢: dealval[[%lf]", *dealval);
					WRITEMSG
					/* ¼ÆËã¸öÈËËùµÃË° */

						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						/* ÀûÂÊ±àºÅ´Ó²úÆ·ÀïÈ¡ */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "ÀûÏ¢Ë°:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "¶¨ÆÚ²¿·ÖÓ¦¸Ã½ÉÀûÏ¢Ë°µÄ²ÎÊı1:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval = rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.mtr_date), g_pub_tx.tx_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "ÀûÏ¢Ë°:llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "ÓâÆÚ²¿·ÖÓ¦¸Ã½ÉÀûÏ¢Ë°µÄ²ÎÊı2:dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf], \
   flt_ratio=[%lf]", daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							lx = daytermval * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;
						sprintf(acErrMsg, "ÓâÆÚ²¿·ÖÓ¦¸Ã½ÉË°µÄÀûÏ¢ÊÇ:lx[%lf]", lx);
						WRITEMSG
							sprintf(acErrMsg, "¶¨ÆÚÓ¦¸ÃÓ¦½ÉË°µÄÀûÏ¢ÊÇamtval[%lf]", amtval);
						WRITEMSG
							amtval = amtval + lx;
						sprintf(acErrMsg, "×ÜµÄÓ¦¸Ã½ÉË°µÄÀûÏ¢ÊÇamtval[%lf]", amtval);
						WRITEMSG
							ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
					if(*val > 0)		
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|--|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
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
					sprintf(acErrMsg, "ÀûÏ¢ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				} else {	/* monterm >6 */

					sprintf(acErrMsg, "´æÆÚ´óÓÚÁù¸öÔÂ");
					WRITEMSG
						ret = pub_base_CalIntstDays(g_td_mst.ic_date, date9307,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* È¡930711Ê±»îÆÚÀûÂÊ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
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
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, date9307,
						 dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));
					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

						ret = pub_base_CalIntstDays(PubMax(date9307,
					g_td_mst.ic_date), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/*
					 * ÔÚ930711ºó¿ª»§,È¡¸Ã²úÆ·µÄ¶ÔÓ¦ÈÕÀûÂÊ
					 * 
					 * ·ñÔò,È¡pubmax(930711,ic_date)µ±ÌìÍ¬
					 * µµ´ÎÀûÂÊ 
					 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);

					sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],date=[19930711]",
						llbh, g_td_parm.cur_no);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
								      PubMax(date9307, g_td_mst.ic_date), &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
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
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date9307,
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
					/* ÓâÆÚÀûÏ¢¼ÆËã */
						if (g_td_parm.ovrd_intst_ind[0] == 'Y') {
						ret = pub_base_CalIntstDays(g_td_mst.mtr_date,
									    g_pub_tx.tx_date, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
					} else {
						dayterm = 0;
					}

					/* È¡»îÆÚÀûÂÊ  */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, HQLLBH);

					sprintf(acErrMsg, "ÓâÆÚ: llbh=[%s],cur_no=[%s],date=[%ld]",
						llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
					WRITEMSG

						ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó[%s][%s][%ld][%.2lf]", llbh, g_td_parm.cur_no, g_pub_tx.tx_date, rate_val);
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
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.mtr_date,
                      g_pub_tx.tx_date , dayterm, g_pub_tx.tx_amt1, rate_val, lx, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

					sprintf(acErrMsg, "lx=[%lf]", lx);
					WRITEMSG

					/* ¼ÆËã¸öÈËËùµÃË° */
						if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
						ret = pub_base_CalIntstDays(PubMax(date99,
										   g_td_mst.ic_date), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						/* È¡²úÆ·±íµÄÀûÂÊ±àºÅ */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "ÀûÏ¢Ë°: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_td_mst.ic_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "ÀûÏ¢dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
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
								sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
								WRITEMSG
									return 1;
							}
						} else {
							daytermval = 0;

						}

						/* È¡»îÆÚÀûÂÊ */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "ÀûÏ¢: llbh=[%s],cur_no=[%s],date=[%ld]",
							llbh, g_td_parm.cur_no, g_td_mst.ic_date);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						g_pub_tx.tx_date, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "ÀûÏ¢dayterm=[%ld],rate_val=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
							daytermval, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
						WRITEMSG

							amtval += rate_val * daytermval
							* floor(g_pub_tx.tx_amt1 * L100) / L100
							* (1 + g_td_mst.flt_ratio) / L360 / L100;

						ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
						if ((ret != 0) && (ret == 1)) {
							sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d",
								ret);
							WRITEMSG
								strcpy(reply, "W100");
							return 1;
						} else if (ret < 0) {
							sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d ",
								ret);
							WRITEMSG
								strcpy(reply, "W101");
							return 1;
						}
						g_pub_intst.tax_intst = pub_base_PubDround_1(amtval);
						(*val) = amtval * sdsl;
						(*val) = pub_base_PubDround_1(*val);
				/*if(*val > 0)	*/
				fprintf(ifp, "-ÀûÏ¢Ë°|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(date99,
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
					sprintf(acErrMsg, "ÀûÏ¢ : dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
					*dealval, *factval, *val, *keepval);
					WRITEMSG

				}
				break;
			default:
				/* ´íÎó·µ»Ø */
				return 1;
			}
		}
	} else {		/* monterm >24 */

		sprintf(acErrMsg, "±£ÖµÀûÏ¢¼ÆËã");
		WRITEMSG

		/* ±£ÖµÀûÏ¢¼ÆËã */
			switch (draw_flag) {
		case 1:
			/* Î´µ½ÆÚÖ§È¡ */

			sprintf(acErrMsg, "Î´µ½ÆÚÖ§È¡ ");
			WRITEMSG

				if (g_td_mst.ic_date >= date93) {	/* date93Ö®ºó¿ª»§µÄ */

				sprintf(acErrMsg, "date93 Ö®ºó¿ª»§µÄ,°´»îÆÚÀûÂÊ¼ÆÏ¢ ");
				WRITEMSG

				/* °´µ½ÆÚ»îÆÚÀûÂÊ¼ÆËã */
					ret = pub_base_CalIntstDays(g_td_mst.ic_date,
				g_pub_tx.tx_date, intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
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
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
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
				fprintf(ifp, "+ÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date,
        g_pub_tx.tx_date,  dayterm, g_pub_tx.tx_amt1, rate_val, *dealval, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval));

				/* ¼ÆËãÀûÏ¢Ë° */
				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_pub_tx.tx_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					lx = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)	*/
				fprintf(ifp, "-ÀûÏ¢Ë°|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|%.2lf|\n", PubMax(g_td_mst.ic_date,
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
			 /* end of date93Ö®ºó¿ª»§µÄ */ 
			else {	/* date93 Ö®Ç°¿ª»§µÄ */

				sprintf(acErrMsg, "date93 Ö®Ç°¿ª»§µÄ,°´»îÆÚÀûÂÊ¼ÆÏ¢ ");
				WRITEMSG

				/*
				 * ´Óic_date,tx_date·Ö¶Î¼ÆÏ¢,Óöµ÷¼´µ÷,²»·Ö¸ßµÍ
				 *  
				 */

					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
							 g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type);
				if (ret != 0) {
					sprintf(acErrMsg, "½ç¶¨ÆÚÏŞ´íÎó");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "llbh=[%s],cur_no=[%s],ic_date=[%ld],tx_date=[%ld],\
        intst_term_type=[%d]", llbh, g_td_parm.cur_no, g_td_mst.ic_date,
					g_pub_tx.tx_date, intst_term_type);
				WRITEMSG

				/* rate_val = ÀûÂÊ * ÌìÊıÖ®ºÍ */
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËã·Ö¶Î»ıÊı´íÎó");
					WRITEMSG
						return 1;
				}
				sprintf(acErrMsg, "·Ö¶Î»ıÊı=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
					rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
				WRITEMSG

					lx = rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
					* (1 + g_td_mst.flt_ratio) / L360 / L100;
				(*dealval) = lx;

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
					/* ½ç¶¨ÆÚÏŞ */
					/* ´Ó¿ª»§ÈÕµ½PubMin(µ½ÆÚÈÕ,date99) */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

					ret = pub_base_intstJDQX(llbh, g_td_parm.cur_no,
								 g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99),
							   intst_term_type);
					if (ret != 0) {
						sprintf(acErrMsg, "½ç¶¨ÆÚÏŞ´íÎó");
						WRITEMSG
							return 1;
					}
					/* rate_val = ÀûÂÊ * ÌìÊıÖ®ºÍ */
					ret = pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.ic_date, g_pub_tx.tx_date, intst_term_type,
							      0, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËã·Ö¶Î»ıÊı´íÎó");
						WRITEMSG
							return 1;
					}
					sprintf(acErrMsg, "lx=[%lf],·Ö¶Î»ıÊı=[%lf],tx_amt=[%lf],flt_ratio=[%lf]",
						lx, rate_val, g_pub_tx.tx_amt1, g_td_mst.flt_ratio);
					WRITEMSG

						lx = lx - rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100
						* (1 + g_td_mst.flt_ratio) / L360 / L100;

					ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
					if (ret != 0 && ret == 1) {
						sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d",
							ret);
						WRITEMSG
							strcpy(reply, "W100");
						return 1;
					} else if (ret < 0) {
						sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d", ret);
						WRITEMSG
							strcpy(reply, "W101");
						return 1;
					}
					g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
					(*val) = pub_base_PubDround_1(lx * sdsl);
				/*if(*val > 0)*/
				fprintf(ifp, "-ÀûÏ¢Ë°|%.08ld-%.08ld|---|%.2lf|%.2lf|%.2lf|%.2lf|\n", g_td_mst.ic_date, PubMin(g_td_mst.mtr_date, date99) ,  lx, sdsl, *val, 
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
			/* µ½ÆÚÖ§È¡ */
			/*
			 * 1. 88/09/10
			 * ÒÔÇ°¿ª»§µÄ±£Öµ´¢Ğî,±£ÖµÆÚ´Ó88/09/10ÖÁµ½ÆÚÈÕ 2.
			 * 89/02/01
			 * ÒÔºóµ½ÆÚµÄ±£Öµ´¢Ğî,´Ó88/09/10ÆğÖ´ĞĞ89/02/01µ÷Õûºó
			 * µÄ´¢Ğî´æ¿îÀûÂÊ 3. 91/12/01--93/03/01
			 * Ö®¼ä´æÈëµÄ±£Öµ´¢Ğî,±£ÖµÆÚ´Ó93/07/11 ÖÁµ½ÆÚÈÕ 4.
			 * 93/03/01--93/07/11
			 * Ö®¼ä´æÈëµÄÒ»ÄêÆÚÒÔÉÏ´¢Ğî´æ¿î,´Ó¿ª»§ÈÕ µ½93/07/11
			 * °´93/07/11¹«²¼µÄ»îÆÚÀûÂÊ¼ÆÏ¢,93/07/11ÖÁµ½ÆÚÈÕ
			 * °´93/07/11¹«²¼µÄÍ¬µµ´Î¶¨ÆÚÀûÂÊ¼ÆÏ¢ 5.
			 * 93/07/11(º¬)Æğ´æÈëµÄÈıÎåÄêÆÚ°´´æµ¥ÉÏÔ­¶¨µÄÀûÂÊ¼ÆÏ¢ 
			 */
		case 3:
			/* ÓâÆÚÖ§È¡ */

			sprintf(acErrMsg, "µ½ÆÚÓâÆÚÀûÏ¢¼ÆËã");
			WRITEMSG

				if (g_td_mst.ic_date < date88) {

				sprintf(acErrMsg, "----------->ÆğÏ¢ÈÕÆÚĞ¡ÓÚ88.09.10");
				WRITEMSG

					if (g_td_mst.mtr_date >= date89) {
					/* ´Ó¿ª»§ÈÕµ½date89·Ö¶Î¼ÆÏ¢ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
								  g_td_mst.opn_date, date89, intst_term_type,
								  Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
					ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* date89 ÏàÓ¦´¢ÖÖµÄÀûÂÊ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = a1 + dayterm * rate_val;	
				if(dayterm*rate_val > 0)
				  fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", date88, g_td_mst.mtr_date,
    				dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
         						 * (1 + g_td_mst.flt_ratio) / L360 / L100);

				} else {
					/* ´Ó¿ª»§ÈÕµ½µ½ÆÚÈÕ·Ö¶Î¼ÆÏ¢ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
					g_td_mst.opn_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
						WRITEMSG
							strcpy(reply, "W111");
						return ret;
					}
				}

				/* ¼ÆËã±£ÖµÀûÏ¢ */
				/* È¡½»Ò×ÈÕµÄ±£ÖµÀûÂÊ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, BAOZHI);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
				if (ret != 0) {
					sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
					WRITEMSG
						strcpy(reply, "W110");
					return ret;
				}
				ret = pub_base_CalIntstDays(date88, g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
					WRITEMSG
						return 1;
				}
				(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 / L360 / L100;
				if(*keepval >0)	
				fprintf(ifp, "+±£ÖµÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date88, g_td_mst.mtr_date,
					 dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

			} else {/* 88.9.10ºó */

				sprintf(acErrMsg, "----------->ÆğÏ¢ÈÕÆÚ´óÓÚ88.9.10");
				WRITEMSG
					if (g_td_mst.ic_date < date91) {

					sprintf(acErrMsg, "µ½ÆÚÓâÆÚÀûÏ¢¼ÆËã,88.9.10ºó¿ª»§µÄ,date91Ç°¿ª»§");
					WRITEMSG

						ret = pub_base_CalIntstDays(g_td_mst.ic_date,
									    g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					/* È¡date89µ±ÈÕµÄÏàÓ¦µÄ´¢ÖÖµÄÀûÂÊ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, g_td_parm.rate_no);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
							 date89, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					a1 = rate_val * dayterm;
				if(a1>0)
				 fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-------  |\n", g_td_mst.ic_date, g_td_mst.mtr_date,
 dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


					ret = pub_base_CalIntstDays(date99, g_td_mst.mtr_date,
					      intst_term_type, &daytermval);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					s1 += rate_val * (double) daytermval;


					/* È¡µ½ÆÚÈÕµÄ±£ÖµÀûÂÊ */
					MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
					strcpy(llbh, BAOZHI);
					ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					      g_td_mst.mtr_date, &rate_val);
					if (ret != 0) {
						sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
						WRITEMSG
							strcpy(reply, "W110");
						return ret;
					}
					(*keepval) = dayterm * rate_val * floor(g_pub_tx.tx_amt1 * L100) / L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
				if(*keepval > 0)
				fprintf(ifp, "+±£ÖµÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|-----|\n", date99, g_td_mst.mtr_date, dayterm, g_pub_tx.tx_amt1, rate_val, *keepval);

					s2 += rate_val * daytermval;

				}
				 /* end of date91 Ç°¿ª»§ */ 
				else {	/* date91 ºó¿ª»§µÄ */
					if (g_td_mst.ic_date < date93) {

						sprintf(acErrMsg, "µ½ÆÚÓâÆÚÀûÏ¢¼ÆËã,date91ºó¿ª»§,date93 Ç°¿ª»§");
						WRITEMSG

						/*
						 * ´Ó¼ÆÏ¢ÈÕµ½µ½ÆÚÈÕ°´·Ö¶ÎÀûÂÊ¼
						 * ÆÏ¢ 
						 */
							MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, g_td_mst.mtr_date,
						intst_term_type, Lone, &a1, ifp);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						/*
						 * µ½ÆÚÈÕ
						 * PubMin(µ½ÆÚÈÕ£¬date99) 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);
						ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
									  g_td_mst.ic_date, PubMin(g_td_mst.mtr_date,
												   date99), intst_term_type, Lone, &s1_tmp, NULL);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
							WRITEMSG
								strcpy(reply, "W111");
							return ret;
						}
						s1 += a1 - s1_tmp;

					} else {	/* 1993/03/01Ö®ºó */

						sprintf(acErrMsg, "µ½ÆÚÓâÆÚÀûÏ¢¼ÆËã,date93 ºó¿ª»§");
						WRITEMSG

							ret = pub_base_CalIntstDays(g_td_mst.ic_date,
										    date9307, intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						/* È¡date9307»îÆÚÀûÂÊ */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, HQLLBH);

						sprintf(acErrMsg, "date93,date9307Õâ¶ÎÊ±¼ä°´»îÆÚ¼ÆÏ¢,llbh=[%s],cur_no=[%s],\
        date=9307", llbh, g_td_parm.cur_no);
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
						       date9307, &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307Õâ¶ÎÊ±¼ä°´»îÆÚ¼ÆÏ¢,date9307Ê±µÄrate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a1 = rate_val * dayterm;
						if (a1>0)
						 fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf| -------  |\n", g_td_mst.ic_date,
 date9307, dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


						sprintf(acErrMsg, "date93,date9307Õâ¶ÎÊ±¼ä°´»îÆÚ¼ÆÏ¢,»ıÊıÎª=[%lf]", a1);
						WRITEMSG

							ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						date9307), g_td_mst.mtr_date,
						 intst_term_type, &dayterm);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
							WRITEMSG
								return 1;
						}
						/*
						 * È¡PubMax(¿ª»§ÈÕÆÚ,date9307)
						 * ÈÕµÄÏàÓ¦µÄÀûÂÊ 
						 */
						MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
						strcpy(llbh, g_td_parm.rate_no);

						sprintf(acErrMsg, "date9307ºóµ½µ½ÆÚÈÕ°´date97Í¬µµ´ÎÀûÂÊ¼ÆÏ¢,llbh=[%s], \
		cur_no=[%s],date=[%ld]", llbh, g_td_parm.cur_no,
							PubMax(g_td_mst.ic_date, date9307));
						WRITEMSG

							ret = pub_base_getllz(llbh, g_td_parm.cur_no,
									      PubMax(g_td_mst.ic_date, date9307), &rate_val);
						if (ret != 0) {
							sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
							WRITEMSG
								strcpy(reply, "W110");
							return ret;
						}
						sprintf(acErrMsg, "date93,date9307Õâ¶ÎÊ±¼ä°´»îÆÚ¼ÆÏ¢,date9307Ê±µÄrate_val\
        =[%lf],dayterm=[%ld]", rate_val, dayterm);
						WRITEMSG

							a2 = rate_val * dayterm;
								if(a2>0)
								fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%d|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.ic_date,
date9307), g_td_mst.mtr_date,
								  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
								 * (1 + g_td_mst.flt_ratio) / L360 / L100);

						sprintf(acErrMsg, "date9307ºóµ½µ½ÆÚÈÕ,Õâ¶ÎÊ±¼ä»ıÊıÎª=[%lf]", a2);
						WRITEMSG

							a1 = a1 + a2;

						ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
						 date99), g_td_mst.mtr_date,
									    intst_term_type, &daytermval);
						if (ret != 0) {
							sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
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
                    		sprintf(acErrMsg,"¼ÆËãÌìÊı´íÎó!!");
                    		WRITEMSG
                    		return 1;
                		}
  
                        * È¡µ½ÆÚÈÕµÄ±£ÖµÀûÂÊ *
                        MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
                        strcpy(llbh,BAOZHI);
  
sprintf(acErrMsg,"µ½ÆÚÈÕµÄ±£ÖµÀûÂÊ llbh=[%s],cur_no=[%s],mtr_date=[%ld]",
       llbh,g_td_parm.cur_no,g_td_mst.mtr_date);
WRITEMSG 
  
                        ret = pub_base_getllz(llbh,g_td_parm.cur_no,
                              g_td_mst.mtr_date,&rate_val);
                        if (ret != 0)
                        {
                            sprintf(acErrMsg,"´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]",ret);
                            WRITEMSG
                            strcpy(reply,"W110");
                            return ret;
                        }
  
sprintf(acErrMsg,"±£ÖµÀûÂÊ rate_val=[%lf],dayterm=[%ld],tx_amt=[%lf],\
        flt_ratio=[%lf]",rate_val,dayterm,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG 
  
                        (*keepval) = rate_val * dayterm * floor(g_pub_tx.tx_amt1*L100)/L100 * (1 + g_td_mst.flt_ratio) / L360 / L100;
  
sprintf(acErrMsg, "±£ÖµÀûÏ¢: keepval=[%lf]", *keepval);
WRITEMSG
						******/

					ret = pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,
									   date99), g_td_mst.mtr_date, intst_term_type, &dayterm);
					if (ret != 0) {
						sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
						WRITEMSG
							return 1;
					}
					s2 = 0.00;
					s2 = rate_val * dayterm;

				}

			}

			/* ÓâÆÚÀûÏ¢ */
			/* µ½ÆÚÈÕµ½date93°´·Ö¶ÎÀûÂÊ¼ÆÏ¢ */

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.rate_no);

			sprintf(acErrMsg, "ÓâÆÚÀûÂÊ µ½ÆÚÈÕµ½date93,llbh=[%s],cur_no=[%s]",
				llbh, g_td_parm.cur_no);
			WRITEMSG
				if (g_td_mst.mtr_date < date93) {
				ret = f_pub_base_getsecrate(llbh, g_td_parm.cur_no,
							  g_td_mst.mtr_date, date93, intst_term_type, Lone, &a2, ifp);
				if (ret != 0) {
					sprintf(acErrMsg, "¼ÆËã·Ö¶ÎÀûÂÊ*ÌìÊı´íÎó [%d] ", ret);
					WRITEMSG
						strcpy(reply, "W111");
					return ret;
				}
			} else {
				a2 = 0.00;
			}

			sprintf(acErrMsg, "ÓâÆÚÀûÂÊ µ½ÆÚÈÕµ½date93,Õâ¶Î·Ö¶Î»ıÊıÎª: [%lf]", a2);
			WRITEMSG

			/* È¡½»Ò×µ±ÈÕµÄ»îÆÚÀûÂÊ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

			sprintf(acErrMsg, "ÓâÆÚÀûÂÊ date93ºó,llbh=[%s],cur_no=[%s],tx_date=[%ld]",
				llbh, g_td_parm.cur_no, g_pub_tx.tx_date);
			WRITEMSG

				ret = pub_base_getllz(llbh, g_td_parm.cur_no,
					       g_pub_tx.tx_date, &rate_val);
			if (ret != 0) {
				sprintf(acErrMsg, "´Ócom_rateÈ¡ÀûÂÊÖµ´íÎó [%d]", ret);
				WRITEMSG
					strcpy(reply, "W110");
				return ret;
			}
			ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date93),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
				WRITEMSG
					return 1;
			}
			sprintf(acErrMsg, "ÓâÆÚÀûÂÊ date93ºó  rate_val= [%lf],dayterm=[%ld]",
				rate_val, dayterm);
			WRITEMSG
				a3 = 0;
			a3 = rate_val * dayterm;
						if(rate_val > 0 && dayterm > 0)
						 fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", PubMax(g_td_mst.mtr_date, date93),
             g_pub_tx.tx_date,
  dayterm, g_pub_tx.tx_amt1, rate_val,  rate_val*dayterm* floor(g_pub_tx.tx_amt1 * L100) / L100
 * (1 + g_td_mst.flt_ratio) / L360 / L100);


			sprintf(acErrMsg, "ÓâÆÚÀûÂÊ date93ºóÀûÏ¢»ıÊıÎª: [%lf]", a3);
			WRITEMSG

				ret = pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date, date99),
			       g_pub_tx.tx_date, intst_term_type, &dayterm);
			if (ret != 0) {
				sprintf(acErrMsg, "¼ÆËãÌìÊı´íÎó!!");
				WRITEMSG
					return 1;
			}
			s1 += rate_val * dayterm;

			sprintf(acErrMsg, "µ½ÆÚÀûÏ¢»ıÊı=[%lf],ÓâÆÚ9307Ç°»ıÊı=[%lf],ÓâÆÚ9307ºó»ıÊı=\
        [%lf],tx_amt=[%lf],flt_ratio=[%lf]", a1, a2, a3, g_pub_tx.tx_amt1,
				g_td_mst.flt_ratio);
			WRITEMSG
				lx = (a1 + a2 + a3) * floor(g_pub_tx.tx_amt1 * L100) / L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			(*dealval) = lx;

			sprintf(acErrMsg, "--------->ÀûÏ¢ÖµÎª:[%lf]", *dealval);
			WRITEMSG

				if (strcmp(g_td_parm.intst_tax_no, SLBH) == 0) {
				ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
				if (ret != 0 && ret == 1) {
					sprintf(acErrMsg, "¶¨ÆÚ²úÆ·ÀûÏ¢Ë°Ë°ÂÊ²»´æÔÚ %d",
						ret);
					WRITEMSG
						strcpy(reply, "W100");
					return 1;
				} else if (ret < 0) {
					sprintf(acErrMsg, "È¡ÀûÏ¢Ë°Ë°ÂÊ´íÎó %d", ret);
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
				fprintf(ifp, "-ÀûÏ¢Ë°| ----  | -- |%.2lf|%.2lf|%.2lf|%.2lf|\n", 
					 		 lx, sdsl, *val, 
						pub_base_PubDround_1(g_pub_tx.tx_amt1+*dealval-*val));
			}
			(*dealval) = pub_base_PubDround_1(*dealval);
			(*val) = pub_base_PubDround_1(*val);
			(*keepval) = pub_base_PubDround_1(*keepval);

			(*factval) = (*dealval) - (*val);
			(*keepval) = (*keepval);	/* Ë°ºó±£ÖµÀûÏ¢ */

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
* º¯ÊıÃû£º    f_pub_base_getsecrate()
* º¯Êı¹¦ÄÜ£º  ¸ù¾İÀûÂÊ±àºÅ ±ÒÖÖ ÆğÏ¢ÈÕ ½»Ò×ÈÕ Ä£Ê½,¼ÆËã·Ö¶ÎÀûÂÊ*´æÆÚÖ®ºÍ
* ×÷Õß/Ê±¼ä£º 2003Äê01ÔÂ05ÈÕ
*
* ²ÎÊı£º
*     ÊäÈë£º ÀûÂÊ±àºÅ rate_no    char *
*            ±ÒÖÖ     cur_no     char *
*            ÆğÏ¢ÈÕÆÚ ic_date    long
*            ½»Ò×ÈÕÆÚ tx_date    long
*            ¼ÆËãÌìÊıÀàĞÍ intst_term_type char 
*                   0--°´Êµ¼ÊÌìÊı¼ÆËã    1--°´Ã¿ÔÂ30Ìì¼ÆËã
*            Ä£Ê½     mode       int  
*                   0--Óöµ÷¼´µ÷,²»·Ö¸ßµÍ 1--Óö¸ß¾Í¸ß,ÓöµÍ²»µÍ
*     Êä³ö:
*            ÀûÂÊ*´æÆÚ rate_val  double *
* ·µ »Ø Öµ: 0 ³É¹¦
*
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/

int f_pub_base_getsecrate(char *rate_no, char *cur_no, long ic_date,
        long tx_date, char intst_term_type[2], int mode, double *rate_val, FILE *ifp)
{
    struct com_rate_c com_rate_c;
    char rateno[4];
    int ret;
    double day_val; /* ÀûÂÊ*ÌìÊıÖ®ºÍ */
    double rate_val_last, llz; /* µ÷Ï¢ºóÇ°Ò»µµÀûÂÊÖµ */
    long   beg_date;      /* Ç°Ò»µµÀûÂÊ¼ÆËãµÄÆğÊ¼ÈÕÆÚ */
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
            sprintf(acErrMsg,"¼ÆËãÌìÊı´íÎó!!");
            WRITEMSG
            return 1;
        }
        
        sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
        WRITEMSG
      
        day_val += dayterm * rate_val_last;

	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
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
        sprintf(acErrMsg,"¼ÆËãÌìÊı´íÎó!!");
        WRITEMSG
        return 1;
    }
    
    sprintf(acErrMsg," dayterm=[%ld],rate_val=[%f]",dayterm,rate_val_last);
    WRITEMSG

    day_val += rate_val_last * dayterm ;
    (*rate_val) = day_val ;
	if(rate_val_last > 0.001 && dayterm > 0 && NULL != ifp)
	fprintf(ifp, "+·Ö¶ÎÀûÏ¢|%.08ld-%.08ld|%ld|%.2lf|%.2lf|%.2lf|  -------  |\n", beg_date,
		tx_date, dayterm, g_pub_tx.tx_amt1, rate_val_last,  day_val* floor(g_pub_tx.tx_amt1 * L100) / L100
          * (1 + g_td_mst.flt_ratio) / L360 / L100);

    Com_rate_Clo_Sel();

    return 0;
}

