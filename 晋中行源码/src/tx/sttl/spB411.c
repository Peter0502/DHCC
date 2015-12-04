/*************************************************
* �� �� ��:  spB411.c
* ���������� ��ɴ������¼�롢���˹��ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��1��26��
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
#include "com_branch_c.h"
#include "mo_fax_c.h"

int spB411()
{
	struct	com_branch_c	sComBranch;	/* ����������� */
	struct	mo_fax_c	sMoFax;		/* ����ǼǱ� */
	char	cOpenBrno[9];			/* �������ʺſ������� */
	char	cPayerAcNo[20];			/* �������ʺ� */
	char	cOperCode[6];			/* ҵ����� */
	int	ret;
	
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sMoFax, 0x00, sizeof( struct mo_fax_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cPayerAcNo, 0x00, sizeof( cPayerAcNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoFax.fax_no );			/* ������ */
	sMoFax.tx_date = g_pub_tx.tx_date;			/* �������� */
	strcpy( sMoFax.tx_br_no, g_pub_tx.tx_br_no );		/* ���׻����� */
	get_zd_data( "0250", sMoFax.requ_name );		/* ������ȫ�� */
	get_zd_data( "0260", sMoFax.payer_name );		/* ������ȫ�� */
	get_zd_data( "0300", cPayerAcNo );			/* �������ʺ� */
	get_zd_long( "0340", &sMoFax.payer_ac_seqn );		/* �������˺���� */
	get_zd_data( "0860", sMoFax.payer_br_no );		/* �����˿������к� */
	get_zd_data( "0820", sMoFax.payer_br_name );		/* �����˿��������� */
	get_zd_data( "0270", sMoFax.payee_name );		/* �տ���ȫ�� */
	get_zd_data( "0310", sMoFax.payee_ac_no );		/* �տ����˺� */
	get_zd_data( "0870", sMoFax.payee_br_no );		/* �տ��˿������к� */
	get_zd_data( "0810", sMoFax.payee_br_name );		/* �տ��˿��������� */
	get_zd_double( "0390", &sMoFax.fax_amt );		/* ������ */
	strcpy( sMoFax.fax_sts, "0" );				/* ����״̬0¼��*/
	strcpy( cOperCode, "10005" );
	
	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻���Ȩ�޴�" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ��鸶���ʺŵ���Ч�ԣ��������ʺ�id */
	ret = pub_base_ChkAc( cPayerAcNo, sMoFax.payer_ac_seqn, \
				cOpenBrno, &sMoFax.payer_ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "��鸶���ʺ���Ч�Դ�" );
		WRITEMSG
		goto ErrExit;	
	}
sprintf( acErrMsg, "br_no=[%s]", g_pub_tx.tx_br_no );
WRITEMSG	
	/* ��������Ƿ��Ѿ���ӡ������Ѿ���ӡ������¼�� */
	ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", g_pub_tx.tx_br_no );
	if( ret == 100 )
    	{
        	sprintf( acErrMsg,"ȡ�����������com_branch��!br_no=[%s]", g_pub_tx.tx_br_no );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "S019" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Com_branch_Sel����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   	}
   	
   	if( sComBranch.spe_ind[0] == '1' )
   	{
   		sprintf( acErrMsg,"�����ӡ״̬Ϊ�Ѵ�ӡ!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B077" );
        	goto ErrExit;
   	}
	
	/* �ǼǴ���ǼǱ���״̬Ϊ¼�� */
	ret = pub_sttl_RegMoFax( sMoFax );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoFax����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
		
	/* ����ˮ��־ */
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
