/*************************************************
* �� �� ��:  spB334.c
* ���������� ������гжһ�Ʊ��Ʊ����
*
* ��    ��:  jane
* ������ڣ� 2003��5��26��
*
* �޸ļ�¼�� 
*   ��   ��: 2010-6-24 11:08:19
*   �� �� ��:xyy
*   �޸�����:
1.��Ʊʱ����Э��״̬Ϊ����ʱ�ſ���(��ȫ��ǩ��)��
�±��Ϊ000�ſ���ȥ��Ʊ������Ʊ����Ӧ��֤�𷵻ضԹ�����ȥ������Э��״̬Ϊ3Э���޸�.
2.��Ʊʱ������ԭƱ��״̬������Э��״̬��������£�
a.��Ʊ��СƱ������֮ǰЭ���Ѿ�Ϊ�����ˣ����޸�Э��״̬��
b.СƱ����Ʊ������Ʊ�Ժ�Ľ��պð����е�Ʊǩ�꣬���״̬�޸�Ϊ������
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"
#include "mo_bank_po_fee_c.h"

int spB334()
{
	int     ret;
	long     day=0;
	long     HQday=0;
	int     iPactNoNum1;                /* ԭ��� */
	int     iPactNoNum2;                /* �±�� */

	char    cCg_flag[2];                /* ״̬�����־  */
	char    cAccPoNo1[17];              /* ԭ�жһ�Ʊ������Ļȡֵ */
	char    cPoNo1[9];                  /* ԭ�жһ�Ʊ������Чλ */
	char    cPoNoHead1[9];              /* ԭ�жһ�Ʊ�������ͷ */
	char    cPoNo11[17];                /* ԭ�жһ�Ʊ�������ͷ+��Чλ */
	char    cAccPoNo2[17];              /* �³жһ�Ʊ������Ļȡֵ */
	char    cPoNo2[9];                  /* �³жһ�Ʊ������Чλ */
	char    cPoNoHead2[9];              /* �³жһ�Ʊ�������ͷ */
	char    cPoNo22[17];                /* �³жһ�Ʊ�������ͷ+��Чλ */
	char    intst_flag[2];
	char    cPayeeName[51];             /* �տ���ȫ�� */
	char    cPayeeAcNo[25];             /* �տ����˺� */
	char    cPayeeBrName[51];           /* �տ��˿��������� */

	double    dPoAmt1 = 0.00;           /* ԭƱ���� */
	double    dPoAmt2 = 0.00;           /* ��Ʊ���� */
	double    dZzAmt  = 0.00;           /* ��ƱʱӦ�ôӱ�֤�𻧿ۻ������㻧�Ľ�� */
	double    dCeAmt  = 0.00;           /* ���жһ�Ʊ */
	double    HQ_rate=0.00;
	double    HQdealval=0.00;

	struct    com_parm_c            sComParm;       /* ���������� */
	struct    mo_bank_po_c          sMoBankPo;      /* �жһ�Ʊ�Ǽǲ� */
	struct    mo_bail_rate_c        sMoBailRate;    /* ��֤�����ʵǼǱ� */
	struct    mo_bail_rate_c        sMoBailRate1;   /* ��֤�����ʵǼǱ� */
	struct    mo_bank_acc_po_c      sMoBankAccPo;   /* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct    mo_bank_po_fee_c      sMobankpofee;   /*�жһ�Ʊ�շѶ����*/

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMobankpofee, 0x00, sizeof( struct mo_bank_po_fee_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	memset( cPayeeName, 0x00, sizeof( cPayeeName ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cPayeeBrName, 0x00, sizeof( cPayeeBrName ) );
	memset( cAccPoNo1, 0x00, sizeof( cAccPoNo1 ) );
	memset( cAccPoNo2, 0x00, sizeof( cAccPoNo2 ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNoHead1, 0x00, sizeof( cPoNoHead1 ) );
	memset( cPoNo11, 0x00, sizeof( cPoNo11 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNoHead2, 0x00, sizeof( cPoNoHead2 ) );
	memset( cPoNo22, 0x00, sizeof( cPoNo22 ) );
	memset( cCg_flag , 0x00 , sizeof(cCg_flag));
	memset( intst_flag, 0x00, sizeof( intst_flag ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoBankAccPo.pact_no );/* �ж�Э���� */
	get_zd_int( "0500", &iPactNoNum1 );            /* ԭ��� */
	get_zd_int( "0510", &iPactNoNum2 );            /* �±�� */
	get_zd_data( "0580", cAccPoNo1 );            /* ԭ�жһ�Ʊ���� */
	get_zd_double( "100E", &dPoAmt1 );            /* ԭƱ���� */
	get_zd_data( "0590", cAccPoNo2 );            /* �³жһ�Ʊ���� */
	get_zd_double( "1004", &dPoAmt2 );            /* ��Ʊ���� */
	get_zd_data( "0270", cPayeeName );            /* �տ���ȫ�� */
	get_zd_data( "0830", cPayeeAcNo );            /* �տ����˺� */
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	get_zd_data( "0810", cPayeeBrName );        /* �տ��˿��������� */
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__, iPactNoNum1, iPactNoNum2 );

	/* �������жһ�Ʊ�����Ƿ������ͷ */
	ret = pub_com_ChkNoteHead1( "111", cAccPoNo1 );
	if ( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankAccPo.pact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳ж�Э����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	if ( sMoBankAccPo.acc_po_sts[0] == '0' )
	{
		sprintf( acErrMsg, "�˳ж�Э��Ϊ¼��״̬!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}
	/*modified by liuxuan 2010-4-16 ����ǩ����������һ��ǩ�����ȫ��ǩ��ſ���Ʊ�����
	else if( sMoBankAccPo.acc_po_sts[0] == '1' )
	{
	sprintf( acErrMsg, "�˳ж�Э��δ��ǩ����δȫ��ǩ��!" );
	WRITEMSG
	strcpy( g_pub_tx.reply, "B211" );
	goto ErrExit;
	}
	*/
	vtcp_log("[%s][%d] dPoAmt1=[%f] dPoAmt2=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2);

	dCeAmt = dPoAmt1 - dPoAmt2;

	vtcp_log("[%s][%d] sMoBankAccPo.bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
	/*************modif by xyy 2010-6-24 12:07:39 *************/
	if (pub_base_CompDblVal(dCeAmt,0.00) < 0 )
	{
		vtcp_log("[%s][%d] СƱ[%f]����Ʊ[%f]���=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2,dCeAmt);
		/***********������Ҫ����Ƿ����㹻�ı�֤�����ȥ����Ʊ*******
		1.����ʵ�ʿ�ǩ�����  
		2.������ǩ�����      
		***************************************************************/
		double dYqBzamt = 0.00;
		double dSign_amt= 0.00;

		if (pub_base_CompDblVal(sMoBankAccPo.bail_amt , 0.00) > 0)
			dSign_amt = sMoBankAccPo.bail_amt/sMoBankAccPo.bail_amt_ratio*100;
		else
			dSign_amt = sMoBankAccPo.sign_amt;

		/* �ж����гжһ�Ʊ�Ǽǲ��ж���ͬһ��Э���ŵĻ�Ʊ���֮����¼��ʱ��Ʊ���ܽ���Ƿ���� */
		ret = sql_sum_double( "mo_bank_po", "po_amt", &dYqBzamt, "pact_no='%s' and po_sts='1'", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��sql_sum_double��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d] ʵ�ʿ�ǩ�����[%f] ��ǩ���[%f]",__FILE__,__LINE__,dSign_amt,dYqBzamt);

		if (pub_base_CompDblVal(dYqBzamt , dSign_amt) > 0)
		{
			sprintf( acErrMsg, "��ǩ�������ڿ�ǩ��������ϵ�Ƽ���!!" );
			WRITEMSG
			goto ErrExit;
		}
		/*****������Ҫ���ӵĲ��ֺͻ�����ǩ�����ֽ��бȽ�******/
		if (pub_base_CompDblVal(dPoAmt2 - dPoAmt1, dSign_amt - dYqBzamt) > 0)
		{
			sprintf( acErrMsg, "��ǩƱ�������,��֤����!!" );
			WRITEMSG
			goto ErrExit;
		}
		if (pub_base_CompDblVal(dPoAmt2 - dPoAmt1, dSign_amt - dYqBzamt) == 0)
		{
			vtcp_log("[%s][%d] ������,��ô��ǩ�����!",__FILE__,__LINE__);
			if ( sMoBankAccPo.acc_po_sts[0] == '1' )
			{
				cCg_flag[0] = 'X';
			}
			else
			{
				sprintf( acErrMsg, "Э��״̬[%s]����ȷ!",sMoBankAccPo.acc_po_sts);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	else/***�ȶƱ������Ʊ***/
	{
		vtcp_log("[%s][%d] ��Ʊ[%f]��СƱ[%f]���=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2,dCeAmt);
		if (iPactNoNum2 == 0 )
		{
			if (pub_base_CompDblVal(dPoAmt2,0.00) != 0)
			{
				sprintf( acErrMsg, "��Ʊ���Ϊ0=[%f]", dPoAmt2 );
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d] ��Ʊ�����,�±��[%d]",__FILE__,__LINE__,iPactNoNum2);
			/*****���Э���Ƿ��Ѿ�Ϊ������������״̬��������Ʊ*****/
			if ( sMoBankAccPo.acc_po_sts[0] != '2' )
			{
				/*add by gong 20110905 ���������Ŵ���У��¶Ի�Ʊ�շѶ�������û���Ŵ�����ģ�����Ϊ�Ѿ�ȫ�����������
				ret = Mo_bank_po_fee_Sel(g_pub_tx.reply ," pact_no='%s'and sts='X' ",sMoBankAccPo.pact_no);
				if(ret)
				{
				ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , sMoBankAccPo.pact_no );
				if( ret )
				{
				sprintf( acErrMsg,"Prepare Mo_bank_acc_po_Dec_Upd �α����[%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
				ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo , g_pub_tx.reply );
				if( ret==100 )
				{
				sprintf( acErrMsg,"�޴�������¼![%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L175" );
				goto ErrExit ;
				}else if( ret ){
				sprintf( acErrMsg,"fetch Mo_bank_acc_po_Fet_Upd error code = [%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
				goto ErrExit ;
				}
				strcpy( sMoBankAccPo.acc_po_sts, "2" );
				ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply  );
				if ( ret )
				{
				sprintf( acErrMsg,"Mo_bank_acc_po_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
				}
				Mo_bank_acc_po_Clo_Upd( );
				sprintf(acErrMsg,"״̬������ϣ��� PASS!");
				WRITEMSG	
				}else{
				sprintf( acErrMsg, "Э��״̬����������������Ʊ!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B213" );
				goto ErrExit;
				}
				end by gong 20110905 ���������Ŵ�*/
				sprintf( acErrMsg, "Э��״̬����������������Ʊ!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B213" );
				goto ErrExit;
			}
			dZzAmt =  dPoAmt1  * sMoBankAccPo.bail_amt_ratio /100 ;
			set_zd_double( "0410", dZzAmt );
			set_zd_double( "0420", dZzAmt );
		}
		else
		{
			vtcp_log("[%s][%d] �ȶƱ�����,�±��[%d]",__FILE__,__LINE__,iPactNoNum2);
		}
	}
	/*******************end by xyy 2010-6-24 12:07:52*****************/
	vtcp_log("[%s][%d] dCeAmt=[%lf]",__FILE__,__LINE__,dCeAmt );

	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	/* �޸ĳжһ�Ʊ�Ǽǲ� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and pact_no_num=%d", \
		sMoBankAccPo.pact_no, iPactNoNum1 );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	sprintf( acErrMsg, "����ԭ��Ʊ���![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG

	pub_base_strpack( sMoBankPo.po_no );

	/* ȡ�жһ�Ʊ����ͷ */
	strncpy( cPoNoHead1, cAccPoNo1, 8 );
	cPoNoHead1[8]='\0';
	pub_base_strpack( cPoNoHead1 );

	/* ȡ�жһ�Ʊ��Чλ */
	strncpy( cPoNo1, cAccPoNo1+8, 8 );
	cPoNo1[8] = '\0';
	sprintf( acErrMsg, "zxxxyu    cPoNo1[%s]", cPoNo1 );
	WRITEMSG

	sprintf( acErrMsg, "��ԭ��Ʊ���![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG
	/* �жһ�Ʊ�������ͷ+��Чλ */
	sprintf( cPoNo11, " %s%s ", cPoNoHead1, cPoNo1 );
	sprintf( acErrMsg, "zxxxyu    cPoNo11[%s]", cPoNo11 );
	WRITEMSG
	pub_base_strpack_all( cPoNo11 );

	sprintf( acErrMsg, "����ԭ��Ʊ���![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG
	if ( strcmp( cPoNo11, sMoBankPo.po_no ) != 0 )
	{
		sprintf( acErrMsg, "ԭ�жһ�Ʊ������ԭ��Ų���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B204" );
		goto ErrExit;
	}
	ret = pub_base_CompDblVal( dPoAmt1, sMoBankPo.po_amt );
	if ( ret !=0 )
	{
		sprintf( acErrMsg, "����ԭ��Ʊ�������![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B207" );
		goto ErrExit;
	}
	if ( sMoBankPo.po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ������Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B212" );
		goto ErrExit;
	}
	else if ( sMoBankPo.po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ�Ѻ�������������Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B228" );
		goto ErrExit;
	}
	else if ( sMoBankPo.po_sts[0] == '4' )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ�����գ���������Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B229" );
		goto ErrExit;
	}
	strcpy( sMoBankPo.po_sts, "3" );    /* �жһ�Ʊ״̬: 3 ��Ʊ */
	sMoBankPo.cash_date = g_pub_tx.tx_date;/* ������������Ϊ��Ʊ���� modified by liuxuan 2006-12-19*/

	ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_bank_po_Clo_Upd();
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );

	if ( strcmp(cAccPoNo2, "") && iPactNoNum2 == 0 )
	{
		sprintf( acErrMsg, "�±�Ų�������ԭ����ظ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B205" );
		goto ErrExit;
	}
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	/* ����¿���Ʊ��������һ����¼���жһ�Ʊ�Ǽǲ��� */
	if ( iPactNoNum2 != 0 )
	{
		if ( iPactNoNum2 == iPactNoNum1 )
		{
			sprintf( acErrMsg, "�±�Ų�������ԭ����ظ�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B205" );
			goto ErrExit;
		}
		vtcp_log( "zyzyzy333   num1[%d] num2[%d]", iPactNoNum1, iPactNoNum2 );
		/* ���ݳжһ�Ʊ��Ų������гжһ�Ʊ�ǼǱ� */
		ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s' and pact_no_num=%d", \
			sMoBankAccPo.pact_no, iPactNoNum2 );
		if ( ret == 0 )
		{
			sprintf( acErrMsg, "����Ѵ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B208" );
			goto ErrExit;
		}
		else if ( ret != 100 )
		{
			TRACE
				sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if ( strcmp( cAccPoNo2, "" ) == 0 )
		{
			sprintf( acErrMsg, "����¿���Ʊ���������³жһ�Ʊ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B232" );
			goto ErrExit;
		}
		/* �������жһ�Ʊ�����Ƿ������ͷ */
		ret = pub_com_ChkNoteHead( "111", cAccPoNo2, g_pub_tx.tel);
		if ( ret )
		{
			sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* ��ѯ�жһ�Ʊ�Ƿ��ѱ����� */
		ret = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CDHP'" );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Com_parm_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack( sComParm.val );

		vtcp_log("pono[%s]notetype[%s]brno[%s]tel[%s]",sMoBankPo.po_no, sComParm.val,g_pub_tx.tx_br_no, g_pub_tx.tel);

		/* �жһ�Ʊ���Ŵ��� */
		/**-- xxx --- @20050824@ ----**/
		g_pub_tx.svc_ind=7021;
		ret=pub_com_NoteUse( 0, 0, sComParm.val, cAccPoNo2, cAccPoNo2, g_pub_tx.tel );
		if ( ret) 
		{
			sprintf( acErrMsg, "���гжһ�Ʊ���ų���!" );
			WRITEMSG
			goto ErrExit;
		}
		if ( dPoAmt2 < 0.005 )
		{
			sprintf( acErrMsg, "����¿���Ʊ���������³жһ�Ʊ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B233" );
			goto ErrExit;
		}
		/* ȡ�жһ�Ʊ����ͷ */
		strncpy( cPoNoHead2, cAccPoNo2, 8 );
		cPoNoHead2[8]='\0';
		pub_base_strpack( cPoNoHead2);

		/* ȡ�жһ�Ʊ��Чλ */
		strncpy( cPoNo2, cAccPoNo2+8, 8 );
		cPoNo2[8] = '\0';
		sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
		WRITEMSG

		/* �жһ�Ʊ�������ͷ+��Чλ */
		sprintf( cPoNo22, " %s%s ", cPoNoHead2, cPoNo2 );
		sprintf( acErrMsg, "zxxxyu    cPoNo22[%s]", cPoNo22 );
		WRITEMSG
		pub_base_strpack_all( cPoNo22 );

		/* �����³жһ�Ʊ����������гжһ�Ʊ�ǼǱ� */
		ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo22 );
		if ( ret == 0 )
		{
			sprintf( acErrMsg, "�³жһ�Ʊ�����Ѵ���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B206" );
			goto ErrExit;
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );    /* ǩ�����к� */
		sMoBankPo.sign_date = g_pub_tx.tx_date;               /* ǩ������ */
		strcpy( sMoBankPo.pact_no, sMoBankAccPo.pact_no );    /* �жһ�ƱЭ���� */
		sMoBankPo.pact_no_num = iPactNoNum2;                  /* ��� */
		strcpy( sMoBankPo.po_no, cPoNo22 );                   /* ��Ʊ���� */
		sMoBankPo.po_amt = dPoAmt2;                           /* Ʊ���� */
		strcpy( sMoBankPo.po_sts, "1" );                      /* �жһ�Ʊ״̬: 1 ǩ�� */
		strcpy( sMoBankPo.po_ind, "1" );                      /* �жһ�Ʊ��־: 1 ���� */

		ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_po_Ins��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	/* ����տ�����Ϣ�����Ķ�������ݳж�Э�����޸ĳжһ�Ʊ������Ϣ�ǼǱ� */
	sprintf( acErrMsg, "cPayeeName=[%s],payee_name=[%s],cPayeeAcNo=[%s],payee_ac_no=[%s],\
		cPayeeBrName=[%s],payee_br_name=[%s]", cPayeeName, sMoBankAccPo.payee_name, \
		cPayeeAcNo, sMoBankAccPo.payee_ac_no, cPayeeBrName, sMoBankAccPo.payee_br_name );
	WRITEMSG

	pub_base_strpack( sMoBankAccPo.payee_name );
	pub_base_strpack( sMoBankAccPo.payee_ac_no );
	pub_base_strpack( sMoBankAccPo.payee_br_name );

	sprintf( acErrMsg, "cPayeeName=[%s],payee_name=[%s],cPayeeAcNo=[%s],payee_ac_no=[%s],\
		cPayeeBrName=[%s],payee_br_name=[%s]", cPayeeName, sMoBankAccPo.payee_name, \
		cPayeeAcNo, sMoBankAccPo.payee_ac_no, cPayeeBrName, sMoBankAccPo.payee_br_name );
	WRITEMSG

	if ( strcmp( cPayeeName, sMoBankAccPo.payee_name ) != 0 \
		|| strcmp( cPayeeAcNo, sMoBankAccPo.payee_ac_no ) != 0 \
		|| strcmp( cPayeeBrName, sMoBankAccPo.payee_br_name ) != 0 \
		|| pub_base_CompDblVal(dCeAmt,0.00) != 0 )
	{
		ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'",sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B199" );
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
		if ( strcmp( cPayeeName, sMoBankAccPo.payee_name ) != 0 )
			strcpy( sMoBankAccPo.payee_name, cPayeeName );
		if ( strcmp( cPayeeAcNo, sMoBankAccPo.payee_ac_no ) != 0 )
			strcpy( sMoBankAccPo.payee_ac_no, cPayeeAcNo );
		if ( strcmp( cPayeeBrName, sMoBankAccPo.payee_br_name ) != 0 )
			strcpy( sMoBankAccPo.payee_br_name, cPayeeBrName );
		/**--- xxx ---- 20051105 ------ �޸ı�֤���� ---**/

		vtcp_log("[%s][%d] sMoBankAccPo.bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);

		/*****************************modify by xyy 2010-6-24 15:29:12****************************/
		if ( pub_base_CompDblVal(dCeAmt,0.00) < 0 )/**СƱ����Ʊ**/
		{
			vtcp_log("[%s][%d] dCeAmt=[%f] СƱ����Ʊ,ԭ״̬[%c]!!",__FILE__,__LINE__,dCeAmt,sMoBankAccPo.acc_po_sts[0]);
			if (cCg_flag[0] == 'X' )
			{
				strcpy( sMoBankAccPo.acc_po_sts, "2" );/***СƱ����Ʊ,����˱�����***/
				vtcp_log("[%s][%d]  СƱ����Ʊ,����˱�����[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_sts);
			}
		}
		else
		{
			vtcp_log("[%s][%d] ��Ҫ�ж�����Ʊ���ǻ�Ʊ,�±��[%d]!",__FILE__,__LINE__,iPactNoNum2);
			if (iPactNoNum2 == 0 )/**��Ʊ**/
			{
				if (pub_base_CompDblVal(dPoAmt2,0.00) != 0)
				{
					sprintf( acErrMsg, "��Ʊ���Ϊ0=[%f]", dPoAmt2 );
					WRITEMSG
					goto ErrExit;
				}
				double dPoTmp=0.00;
				vtcp_log("[%s][%d] ��ƱҪ����ԭ��֤��!!!",__FILE__,__LINE__,iPactNoNum2);

				if (pub_base_CompDblVal(sMoBankAccPo.bail_amt , 0.00) > 0)
				{
					dPoTmp = pub_base_PubDround_1(dPoAmt1  * sMoBankAccPo.bail_amt_ratio /100) ;
					vtcp_log("[%s][%d] ԭƱ��[%f] ����[%f] ���[%f]",__FILE__,__LINE__,dPoAmt1,sMoBankAccPo.bail_amt_ratio,dPoTmp);
					sMoBankAccPo.bail_amt=sMoBankAccPo.bail_amt - dPoTmp;
				}
			}
			else/***��Ʊ��СƱ***/
			{
				vtcp_log("[%s][%d] dCeAmt=[%f] ��Ʊ��СƱ��ȶƱ,״̬[%c]!!",__FILE__,__LINE__,dCeAmt,sMoBankAccPo.acc_po_sts[0]);

				if (sMoBankAccPo.acc_po_sts[0] == '2' && pub_base_CompDblVal(dCeAmt,0.00) > 0)
				{
					vtcp_log("[%s][%d] Э���״̬=[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_sts);
					strcpy( sMoBankAccPo.acc_po_sts, "1" );/***��Ʊ��СƱ����������Ϊ���***/
				}
			}
		}
		/****************************end by xyy 2010-6-24 15:29:26*******************************/
		ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Mo_bank_acc_po_Clo_Upd();
	}
	/* ����Ʊͬʱ�¿���Ʊʱ����Ҫ�ټ�һ����ˮ */
	if ( iPactNoNum2 != 0 )
	{
		/* �жһ�Ʊ����Ҫ��ӡ�Ĳ��̶ֹ����� */
		set_zd_data( "0740", "��������" );        /* ������ȫ�� */
		set_zd_data( "0310", "401175056393" );            /* �������к� */
		set_zd_data( "0820", "ɽ��ʡ�������ܴ�����˳�ǽ�129��" );    /* �����е�ַ */
	}
	g_pub_tx.tx_amt1 = dZzAmt;
	/** begin add by chenchao 2011/11/23 20:19:00 **/ 
	ret = sql_count("mo_bank_po_fee","num=%d and pact_no='%s' ",iPactNoNum1,sMoBankAccPo.pact_no);
	if(ret>0)
	{
		ret = sql_execute("update mo_bank_po_fee set new_num = %ld where num=%ld and pact_no='%s' ",iPactNoNum2,iPactNoNum1,sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( g_pub_tx.reply, "H034" );
			sprintf( acErrMsg, "����Ʊ����ϸ��ʧ��[%d]", ret );
			WRITEMSG
			goto ErrExit; 
		}
	}else if(ret == 0)
	{
		ret = sql_execute("update mo_bank_po_fee set new_num = %ld where new_num=%ld and pact_no='%s' ",iPactNoNum2,iPactNoNum1,sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( g_pub_tx.reply, "H034" );
			sprintf( acErrMsg, "����Ʊ����ϸ��ʧ��[%d]", ret ); 
			WRITEMSG
			goto ErrExit; 
		}
	}else{
		sprintf( g_pub_tx.reply, "H034" );
		sprintf( acErrMsg, "���bank_po_feeʧ��[%d]", ret ); 
		WRITEMSG
		goto ErrExit; 
	}
	/** end add by chenchao 2011/11/23 20:19:00 **/
	
	/* ����Ƿ��б�֤���������������ȡ���¼�����û����ֻ�޸ĵǼǲ���û�������� */
	get_zd_data( "0320", sMoBankAccPo.bail_ac_no );
	/***
	if ( strcmp( sMoBankAccPo.bail_ac_no, "" ) != 0 && sMoBankAccPo.bail_amt>0.005)
	***/
	if ( strcmp( sMoBankAccPo.bail_ac_no, "" ) != 0 && dZzAmt >0.005)/*** �ж�ȡ���� mod by chenchao 2013/3/20 19:58:23 **/
	{
		/**--- xxx ---- 20051104 ---- ���㱣֤����Ϣ ----**/
		if ( pub_base_CompDblVal(dZzAmt,0.00) < 0 )
		{
			/**sprintf( acErrMsg, "�¿��жһ�Ʊ���ܳ���ԭ�жһ�Ʊ��� ԭ=[%.2f]��=[%.2f]",dPoAmt1,dPoAmt2);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B341" );
			goto ErrExit;***/
			goto OkExit;
		}
		if ( pub_base_CompDblVal(dZzAmt,0.00) == 0 )
			goto OkExit;

		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply," bail_type='1' and pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if ( ret==100 )    
		{
			strcpy(intst_flag,"0");
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;                    /** Ӧ˰��Ϣ **/
			g_pub_intst.factval=0.00;                    /** ʵ����Ϣ **/
			g_pub_intst.val=0.00;                        /** ��Ϣ˰ **/
			g_pub_intst.keepval=0.00;                    /** ��ֵ��Ϣ **/
			set_zd_data("0153","52101");                /** ��֤����Ϣ **/
			set_zd_double("102P",g_pub_intst.dealval);
			goto OkExit1;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		memcpy(&sMoBailRate1,&sMoBailRate,sizeof(sMoBailRate));
		ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
		if ( ret )
		{
			sprintf( acErrMsg, "ȡ���ʴ���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}
		/** �ж��Ƿ��� **/
		if ( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** ��ǰ֧ȡ **/
		{
			/** �������� **/
			if ( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
			{
				sprintf( acErrMsg, "������������!");
				WRITEMSG
				goto ErrExit;
			}
			day=0;
			/** ������Ϣ **/
			if ( pub_base_CompDblVal(dZzAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; ����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dZzAmt * HQday / L360 / L100;����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * dZzAmt * HQday / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dZzAmt;
				sMoBailRate.amt=dZzAmt;
			}
		}
		else
		{
			/** �������� **/
			if ( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
			{
				sprintf( acErrMsg, "������������!");
				WRITEMSG
				goto ErrExit;
			}
			/** ������Ϣ **/
			if ( pub_base_CompDblVal(dZzAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; ����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dZzAmt * HQday / L360 / L100;����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * dZzAmt * day / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dZzAmt;
				sMoBailRate.amt=dZzAmt;
			}
		}
		sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);

		g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
		if ( pub_base_CompDblVal(g_pub_intst.dealval,0.00)<0 )
		{
			sprintf(acErrMsg,"������Ϣ�������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}
		intst_flag[0]='1'; 
		/** ���±�֤���Ϣ��� **/
		sMoBailRate.intst_date=g_pub_tx.tx_date;
		sMoBailRate.end_trace_no=g_pub_tx.trace_no;
		sMoBailRate.intst_amt=g_pub_intst.dealval;
		strcpy(sMoBailRate.sts,"*");
		ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		Mo_bail_rate_Clo_Upd();

		/** �����µı�֤����Ϣ�ǼǱ� **/
		vtcp_log("[%s][%d]ע��: �±�֤��=[%f]",__FILE__,__LINE__,sMoBailRate1.amt);
		if ( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
		{
			ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bail_rate_Ins��![%d]",ret);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
		}
		g_pub_intst.tax_intst=g_pub_intst.dealval;    /** Ӧ˰��Ϣ **/
		g_pub_intst.factval=g_pub_intst.dealval;    /** ʵ����Ϣ **/ 
		g_pub_intst.val=0.00;                        /** ��Ϣ˰ **/
		g_pub_intst.keepval=0.00;                    /** ��ֵ��Ϣ **/

		dZzAmt=dZzAmt+g_pub_intst.dealval;
		set_zd_double("0420",dZzAmt);
		set_zd_data("0153","52101");                /** ��֤����Ϣ **/
		set_zd_double("102P",g_pub_intst.dealval);
		goto OkExit1;
	}else if ( pub_base_CompDblVal(dZzAmt,0.00) > 0 )/***������0 ��Ҫ���� mod by chenchao 2013/3/20 19:58:59**/
	{
		sprintf( acErrMsg, "����Լ0ȥ����[%.2lf]",dZzAmt);
		WRITEMSG
		goto OkExit1;
	}
	
OkExit:
	/**----- xxx ---- @20050819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	strcpy( g_pub_tx.note_type, sComParm.val );
	strcpy( g_pub_tx.beg_note_no, cPoNo11 );
	strcpy( g_pub_tx.brf, cPoNo22 );
	g_pub_tx.svc_ind=0;
	strcpy(g_pub_tx.no_show,"1");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	/**----- xxx ---- @20050819@ ---- ��������Ҫ��Э���� ----**/
		strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	strcpy( g_pub_tx.note_type, sComParm.val );
	strcpy( g_pub_tx.beg_note_no, cPoNo11 );
	strcpy( g_pub_tx.brf, cPoNo22 );
	g_pub_tx.svc_ind=0;
	strcpy(g_pub_tx.no_show,"1");
	if ( intst_flag[0]=='1' )        /*** ��֤���Ϣ�� ***/
	{
		strcpy(g_pub_tx.note_type,"Y");
	}
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if( strncmp(g_pub_tx.reply,"0000",4) == 0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
