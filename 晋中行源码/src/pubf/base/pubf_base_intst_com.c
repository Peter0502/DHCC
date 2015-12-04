#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_intst_com.c(������Ϣ�Ĺ��ú���)
* ����������    ��Ϣ����Ĺ��ú���
* ���ú���˵����
*
*
* ��    ��:     rob
* ������ڣ�    2003��01��05��
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
#include "com_rate_c.h"
#include "td_mst_hst_c.h"
#include "com_holiday_c.h"

/**********************************************************************
* ��������    pub_base_getllz()
* �������ܣ�  �������ʱ�� ���� ����,ȡ��Ӧ������ֵ
* ����/ʱ�䣺 2003��01��05��
*
* ������
*     ���룺 ���ʱ�� rate_no    char(3)
*            ����     cur_no     char(2)
*            ����     date       long
*     ���:
*            ����ֵ(��)   rate_val   double 
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
struct com_rate_c Gl_com_rate_c;

int pub_base_getllz(char *rate_no, char *cur_no, long date, double *rate_val)
{
    struct com_rate_c com_rate_c;
    int ret;

        sprintf(acErrMsg, " [%d][%s][%s]",
			date,cur_no,rate_no );
        WRITEMSG
    
    MEMSET_DEBUG(&com_rate_c, 0x0, sizeof(struct com_rate_c));

	if( !strncmp(Gl_com_rate_c.cur_no,cur_no,2) &&
		!strcmp(Gl_com_rate_c.rate_code,rate_no) &&
		Gl_com_rate_c.beg_date<=date && Gl_com_rate_c.end_date>date )
	{
		ret=0;
		MEMCPY_DEBUG( &com_rate_c, &Gl_com_rate_c, sizeof(com_rate_c) );
	}
	else
    	ret = Com_rate_Sel(g_pub_tx.reply, &com_rate_c, "cur_no = '%s' and rate_code  = '%s' and beg_date <= %ld and end_date > %ld ", cur_no, rate_no, date, date);
    if (ret == 100)
    {
        sprintf(acErrMsg, " NOTFOUND record in com_rate[%d][%s][%s] ",
			date,cur_no,rate_no );
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
		MEMCPY_DEBUG( &Gl_com_rate_c, &com_rate_c, sizeof(com_rate_c) );

    (*rate_val) = com_rate_c.rate_val * L360 / com_rate_c.rate_type ;
    (*rate_val) = (*rate_val) * L100 / com_rate_c.rate_unit;

	sprintf(acErrMsg, "����ֵΪ: [%lf]", *rate_val);
	WRITEMSG
   
    return 0;
}


/**********************************************************************
* ��������    pub_base_getsecrate()
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

int pub_base_getsecrate(char *rate_no, char *cur_no, long ic_date,
        long tx_date, char intst_term_type[2], int mode, double *rate_val)
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

    Com_rate_Clo_Sel();

    return 0;
}



/**********************************************************************
* ��������    pub_base_intstJDQX()
* �������ܣ�  �������ʱ�� ���� ��Ϣ�� ������,ȷ����Ӧ�����ʱ��
* ����/ʱ�䣺 2003��01��05��
*
* ������
*     ���룺 ���ʱ�� rate_no    char *
*            ����     cur_no     char *
*            ��Ϣ���� ic_date    long
*            �������� tx_date    long
*            ����������־ bz     int
*     ���:
*            ���ʱ�� rate_no1   char *
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_intstJDQX(char *rate_no, char *cur_no, long ic_date,
            long tx_date, char intst_term_type[2])
{
    long dayterm;
    int ret;
    char rateno[4];

    ret=pub_base_CalIntstDays(ic_date, tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"������������!!");
        WRITEMSG
        return 1;
    }
   
sprintf(acErrMsg," dayterm = [%ld]",dayterm);
WRITEMSG
    
    if (dayterm >= 24 * 30)
    {
        strcpy(rateno,ZCZQ24);    
    }
    else if (dayterm >= 12 * 30)
    {
        strcpy(rateno,ZCZQ12);
    }
    else if (dayterm >= 6 * 30)
    {
        strcpy(rateno,ZCZQ06);
    }
    else if (dayterm >= 3 * 30)
    {
        strcpy(rateno,ZCZQ03);
    }
    else 
    {
        strcpy(rateno,HQLLBH);
    }
    
    MEMSET_DEBUG(rate_no,0x0,sizeof(rate_no));
    strcpy(rate_no, rateno);
    sprintf(acErrMsg,"rateno=[%s],[%s]",rate_no,rateno);
    WRITEMSG
    return 0;
}

/**********************************************************************
* ��������    pub_base_gettdmsthst()
* ��������:�����ʻ�id����Ų�ѯ���ڴ����ϸ�������ں���˻������ͻ���
* ����/ʱ�䣺 2003��01��05��
*
* ������
*     ���룺 �ʻ�ID   ac_id      long 
*            �ʻ���� ac_seqn    int
*            ����     jyrq       long *
*     ���:
*            ����     js99       double *
*            ���     ye99       double *
*            ��Ӧ��   reply      char  *
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/

int pub_base_gettdmsthst(long ac_id, int ac_seqn, long *jyrq,
            double *js99, double *ye99, char *reply)
{
    int ret;
    struct td_mst_hst_c td_mst_hst_c,td_mst_hst_c1;
 
sprintf(acErrMsg, "prepare error code [%d] ", ret);
WRITEMSG

    ret = Td_mst_hst_Dec_Sel(reply, " ac_id = %ld and ac_seqn = %d \
          and tx_date < %ld order by tx_date,trace_no", ac_id, ac_seqn, jyrq);
    if (ret != 0)
    {
        sprintf(acErrMsg, "prepare error code [%d] ", ret);
        WRITEMSG
        /* strcpy(reply, "D101"); */
        return ret;

    }
    MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
     
    while (1)
    {
        ret = Td_mst_hst_Fet_Sel(&td_mst_hst_c1, reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
            WRITEMSG
            /* strcpy(g_pub_tx.reply, "D103"); */
            Td_mst_hst_Clo_Sel();
            return ret;
   
        }
        else if (ret == 100)
        {
            break;
        }
        MEMSET_DEBUG(&td_mst_hst_c,0x0,sizeof(struct td_mst_hst_c));
        MEMCPY_DEBUG(&td_mst_hst_c,&td_mst_hst_c1,sizeof(struct td_mst_hst_c));
    }
    
    Td_mst_hst_Clo_Sel();
    *jyrq = td_mst_hst_c.tx_date ;
    *js99 = td_mst_hst_c.intst_acm;
    *ye99 = td_mst_hst_c.bal; 
    if (jyrq == 0)
    {
        sprintf(acErrMsg, "fetch com_rate error [%d] ", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D104"); 
        return 100;
    }
    return 0;
}

/**********************************************************************
* ��������    pub_base_tuneday()
* �������ܣ�  �ж����⴦������(������֧��������)
* ����/ʱ�䣺 2003��01��05��
*
* ������
*     ���룺 ��������    tx_date  long
*            ��Ϣ����    ic_date  long
*            ��������    mtr_date long 
*     ���:
* �� �� ֵ:  ��������
*
* �޸���ʷ��
*
********************************************************************/


int pub_base_tuneday(long pubjx_rq, long pubjx_khrq, long pubjx_dqrq)
{
    short t_ts=0,ymd1[3],ymd2[3],ymd3[3];

	ymd1[0]=(short)(pubjx_khrq/100%100);
	ymd1[1]=(short)(pubjx_khrq%100);
	ymd1[2]=(short)(pubjx_khrq/10000);  
	
	ymd2[0]=(short)(pubjx_rq/100%100);
	ymd2[1]=(short)(pubjx_rq%100);
	ymd2[2]=(short)(pubjx_rq/10000);  

	ymd3[0]=(short)(pubjx_dqrq/100%100);
	ymd3[1]=(short)(pubjx_dqrq%100);
	ymd3[2]=(short)(pubjx_dqrq/10000);  

TRACE;
	if (ymd1[0]==2 && ymd1[1]==29 && ymd3[0]==2 && ymd3[1]==28 && 
		pubjx_rq!=pubjx_khrq && pubjx_rq!=pubjx_dqrq)
	 	return 1;
	else
		return 0;
}

long PubMax (long rq1, long rq2)
{
	if (rq1>rq2)
		return (rq1);
	else
		return (rq2);
}

long PubMin (long rq1, long rq2)
{
	if (rq1>rq2)
		return (rq2);
	else
		return (rq1);
}


/**********************************************************************
* ��������    pub_base_checktxdate()
* �������ܣ�  �ж��Ƿ�����ǰ֧ȡ
* ����/ʱ�䣺 2003��03��02��
*
* ������
*     ���룺 
*     ���:
* �� �� ֵ:  100 ��ǰ֧ȡ 0 ������ǰ֧ȡ ���� ����
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_checktxdate(long tx_date,long mtr_date)
{
    long nextdate;
    long date;
    int ret;
    struct com_holiday_c com_holiday;

    date = tx_date;
    if (tx_date >= mtr_date)
    {
        return 0;
    }

    ret = pub_base_end_month(date);
    if (ret == 0)
    {
        ret = pub_base_end_year(date);
        if (ret == 0)
        {
            nextdate = (date / 10000 + 1) * 10000 + 101;
        }
        else
        {
            nextdate = (date / 100 +1 ) * 100 + 1;
        }
    }
    else
    {
        nextdate = date + 1;
    }
    ret = pub_base_date_jjr(nextdate, &com_holiday);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ȡ�ڼ����ļ����󣡣���");
        WRITEMSG
        return ret;
    } 
    /* �жϵ������Ƿ��ڴ˽ڼ����� */
    if (mtr_date < com_holiday.beg_date || mtr_date > com_holiday.end_date)
    {
        return 100;
    }
    return 0;
}


/**********************************************************************
* ��������    pub_base_getfltrate()
* �������ܣ�  ���ݲ�Ʒ�ż�����ò�Ʒ����������
* ����/ʱ�䣺 2003��05��06��
*
* ������
*     ���룺 prdt_no char(3) ��Ʒ��
*     ���:
*			 min_rate double �������
*			 max_rate double �������
* �� �� ֵ:  0 �ɹ�
*			!0 ���ɹ�
* �޸���ʷ��
*
********************************************************************/
int pub_base_getfltrate(char prdt_no[4],double *min_rate,double *max_rate)
{
	char flag;
	int ret;
	double rate_val;
	struct in_parm_c in_parm_c;

	MEMSET_DEBUG(&in_parm_c, 0x0, sizeof(struct in_parm_c));

	flag = prdt_no[0];
	switch(flag)
	{
		case '1':
			break;
		case '2':
			break;
		case '3':
			break;
		case '4':
			break;
		case '9':
			ret = In_parm_Sel(g_pub_tx.reply,&in_parm_c,"prdt_no='%s' \
				and star_date <= %ld and stop_date > %ld", prdt_no, 
				g_pub_tx.tx_date, g_pub_tx.tx_date);
			if (ret != 0)
			{
				sprintf(acErrMsg,"��ѯ�ڲ���Ʒ�����!! [%d]",ret);
				WRITEMSG
				return ret;
			}

			ret = pub_base_getllz(in_parm_c.rate_no,"01",g_pub_tx.tx_date,
					&rate_val);
			if (ret != 0)
			{
				sprintf(acErrMsg,"ȡ����ֵ����!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"W110");
				return ret;
			}
			break;
		default:
			break;
	}
	return 0;
}

/**********************************************************************
* ��������    pub_base_getrateval()
* �������ܣ�  ������Ϣ�պͽ����ռ��㿿������
* ����/ʱ�䣺 2003��05��06��
*
* ������
*     ���룺 ic_date  long	 ��Ϣ��
*			 tx_date  long	 ������
*			 cur_no[2] char   ����
*     ���:
*			 rate_val  double ����ֵ
* �� �� ֵ:  0 �ɹ�
*			!0 ���ɹ�
* �޸���ʷ��
*
********************************************************************/
int pub_base_getrateval(long ic_date, long tx_date, char cur_no[3], 
	double *rate_val)
{
	int ret=0;
	long tmp_date12=0, tmp_date06=0, tmp_date03;

	char llbh[5];

	MEMSET_DEBUG(llbh, 0x0, sizeof(llbh));

	ret = pub_base_deadlineM(ic_date, 12, &tmp_date12);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����12���º�����ڴ���!!");
		WRITEMSG
		return ret;
	}

	ret = pub_base_deadlineM(ic_date, 6, &tmp_date06);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����06���º�����ڴ���!!");
		WRITEMSG
		return ret;
	}

	ret = pub_base_deadlineM(ic_date, 3, &tmp_date03);
	if (ret != 0)
	{
		sprintf(acErrMsg, "����03���º�����ڴ���!!");
		WRITEMSG
		return ret;
	}

	if (tx_date >= tmp_date12)
	{
		/* ����ֵΪ����һ�� */
		strcpy(llbh, ZCZQ12);
	}
	else if (tx_date >= tmp_date06)
	{
		/* ����ֵΪ�������� */
		strcpy(llbh, ZCZQ06);
	}
	else if (tx_date >= tmp_date03)
	{
		/* ����ֵΪ���������� */
		strcpy(llbh, ZCZQ03);
	}
	else 
	{
		/* ���� */
		strcpy(llbh, HQLLBH);
	}

	*rate_val=0.00;
	sprintf(acErrMsg, "���ʱ��Ϊ:[%s]", llbh);
	WRITEMSG
	ret = pub_base_getllz(llbh,cur_no,tx_date,rate_val);
	if (ret != 0)
	{ 
		sprintf(acErrMsg,"��com_rateȡ����ֵ���� [%d]",ret); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"W110"); 
		return ret;
	}

	return 0;
}
