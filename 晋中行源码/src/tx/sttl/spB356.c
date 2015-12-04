/*************************************************
* �� �� ��:  spB356.c
* ���������� ������гжһ�Ʊ��Ѻ���⹦��
*
* ��    ��:  dongxy
* ������ڣ� 2006��09��15��
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
#include "mo_bank_po_c.h"
#include "mo_dep_reg_c.h"


int spB356()
{
        struct  mo_dep_reg_c            sMoDepReg;      /* ��Ѻ�浥�Ǽǲ� */
        struct  mo_bank_po_c            sMoBankPo;      /* ���гжһ�Ʊ�Ǽǲ� */

	char    cLoadName[50];       		   		/* �ϴ��ļ��� */
	double	sum_je=0.00;
	int		ret=0;

	/* ���ݳ�ʼ�� */
        memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
        memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoDepReg.pact_no );			/* ��Ѻ��� */

	/* �����Ѻ�浥�ǼǱ����Ƿ��Ѵ��ڴ���ѺЭ���� */
	ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_dep_reg_Sel����![%d][%s]", ret,sMoDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}

	get_zd_double( "1002", &sMoDepReg.po_amt );			/* ��Ѻ�ܽ�� */
	/* �����Ѻ�ܽ���Ƿ���� */
	ret = sql_sum_double("mo_dep_reg","po_amt",&sum_je," pact_no='%s' ",sMoDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "�����ܽ�����![%d][%s]", ret,sMoDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}
	if( pub_base_CompDblVal(sum_je,sMoDepReg.po_amt)!=0 )
	{
		sprintf( acErrMsg, "�ܽ�����![%.2f][%.2f][%s]",sum_je,sMoDepReg.po_amt,sMoDepReg.pact_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B186");
		goto ErrExit;
	}

	ret = Mo_dep_reg_Dec_Upd( g_pub_tx.reply, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret )
	{
		sprintf(acErrMsg,"ִ��Mo_dep_reg_Dec_Upd����![%d]",ret);
		WRITEMSG
       		goto ErrExit;
	}

	while(1)
	{
		ret = Mo_dep_reg_Fet_Upd( &sMoDepReg, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_dep_reg_Fet_Upd����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

                /** �жϴ˻�Ʊ��״̬�ͽ�� **/
                ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "po_no='%s'",sMoDepReg.po_no);
                if( ret )
                {
                        sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd����![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

                ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
                if( ret == 100 )
                {
                        sprintf( acErrMsg, "�޴˳жһ�Ʊ!" );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B217" );
                        goto ErrExit;
                }
                else if( ret )
                {
                        sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }
				/****add by liuyue 20070104 ***
                if( sMoBankPo.po_sts[0] != '5' || sMoBankPo.po_ind[0] != '1' )*/
                if( (sMoBankPo.po_sts[0] != '5' &&  sMoBankPo.po_sts[0] !='9') || sMoBankPo.po_ind[0] != '1' )
                {
                        sprintf( acErrMsg, "�жһ�Ʊ�����������ף����ܳ���!po_sts=[%c],po_ind=[%c]",sMoBankPo.po_sts[0],sMoBankPo.po_ind[0] );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B062" );
                        goto ErrExit;
                }

                sMoBankPo.po_sts[0] = '1';      /** ��Ʊ״̬��Ϊǩ�� **/
                ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
                if ( ret )
                {
                        sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B068" );
                        goto ErrExit;
                }
                Mo_bank_po_Clo_Upd();

		ret = Mo_dep_reg_Del_Upd(sMoDepReg,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_dep_reg_Del_Upd����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

	}
	Mo_dep_reg_Clo_Sel ();

	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "�жһ�Ʊ��Ѻ�浥����" );	/* ��ע */
	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sMoDepReg.pact_no);
	strcpy(g_pub_tx.beg_note_no,sMoDepReg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
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
