/*************************************************
* �� �� ��:  spB801.c
* ���������� ��ɵǼ��ٴ���ǼǱ����ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��7��23��
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

int spB801()
{
	struct	mo_again_ln_c	sMoAgainLn,sMoAgainLn_c;	/* �ٴ���ǼǱ� */
	int	ret;
	char flag[2];

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	memset( &sMoAgainLn_c, 0x00, sizeof( struct mo_again_ln_c ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0680", flag);	/* 0����   1�޸� */
	get_zd_data( "0670", sMoAgainLn.again_ln_type );	/* �ٴ������� */
	get_zd_data( "0580", sMoAgainLn.pact_no );			/* Э���� */
	strcpy( sMoAgainLn.br_no, g_pub_tx.tx_br_no );		/* �������� */
	strcpy( sMoAgainLn.tel, g_pub_tx.tel );				/* ����Ա */
	sMoAgainLn.reg_date = g_pub_tx.tx_date;				/* �Ǽ����� */
	get_zd_double( "1001", &sMoAgainLn.amt );			/* ������ */
	get_zd_long( "0440", &sMoAgainLn.to_date );			/* �������� */
	get_zd_double( "0840", &sMoAgainLn.rate );			/* ���� */

	switch( atoi(flag) ){
		case 0:
			ret = Mo_again_ln_Sel(g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'",sMoAgainLn.pact_no);
			if( ret == 0 )
			{
				sprintf( acErrMsg, "�ٴ���ǼǱ����Ѿ����ڴ�Э����!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B106" );
				goto ErrExit;
			}else if( ret != 100 )
			{
				sprintf( acErrMsg, "ִ��Mo_again_ln_Sel��![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			/* ȡ�ٴ����Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
			ret = pub_base_GetParm_Str( "ZDK", 1, sMoAgainLn.prdt_no );
			if( ret )
			{
				sprintf( acErrMsg, "ȡ�ٴ����Ʒ��Ŵ�!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B255" );
				goto ErrExit;
			}

			/* �Ǽ��ٴ���ǼǱ�����־Ϊ¼�� */
			strcpy( sMoAgainLn.sts, "0" );				/* ��־��0¼�� */
			ret = Mo_again_ln_Ins( sMoAgainLn, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_again_ln_Ins����!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			break;
        case 1 :
            /* �޸� */
            ret = Mo_again_ln_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts ='0'",
                  sMoAgainLn.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            ret = Mo_again_ln_Fet_Upd(&sMoAgainLn_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�ٴ���ǼǱ��в����ڴ�Э����!ret=[%d]",ret);
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B107");
                goto ErrExit;
            }

            if (sMoAgainLn_c.sts[0] != '0')
            {
                sprintf(acErrMsg,"�ñ��ٴ���״̬����ȷ!");
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B286");
                goto ErrExit;
            }

		strcpy( sMoAgainLn.sts, "0" );				/* ��־��0¼�� */
	    strcpy(sMoAgainLn.rowid, sMoAgainLn_c.rowid);
            ret = Mo_again_ln_Upd_Upd(sMoAgainLn, g_pub_tx.reply);
            if ( ret != 0)
            {
                sprintf(acErrMsg,"�޸��ٴ���Ǽǲ�����!! ret =[%s]",ret);
                WRITEMSG
                Mo_again_ln_Clo_Upd();
                strcpy(g_pub_tx.reply,"B107");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"�ٴ�����Ϣ�޸�");
            Mo_again_ln_Clo_Upd();
            sprintf(acErrMsg,"�޸��ٴ���Ǽǲ�pass!! ");
            WRITEMSG
            break;
        default :
            sprintf(acErrMsg,"�������ͳ���!! type=[%c] 0¼��1�޸�2ɾ��",flag);
            goto ErrExit;
            break;
	}

	/* ����ˮ��־ */
	/**----- xxx ---- @20030821@ ---- ��������Ҫ��Э���� ----**/
	strcpy( g_pub_tx.brf, "�ٴ���ά��" );
	strcpy(g_pub_tx.ac_no,sMoAgainLn.pact_no);
	g_pub_tx.tx_amt1=sMoAgainLn.amt;
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
