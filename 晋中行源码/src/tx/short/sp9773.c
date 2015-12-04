/*************************************************
* �� �� ��:  sp9773.c
* ���������� ֱ���׻��ӽ���
*			 ��������������������
*			 ���������������������
*
* ��ڲ�����flag_amt	���㷽��
*						1����������������������
*						2����������������������
*			buy_curno	�������
*			sale_curno	��������
*			buy_acc		�����Ƽ�
*			sale_acc	�����Ƽ�
* 			buy_amt		������
*			��sale_amt	�������
*
* ���ڲ�����buy_amt		������
*			��sale_amt	�������
*
* ��    ��:  jane
* ������ڣ� 2003��4��15��
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

int sp9773()
{
	char flag_amt[2];
	double buy_acc,sale_acc,buy_amt,sale_amt,RMB_amt;
	char buy_curno[3],sale_curno[3];

	int	ret;

	struct com_exch_rate_c buy_exch_rate;
	struct com_exch_rate_c sale_exch_rate;

	pub_base_sysinit();

	get_zd_data("0690",flag_amt);	/*���㷽��*/
	get_zd_data("0210",buy_curno);	/*�������*/
	get_zd_data	("0220",sale_curno);/*��������*/
	get_zd_double("0961",&buy_acc);	/*�����Ƽ�*/
	get_zd_double("0962",&sale_acc);/*�����Ƽ�*/
	get_zd_double("0400",&buy_amt);	/*������*/
	get_zd_double("0410",&sale_amt);/*�������*/

	if( (pub_base_CompDblVal(buy_amt,0.00)==0 )&&(pub_base_CompDblVal(sale_amt,0.00)==0 ))
	{
		strcpy( g_pub_tx.reply, "W700" );
		sprintf( acErrMsg, "����������Ϊ0");
		WRITEMSG
		goto ErrExit;
	}

	if( !strcmp( buy_curno,RMB )||!strcmp( sale_curno,RMB) )
	{
		strcpy( g_pub_tx.reply, "W703" );
		sprintf( acErrMsg, "���ֲ����������");
		WRITEMSG
		goto ErrExit;
	}

	/*�������*/
	memset(&buy_exch_rate,0x00,sizeof(struct com_exch_rate_c));
	memset(&sale_exch_rate,0x00,sizeof(struct com_exch_rate_c));

	/*�������-��������� �ó������ odbuy*/
/**********JYW
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&buy_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and date<='%ld' and last_date>='%ld'",buy_curno,RMB,g_pub_tx.tx_date,g_pub_tx.tx_date);
***********/
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&buy_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and \"date\"<='%ld'",buy_curno,RMB,g_pub_tx.tx_date);
	if ( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ�������ʱ����!curno1[%s]curno2[%s]",buy_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf( acErrMsg, "��ѯ�������ʱ��¼������!curno1[%s]curno2[%s]",buy_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	buy_exch_rate.od_buy /= buy_exch_rate.unit;

	if( buy_acc>0.00005 &&
		( ( buy_acc-buy_exch_rate.od_buy*100.0)>0.00005 ||
		  ( buy_acc-buy_exch_rate.od_buy*100.0)<-0.00005 ) )/*��Ļ�����������Ƽ۲��Ҳ����ڵ��ջ���*/
	{
		/*��Ҫ����ҵ����Ȩ,��ʱ����*/
		strcpy( g_pub_tx.reply, "W702" );
		sprintf( acErrMsg, "��Ҫ����ҵ����Ȩ����ʱ����");
		WRITEMSG
		goto ErrExit;
	}
	else if( buy_acc<0.00005 )
	{
		buy_acc=buy_exch_rate.od_buy*100.0;
	}

	/*���������-������� �ó������� odsale*/
/************JYW
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&sale_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and date<='%ld' and last_date>='%ld'",sale_curno,RMB,g_pub_tx.tx_date,g_pub_tx.tx_date);
*************/
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&sale_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and \"date\"<='%ld' ",sale_curno,RMB,g_pub_tx.tx_date);
	if ( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ�������ʱ����!curno1[%s]curno2[%s]",sale_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf( acErrMsg, "��ѯ�������ʱ��¼������!curno1[%s]curno2[%s]",sale_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	sale_exch_rate.od_sale /= sale_exch_rate.unit;

	if( sale_acc>0.00005 &&
		( ( sale_acc-sale_exch_rate.od_sale*100.0)>0.00005 ||
		  ( sale_acc-sale_exch_rate.od_sale*100.0)<-0.00005 ) )/*��Ļ�����������Ƽ۲��Ҳ����ڵ��ջ���*/
	{
		/*��Ҫ����ҵ����Ȩ,��ʱ����*/
		strcpy( g_pub_tx.reply, "W702" );
		sprintf( acErrMsg, "��Ҫ����ҵ����Ȩ����ʱ����");
		WRITEMSG
		goto ErrExit;
	}
	else if( sale_acc<0.00005 )
	{
		sale_acc=sale_exch_rate.od_sale*100.0;
	}

	if(flag_amt[0]=='1')/*��������������������*/
	{
		vtcp_log("һ���׻��������������������");
		RMB_amt=buy_amt*buy_acc/100.0;
		vtcp_log("RMB_amt[%lf]=buy_amt[%lf]*buy_acc[%lf]/100",
				buy_acc,buy_amt,RMB_amt);
		sale_amt=RMB_amt/sale_acc*100.0;
		vtcp_log("sale_amt[%lf]=RMB_amt[%lf]/sale_acc[%lf]*100",
				sale_amt,RMB_amt,sale_acc);
		set_zd_double("0410",sale_amt);	/*�������*/
	}
	else if(flag_amt[0]=='2')/*��������������������*/
	{
		vtcp_log("һ���׻��������������������");
		RMB_amt=sale_amt*sale_acc/100.0;
		vtcp_log("RMB_amt[%lf]=sale_amt[%lf]*sale_acc[%lf]/100",
				RMB_amt,sale_amt,sale_acc);
		buy_amt=RMB_amt/buy_acc*100.0;
		vtcp_log("buy_amt[%lf]=RMB_amt[%lf]/buy_acc[%lf]*100",
				buy_amt,RMB_amt,buy_acc);
		set_zd_double("0400",buy_amt);	/*������*/
	}

	set_zd_double("0961",buy_acc);
	set_zd_double("0962",sale_acc);

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
