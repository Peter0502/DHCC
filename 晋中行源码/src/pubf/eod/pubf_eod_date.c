/*************************************************
* �ļ���:    pubf_com_date.c
* ���������� ���մ����������ڼ�麯����
*           
*
* ��   ��:   jack	 
* ������ڣ� 2004��1��13��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "math.h"
#include "com_holiday_c.h"

/**********************************************************************
 * ��  �� ����  pub_eod_chkSysdate
 * ���� ����:   
 *		������ִ�������Ƿ��ǹ涨������֮��
 * 		0��ÿ�죻1��5�գ�2��ÿ�ܣ�3��ÿѮ��4��ÿ�£�5��ÿ����
 *		6��ÿ���ꣻ7��ÿ��             
 * ����/ʱ�䣺  2004��1��13��
 * 
 * ��  ����
 *     ���룺  
 *		long sysdate,		ϵͳ��������
 *		char *exec_pd,		ִ������
 *		char *pd_month,		�����ڵĵڼ�����
 *		char *exec_beg,		ÿ�µĿ�ʼ����
 *		char *exec_end 		ÿ�µ���ֹ����
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int	pub_eod_chkSysdate( 
		long sysdate,
		char *exec_pd,
		char *pd_month,
		char *exec_beg,
		char *exec_end )
{
	int	ret;

	switch ( exec_pd[0] )
	{
		case '0':	/* ÿ�գ����ü�� */
			ret = 0;
			break;			
		case '1':	/* 5�� */
			ret = pub_eod_chkFive( sysdate, exec_beg, exec_end );
			sprintf( acErrMsg, "ret=[%d]",ret);		WRITEMSG
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��5�ռ���" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '2':	/* ÿ�� */

			ret = pub_eod_chkWeek( sysdate, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿ�ܼ���" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '3':	/* ÿѮ */

			ret = pub_eod_chkXun( sysdate, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿѮ����" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '4':	/* ÿ�� */
			ret = pub_eod_chkMonth( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿ�¼���" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '5':	/* ÿ�� */
			ret = pub_eod_chkQuarter( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿ������" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '6':	/* ÿ���� */
			ret = pub_eod_chkHalf( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿ�������" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '7':	/* ÿ�� */
			ret = pub_eod_chkYear( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��ÿ�����" );
				WRITEMSG
				return(-1);	
			}
			break;
		default:	/* ���� */
			sprintf( acErrMsg, "�����ڴ�ִ������[%s]", exec_pd );
			WRITEMSG
			strcpy( g_pub_tx.reply, "G004" );
			return(-1);		
	}
	return(0);	
}
/**********************************************************************
 * ��  �� ����  pub_eod_chkFive
 * ���� ����:   
 *		����������еĵڼ���
 *             
 * ����/ʱ�䣺  2004��1��13��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_eod_chkFive( long sys_date, char *exec_beg, char *exec_end )
{
	long	ncrq;
	long	y1, m1, d1, days, days1, days2, curno;

	pub_base_get_ymd( &y1, &m1, &d1, sys_date, &curno);
	ncrq = y1 * 10000 + 101;

	days = pub_base_CalTrueDays( ncrq, sys_date ) + 1;

	days = days%5;
	if( days == 0 ) days = 5;
	days1 = atoi( exec_beg );
	days2 = atoi( exec_end );
	if( days1 == 99 && days2 == 99 )
	{
		days1 = 5;
		days2 = 5;
	}

	if( days1 <= days && days <= days2)
	{
		return 0;
	}

	return 1;
}

/**********************************************************************
 * ��  �� ����  pub_eod_chkWeek
 * ���� ����:   
 *		��ִ�м��
 *             
 * ����/ʱ�䣺  2004��1��13��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int pub_eod_chkWeek( long sys_date, char *exec_beg, char *exec_end )
{
	long	xq, xq1, xq2;	
	
	xq = pub_base_day_week( sys_date );
	if( xq < 0 ) return 1;

	xq1 = atol( exec_beg );
	xq2 = atol( exec_end );
	
	if( xq1 == 99 && xq2 == 99 )
	{
		xq1 = 7;
		xq2 = 7;
	}
	
	if( xq1 <= xq && xq <= xq2 )
	{
		return 0;
	}

	return 1;
}

/**********************************************************************
 * ��  �� ����  pub_eod_chkXun
 * ���� ����:   
 *		Ѯִ�м��
 *             
 * ����/ʱ�䣺  2004��1��13��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
pub_eod_chkXun( long sys_date, char *exec_beg, char *exec_end )
{
	long	xq,xq1,xq2;
	long	lBeg,lEnd;
	int	ret;
		
	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	
	xq = pub_base_day_xun( sys_date );
	if( xq < 0 ) return 1;

	/* �����Ѯĩ�����ж� */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_xun( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}

	xq1 = atol( exec_beg );
	xq2 = atol( exec_end );
	if( xq1 <= xq && xq <= xq2 )
	{
		return 0;
	}

	return 1;
}

/**********************************************************************
 * ��  �� ����  pub_eod_chkMonth
 * ���� ����:   
 *		��ִ�м��
 *             
 * ����/ʱ�䣺  power//2004��1��14��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date		ϵͳ����
 *	      char *pd_month		�����ڵڼ�����
 *            char *exec_beg		���ڿ�ʼִ������
 *	      char *exec_end		������ִֹ������
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
pub_eod_chkMonth( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* �����ڵĵڼ����� */
	long	lDay;		/* ���ڵĵڼ��� */
	int	ret;

	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );

	/* �������ĩ�����ж� */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* ���������ĩ */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}

/**********************************************************************
 * ��  �� ����  pub_eod_chkQuarter
 * ���� ����:   
 *		����ִ�м��
 *             
 * ����/ʱ�䣺  power//2004��1��14��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date		ϵͳ����
 *	      char *pd_month		�����ڵڼ�����
 *            char *exec_beg		���ڿ�ʼִ������
 *	      char *exec_end		������ִֹ������
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
pub_eod_chkQuarter( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* �����ڵĵڼ����� */
	long	lDay;		/* ���ڵĵڼ��� */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	/* �ж��Ǽ��ȵĵڼ����� */
	lMonth = pub_base_month_quarter( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* �������ĩ�����ж� */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* �ж����·ݵĵڼ��� */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * ��  �� ����  pub_eod_chkHalf
 * ���� ����:   
 *		����ִ�м��
 *             
 * ����/ʱ�䣺  power//2004��1��14��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date		ϵͳ����
 *	      char *pd_month		�����ڵڼ�����
 *            char *exec_beg		���ڿ�ʼִ������
 *	      char *exec_end		������ִֹ������
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
pub_eod_chkHalf( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* �����ڵĵڼ����� */
	long	lDay;		/* ���ڵĵڼ��� */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	/* �ж��ǰ���ĵڼ����� */
	lMonth = pub_base_month_half( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* �������ĩ�����ж� */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* �ж����·ݵĵڼ��� */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * ��  �� ����  pub_eod_chkYear
 * ���� ����:   
 *		��ִ�м��
 *             
 * ����/ʱ�䣺  power//2004��1��14��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date		ϵͳ����
 *	      char *pd_month		�����ڵڼ�����
 *            char *exec_beg		���ڿ�ʼִ������
 *	      char *exec_end		������ִֹ������
 *
 *     �����  
 *
 *   ����ֵ��  0:����ִ��1������ִ��
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
pub_eod_chkYear( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* �����ڵĵڼ����� */
	long	lDay;		/* ���ڵĵڼ��� */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	sprintf( acErrMsg, " lBeg=[%ld],lEnd=[%ld], lMon=[%ld]", lBeg, lEnd, lMon );
	WRITEMSG
	
	/* �ж��ǰ���ĵڼ����� */
	lMonth = pub_base_month_year( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* �������ĩ�����ж� */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )
		{
			sprintf( acErrMsg, " aaaaaaaaaaaaaa ret=[%d]",ret );
	WRITEMSG
			return 1;
		}
		else
		{
			sprintf( acErrMsg, " lBeg=[%ld],lEnd=[%ld], lMon=[%ld]", lBeg, lEnd, lMon );
	WRITEMSG
			return 0;
		}
	}
	
	/* �ж����·ݵĵڼ��� */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * ��  �� ����  pub_eod_chkHoliday
 * ���� ����:   
 *		���ڼ��գ��ڼ����Ƿ�ִ��
 *             
 * ����/ʱ�䣺  power//2004��1��18��
 * 
 * ��  ����
 *     ���룺  
 *	      long sys_date		ϵͳ����
 *
 *     �����  
 *
 *   ����ֵ��  0:���ڽڼ��շ�Χ  1���ڽڼ��շ�Χ -1:ִ�д���
 *           
 * �޸���ʷ��  �����޸��ˡ�ʱ�䡢�޸����ݣ���ʱ�䵹��ʽ
 *
********************************************************************/
int	pub_eod_chkHoliday( long sys_date )
{
	struct com_holiday_c sComHoliday;	/* �ڼ��� */
	int	ret;
	
	memset( &sComHoliday, 0x00, sizeof( struct com_holiday_c ) );
	
	/* ���ҽڼ����ļ���������������ڣ���ֱ�ӷ�����ȷ */
	ret = Com_holiday_Sel( g_pub_tx.reply , &sComHoliday , "beg_date <= %ld \
				and end_date >= %ld" , sys_date, sys_date );
	if ( ret == 100 )
	{
		return 0;	
	}else if ( ret )
	{
		sprintf( acErrMsg, "���ҽڼ��մ�[%d]", ret );
		WRITEMSG
		return(-1);	
	}
	
	return(1);
}

