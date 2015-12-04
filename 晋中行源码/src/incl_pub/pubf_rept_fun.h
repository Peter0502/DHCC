/*******************************��������***********************************/
#define D_CURNO     	RMB
#define BUFLEN			1023			/** ��ʱ��������С  **/
#define STR_TMP_LEN     1024			/** ���к�����ʱ�ַ��������ĳ���**/
#define LINE_LEN		1024			/** ����һ�еĴ�С  **/
#define CAL_FORM_LEN    1024			/** ��ʽ����**/
#define VAL_VAL_LEN     256             /** ����������    **/
#define FILE_NAME_LEN   256             /** �����ļ�������  **/
#define CAL_FORM_NUM    1024            /** ��������      **/
#define MAXBDSLEN       1024            /** ������㹫ʽ�ĳ��� **/
#define MAX_BRNO_NUM    50              /** ��������      **/
#define BRNO_LEN        5               /** �����ų���      **/

/*******************************��������***********************************/
#include "com_branch_c.h"
/*�����е�����*/
#ifndef EXTERNREPT
long	g_rept_date;					/*����������*/
struct  com_branch_c    g_brno[100];     /*���������,���50������*/
struct  com_branch_c    g_brno_tmp;     /*��ʱ���������,�5���ַ�*/
struct  PRT_VAL                         /* ��ӡֵ�ṹ */
{
	char    val_row[4];                 /* ��   */
	char    val_col[2];                 /* ��   */
	char    val_type[3];                /* ����  s �ַ��� d ���� f ������*/
	int     val_len;                    /* ���� */
	int     val_flag;                   /* �Ƿ��Ѿ�����  0-û����  1-�������*/
	char    val_val[VAL_VAL_LEN];       /* ֵ   */
};

struct PRT_VAL g_prt_val[1024];         /* ���汨�������еı��� */

struct  CAL_FORM                        /* ȡֵ��ʽ�ṹ */
{
	char    var_row[4];                 /* ����������   */
	char    var_col[2];                 /* ������(��)   */
	char    var_type[3];                /* ����  s �ַ��� d ���� f ������*/
	char    var_body[CAL_FORM_LEN];     /* ���㹫ʽ     */
};

struct  CAL_FORM    g_cal_form[CAL_FORM_NUM];   /*  ���㹫ʽ            */

/*һ����������*/
#else
extern long	g_rept_date;					/*����������*/
extern struct  com_branch_c    g_brno[50];     /*���������,���50������*/
extern struct  com_branch_c    g_brno_tmp;     /*��ʱ���������,�5���ַ�*/
extern struct PRT_VAL g_prt_val[1024];         /* ���汨�������еı��� */
extern struct  CAL_FORM    g_cal_form[CAL_FORM_NUM];   /*  ���㹫ʽ            */

#endif

/*****************************��������************************************/
char *f_get_current_wi_bal(char *_acchrt);
char *f_get_current_bal(char *_acchrt);
char *f_get_yesterday_bal(char *_acchrt);
char *f_get_yesterday_wi_bal(char *_acchrt);
char *f_get_befyesterday_bal(char *_acchrt);
char *f_get_befmonday_bal(char *_acchrt);
char *f_get_tenday_bal(char *_acchrt);
char *f_get_tenday_wi_bal(char *_acchrt);
char *f_get_month_bal(char *_acchrt);
char *f_get_month_wi_bal(char *_acchrt);
char *f_get_season_bal(char *_acchrt);
char *f_get_season_wi_bal(char *_acchrt);
char *f_get_year_bal(char *_acchrt);
char *f_get_year_wi_bal(char *_acchrt);
char *f_get_current_d_amt(char *_acchrt);
char *f_get_current_d_amt_wi(char *_acchrt);
char *f_get_current_c_amt(char *_acchrt);
char *f_get_current_c_amt_wi(char *_acchrt);
char *f_get_tenday_d_amt(char *_acchrt);
char *f_get_tenday_d_amt_wi(char *_acchrt);
char *f_get_tenday_c_amt(char *_acchrt);
char *f_get_tenday_c_amt_wi(char *_acchrt);
char *f_get_month_d_amt(char *_acchrt);
char *f_get_month_d_amt_wi(char *_acchrt);
char *f_get_month_c_amt(char *_acchrt);
char *f_get_month_c_amt_wi(char *_acchrt);
char *f_get_season_d_amt(char *_acchrt);
char *f_get_season_d_amt_wi(char *_acchrt);
char *f_get_season_c_amt(char *_acchrt);
char *f_get_season_c_amt_wi(char *_acchrt);
char *f_get_year_d_amt(char *_acchrt);
char *f_get_year_d_amt_wi(char *_acchrt);
char *f_get_year_c_amt(char *_acchrt);
char *f_get_year_c_amt_wi(char *_acchrt);
char *f_get_today_d_cnt(char *_acchrt);
char *f_get_today_c_cnt(char *_acchrt);
char *f_get_tenday_d_cnt(char *_acchrt);
char *f_get_tenday_c_cnt(char *_acchrt);
char *f_get_month_d_cnt(char *_acchrt);
char *f_get_month_c_cnt(char *_acchrt);
char *f_get_season_d_cnt(char *_acchrt);
char *f_get_season_c_cnt(char *_acchrt);
char *f_get_year_d_cnt(char *_acchrt);
char *f_get_year_c_cnt(char *_acchrt);
double f_get_bal(char * _acchrt, int    _d_flag, char * _dc_flag);
double f_get_bal_d(char * _acchrt, int  _d_flag);
double f_get_l_bal(char * _acchrt, int  _d_flag, char * _dc_flag);
double f_get_l_bal_d(char * _acchrt, int    _d_flag);
double f_get_d_bal(char * _acchrt, int  _d_flag);
double f_get_c_bal(char * _acchrt, int  _d_flag);
/************************************************************/
double f_get_bal_short(char * _prdt_no, int _d_flag);
char *f_get_current_bal_short(char *_prdt_no);
char *f_get_yesterday_bal_short(char *_prdt_no);
char  *f_get_tenday_bal_short(char *_prdt_no);
char  *f_get_month_bal_short(char *_prdt_no);
char  *f_get_season_bal_short(char *_prdt_no);
char  *f_get_year_bal_short(char *_prdt_no);
char  *f_get_current_d_amt_short(char *_prdt_no);
char  *f_get_current_c_amt_short(char *_prdt_no);
char *f_get_tenday_d_amt_short(char *_prdt_no);
char *f_get_tenday_c_amt_short(char *_prdt_no);
char *f_get_month_d_amt_short(char *_prdt_no);
char *f_get_month_c_amt_short(char *_prdt_no);
char *f_get_season_d_amt_short(char *_prdt_no);
char *f_get_season_c_amt_short(char *_prdt_no);
char *f_get_year_d_amt_short(char *_prdt_no);
char *f_get_year_c_amt_short(char *_prdt_no);
char *f_get_max_ln_bal();
char *f_get_maxsum_ln_bal();
char *pub_rept_dealfunc(char *var_body);
