/*************************************************
* �� �� ��:  spB804.c
* ���������� ��ɷ�չ�ٴ������ʹ��ܣ��޸��ٴ���
*	     �ǼǱ����ܻ��������ǰ�����
*
* ��    ��:  jane
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��: 20031011
*   �� �� ��:
*   �޸�����: ���Ӽ����Ļ������ʺ��Ƿ�Ϊ����׼�����ʺ�
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "mo_again_ln_c.h"
#include "mo_pay_again_ln_c.h"

int spB804()
{
	struct	com_branch_c		sComBranch;		/* ����������� */
	struct	mo_again_ln_c		sMoAgainLn;		/* �ٴ���ǼǱ� */
	struct	mo_pay_again_ln_c	sMoPayAgainLn;		/* �ٴ����ǼǱ� */
	char zbj_ac_no[30];
	int	ret;

	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoPayAgainLn, 0x00, sizeof( struct mo_pay_again_ln_c ) );
	memset( zbj_ac_no, 0x00, sizeof( zbj_ac_no ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoAgainLn.pact_no );		/* Э���� */
	get_zd_double( "1001", &g_pub_tx.tmp_amt1 );	/* ��ǰ������ */
	get_zd_double( "1002", &g_pub_tx.tx_amt1 );		/* ��ǰ������Ϣ */
	vtcp_log("�������[%.2f],��Ϣ��[%.2f]\n",g_pub_tx.tmp_amt1,g_pub_tx.tx_amt1);
	set_zd_double( "1218", g_pub_tx.tmp_amt1+g_pub_tx.tx_amt1);
	get_zd_data( "0310", zbj_ac_no );		/* ����׼�����ʺ� */
        pub_base_strpack(zbj_ac_no);
	g_reply_int = sql_count( "mdm_ac_rel a,in_mst b", 
		"a.ac_no='%s' and b.ac_id=a.ac_id and b.ac_seqn=a.ac_seqn and b.prdt_no='902'", zbj_ac_no );
	if( g_reply_int < 0 )
	{
		sprintf( acErrMsg, "ִ��sql_count Error[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int == 0 )
	{
		strcpy( g_pub_tx.reply, "B324" );
		sprintf( acErrMsg, "�ʺŲ�������׼�����ʺ�![%s][%d]", zbj_ac_no, ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ����ٴ����ǼǱ���Э���š���״̬Ϊ¼��ļ�¼�Ƿ���� */
	/*
	ret=  Mo_pay_again_ln_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and amt=%lf \
		and sts=0 ", sMoAgainLn.pact_no, g_pub_tx.tmp_amt1);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_again_ln_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_pay_again_ln_Fet_Upd( &sMoPayAgainLn, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ٴ����ǼǱ��в����ڴ�Э����!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B109" );
        goto ErrExit;
   	}
	else if( ret )
    {
    	sprintf( acErrMsg, "ִ��Mo_pay_again_ln_Fet_Upd��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
	*/
	/* �޸��ٴ���ǼǱ����ܻ�����ܻ������ǰ�����,
	   ����Ƿ��Ѿ�ȫ�����꣬����ǣ�״̬Ϊ2ȫ�����������Ϊ1���� */
	ret = Mo_again_ln_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoAgainLn.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_again_ln_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_again_ln_Fet_Upd( &sMoAgainLn, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ٴ���ǼǱ��в����ڴ�Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B107" );
                goto ErrExit;
    }else if( ret )
    {
    	sprintf( acErrMsg, "ִ��Mo_again_ln_Fet_Upd��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
	/**----- xxx ---- @20030821@ ----- ȡ�ٽ���ʺż����� ----**/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld ",sMoAgainLn.ac_id);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_again_ln_Dec_Upd��[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	/**------------------------------------------------------**/
	set_zd_data("0300",g_mdm_ac_rel.ac_no);

	/* �޸��ٴ����Ǽǲ�,���������ʴ���״̬��Ϊ1ȫ������ ����������� liuxuan 
	strcpy( sMoPayAgainLn.sts, "1" );
	ret = Mo_pay_again_ln_Upd_Upd( sMoPayAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸��ٴ����ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	Mo_pay_again_ln_Clo_Upd();
	*/

	/* �ܻ�����ܻ������ǰ������+ ��Ϣ */
	sMoAgainLn.sum_amt = sMoAgainLn.sum_amt + g_pub_tx.tmp_amt1 +g_pub_tx.tx_amt1;
	ret = pub_base_CompDblVal( sMoAgainLn.sum_amt, sMoAgainLn.amt );
	if( ret >= 0 )
		strcpy( sMoAgainLn.sts, "2" );					/* ��־��2ȫ������ */
	sMoAgainLn.pay_date=g_pub_tx.tx_date;				/* �������� */
	sMoAgainLn.cur_amt=g_pub_tx.tmp_amt1 +g_pub_tx.tx_amt1; /* ���ڻ���� */
	ret = Mo_again_ln_Upd_Upd( sMoAgainLn, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸��ٴ���ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	Mo_again_ln_Clo_Upd();

	/* ����ˮ��־ */
	/**----- xxx ---- @20030821@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	strcpy(g_pub_tx.no_show,"1");
	sprintf(g_pub_tx.beg_note_no,"%.2f",g_pub_tx.tx_amt1);
	g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;
	vtcp_log("vvvvvvv g_pub_tx.tx_amt1[%.2f]\n",g_pub_tx.tx_amt1);
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
