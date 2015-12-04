/*************************************************
* �� �� ��:  sp9695.c
* ���������� ��֤������ά����ݲ�ѯ
*
* ��    ��:   
* ������ڣ� 2003��11��02��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bail_rate_c.h"

int sp9695()
{
	struct	mo_bail_rate_c	sMoBailRate;		/* ��֤�����ʵǼǱ� */
	char    czm[2];
	int	ret;

	/* ���ݳ�ʼ�� */
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
    pub_base_sysinit();
	get_zd_data( "0680", czm );							/* ������ */
	get_zd_data( "0670", sMoBailRate.bail_type );		/* ��֤������ */
	get_zd_data( "0330", sMoBailRate.pact_no );			/* Э���� */

	if( czm[0]=='0' )		/*** ���� ***/
	{
		strcpy(g_pub_tx.reply,"0000");
		return 0;
	}

	if( sMoBailRate.bail_type[0]=='1' )				/** �жұ�֤�� **/
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&sMoBailRate,"pact_no='%s' and bail_type[1]='1' and sts='1'",sMoBailRate.pact_no);
	}
	else if( sMoBailRate.bail_type[0]=='2' )		/** �жұ�֤�� **/
	{
		ret=Mo_bail_rate_Sel(g_pub_tx.reply,&sMoBailRate,"pact_no='%s' and bail_type[1]='2' and sts='1'",sMoBailRate.pact_no);
	}
	else
	{
   		sprintf( acErrMsg, "��֤�����ʹ���![%d][%s][%s]",ret,sMoBailRate.pact_no,sMoBailRate.bail_type );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if( ret )
   	{
   		sprintf( acErrMsg, "�ǼǱ�֤�����ʵǼǱ�����![%d][%s][%s]",ret,sMoBailRate.pact_no,sMoBailRate.bail_type );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	set_zd_data( "0320", sMoBailRate.bail_ac_no );		/* ��֤���ʺ� */
	set_zd_data( "0310", sMoBailRate.ac_no );			/* ��Ϣ�ʺ� */
	set_zd_double( "0390",sMoBailRate.amt );			/* ��֤���� */
	set_zd_long( "0440", sMoBailRate.beg_date );		/* ��Ϣ���� */
	set_zd_long( "0450", sMoBailRate.end_date );		/* �������� */
	set_zd_double( "0840",sMoBailRate.ic_rate );		/* ���� */

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
