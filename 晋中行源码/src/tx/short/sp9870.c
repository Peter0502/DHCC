/*************************************************
* �� �� ��:  sp9870.c
* ���������� ��������Э���Ż������ֵǼǱ���Ϣ
*
* ��    ��:  jack
* ������ڣ� 2005��3��14��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "mdm_ac_rel_c.h"

int sp9870()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	int	ret;
	char cAc_no[25];
	memset(cAc_no,0,sizeof(cAc_no));
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	
	get_zd_data( "0630", sMoDiscnt.rpact_no );	/* ������Э���� */
	/** add by LiuHuafeng 20061027 ���������Э����ǿգ���ô�����˺��Ƿ���ڣ�
		���������ô�����˺��ҵ�id�ţ�Ȼ���ڸ���id������Э��� **/
	pub_base_strpack(sMoDiscnt.rpact_no);
	if(strlen(sMoDiscnt.rpact_no)==0)
	{
		struct mdm_ac_rel_c sMdm_ac_rel;
		memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
		get_zd_data("0300",cAc_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
		if(ret)
		{
			vtcp_log("%s,%d ��ѯ�˻����ʹ�ϵ�����,ret=[%d]",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "�˺Ų�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B242" );
			goto ErrExit;
		}
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "ac_id=%d ", sMdm_ac_rel.ac_id );
	}
	else
	{/* end by LiuHuafeng 2006-10-27 22:34 */
		/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "rpact_no='%s'", sMoDiscnt.rpact_no );
	}
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�������Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B138" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	if( sMoDiscnt.pn_type[0] == '6' )	/* ��������  */
		set_zd_data( "0690", "1" );	/* ��� */
	if( sMoDiscnt.pn_type[0] == '7' )	/* ��������  */
		set_zd_data( "0690", "2" );	/* �ع� */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
	set_zd_double( "0850", sMoDiscnt.rtpn_rate );	/* �������� */
	/**
		set_zd_long( "0440", sMoDiscnt.out_date );	* ��Ʊ��Ʊ�� *
		set_zd_data( "0580", sMoDiscnt.payer_brno );* �������к� *
		set_zd_data( "0590", sMoDiscnt.payee_brno );�տ��˿������к�
		set_zd_data( "0330", sMoDiscnt.in_ac_no ); * �ͻ��ʺ� *
	**/
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* ��Ʊ������ */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* ��Ʊ���ʺ� */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* ��Ʊ��ȫ�� */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* ������ȫ�� */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* �տ����ʺ� */
	set_zd_data( "0270", sMoDiscnt.payee );		/* �տ���ȫ�� */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* �տ��˿�����ȫ�� */

	set_zd_data( "0900", sMoDiscnt.note_type );	/* Ʊ������ */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* Ʊ�ݺ� */

	set_zd_data( "0370", sMoDiscnt.rin_ac_no );	/* �������˻� */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* ��;���� */
	set_zd_double( "1001", sMoDiscnt.pn_int );	/* ������Ϣ */
	set_zd_data("0910",sMoDiscnt.br_no);

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
