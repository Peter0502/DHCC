/*************************************************
* �� �� ��:  spB914.c
* ���������� ������֡�ת���֣����ת�룩�����ջصļ��˹��ܣ�
*	     �����޸����ֵǼǲ����������ȡ���ӽ���
*	    ���������������롢��ش���ӽ��ף���
*
* ��    ��:  jane
* ������ڣ� 2003��4��9��
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
#include "mo_discnt_c.h"
#include "com_parm_c.h"

int spB914()
{
	struct	mo_discnt_c		sMoDiscnt;		/* ���ֵǼǱ� */
	struct	dd_mst_c		sDdMst;			/* ���ڴ�����ļ� */
	struct 	com_parm_c 		sComParm;		/* �������� */
	char	cPayAcType[2];					/* �����ʻ����� */
	char	cLnAcNo[25];					/* �����˺� */
	int	ret;
	double	T_amt=0.00;
        
        char    tmp[20];

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );
	memset( cLnAcNo, 0x00, sizeof( cLnAcNo ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */
	get_zd_data( "0670", cPayAcType );			/* �����ʻ����� */
	get_zd_data( "0330", cLnAcNo );				/* �����˺� */
	/*--- xxx --- @20030818@ ---- ��Ҫ�޸���ˮ���ں�������ˮ�� ***
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;		* ��ˮ���� *
	sMoDiscnt.trace_no = g_pub_tx.trace_no;		* ������ˮ�� *
	*/
vtcp_log( "�����˺�[%s]", cLnAcNo );
	/* ��������Э���Ų������ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and sts!='*'", sMoDiscnt.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ˼�¼!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B121" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* ���״̬�Ƿ�Ϊ����������������Ϊ���ֻ������ת�� *
	if( sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "������Э�����ͷ����ֻ����ת�룬�������˽���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B165" );
		goto ErrExit;
	}**/
if(sMoDiscnt.po_knd[0]=='P')/**NEWDP**/
	{
        	if (sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2') {
            	sprintf(acErrMsg, "������Э�����ͷ����ֻ����ת�룬�������˽���!");
            	WRITEMSG
            	strcpy(g_pub_tx.reply, "B165");
            	goto ErrExit;
        	}
    	}
    	else
    	{
        	if (sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' && sMoDiscnt.pn_type[0] != '3') {
            sprintf(acErrMsg, "������Э�����ͷ����ֻ����ת�룬�������˽���!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B165");
            goto ErrExit;
        	}
    	}/****end******/
	if( sMoDiscnt.pn_type[0] == '1' )	/* ���� */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "�˱�����δ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}else if( sMoDiscnt.sts[0] == '2' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}else			/* ���ת�� */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱�������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* �޸����ֵǼǱ��������-������=0��״̬Ϊ�����������ʻ����� */
	strcpy( sMoDiscnt.pay_ac_type, cPayAcType );	/* �����ʻ����� */
	strcpy( sMoDiscnt.sts, "*" );
	T_amt=sMoDiscnt.tranx_bal;
	sMoDiscnt.tranx_bal = 0;
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;		/* ��ˮ���� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;		/* ������ˮ�� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

vtcp_log( "janejanejane   [0450]=[%ld]", sMoDiscnt.not_matr_date );
	set_zd_long("0450",sMoDiscnt.not_matr_date);		/* Ʊ�ݵ������ڣ�Ϊ��Ȩ���� */
	/* ����ˮ��־ */
	/**---- xxx --- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	g_pub_tx.tx_amt1=0.00;
	if(cPayAcType[0] == '0' || cPayAcType[0] == '1' )		/* �����ʻ� */
	{
		sprintf(acErrMsg,"�ڲ������Ŀ");
		WRITEMSG
		goto	OkExit1;
	}else if( cPayAcType[0] == '2' )	/* ����ʻ� */
	{		
		sprintf(acErrMsg,"�Թ������ʻ�");
		WRITEMSG
		goto	OkExit2;
	}

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
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
