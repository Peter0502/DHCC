#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_base_number.c
* ����������   
*              pub_base_GetDigit     16���Ƶ�����ת��Ϊ10����
*              pub_base_Combine      ��λ16���Ƶ�����ת��Ϊ10����
*              pub_base_PubDround_1  ��Ϣ�������� 
*              pub_base_CompDblVal   �Ƚ�����DOUBLE�����ݴ�С
*              pub_base_Supper_CompDblVal   �Ƚ�����DOUBLE�����ݴ�С
*              str2dbl(ibuf,ibuflen,idot,obuf); ��ָ�����Ƚ��ַ���������ת����double��
* 
* �������ܣ� 
* ��    ��:
* ������ڣ�   2003��12��16��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
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
 * �� �� ����  pub_base_GetDigit
 * �������ܣ�  16���Ƶ�����ת��Ϊ10����
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 * 
 * ��    ����
 *     ��  �룺ch��  16���Ƶ�����
 *      
 *     ��  ������
 *
 *     ����ֵ��10���Ƶ�����
 *
 * ע    �ͣ�  '2'  ==> 2     
 *             'f' == > 15
 *          
 * �޸���ʷ��
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
 * �� �� ����  pub_base_Combine
 * �������ܣ�  ��λ16���Ƶ�����ת��Ϊ10����
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺Twochar:  16���Ƶ�����
 *      
 *     ��  ������
 *
 *     ����ֵ��10���Ƶ�����
 *
 * ע    �ͣ�  "12" ==> 0x 12     
 *             "ef" == 0x ef
 *
 * �޸���ʷ��
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
 * �� �� ����  pub_base_PubDround_1
 * �������ܣ�  ��Ϣ��������
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺lx:  ��Ϣ
 *      
 *     ��  ������
 *
 *     ����ֵ��lx:  �����������봦������Ϣ
 *
 * �޸���ʷ��
 *                   
********************************************************************/

double pub_base_PubDround_1 ( double lx ) /*��Ϣ��������*/
{
	double llx=0.00;
	int i;
	char clx[40];
/*
	vtcp_log("[%s][%d]��Ϣ�������뺯�����:lx=[%f]",__FILE__,__LINE__,lx);
	vtcp_log("���:lx=[%lf]",lx);
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
						rstod(clx, &llx);						/****ԭ�������ڴ�������ʱ�򣬳�����������ʱδ��λ��������������ݳ��ִ��� modify at 20131104****/
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
						/****ԭ�������ڴ�������ʱ�򣬳�����������ʱδ��λ��������������ݳ��ִ��� modify at 20131104****/
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
 * �� �� ����  pub_base_CompDblVal
 * �������ܣ�  �Ƚ�����DOUBLE�����ݴ�С
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺amt1:  ��һ��DOUBLE������
 *             amt2:  �ڶ���DOUBLE������
 *      
 *     ��  ������
 *
 *     ����ֵ��0  ���
 *	           1  ��һ�����ڵڶ���
 *		      -1  ��һ��С�ڵڶ���
 *
 * �޸���ʷ��
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
 * �� �� ����  pub_base_Supper_CompDblVal
 * �������ܣ�  �Ƚ�����DOUBLE�����ݴ�С
 * ��    �ߣ�  
 * ���ʱ�䣺  2005��03��24��
 *
 * ��    ����
 *     ��  �룺amt1:  ��һ��DOUBLE������
 *             amt2:  �ڶ���DOUBLE������
 *             amt3:  ������DOUBLE������
 *      
 *     ��  �������ı���
 *
 *     ����ֵ��-1 ʧ��
 *
 * �޸���ʷ��
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
 * �� �� ����  pub_base_floor()
 * �������ܣ�  
 * ��    �ߣ�  
 * ���ʱ�䣺  2005��03��24��
 *
 * ��    ����
 *     ��  �룺number:  ��һ��DOUBLE������
 *      
 *     ��  ����
 *
 *     ����ֵ��
 *
 * �޸���ʷ��
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
	   vtcp_log("floor����:����[%f],����[%f]\n",a,b);
        return(b);
}



/* ���� ������� �� ���տ������ ���� ʵ����� FD74 */
int iSetAdditional_Amounts(double dAccount_amt, double dAvailable_amt )
{
	/*
	�˻�����	�������	���Ҵ���	������	���	�˻�����	�������	���Ҵ���	������	���
   n2					n2				an3				an1			n12		n2				n2				an3					an1			n12
	*/
	char cFD74[41];
	char cAmount[13];
	
	if(dAccount_amt < 0 || dAvailable_amt < 0)
	{
     vtcp_log("������!! dAccount_amt==[%f] dAvailable_amt==[%f]", dAccount_amt, dAvailable_amt);
		 return -1;
	}
	if(dAccount_amt >= 10000000000.0 || dAvailable_amt >= 10000000000.0 )
	{
     vtcp_log("����!!");     
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


