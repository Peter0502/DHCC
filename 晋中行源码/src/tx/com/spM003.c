/*************************************************
* �� �� ��:  spM003.c
* ���������� �ֽ�����ӽ��סi[����]
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

int spM003()
{
	int ret;
	char teller[6];
	char prtime[9];
	int  hour,min,sec;

	struct	com_tel_connect_c	g_com_tel_connect;
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));

	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	pub_base_sysinit();
  
	get_zd_data("0920",teller);					/* �Է���Ա */ 
	get_zd_data("0210",g_pub_tx.cur_no);		/* ���� */ 
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/* ������ */

	strcpy(g_pub_tx.add_ind,"0");				/* ����־ */

	pub_base_strpack(teller);
	pub_base_strpack(g_pub_tx.tel);
	strcpy( g_pub_tx.brf, "�ֽ����" );
	vtcp_log("[%s][%d] Ray1 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	if( !strcmp(g_pub_tx.tel,teller) )
	{
		sprintf(acErrMsg,"ֻ����������Ա������!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O058" );
		goto ErrExit;
	}
	if( !strcmp(teller,"") )
	{
		sprintf(acErrMsg,"���չ�Ա����Ϊ��!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "C119" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d] Ray2 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	/*** Ϊ����ATM���峮����tx_code=9325��ʱ����0.00 ***/
	vtcp_log("[%s][%d] g_pub_tx.tx_code = [%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
  if( memcmp(g_pub_tx.tx_code,"9325",4) != 0 && pub_base_CompDblVal( g_pub_tx.tx_amt1,0.00 )==0 )
	{
		sprintf(acErrMsg,"�����븶�����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O163");
		goto ErrExit;
	}

	/*** ����֮ǰ����Ƿ����и�����δ���� ***/
	vtcp_log("[%s][%d] Ray3 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
		 "tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'"
		, g_pub_tx.tel,teller,g_pub_tx.cur_no);
	vtcp_log("[%s][%d] Ray4 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	if( ret==100 )
	{
	vtcp_log("[%s][%d] Ray6 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		strcpy(g_com_tel_connect.ind,"A");					/*δȡ״̬*/
		strcpy(g_com_tel_connect.tel,g_pub_tx.tel);			/*������Ա*/
		strcpy(g_com_tel_connect.tw_tel,teller);			/*�Է���Ա*/
		strcpy(g_com_tel_connect.br_no,g_pub_tx.tx_br_no);	/*�����*/
		strcpy(g_com_tel_connect.cur_no,g_pub_tx.cur_no);	/*�Һ�*/
		g_com_tel_connect.amt=g_pub_tx.tx_amt1;				/*�������*/
		g_com_tel_connect.tx_date=g_pub_tx.tx_date;			/*��������*/		
		g_com_tel_connect.trace_no=g_pub_tx.trace_no;		/*��ˮ��*/
		
		ret = Com_tel_connect_Ins( g_com_tel_connect,g_pub_tx.reply );
	vtcp_log("[%s][%d] Ray7 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǹ�Ա���ӵǼǱ��쳣����!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O236");	
			goto ErrExit;
		}
	}
	else if( ret==0 )
	{
		/**
		g_com_tel_connect.amt += g_pub_tx.tx_amt1;
	vtcp_log("[%s][%d] Ray5 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

		ret=sql_execute("update com_tel_connect set amt=%lf where \
			tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'", \
			g_com_tel_connect.amt,g_pub_tx.tel,teller,g_pub_tx.cur_no);
		if( ret )
		{
			sprintf(acErrMsg,"�޸Ĺ�Ա���ӵǼǱ�ʧ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O235");			
			goto ErrExit;
		}
		**/
		sprintf(acErrMsg,"�ù�Ա���и������Է���Աδ����ļ�¼");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O235");			
			goto ErrExit;
	}
	else 
	{
		sprintf(acErrMsg,"�޸Ĺ�Ա���ӵǼǱ��쳣����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O234");	
		goto ErrExit;
	}

	strcpy( g_pub_tx.ct_ind, "1" );    /*��ת1-��2-ת*/
	vtcp_log("[%s][%d] Ray8 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	ret = pub_acct_cash( );
	vtcp_log("[%s][%d] Ray9 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	if( ret )
	{
		sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.cur_no, "01" );	/* ���� */
	strcpy( g_pub_tx.ac_no, "10104" );	/* ��;������ֽ� */
	strcpy( g_pub_tx.ct_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.hst_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.brf, "�ֽ����" );
	strcpy(g_pub_tx.add_ind,"1");				/* ����־ */
	
	vtcp_log("[%s][%d] Ray10 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	

	/*��ӡʱ�� add by andy*/
	pub_base_GetTime2( &hour, &min, &sec );
	memset( prtime, 0x00, sizeof( prtime ) );
	sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*��ӡʱ��*/
	set_zd_data( "0580", prtime );
vtcp_log( "zy   prtime[%s]",prtime );
	set_zd_data("1191",g_com_tel_connect.cur_no);
	set_zd_double("1194",g_pub_tx.tx_amt1);
	vtcp_log("[%s][%d] Ray11 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

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

