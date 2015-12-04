/*************************************************
* �� �� ��:  spB354.c
* ���������� ��ɵ����ʺźͽ�ݺŹ�ϵ�Ľ����ͽ��
*
* ��    ��:  Dongxy
* ������ڣ� 2006��9��18��
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
#include "mo_bank_acc_po_c.h"

int spB354()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* ������ϵ�Ǽǲ� */
	char	cOperCode[2];			/* ������ */
	int		ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cOperCode );		/** �������� **/
	get_zd_data( "0330", sMoBankAccPo.pact_no );	/** �ж�Э��� **/

	vtcp_log("[%s][%d]========��������=[%s]",__FILE__,__LINE__,cOperCode);


	if(cOperCode[0] == '0')	/** ǰ̨ѡ���� **/
	{
		vtcp_log("[%s][%d]========��ʼ����",__FILE__,__LINE__);
		/*	���ݹ����ָʾ�����Ʊ��״̬��  2006��12��14
        	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and acc_po_sts in ('2','3')", \
		*/
        	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ", \
                                sMoBankAccPo.pact_no);
	        if ( ret )
	        {
       	         	sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}

        	ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
        	if( ret == 100 )
        	{
                	sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "B199" );
                	goto ErrExit;
        	}else if( ret )
        	{
                	sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}
     /***
		if(sMoBankAccPo.acc_po_sts[0] != '0')
		{
                        sprintf( acErrMsg, "��Э�������������ף�������ɾ��" );
                        sprintf( g_pub_tx.reply, "B164" );
                        WRITEMSG
                        goto ErrExit;

		}***/
		/** �޸ı�֤���ʺźͱ��� **/
		get_zd_data( "0320", sMoBankAccPo.bail_ac_no );	/** ��֤���ʺ� **/
		get_zd_double("1004", &sMoBankAccPo.bail_amt );	/** ��֤��� **/

		pub_base_old_acno( sMoBankAccPo.bail_ac_no );
		vtcp_log("[%s][%d]========bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
		sMoBankAccPo.bail_amt_ratio=100;
		vtcp_log("[%s][%d]========bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);

	}
	else if(cOperCode[0] == '1')	/** ǰ̨ѡ��ɾ�� **/
	{
		vtcp_log("[%s][%d]========��ʼɾ��",__FILE__,__LINE__);
		get_zd_data( "0320", sMoBankAccPo.bail_ac_no );	/** ��֤���ʺ� **/
                ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and bail_ac_no='%s'", \
                                sMoBankAccPo.pact_no, sMoBankAccPo.bail_ac_no );
                if ( ret )
                {
                        sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

                ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
                if( ret == 100 )
                {
                        sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B199" );
                        goto ErrExit;
                }else if( ret )
                {
                        sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

		
		memset( sMoBankAccPo.bail_ac_no, 0x00, sizeof(sMoBankAccPo.bail_ac_no)-1 );	/** ��֤���ʺ� **/
		sMoBankAccPo.bail_amt=0;	/** ��֤��� **/
		sMoBankAccPo.bail_amt_ratio=0;
                set_zd_data( "0320", sMoBankAccPo.bail_ac_no ); /** ��֤���ʺ� **/
                set_zd_double("1004", sMoBankAccPo.bail_amt );  /** ��֤��� **/

	}
	else
	{
		sprintf( acErrMsg, "�������ʹ���![%s]",cOperCode );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O020" );
		goto ErrExit;
	}

        ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
        if ( ret )
        {
               	sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
               	WRITEMSG
               	goto ErrExit;
        }

        Mo_bank_acc_po_Clo_Upd();

	set_zd_data( "0300", sMoBankAccPo.payer_ac_no ); /* ��Ʊ���ʺ�*/
	set_zd_data( "0250", sMoBankAccPo.payer_name ); /* ��Ʊ������*/

	vtcp_log("[%s][%d]========��ʼд��ˮ",__FILE__,__LINE__);
	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "��֤���ʺŹ���" );	/* ��ע */

	/* ����ˮ��־ */
	memcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no,19);	/* ��ˮ����ʺ���19λ�� */
	memcpy(g_pub_tx.beg_note_no,sMoBankAccPo.pact_no,16);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]========�������",__FILE__,__LINE__);

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
