/* *****************************************************************************/
/*  Function Name: iGetAreaTax()                                               */
/*  Description  : ����Ϣ˰����������Ϣ˰(�ӽ����յ��㵽99��)                  */
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
        long daytermval=0/* ��Ϣ������ */,tot_cnt=0/*����ϸ����*/;
	long hq_dayterm=0,dq_dayterm=0;	/* �ֶεĻ��ںͶ��ڵ����� */
	long date99=19991101,hst_tx_date=0;/* ��ϸ��¼�Ľ������� */
	char   llbh[3]; /* ���ʱ�� */ 	char jsts[10];/* ���������ķ�ʽ,0ʵ��  1 30�� */
	double rate_val=0,hq_amtval=0,dq_amtval=0,lxs=0,dq_lx=0,hq_lx=0,dq_acm=0,hq_acm=0; 
	struct com_sys_parm_c tmp_com_sys_parm;

	memset(jsts,'\0',sizeof(jsts)); strcpy(jsts,intst_term_type);
	memset(&g_mo_int_tax_hst,'\0',sizeof(g_mo_int_tax_hst));

	vtcp_log("ע��: �������¼�����Ϣ˰����[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("ע��: ��������Ӧ����Ϣ[%.2f],ʵ����Ϣ[%.2f],��Ϣ˰[%.2f],���������ķ�ʽ[%s],��������[%.5f],��������[%.5f],����֤��[%s]\n",*dealval,*factval,*val,jsts,ll_ddval,ll_hhval,edu_ind);
	/* �����Զ�ת�滧 pub_tx.tx_date ���,���Խ�������ֻ�ܴ����ݿ���ȡ */
	memset(&tmp_com_sys_parm,'\0',sizeof(tmp_com_sys_parm));
	if( pub_base_GetSysparm( &tmp_com_sys_parm)){
		vtcp_log("[%s][%d]ȡ������������ʱ�����\n",__FILE__,__LINE__);
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
		case 0:		/* ���� */
	/* δ������ô��������Ӧ��˰����Ϣ */
	if(g_td_mst.mtr_date>g_pub_tx.tx_date){
		ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"������������!!");
			WRITEMSG
			return 1;
		}

		/* ȡ�������� */
		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,HQLLBH);
sprintf(acErrMsg,"���� : ���ʱ��=[%s],����=[%s],��������=[%ld]",
        llbh,g_pub_tx.cur_no,g_pub_tx.tx_date);
WRITEMSG

		ret = pub_base_getllz(llbh,g_pub_tx.cur_no,
		g_pub_tx.tx_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
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
sprintf(acErrMsg,"��˰����=[%d],��Ϣ����=[%lf],����=[%lf],������=[%lf]",hq_dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
WRITEMSG

		/* �����������˰ */
		if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
			vtcp_log("[%s][%d]ȡ�ֶ�˰�ʱ�˰��",__FILE__,__LINE__);
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

				ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); /* ���� */
			   if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				vtcp_log("[%s][%d]�ֶ�����[%d]\n",__FILE__,__LINE__,daytermval);
				if(hq_dayterm>0){	
					vtcp_log("��ĸ����Ϊ0 [%d]\n",hq_dayterm);
					hq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
				}

				/* ׼����ϸ */
				g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
				g_mo_int_tax_hst.rate=rate_val * (1 + g_td_mst.flt_ratio) ;/* ���� */
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;/* ��Ϣ */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;	/* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
			Com_intst_tax_rate_Clo_Sel();

				lxs= hq_lx * hq_amtval;
			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		}
	}
	else{	/* ���ڻ����� */
		ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_td_mst.mtr_date,intst_term_type,&dq_dayterm);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���㶨����������!!");
			WRITEMSG
			return 1;
		}

		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,g_td_parm.rate_no);

		ret = pub_base_getllz(llbh,g_td_parm.cur_no,g_td_mst.ic_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W110");
			return ret;
		}
		double tmp_rate_val; tmp_rate_val=rate_val;
		dq_lx = dq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 
			* (1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"Ӧ��˰�Ķ�����Ϣ[%.2f] : ���ʱ��=[%s],����=[%s],��Ϣ����=[%ld],����[%.5f]",
        dq_lx,llbh,g_pub_tx.cur_no,dq_dayterm,rate_val);
WRITEMSG
		
		/* Ӧ��˰������Ϣ���� */
		hq_dayterm=0;
		if (g_td_parm.ovrd_intst_ind[0] == 'Y'){
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.mtr_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"���������������!!");
				WRITEMSG
				return 1;
			}
		}
		MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
		strcpy(llbh,HQLLBH);

		ret = pub_base_getllz(llbh,g_pub_tx.cur_no,g_pub_tx.tx_date,&rate_val);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W110");
			return ret;
		}

		hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;

sprintf(acErrMsg,"Ӧ��˰�Ļ�����Ϣ[%.2f] : ����[%.2f],��Ϣ����=[%ld],����[%.5f]",
        hq_lx,g_pub_tx.tx_amt1,hq_dayterm,rate_val);
WRITEMSG

		/* �����������˰ */
		if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
			vtcp_log("ȡ�ֶ�˰�ʱ�˰��");
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

					vtcp_log("[%s][%d]���ڷֶ�����[%d],˰��[%.5f],������ռ�İٷֱ�[%.5f]\n",__FILE__,__LINE__,daytermval,g_com_intst_tax_rate.tax_rate_val,dq_amtval);

					/* ׼����ϸ */
					g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
					g_mo_int_tax_hst.end_date=PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date);
					g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
					g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
					g_mo_int_tax_hst.rate=tmp_rate_val * (1 + g_td_mst.flt_ratio) ;/*���� */
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
				if (g_td_parm.ovrd_intst_ind[0] == 'Y'){	/* ���ڲ��ּ�����Ϣ */
					if(g_td_mst.mtr_date < g_com_intst_tax_rate.end_date){
						ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
						if(hq_dayterm>0){	/* ��ĸ����Ϊ0 */
							vtcp_log("��ĸ����Ϊ0[%d]\n",hq_dayterm);
							hq_amtval +=g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
						}

						vtcp_log("[%s][%d]���ڷֶ�����[%d]\n",__FILE__,__LINE__,daytermval);

						/* ׼����ϸ */
						g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date);
						g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
						g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
						g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
						g_mo_int_tax_hst.rate=rate_val*(1+g_td_mst.flt_ratio);/*���� */
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
						if(ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
				if (ret != 0){
					sprintf(acErrMsg,"����ֶ���������!!");
					WRITEMSG
					return 1;
				}
			}
			Com_intst_tax_rate_Clo_Sel();

				lxs= dq_lx * dq_amtval + hq_lx * hq_amtval;
vtcp_log("[%s][%d]������Ϣ[%.2f],�ٷֱ�[%.5f],������Ϣ[%.2f],�ٷֱ�[%.5f]\n",__FILE__,__LINE__,dq_lx,dq_amtval,hq_lx,hq_amtval);
			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		}
	}
	vtcp_log("[%s][%d]�ֶ����Ӧ��˰��Ϣ[%.2f],˰��[%.2f]\n",__FILE__,__LINE__,dq_lx+hq_lx,*val);
	return(0);

	case 1:		/* ���� */
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
			vtcp_log("[%s][%d]�÷ֶ���[%ld],ֹ[%ld]��������ϸ\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
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
						vtcp_log("[%s][%d]�����[%ld]û����ϸ!!!û���ֶ���˰,�����溯����İ�\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if(hst_cnt==1){
					vtcp_log("ÿ�ηֶεĵ�һ�ʵĻ������ϴε����*����");
					if(g_td_mst.ic_date==g_td_mst_hst.tx_date){
						vtcp_log("�˻��ĵ�һ����ϸ[%ld],��if��ķ�����\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
					vtcp_log("[%s][%d]�ٽ�����[%ld],����[%d]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0){
						vtcp_log("[%d]�����Ǳʼ�¼[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else 
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]�ֶ���[%ld],����[%d],����[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++; tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if(g_pub_tx.tx_date>=g_td_mst.mtr_date){
				vtcp_log("��ʼ���㶨�ڲ�����Ϣ˰[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"Ӧ��˰�Ķ���˰[%.2f] : ����=[%s],����=[%.2f],����[%.5f]",
			lxs,g_pub_tx.cur_no,dq_acm,ll_ddval);
WRITEMSG
				/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=dq_acm;
                g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
                g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
                g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
                if(ret=insert_hst(&g_mo_int_tax_hst))
                    return(ret);

				if(g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("���һ�ʽ��������������Ϣ˰�ĵ���,�ʻ��ӵ����������ռ����Ϣ˰\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
				lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("���������Ϣ˰[%.2f],����[%d]\n",lxs,daytermval);
					/* ׼����ϸ */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else{
				vtcp_log("[%s][%d]��ǰ֧ȡ��������Ϣ\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if(hst_cnt==1 && tot_cnt>0){
					vtcp_log("[%s][%d]��ǰ֧ȡ,��ʷ��ϸ��û�����˰������[%ld][%ld]�ļ�¼,�������",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("����[%d],����[%.2f],����[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"��ǰ֧��������Ϣ˰[%.2f] : ��ǰ֧ȡ���ڻ���[%.2f],����=[%ld],����[%.5f]",
			lxs,dq_acm,daytermval,ll_hhval);
WRITEMSG
					/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
				/* ���ں���ڲ����ֶַ� */
				/* ��Ϊ���������������ж���ֹ�����Բ����ظ��������ڲ��ֵĻ�����Ϣ */
	  		if(g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]���ں�Ļ��ڲ��ֻ����Էֶ�\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]�����գ��ֶ�ֹ��[%ld]������[%d],���ڻ���[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

				lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"������Ϣ˰[%.2f] : ���ڲ��ֻ���[%.2f],����=[%ld],����[%.5f]",
			lxs,hq_acm,daytermval,ll_hhval);
WRITEMSG
					/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
		}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
vtcp_log("[%s][%d]�ֶ����˰��[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	case 6:		/* �������� */
		vtcp_log("ȡ�ֶ�˰�ʱ�˰��");
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

				vtcp_log("[%s][%d]�ֶ�����[%d]\n",__FILE__,__LINE__,daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"����ֶ���������!!");
					WRITEMSG
					return 1;
				}
				/* ׼����ϸ */
				g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
				g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);/*���� */
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
			Com_intst_tax_rate_Clo_Sel();

			(*val) = pub_base_PubDround_1(lxs);
			(*factval) = (*dealval) -(*val);
		return(0);
	case 7:		/* �������� */
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
				vtcp_log("[%s][%d]�������н���֤����ôֻ�����ڲ�����Ϣ˰\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				lxs +=g_td_mst.bal * daytermval * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"Ӧ�ɵ�˰[%.2f] :����=[%.2f],����[%.5f],����[%d]",
			lxs,g_td_mst.bal,ll_hhval,daytermval);
WRITEMSG
				/* ׼����ϸ */
				g_mo_int_tax_hst.beg_date=PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /*��Ϣ*/
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
		}
		else
		{
			vtcp_log("û���ڻ���û����֤����ô�㷨ͬ����,����ϸ�ֶ���\n");
			vtcp_log("[%s][%d]�÷ֶ���[%ld],ֹ[%ld]�������ϸ\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
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
						vtcp_log("[%s][%d]�����[%ld]û����ϸ!!!û���ֶ���˰,�����溯����İ�\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if(hst_cnt==1){
					vtcp_log("ÿ�ηֶεĵ�һ�ʵĻ������ϴε����*����");
					if(g_td_mst.ic_date==g_td_mst_hst.tx_date){
						vtcp_log("�˻��ĵ�һ����ϸ[%ld],��if��ķ�����\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
					vtcp_log("[%s][%d]�ٽ�����[%ld],����[%d]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0){
						vtcp_log("[%d]�����Ǳʼ�¼[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]�ֶ���[%ld],����[%d],����[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++;tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if(g_pub_tx.tx_date>=g_td_mst.mtr_date){
				vtcp_log("��ʼ���㶨�ڲ�����Ϣ˰[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"Ӧ�ɵĶ���˰[%.2f] : ����=[%.2f],����[%.5f]",
			lxs,dq_acm,ll_ddval);
WRITEMSG
				/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=dq_acm;
                g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
                g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
                g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
                if(ret=insert_hst(&g_mo_int_tax_hst))
                    return(ret);

				if(g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("���һ�ʽ��������������Ϣ˰�ĵ���,�ʻ��ӵ����������ռ����Ϣ˰\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
				lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						   * g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("���������Ϣ˰[%.2f],����[%d]\n",lxs,daytermval);
					/* ׼����ϸ */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else{
				vtcp_log("[%s][%d]��ǰ֧ȡ��������Ϣ\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if(hst_cnt==1 && tot_cnt>0){
					vtcp_log("[%s][%d]��ǰ֧ȡ,��ʷ��ϸ��û�����˰������[%ld][%ld]�ļ�¼,�������",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0){
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
						return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("����[%d],����[%.2f],����[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"��ǰ֧��������Ϣ˰[%.2f] : ��ǰ֧ȡ���ڻ���[%.2f],����=[%ld],����[%.5f]",
			lxs,dq_acm,daytermval,ll_hhval);
WRITEMSG
				/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
				/* ���ں���ڲ����ֶַ� */
				/* ��Ϊ���������������ж���ֹ�����Բ����ظ��������ڲ��ֵĻ�����Ϣ */
	  		if(g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]���ں�Ļ��ڲ��ֻ����Էֶ�\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0){
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
					return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]�ֶ��𣭷ֶ�ֹ��[%ld]������[%d],���ڻ���[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

					lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
sprintf(acErrMsg,"������Ϣ˰[%.2f] : ���ڲ��ֻ���[%.2f],����=[%ld],����[%.5f]",
			lxs,hq_acm,daytermval,ll_hhval);
WRITEMSG
				/* ׼����ϸ */
				g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ret=insert_hst(&g_mo_int_tax_hst))
					return(ret);
			}
		}
	}
		Com_intst_tax_rate_Clo_Sel();

		(*val) = pub_base_PubDround_1(lxs);
		(*factval) = (*dealval) -(*val);
vtcp_log("[%s][%d]�ֶ����˰��[%.2f]\n",__FILE__,__LINE__,*val);
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
		vtcp_log("����ֶ���Ϣ˰��ʷ���[%d]\n",ret);
		Mo_int_tax_hst_Debug(g_mo_int_tax_hst);
		return(ret);
	}
	return(0);
}

int iGetAreaTax_ZCZQ_TK(long real_tx_date,double *val,double *factval,double *dealval,char *intst_term_type,int acttype,double ll_ddval,double ll_hhval,char *edu_ind,int ss_flag)
{
	vtcp_log("ע��: �������¼�����Ϣ˰����[%s][%d]intst_term_type===[%s]\n",__FILE__,__LINE__,intst_term_type);

	struct com_intst_tax_rate_c g_com_intst_tax_rate;
	struct mo_int_tax_hst_c     g_mo_int_tax_hst;
	struct td_mst_hst_c         g_td_mst_hst;
	int i=0,ret=0,hst_cnt=0;
	long daytermval=0/* ��Ϣ������ */,tot_cnt=0/*����ϸ����*/;
	long hq_dayterm=0,dq_dayterm=0;	/* �ֶεĻ��ںͶ��ڵ����� */
	long date99=19991101,hst_tx_date=0;/* ��ϸ��¼�Ľ������� */
	char   llbh[3]; /* ���ʱ�� */ 	char jsts[10];/* ���������ķ�ʽ,0ʵ��  1 30�� */
	double rate_val=0,hq_amtval=0,dq_amtval=0,lxs=0,dq_lx=0,hq_lx=0,dq_acm=0,hq_acm=0; 
	struct com_sys_parm_c tmp_com_sys_parm;
 /* intst_term_type[0]='0';*/
	memset(&g_com_intst_tax_rate,'\0',sizeof(struct com_intst_tax_rate_c));
	memset(jsts,'\0',sizeof(jsts)); strcpy(jsts,intst_term_type);
	memset(&g_mo_int_tax_hst,'\0',sizeof(struct mo_int_tax_hst_c));
	memset(&g_td_mst_hst,'\0',sizeof(struct td_mst_hst_c));

	vtcp_log("ע��: �������¼�����Ϣ˰����[%s][%d]aaaaaa%s\n",__FILE__,__LINE__,intst_term_type);
	vtcp_log("ע��: ��������ʵ����Ϣ[%.2f]\n",*factval);
	vtcp_log("ע��: �������Ľ���֤��[%s]\n",edu_ind);
	vtcp_log("ע��: ����������Ϣ˰[%.2f]\n",*val);
	vtcp_log("ע��: �������¼�����Ϣ˰����[%s][%d]aaaaaa%s\n",__FILE__,__LINE__,intst_term_type);
	vtcp_log("ע��: �������ļ��������ķ�ʽ[%s]\n",jsts);
	vtcp_log("ע��: �������Ķ�������[%.5f]\n",ll_ddval);
	vtcp_log("ע��: �������Ļ�������[%.5f]\n",ll_hhval);
	vtcp_log("ע��: ��������Ӧ����Ϣ[%.2f]\n",*dealval);
	/* �����Զ�ת�滧 pub_tx.tx_date ���,���Խ�������ֻ�ܴ����ݿ���ȡ */
	memset(&tmp_com_sys_parm,'\0',sizeof(tmp_com_sys_parm));
	if ( pub_base_GetSysparm( &tmp_com_sys_parm))
	{
		vtcp_log("[%s][%d]ȡ������������ʱ�����\n",__FILE__,__LINE__);
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
					vtcp_log("[%s][%d]ȡ�ֶ�˰�ʱ�˰��[%ld]",__FILE__,__LINE__,g_td_mst.ic_date);

	switch(acttype)
	{
	case 0:		/* ���� */
		/* δ������ô��������Ӧ��˰����Ϣ */
		if (g_td_mst.mtr_date>g_pub_tx.tx_date)
		{
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"������������!!");
				WRITEMSG
					return 1;
			}
			/* ȡ�������� */
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,HQLLBH);
			sprintf(acErrMsg,"���� : ���ʱ��=[%s],����=[%s],��������=[%ld]",
				llbh,g_pub_tx.cur_no,g_pub_tx.tx_date);
			WRITEMSG

				ret = pub_base_getllz(llbh,g_pub_tx.cur_no,
				real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
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
			sprintf(acErrMsg,"��˰����=[%ld],��Ϣ����=[%lf],����=[%lf],������=[%lf]",hq_dayterm,rate_val,g_pub_tx.tx_amt1,g_td_mst.flt_ratio);
			WRITEMSG

			/* �����������˰ */
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
			{
				vtcp_log("[%s][%d]ȡ�ֶ�˰�ʱ�˰��",__FILE__,__LINE__);
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
					ret=pub_base_CalIntstDays(PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); /* ���� */
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					vtcp_log("[%s][%d]�ֶ�����[%ld]\n",__FILE__,__LINE__,daytermval);
					if (hq_dayterm>0)
					{	
						vtcp_log("��ĸ����Ϊ0 [%ld]\n",hq_dayterm);
						hq_amtval += g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
					}
					/* ׼����ϸ */
					g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
					g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
					g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
					g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
					g_mo_int_tax_hst.rate=rate_val * (1 + g_td_mst.flt_ratio) ;/* ���� */
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;/* ��Ϣ */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;	/* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
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
		{	/* ���ڻ����� */
			ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.ic_date),g_td_mst.mtr_date,intst_term_type,&dq_dayterm);
			if (ret != 0)
			{
				sprintf(acErrMsg,"���㶨����������!!");
				WRITEMSG
					return 1;
			}
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,g_td_parm.rate_no);

			ret = pub_base_getllz(llbh,g_td_parm.cur_no,real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
				WRITEMSG
					strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			double tmp_rate_val; tmp_rate_val=rate_val;
			dq_lx = dq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100 
				* (1 + g_td_mst.flt_ratio) / L360 / L100;
			sprintf(acErrMsg,"Ӧ��˰�Ķ�����Ϣ[%.2f] : ���ʱ��=[%s],����=[%s],��Ϣ����=[%ld],����[%.5f]",
				dq_lx,llbh,g_pub_tx.cur_no,dq_dayterm,rate_val);
			WRITEMSG

			/* Ӧ��˰������Ϣ���� */
				hq_dayterm=0;
			if (g_td_parm.ovrd_intst_ind[0] == 'Y')
			{
				ret=pub_base_CalIntstDays(PubMax(date99,g_td_mst.mtr_date),g_pub_tx.tx_date,intst_term_type,&hq_dayterm);
				if (ret != 0)
				{
					sprintf(acErrMsg,"���������������!!");
					WRITEMSG
						return 1;
				}
			}
			MEMSET_DEBUG(llbh,0x0,sizeof(llbh));
			strcpy(llbh,HQLLBH);

			ret = pub_base_getllz(llbh,g_pub_tx.cur_no,real_tx_date,&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
				WRITEMSG
					strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			hq_lx = hq_dayterm * rate_val * floor(g_pub_tx.tx_amt1*L100)/L100
				* (1 + g_td_mst.flt_ratio) / L360 / L100;

			sprintf(acErrMsg,"Ӧ��˰�Ļ�����Ϣ[%.2f] : ����[%.2f],��Ϣ����=[%ld],����[%.5f]",
				hq_lx,g_pub_tx.tx_amt1,hq_dayterm,rate_val);
			WRITEMSG

			/* �����������˰ */
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
			{
				vtcp_log("ȡ�ֶ�˰�ʱ�˰��");
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

						vtcp_log("[%s][%d]���ڷֶ�����[%ld],˰��[%.5f],������ռ�İٷֱ�[%.5f]\n",__FILE__,__LINE__,daytermval,g_com_intst_tax_rate.tax_rate_val,dq_amtval);

						/* ׼����ϸ */
						g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
						g_mo_int_tax_hst.end_date=PubMin(g_td_mst.mtr_date,g_com_intst_tax_rate.end_date);
						g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
						g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
						g_mo_int_tax_hst.rate=tmp_rate_val * (1 + g_td_mst.flt_ratio) ;/*���� */
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
						if(ss_flag != 1)
						{
							if (ret=insert_hst(&g_mo_int_tax_hst))
								return(ret);
						}
					}
					if (g_td_parm.ovrd_intst_ind[0] == 'Y')
					{	/* ���ڲ��ּ�����Ϣ */
						if (g_td_mst.mtr_date < g_com_intst_tax_rate.end_date)
						{
							ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval); 
							if (hq_dayterm>0)
							{	/* ��ĸ����Ϊ0 */
								vtcp_log("��ĸ����Ϊ0[%ld]\n",hq_dayterm);
								hq_amtval +=g_com_intst_tax_rate.tax_rate_val * daytermval / hq_dayterm;
							}
							vtcp_log("[%s][%d]���ڷֶ�����[%ld]\n",__FILE__,__LINE__,daytermval);

							/* ׼����ϸ */
							g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date);
							g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
							g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
							g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
							g_mo_int_tax_hst.rate=rate_val*(1+g_td_mst.flt_ratio);/*���� */
							g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
							g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
							g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
							if(ss_flag != 1)
							{
								if (ret=insert_hst(&g_mo_int_tax_hst))
									return(ret);
							}
						}
					}
					if (ret != 0)
					{
						sprintf(acErrMsg,"����ֶ���������!!");
						WRITEMSG
							return 1;
					}
				}
				Com_intst_tax_rate_Clo_Sel();

				lxs= dq_lx * dq_amtval + hq_lx * hq_amtval;
				vtcp_log("[%s][%d]������Ϣ[%.2f],�ٷֱ�[%.5f],������Ϣ[%.2f],�ٷֱ�[%.5f]\n",__FILE__,__LINE__,dq_lx,dq_amtval,hq_lx,hq_amtval);
				(*val) = pub_base_PubDround_1(lxs);
				(*factval) = (*dealval) -(*val);
			}
		}
		vtcp_log("[%s][%d]�ֶ����Ӧ��˰��Ϣ[%.2f],˰��[%.2f]\n",__FILE__,__LINE__,dq_lx+hq_lx,*val);
		return(0);

	case 1:		/* ���� */
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
			vtcp_log("[%s][%d]�÷ֶ���[%ld],ֹ[%ld]��������ϸ\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
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
						vtcp_log("[%s][%d]�����[%ld]û����ϸ!!!û���ֶ���˰,�����溯����İ�\n",__FILE__,__LINE__,g_td_mst.ac_id);
						return(0);
					}
					break;
				}
				hst_tx_date=g_td_mst_hst.tx_date;
				if (hst_cnt==1)
				{
					vtcp_log("ÿ�ηֶεĵ�һ�ʵĻ������ϴε����*����");
					if (g_td_mst.ic_date==g_td_mst_hst.tx_date)
					{
						vtcp_log("�˻��ĵ�һ����ϸ[%ld],��if��ķ�����\n",g_td_mst_hst.tx_date);
						daytermval=0;
					}
					else
					{
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					}
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					vtcp_log("[%s][%d]�ٽ�����[%ld],����[%ld]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
					if ( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
						pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0)
					{
						vtcp_log("[%d]�����Ǳʼ�¼[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
						dq_acm += g_td_mst_hst.tx_amt * daytermval;
					}
					else 
						dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
				}
				ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
						return 1;
				}
				dq_acm += g_td_mst_hst.tx_amt * daytermval;
				vtcp_log("[%s][%d]�ֶ���[%ld],����[%ld],����[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
				hst_cnt++; tot_cnt++;
			}
			Td_mst_hst_Clo_Sel();

			if (g_pub_tx.tx_date>=g_td_mst.mtr_date)
			{
				vtcp_log("��ʼ���㶨�ڲ�����Ϣ˰[%s][%d]\n",__FILE__,__LINE__);

				lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"Ӧ��˰�Ķ���˰[%.2f] : ����=[%s],����=[%.2f],����[%.5f]",
					lxs,g_pub_tx.cur_no,dq_acm,ll_ddval);
				WRITEMSG
				/* ׼����ϸ */
					g_mo_int_tax_hst.intst_acm=dq_acm;
				g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}

				if (g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
				{
					vtcp_log("���һ�ʽ��������������Ϣ˰�ĵ���,�ʻ��ӵ����������ռ����Ϣ˰\n");

					ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					vtcp_log("���������Ϣ˰[%.2f],����[%ld]\n",lxs,daytermval);
					/* ׼����ϸ */
					g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
			}
			else
			{
				vtcp_log("[%s][%d]��ǰ֧ȡ��������Ϣ\n",__FILE__,__LINE__);
				long tmp_days=0; double tmp_acm=0;
				if (hst_cnt==1 && tot_cnt>0)
				{
					vtcp_log("[%s][%d]��ǰ֧ȡ,��ʷ��ϸ��û�����˰������[%ld][%ld]�ļ�¼,�������",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
					ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					tmp_acm=g_td_mst.bal * tmp_days;
					vtcp_log("����[%d],����[%.2f],����[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
				}
				lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"��ǰ֧��������Ϣ˰[%.2f] : ��ǰ֧ȡ���ڻ���[%.2f],����=[%ld],����[%.5f]",
					lxs,dq_acm,daytermval,ll_hhval);
				WRITEMSG
				/* ׼����ϸ */
					g_mo_int_tax_hst.intst_acm=hq_acm;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			/* ���ں���ڲ����ֶַ� */
			/* ��Ϊ���������������ж���ֹ�����Բ����ظ��������ڲ��ֵĻ�����Ϣ */
			if (g_pub_tx.tx_date>=g_td_mst.mtr_date )
			{
				vtcp_log("[%s][%d]���ں�Ļ��ڲ��ֻ����Էֶ�\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
						return 1;
				}
				hq_acm += g_td_mst.bal * daytermval;
				vtcp_log("[%s][%d]�����գ��ֶ�ֹ��[%ld]������[%ld],���ڻ���[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

				lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"������Ϣ˰[%.2f] : ���ڲ��ֻ���[%.2f],����=[%ld],����[%.5f]",
					lxs,hq_acm,daytermval,ll_hhval);
				WRITEMSG
				/* ׼����ϸ */
					g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
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
		vtcp_log("[%s][%d]�ֶ����˰��[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	case 6:		/* �������� */
		vtcp_log("ȡ�ֶ�˰�ʱ�˰��");
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

			vtcp_log("[%s][%d]�ֶ�����[%ld]\n",__FILE__,__LINE__,daytermval);
			if (ret != 0)
			{
				sprintf(acErrMsg,"����ֶ���������!!");
				WRITEMSG
					return 1;
			}
			/* ׼����ϸ */
			g_mo_int_tax_hst.beg_date=PubMax(g_td_mst.ic_date,g_com_intst_tax_rate.beg_date);
			g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
			g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
			g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
			g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);/*���� */
			g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
			g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
			g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
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
	case 7:		/* �������� */
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
				vtcp_log("[%s][%d]�������н���֤����ôֻ�����ڲ�����Ϣ˰\n",__FILE__,__LINE__);
				ret=pub_base_CalIntstDays(PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
				if (ret != 0)
				{
					sprintf(acErrMsg,"������������!!");
					WRITEMSG
						return 1;
				}
				lxs +=g_td_mst.bal * daytermval * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
					* g_com_intst_tax_rate.tax_rate_val;
				sprintf(acErrMsg,"Ӧ�ɵ�˰[%.2f] :����=[%.2f],����[%.5f],����[%d]",
					lxs,g_td_mst.bal,ll_hhval,daytermval);
				WRITEMSG
				/* ׼����ϸ */
					g_mo_int_tax_hst.beg_date=PubMax(g_com_intst_tax_rate.beg_date,g_td_mst.mtr_date);
				g_mo_int_tax_hst.end_date=PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date);
				g_mo_int_tax_hst.tx_amt=g_pub_tx.tx_amt1;
				g_mo_int_tax_hst.intst_acm=g_pub_tx.tx_amt1 * daytermval;/* ���� */
				g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
				g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /*��Ϣ*/
				g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
				g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
				if(ss_flag != 1)
				{
					if (ret=insert_hst(&g_mo_int_tax_hst))
						return(ret);
				}
			}
			else
			{
				vtcp_log("û���ڻ���û����֤����ô�㷨ͬ����,����ϸ�ֶ���\n");
				vtcp_log("[%s][%d]�÷ֶ���[%ld],ֹ[%ld]�������ϸ\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
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
							vtcp_log("[%s][%d]�����[%ld]û����ϸ!!!û���ֶ���˰,�����溯����İ�\n",__FILE__,__LINE__,g_td_mst.ac_id);
							return(0);
						}
						break;
					}
					hst_tx_date=g_td_mst_hst.tx_date;
					if (hst_cnt==1)
					{
						vtcp_log("ÿ�ηֶεĵ�һ�ʵĻ������ϴε����*����");
						if (g_td_mst.ic_date==g_td_mst_hst.tx_date)
						{
							vtcp_log("�˻��ĵ�һ����ϸ[%ld],��if��ķ�����\n",g_td_mst_hst.tx_date);
							daytermval=0;
						}
						else
						{
							ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
						}
						if (ret != 0)
						{
							sprintf(acErrMsg,"������������!!");
							WRITEMSG
								return 1;
						}
						vtcp_log("[%s][%d]�ٽ�����[%ld],����[%ld]\n",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,daytermval);
						if ( pub_base_CompDblVal(g_td_mst_hst.tx_amt,g_td_mst.bal)==0 || \
							pub_base_CompDblVal(-g_td_mst_hst.tx_amt,g_td_mst.bal)==0)
						{
							vtcp_log("[%d]�����Ǳʼ�¼[%.2f]\n",__LINE__,g_td_mst_hst.tx_amt);
							dq_acm += g_td_mst_hst.tx_amt * daytermval;
						}
						else
							dq_acm += (g_td_mst_hst.bal-g_td_mst_hst.tx_amt) * daytermval;
					}
					ret=pub_base_CalIntstDays(g_td_mst_hst.tx_date,PubMin( PubMin(g_td_mst.mtr_date,g_pub_tx.tx_date),g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					dq_acm += g_td_mst_hst.tx_amt * daytermval;
					vtcp_log("[%s][%d]�ֶ���[%ld],����[%ld],����[%.2f]\n",__FILE__,__LINE__,g_td_mst_hst.tx_date,daytermval,dq_acm);
					hst_cnt++;tot_cnt++;
				}
				Td_mst_hst_Clo_Sel();

				if (g_pub_tx.tx_date>=g_td_mst.mtr_date)
				{
					vtcp_log("��ʼ���㶨�ڲ�����Ϣ˰[%s][%d]\n",__FILE__,__LINE__);

					lxs +=dq_acm * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"Ӧ�ɵĶ���˰[%.2f] : ����=[%.2f],����[%.5f]",
						lxs,dq_acm,ll_ddval);
					WRITEMSG
					/* ׼����ϸ */
						g_mo_int_tax_hst.intst_acm=dq_acm;
					g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100;
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}

					if (g_td_mst.mtr_date>g_com_intst_tax_rate.beg_date && hst_cnt==1)
					{
						vtcp_log("���һ�ʽ��������������Ϣ˰�ĵ���,�ʻ��ӵ����������ռ����Ϣ˰\n");

						ret=pub_base_CalIntstDays(PubMax(hst_tx_date,g_com_intst_tax_rate.beg_date),PubMin( g_td_mst.mtr_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
						if (ret != 0)
						{
							sprintf(acErrMsg,"������������!!");
							WRITEMSG
								return 1;
						}
						lxs +=g_td_mst.bal * daytermval * ll_ddval * (1 + g_td_mst.flt_ratio) / L360 / L100 
							* g_com_intst_tax_rate.tax_rate_val;
						vtcp_log("���������Ϣ˰[%.2f],����[%ld]\n",lxs,daytermval);
						/* ׼����ϸ */
						g_mo_int_tax_hst.tx_amt=g_td_mst.bal;
						g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
						g_mo_int_tax_hst.rate=ll_ddval * (1 + g_td_mst.flt_ratio);
						g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
						g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
						g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
						if(ss_flag != 1)
						{
							if (ret=insert_hst(&g_mo_int_tax_hst))
								return(ret);
						}
					}
				}
				else
				{
					vtcp_log("[%s][%d]��ǰ֧ȡ��������Ϣ\n",__FILE__,__LINE__);
					long tmp_days=0; double tmp_acm=0;
					if (hst_cnt==1 && tot_cnt>0)
					{
						vtcp_log("[%s][%d]��ǰ֧ȡ,��ʷ��ϸ��û�����˰������[%ld][%ld]�ļ�¼,�������",__FILE__,__LINE__,g_com_intst_tax_rate.beg_date,g_com_intst_tax_rate.end_date);
						ret=pub_base_CalIntstDays(g_com_intst_tax_rate.beg_date,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&tmp_days);
						if (ret != 0)
						{
							sprintf(acErrMsg,"������������!!");
							WRITEMSG
								return 1;
						}
						tmp_acm=g_td_mst.bal * tmp_days;
						vtcp_log("����[%d],����[%.2f],����[%.2f]\n",tmp_days,g_td_mst.bal,tmp_acm);
					}
					lxs += (dq_acm + tmp_acm) * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"��ǰ֧��������Ϣ˰[%.2f] : ��ǰ֧ȡ���ڻ���[%.2f],����=[%ld],����[%.5f]",
						lxs,dq_acm,daytermval,ll_hhval);
					WRITEMSG
					/* ׼����ϸ */
						g_mo_int_tax_hst.intst_acm=hq_acm;
					g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
					if(ss_flag != 1)
					{
						if (ret=insert_hst(&g_mo_int_tax_hst))
							return(ret);
					}
				}
				/* ���ں���ڲ����ֶַ� */
				/* ��Ϊ���������������ж���ֹ�����Բ����ظ��������ڲ��ֵĻ�����Ϣ */
				if (g_pub_tx.tx_date>=g_td_mst.mtr_date )
				{
					vtcp_log("[%s][%d]���ں�Ļ��ڲ��ֻ����Էֶ�\n",__FILE__,__LINE__);
					ret=pub_base_CalIntstDays(PubMax(g_td_mst.mtr_date,g_com_intst_tax_rate.beg_date),PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),intst_term_type,&daytermval);
					if (ret != 0)
					{
						sprintf(acErrMsg,"������������!!");
						WRITEMSG
							return 1;
					}
					hq_acm += g_td_mst.bal * daytermval;
					vtcp_log("[%s][%d]�ֶ��𣭷ֶ�ֹ��[%ld]������[%ld],���ڻ���[%.2f]\n",__FILE__,__LINE__,PubMin(g_pub_tx.tx_date,g_com_intst_tax_rate.end_date),daytermval,hq_acm);

					lxs	+= hq_acm * ll_hhval * (1 + g_td_mst.flt_ratio) / L360 / L100 
						* g_com_intst_tax_rate.tax_rate_val;
					sprintf(acErrMsg,"������Ϣ˰[%.2f] : ���ڲ��ֻ���[%.2f],����=[%ld],����[%.5f]",
						lxs,hq_acm,daytermval,ll_hhval);
					WRITEMSG
					/* ׼����ϸ */
						g_mo_int_tax_hst.intst_acm=g_td_mst.bal * daytermval;
					g_mo_int_tax_hst.rate=ll_hhval * (1 + g_td_mst.flt_ratio);
					g_mo_int_tax_hst.intst=g_mo_int_tax_hst.intst_acm * g_mo_int_tax_hst.rate / L360 / L100; /* ��Ϣ */
					g_mo_int_tax_hst.intst_tax=g_mo_int_tax_hst.intst * g_com_intst_tax_rate.tax_rate_val;  /* ��Ϣ˰ */
					g_mo_int_tax_hst.intst_tax_rate=g_com_intst_tax_rate.tax_rate_val;/* ˰�� */
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
		vtcp_log("[%s][%d]�ֶ����˰��[%.2f]\n",__FILE__,__LINE__,*val);
		return (0);
	}
}
