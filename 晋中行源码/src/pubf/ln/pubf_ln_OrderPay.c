/*************************************************
* 文 件 名: pubf_ln_AcCheck.c
* 功能描述：   
*              pub_ln_OrderPay     根据还款顺序定义确定各种类型的还款金额
*
* 作    者:    rob
* 完成日期：   2004年01月18日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_pay_seq_c.h"

/**********************************************************************
* 函 数 名：  pub_ln_OrderPay
* 函数功能：  根据还款顺序定义确定各种类型的还款金额
* 作    者：  rob
* 完成时间：  2004年01月18日
*
* 参    数： 
*		入口：贷款利息结构，还款顺序代码，还款总额
*		出口：贷款利息结构，实际归金额
*		返回：0成功1失败
*
* 修改历史：
*                   
********************************************************************/
int pub_ln_OrderPay( struct S_ln_intst *ln_intst,
	char *seq_code,double *p_pay_amt )
{	
	struct ln_pay_seq_c payseq;
	double pay_amt,tmpamt,tmpbal;
	int ret,i,j,k;

	ret=i=j=k=0;
	pay_amt=*p_pay_amt;

	ret=Ln_pay_seq_Sel( g_pub_tx.reply,&payseq,"pay_seq_code='%s'",seq_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"NO FIND in ln_pay_seq [%s]",seq_code);
		WRITEMSG
		strcpy( payseq.pay_seqn,"ABCDEFGHIJKLMNO" );
		strcpy( payseq.deal_mode,"0" );/**1为尽量扣标志 2为无法还清，不处理，不继续 其他不处理**/
	}
	else if( ret )
	{
		sprintf(acErrMsg,"FIND in ln_pay_seq [%s] error",seq_code);
		WRITEMSG
		goto ErrExit;
	}

	for( i='A';i<='O';i++ )
	{
		tmpbal=0.00;
		for( j=0;j<LN_INTST_TYPS;j++ )
		{
			if( payseq.pay_seqn[j]==i )
			{
				for( k=0;k<LN_INTST_TYPS;k++ )
				{
					if( ln_intst[k].type==j )
					{
						tmpbal+=ln_intst[k].intst_amt;
vtcp_log("find [%d] [%c] [%.2lf]",k,i,tmpbal );
						break;
					}
				}
			}
		}
vtcp_log("tmpbal [%.2lf]",tmpbal );
		if( tmpbal==0.00 ) 
		{
			vtcp_log("zero is [%c] [%.2lf]",i,tmpbal );
			continue;
		}
	/****GXJ	
	if( tmpbal>pay_amt)
	***/	
	if(pub_base_CompDblVal(tmpbal,pay_amt)>0)
	{
	
		/*如果处理方式为1(无法还清,尽量扣)则扣除处理:
				逐项扣减各项金额,扣完为止,结束返回成功*/
			if( payseq.deal_mode[0]=='1' )
			{
				for( j=0;j<LN_INTST_TYPS;j++ )
				{
					if( payseq.pay_seqn[j]==i )
					{
						for( k=0;k<LN_INTST_TYPS;k++ )
						{
							if( ln_intst[k].type==j )
							{
					if( pay_amt-ln_intst[k].intst_amt>-0.005 )
								{
								pay_amt-=ln_intst[k].intst_amt;
									ln_intst[k].pay_amt=ln_intst[k].intst_amt;
}
								else
								{
									ln_intst[k].pay_amt=pay_amt;
									pay_amt=0.00;
								}
								break;
							}
						}	
					}
				}
				break;
			}
			/*如果处理方式为2（无法还清，不处理，不继续）结束返回成功*/
			else if( payseq.deal_mode[0]=='2' )
			{
				break;
			}
			/*如果处理方式为0（无法还清，不处理，继续），则继续处理下一项。*/
			else
			{
				;
			}
		}
		else
		{
			/*pay_amt-=tmpbal;*/
				for( j=0;j<LN_INTST_TYPS;j++ )
				{
				if( payseq.pay_seqn[j]==i )
					{
						for( k=0;k<LN_INTST_TYPS;k++ )
						{
					if( ln_intst[k].type==j )
							{
								pay_amt-=ln_intst[k].intst_amt;
								ln_intst[k].pay_amt=ln_intst[k].intst_amt;
								break;
							}
						}	
					}
				}
			if( pay_amt<0.005 ) break;
		}
	}

	*p_pay_amt=pay_amt;

	return 0;
ErrExit:
	return ret; 
}
