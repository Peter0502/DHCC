/*************************************************
* �� �� ��: pubf_rept_cal.c
* ��������: �ձ���ͳ�Ƹ������
*
* �������:   2004��05��04
* �޸ļ�¼:
*	 1. ��    ��:
*		�� �� ��:
*		�޸�����:
*	 2.
*************************************************/

#define EXTERN
#define EXTERNREPT
#include <string.h>
#include "public.h"
#include "gl_prdt_dyn_c.h"
#include "pubf_rept_fun.h"
#include "gl_mst_c.h"

/***************************************
�������ܣ�ͳ������(GL_MSt)double���ֶ�
������
    ��ڣ�	������ char *br_no_lst
			��Ŀ�� char *acc_hrt
    		�ֶ��� char *sum_field
���أ�ͳ�ƽ��
ע������ ֻͳ�������
***************************************/
double sum_GlMst_double( char *br_no_lst, char *acc_hrt, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}

	sprintf( tmpstr, "acc_hrt='%s' and cur_no='01'", acc_hrt );
	strcat( wherelist, tmpstr );

	g_reply_int = sql_sum_double( "gl_mst", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "   br_no[%s]", br_no_lst );
		vtcp_log( " acc_hrt[%s]", acc_hrt );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "����ͳ��double���ֶγ���![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
�������ܣ�ͳ���ֽ�̨��(CASH_MST)double���ֶ�
������
    ��ڣ�	������ char *br_no_lst
			����Ա char *tel_lst
    		�ֶ��� char *sum_field
���أ�ͳ�ƽ��
ע������ ֻͳ�������
***************************************/
double sum_CashMst_double( char *br_no_lst, char *tel_lst, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	if( strlen( tel_lst ) )
	{
		sprintf( tmpstr, "tel in (%s) and ", tel_lst );
		strcat( wherelist, tmpstr );
	}
	strcat( wherelist, "cur_no='01'" );

	g_reply_int = sql_sum_double( "cash_mst", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "  br_no[%s]", br_no_lst );
		vtcp_log( "    tel[%s]", tel_lst );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "�ֽ�̨��ͳ��double���ֶγ���![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
�������ܣ�ͳ�Ʋ�Ʒ��̬��(GL_PRDT_DYN)double���ֶ�
������
    ��ڣ�	������ char *br_no_lst
			��Ʒ�� char *prdt_no_lst
    		�ֶ��� char *sum_field
���أ�ͳ�ƽ��
***************************************/
double sum_GlPrdtDyn_double( char *br_no_lst, char *prdt_no_lst, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	memset( wherelist, 0x00, sizeof( wherelist ) );
	if( strlen( br_no_lst ) )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	if( prdt_no_lst != '\0' )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr, "prdt_no in (%s) and ", prdt_no_lst );
		strcat( wherelist, tmpstr );
	}
	strcat( wherelist, "1=1" );

	vtcp_log("sum_field[%s]",sum_field);
	g_reply_int = sql_sum_double( "gl_prdt_dyn", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "    br_no[%s]", br_no_lst );
		vtcp_log( "  prdt_no[%s]", prdt_no_lst );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "��Ʒ��̬��ͳ��double���ֶγ���![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
�������ܣ�ͳ�ƿ�������(ASS_GL)double���ֶ�
������
    ��ڣ�	������ char *br_no_lst
			��Ŀ�� char *acc_hrt
    		�ֶ��� char *sum_field
���أ�ͳ�ƽ��
ע��ֻͳ��״̬Ϊ�����ļ�¼
***************************************/
double sum_AssGl_double( char *br_no_lst, char *acc_hrt, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	sprintf( tmpstr, "acc_hrt='%s' and sts='1'", acc_hrt );
	strcat( wherelist, tmpstr );

	g_reply_int = sql_sum_double( "ass_gl", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "    br_no[%s]", br_no_lst );
		vtcp_log( "  acc_hrt[%s]", acc_hrt );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "��������ͳ��double���ֶγ���![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

