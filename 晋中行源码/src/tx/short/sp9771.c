/*************************************************
* �� �� ��:  sp9771.c
* ���������� ���һ��ӽ���
*			 ������������ѯ������
*			 ����:sale_amt
*			 ���:buy_amt
*
* ��    ��:  jane
* ������ڣ� 2003��4��06��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_exch_rate_c.h"

int sp9771()
{
	double buy_acc,sale_acc,buy_amt,sale_amt;
	char buy_curno[3],sale_curno[3];

	int	ret;

	struct com_exch_rate_c exch_rate;
	
	pub_base_sysinit();

	get_zd_data("0210",buy_curno);		/*�������*/
	get_zd_data("0220",sale_curno);		/*��������*/
    get_zd_double("0961",&buy_acc);		/*�����Ƽ�*/
    get_zd_double("0962",&sale_acc);	/*�����Ƽ�*/
    get_zd_double("0410",&sale_amt);	/*�������*/

	if( pub_base_CompDblVal(sale_amt,0.00)==0)
	{
		strcpy( g_pub_tx.reply, "L071" );
		sprintf( acErrMsg, "������Ϊ0");
		WRITEMSG
		goto ErrExit;
	}

	/*�������*/
	memset(&exch_rate,0x00,sizeof(struct com_exch_rate_c));

	if( !strcmp( buy_curno,RMB ) )	/*���������-������� �ó������� odsale*/
	{
/******JYW
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and date<='%ld' and last_date>='%ld'",sale_curno,buy_curno,g_pub_tx.tx_date,g_pub_tx.tx_date);
********/
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and \"date\"<='%ld' ",sale_curno,buy_curno,g_pub_tx.tx_date);
		if ( ret&&ret!=100 )
		{
			sprintf( acErrMsg, "��ѯ�������ʱ����!curno1[%s]curno2[%s]",sale_curno,buy_curno );
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==100)
		{
			sprintf( acErrMsg, "��ѯ�������ʱ��¼������!curno1[%s]curno2[%s]",sale_curno,buy_curno );
			WRITEMSG
			goto ErrExit;
		}
		exch_rate.od_sale /= exch_rate.unit;

		if( sale_acc>0.00005 &&
			( ( sale_acc-exch_rate.od_sale*100.0)>0.00005 ||
			  ( sale_acc-exch_rate.od_sale*100.0)<-0.00005 ) )/*��Ļ�����������Ƽ۲��Ҳ����ڵ��ջ���*/
		{
			/*��Ҫ����ҵ����Ȩ,��ʱ����*/
			strcpy( g_pub_tx.reply, "W702" );
			sprintf( acErrMsg, "��Ҫ����ҵ����Ȩ����ʱ����");
			WRITEMSG
			goto ErrExit;
		}
		else if( sale_acc<0.00005 )
		{
			sale_acc=exch_rate.od_sale*100.0;
		}
		set_zd_double("0962",sale_acc);

		buy_amt=sale_amt*sale_acc/100.0;
	}
	else if( !strcmp( sale_curno,RMB ) )	/*�������-��������� �ó������ odbuy*/
	{
/**********JYW
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and date<='%ld' and last_date>='%ld'",buy_curno,sale_curno,g_pub_tx.tx_date,g_pub_tx.tx_date);
**************/
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and \"date\"<='%ld' ",buy_curno,sale_curno,g_pub_tx.tx_date);
		if ( ret&&ret!=100 )
		{
			sprintf( acErrMsg, "��ѯ�������ʱ����!curno1[%s]curno2[%s]",buy_curno,sale_curno );
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==100)
		{
			sprintf( acErrMsg, "��ѯ�������ʱ��¼������!curno1[%s]curno2[%s]",buy_curno,sale_curno );
			WRITEMSG
			goto ErrExit;
		}
		exch_rate.od_buy /= exch_rate.unit;

		if( buy_acc>0.00005 &&
			( ( buy_acc-exch_rate.od_buy*100.0)>0.00005 ||
			  ( buy_acc-exch_rate.od_buy*100.0)<-0.00005 ) )/*��Ļ�����������Ƽ۲��Ҳ����ڵ��ջ���*/
		{
			/*��Ҫ����ҵ����Ȩ,��ʱ����*/
			strcpy( g_pub_tx.reply, "W702" );
			sprintf( acErrMsg, "��Ҫ����ҵ����Ȩ����ʱ����");
			WRITEMSG
			goto ErrExit;
		}
		else if( buy_acc<0.00005 )
		{
			buy_acc=exch_rate.od_buy*100.0;
		}
		set_zd_double("0961",buy_acc);

		buy_amt=sale_amt/buy_acc*100.0;
	}
	else
	{
		strcpy( g_pub_tx.reply, "W701" );
		sprintf( acErrMsg, "������������������������ֶ������");
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("buy_acc[%lf]sale_acc[%lf]buy_amt[%lf]sale_amt[%lf]",
			  buy_acc,sale_acc,buy_amt,sale_amt);

    set_zd_double("0400",buy_amt);	/*������*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
