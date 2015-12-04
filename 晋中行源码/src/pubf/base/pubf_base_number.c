#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_base_number.c
* 功能描述：   
*              pub_base_GetDigit     16进制的数字转化为10进制
*              pub_base_Combine      两位16进制的数字转化为10进制
*              pub_base_PubDround_1  利息四舍五入 
*              pub_base_CompDblVal   比较两个DOUBLE型数据大小
*              pub_base_Supper_CompDblVal   比较三个DOUBLE型数据大小
*              str2dbl(ibuf,ibuflen,idot,obuf); 按指定长度将字符串的内容转换成double型
* 
* 函数功能： 
* 作    者:
* 完成日期：   2003年12月16日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/

# include	<sys/types.h>
# include	<stdio.h>
# include	<time.h>
# include   <math.h>

extern double floor(double);

static double D005 = 0.005;
static long   LONE = 1;
static long   L100 = 100;
static double DZERO = 0.00;

/**********************************************************************
 * 函 数 名：  pub_base_GetDigit
 * 函数功能：  16进制的数字转化为10进制
 * 作    者：  
 * 完成时间：  2003年12月16日
 * 
 * 参    数：
 *     输  入：ch：  16进制的数字
 *      
 *     输  出：无
 *
 *     返回值：10进制的数字
 *
 * 注    释：  '2'  ==> 2     
 *             'f' == > 15
 *          
 * 修改历史：
 *                   
***********************************************************************/

unsigned char pub_base_GetDigit(unsigned char ch)
{
	if ( ch >= '0' && ch <= '9' )
		return ch - '0' ;
	else if ( ch >= 'a' && ch <= 'f' )
			return ch - 'a' + 10 ;
		else if ( ch >= 'A' && ch <= 'F' )
				return ch - 'A' + 10 ;
			else
				 return 0;
}

/**********************************************************************
 * 函 数 名：  pub_base_Combine
 * 函数功能：  两位16进制的数字转化为10进制
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：Twochar:  16进制的数字
 *      
 *     输  出：无
 *
 *     返回值：10进制的数字
 *
 * 注    释：  "12" ==> 0x 12     
 *             "ef" == 0x ef
 *
 * 修改历史：
 *                   
********************************************************************/

unsigned char pub_base_Combine( unsigned char *Twochar)
{
	 unsigned char ch ;

	 ch  = ( ( ( pub_base_GetDigit (Twochar[0]) ) << 4 ) & 0xf0  ) +
			( ( pub_base_GetDigit (Twochar[1]) )  & 0x0f  ) ;

	 return ch ;

}


/**********************************************************************
 * 函 数 名：  pub_base_PubDround_1
 * 函数功能：  利息四舍五入
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：lx:  利息
 *      
 *     输  出：无
 *
 *     返回值：lx:  经过四舍五入处理后的利息
 *
 * 修改历史：
 *                   
********************************************************************/

double pub_base_PubDround_1 ( double lx ) /*利息四舍五入*/
{
	double llx=0.00;
	int i;
	char clx[40];
/*
	vtcp_log("[%s][%d]利息四舍五入函数入口:lx=[%f]",__FILE__,__LINE__,lx);
	vtcp_log("入口:lx=[%lf]",lx);
*/
	MEMSET_DEBUG( clx,0,sizeof(clx) );

    if (lx >= 0.005 || lx <= -0.005)
	{
        if (lx < D005 ) 
		{
			sprintf(clx,"%.5lf",lx);
			for(i=1;i<=strlen(clx);i++)
			{
				if(clx[i]=='.')
				{
					if(clx[i+3] >='5') {
						clx[i+3]='0';
						clx[i+4]='0';
						clx[i+5]='0';
						rstod(clx, &llx);						/****原处理方法在处理负数的时候，出现四舍五入时未进位的情况，导致数据出现错误 modify at 20131104****/
						if(lx <= -0.005)
						{
							llx -= 0.01;
						}else{
							llx += 0.01;
						}
						/****modify end 20131104****/
					}
					else {
						clx[i+3]='0';
						clx[i+4]='0';
						clx[i+5]='0';
						rstod(clx, &llx);
					}
					break;
				}
			}
        }
		else
		{
			sprintf(clx,"%.5lf",lx);
			for(i=0;i<=strlen(clx);i++)
			{
				if(clx[i]=='.')
				{
					if(clx[i+3] >='5') {
						clx[i+3]='0';
						clx[i+4]='0';
						clx[i+5]='0';
						rstod(clx, &llx);
						/****原处理方法在处理负数的时候，出现四舍五入时未进位的情况，导致数据出现错误 modify at 20131104****/
						if(lx <= -0.005)
						{
							llx -= 0.01;
						}else{
							llx += 0.01;
						}
						/****modify end 20131104****/
					}
					else {
						clx[i+3]='0';
						clx[i+4]='0';
						clx[i+5]='0';
						rstod(clx, &llx);					}
					break;
				}
			}
		}
    }
    else 
	{
		llx=DZERO;
	}
	vtcp_log("[%s][%d]ttooooooootttttclx=[%s]\n",__FILE__,__LINE__,clx);
    return llx;
}


/**********************************************************************
 * 函 数 名：  pub_base_CompDblVal
 * 函数功能：  比较两个DOUBLE型数据大小
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：amt1:  第一个DOUBLE型数据
 *             amt2:  第二个DOUBLE型数据
 *      
 *     输  出：无
 *
 *     返回值：0  相等
 *	           1  第一个大于第二个
 *		      -1  第一个小于第二个
 *
 * 修改历史：
 *                   
********************************************************************/

pub_base_CompDblVal(amt1,amt2)
double amt1,amt2;
{
    if(amt1-amt2>0.005) return 1;
    if(amt2-amt1>0.005) return -1;

    return (0);
}

/**********************************************************************
 * 函 数 名：  pub_base_Supper_CompDblVal
 * 函数功能：  比较三个DOUBLE型数据大小
 * 作    者：  
 * 完成时间：  2005年03月24日
 *
 * 参    数：
 *     输  入：amt1:  第一个DOUBLE型数据
 *             amt2:  第二个DOUBLE型数据
 *             amt3:  第三个DOUBLE型数据
 *      
 *     输  出：最大的变量
 *
 *     返回值：-1 失败
 *
 * 修改历史：
 *                   
********************************************************************/
double pub_base_Supper_CompDblVal(amt1,amt2,amt3)
double amt1,amt2,amt3;
{
	int ret=0;
	int ret1=0;

	ret=pub_base_CompDblVal(amt1,amt2);
	if( ret>=0 )
	{
		ret1=pub_base_CompDblVal(amt1,amt3);
		if( ret1>=0 )
			return amt1;
		else
			return amt3;
	}
	else
	{
		ret1=pub_base_CompDblVal(amt2,amt3);
		if( ret1>=0 )
			return amt2;
		else
			return amt3;
	}
	
    return (-1);
}

/**********************************************************************
 * 函 数 名：  pub_base_floor()
 * 函数功能：  
 * 作    者：  
 * 完成时间：  2005年03月24日
 *
 * 参    数：
 *     输  入：number:  第一个DOUBLE型数据
 *      
 *     输  出：
 *
 *     返回值：
 *
 * 修改历史：
 *                   
********************************************************************/
double pub_base_floor(double number)
{
	if (pub_base_CompDblVal(number, 0.00) == 1)
	{
		return( floor(number) );
	}
	else
	{
		return( -floor(-number) );
	}
}

/****************************************************************************/
/*  OBJECT :  string  to  double                                            */
/*  INPUT  :  ibuf      ->  456123                                          */
/*            ibuflen   ->  6                                               */
/*            idot      ->  3      express string = 456.123                 */
/*  OUTPUT :  obuf                                                          */
/*  RETURN :  0         ->  sucessed                                        */
/*            -1        ->  failed                                          */
/****************************************************************************/
int str2dbl( ibuf,ibuflen,idot,obuf )
char  *ibuf       ;
short ibuflen     ;
short idot        ;
double *obuf       ;
{
	char lstrbuf[36];
	short loopi ;
	MEMSET_DEBUG(lstrbuf, 0 , sizeof(lstrbuf));
	/*********** chech input data ****************/
	if( strlen(ibuf) < ibuflen )
		return -1;
	if( strlen(ibuf) < idot )
		return -1;
	if( idot > ibuflen )
		return -1;
	/********** add dot to string *****************/
	for( loopi=0;loopi<ibuflen+1;loopi++ )
	{
		if( loopi == (ibuflen-idot) )
			lstrbuf[loopi] = '.' ;
		else
			lstrbuf[loopi] = *ibuf++ ;
	}
	lstrbuf[loopi] = 0x00 ;
	/********* string to decimal *********************/
	*obuf=atof(lstrbuf);
	return 0;
}

double floor(double a)
{
		char num[51];
		double b = 0;

		MEMSET_DEBUG(num,0,sizeof(num));
		sprintf(num,"%.0f",a);
		b = atof(num);
        if(b<0)
        {
                b-=1;
        }
	   vtcp_log("floor函数:传入[%f],传出[%f]\n",a,b);
        return(b);
}



/* 根据 账面余额 和 当日可用余额 设置 实际余额 FD74 */
int iSetAdditional_Amounts(double dAccount_amt, double dAvailable_amt )
{
	/*
	账户类型	余额类型	货币代码	余额符号	余额	账户类型	余额类型	货币代码	余额符号	余额
   n2					n2				an3				an1			n12		n2				n2				an3					an1			n12
	*/
	char cFD74[41];
	char cAmount[13];
	
	if(dAccount_amt < 0 || dAvailable_amt < 0)
	{
     vtcp_log("金额错误!! dAccount_amt==[%f] dAvailable_amt==[%f]", dAccount_amt, dAvailable_amt);
		 return -1;
	}
	if(dAccount_amt >= 10000000000.0 || dAvailable_amt >= 10000000000.0 )
	{
     vtcp_log("金额超大!!");     
		 return -1;
	}	
	
	MEMSET_DEBUG(cFD74, 0, sizeof(cFD74) );
	MEMSET_DEBUG(cAmount, 0, sizeof(cAmount) );
	
	MEMCPY_DEBUG(cFD74, "10", 2);
	MEMCPY_DEBUG(cFD74+2, "01", 2);
	MEMCPY_DEBUG(cFD74+4, "156", 3);	
	MEMCPY_DEBUG(cFD74+7, "D", 1);
	
	iDblToStr_12(dAccount_amt, cAmount);	
	MEMCPY_DEBUG(cFD74+8, cAmount, 12);	
	
	
	MEMCPY_DEBUG(cFD74+20, "10", 2);
	MEMCPY_DEBUG(cFD74+22, "01", 2);
	MEMCPY_DEBUG(cFD74+24, "156", 3);	
	MEMCPY_DEBUG(cFD74+27, "D", 1);
	
	MEMSET_DEBUG(cAmount, '0', sizeof(cAmount) );	
	cAmount[sizeof(cAmount)-1] = 0;
	iDblToStr_12(dAvailable_amt, cAmount);	
	MEMCPY_DEBUG(cFD74+28, cAmount, 12);	
	
	cFD74[40] = 0;
	set_zd_data("0740", cFD74);	
		
	return 0;
}


