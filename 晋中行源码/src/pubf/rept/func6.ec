/**************************  声明基础函数  ******************************/
/***  根据科目号、存期取期初结存、户数；本期借贷发生额、开销户数；
		本期结存金额户数 ***/
char *f_get_current_dtno_bal(char *_acchrt, char *_term);
char *f_get_yesterday_dtno_bal(char *_acchrt, char *_term);
char *f_get_tenday_dtno_bal(char *_acchrt, char *_term);
char *f_get_month_dtno_bal(char *_acchrt, char  *_term);
char *f_get_season_dtno_bal(char *_acchrt, char *_term);
char *f_get_year_dtno_bal(char *_acchrt, char *_term);
double f_get_dtno_bal(char * _acchrt, int	_d_flag , int	_term);
char	*f_get_current_d_dtno_amt(char *_acchrt, char *_term);
char	*f_get_current_c_dtno_amt(char *_acchrt, char *_term);
char	*f_get_tenday_d_dtno_amt(char *_acchrt,char *_term);
char	*f_get_tenday_c_dtno_amt(char *_acchrt, char *_term);
char	*f_get_month_d_dtno_amt(char *_acchrt,char *_term);
char	*f_get_month_c_dtno_amt(char *_acchrt,char *_term);
char	*f_get_season_d_dtno_amt(char *_acchrt,char *_term);
char	*f_get_season_c_dtno_amt(char *_acchrt, char *_term);
char	*f_get_year_d_dtno_amt(char *_acchrt, char *_term);
char	*f_get_year_c_dtno_amt(char *_acchrt,char *_term);
char	*f_get_today_d_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_today_c_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_tenday_d_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_tenday_c_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_month_d_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_month_c_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_season_d_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_season_c_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_year_d_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_year_c_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_today_opn_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_tenday_opn_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_month_opn_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_season_opn_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_year_opn_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_today_cls_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_tenday_cls_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_month_cls_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_season_cls_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_year_cls_dtno_cnt(char *_acchrt, char *_term);
char	*f_get_current_dtno_acnovol(char *_acchrt,char *_term);
char	*f_get_yesterday_dtno_acnovol(char *_acchrt,char *_term);
char	*f_get_tenday_dtno_acnovol(char *_acchrt,char *_term);
char	*f_get_month_dtno_acnovol(char *_acchrt,char *_term);
char	*f_get_season_dtno_acnovol(char *_acchrt,char *_term);
char	*f_get_year_dtno_acnovol(char *_acchrt,char *_term);

/**************************   声 明 结 束  ******************************/

/***
	实现函数  根据科目/存期取当前(本日)余额
	input	:
				char_acchrt	科目号
				int _term
	output	:
				char	l_s_bal	余额
***/
char *f_get_current_dtno_bal(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 0, l_term));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目/存期取昨日余额
	input	:
				char	_acchrt	科目号
				int		_term
	output	:
				char	l_s_bal	余额
***/
char *f_get_yesterday_dtno_bal(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 1, l_term));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目/存期取旬初余额
	input	:
				char	_acchrt	科目号
				int	_term
	output	:
				char	l_s_bal	余额
***/
char *f_get_tenday_dtno_bal(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 2, l_term));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目号/存期取月初余额
	input	:
				char	_acchrt		科目号
				int		_term
	output	:
				char	l_s_bal		月初余额
***/
char *f_get_month_dtno_bal(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 3, l_term));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	实现函数  根据科目号/存期取季初余额
	input	:
				char	_acchrt		科目号
				int		_term
	output	:
				char	l_s_bal		余额
***/
char *f_get_season_dtno_bal(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 4, l_term));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目/存期 取年初余额
	input	:
				int	_acchrt	科目号
				int _term   存期
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_year_dtno_bal(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
		int		l_term;
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	sprintf(l_s_bal,"%f", f_get_dtno_bal(l_acchrt, 5, l_term));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*
	(公共函数)根据科目(借、贷科目号)、存期 取余额或者期初余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年

				int	_term		存期

	output	:
				char	l_bal	余额	
*/
double f_get_dtno_bal(char * _acchrt, int	_d_flag , int	_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_field[256];
		char	l_sql[256];
		char	l_tmp[256];
		double	l_bal = 0.00;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));
	memset(l_sql, 0x00, sizeof(l_sql));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	l_term = _term;
	strcpy(l_acchrt, _acchrt);

	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
			sprintf(l_field, "%s", "cr_bal");
			break;
		case 1:				/* 昨日     */
			sprintf(l_field, "%s", "lcd_bal");
			break;
		case 2:				/* 本旬		*/
			sprintf(l_field, "%s", "tdcr_bal");
			break;
		case 3:				/* 本月		*/
			sprintf(l_field, "%s", "mcr_bal");
			break;
		case 4:				/* 本季		*/
			sprintf(l_field, "%s", "qcr_bal");
			break;
		case 5:				/* 本年		*/
			sprintf(l_field, "%s", "ycr_bal");
			break;
		default:
			break;
	}

	/* 业务机构 */

		sprintf(l_sql,"select sum(%s) from gl_i_sm where date = %ld \
			and brno = '%s' and curno = '%s' and acchrt = '%s' and term=%d",\
			l_field, g_rept_date, g_brno_tmp.brno, D_CURNO, l_acchrt, l_term);

	EXEC SQL prepare gis_id from :l_sql;
	MYSQLERR

	EXEC SQL declare glism_cur cursor for gis_id;
	MYSQLERR

	EXEC SQL open glism_cur;
	MYSQLERR

    EXEC SQL fetch glism_cur into :l_bal :a1;
	MYSQLERR

	if(a1) 	l_bal = 0.00;

	sprintf(acErrMsg,"g_rept_date[%ld] l_field[%s] brno[%s] acchrt[%s] term[%d] l_bal[%lf]",	g_rept_date,l_field,g_brno_tmp.brno,l_acchrt,l_term,l_bal);
	WRITEMSG

   	EXEC SQL close glism_cur;
   	MYSQLERR

	EXEC SQL free glism_cur;

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}


/*************************  发生额  *********************************/
/*** 
	取借方发生额   本日
***/
char	*f_get_current_d_dtno_amt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(rdd_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	取贷方发生额   本日
***/
char	*f_get_current_c_dtno_amt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);	

	EXEC SQL select sum(rcd_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本旬
***/
char	*f_get_tenday_d_dtno_amt(char *_acchrt,char * _term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tddr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	取贷方发生额   本旬
***/
char	*f_get_tenday_c_dtno_amt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
		EXEC SQL select sum(tdcr_amt) into :l_amt :a1
				   from gl_i_sm
				  where date = :g_rept_date and 
						brno = :g_brno_tmp.brno and
						curno = '01' and
						term = :l_term and
						acchrt = :l_acchrt;
		MYSQLERR
		if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本月
***/
char	*f_get_month_d_dtno_amt(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);

	vtcp_log("g_rept_date[%ld]  brno[%s] acchrt[%s] term[%d]",\
			g_rept_date,g_brno_tmp.brno,l_acchrt,l_term);
	EXEC SQL select sum(mdr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;
	sprintf(acErrMsg,"g_rept_date[%ld] brno[%s] acchrt[%s] term[%d] l_amt[%lf]",	g_rept_date,g_brno_tmp.brno,l_acchrt,l_term,l_amt);
	WRITEMSG

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取贷方发生额   本月
***/
char	*f_get_month_c_dtno_amt(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(mcr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本季
***/
char	*f_get_season_d_dtno_amt(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qdr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取贷方发生额   本季
***/
char	*f_get_season_c_dtno_amt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qcr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本年
***/
char	*f_get_year_d_dtno_amt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(ydr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取贷方发生额   本年
***/
char	*f_get_year_c_dtno_amt(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		double	l_amt;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_amt[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(ycr_amt) into :l_amt :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_amt = 0.00;

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*********************  借贷笔数  **************************/
/*** 
	取本日借笔数
***/
char	*f_get_today_d_dtno_cnt(char *_acchrt, char * _term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(rdd_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本日贷笔数
***/
char	*f_get_today_c_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(rcd_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本旬借笔数
***/
char	*f_get_tenday_d_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tddr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本旬贷笔数
***/
char	*f_get_tenday_c_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tdcr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本月借笔数
***/
char	*f_get_month_d_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(mdr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本月贷笔数
***/
char	*f_get_month_c_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(mcr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本季借笔数
***/
char	*f_get_season_d_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qdr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本季贷笔数
***/
char	*f_get_season_c_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qcr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年借笔数
***/
char	*f_get_year_d_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(ydr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年贷笔数
***/
char	*f_get_year_c_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(ycr_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}

/**************** 开销户数  ***************************/
/*** 
	取本日开户数
***/
char	*f_get_today_opn_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(d_copncnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本旬开户数
***/
char	*f_get_tenday_opn_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tdopn_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本月开户数
***/
char	*f_get_month_opn_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(mopncnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本季开户数
***/
char	*f_get_season_opn_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qopn_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年开户数
***/
char	*f_get_year_opn_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(yopn_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}


/*** 
	取本日销户数
***/
char	*f_get_today_cls_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(cdcls_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本旬销户数
***/
char	*f_get_tenday_cls_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tdcls_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本月销户数
***/
char	*f_get_month_cls_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(mcls_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本季销户数
***/
char	*f_get_season_cls_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qcls_cnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年销户数
***/
char	*f_get_year_cls_dtno_cnt(char *_acchrt, char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(yclscnt) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}

/*********************  结余户数  *************************/
/*** 
	取当前结余户数
***/
char	*f_get_current_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	vtcp_log("aaaaaaaaaaa");
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	if( strncmp(g_brno_tmp.brno,"00000",5) )
	EXEC SQL select sum(acno_vol) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}

/*** 
	取昨日结余户数
***/
char	*f_get_yesterday_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(lacnovol) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本旬初结余户数
***/
char	*f_get_tenday_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(tdacnovol) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本月初结余户数
***/
char	*f_get_month_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(macnovol) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本季初结余户数
***/
char	*f_get_season_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(qacnovol) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年初结余户数
***/
char	*f_get_year_dtno_acnovol(char *_acchrt,char *_term)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		int		l_count;
		int		l_term;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	char	l_s_count[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	l_term = atoi(_term);
	
	EXEC SQL select sum(ncjyhs) into :l_count :a1
			   from gl_i_sm
			  where date = :g_rept_date and 
					brno = :g_brno_tmp.brno and
					curno = '01' and
					term = :l_term and 
					acchrt = :l_acchrt;
	MYSQLERR
	if(a1) l_count = 0;

	sprintf(l_s_count, "%d", l_count);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*******************************************************************/
