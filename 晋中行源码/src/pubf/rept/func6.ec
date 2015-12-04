/**************************  ������������  ******************************/
/***  ���ݿ�Ŀ�š�����ȡ�ڳ���桢���������ڽ�����������������
		���ڽ����� ***/
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

/**************************   �� �� �� ��  ******************************/

/***
	ʵ�ֺ���  ���ݿ�Ŀ/����ȡ��ǰ(����)���
	input	:
				char_acchrt	��Ŀ��
				int _term
	output	:
				char	l_s_bal	���
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
	ʵ�ֺ���  ���ݿ�Ŀ/����ȡ�������
	input	:
				char	_acchrt	��Ŀ��
				int		_term
	output	:
				char	l_s_bal	���
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
	ʵ�ֺ���  ���ݿ�Ŀ/����ȡѮ�����
	input	:
				char	_acchrt	��Ŀ��
				int	_term
	output	:
				char	l_s_bal	���
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
	ʵ�ֺ���  ���ݿ�Ŀ��/����ȡ�³����
	input	:
				char	_acchrt		��Ŀ��
				int		_term
	output	:
				char	l_s_bal		�³����
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
	ʵ�ֺ���  ���ݿ�Ŀ��/����ȡ�������
	input	:
				char	_acchrt		��Ŀ��
				int		_term
	output	:
				char	l_s_bal		���
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
	ʵ�ֺ���  ���ݿ�Ŀ/���� ȡ������
	input	:
				int	_acchrt	��Ŀ��
				int _term   ����
	output	:
				char	l_s_bal	������/���
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
	(��������)���ݿ�Ŀ(�衢����Ŀ��)������ ȡ�������ڳ����
	input	:
				char _acchrt[8]	��Ŀ��
				int	 _d_flag	������ʱ��
						 	 0  ��ǰ���(����)
							 1  ����
							 2  ��Ѯ
							 3  ����
							 4  ����
							 5  ����

				int	_term		����

	output	:
				char	l_bal	���	
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
		case 0:				/*  ��ǰ���  (����) */
			sprintf(l_field, "%s", "cr_bal");
			break;
		case 1:				/* ����     */
			sprintf(l_field, "%s", "lcd_bal");
			break;
		case 2:				/* ��Ѯ		*/
			sprintf(l_field, "%s", "tdcr_bal");
			break;
		case 3:				/* ����		*/
			sprintf(l_field, "%s", "mcr_bal");
			break;
		case 4:				/* ����		*/
			sprintf(l_field, "%s", "qcr_bal");
			break;
		case 5:				/* ����		*/
			sprintf(l_field, "%s", "ycr_bal");
			break;
		default:
			break;
	}

	/* ҵ����� */

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


/*************************  ������  *********************************/
/*** 
	ȡ�跽������   ����
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
	ȡ����������   ����
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
	ȡ�跽������   ��Ѯ
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
	ȡ����������   ��Ѯ
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
	ȡ�跽������   ����
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
	ȡ����������   ����
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
	ȡ�跽������   ����
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
	ȡ����������   ����
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
	ȡ�跽������   ����
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
	ȡ����������   ����
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


/*********************  �������  **************************/
/*** 
	ȡ���ս����
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
	ȡ���մ�����
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
	ȡ��Ѯ�����
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
	ȡ��Ѯ������
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
	ȡ���½����
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
	ȡ���´�����
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
	ȡ���������
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
	ȡ����������
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
	ȡ��������
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
	ȡ���������
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

/**************** ��������  ***************************/
/*** 
	ȡ���տ�����
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
	ȡ��Ѯ������
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
	ȡ���¿�����
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
	ȡ����������
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
	ȡ���꿪����
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
	ȡ����������
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
	ȡ��Ѯ������
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
	ȡ����������
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
	ȡ����������
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
	ȡ����������
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

/*********************  ���໧��  *************************/
/*** 
	ȡ��ǰ���໧��
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
	ȡ���ս��໧��
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
	ȡ��Ѯ�����໧��
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
	ȡ���³����໧��
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
	ȡ���������໧��
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
	ȡ��������໧��
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
