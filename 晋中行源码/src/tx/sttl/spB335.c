/*************************************************
* �� �� ��:  spB335.c
* ���������� ��������˺ű�������߸����˽�Ǯֱ�ӻ�
*	     �뱣֤���˻�����ͨ�����㻧���ڻ�Ʊ����
*	     ǰ�����˽��ף����Ҳֽɴ汣֤��ʱҲ����
*	     ����;
*
* ��    ��:  jane
* ������ڣ� 2003��5��28��
*
* �޸ļ�¼�� 
*   ��   ��: 2011-8-10 13:18:14
*   �� �� ��:chenchao
*   �޸�����:����Ҫ��:������ά���Ǽ������Ϣ,��Ϊ��֤�����п��ܻ�û�����ɳж�Э����

*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bail_rate_c.h"
#include "mo_sg_cash_po_c.h"
#include "dd_mst_c.h"

int spB335()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_bail_rate_c 		sMo_bail_rate;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* ���ҲֵǼǱ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* �����˺Ŷ��ձ� */
	struct	dd_mst_c		sDd_mst;
	struct	dd_mst_c 			sDdMst;			/* �������ļ� */
	double	dAmt;								/* ���ɽ�� */
	char	cOperCode[2];						/* ������ */
	char	cAmtSource[2];						/* �����Դ */
	char	cAcNo[20];							/* �ۿ��˺� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	int	ret;
	char	cPact_no[21];
	

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMo_bail_rate, 0x00, sizeof( sMo_bail_rate ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sDd_mst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cAmtSource, 0x00, sizeof( cAmtSource ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0680", cOperCode );			/* ������ */
	get_zd_data( "0330", sMoBankAccPo.pact_no );		/* �жһ�ƱЭ���� */
	get_zd_double( "1004", &dAmt );				/* ���ɽ�� */
	get_zd_data( "0670", cAmtSource );			/* �����Դ */
	get_zd_data( "0310", cAcNo );				/* �ۿ��˺� */
	get_zd_data( "0320", cBailAcNo );			/* ��֤���ʺ� */

	/** ���ʺŴ��� **/
	pub_base_old_acno( cAcNo );
	pub_base_old_acno( cBailAcNo );
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",cBailAcNo);
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Sel��![%d][%s]",ret,cBailAcNo);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "�Ǳ�֤���˻��������ô˽���![%d][%s]",ret,cBailAcNo);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if( cOperCode[0] == '2' )			/* ���ɱ�֤�� */
	{
		/* �����ɽ��ӵ��жһ�Ʊ������Ϣ�Ǽǲ��б�֤���һ���� */
		ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
		if( ret && ret!=100)
		{
			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}else if(ret == 0)
		{
			if( sMoBankAccPo.acc_po_ind[0] == '1' )
			{
				sprintf( acErrMsg, "�˳ж��ѵ���!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B263" );
				goto ErrExit;
			}

			/* xxx ---- 20031107 ---- ��鱣֤���ʺ��Ƿ���ȷ ----*/
			pub_base_strpack(sMoBankAccPo.bail_ac_no);
			sprintf( acErrMsg, "��֤���ʺ�[%s][%s]",sMoBankAccPo.bail_ac_no,cBailAcNo);
			WRITEMSG
			if( strlen(sMoBankAccPo.bail_ac_no)>0 && strcmp(sMoBankAccPo.bail_ac_no,cBailAcNo) )
			{
	   			sprintf( acErrMsg, "��֤���ʺŴ���![%s][%s]",sMoBankAccPo.bail_ac_no,cBailAcNo);
				WRITEMSG
				strcpy(g_pub_tx.reply,"B258");
				goto ErrExit;
			}
			if( strlen(sMoBankAccPo.bail_ac_no)==0 )
			{
				strcpy(sMoBankAccPo.bail_ac_no,cBailAcNo);
			}
			/**------------- xxx -------------- end -----------------**/

			/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
			sMoBankAccPo.bail_amt += dAmt;		/* ��֤��� */
			sMoBankAccPo.bail_amt_ratio = pub_base_PubDround_1(sMoBankAccPo.bail_amt*100/sMoBankAccPo.sign_amt);/** ���±�֤�����***/
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			Mo_bank_acc_po_Clo_Upd();
                	
			/* ����ж�����Ϊ���Ҳ֣������޸ı��ҲֵǼǲ� */
			ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoBankAccPo.pact_no );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Dec_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCashPo, g_pub_tx.reply );
			if ( ret == 100 )
			{
				sprintf( acErrMsg, "�˳жҷǱ��Ҳ�!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "B267" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Fet_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
			sMoSgCashPo.safeg_sum += dAmt;		/* �Ѵ汣֤���� */
			ret = Mo_sg_cash_po_Upd_Upd( sMoSgCashPo , g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ı��ҲֵǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
                	
			Mo_sg_cash_po_Clo_Upd( );
			/* ����ж�����Ϊ���ʵǼǱ� */
			ret =Mo_bail_rate_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoBankAccPo.pact_no );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
                	
			ret = Mo_bail_rate_Fet_Upd( &sMo_bail_rate, g_pub_tx.reply );
			if ( ret && ret!=100)
			{
				sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Fet_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
			sMo_bail_rate.amt += dAmt;		/* �Ѵ汣֤���� */
			ret = Mo_bail_rate_Upd_Upd( sMo_bail_rate , g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ı��ҲֵǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;	
			}
                	
			Mo_bail_rate_Clo_Upd();
		}else{
			sprintf( acErrMsg, "��ѯʧ��[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
		
			
		/* Ϊ����ˮ��׼�� */
		strcpy( g_pub_tx.brf, "�жһ�Ʊ���ɱ�֤��" );	/* ��ע */
	}

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.no_show,"1");
	strcpy(g_pub_tx.add_ind,cOperCode);			/** �ɴ油�� **/
	strcpy(g_pub_tx.note_type,sMoBankAccPo.acc_type);	/*** �жһ�Ʊ���� ***/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "0820", sDdMst.name );    /* Ϊ��ӡ������ */

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
