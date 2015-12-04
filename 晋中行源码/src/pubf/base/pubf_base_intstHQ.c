#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intstHQ.c
* ����������    ���ڼ�����Ϣ
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
#include "mo_bail_rate_c.h"
#include "dd_mst_c.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"
#include "mo_sxc_hst_intst_c.h"

/**********************************************************************
* ��������    pub_base_CalIntstHQ()
* �������ܣ�  ���ڼ�����Ϣ
* ����/ʱ�䣺 2003��12��28��
*
* ������
*     ���룺
            ģʽ              int
            1--������Ϣ 2--��Ϣ�ռ�Ϣ
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
int pub_base_CalIntstHQ(int mode,double *dealval, double *factval,
                  double *val, double *keepval, char *reply)
{
    double lxjs;          /* ��Ϣ���� */   
    double rate_val;      /* ����ֵ */
    double sdsl;          /* ˰��ֵ */
    double lx;            /* ��Ϣ */
    double flt_ratio;     /* �������� */
    double tx_amt;        /* ������ */
    char intst_term_type[2]; /* ��Ϣ�������� */
    char llbh[4];         /* ���ʱ�� */
    char cur_no[3];       /* ���� */
    char tax_no[4];       /*˰�ʱ�� */
    long tx_date;         /* �������� */
    long grjxrq ;         /* ���˽�Ϣ���� */
    int year,month,day;
    long dayterm;
    int ret;

    struct mo_bail_rate_c g_mo_bail_rate_c;
    memset( &g_mo_bail_rate_c,0,sizeof(struct mo_bail_rate_c));

sprintf(acErrMsg,"���ڼ�Ϣ������ʼ����!!");
WRITEMSG
    (*dealval) = 0.00;
    (*factval) = 0.00;
    (*val)   = 0.00;
    (*keepval) = 0.00;   
    
    lxjs = g_dd_mst.intst_acm;
    flt_ratio = g_dd_mst.flt_ratio;

    strcpy(intst_term_type,g_dd_parm.intst_term_type);

    tx_amt = g_pub_tx.tx_amt1;
    tx_date = g_pub_tx.tx_date;
  
    MEMSET_DEBUG(cur_no, 0x0, sizeof(cur_no));
    MEMSET_DEBUG(tax_no, 0x0, sizeof(tax_no));
    strcpy(cur_no, g_dd_parm.cur_no);
    strcpy(tax_no, g_dd_parm.tax_no);
    
	if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
         sprintf(acErrMsg,"��Ʒ���еļ�Ϣ�������ʹ��� type=[%s]",
                        g_dd_parm.intst_term_type );
         WRITEMSG
         MEMSET_DEBUG(reply, 0x0, sizeof(reply));
         strcpy(reply, "W107");
         return 1;
    }

    pub_base_get_date(&year,&month,&day);
    if (month > 6)
    {
        grjxrq = (year + Lone) * 10000 + 06 * 100 + 30;
    }
    else
    {
        grjxrq = year * 10000 + 06 * 100 + 30;
    }

    mode = 1; /* ���ü������ */
 
    switch (mode)
    {
        case 1 :     /* ������Ϣ */
            

sprintf(acErrMsg,"������Ϣ");
WRITEMSG
            /* ���������յ���Ϣ�յ����� *
			ret=pub_base_CalIntstDays(tx_date,grjxrq,
                intst_term_type,&dayterm);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������������!!");
                WRITEMSG
                return 1;
            }

            dayterm = dayterm + Lone;
            
            if (tx_amt < 0.005)
            {
                tx_amt = - floor(-tx_amt*L100)/L100;
            }
            else
            {
                tx_amt = floor(tx_amt*L100)/L100;
            }
            
            * ������Ϣ���� *

sprintf(acErrMsg,"lxjs=[%lf], tx_amt=[%lf],dayterm=[%ld]",lxjs,tx_amt,dayterm);
WRITEMSG

            lxjs = lxjs - tx_amt * dayterm;

sprintf(acErrMsg,"lxjs=[%lf]",lxjs);
WRITEMSG
            *********/
            
            /* ȡ�������� */
            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
			/*modified by liuxuan 20061102 �����ڲ�����ȡ���ʱ�ż�����Ϣ */
				/**modified by ligl 2007-1-4 15:33***/
				if(memcmp(g_dd_parm.rate_no,"B00",3)==0)/**���Ϊһ��ͨȡ��������**/
				{
            	strcpy(llbh,HQLLBH);
				}
				else
				/*******end********/
            strcpy(llbh,g_dd_parm.rate_no);
			if(strlen(llbh)==0)
            	strcpy(llbh,HQLLBH);
			/* modify over */

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

	if (memcmp(g_dd_parm.prdt_no,"131",3)==0)
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&g_mo_bail_rate_c,"bail_ac_no='%s'",g_pub_tx.ac_no);
		if (ret==0)
		{	
		rate_val=g_mo_bail_rate_c.ic_rate;
		}
		else
		{
		sprintf(acErrMsg,"���ҳ���!![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return ret;
		}
	}else 	if (memcmp(g_dd_parm.prdt_no,"142",3)==0)
	{
		rate_val=g_dd_mst.rate;
	}
	else
	{
		ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
	}
		/* added by liuxuan 20071010 ����Э���� */
		struct dd_mst_c tmp_dd_mst;
		memset(&tmp_dd_mst,'\0',sizeof(tmp_dd_mst));
		ret = Dd_mst_Sel(g_pub_tx.reply, &tmp_dd_mst, "ac_id=%ld and ac_seqn=%d",
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
		if (ret != 0 && ret!=100)
		{
			sprintf(acErrMsg, "��ѯ�������ļ�����!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(g_dd_mst.ac_type[0]=='6'){
			vtcp_log("[%s][%d]�˻�[%ld]��Э����,ȡ�ֻ�����\n",__FILE__,__LINE__,g_pub_tx.ac_id);
			rate_val=g_dd_mst.rate;
		}
		if (memcmp(g_dd_mst.prdt_no,"142",3)==0)/**  ������֤��ȡ�ֻ����� add by chenchao 20120619**/
		{
			rate_val = g_dd_mst.rate;
		}
		/* added over */

sprintf(acErrMsg,"rate_val=[%lf],lxjs=[%lf],flt_ratio=[%lf]��g_dd_mst.ac_type[0]=[%c],g_dd_mst.prdt_no=[%s]",
        rate_val,lxjs,flt_ratio,g_dd_mst.ac_type[0],g_dd_mst.prdt_no);
WRITEMSG

            lx = rate_val * lxjs * (1 + flt_ratio) / L360 / L100;
            
            if (strcmp(tax_no,SLBH) == 0)
            {
                ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
                if (ret != 0 && ret == 1)
                {
                    sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                    WRITEMSG
                    MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                    strcpy(reply,"W100");
                    return 1;
                }
                else if (ret < 0)
                {
                    sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d",ret);
                    WRITEMSG
                    strcpy(reply,"W101");
                    return 1;
                }
				g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                (*val) = lx * sdsl;
				vtcp_log("[%s][%d]˰����[%.5f]\n",__FILE__,__LINE__,sdsl);
            }
            
            (*dealval) = pub_base_PubDround_1(lx);
            (*val)     = pub_base_PubDround_1(*val);

            if (*dealval < 0)
            {
                (*dealval) = 0.00;
                (*val)     = 0.00;
                (*factval) = 0.00;
            }
            
            (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        *dealval,*factval,*val,*keepval);
WRITEMSG

            break;

        case 2 :     /* ��Ϣ�ռ�Ϣ */

sprintf(acErrMsg,"��Ϣ�ռ�Ϣ");
WRITEMSG

            /* ȡ�������� *
            if (month != 6 || day != 30)
            {
                sprintf(acErrMsg, "���첻�ǽ�Ϣ�� [%d]/[%d]/[%d]",
                        year, month, day);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply, "W113");
                return 1;
            }*/
            MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));
            strcpy(llbh,HQLLBH);

sprintf(acErrMsg,"llbh=[%s],cur_no=[%s],tx_date=[%ld]",llbh,cur_no,tx_date);
WRITEMSG

	if (memcmp(g_dd_mst.prdt_no,"142",3)==0)/**  ������֤��ȡ�ֻ����� add by chenchao 20120619**/
	{
		rate_val = g_dd_mst.rate;
	}
	else
	{
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret);
                WRITEMSG
                MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                strcpy(reply,"W110");
                return ret;
            }
        }
            /* added by liuxuan 20071010 ����Э���� */
		struct dd_mst_c tmp_dd_mst1;
		memset(&tmp_dd_mst1,'\0',sizeof(tmp_dd_mst1));
		ret = Dd_mst_Sel(g_pub_tx.reply, &tmp_dd_mst1, "ac_id=%ld and ac_seqn=%d",
		g_pub_tx.ac_id, g_pub_tx.ac_seqn);
		if (ret != 0 && ret!=100)
		{
			sprintf(acErrMsg, "��ѯ�������ļ�����!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			return 1;
		}
		if(g_dd_mst.ac_type[0]=='6'){
			vtcp_log("[%s][%d]�˻�[%ld]��Э����,ȡ�ֻ�����\n",__FILE__,__LINE__,g_pub_tx.ac_id);
			rate_val=g_dd_mst.rate;
		}
sprintf(acErrMsg,"lxjs=[%lf],rate_val=[%lf],flt_ratio=[%lf]",
        lxjs,rate_val,flt_ratio);
WRITEMSG

            lx = lxjs * rate_val * (1 + flt_ratio) / L360 / L100;
            
            if (strcmp(tax_no,SLBH) == 0)
            {
                ret = pub_base_GetRate(SLBH, tx_date, &sdsl);
                if (ret != 0 && ret == 1)
                {
                    sprintf(acErrMsg,"���ڲ�Ʒ��Ϣ˰˰�ʲ����� %d",ret);
                    WRITEMSG
                    MEMSET_DEBUG(reply, 0x0, sizeof(reply));
                    strcpy(reply,"W100");
                    return 1;
                }
                else if (ret < 0)
                {
                    sprintf(acErrMsg,"ȡ��Ϣ˰˰�ʴ��� %d",ret);
                    WRITEMSG
                    strcpy(reply,"W101");
                    return 1;
                }
				g_pub_intst.tax_intst = pub_base_PubDround_1(lx);
                (*val) = lx * sdsl;
            }
            
            (*dealval) = pub_base_PubDround_1(lx);
            (*val)     = pub_base_PubDround_1(*val);

            if (*dealval < 0)
            {
                (*dealval) = 0.00;
                (*val)     = 0.00;
                (*factval) = 0.00;
            }
            
            (*factval) = (*dealval) - (*val);

sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf],keepval=[%lf]",
        dealval,factval,val,keepval);
WRITEMSG

            break;
        default :
            break;
    }
    
sprintf(acErrMsg,"���ڼ�Ϣ�������ý���!!");
WRITEMSG
    return 0;
}

/******************************************************************
 ****    �������Ĵ���Ϣ  add by zqbo 20141121                  ****
 ****    ����˵����                                            ****
 ****    tx_amt �ͷŽ����Ϊ���Ĵ�Ǽǲ�����ʾȫ�ͷ�     ****
 ****    flag   ��־��                                         ****
 ****               '0'-������Ϣ������Ҫ�Ǽ���ϸ               ****
 ****               '2'-�ͷ���Ϣ����Ҫ�Ǽ���ϸ                 ****
 ****               '4'-��Լ��Ϣ����Ҫ�Ǽ���ϸ                 ****
 ******************************************************************/
 
int iSxc_intst(long ac_id, long ac_seqn, double tx_amt, double *intst, char flag)
{
    int iRet = 0;
    double dIntst = 0.00;        /**�ܹ������Ϣ����ֵ**/
    double dIntst_hst = 0.00;    /**ÿ���ͷŵĽ����Ϣ**/
    double dTx_amt = 0.00;       /**ÿ�����ͷŽ��**/
    double dSy_tx_amt = tx_amt;  /**ʣ���ͷŽ��=��ʼ��Ϊ�ͷŽ��**/
    
    struct mo_sxc_hst_c       sMo_sxc_hst;
    struct mo_sxc_hst_c       sMo_sxc_hst1;
    memset(&sMo_sxc_hst,       0x00, sizeof(sMo_sxc_hst));
    memset(&sMo_sxc_hst1,      0x00, sizeof(sMo_sxc_hst1));
    
    if(flag != '2' && flag != '4' && flag != '0')
    {
        strcpy(g_pub_tx.reply, "S036");
        sprintf( acErrMsg, "�����flag��־[%c]����Ϊ2 4 0!", flag );
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%ld and sts='1' order by rate_date desc ", ac_id, ac_seqn); /**����ʱ�޷���Dec_Upd**/
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D101");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
        /***ÿ��ѭ����ʼ��***/
        dIntst_hst = 0.00;
        dTx_amt = 0.00;
        memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
        
        iRet = Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
        if(iRet == 100)
        {
            break;
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Dec_Upd����!!!" );
            WRITEMSG
            goto ErrExit;
        }
        /***ȷ��ÿ����ϸ�����ͷŵĽ��***/
        if(pub_base_CompDblVal(dSy_tx_amt, sMo_sxc_hst.hst_bal) <0 )
        {
            dTx_amt = dSy_tx_amt;
        }
        else
        {
            dTx_amt = sMo_sxc_hst.hst_bal;
        }
        
        /**------------�����ͷŽ�����Ϣ ���Ǽǽ�Ϣ��ϸ-------------***/
        iRet = iSxc_hst_intst(sMo_sxc_hst, dTx_amt, &dIntst_hst, flag);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf(acErrMsg, "��Ϣ֧����Ŀ���˴���!!");
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        /**----------------����ԭ�Ǽ���ϸ----------------**/
        if(flag != '0')
        {
            sMo_sxc_hst.intst += dIntst_hst;
            sMo_sxc_hst.hst_bal -= dTx_amt;
            if(sMo_sxc_hst.hst_bal == 0.00)  /**��ϸ���Ϊ0��״̬��Ϊ����**/
            {
                    sMo_sxc_hst.sts[0] = '*';
            }
            iRet = sql_execute("update mo_sxc_hst set intst=%lf, hst_bal=%lf, sts='%s' where ac_id=%ld and ac_seqn=%d and rate_date=%ld and sts='1'", \
                                       sMo_sxc_hst.intst, sMo_sxc_hst.hst_bal, sMo_sxc_hst.sts, ac_id, ac_seqn, sMo_sxc_hst.rate_date);
            if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "Mo_sxc_hst_Upd_Upd����!!!" );
                WRITEMSG
                Mo_sxc_hst_Clo_Sel();
                goto ErrExit;
            }
            /*�������Ĵ����,�����ѽ���Ϣ��������ת�����Ĵ棬�������� add by wp 20151126*/
            iRet = sql_execute("update mo_sxc set bal=bal-%lf where ac_id=%ld and ac_seqn=%d and  sts='1'", \
                                       dTx_amt, ac_id, ac_seqn);
            if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "����Mo_sxc.bal����!!!" );
                WRITEMSG
                goto ErrExit;
            }
        }
        /**----------�ۼ��ͷ�����Ϣ------------**/
        dIntst += dIntst_hst; 
        
        /**-----------������Ƿ�����˳�-----------------**/
        dSy_tx_amt -= dTx_amt;
        if(dSy_tx_amt == 0.00)
        {
            break;
        }
    }
    Mo_sxc_hst_Clo_Sel();
    
    (*intst) = dIntst;
    vtcp_log("����ϢΪ[%lf]", dIntst);
    
OkExit:
    sprintf(acErrMsg, "�ͷż�Ϣ����ɹ�!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "�ͷż�Ϣ����ʧ��!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}

/******************************************************************
 ****    �������Ĵ�ÿһ����ϸ����Ϣ  add by zqbo 20141121      ****
 ****    ����˵����                                            ****
 ****    tx_amt �ͷŽ��                                       ****
 ****    flag   ��־��                                         ****
 ****               '0'-������Ϣ������Ҫ�Ǽ���ϸ               ****
 ****               '2'-�ͷ���Ϣ����Ҫ�Ǽ���ϸ                 ****
 ****               '3'-��Ϣ����Ҫ�Ǽ���ϸ                     ****
 ****               '4'-��Լ��Ϣ����Ҫ�Ǽ���ϸ                 ****
 ******************************************************************/

int iSxc_hst_intst(struct mo_sxc_hst_c mo_sxc_hst, double tx_amt, double *intst_hst, char flag)
{
    int iRet = 0;
    int iDays = 0;
    int iIntst_days = 0;
    long   lRate_date = 0;
    double dRate_val = 0.00;
    double dIntst_hst = 0.00;
    
    char cRate_code[4];
    struct mo_sxc_hst_intst_c sMo_sxc_hst_intst;
    
    memset(cRate_code, 0x00, sizeof(cRate_code));
    memset(&sMo_sxc_hst_intst, 0x00, sizeof(sMo_sxc_hst_intst));
    
    /***----����ȡ���ʵ�����----***/
    vtcp_log("aaaaaaaaaaaaaaaaaaϵͳ����==[%ld]", g_pub_tx.tx_date);
    iDays = pub_base_CalTrueDays(mo_sxc_hst.rate_date, g_pub_tx.tx_date);
    
    lRate_date = mo_sxc_hst.ic_date; /**����Ϊ��Ϣ�յ�����**/
    
    /***----ȡ�������ʱ��----***/
    if(iDays > 0 && iDays < 7)
    {
        strcpy(cRate_code, HQLLBH);
        
        lRate_date = g_pub_tx.tx_date;  /**����Ϊ֧ȡ�յ�����**/
    }
    else if(iDays >= 7 && iDays < 90)
    {
        strcpy(cRate_code, QTTZCK);
        
        lRate_date = g_pub_tx.tx_date; /**֪ͨ���Ϊ֧ȡ�յ�����**/
    }
    else if(iDays >= 90 && iDays < 180)
    {
         strcpy(cRate_code, ZCZQ03);
         
    }
    else if(iDays >= 180 && iDays < 360)
    {
        strcpy(cRate_code, ZCZQ06);
        
    }
    else if(iDays >=360 && iDays < 720)
    {
        strcpy(cRate_code, ZCZQ12);
        
    }
    else if(iDays >=720 && iDays < 1080)
    {
        strcpy(cRate_code, ZCZQ24);
        
    }
    else if(iDays >=1080 && iDays < 1800)
    {
        strcpy(cRate_code, ZCZQ36);
        
    }
    else if(iDays >= 1800)
    {
        strcpy(cRate_code, "205");
        
    }
    else if(iDays <= 0)
    {
        iDays = 0;
        strcpy(cRate_code, HQLLBH);
    }
    else
    {
        sprintf(acErrMsg, "������������![%d]", iDays);
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = pub_base_getllz(cRate_code, "01", lRate_date, &dRate_val);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W110");
    }
    /**----�����Ϣ����, Ŀǰ�ǰ�ÿ��30����-----**/
    iIntst_days = pub_base_CalTrueDays(mo_sxc_hst.ic_date, g_pub_tx.tx_date);
    /** -----������Ϣ---------**/
    dIntst_hst = pub_base_PubDround_1(tx_amt * iIntst_days * dRate_val / L360 / L100);
    vtcp_log("iIntst_days[%ld], dRate_val[%lf], tx_amt[%lf] dIntst_hst[%lf]", iIntst_days, dRate_val, tx_amt, dIntst_hst);
    
    /***-------�Ǽ��ͷŽ�Ϣ��ϸ---------***/
    if(flag != '0')
    {
        sMo_sxc_hst_intst.ac_id     = mo_sxc_hst.ac_id;
        sMo_sxc_hst_intst.ac_seqn   = mo_sxc_hst.ac_seqn;
        sMo_sxc_hst_intst.rate_date = mo_sxc_hst.rate_date;
        sMo_sxc_hst_intst.ic_date   = mo_sxc_hst.ic_date;
        if(flag != '3')
        {
            sMo_sxc_hst_intst.hst_bal = mo_sxc_hst.hst_bal - tx_amt;
        }
        else
        {
            sMo_sxc_hst_intst.hst_bal = mo_sxc_hst.hst_bal;	
        }
        strcpy(sMo_sxc_hst_intst.opn_br_no, mo_sxc_hst.opn_br_no);
        strcpy(sMo_sxc_hst_intst.tx_br_no,  g_pub_tx.tx_br_no);
        sMo_sxc_hst_intst.tx_amt = tx_amt;
        sMo_sxc_hst_intst.intst = dIntst_hst;
        sMo_sxc_hst_intst.rate = dRate_val;
        sMo_sxc_hst_intst.days = iIntst_days;
        strcpy(sMo_sxc_hst_intst.rate_code, cRate_code);
        strcpy(sMo_sxc_hst_intst.tx_tel,    g_pub_tx.tel);
        sMo_sxc_hst_intst.tx_date = g_pub_tx.tx_date;
        sMo_sxc_hst_intst.acct_date = g_pub_tx.tx_date;
        sMo_sxc_hst_intst.trace_no = g_pub_tx.trace_no;
        strcpy(sMo_sxc_hst_intst.tx_br_no,  g_pub_tx.tx_br_no);
        strcpy(sMo_sxc_hst_intst.tx_tel,    g_pub_tx.tel);
        sMo_sxc_hst_intst.type[0] = flag;   /**����Ϊ 2-�ͷ� 3-��Ϣ 4-��Լ��Ϣ**/
        sMo_sxc_hst_intst.sts[0] = '*';     /**״̬��Ϊ*-����**/
		
	iRet = Mo_sxc_hst_intst_Ins(sMo_sxc_hst_intst, g_pub_tx.reply);
	if(iRet)
	{
	    strcpy(g_pub_tx.reply, "D102");
	    sprintf( acErrMsg, "Mo_sxc_Dec_Upd����!!!" );
	    WRITEMSG
	    goto ErrExit;
	}
    }
    
    (*intst_hst) = dIntst_hst;
    vtcp_log("��ϸ��ϢΪ[%lf]", dIntst_hst);
	    
OkExit:
    sprintf(acErrMsg, "��Ϣ��ϸ�Ǽǳɹ�!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
        
    sprintf(acErrMsg, "��Ϣ��ϸ�Ǽ�ʧ��!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
	    
}
