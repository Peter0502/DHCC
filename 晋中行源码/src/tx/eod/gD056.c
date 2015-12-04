/*************************************************************
* �� �� ��: gD056.c
* ����������͸֧�ļ� ->͸֧�ʵ�
*
* ��    ��: lance
* �������: 2003��04��01��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "od_bill_c.h"

gD056()
{
	int    ret = 0;
	char   tmp_str[9],tmp_str1[3];
	int	   tmp_int;
	double tmp_amt;
	long   tmp_date;

	struct  od_bill_c	g_od_bill;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	memset(tmp_str,0,sizeof(tmp_str));
	memset(tmp_str1,0,sizeof(tmp_str1));
	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_bill,0x00,sizeof(struct od_bill_c));

	sprintf(tmp_str,"%8ld",g_pub_tx.tx_date);
	strncpy(tmp_str1,tmp_str+6,2);
	if(tmp_str1[0]=='0')
	{
		memset(tmp_str,0,sizeof(tmp_str));
		strncpy(tmp_str,tmp_str1+1,1);
		tmp_int=atoi(tmp_str);
	}
	else tmp_int=atoi(tmp_str1);

		sprintf(acErrMsg,"tmp_int=[%d]",tmp_int);
		WRITEMSG

	ret = Od_parm_Dec_Sel( g_pub_tx.reply," pay_date=%d ",tmp_int);
	if( ret )
	{
		sprintf(acErrMsg,"�����α��쳣[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

TRACE
	while(1)
	{
		ret=Od_parm_Fet_Sel(&g_od_parm,g_pub_tx.reply);
		if(ret==100)break;
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ͸֧��Ʒ�����쳣[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

TRACE
		ret=Od_mst_Dec_Upd(g_pub_tx.reply,"prdt_no='%s'",g_od_parm.prdt_no);
		if(ret)
		{
			sprintf(acErrMsg,"�����α��쳣[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D101");
			goto ErrExit;
		}

		while(1)
		{
TRACE
			ret=Od_mst_Fet_Upd(&g_od_mst,g_pub_tx.reply);
			if(ret==100)break;
			else if(ret)
			{
				sprintf(acErrMsg,"��ѯ͸֧�ļ��쳣[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}

TRACE
			if(pub_base_CompDblVal(g_od_mst.intst_acm,0.00)|| \
			   pub_base_CompDblVal(g_od_mst.bal,0.00)|| \
			   pub_base_CompDblVal(g_od_mst.intst_bal,0.00))
			{
				ret=sql_max_int("od_bill","curr_cnt","ac_id=%ld and ac_seqn=%d", 								g_od_mst.ac_id,g_od_mst.ac_seqn);
				if( ret<0 )
				{
					sprintf(acErrMsg,"����sql_max_int()�쳣[%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_od_bill.curr_cnt=ret+1;
				g_od_bill.ac_id=g_od_mst.ac_id;
				g_od_bill.ac_seqn=g_od_mst.ac_seqn;

				g_od_bill.amt=g_od_mst.intst_amt;
				g_od_bill.bal=g_od_mst.bal;
				g_od_bill.drt_bal=g_od_mst.drt_bal;
				g_od_bill.intst_acm=g_od_mst.intst_acm;
				g_od_bill.drt_intst_acm=g_od_mst.drt_intst_acm;
				g_od_bill.intst_amt=g_od_mst.intst_amt;
				g_od_bill.intst_bal=g_od_mst.intst_bal;
				g_od_bill.drt_intst_bal=g_od_mst.drt_intst_bal;

				ret=pub_base_deadlineM(g_pub_tx.tx_date,-1,&g_od_bill.beg_date);
				if(ret)
				{
					sprintf(acErrMsg,"����pub_base_deadlineM�쳣[%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_od_bill.end_date=g_pub_tx.tx_date;

				ret=pub_base_deadlineM(g_pub_tx.tx_date,g_od_parm.pay_intvl, \
										&tmp_date);
				if(ret)
				{
					sprintf(acErrMsg,"����pub_base_deadlineM�쳣[%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				ret=pub_base_deadlineD(tmp_date,g_od_parm.pay_drt_days, \
										&g_od_bill.pay_date);
				if(ret)	
				{
					sprintf(acErrMsg,"����pub_base_deadlineD�쳣[%d]",ret);
					WRITEMSG
					goto ErrExit;
				}

				g_od_bill.tx_date=g_pub_tx.tx_date;
				g_od_bill.lst_date=g_od_mst.lst_date;
				strcpy(g_od_bill.pay_sts,"N");
				strcpy(g_od_bill.curr_sts,"2");

				tmp_amt=g_od_mst.cash_bal;

				ret=Od_bill_Ins(g_od_bill,g_pub_tx.reply);
				if(ret)
				{
					sprintf(acErrMsg,"�Ǽ�͸֧�ʵ��ļ��쳣[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D101");
					goto ErrExit;
				}

				/*�޸�͸֧�ļ�*/
				memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
				g_od_mst.ac_id=g_od_bill.ac_id;
				g_od_mst.ac_seqn=g_od_bill.ac_seqn;
				g_od_mst.bal=g_od_bill.bal;
				g_od_mst.cash_bal=tmp_amt;

	  		}/*END OF IF*/

			ret=Od_mst_Upd_Upd(g_od_mst,g_pub_tx.reply);
			if(ret)
			{
				sprintf(acErrMsg,"�Ǽ�͸֧�ļ��쳣[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
				goto ErrExit;
			}

		}/*END OF INSIDE  WHILE*/

		Od_mst_Clo_Upd();

	}/*END OF OUTSIDE WHILE*/

	Od_parm_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
