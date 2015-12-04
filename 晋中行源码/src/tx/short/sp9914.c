/*************************************************
* �� �� ��:  sp9914.c
* ���������� ��Ȩ���ۼ�Ȩ����ɱ� 
*
* ��    ��:  
* ������ڣ� 2003��6��09��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"

int sp9914()
{
	struct	mo_righ_invest_c	sMo_righ_invest;	/* Ȩ����Ͷ�ʵǼǲ� */
	int	ret=0,sal_num=0;
	double amt=0.00;
	double sj1,sj2;

	memset( &sMo_righ_invest, 0x00, sizeof( struct mo_righ_invest_c ) );

	get_zd_data( "0580", sMo_righ_invest.pact_no );	/* Ȩ��Ͷ��Э���� */
	get_zd_int( "0530", &sal_num );					/* ���۹��� */

	/* ����Ȩ����Ͷ��Э���Ų���Ȩ����Ͷ�ʵǼǱ� */
	ret = Mo_righ_invest_Sel( g_pub_tx.reply, &sMo_righ_invest, "pact_no='%s' ",sMo_righ_invest.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴�Ȩ��Ͷ��Э����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B082" );
		goto ErrExit;
    }
	else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_righ_invset_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
   	}

	if( sal_num>sMo_righ_invest.num )
   	{
   		sprintf( acErrMsg, "���۹������ڳ��й���[%d]", sMo_righ_invest.num);
        WRITEMSG
		sprintf( g_pub_tx.reply, "B401" );
        goto ErrExit;
   	}
	else if( sal_num==sMo_righ_invest.num )
   	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_righ_invest.ac_id);
		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��In_mst_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   		}
		set_zd_double( "A029", g_in_mst.bal );		/* ���۳ɱ� */
	}
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_righ_invest.ac_id);
		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��In_mst_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   		}
		vtcp_log("sal_num=[%d],num=[%d]",sal_num,sMo_righ_invest.num);
		sj1=sal_num;
		sj2=sMo_righ_invest.num;
		amt=pub_base_PubDround_1( (sj1/sj2) * g_in_mst.bal);
		vtcp_log("amt=[%lf]",amt);
		set_zd_double( "0400", amt );		/* ���۳ɱ� */
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
