/*************************************************
* �� �� ��:  spB803.c
* ���������� ����ٴ������Ϣ¼�빦�ܣ��Ǽ��ٴ���
*		 ����ǼǱ���
*
* ��	��:  jane
* ������ڣ� 2003��8��23��
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
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"

int spB803()
{
	struct	mo_again_ln_c		sMoAgainLn;		/* �ٴ���ǼǱ� */
	struct	mo_pay_again_ln_c	sMoPayAgainLn;	/* �ٴ����ǼǱ� */
	double	dAmt=0;								/* ��ǰ������ */
	double	dSumAmt=0;							/* �����ܶ� */
	int		ret;
	long	days=0;		/** ������Ϣ������ **/
	long	date;		/** �������� **/
	double  dLx;		/** �����������Ϣ **/

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoPayAgainLn, 0x00, sizeof( struct mo_pay_again_ln_c ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoPayAgainLn.pact_no );	/* Э���� */
	get_zd_double( "1001", &dAmt );					/* ����� */
	get_zd_long( "0050", &date );			/* �������� */
 vtcp_log("%s,%d,date=[%d]\n",__FILE__,__LINE__,date);

	/* ����ٴ����ǼǱ����Ƿ���ڴ�Э���Ų���״̬Ϊ¼�� */
	ret = Mo_again_ln_Sel( g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'",\
					 sMoPayAgainLn.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ٴ���ǼǱ��в����ڴ�Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B118" );
		goto ErrExit;
	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_again_ln_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( sMoAgainLn.sts[0] == '0' )
	{
		sprintf( acErrMsg, "�˱��ٴ��δ����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B118" );
		goto ErrExit;
	}else if( sMoAgainLn.sts[0] == '2' )
	{
		sprintf( acErrMsg, "�˱��ٴ�����ȫ������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "" );
		goto ErrExit;
	}

	/* ����ѻ����ܽ��+���λ������Ƿ񳬹������� */
	dSumAmt = sMoAgainLn.sum_amt + dAmt;
	if ( pub_base_CompDblVal( dSumAmt, sMoAgainLn.amt ) == 1 )
	{
		sprintf( acErrMsg, "�ѻ����ܽ���������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B283" );
		goto ErrExit;
	}

	/* ����ٴ����ǼǱ���Э����״̬Ϊ¼��ļ�¼�Ƿ���� */
	ret = Mo_pay_again_ln_Sel( g_pub_tx.reply, &sMoPayAgainLn, "pact_no='%s'\
							and sts=0 ", sMoPayAgainLn.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "��Э���л���¼�뵫δ���˼�¼!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B290" );
        goto ErrExit;
   	}
	else if( ret != 100 && ret )
    {
    	sprintf( acErrMsg, "ִ��Mo_pay_again_ln_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }


	/** ��Ӽ�����Ϣ **/
	days = pub_base_CalTriDays(sMoAgainLn.reg_date,date);
 vtcp_log("%s,%d,����õ�����=[%d]\n",__FILE__,__LINE__,days);
 vtcp_log("%s,%d,����=[%f]\n",__FILE__,__LINE__,sMoAgainLn.rate);
	dLx = days * dAmt * sMoAgainLn.rate / 30000;
 vtcp_log("%s,%d,����õ���Ϣ=[%f]\n",__FILE__,__LINE__,dLx);


	/* �Ǽ��ٴ����ǼǱ�����־Ϊ¼�� */
	sMoPayAgainLn.intst_amt = dLx;
	sMoPayAgainLn.amt = dAmt;					/* ������ */
	strcpy( sMoPayAgainLn.sts, "0" );			/* ��־��0¼�� */
	sMoPayAgainLn.pay_date = g_pub_tx.tx_date;	/* �������� */

	ret = Mo_pay_again_ln_Ins( sMoPayAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_again_ln_Ins����!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_double("0390", dLx);

	/* ����ˮ��־ */
	/**----- xxx ---- @20030821@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	sprintf(g_pub_tx.beg_note_no,"%.2f",sMoPayAgainLn.intst_amt);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
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
}
