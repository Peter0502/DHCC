/*************************************************
* �� �� ��:  spB337.c
* ���������� �������нӵ���Ʊ�˿����м�����ί���տ�
*	     ƾ֤����Ʊ����к���,һ���ǵ��ں������
*	     ���������Ʊ���ڵ��ڵ������Ҹ�����24403
*	     �跽����Ⱥ�����Ʊ����������Ϊ�㣩��
*	     ���մ����Ʊ���ڡ�
*
* ��    ��:  jane
* ������ڣ� 2003��5��30��
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
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"
#include "com_item_c.h"

int spB337()
{
	struct	com_item_c		wd_com_item;	/* �жһ�Ʊ�Ǽǲ� */
	struct	com_parm_c		wd_com_parm;	/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_po_c		sMoBankPo;	/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_acc_po_c	sMoBankAccPo;/* ���гжһ�Ʊ������Ϣ�ǼǱ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;	/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 			sDdMst;		/* �������ļ� */
	double	dPoAmt;							/* Ʊ���� */
	char	cCtInd[2];						/* ��ת��־ */
	char	cPoNo[17];						/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];					/* �жһ�Ʊ�������ͷ */
	char    cPoNo2[17];					/* �жһ�Ʊ�������ͷ+��Чλ */
	char    cPoNo4[9];					/* �жһ�Ʊ������Чλ����Ч���� */
	int		iPoNo;						/* �жһ�Ʊ������Чλ����Ч���� */
	char	cAccHrt[6];						/* ͬ�ǿ�Ŀ������ */
	char	cAcNo[25];						/* ת����Ļ�������ת���˺� */
	char	cTemp[30];						/* Ϊ��ӡ�ж�Э���żӱ�Ŷ������ʱ���� */
	int		ret;
	int     parm_ind = 0;     
	char	cPact_no[21];
	double	dSum_amt=0.00;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cCtInd, 0x00, sizeof( cCtInd ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo4, 0x00, sizeof( cPoNo4 ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cTemp, 0x00, sizeof( cTemp ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ
	get_zd_data( "0580", sMoBankPo.po_no );	* �жһ�Ʊ���� *
	sMoBankPo.cash_date = g_pub_tx.tx_date;		* �������� *
	get_zd_double( "100E", &dPoAmt );		* Ʊ���� *
	get_zd_data( "0670", cCtInd );			* ��ת��־ *
	get_zd_data( "1021", cAcNo );			* ת���˺� *
	**************************/
	get_zd_data("0370",cPact_no);		/*** �ж�Э��� **/
	/****
	* �������жһ�Ʊ�����Ƿ������ͷ *
	ret = pub_com_ChkNoteHead1( "111", sMoBankPo.po_no );
	if ( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	* ȡ�жһ�Ʊ����ͷ *
	strncpy( cPoNoHead, sMoBankPo.po_no, 8 );
	cPoNoHead[8] = '\0';
	pub_base_strpack( cPoNoHead );

	 ȡ�жһ�Ʊ��Чλ 
	strncpy( cPoNo, sMoBankPo.po_no+8, 8 );
	cPoNo[8] = '\0';
	set_zd_data( "0610", cPoNo );
	sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
	WRITEMSG

	 Ϊ�����Ǽǲ���ֵ��׼����ֻ����Чλ�е���Ч���� 
	iPoNo = atol( cPoNo );				 ����Чλת�������� 
	sprintf( cPoNo4, "%d", iPoNo );			�ٽ���ת�����ַ��� 
	pub_base_strpack_all( cPoNo4 );
	set_zd_data( "0620", cPoNo4 );
	sprintf( acErrMsg, "zxxxyu    iPoNo[%d]  cPoNo4[%s]",iPoNo, cPoNo4 );
	WRITEMSG

	 �жһ�Ʊ�������ͷ+��Чλ��Ϊ��ӡ�жһ�Ʊ��׼�� 
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
	pub_base_strpack_all( cPoNo2 );
	set_zd_data( "0310", cPoNo2 );
	sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
	WRITEMSG

	 �޸ĳжһ�Ʊ�ǼǱ���״̬��Ϊ���� */
	
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_sts not in ('2','3')", sMoBankAccPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		memset(&sMoBankPo,0x00,sizeof(sMoBankPo));
		ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if ( sMoBankPo.po_sts[0] != '4' )
		{
			sprintf( acErrMsg, "�˳жһ�Ʊδ�����յǼ�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B221" );
			goto ErrExit;
		}
		/* Ϊ��ӡ���гжһ�Ʊ�跽��Ʊ��׼�� */
		pub_base_strpack( sMoBankPo.pact_no );	
		sprintf( cTemp, "%s--%d", sMoBankPo.pact_no, sMoBankPo.pact_no_num );
		set_zd_data( "0330", cTemp );	
	
		if ( g_pub_tx.tx_date < sMoBankAccPo.matr_date )
		{
			sprintf( acErrMsg, "ֻ���ڻ�Ʊ�����յ������֮����к���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B227" );
			goto ErrExit;
		}
	
		/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ���״̬Ϊ���� */
		strcpy( sMoBankPo.po_sts, "2" );		/* ��־��2 ���� */
		sMoBankPo.cash_date=g_pub_tx.tx_date;
		
		ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* Ϊ�ǻ����ˮʱ��ֵ�� */
		strcpy( g_pub_tx.note_type, "111" );				/* ���гжһ�Ʊ */
		strcpy( g_pub_tx.beg_note_no, sMoBankPo.po_no );	/* ���гжһ�Ʊ���� */
        	strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
		/* ����ˮ��־ */
		/**----- xxx ---- 20030819 ---- ��������Ҫ��Э���� ----**/
		g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    /* �жһ�ƱƱ���� */
		g_pub_tx.svc_ind=0;
		strcpy( g_pub_tx.brf, "���гжһ�Ʊ����" );
		strcpy(g_pub_tx.no_show,"0");
		
		ret = pub_ins_trace_log();
		if ( ret )
		{
			sprintf( acErrMsg, "����ˮ��־��" );
			WRITEMSG
			goto ErrExit;
		}
		dSum_amt += sMoBankPo.po_amt;
	}
	Mo_bank_po_Clo_Upd();
	set_zd_double("100E",dSum_amt);
	if (sMoBankAccPo.acc_po_sts[0] == '9')   /** ���л�Ʊ **/
		parm_ind = 6;
	else
	{
		switch(sMoBankAccPo.acc_type[0])
		{  
		case '1':   /** ���ж� **/
			parm_ind = 1;
			break;
		case '2':   /** ��ѹ�浥 **/
			parm_ind = 2;
			break;
		case '3':   /** 100%��֤�� **/
			parm_ind = 3;
			break;
		case '4':   /** �����ж� **/
			parm_ind = 4;
			break;
		case '5':   /** ��Ʊ��СƱ **/
			parm_ind = 5;
			break;
		default:
			sprintf( acErrMsg, "[%s][%d]��Ʊ���ʹ���![%s]",__FILE__,__LINE__,sMoBankAccPo.acc_type);
			WRITEMSG
			strcpy( g_pub_tx.reply, "T068" );
			goto ErrExit;

		}
	}
	/** �Ӳ�����com_parmȡ�ò�ͬ���ͳжһ�Ʊ�Ŀ�Ŀ **/
	ret = Com_parm_Sel(g_pub_tx.reply,&wd_com_parm,"parm_code='CDHPR' and parm_seqn=%d",parm_ind);
	if (ret)
	{
		sprintf( acErrMsg, "[%s][%d]��ѯ���������![%d]",__FILE__,__LINE__, ret );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(wd_com_parm.val);
	/** �жһ�Ʊ�Ŀ�Ŀ **/
	ret = Com_item_Sel(g_pub_tx.reply,&wd_com_item,"acc_no='%s'",wd_com_parm.val);
	if (ret)
	{
		sprintf( acErrMsg, "[%s][%d]��ѯ��Ŀ����![%d][%s]",__FILE__,__LINE__, ret,wd_com_parm.val );
		WRITEMSG
		goto ErrExit;
	}
	
	sprintf( acErrMsg, "�жҿ�ĿΪ:[%s][%s]",wd_com_parm.val,wd_com_item.acc_hrt );
	WRITEMSG

	set_zd_data( "0640", wd_com_item.acc_hrt );     /* �жһ�Ʊ���ʿ�Ŀ */
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
