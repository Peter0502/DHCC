/**************************  声明基础函数  ******************************/
double f_get_brno_bal(char *_brno, char *_acchrt, int _d_flag, char *_dc_flag);
double f_get_brno_bal_d(char *_brno, char * _acchrt, int  _d_flag);
char *f_get_brno_current_bal(char *_brno, char *_acchrt);

double f_get_brno_d_bal(char *_brno, char * _acchrt, int _d_flag);
double f_get_brno_c_bal(char *_brno, char * _acchrt, int _d_flag);
char    *f_get_brno_cc_bal(char *_brno, char    *_acchrt);
char    *f_get_brno_cd_bal(char *_brno, char    *_acchrt);

/***
	实现函数  根据科目取当前(本日)余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_brno_current_bal(char *_brno, char *_acchrt)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
		char	l_dc[3];
	EXEC SQL END DECLARE SECTION;

	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_acchrt, _acchrt);

	EXEC SQL select dcind into :l_dc from com_acc where acchrt = :l_acchrt;
	MYSQLERR

	if( l_dc[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_brno_bal_d(_brno, l_acchrt, 0));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_brno_bal(_brno, l_acchrt, 0, l_dc));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*

							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
				char _dc_flag	借贷方向标志
							 1  借
							 2  贷
	output	:
				char	l_s_amt	发生额
*/
double f_get_brno_bal(char *_brno, char * _acchrt, int	_d_flag, char * _dc_flag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_brno[10];
		char	l_acchrt[8];
		char	l_field[256];
		char	l_sql[256];
		char	l_tmp[256];
		double	l_bal = 0.00;
		int		a1;
		int		_o_flag;
	EXEC SQL END DECLARE SECTION;

	memset(l_brno, 0x00, sizeof(l_brno));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));
	memset(l_sql, 0x00, sizeof(l_sql));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	strcpy(l_brno,_brno);
	strcpy(l_acchrt, _acchrt);
	sprintf(l_tmp, "%c", _dc_flag[0]);
	_o_flag = atoi(l_tmp);

	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "dr_bal");
			}else 									/*  贷方  */
			{
				sprintf(l_field, "%s", "cr_bal");
			}
			break;
		case 1:				/* 昨日     */
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "ldd_bal");
			}else									/*  贷方  */
			{
				sprintf(l_field, "%s", "lcd_bal");
			}
			break;
		case 2:				/* 本旬		*/
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "tddr_bal");
			}else									/*  贷方  */
			{
				sprintf(l_field, "%s", "tdcr_bal");
			}
			break;
		case 3:				/* 本月		*/
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "mdr_bal");
			}else									/*  贷方  */
			{
				sprintf(l_field, "%s", "mcr_bal");
			}
			break;
		case 4:				/* 本季		*/
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "qdr_bal");
			}else									/*  贷方  */
			{
				sprintf(l_field, "%s", "qcr_bal");
			}
			break;
		case 5:				/* 本年		*/
			if( _o_flag == 1 ){						/*  借方  */
				sprintf(l_field, "%s", "ydr_bal");
			}else									/*  贷方  */
			{
				sprintf(l_field, "%s", "ycr_bal");
			}
			break;
		default:
			break;
	}

	/* 业务机构 */
	/*
	if(atoi(g_brno_tmp.type)>=1 && atoi(g_brno_tmp.type)<=9)
	{
		sprintf(l_sql,"select %s from gl_mst where date = %ld and brno = '%s' and curno = '%s' and acchrt = '%s'",l_field, g_rept_date, l_brno, D_CURNO, l_acchrt);
	}else	* 虚拟机构 *
	{
		sprintf(l_sql,"select %s from gl_mst_sm where date = %ld and brno = '%s' and curno = '%s' and acchrt = '%s'",l_field, g_rept_date, l_brno, D_CURNO, l_acchrt);
	}
	*/

	sprintf(l_sql,"select %s from gl_mst where date = %ld and brno = '%s' and curno = '%s' and acchrt = '%s'",l_field, g_rept_date, l_brno, D_CURNO, l_acchrt);

	EXEC SQL prepare qry_glmst_a from :l_sql;
	MYSQLERR

	EXEC SQL declare glmst_cur_a cursor for qry_glmst_a;
	MYSQLERR

	EXEC SQL open glmst_cur_a;

	MYSQLERR

    EXEC SQL fetch glmst_cur_a into :l_bal :a1;
	MYSQLERR

	if(a1) 	l_bal = 0.00;

   	EXEC SQL close glmst_cur_a;
   	MYSQLERR

	EXEC SQL free glmst_cur_a;

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	科目号为双性科目
	根据机构号、科目号计算借方余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*/
double f_get_brno_bal_d(char *_brno, char * _acchrt, int	_d_flag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_brno[10];
		char	l_acchrt[8];
		char	l_field[256];
		char	l_sql[256];
		double	l_bal = 0.00;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	memset(l_brno, 0x00, sizeof(l_brno));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));
	memset(l_sql, 0x00, sizeof(l_sql));

	strcpy(l_brno, _brno);
	strcpy(l_acchrt, _acchrt);

	/* 业务机构 */
	/*
	if(atoi(g_brno_tmp.type) >= 1 && atoi(g_brno_tmp.type) <= 9)
	{
	*/
		switch(_d_flag)
		{
			case 0:				/*  当前余额  (本日) */
				EXEC SQL select dr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01'  and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select cr_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}

				if(a1) l_bal = 0.00;
				break;
			case 1:				/* 昨日     */
				EXEC SQL select ldd_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select lcd_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 2:				/* 本旬		*/
				EXEC SQL select tddr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select tdcr_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 3:				/* 本月		*/
				EXEC SQL select mdr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select mcr_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 4:				/* 本季		*/
				EXEC SQL select qdr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select qcr_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 5:				/* 本年		*/
				EXEC SQL select ydr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select ycr_bal into :l_bal :a1
							   from gl_mst
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			default:
				break;
		}		/* end of switch */
	/*
	}else	* 虚拟机构 *
	{
		switch(_d_flag)
		{
			case 0:	
				EXEC SQL select dr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select cr_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 1:	
				EXEC SQL select ldd_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select lcd_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 2:	
				EXEC SQL select tddr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select tdcr_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 3:	
				EXEC SQL select mdr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select mcr_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 4:	
				EXEC SQL select qdr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select qcr_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			case 5:
				EXEC SQL select ydr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(	a1) l_bal = 0.00;
				if( l_bal = 0.00 )
				{
					EXEC SQL select ycr_bal into :l_bal :a1
							   from gl_mst_sm
							  where date = :g_rept_date and 
									brno = :l_brno and
									curno = '01' and
									acchrt = :l_acchrt;
				}
				if(a1) l_bal = 0.00;
				break;
			default:
				break;
		}* end of switch*
	}* end if*
	*/

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}



/*******************************************************************/
/*取贷方余额*/
char	*f_get_brno_cc_bal(char *_brno, char	*_acchrt)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
	EXEC SQL END DECLARE SECTION;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_brno_c_bal(_brno, l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/* 取借方余额*/
char	*f_get_brno_cd_bal(char *_brno, char	*_acchrt)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_acchrt[8];
	EXEC SQL END DECLARE SECTION;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_brno_d_bal(_brno, l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	借方余额
	科目号为双性科目
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*/
double f_get_brno_d_bal(char *_brno, char * _acchrt, int	_d_flag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_brno[10];
		char	l_acchrt[8];
		char	l_field[256];
		char	l_sql[256];
		double	l_bal = 0.00;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	memset(l_brno, 0x00, sizeof(l_brno));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));
	memset(l_sql, 0x00, sizeof(l_sql));

	strcpy(l_acchrt, _acchrt);

	/* 业务机构 */
	if(atoi(g_brno_tmp.type) >= 1 && atoi(g_brno_tmp.type) <= 9)
	{
		switch(_d_flag)
		{
			case 0:				/*  当前余额  (本日) */
				EXEC SQL select dr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01'  and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 1:				/* 昨日     */
				EXEC SQL select ldd_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 2:				/* 本旬		*/
				EXEC SQL select tddr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 3:				/* 本月		*/
				EXEC SQL select mdr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 4:				/* 本季		*/
				EXEC SQL select qdr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 5:				/* 本年		*/
				EXEC SQL select ydr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			default:
				break;
		}		/* end of switch */
	}else	/* 虚拟机构 */
	{
		switch(_d_flag)
		{
			case 0:				/*  当前余额  (本日) */
				EXEC SQL select dr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 1:				/* 昨日     */
				EXEC SQL select ldd_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 2:				/* 本旬		*/
				EXEC SQL select tddr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 3:				/* 本月		*/
				EXEC SQL select mdr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 4:				/* 本季		*/
				EXEC SQL select qdr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 5:				/* 本年		*/
				EXEC SQL select ydr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(	a1) l_bal = 0.00;
				break;
			default:
				break;
		}/* end of switch*/
	}/* end if*/

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	贷方余额
	科目号为双性科目
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*/
double f_get_brno_c_bal(char *_brno, char * _acchrt, int	_d_flag)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char	l_brno[10];
		char	l_acchrt[8];
		char	l_field[256];
		char	l_sql[256];
		double	l_bal = 0.00;
		int		a1;
	EXEC SQL END DECLARE SECTION;

	memset(l_brno, 0x00, sizeof(l_brno));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));
	memset(l_sql, 0x00, sizeof(l_sql));

	strcpy(l_brno, _brno);
	strcpy(l_acchrt, _acchrt);

	/* 业务机构 */
	if(atoi(g_brno_tmp.type) >= 1 && atoi(g_brno_tmp.type) <= 9)
	{
		switch(_d_flag)
		{
			case 0:				/*  当前余额  (本日) */
				EXEC SQL select cr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01'  and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 1:				/* 昨日     */
				EXEC SQL select lcd_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 2:				/* 本旬		*/
				EXEC SQL select tdcr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 3:				/* 本月		*/
				EXEC SQL select mcr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 4:				/* 本季		*/
				EXEC SQL select qcr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 5:				/* 本年		*/
				EXEC SQL select ycr_bal into :l_bal :a1
						   from gl_mst
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			default:
				break;
		}		/* end of switch */
	}else	/* 虚拟机构 */
	{
		switch(_d_flag)
		{
			case 0:				/*  当前余额  (本日) */
				EXEC SQL select cr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 1:				/* 昨日     */
				EXEC SQL select lcd_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 2:				/* 本旬		*/
				EXEC SQL select tdcr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 3:				/* 本月		*/
				EXEC SQL select mcr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 4:				/* 本季		*/
				EXEC SQL select qcr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(a1) l_bal = 0.00;
				break;
			case 5:				/* 本年		*/
				EXEC SQL select ycr_bal into :l_bal :a1
						   from gl_mst_sm
						  where date = :g_rept_date and 
								brno = :l_brno and
								curno = '01' and
								acchrt = :l_acchrt;
				if(	a1) l_bal = 0.00;
				break;
			default:
				break;
		}/* end of switch*/
	}/* end if*/

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
