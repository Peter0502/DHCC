/*************************************************
* 文件名:    pubf_com_date.c
* 功能描述： 日终处理主控日期检查函数族
*           
*
* 作   者:   jack	 
* 完成日期： 2004年1月13日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "math.h"
#include "com_holiday_c.h"

/**********************************************************************
 * 函  数 名：  pub_eod_chkSysdate
 * 函数 功能:   
 *		检查程序执行日期是否是规定的期限之内
 * 		0：每天；1：5日；2：每周；3：每旬；4：每月；5：每季；
 *		6：每半年；7：每年             
 * 作者/时间：  2004年1月13日
 * 
 * 参  数：
 *     输入：  
 *		long sysdate,		系统日终日期
 *		char *exec_pd,		执行周期
 *		char *pd_month,		周期内的第几个月
 *		char *exec_beg,		每月的开始日期
 *		char *exec_end 		每月的终止日期
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
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
		case '0':	/* 每日，不用检查 */
			ret = 0;
			break;			
		case '1':	/* 5日 */
			ret = pub_eod_chkFive( sysdate, exec_beg, exec_end );
			sprintf( acErrMsg, "ret=[%d]",ret);		WRITEMSG
			if ( ret )
			{
				sprintf( acErrMsg, "执行5日检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '2':	/* 每周 */

			ret = pub_eod_chkWeek( sysdate, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每周检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '3':	/* 每旬 */

			ret = pub_eod_chkXun( sysdate, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每旬检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '4':	/* 每月 */
			ret = pub_eod_chkMonth( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每月检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '5':	/* 每季 */
			ret = pub_eod_chkQuarter( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每季检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '6':	/* 每半年 */
			ret = pub_eod_chkHalf( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每半年检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		case '7':	/* 每年 */
			ret = pub_eod_chkYear( sysdate, pd_month, exec_beg, exec_end );
			if ( ret )
			{
				sprintf( acErrMsg, "执行每年检查错" );
				WRITEMSG
				return(-1);	
			}
			break;
		default:	/* 出错 */
			sprintf( acErrMsg, "不存在此执行周期[%s]", exec_pd );
			WRITEMSG
			strcpy( g_pub_tx.reply, "G004" );
			return(-1);		
	}
	return(0);	
}
/**********************************************************************
 * 函  数 名：  pub_eod_chkFive
 * 函数 功能:   
 *		检查是五天中的第几天
 *             
 * 作者/时间：  2004年1月13日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
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
 * 函  数 名：  pub_eod_chkWeek
 * 函数 功能:   
 *		周执行检查
 *             
 * 作者/时间：  2004年1月13日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
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
 * 函  数 名：  pub_eod_chkXun
 * 函数 功能:   
 *		旬执行检查
 *             
 * 作者/时间：  2004年1月13日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date
 *            char *exec_beg
 *	      char *exec_end
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
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

	/* 如果是旬末，则判断 */
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
 * 函  数 名：  pub_eod_chkMonth
 * 函数 功能:   
 *		月执行检查
 *             
 * 作者/时间：  power//2004年1月14日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date		系统日期
 *	      char *pd_month		周期内第几个月
 *            char *exec_beg		月内开始执行日期
 *	      char *exec_end		月内终止执行日期
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
pub_eod_chkMonth( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* 周期内的第几个月 */
	long	lDay;		/* 月内的第几天 */
	int	ret;

	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );

	/* 如果是月末，则判断 */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* 如果不是月末 */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}

/**********************************************************************
 * 函  数 名：  pub_eod_chkQuarter
 * 函数 功能:   
 *		季度执行检查
 *             
 * 作者/时间：  power//2004年1月14日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date		系统日期
 *	      char *pd_month		周期内第几个月
 *            char *exec_beg		月内开始执行日期
 *	      char *exec_end		月内终止执行日期
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
pub_eod_chkQuarter( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* 周期内的第几个月 */
	long	lDay;		/* 月内的第几天 */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	/* 判断是季度的第几个月 */
	lMonth = pub_base_month_quarter( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* 如果是月末，则判断 */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* 判断是月份的第几天 */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * 函  数 名：  pub_eod_chkHalf
 * 函数 功能:   
 *		半年执行检查
 *             
 * 作者/时间：  power//2004年1月14日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date		系统日期
 *	      char *pd_month		周期内第几个月
 *            char *exec_beg		月内开始执行日期
 *	      char *exec_end		月内终止执行日期
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
pub_eod_chkHalf( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* 周期内的第几个月 */
	long	lDay;		/* 月内的第几天 */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	/* 判断是半年的第几个月 */
	lMonth = pub_base_month_half( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* 如果是月末，则判断 */
	if ( lBeg == 99 && lEnd == 99 )
	{
		ret = pub_base_end_month( sys_date );
		if ( ret )	return 1;
		else 	return 0;
	}
	
	/* 判断是月份的第几天 */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * 函  数 名：  pub_eod_chkYear
 * 函数 功能:   
 *		年执行检查
 *             
 * 作者/时间：  power//2004年1月14日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date		系统日期
 *	      char *pd_month		周期内第几个月
 *            char *exec_beg		月内开始执行日期
 *	      char *exec_end		月内终止执行日期
 *
 *     输出：  
 *
 *   返回值：  0:可以执行1：不可执行
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
pub_eod_chkYear( 
		long sys_date, 
		char *pd_month, 
		char *exec_beg, 
		char *exec_end )
{
	long	lMon,lBeg,lEnd;
	long	lMonth;		/* 周期内的第几个月 */
	long	lDay;		/* 月内的第几天 */
	int	ret;


	lBeg = atol( exec_beg );
	lEnd = atol( exec_end );
	lMon = atol( pd_month );

	sprintf( acErrMsg, " lBeg=[%ld],lEnd=[%ld], lMon=[%ld]", lBeg, lEnd, lMon );
	WRITEMSG
	
	/* 判断是半年的第几个月 */
	lMonth = pub_base_month_year( sys_date );
	if( lMonth < 0 || lMonth != lMon ) return 1;
	
	/* 如果是月末，则判断 */
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
	
	/* 判断是月份的第几天 */
	lDay = pub_base_day_month( sys_date );
	if( lBeg <= lDay && lDay <= lEnd )	return 0;
	else	return 1;
}


/**********************************************************************
 * 函  数 名：  pub_eod_chkHoliday
 * 函数 功能:   
 *		检查节假日，节假日是否执行
 *             
 * 作者/时间：  power//2004年1月18日
 * 
 * 参  数：
 *     输入：  
 *	      long sys_date		系统日期
 *
 *     输出：  
 *
 *   返回值：  0:不在节假日范围  1：在节假日范围 -1:执行错误
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int	pub_eod_chkHoliday( long sys_date )
{
	struct com_holiday_c sComHoliday;	/* 节假日 */
	int	ret;
	
	memset( &sComHoliday, 0x00, sizeof( struct com_holiday_c ) );
	
	/* 查找节假日文件，如果不在区间内，则直接返回正确 */
	ret = Com_holiday_Sel( g_pub_tx.reply , &sComHoliday , "beg_date <= %ld \
				and end_date >= %ld" , sys_date, sys_date );
	if ( ret == 100 )
	{
		return 0;	
	}else if ( ret )
	{
		sprintf( acErrMsg, "查找节假日错[%d]", ret );
		WRITEMSG
		return(-1);	
	}
	
	return(1);
}

