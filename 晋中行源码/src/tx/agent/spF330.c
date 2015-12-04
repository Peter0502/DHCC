/*************************************************
* �� �� ��:  spF330.c
* ���������� ��֤��ȡ���
*
* ��    ��:  mike
* ������ڣ� 2004��11��6��
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
#include "protect_note_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"

int spF330()
{
	struct	protect_note_c		sProtectNote;	/* ����Э��Ǽǲ� */
	struct	mo_bail_rate_c		sMoBailRate;	/* ��֤�����ʵǼǱ� */
	struct	mo_bail_rate_c		sMoBailRate1;	/* ��֤�����ʵǼǱ� */
	double	dTempAmt=0.00;						/* ��ƱʱӦ�ôӱ�֤�𻧿ۻ������㻧�Ľ�� */
	int		ret;
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	HQdealval=0.00;
	char	bail_ac_no[25];
	char	intst_ac_no[25];
	char	intst_flag[2];

	/* ���ݳ�ʼ�� */
	memset( &sProtectNote, 0x00, sizeof( struct protect_note_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sProtectNote.pact_no );		/* ����Э���� */
	get_zd_data( "0320", bail_ac_no );					/* ��֤���ʺ� */
	get_zd_data( "0310", intst_ac_no );					/* ��Ϣ�ʺ� */
	get_zd_double( "0390", &dTempAmt );					/* ȡ���� */
	get_zd_data( "0670" , intst_flag );					/* ��Ϣ��־ */
	
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Protect_note_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sProtectNote.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Protect_note_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Protect_note_Fet_Upd( &sProtectNote, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˱���Э���Ų�����!" );
       	WRITEMSG
        strcpy( g_pub_tx.reply, "B351" );
        goto ErrExit;
    }
	else if( ret )
    {
    	sprintf( acErrMsg, "ִ��Protect_note_Fet_Upd��![%d]", ret );
       	WRITEMSG
        goto ErrExit;
    }

/*
	if( sProtectNote.portect_sts[0] != '0' )
	{
		sprintf( acErrMsg, "�˳ж�Э��Ϊ¼��״̬!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}
*/
	if( pub_base_CompDblVal(dTempAmt,sProtectNote.bail_amt) > 0 )
	{
		sprintf( acErrMsg, "ȡ�������![%.2f][%.2f]",dTempAmt,sProtectNote.bail_amt );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P115" );
		goto ErrExit;
	}
	pub_base_strpack(sProtectNote.bail_ac_no);
	pub_base_strpack(bail_ac_no);
	if( strcmp(sProtectNote.bail_ac_no,bail_ac_no) )
	{
		sprintf( acErrMsg, "Э���Ŷ�Ӧ�ı�֤���ʺ���¼�뱣֤���ʺŲ�һ��!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B329" );
		goto ErrExit;
	}

	/* �޸ı���Э����Ϣ�Ǽǲ� */
	if( pub_base_CompDblVal(dTempAmt,0.00) > 0 )
		sProtectNote.bail_amt=sProtectNote.bail_amt-dTempAmt;

	ret = Protect_note_Upd_Upd( sProtectNote, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Protect_note_Clo_Upd();

	/** ��������Ҫ��Э���� **/
	strcpy(g_pub_tx.ac_no,sProtectNote.pact_no);
	g_pub_tx.tx_amt1 = dTempAmt;
	strcpy( g_pub_tx.beg_note_no, bail_ac_no );
	strcpy( g_pub_tx.brf, "��֤��ȡ��" );
	g_pub_tx.svc_ind=0;
	strcpy( g_pub_tx.note_type,intst_flag );	/** �������Ƿ��Ϣ�� **/
	strcpy(g_pub_tx.no_show,"1");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");

	if( intst_flag[0]=='Y' )
	{
		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1' ", sProtectNote.pact_no );
   		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d][%s]",ret,sProtectNote.pact_no);
       		WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
       		goto ErrExit;
   		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
   		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d][%s]",ret,sProtectNote.pact_no);
       		WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
       		goto ErrExit;
   		}
		memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );

		pub_base_strpack(sMoBailRate.ac_no);
		pub_base_strpack(intst_ac_no);
		if( strcmp(sMoBailRate.ac_no,intst_ac_no) )
		{
			sprintf( acErrMsg, "�������Ϣ�ʺ�������Ĳ�һ��![%s][%s]",sMoBailRate.ac_no,intst_ac_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B343" );
			goto ErrExit;
		}

		ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
		if( ret )
		{
			sprintf( acErrMsg, "ȡ���ʴ���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}

		/** �ж��Ƿ��� **/
		if( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** ��ǰ֧ȡ **/
		{
			/** �������� **/
			if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
   			{
   				sprintf( acErrMsg, "������������!");
       			WRITEMSG
       			goto ErrExit;
   			}
			day=0;
		}
		else
		{
			/** �������� **/
			if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
   			{
   				sprintf( acErrMsg, "������������!");
       			WRITEMSG
       			goto ErrExit;
   			}
			if( pub_base_CalDays(sMoBailRate.end_date,g_pub_tx.tx_date,"0",&HQday) )
   			{
   				sprintf( acErrMsg, "������������!");
       			WRITEMSG
       			goto ErrExit;
   			}
   		}

sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);
		/** ������Ϣ **/
		if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
		{
       		HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
       		g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
			g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
			sMoBailRate1.amt=0.00;
		}
		else
		{
       		HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;
       		g_pub_intst.dealval = sMoBailRate.ic_rate * dTempAmt * day / L360 / L100;
			g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
			sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
			sMoBailRate.amt=dTempAmt;
		}
		g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
		if ( pub_base_CompDblVal(g_pub_intst.dealval)<0 )
		{
			sprintf(acErrMsg,"������Ϣ�������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}

		/** ���±�֤���Ϣ��� **/
		sMoBailRate.intst_date=g_pub_tx.tx_date;
		sMoBailRate.end_trace_no=g_pub_tx.trace_no;
		sMoBailRate.intst_amt=g_pub_intst.dealval;
		strcpy(sMoBailRate.sts,"*");
		ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
		if( ret )
   		{
   			sprintf( acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d][%s]",ret,sProtectNote.pact_no);
       		WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
       		goto ErrExit;
   		}
		Mo_bail_rate_Clo_Upd();

		/** �����µı�֤����Ϣ�ǼǱ� **/
		if( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
		{
			ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
			if( ret )
   			{
   				sprintf( acErrMsg, "ִ��Mo_bail_rate_Ins��![%d]",ret);
       			WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
       			goto ErrExit;
   			}
		}
		g_pub_intst.tax_intst=g_pub_intst.dealval;	/** Ӧ˰��Ϣ **/
		g_pub_intst.factval=g_pub_intst.dealval;	/** ʵ����Ϣ **/
		g_pub_intst.val=0.00;						/** ��Ϣ˰ **/
		g_pub_intst.keepval=0.00;					/** ��ֵ��Ϣ **/

		dTempAmt=dTempAmt+g_pub_intst.dealval;
		set_zd_data("0153","52101");				/** ��֤����Ϣ **/
		set_zd_double("102P",g_pub_intst.dealval);
	}
	else
	{
		g_pub_intst.dealval=0.00;
		g_pub_intst.tax_intst=0.00;					/** Ӧ˰��Ϣ **/
		g_pub_intst.factval=0.00;					/** ʵ����Ϣ **/
		g_pub_intst.val=0.00;						/** ��Ϣ˰ **/
		g_pub_intst.keepval=0.00;					/** ��ֵ��Ϣ **/
		set_zd_data("0153","52101");				/** ��֤����Ϣ **/
		set_zd_double("102P",g_pub_intst.dealval);
	}
	set_zd_double("0410",dTempAmt);

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
