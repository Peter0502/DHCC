/*************************************************
* �� �� ��: pubf_ln_AcCheck.c
* ����������   
*              pub_ln_OrderPay     ���ݻ���˳����ȷ���������͵Ļ�����
*
* ��    ��:    rob
* ������ڣ�   2004��01��18��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_pay_seq_c.h"

/**********************************************************************
* �� �� ����  pub_ln_OrderPay
* �������ܣ�  ���ݻ���˳����ȷ���������͵Ļ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��01��18��
*
* ��    ���� 
*		��ڣ�������Ϣ�ṹ������˳����룬�����ܶ�
*		���ڣ�������Ϣ�ṹ��ʵ�ʹ���
*		���أ�0�ɹ�1ʧ��
*
* �޸���ʷ��
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
		strcpy( payseq.deal_mode,"0" );/**1Ϊ�����۱�־ 2Ϊ�޷����壬������������ ����������**/
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
	
		/*�������ʽΪ1(�޷�����,������)��۳�����:
				����ۼ�������,����Ϊֹ,�������سɹ�*/
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
			/*�������ʽΪ2���޷����壬���������������������سɹ�*/
			else if( payseq.deal_mode[0]=='2' )
			{
				break;
			}
			/*�������ʽΪ0���޷����壬�������������������������һ�*/
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
