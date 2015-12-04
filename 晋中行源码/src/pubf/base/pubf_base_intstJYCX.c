#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstJYCX.c
* ����������    �����ȡ������Ϣ(��������)
* ���ú���˵����
*
*
* ��    ��:     rob
* ������ڣ�    2003��12��28��
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"


/**********************************************************************
* ��������    pub_base_CalIntstJYCX()
* �������ܣ�  �����ȡ������Ϣ
* ����/ʱ�䣺 2003��12��28��
*
* ������
*     ���룺
*     ���: Ӧ����Ϣ          double
*           ʵ����Ϣ          double
*           ������Ϣ˰        double
*           ��ֵ��Ϣ          double
*	    ����֤��	      char(2)  1-�� 0-��
*           ��Ӧ��            char(4)
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_CalIntstJYCX(double *dealval, double *factval,
                  double *val, double *keepval, char edu_ind[2], char *reply)
{
	char llbh[4];
	int ret;
	double sdsl;
	long tmprq, date99;
	double ye99,js99;
	double lx;
	long dayterm;
	double rate_val;
    double dd_rate_val=0,hh_rate_val=0; /*���ںͻ�������ֵ,�����ֶ���Ϣ˰����ʡȥ����ȡ���ʵĲ���*/
	*dealval = 0.00;
	*factval = 0.00;
	*val = 0.00;
	*keepval = 0.00;

	sprintf(acErrMsg, "dealval=[%lf], factval=[%lf], val=[%lf], keepval=[%lf]",
		*dealval, *factval, *val, *keepval);
	WRITEMSG

	date99 = 19991101;

	if (edu_ind[0] == 'Y')
	{
		sprintf(acErrMsg, "�н���֤���Ľ�������");
		WRITEMSG
		if (g_pub_tx.tx_date < g_td_mst.mtr_date)
		{
			sprintf(acErrMsg, "δ����֧ȡ");
			WRITEMSG

			ret = pub_base_CalMouths(g_td_mst.opn_date, g_pub_tx.tx_date);
			if (ret < 0)
			{
				sprintf(acErrMsg, "������������!! [%ld], [%ld]", 
					g_td_mst.opn_date, g_pub_tx.tx_date);
				WRITEMSG
				return ret;
			}

			if (ret < 3)
			{
				/* ������ */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, HQLLBH);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			}
			else if ( ret < 6 )
			{
				/* �������� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ03);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 12 )
			{
				/* �������� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ06);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if (ret < 24)
			{
				/* ��������ȡһ�� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ12);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 36 )
			{
				/* ��������ȡ����  */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ24);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else if ( ret < 60 )
			{
				/* ��������ȡ���� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, ZCZQ36);
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}
			else 
			{
				/* ��������ȡ���� */
				MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
				strcpy(llbh, "205");
				ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			}


sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}

			*dealval = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			(*dealval) = pub_base_PubDround_1( *dealval );
			(*val) = 0.00;
			(*factval) = (*dealval);
dd_rate_val=rate_val;
		}
		else
		{
			/* ���ڻ����� */
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
dd_rate_val=rate_val;
			/* ������Ϣ */
sprintf(acErrMsg, "intst_acm=[%lf], rate_val=[%lf] ", g_td_mst.intst_acm, rate_val);
WRITEMSG

			*dealval = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			(*val) = 0.00;

sprintf(acErrMsg, "��Ϣֵ:[%lf], ��Ϣ˰ֵ:[%lf]", *dealval, *val);
WRITEMSG

			/* ������Ϣ */

			ret=pub_base_CalIntstDays(g_td_mst.mtr_date, 
					g_pub_tx.tx_date, g_td_parm.intst_term_type,&dayterm); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"������������!!"); 
				WRITEMSG 
				return 1; 
			}

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			(*val) = rate_val * dayterm * g_pub_tx.tx_amt1 * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			
			(*dealval) += (*val);
			(*dealval) = pub_base_PubDround_1( *dealval );
		}
	}
	else if (edu_ind[0] == 'N')
	{
		sprintf(acErrMsg, "�޽���֤���Ľ�������");
		WRITEMSG
		if (g_pub_tx.tx_date < g_td_mst.mtr_date)
		{
			sprintf(acErrMsg, "δ����֧ȡ");
			WRITEMSG
			
			/* ���������ʼ��� */
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			lx = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG					
			(*dealval) = pub_base_PubDround_1( lx );
			
			if (g_td_mst.ic_date >= date99)
        		{
TRACE
            			js99 = 0.00;
           			ye99 = 0.00;
        		}
        		else
        		{   /* ��ѯdate99֮ǰ�Ķ��ڴ����ϸ����js99 ye99��ֵ */
TRACE
           	 		tmprq = date99;
            		ret = pub_base_gettdmsthst(g_td_mst.ac_id,
							g_td_mst.ac_seqn,&tmprq,&js99,&ye99,reply);
            		if (ret != 0)
            		{
                		sprintf(acErrMsg,"ȡ���ڴ����ϸ���� [%d]",ret);
                		WRITEMSG
                		return 1;
            		}
        		}

 			ret=pub_base_CalIntstDays(date99,g_td_mst.mtr_date,
 				g_td_parm.intst_term_type,&dayterm);
 			if (ret != 0)
 			{
 				sprintf(acErrMsg,"������������!!");
 				WRITEMSG
				 return 1;
 			}
 			/* (*val) = lx * (1 - (js99 - ye99 * dayterm) / g_td_mst.intst_acm);*/
			(*val) = lx;

			(*dealval) = pub_base_PubDround_1( *dealval );

		}
		else
		{
			/* ���ڻ����� */
			sprintf(acErrMsg, "���ڻ�����֧ȡ!!");
			WRITEMSG
			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, g_td_parm.felbck_rate_no);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_td_mst.opn_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
dd_rate_val=rate_val;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG			
			/* ������Ϣ */
			lx = g_td_mst.intst_acm * rate_val * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]date99=[%ld]",lx,rate_val,date99);
WRITEMSG
			(*dealval) = lx;
sprintf(acErrMsg,"lx=[%lf],rate_val=[%lf]dealval=[%ld]",lx,rate_val,dealval);
WRITEMSG				
			/* ���ڲ�����Ϣ˰ */	
			if (g_td_mst.ic_date >= date99)
        	{
TRACE
           		TRACE
           		js99 = 0.00;
           		ye99 = 0.00;
        	}
        	else
        	{   /* ��ѯdate99֮ǰ�Ķ��ڴ����ϸ����js99 ye99��ֵ */
TRACE
           		tmprq = date99;
           		ret = pub_base_gettdmsthst(g_td_mst.ac_id, 
						g_td_mst.ac_seqn,&tmprq,&js99,&ye99,reply);
           		if (ret != 0)
           		{
               		sprintf(acErrMsg,"ȡ���ڴ����ϸ���� [%d]",ret);
               		WRITEMSG
               		return 1;
           		}
        	}
			
			/***
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)

			****/
 			ret=pub_base_CalIntstDays(date99,g_td_mst.mtr_date,
 				g_td_parm.intst_term_type,&dayterm);
 			if (ret != 0)
 			{
 				sprintf(acErrMsg,"������������!!");
 				WRITEMSG
				 return 1;
 			}
 			/* (*val) += lx * (1 - (js99 - ye99 * dayterm) / g_td_mst.intst_acm);*/
			(*val) = lx;
			sprintf(acErrMsg, "CGH***���ڲ�����Ϣ˰:[%lf]", *val);
			WRITEMSG

			/* ������Ϣ */

			ret=pub_base_CalIntstDays(g_td_mst.mtr_date, 
					g_pub_tx.tx_date, g_td_parm.intst_term_type,&dayterm); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"������������!!"); 
				WRITEMSG 
				return 1; 
			}

			MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			strcpy(llbh, HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld],rate_val=[%lf]",
		llbh,g_td_parm.cur_no,g_pub_tx.tx_date,rate_val);
WRITEMSG

			ret = pub_base_getllz(llbh, g_td_parm.cur_no, 
							g_pub_tx.tx_date, &rate_val); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
				WRITEMSG 
				MEMSET_DEBUG(reply, 0x0, sizeof(reply)); 
				strcpy(reply,"W110"); 
				return ret; 
			}
hh_rate_val=rate_val;
			lx = rate_val * dayterm * g_pub_tx.tx_amt1 * \
					(1 + g_td_mst.flt_ratio) / L360 / L100;
			
			(*dealval) += lx;
			
			/* ���ڲ�����Ϣ */
			/*****
			if (strcmp(g_td_parm.intst_tax_no,SLBH) == 0)
 			
			*******/

			(*val) += lx;
			sprintf(acErrMsg, "���ڲ�����Ϣ˰:[%lf]", lx);
			WRITEMSG

			(*dealval) = pub_base_PubDround_1( *dealval );
		}
	}
	else
	{
		sprintf(acErrMsg, "����֤����־����!! [%s]", edu_ind);
		WRITEMSG
		return 1;
	}
	
   	if( strcmp(g_td_parm.intst_tax_no, SLBH) == 0 || pub_base_CompDblVal((*val), 0.00) == 1)
   	{
 		/* ȡ��Ϣ˰˰�� */
       	ret = pub_base_GetRate(SLBH, g_pub_tx.tx_date, &sdsl);
       	if ((ret != 0) && (ret == 1))
       	{
       		sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
       		WRITEMSG
       		strcpy(reply,"W100");
       		return 1;
       	}
       	else if(ret < 0)
       	{
       		sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d ", ret);
       		WRITEMSG
       		strcpy(reply,"W101");
       		return 1;
       	 }
		g_pub_intst.tax_intst = pub_base_PubDround_1(*val);
       	(*val) = (*val) * sdsl;
       	(*val) = pub_base_PubDround_1(*val);
    }
	(*factval) = (*dealval) - (*val);

	/* added by liuxuan 2007-08-01 �޸���Ϣ˰ */
	if(g_pub_tx.tx_date<=g_td_mst.mtr_date && edu_ind[0]=='Y'){
		vtcp_log("[%s][%d]�յ��ڻ���δ��������֤������˰,�����÷ֶ���Ϣ����\n",__FILE__,__LINE__);
		return(0);
	}
	if(strcmp(g_td_parm.intst_tax_no,SLBH) == 0){
		vtcp_log("[%s][%d]��ʼ���÷ֶ�����Ϣ˰����\n",__FILE__,__LINE__);
		if(iGetAreaTax(val,factval,dealval,g_td_parm.intst_term_type,7,dd_rate_val,hh_rate_val,edu_ind)){
			vtcp_log("[%s][%d]���÷ֶ�����Ϣ˰��������\n",__FILE__,__LINE__);
			return(1);
		}
	}
	/* add over */
	return 0;
}
