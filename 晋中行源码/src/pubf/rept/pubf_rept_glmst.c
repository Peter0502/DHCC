/***************************************************************
* �� �� ��:  pubf_rept_glmst.c
* ����������
*					********  ������������  *******
*			1.char 	*f_prt_null();
*			2.char 	*f_prt_line();
*			3.char	*f_get_brno();				* ȡ�������� 	*
*			4.char 	*f_get_year();				* ȡϵͳ���� ��*
*			5.char	*f_get_month();				* ȡϵͳ���� ��*
*			6.char	*f_get_day();				* ȡϵͳ���� ��*
*			7.char 	*f_get_season();			* ȡϵͳ���� ��*
*
*					********    ���㺯��   ********
*			8.double 	f_get_bal(char * _acchrt, int _d_flag, char * _dc_flag);
*			9.double 	f_get_bal_d(char * _acchrt, int	_d_flag);
*					*******************************
*			10.char 	*f_get_current_bal(char *_acchrt);
*			11.char 	*f_get_current_wi_bal(char *_acchrt);
*
*			12.char 	*f_get_yesterday_bal(char *_acchrt);
*			13.char 	*f_get_befyesterday_bal(char *_acchrt);
*
*			14.char 	*f_get_befmonday_bal(char *_acchrt);
*			15.char 	*f_get_yesterday_wi_bal(char *_acchrt);
*
*			16.char 	*f_get_tenday_bal(char *_acchrt);
*			17.char 	*f_get_tenday_wi_bal(char *_acchrt);
*
*			18.char 	*f_get_month_bal(char *_acchrt);
*			19.char 	*f_get_month_wi_bal(char *_acchrt);
*
*			20.char 	*f_get_season_bal(char *_acchrt);
*			21.char 	*f_get_season_wi_bal(char *_acchrt);
*
*			22.char 	*f_get_year_bal(char *_acchrt);
*			23.char 	*f_get_year_wi_bal(char *_acchrt);
*
*			24.char    *f_get_current_d_amt(char *_acchrt);
*			25.char    *f_get_current_d_amt_wi(char *_acchrt);
*
*			26.char    *f_get_current_c_amt(char *_acchrt);
*			27.char    *f_get_current_c_amt_wi(char *_acchrt);
*
*			28.char    *f_get_tenday_d_amt(char *_acchrt);
*			29.char    *f_get_tenday_d_amt_wi(char *_acchrt);
*
*			30.char    *f_get_tenday_c_amt(char *_acchrt);
*			31.char    *f_get_tenday_c_amt_wi(char *_acchrt);
*
*			32.char    *f_get_month_d_amt(char *_acchrt);
*			33.char    *f_get_month_d_amt_wi(char *_acchrt);
*
*			34.char    *f_get_month_c_amt(char *_acchrt);
*			35.char    *f_get_month_c_amt_wi(char *_acchrt);
*
*			36.char    *f_get_season_d_amt(char *_acchrt);
*			37.char    *f_get_season_d_amt_wi(char *_acchrt);
*
*			38.char    *f_get_season_c_amt(char *_acchrt);
*			39.char    *f_get_season_c_amt_wi(char *_acchrt);
*
*			40.char    *f_get_year_d_amt(char *_acchrt);
*			41.char    *f_get_year_d_amt_wi(char *_acchrt);
*
*			42.char    *f_get_year_c_amt(char *_acchrt);
*			43.char    *f_get_year_c_amt_wi(char *_acchrt);
*
*			44.char    *f_get_today_d_cnt(char *_acchrt);
*			45.char    *f_get_today_c_cnt(char *_acchrt);
*
*			46.char    *f_get_tenday_d_cnt(char *_acchrt);
*			47.char    *f_get_tenday_c_cnt(char *_acchrt);
*
*			48.char    *f_get_month_d_cnt(char *_acchrt);
*			49.char    *f_get_month_c_cnt(char *_acchrt);
*
*			50.char    *f_get_season_d_cnt(char *_acchrt);
*			51.char    *f_get_season_c_cnt(char *_acchrt);
*
*			52.char    *f_get_year_d_cnt(char *_acchrt);
*			53.char    *f_get_year_c_cnt(char *_acchrt);
*
*				******** ������---ȡȥ��ͬ����� ********
*			54.double f_get_l_bal(char * _acchrt,int  _d_flag, char * _dc_flag);
*			55.double f_get_l_bal_d(char * _acchrt, int	_d_flag);
*			56.char *f_get_l_current_bal(char *_acchrt);
*			57.char *f_get_l_yesterday_bal(char *_acchrt);
*			58.char *f_get_l_tenday_bal(char *_acchrt);
*			59.char *f_get_l_month_bal(char *_acchrt);
*			60.char *f_get_l_season_bal(char *_acchrt);
*			61.char *f_get_l_year_bal(char *_acchrt);
*
*			62.double f_get_d_bal(char * _acchrt, int	_d_flag);
*			63.double f_get_c_bal(char * _acchrt, int	_d_flag);
*			64.char    *f_get_cc_bal(char  *_acchrt);
*			65.char    *f_get_lc_bal(char  *_acchrt);
*			66.char    *f_get_tc_bal(char  *_acchrt);
*			67.char    *f_get_mc_bal(char  *_acchrt);
*			68.char    *f_get_sc_bal(char  *_acchrt);
*			69.char    *f_get_yc_bal(char  *_acchrt);
*			70.char    *f_get_cd_bal(char  *_acchrt);
*			71.char    *f_get_ld_bal(char  *_acchrt);
*			72.char    *f_get_td_bal(char  *_acchrt);
*			73.char    *f_get_md_bal(char  *_acchrt);
*			74.char    *f_get_sd_bal(char  *_acchrt);
*			75.char    *f_get_yd_bal(char  *_acchrt);
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
#include "com_branch_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "gl_mst_hst_c.h"
#include "gl_tsub_c.h"

struct  com_branch_c 	vcom_branch;
struct  com_item_c 		vcom_item;
struct  gl_mst_c 		vgl_mst;
struct  gl_mst_hst_c 	vgl_mst_hst;
struct  gl_tsub_c 		vgl_tsub;

static	int	ret=0;

/***
	ȡ�������� 
***/
char	*f_get_brno()
{
	char	l_name[20];

	memset(l_name, 0x00, sizeof(l_name));
	memset(&vcom_branch, 0x00, sizeof(struct com_branch_c));

	ret=Com_branch_Sel(g_pub_tx.reply,&vcom_branch, \
					  "br_no='%s'",g_brno_tmp.br_no);
	if( 100 == ret )	
	{
		strcpy(l_name," ");
	}

	strcpy(l_name,vcom_branch.br_name);
	
	return l_name;
ErrExit:
	strcpy(l_name, "");
	return l_name;
}
char *f_get_year()
{
	char	l_return[256];
	sprintf(l_return, "%04d", g_rept_date/100/100);
	return l_return;
}
char *f_get_month()
{
	char	l_return[256];
	sprintf(l_return, "%02d", g_rept_date/100%100);
	return l_return;
}
char *f_get_day()
{
	char	l_return[256];
	sprintf(l_return, "%02d",g_rept_date%100);
	return l_return;
}
char *f_get_season()
{
	char	l_return[256];
	int		l_month;

	l_month = g_rept_date/100%100;

	if( l_month>=1 && l_month <=3 )
		strcpy( l_return, "һ");
	else if( l_month>=4 && l_month <=6 )
		strcpy( l_return, "��");
	else if( l_month>=6 && l_month <=9 )
		strcpy( l_return, "��");
	else if( l_month>=10 && l_month <=12 )
		strcpy( l_return, "��");
	else
		strcpy( l_return, "  ");
	return l_return;
}
/***  
	��ӡ�ո�
***/
char *f_prt_null()
{
	char	l_return[256];

	memset(l_return, 0x00, sizeof(l_return));

	strcpy(l_return, "  ");
	return l_return;
}

/***  
	��ӡ "����"
***/
char *f_prt_line()
{
	char	l_return[256];

	memset(l_return, 0x00, sizeof(l_return));

	strcpy(l_return, "       ��");
	return l_return;
}

/***
	ȡ������Ԫ
***/
char *f_get_current_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal,0x00,sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal,f_get_current_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀȡ��ǰ(����)���
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������,���
***/
char *f_get_current_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 0));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 0, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ȡ������Ԫ
***/
char *f_get_yesterday_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_yesterday_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/**********************************************************
	ʵ�ֺ���  ���ݿ�Ŀȡǰ�����
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***********************************************************/
char *f_get_befmonday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	long 	qtrq,ycrq;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(&vgl_tsub, 0x00, sizeof(struct gl_tsub_c));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	ycrq=ycrq/100*100+1;
	pub_base_deadlineD(ycrq,-1,&qtrq);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	ret=Gl_tsub_Sel(g_pub_tx.reply,&vgl_tsub,"date=%ld and term_type='5' and \
					br_no='%s' and cur_no='01' and acc_hrt='%s'", \
					qtrq,g_brno_tmp.br_no,l_acchrt);
	if(ret==100)
	{
		vgl_tsub.bt_dr_bal=vgl_tsub.bt_cr_bal=0.00;
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0' || vcom_item.dc_ind[0]=='1' )
	{
		sprintf(l_s_bal,"%f", vgl_tsub.bt_dr_bal);
	}
	else
	{
		sprintf(l_s_bal,"%f", vgl_tsub.bt_cr_bal);
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char *f_get_befyesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	long qtrq;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(&vgl_mst_hst, 0x00, sizeof(struct gl_mst_hst_c));

	strcpy(l_acchrt, _acchrt);
	pub_base_deadlineD(g_rept_date,-2,&qtrq);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	ret=Gl_mst_hst_Sel(g_pub_tx.reply,&vgl_mst_hst,"date=%ld and term_type='5' \
					and br_no='%s' and cur_no='01' and acc_hrt='%s'", \
					qtrq,g_brno_tmp.br_no,l_acchrt);
	if(ret)
	{
		vgl_mst_hst.dr_bal=vgl_mst_hst.cr_bal=0.00;
	}

	if( vcom_item.dc_ind[0] == '0' || vcom_item.dc_ind[0]=='1' )
	{
		sprintf(l_s_bal,"%f", vgl_mst_hst.dr_bal);
	}
	else
	{
		sprintf(l_s_bal,"%f", vgl_mst_hst.cr_bal);
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char *f_get_yesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 1));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 1, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ȡ������Ԫ
***/
char *f_get_tenday_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_tenday_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	ʵ�ֺ���  ���ݿ�ĿȡѮ�����
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_tenday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 2));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 2, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ȡ������Ԫ
***/
char *f_get_month_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_month_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�³����
	input	:
				int		_acchrt		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�³����
***/
char *f_get_month_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 3));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 3, vcom_item.dc_ind));
	}
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ȡ������Ԫ
***/
char *f_get_season_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�������
	input	:
				int		_acchrt		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�����
***/
char *f_get_season_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 4));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 4, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ȡ������Ԫ
***/
char *f_get_year_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀȡ������
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_year_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 5));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 5, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/**********************************************************
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	��Ŀ��Ϊ���Կ�Ŀ
	���ݻ����š���Ŀ�ż���跽���
	input	:
				char _acchrt[8]	��Ŀ��
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
**********************************************************/
double f_get_bal(char * _acchrt, int	_d_flag, char * _dc_flag)
{
	char	l_acchrt[8];
	char	l_tmp[256];
	double	l_bal = 0.00;
	int		_o_flag;

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_tmp, "%c", _dc_flag[0]);
	_o_flag = atoi(l_tmp);

	/* ҵ����� */
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and cur_no='%s' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no,D_CURNO,l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	switch(_d_flag)
	{
		case 0:				/*  ��ǰ���  (����) */
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.dr_bal;
			}else 									/*  ����  */
			{
				l_bal=vgl_mst.cr_bal;
			}
			break;
		case 1:				/* ����     */
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.ldd_bal;
			}else									/*  ����  */
			{
				l_bal=vgl_mst.lcd_bal;
			}
			break;
		case 2:				/* ��Ѯ		*/
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.tddr_bal;
			}else									/*  ����  */
			{
				l_bal=vgl_mst.tdcr_bal;
			}
			break;
		case 3:				/* ����		*/
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.mdr_bal;
			}else									/*  ����  */
			{
				l_bal=vgl_mst.mcr_bal;
			}
			break;
		case 4:				/* ����		*/
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.qdr_bal;
			}else									/*  ����  */
			{
				l_bal=vgl_mst.qcr_bal;
			}
			break;
		case 5:				/* ����		*/
			if( _o_flag == 1 ){						/*  �跽  */
				l_bal=vgl_mst.ydr_bal;
			}else									/*  ����  */
			{
				l_bal=vgl_mst.ycr_bal;
			}
			break;
		default:
			break;
	}

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/*******************************************************
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	��Ŀ��Ϊ˫�Կ�Ŀ
	���ݻ����š���Ŀ�ż���跽���
	input	:
				char _acchrt[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����
	output	:
				char	l_s_amt	������
*******************************************************/
double f_get_bal_d(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
		cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no,l_acchrt);
	if(ret==100)
	{
		 goto ErrExit;
	}

	/* ҵ����� */
	switch(_d_flag)
	{
		case 0:				/*  ��ǰ���  (����) */
			if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
			{
				l_bal=vgl_mst.dr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
			{
				l_bal=vgl_mst.cr_bal;
			}
			else l_bal = 0.00;
			break;
		case 1:				/* ���� */
			if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
			{
				l_bal=vgl_mst.ldd_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
			{
				l_bal=vgl_mst.lcd_bal;
			}
			else l_bal = 0.00;
			break;
		case 2:				/* ��Ѯ		*/
			if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
			{
				l_bal=vgl_mst.tddr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
			{
				l_bal=vgl_mst.tdcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 3:				/* ����	*/
			if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
			{
				l_bal=vgl_mst.mdr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
			{
				l_bal=vgl_mst.mcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 4:				/* ����	*/
			if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
			{
				l_bal=vgl_mst.qdr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
			{
				l_bal=vgl_mst.qcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 5:				/* ����	*/
			if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
			{
				l_bal=vgl_mst.ydr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
			{
				l_bal=vgl_mst.ycr_bal;
			}
			else l_bal = 0.00;
			break;
		default:
			break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}


/***
	ȡ������Ԫ
***/
char *f_get_current_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_current_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_current_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.rdd_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/***
	ȡ������Ԫ
***/
char *f_get_current_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_current_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_current_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.rcd_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/***
	ȡ������Ԫ
***/
char *f_get_tenday_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_tenday_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ�跽������   ��Ѯ
***/
char	*f_get_tenday_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.tddr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/***
	ȡ������Ԫ
***/
char *f_get_tenday_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_tenday_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ����������   ��Ѯ
***/
char	*f_get_tenday_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.tdcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/***
	ȡ������Ԫ
***/
char *f_get_month_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_month_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_month_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.mdr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	ȡ������Ԫ
***/
char *f_get_month_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_month_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_month_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.mcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	ȡ������Ԫ
***/
char *f_get_season_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_d_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_season_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.qdr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	ȡ������Ԫ
***/
char *f_get_season_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_c_amt(_acchrt));
	printf("l_s_wi_bal=[%s]\n",l_s_wi_bal);
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_season_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.qcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	ȡ������Ԫ
***/
char *f_get_year_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_d_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	ȡ�跽������   ����
***/
char	*f_get_year_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.ydr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	ȡ������Ԫ
***/
char *f_get_year_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_c_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	ȡ����������   ����
***/
char	*f_get_year_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.ycr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	ȡ��ǰ���໧��-----------��ɾ�����ֶ�
***/

/*** 
	ȡ���ս����
***/
char	*f_get_today_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.rdd_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ���մ�����
***/
char	*f_get_today_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.rcd_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ��Ѯ�����
***/
char	*f_get_tenday_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.tddr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ��Ѯ������
***/
char	*f_get_tenday_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.tdcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ���½����
***/
char	*f_get_month_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.mdr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ���´�����
***/
char	*f_get_month_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.mcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ���������
***/
char	*f_get_season_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.qdr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	ȡ����������
***/
char	*f_get_season_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.qcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	ȡ��������
***/
char	*f_get_year_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.ydr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	ȡ���������
***/
char	*f_get_year_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.ycr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	ȡ���տ�����	-----��ɾ��
***/

/*** 
	ȡ��Ѯ������	-----��ɾ��
***/

/*** 
	ȡ���¿�����	-----��ɾ��
***/

/*** 
	ȡ����������	-----��ɾ��
***/

/*** 
	ȡ���꿪����	-----��ɾ��
***/

/*** 
	ȡ����������
***/

/*** 
	ȡ��Ѯ������
***/

/*** 
	ȡ����������
***/

/*** 
	ȡ����������
***/

/*** 
	ȡ����������
***/

/*** 
	ȡ���ս��໧��
***/

/*** 
	ȡ��Ѯ�����໧��
***/

/*** 
	ȡ���³����໧��
***/

/*** 
	ȡ���������໧��
***/

/*** 
	ȡ��������໧��
***/

/*******************************************************************/
/*  get the same date of last year */

/***
	ʵ�ֺ���  ���ݿ�Ŀȡ��ǰ(����)���
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_l_current_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 0));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 0, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�Ŀȡ�������
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_l_yesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 1));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 1, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�ĿȡѮ�����
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_l_tenday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 2));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 2, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�³����
	input	:
				int		_acchrt		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�³����
***/
char *f_get_l_month_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 3));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 3, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀ��ȡ�������
	input	:
				int		_acchrt		��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal		�����
***/
char *f_get_l_season_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 4));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 4, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	ʵ�ֺ���  ���ݿ�Ŀȡ������
	input	:
				int	_acchrt	��Ŀ��(�����ͣ�Ŀ���ǵ��÷���)
	output	:
				char	l_s_bal	������/���
***/
char *f_get_l_year_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 5));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 5, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	��Ŀ��Ϊ���Կ�Ŀ
	���ݻ����š���Ŀ�ż���跽���
	input	:
				char _acchrt[8]	��Ŀ��
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
*/
double f_get_l_bal(char * _acchrt, int	_d_flag, char * _dc_flag)
{
	char	l_acchrt[8];
	char	l_tmp[256];
	long	l_date;
	double	l_bal = 0.00;
	int		_o_flag;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_tmp, "%c", _dc_flag[0]);
	_o_flag = atoi(l_tmp);

	l_date = (g_rept_date/10000-1)*10000+g_rept_date%10000;

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='%s' and acc_hrt='%s'",l_date,g_brno_tmp.br_no, \
					D_CURNO, l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	switch(_d_flag)
	{
		case 0:				/*��ǰ���(����)*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.dr_bal;
			else 									/*  ����  */
				l_bal=vgl_mst.cr_bal;
			break;
		case 1:				/*����*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.ldd_bal;
			else									/*  ����  */
				l_bal=vgl_mst.lcd_bal;
			break;
		case 2:				/*��Ѯ*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.tddr_bal;
			else									/*  ����  */
				l_bal=vgl_mst.tdcr_bal;
			break;
		case 3:				/*����*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.mdr_bal;
			else									/*  ����  */
				l_bal=vgl_mst.mcr_bal;
			break;
		case 4:				/*����*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.qdr_bal;
			else									/*  ����  */
				l_bal=vgl_mst.qcr_bal;
			break;
		case 5:				/* ����		*/
			if( _o_flag == 1 )						/*  �跽  */
				l_bal=vgl_mst.ydr_bal;
			else									/*  ����  */
				l_bal=vgl_mst.ycr_bal;
			break;
		default:
			break;
	}

	/* ҵ����� */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/*****************************************************
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	��Ŀ��Ϊ˫�Կ�Ŀ
	���ݻ����š���Ŀ�ż���跽���
	input	:
				char _acchrt[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����
	output	:
				char	l_s_amt	������
******************************************************/
double f_get_l_bal_d(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	char	l_field[256];
	double	l_bal = 0.00;
	long	l_date;
	int		a1;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));

	l_date = (g_rept_date/10000-1)*10000+g_rept_date%10000;

	strcpy(l_acchrt, _acchrt);

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='%s' and acc_hrt='%s'",l_date,g_brno_tmp.br_no, \
					D_CURNO, l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	/* ҵ����� */
	switch(_d_flag)
	{
		case 0:				/*  ��ǰ���  (����) */
				if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
				{
					l_bal=vgl_mst.dr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
				{
					l_bal=vgl_mst.cr_bal;
				}
				else l_bal=0.00;
				break;
		case 1:				/* ���� */
				if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
				{
					l_bal=vgl_mst.ldd_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
				{
					l_bal=vgl_mst.lcd_bal;
				}
				else l_bal=0.00;
				break;
		case 2:				/* ��Ѯ	*/
				if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
				{
					l_bal=vgl_mst.tddr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
				{
					l_bal=vgl_mst.tdcr_bal;
				}
				else l_bal=0.00;
				break;
		case 3:				/* ����	*/
				if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
				{
					l_bal=vgl_mst.mdr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
				{
					l_bal=vgl_mst.mcr_bal;
				}
				else l_bal=0.00;
				break;
		case 4:				/* ����	*/
				if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
				{
					l_bal=vgl_mst.qdr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
				{
					l_bal=vgl_mst.qcr_bal;
				}
				else l_bal=0.00;
				break;
		case 5:				/* ����	*/
				if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
				{
					l_bal=vgl_mst.ydr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
				{
					l_bal=vgl_mst.ycr_bal;
				}
				else l_bal=0.00;
				break;
		default:
				break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
/***************************************************************/
/*ȡ�������*/
char	*f_get_cc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_lc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 1));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_tc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 2));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_mc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 3));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_sc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 4));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_yc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 5));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}


/***************************************************************/
/* ȡ�跽���*/
char	*f_get_cd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_ld_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 1));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_td_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 2));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_md_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 3));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_sd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 4));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

char	*f_get_yd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 5));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/*
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	�跽���
	��Ŀ��Ϊ˫�Կ�Ŀ
	input	:
				char _acchrt[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����
	output	:
				char	l_s_amt	������
*/
double f_get_d_bal(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

 	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
	 	goto ErrExit;
	}

	/* ҵ����� */
	switch(_d_flag)
	{
		case 0:				/*  ��ǰ���  (����) */
			if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
			{
				l_bal =vgl_mst.dr_bal;
			}
			else l_bal = 0.00;
			break;
		case 1:				/* ���� */
			if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
			{
				l_bal =vgl_mst.ldd_bal;
			}
			else l_bal = 0.00;
			break;
		case 2:				/* ��Ѯ	*/
			if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
			{
				l_bal =vgl_mst.tddr_bal;
			}
			else l_bal = 0.00;
			break;
		case 3:				/* ����	*/
			if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
			{
				l_bal =vgl_mst.mdr_bal;
			}
			else l_bal = 0.00;
			break;
		case 4:				/* ����	*/
			if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
			{
				l_bal =vgl_mst.qdr_bal;
			}
			else l_bal = 0.00;
			break;
		case 5:				/* ����		*/
			if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
			{
				l_bal =vgl_mst.ydr_bal;
			}
			else l_bal = 0.00;
			break;
		default:
			break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
/*
	(��������)���ݿ�Ŀ(�衢����Ŀ��)ȡ�������ڳ����
	�������
	��Ŀ��Ϊ˫�Կ�Ŀ
	input	:
				char _acchrt[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����
	output	:
				char	l_s_amt	������
*/
double f_get_c_bal(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

 	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
	 	goto ErrExit;
	}

	/* ҵ����� */
	switch(_d_flag)
	{
		case 0:				/*  ��ǰ���  (����) */
				if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
				{
					l_bal =vgl_mst.cr_bal;
				}
				else l_bal = 0.00;
				break;
		case 1:				/* ����     */
				if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
				{
					l_bal =vgl_mst.lcd_bal;
				}
				else l_bal = 0.00;
				break;
		case 2:				/* ��Ѯ		*/
				if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
				{
					l_bal =vgl_mst.tdcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 3:				/* ����		*/
				if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
				{
					l_bal =vgl_mst.mcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 4:				/* ����		*/
				if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
				{
					l_bal =vgl_mst.qcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 5:				/* ����		*/
				if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
				{
					l_bal =vgl_mst.ycr_bal;
				}
				else l_bal = 0.00;
				break;
		default:
				break;
	}/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
