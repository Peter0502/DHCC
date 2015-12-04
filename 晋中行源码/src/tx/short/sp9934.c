/*************************************************
* �� �� ��:  sp9934.c
* ���������� ��Ʊ�����ͺ�Ʊ�ݺŻ���������Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��6��21��
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

int sp9934()
{
	struct	mo_discnt_c	sMoDiscnt1,sMoDiscnt2;	/* ���ֵǼǱ� */
	int	ret;
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0 , sizeof(cKinbr));

	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );

	get_zd_data( "0900", sMoDiscnt1.note_type );	/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt1.pnote_no );	/* Ʊ�ݺ� */
	get_zd_data("0030",cKinbr);

	/* ������ֵǼǱ����Ƿ���ڴ�Ʊ�� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' \
			and pnote_no='%s' and pn_type='4' and br_no ='%s' order by wrk_date desc ", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no ,cKinbr);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ!01" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B130" );
        goto ErrExit;
   	}else if( ret )
   	{
      	sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

sprintf(acErrMsg, "pn_type=[%s], [%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no );
WRITEMSG

	/* �ټ�����ֵǼǱ��д�Ʊ�ݵĻ�����Ϣ */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' \
			and pnote_no='%s' and (pn_type='1' or pn_type='2' ) and br_no='%s' order by wrk_date desc ",\ /*�񰡣������������һ�°�*/
			sMoDiscnt1.note_type, sMoDiscnt1.pnote_no,cKinbr );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ!02" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
     }else if( ret )
     {
     	sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
     }

sprintf(acErrMsg, "pn_type=[%s], [%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no );
WRITEMSG
	 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld and ac_seqn=0 ",sMoDiscnt2.ac_id);
	 if( ret )
	 {
			sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B130" );
            goto ErrExit;
	 }

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0330" , g_mdm_ac_rel.ac_no );		/* �����ʺ� */
    set_zd_double( "1002", sMoDiscnt2.par_amt );	/* ��Ʊ��� */
	set_zd_long( "0450", sMoDiscnt2.not_matr_date );/* ��Ʊ������ */
	set_zd_data( "0310", sMoDiscnt2.rem_ac_no );	/* ��Ʊ���ʺ� */
	set_zd_data( "0260", sMoDiscnt2.remitter );	/* ��Ʊ��ȫ�� */
	set_zd_data( "0810", sMoDiscnt2.payer_brno_name );/* ������ȫ�� */
	set_zd_data( "0320", sMoDiscnt2.payee_ac_no );	/* �տ����ʺ� */
	set_zd_data( "0270", sMoDiscnt2.payee );		/* �տ���ȫ�� */
	set_zd_data( "0820", sMoDiscnt2.payee_brno_name );/* �տ��˿�����ȫ�� */
	set_zd_data( "0680", sMoDiscnt1.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_double( "0850", sMoDiscnt1.tpn_rate );	/* ת�������� */
	set_zd_double( "1001", sMoDiscnt1.tpn_int );	/* ת������Ϣ */
	set_zd_data( "0250", sMoDiscnt1.tpn_name );	/* ���������� */

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
