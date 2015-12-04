/***************************************************************
* �� �� ��:     spD011.c
* ����������	��Ϣ����(��Ϣ����Ϣ)
*
* ��    ��:     jack
* ������ڣ�    2004��1��10��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"
#include "mo_infrm_c.h"

static    long beg_date,end_date;
spD011()
{
	long jxrq;
    int ret;
	double dealval,val,factval,keepval;

	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
   
	dealval = 0.00;
	val = 0.00;
	factval = 0.00;
	keepval = 0.00;

    /*** ��ʼ��ȫ�ֱ��� ***/
    if (pub_base_sysinit())
    {
        strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0230",g_pub_tx.prdt_code);
    get_zd_double("0410",&g_pub_tx.tx_amt1);
    get_zd_long("0460",&beg_date);
    get_zd_long("0470",&end_date);
    get_zd_double("0840",&g_pub_tx.rate);

    g_pub_tx.ac_id = 999999999;
    g_pub_tx.ac_seqn = 1;
sprintf(acErrMsg,"beg_date[%ld]end_date[%ld]!" , beg_date,end_date);
WRITEMSG 
    ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
          g_pub_tx.prdt_code);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�����!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no = '%s'",
              g_pub_tx.prdt_code);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ���ڲ�Ʒ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"û�иò�Ʒ��!![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D157");
            goto ErrExit;
        }

        /* ����һ�������ʻ� */
        strcpy(g_pub_tx.ac_id_type,"2"); /* ���ڲ�Ʒ */
        g_pub_tx.tx_date = beg_date;
        ret = dummy_td_mst();
        if (ret != 0)
        {
            sprintf(acErrMsg,"���ⶨ���ʻ�����!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D158");
            goto ErrExit;
        }

        /* ����汾ȡϢ �����ȡ ������ȡȡ����ϸ ֪ͨ���Ǽ�֪ͨ�Ǽǲ� */
        if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2' || \
            g_td_parm.td_type[0] == '3')
        {
            ret = dummy_td_mst_hst(beg_date,end_date);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���ⶨ���ʻ�ȡ����ϸ����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"D158");
                goto ErrExit;
            }
        }
        if ( g_td_parm.td_type[0] == '4' )
        {
            ret = dummy_mo_infrm();
            if (ret != 0)
            {
                sprintf(acErrMsg,"����Ǽ�֪ͨ�Ǽǲ�����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"D158");
                goto ErrExit;
            }
        }
    }
    else
    {
        /* ����һ�������ʻ� */
        strcpy(g_pub_tx.ac_id_type,"1"); /* ���ڲ�Ʒ */
        g_pub_tx.tx_date = beg_date;
        ret = dummy_dd_mst();
        if (ret != 0)
        {
            sprintf(acErrMsg,"��������ʻ�����!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D158");
            goto ErrExit;
        }

		ret = pub_base_half_year(g_pub_tx.tx_date);
		if (ret == 1)
		{
			/* �ϰ��� */
            jxrq = g_pub_tx.tx_date / 10000 *10000 + 701;
		}
		else
		{
			jxrq = (g_pub_tx.tx_date / 10000 + 1) * 10000 + 701;
		}

		while(end_date >= jxrq)
		{
			/* ���˽�Ϣ�ս�Ϣ */
            g_pub_tx.tx_date = jxrq;
    		ret = pub_base_CalIntst(&g_pub_intst);
    		if (ret != 0)
    		{
        		sprintf(acErrMsg,"���ü�����Ϣ��������!!!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"D158");
        		goto ErrExit;
    		}
			g_dd_mst.bal += g_pub_intst.factval;
			dealval += g_pub_intst.dealval;
			val  += g_pub_intst.val;
			factval += g_pub_intst.factval;

    		/*** ������Ϣ�����ĺ��� ***/
sprintf(acErrMsg,"XXXXXintst_acm=[%lf]",g_dd_mst.intst_acm);
WRITEMSG
    		g_dd_mst.intst_acm=0.00;
			g_pub_tx.tx_amt1 = g_dd_mst.bal;
    		if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
        		g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
        		0.00,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
        		g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
    		{
        		sprintf(acErrMsg,"������Ϣ��������!");
        		WRITEMSG
				goto ErrExit;
    		}

sprintf(acErrMsg,"XXXXXintst_acm=[%lf]",g_dd_mst.intst_acm);
WRITEMSG
			jxrq = (jxrq/10000 + 1) * 10000 + 701;
		}

        g_pub_tx.tx_date = end_date;
        /* ����ȡ����� */
        if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            goto ErrExit;
        }

    }

    g_pub_tx.tx_date = end_date;
    strcpy( g_pub_intst.edu_ind , "Y" );
    sprintf(acErrMsg, "intst_acm=[%lf]", g_td_mst.intst_acm);
    WRITEMSG
    ret = pub_base_CalIntst(&g_pub_intst);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���ü�����Ϣ��������!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D158");
        goto ErrExit;
    }
	dealval += g_pub_intst.dealval;
	val += g_pub_intst.val;
	keepval += g_pub_intst.keepval;
    factval += g_pub_intst.factval;

	/*** ������� ***/
    set_zd_double("1001",dealval);
    set_zd_double("1002",val);
    set_zd_double("1003",keepval);
    set_zd_double("1004",factval + keepval);

OkExit:
	strcpy(g_pub_tx.reply,"D159");
    sprintf(acErrMsg,"��Ϣ������ɹ�!! ");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"��Ϣ����Ϣ���ɹ�!! ");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int dummy_dd_mst()
{
    double rate_val;
    int ret;

    g_dd_mst.ac_id = g_pub_tx.ac_id;
    g_dd_mst.ac_seqn = g_pub_tx.ac_seqn;
    strcpy(g_dd_mst.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(g_dd_mst.prdt_no,g_pub_tx.prdt_code);
    g_dd_mst.bal=g_pub_tx.tx_amt1;
    g_dd_mst.acbo_bal=g_pub_tx.tx_amt1;
    g_dd_mst.ys_bal=0.00;
    g_dd_mst.hst_bal=g_pub_tx.tx_amt1;
    g_dd_mst.rate=g_pub_tx.rate;
    g_dd_mst.flt_ratio=0.00;
    g_dd_mst.opn_date=g_pub_tx.tx_date;
    g_dd_mst.ic_date=g_pub_tx.tx_date;
    g_dd_mst.lst_date=g_pub_tx.tx_date;

    /**�˻�����**/
    if( g_pub_tx.ac_type[0]=='S' || g_pub_tx.ac_type[0]=='D' )
    {
        strcpy(g_dd_mst.sttl_type,g_pub_tx.ac_type);
        strcpy(g_dd_mst.ac_type,"5");
    }
    else
    {
        strcpy(g_dd_mst.ac_type,g_pub_tx.ac_type);
        strcpy(g_dd_mst.sttl_type,"D");
    }

    /***ȡ���ʱ�Ŷ�Ӧ������***/
    ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,
        g_pub_tx.tx_date,&rate_val);
    if ( ret )
    {
        sprintf(acErrMsg,"ȡ����ֵ����!!");
        WRITEMSG
        return ret;
    }

    g_dd_mst.rate = rate_val;
    if(!strcmp(g_pub_tx.intst_type,""))
    {
        strcpy(g_pub_tx.intst_type,g_dd_parm.intst_type);
    }
    strcpy(g_dd_mst.intst_type,g_pub_tx.intst_type);

    /*** ������Ϣ�����ĺ��� ***/
    g_dd_mst.intst_acm=0.00;
    if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
        g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
        0.00,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
        g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
    {
        sprintf(acErrMsg,"������Ϣ��������!");
        WRITEMSG
        return 1;
    }

sprintf(acErrMsg,"intst_acm=[%lf]",g_dd_mst.intst_acm);
WRITEMSG

    g_dd_mst.hst_cnt=0;
    g_dd_mst.hst_pg=0;
    /** �����־��ֵ **/
    if( g_dd_parm.opn_aplic_ind[0]=='Y'||g_dd_mst.ac_type[0]=='1'|| \
        g_dd_mst.ac_type[0]=='3'||g_dd_mst.ac_type[0]=='4')
        strcpy(g_dd_mst.ac_sts,"0");
    else
        strcpy(g_dd_mst.ac_sts,"1");
    strcpy(g_dd_mst.hold_sts,"0");
    g_dd_mst.hold_amt=0.00;
    g_dd_mst.ctl_amt=0.00;
    strcpy(g_dd_mst.od_ind,"N");
    strcpy(g_dd_mst.draw_pwd,"N");
    g_dd_mst.cif_no=g_pub_tx.cif_no;
    strcpy(g_dd_mst.name,g_pub_tx.name);

    return 0;
}

int  dummy_td_mst()
{
    int ret;
    int dedr_intvl;
    double rate_val;
    long l_term=0;

    g_td_mst.ac_id=g_pub_tx.ac_id;
    g_td_mst.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(g_td_mst.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(g_td_mst.prdt_no,g_pub_tx.prdt_code);
    g_td_mst.bal=g_pub_tx.tx_amt1;
    g_td_mst.acbo_bal=g_pub_tx.tx_amt1;
    g_td_mst.ys_bal=0.00;
    g_td_mst.hst_bal=g_pub_tx.tx_amt1;
    g_td_mst.rate=0.00;
    g_td_mst.flt_ratio=0.00;

    g_td_mst.opn_date=g_pub_tx.tx_date;
    g_td_mst.ic_date=g_pub_tx.tx_date;
    g_td_mst.lst_date=g_pub_tx.tx_date;

    /**���㵽������**/
    if( g_td_parm.term_type[0]=='D' )
    {
        l_term=g_td_parm.term;
        pub_base_deadlineD(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
    }
    if( g_td_parm.term_type[0]=='Y' )
    {
        g_td_parm.term_type[0]='M';
        g_td_parm.term*=12;
    }
    if( g_td_parm.term_type[0]=='M' )
    {
        l_term=g_td_parm.term;
        pub_base_deadlineM(g_td_mst.opn_date,l_term,&g_td_mst.mtr_date);
    }
    g_pub_tx.mach_date=g_td_mst.mtr_date;

    sprintf(acErrMsg,"term_type[%s] term[%d][%d]opn[%d]mtr[%d]",
        g_td_parm.term_type,l_term,g_td_parm.term,
        g_td_mst.opn_date,g_td_mst.mtr_date);
    WRITEMSG

    /***ȡ���ʱ�Ŷ�Ӧ������***/
    ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,
          g_pub_tx.tx_date,&rate_val);
    if ( ret )
    {
        sprintf(acErrMsg,"ȡ����ֵ����!!");
        WRITEMSG
        return 1;
    }
    g_td_mst.rate = rate_val;

    if (g_td_parm.td_type[0]=='1' || g_td_parm.td_type[0]=='2'
        || g_td_parm.td_type[0] == '3')
    {
        /* �����ȡ ������ȡ �汾ȡϢ �����ܷ������� */
        if (g_td_parm.intvl_type[0] == 'Y')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 12;
        }
        else if (g_td_parm.intvl_type[0] == 'Q')
        {
            dedr_intvl=g_td_parm.dedr_intvl * 4;
        }
        else if (g_td_parm.intvl_type[0] == 'M')
        {
            dedr_intvl=g_td_parm.dedr_intvl;
        }
        else if (g_td_parm.intvl_type[0] == 'D')
        {
            /* ֻ�������µ������� */
            dedr_intvl = g_td_parm.dedr_intvl / 30;
        }
        else
        {
            sprintf(acErrMsg,"���ڲ�Ʒ���еļ�����ʹ���!!");
            WRITEMSG
            return 1;
        }

        if (g_td_parm.term_type[0] == 'Y')
        {
            g_td_mst.ttl_cnt = g_td_parm.term * 12 / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'M')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / dedr_intvl;
        }
        else if (g_td_parm.term_type[0] == 'D')
        {
            g_td_mst.ttl_cnt = g_td_parm.term / 30 / dedr_intvl;
        }
    }

    /**�˻�����**/
    switch( g_td_parm.td_type[0] )
    {
        case '1': /*�����ȡ*/
            g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
            sprintf(acErrMsg,"tx_cnt=[%d] ",g_td_mst.tx_cnt);
            WRITEMSG
            break;
        case '2': /*������ȡ*/
            /**NEED ��Ҫ����**/
            g_td_mst.td_amt=g_pub_tx.tx_amt1;
            g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1;
        case '3': /*�汾ȡϢ*/

            /**NEED ��Ҫ����**/
            /* ������Ϣ */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
                sprintf(acErrMsg,"����汾ȡϢ��Ϣ����!!");
                WRITEMSG
                return 1;
             }
            sprintf(acErrMsg,"�汾ȡϢ��ϢΪ:[%lf]",g_pub_intst.factval);
            WRITEMSG
            g_td_mst.td_amt=g_pub_intst.factval / g_td_mst.ttl_cnt;

            break;
        default :
            g_td_mst.td_amt=0.00;
            break;
    }

    if(!strcmp(g_pub_tx.intst_type,""))
    {
        strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);
    }
    strcpy(g_td_mst.intst_type,g_pub_tx.intst_type);

    /*** ������Ϣ�����ĺ��� NEED�������Ҫ�Ľ�����Ϊ��ͬ��ʽҪ��ͬ���ڴ���***/
    g_td_mst.intst_acm=0.00;

    if (g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '2')
    /* �����ȡ ������ȡ */
    {
        TRACE
        if( pub_base_CalAcm(g_pub_tx.tx_date,g_pub_tx.tx_date,
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
            0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            return 1;
        }      
    }

    if (g_td_parm.td_type[0] == '7')
    /* �������� */
    {
        TRACE
        sprintf(acErrMsg,"beg_date[%ld]end_date[%ld]!" , beg_date,end_date);
        WRITEMSG 
        sprintf(acErrMsg,"g_td_parm.intst_term_type[%s]g_td_parm.acm_calc_mode[%s]!" , g_td_parm.intst_term_type,g_td_parm.acm_calc_mode);
        WRITEMSG 
        sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf]g_td_mst.intst_type[%s]!" , g_pub_tx.tx_amt1,g_td_mst.intst_type);
        WRITEMSG   
        if (g_td_mst.intst_type[0] == '1')
        {
        	strcpy(g_td_mst.intst_type, "2");
        }   
        /* ֻ����һ�� */   
        if( pub_base_CalAcm(beg_date,g_td_mst.mtr_date,
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
            0.00,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            return 1;
        }
        /* ȡ����� */   
        if( pub_base_CalAcm(g_pub_tx.tx_date,g_td_mst.mtr_date,
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
            g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
        {
            sprintf(acErrMsg,"������Ϣ��������!");
            WRITEMSG
            return 1;
        }         
        
        sprintf(acErrMsg,"lance����[%lf]!" , g_td_mst.intst_acm);
        WRITEMSG        
    }   
    g_td_mst.hst_cnt=0;
    /** �����־��ֵ **/
    strcpy(g_td_mst.ac_sts,"1");
    strcpy(g_td_mst.hold_sts,"0");
    g_td_mst.hold_amt=0.00;
    g_td_mst.ctl_amt=0.00;
    strcpy(g_td_mst.tfr_ind,"N"); /**ԤԼת���־NEED**/
    g_td_mst.cif_no=g_pub_tx.cif_no;
    strcpy(g_td_mst.name,g_pub_tx.name);

    return 0;
}
int dummy_td_mst_hst(long beg_date,long end_date)
{
    int i=0;
    int num;
    int ret;
    int flag;
    long l_date;
    struct td_mst_hst_c td_mst_hst_c;
    memset(&td_mst_hst_c,0x00,sizeof(struct td_mst_hst_c));

    
    i = g_td_mst.tx_cnt;
    g_pub_tx.tx_date = beg_date;

    flag = 0;
while(g_td_mst.tx_cnt + 1 <= g_td_mst.ttl_cnt)
{ 
    strcpy(g_pub_tx.ct_ind,"1");
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
                sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                WRITEMSG
                return 1;
            }
            g_pub_tx.tx_date = l_date;
            if (end_date < l_date)
            {
                flag = 1; /* �˳�ѭ�� */
            }
            break;
        case 'D':
            num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
            ret = pub_base_deadlineD(g_td_mst.ic_date,num,
                  &l_date);
            if (ret != 0)
            {
                sprintf(acErrMsg,"������ȡ��ʱ�����!!");
                WRITEMSG
                return 1;
            }
            g_pub_tx.tx_date = l_date;
            if (end_date < l_date)
            {
                flag = 1; /* �˳�ѭ�� */
            }
            break;
        default:
            break;
    }

    switch(g_td_parm.td_type[0])
    {
        case '1':  /* �����ȡ */
            strcpy(g_pub_tx.add_ind,"1"); /* ��� */
            if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
                g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                return 1;
            }

            g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
            g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;
  
            g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
            g_td_mst.hst_cnt++;

            g_td_mst.tx_cnt++;
            g_td_mst.lst_date=g_pub_tx.tx_date;

            break;
        case '2':  /* ������ȡ */
            strcpy(g_pub_tx.add_ind,"0");
            if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
                g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
            {
                sprintf(acErrMsg,"������Ϣ��������!");
                WRITEMSG
                return 1;
            }
            g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
            g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;

            g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
            g_td_mst.hst_cnt++;

            g_td_mst.tx_cnt++;
            g_td_mst.lst_date=g_pub_tx.tx_date;

            break;
        case '3':  /* �汾ȡϢ */
            strcpy(g_pub_tx.add_ind,"0");
            g_td_mst.hst_cnt++;

            g_td_mst.tx_cnt++;
            g_td_mst.lst_date=g_pub_tx.tx_date;
            break;
        default:
            break;
    }   

    td_mst_hst_c.trace_no=g_pub_tx.trace_no;
    td_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(td_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(td_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
    td_mst_hst_c.ac_id=g_pub_tx.ac_id;
    td_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(td_mst_hst_c.tx_code,g_pub_tx.tx_code);
    strcpy(td_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);
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

    strcpy(td_mst_hst_c.note_type,g_pub_tx.note_type);
    strcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    strcpy(td_mst_hst_c.brf,g_pub_tx.brf);
    strcpy(td_mst_hst_c.tel,g_pub_tx.tel);
    strcpy(td_mst_hst_c.chk,g_pub_tx.chk);
    strcpy(td_mst_hst_c.auth,g_pub_tx.auth);

    td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
    ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"�ǼǶ�����ϸ�ʴ���[%d]!",ret);
        WRITEMSG
        return 1;
    }

    if (flag == 1)
    {
        break;
    }
}
    return 0;
}

int dummy_mo_infrm()
{
    int ret;
    struct mo_infrm_c mo_infrm_c;
    memset(&mo_infrm_c, 0x0, sizeof(struct mo_infrm_c));

    mo_infrm_c.ac_id = g_pub_tx.ac_id;
    mo_infrm_c.ac_seqn = g_pub_tx.ac_seqn;
    
    mo_infrm_c.infrm_date =  pub_base_daynumSUB(g_pub_tx.tx_date,
                          g_td_parm.dedr_intvl);
    mo_infrm_c.prm_draw_date = g_pub_tx.tx_date;
    mo_infrm_c.prm_draw_amt = g_pub_tx.tx_amt1;
    mo_infrm_c.fct_wd_amt = 0.00;
    mo_infrm_c.occ_date = 0;
    strcpy(mo_infrm_c.sts, "0");
    strcpy(mo_infrm_c.name,g_pub_tx.name);
    mo_infrm_c.wrk_date = mo_infrm_c.infrm_date;
    mo_infrm_c.trace_no = g_pub_tx.trace_no;
   
    ret = Mo_infrm_Ins(mo_infrm_c, g_pub_tx.reply);
    if (ret != 0)
    {
         sprintf(acErrMsg,"��֪ͨ�Ǽǲ��в����¼����!! [%d]",ret);
         WRITEMSG
         return ret;
    }
    return 0;
}
