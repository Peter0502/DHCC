/*************************************************
* �� �� ��:  spB108.c
* ���������� ������л�Ʊ��ʧ/��ҵĹ���
*
* ��    ��:  Terry
* ������ڣ� 2003��1��19��
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
#include "mo_loss_c.h"

int spB108()
{
	struct	mo_bank_po_c	sMoBankPo;	/* ���л�Ʊ�Ǽǲ� */
	struct  mo_loss_c	sMoLoss;	/* ��ʧ��ҵǼǱ� */
	char	cIdType[3];			/* ֤������ */
	char	cIdNo[21];			/* ֤������ */
	char	cName[51];			/* ���������� */
	char	cFlag[2];			/* ��ʧ��ұ�־0��ʧ1��� */
	char	cSts[2];			/* ״̬ */
	char	cPerSts[2];			/* ǰ��״̬ */
	char	cOperCode[6];				/* ҵ����� */
	int	ret;
	
	/* ���ݳ�ʼ�� */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoLoss, 0x00, sizeof( struct mo_loss_c ) );
	
	memset( cIdType, 0x00, sizeof ( cIdType ) );
	memset( cIdNo, 0x00, sizeof ( cIdNo ) );
	memset( cName, 0x00, sizeof ( cName ) );
	memset( cFlag, 0x00, sizeof ( cFlag ) );
	memset( cSts, 0x00, sizeof ( cSts ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cFlag );			/* ��ʧ��ұ�־0��ʧ1��� */
	get_zd_data( "0590", sMoBankPo.po_no );		/* ��Ʊ���� */
	get_zd_data( "0680", cIdType );			/* ֤������ */
	get_zd_data( "0620", cIdNo );			/* ֤������ */
	get_zd_data( "0250", cName );			/* ������ȫ�� */
	strcpy( cOperCode, "10001" );
	
	pub_base_strpack( sMoBankPo.po_no );
	
	if ( cFlag[0] == '0' )		/* ��ʧ */
	{
		strcpy( cPerSts, "1" );
		strcpy( cSts, "0" );
	}else				/* ��� */
	{
		strcpy( cPerSts, "0" );
		strcpy( cSts, "1" );
	}
	/* ���һ�Ʊ�Ƿ���� */
	ret = sql_count( "mo_bank_po", "po_no = '%s'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "�޴����л�Ʊ" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}
	
	/* ����Ʊ״̬���߱�־�Ƿ����� */
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_ind = '%s'", \
					sMoBankPo.po_no , cPerSts );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "���л�Ʊ��־������ret=[%d]",ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B032" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "���л�Ʊ�ǼǱ����ڶ�����¼" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B019" );
		goto ErrExit;	
	}

	/* �������α� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s' \
				and po_ind = '%s'", sMoBankPo.po_no, cPerSts );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ����޴����л�Ʊ" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
			
	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻����Ƿ���ǩ���д�" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* �޸Ļ�Ʊ�ǼǱ� */
	strcpy( sMoBankPo.po_ind, cSts );		/* ��Ʊ��־ */	
	
	/* �޸Ļ�Ʊ�ǼǱ� */
	ret = Mo_bank_po_Upd_Upd( sMoBankPo , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����л�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B017" );
		goto ErrExit;	
	}
	
	Mo_bank_po_Clo_Upd( );
	
	/* �Ǽǹ�ʧ�ǼǱ� ,˭��ʧ˭��� */
	strcpy( sMoLoss.ac_no, sMoBankPo.po_no );/* ��Ʊ���� */
	strcpy( sMoLoss.id_type, cIdType);	/* ֤������ */
	strcpy( sMoLoss.id_no, cIdNo );		/* ֤������ */
	strcpy( sMoLoss.name, cName );		/* ���������� */

	if ( cFlag[0] == '0' )
	{
		strcpy( sMoLoss.loss_ind , "1" );	/* ��ʧ��־ */
		sMoLoss.loss_date = g_pub_tx.tx_date;	/* ��ʧ���� */
		strcpy( sMoLoss.loss_br_no, g_pub_tx.tx_br_no );	/* ��ʧ���� */
		strcpy( sMoLoss.loss_tel, g_pub_tx.tel );	/* ��ʧ��Ա */
	
		ret = Mo_loss_Ins( sMoLoss, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�Ǽǹ�ʧ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	else 
	{
		pub_base_strpack(sMoLoss.ac_no);
		/* ��ѯ��ʧ�ǼǱ� */
		ret = Mo_loss_Dec_Upd( g_pub_tx.reply , "ac_no = '%s' \
				and id_type = '%s' and id_no = '%s' \
				and loss_ind = '1' and unloss_ind IS NULL" ,\
				sMoLoss.ac_no, sMoLoss.id_type, sMoLoss.id_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_loss_Dec_Upd����" );
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Mo_loss_Fet_Upd( &sMoLoss, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "û�ҵ���ʧ��¼" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B033" );
			goto ErrExit;	
		}else if ( ret )
		{
			sprintf( acErrMsg, "ȡ��ʧ��¼����[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
				
		/* �Ǽǽ�ҵǼǱ� */
		strcpy( sMoLoss.unloss_ind , "1" );	/* ��ұ�־ */
		sMoLoss.unloss_date = g_pub_tx.tx_date;	/* ������� */
		strcpy( sMoLoss.unloss_tel, g_pub_tx.tel );	/* ��ҹ�Ա */		
		
		ret = Mo_loss_Upd_Upd( sMoLoss, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸Ĺ�ʧ�ǼǱ���" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B034" );
			goto ErrExit;	
		}
		
		Mo_loss_Clo_Upd( );	
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

