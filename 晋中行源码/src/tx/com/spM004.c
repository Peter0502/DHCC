/*************************************************
* �� �� ��:  spM004.c
* ���������� �ֽ�����ӽ���	[����]
*
* ��    ��:  andy
* ������ڣ� 2004��3��12��
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
#include "com_tel_connect_c.h"

int spM004()
{
	int ret;
	char teller[6];

	struct	com_tel_connect_c	g_com_tel_connect;
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));

	/*ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ��*/
	pub_base_sysinit();

	get_zd_data("0920",teller);					/* �Է���Ա */ 

	strcpy(g_pub_tx.add_ind,"1");				/* ����־ */
	if( !strcmp(g_pub_tx.tel,teller) )
	{
		sprintf(acErrMsg,"ֻ����������Ա������!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O055" );
		goto ErrExit;
	}

	ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect,
		"tel='%s' and tw_tel='%s' and ind='A'",teller,g_pub_tx.tel);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ա���ӵǼǱ��в����ڸñ��ֽ�֧��!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O054" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���!ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}

	strcpy( g_pub_tx.cur_no,g_com_tel_connect.cur_no );
	g_pub_tx.tx_amt1 = g_com_tel_connect.amt;
	strcpy( g_pub_tx.ct_ind, "1" );    /*��ת1-��2-ת*/

	ret = pub_acct_cash();
	if( ret )
	{
		sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	/*�޸ĵǼǱ�״̬*/
	ret = sql_execute("update com_tel_connect set ind='1' where \
				tel='%s' and tw_tel='%s' and ind='A'",teller,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"�޸ĵǼǱ�״̬ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.cur_no, "01" );	/* ���� */
	strcpy( g_pub_tx.ac_no, "10104" );	/* ��;������ֽ� */
	strcpy( g_pub_tx.ct_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.hst_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.brf, "�ֽ����" );
	g_pub_tx.tx_amt1 = g_com_tel_connect.amt;
	strcpy(g_pub_tx.add_ind,"0");				/* ����־ */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("1181",g_com_tel_connect.cur_no);
	set_zd_double("1184",g_com_tel_connect.amt);

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

