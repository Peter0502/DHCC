/***************************************************************
* �� �� ��:     pubf_rept_glprdy.c
* ����������
*					********  ������������  *******
*				1.double f_get_bal_short(char * _prdt_no, int _d_flag);
*				2.char 	*f_get_current_bal_short(char *_prdt_no);
*				3.char 	*f_get_yesterday_bal_short(char *_prdt_no);
*				4.char 	*f_get_tenday_bal_short(char *_prdt_no);
*				5.char 	*f_get_month_bal_short(char *_prdt_no);
*				6.char 	*f_get_season_bal_short(char *_prdt_no);
*				7.char 	*f_get_year_bal_short(char *_prdt_no);
*				8.char  *f_get_current_d_amt_short(char *_prdt_no);
*				9.char  *f_get_current_c_amt_short(char *_prdt_no);
*				10.char *f_get_tenday_d_amt_short(char *_prdt_no);
*				11.char *f_get_tenday_c_amt_short(char *_prdt_no);
*				12.char *f_get_month_d_amt_short(char *_prdt_no);
*				13.char *f_get_month_c_amt_short(char *_prdt_no);
*				14.char *f_get_season_d_amt_short(char *_prdt_no);
*				15.char *f_get_season_c_amt_short(char *_prdt_no);
*				16.char *f_get_year_d_amt_short(char *_prdt_no);
*				17.char *f_get_year_c_amt_short(char *_prdt_no);
*
*				18.char *f_get_max_ln_bal();
*				19.char *f_get_maxsum_ln_bal();
* ��    ��:
* ������ڣ�
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:  xxx
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "pubf_rept_fun.h"
#include "gl_prdt_dyn_c.h"
#include "com_branch_c.h"

struct  gl_prdt_dyn_c       vgl_prdt_dyn;
int	ret=0;

/************************************************************
	ʵ�ֺ���  ���ݿ�Ŀȡ��ǰ(����)���
	input	:
				int	_prdt_no	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
*************************************************************/
char *f_get_current_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 0));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�Ŀȡ�������
	input	:
				int	_prdt_no	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_yesterday_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 1));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�ĿȡѮ�����
	input	:
				int	_prdt_no	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_tenday_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 2));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�³����
	input	:
				int		_prdt_no		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�³����
***/
char *f_get_month_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 3));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�������
	input	:
				int		_prdt_no		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�����
***/
char *f_get_season_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 4));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀȡ������
	input	:
				int	_prdt_no	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_year_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_dc[3];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 5));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*
	(��������)���ݲ�Ʒ��ȡ�������ڳ����
	���ݻ����š���Ŀ�ż���跽���
	input	:
				char _prdt_no[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����
				char _dc_flag	��������־
							 1  ��
							 2  ��
	output	:
				char	l_s_amt	������
****/
double f_get_bal_short(char * _prdt_no, int	_d_flag)
{
	char	l_prdt_no[10];
	double	l_bal = 0.00;

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));

	strncpy(l_prdt_no, _prdt_no,3);

	/* ҵ����� */
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and \
						prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		switch(_d_flag)
		{
			case 0:				/* ��ǰ��� (����) */
				l_bal+=vgl_prdt_dyn.cr_bal;
				break;
			case 1:				/* ���� */
				l_bal+=vgl_prdt_dyn.lcd_bal;
				break;
			case 2:				/* ��Ѯ	*/
				l_bal+=vgl_prdt_dyn.tdcr_bal;
				break;
			case 3:				/* ����	*/
				l_bal+=vgl_prdt_dyn.m_cr_bal;
				break;
			case 4:				/* ����	*/
				l_bal+=vgl_prdt_dyn.q_cr_bal;
				break;
			case 5:				/* ����	*/
				l_bal+=vgl_prdt_dyn.y_cr_bal;
				break;
			default:
				break;
		}
	}

	Gl_prdt_dyn_Clo_Sel();

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/***********************************************************************/
/*** 
	ȡ�跽������   ����
***/
char	*f_get_current_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and \
		prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.rdd_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	ȡ����������   ����
***/
char	*f_get_current_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.rcd_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	ȡ�跽������   ��Ѯ
***/
char	*f_get_tenday_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.td_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	ȡ����������   ��Ѯ
***/
char	*f_get_tenday_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.td_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	ȡ�跽������   ����
***/
char	*f_get_month_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.m_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_month_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.m_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_season_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no,_prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.q_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_season_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.q_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_year_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.y_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_year_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no,_prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' \
		and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.y_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*******************************************************************/
/*** ���һ�ʴ������  ***/
char *f_get_max_ln_bal()
{
	double	l_bal;
	long	l_date_begin;
	long	l_date_end;
	char	l_s_bal[256];

	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	l_date_end = g_rept_date;
	l_date_begin = g_rept_date/100 + 1;

	ret=sql_max_double("ln_mst","bal",&l_bal,"1=1");
	if(ret)
	{
		goto ErrExit;
	}

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}

/*** ���ݲ�Ʒ��ȡ�������� ***/
char *f_get_maxsum_ln_bal()
{
	double	l_bal;
	char	l_s_bal[256];

	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	/*
	sprintf(l_sql, "select sum(bal) a1 from ln_mst where acchrt in (select acchrt from com_acc where curno[1]='7') group by acid order by a1 desc");

	EXEC SQL prepare qry_sumbal from :l_sql;
	MYSQLERR

	EXEC SQL declare sumbal_cur cursor for qry_sumbal;
	MYSQLERR

	EXEC SQL open sumbal_cur;
	MYSQLERR

	EXEC SQL fetch sumbal_cur into :l_bal :a2;
	MYSQLERR

	if(a2) l_bal = 0.00;

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}*/

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}
