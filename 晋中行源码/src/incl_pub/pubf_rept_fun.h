/*******************************常量定义***********************************/
#define D_CURNO     	RMB
#define BUFLEN			1023			/** 临时缓冲区大小  **/
#define STR_TMP_LEN     1024			/** 所有函数临时字符串变量的长度**/
#define LINE_LEN		1024			/** 读入一行的大小  **/
#define CAL_FORM_LEN    1024			/** 公式长度**/
#define VAL_VAL_LEN     256             /** 计算结果长度    **/
#define FILE_NAME_LEN   256             /** 报表文件名长度  **/
#define CAL_FORM_NUM    1024            /** 最大变量数      **/
#define MAXBDSLEN       1024            /** 保存计算公式的长度 **/
#define MAX_BRNO_NUM    50              /** 最大机构数      **/
#define BRNO_LEN        5               /** 机构号长度      **/

/*******************************变量定义***********************************/
#include "com_branch_c.h"
/*主控中的声明*/
#ifndef EXTERNREPT
long	g_rept_date;					/*出报表日期*/
struct  com_branch_c    g_brno[100];     /*保存机构号,最多50个机构*/
struct  com_branch_c    g_brno_tmp;     /*临时保存机构号,最长5个字符*/
struct  PRT_VAL                         /* 打印值结构 */
{
	char    val_row[4];                 /* 行   */
	char    val_col[2];                 /* 列   */
	char    val_type[3];                /* 属性  s 字符串 d 整型 f 浮点型*/
	int     val_len;                    /* 长度 */
	int     val_flag;                   /* 是否已经计算  0-没计算  1-计算完毕*/
	char    val_val[VAL_VAL_LEN];       /* 值   */
};

struct PRT_VAL g_prt_val[1024];         /* 保存报表中所有的变量 */

struct  CAL_FORM                        /* 取值公式结构 */
{
	char    var_row[4];                 /* 变量所在行   */
	char    var_col[2];                 /* 变量名(列)   */
	char    var_type[3];                /* 属性  s 字符串 d 整型 f 浮点型*/
	char    var_body[CAL_FORM_LEN];     /* 计算公式     */
};

struct  CAL_FORM    g_cal_form[CAL_FORM_NUM];   /*  计算公式            */

/*一般程序的声明*/
#else
extern long	g_rept_date;					/*出报表日期*/
extern struct  com_branch_c    g_brno[50];     /*保存机构号,最多50个机构*/
extern struct  com_branch_c    g_brno_tmp;     /*临时保存机构号,最长5个字符*/
extern struct PRT_VAL g_prt_val[1024];         /* 保存报表中所有的变量 */
extern struct  CAL_FORM    g_cal_form[CAL_FORM_NUM];   /*  计算公式            */

#endif

/*****************************函数声明************************************/
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
