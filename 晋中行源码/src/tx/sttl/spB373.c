/*************************************************
* �� �� ��:  spB373.c
* ���������� ��֤��ȡ���
*
* ��    ��:  
* ������ڣ� 2003��11��6��
*
* �޸ļ�¼�� 
*   ��   ��: 20110909
*   �� �� ��: chenchao
*   �޸�����: ������������,��ȡ��ʱͨ���Ŵ�����,ȡ����Ϊ�Ŵ��������,Ӧ��Ϊ������֤������ǵ��ڴ���Ľ��
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"
#include "dd_mst_c.h"

int spB373()
{
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_bail_rate_c		sMoBailRate;	/* ��֤�����ʵǼǱ� */
	struct	mo_bail_rate_c		sMoBailRate1;	/* ��֤�����ʵǼǱ� */
	struct	mo_bail_rate_c		sMoBailRate_cc;
	struct	dd_mst_c		sDd_mst;
	double	dTempAmt=0.00;						/* ��ƱʱӦ�ôӱ�֤�𻧿ۻ������㻧�Ľ�� */
	int		ret;
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	dSum_bail_amt=0.00;
	double	HQdealval=0.00;
	char	bail_ac_no[25];
	char	intst_ac_no[25];
	char	intst_flag[2];
	char	cBail_type[2];

	/* ���ݳ�ʼ�� */
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate_cc, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	memset (cBail_type,0x00,sizeof(cBail_type) );
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoBankAccPo.pact_no );				/* �ж�Э���� */
	get_zd_data( "0320", bail_ac_no );					/* ��֤���ʺ� */
	get_zd_data( "0310", intst_ac_no );					/* ��Ϣ�ʺ� */
	get_zd_double( "0390", &dTempAmt );					/* ȡ���� */
	get_zd_data( "0670" , intst_flag );					/* ��Ϣ��־ */
	get_zd_data( "0680", cBail_type );					/* ��֤������ */

	/** ���ʺ� **/
	pub_base_old_acno( bail_ac_no );
	pub_base_old_acno( intst_ac_no );
	
	zip_space(sMoBankAccPo.pact_no);
	
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s')",bail_ac_no);
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Sel��![%d][%s]",ret,bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(memcmp(sDd_mst.prdt_no,"131",3) && memcmp(sDd_mst.prdt_no,"142",3) )
	{
		sprintf( acErrMsg, "�Ǳ�֤���˻��������ô˽���![%d][%s]",ret,bail_ac_no);
		WRITEMSG
		sprintf( g_pub_tx.reply, "B334" );
		goto ErrExit;
	}
	if(cBail_type[0] == '1')
	{	
		if(strlen(sMoBankAccPo.pact_no)>0 )
		{
			/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
			ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
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
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		
		}else{
			ret = sql_sum_double("mo_bail_rate","amt",&dSum_bail_amt,"bail_ac_no='%s' and sts='1'",bail_ac_no);
			if(ret)
			{
				sprintf( acErrMsg, "ִ��sql_sum_double��![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "AT03" );
				goto ErrExit;
			}
			if(sDd_mst.bal-dSum_bail_amt-dTempAmt<-0.005)
			{
				sprintf( acErrMsg, "ȡ�������![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "A030" );
				goto ErrExit;
			}
		
			/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
			ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "bail_ac_no='%s' and acc_po_sts!='2' and acc_po_sts!='3'", bail_ac_no );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "û��ǩ��Э�鲻��Ҫ�޸ļ�¼,ֱ�Ӽ���!" );
				WRITEMSG
				set_zd_double("0410",dTempAmt);

				goto OkExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		/*****
		double dPoAmt=0.00; **��ǩ���жһ�Ʊ���**
		double dYqBzamt =0.00;**ǩ���жһ�Ʊ���ڱ�֤�����**
		ret = sql_sum_double("mo_bank_po","po_amt",&dPoAmt,"pact_no='%s' and po_sts='1'",sMoBankAccPo.pact_no);
		if (ret) {
			sprintf(acErrMsg, "ִ��sql_sum_double��[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"��ǩ���Ļ�Ʊ����ܶ�=[%lf],sign_amt=[%lf]",dPoAmt,sMoBankAccPo.sign_amt);
        	WRITEMSG
        	***  ��֤����ǩ������û����  ����������֤��ȡ�� ***
		if(dPoAmt > 0.005 && sMoBankAccPo.matr_date > g_pub_tx.tx_date)
		{
			sprintf( acErrMsg, "��֤��δ���ڲ�����ȡ��!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B302" );
			goto ErrExit;
		}
		dYqBzamt = pub_base_PubDround_1(dPoAmt*sMoBankAccPo.bail_amt_ratio/100.00);
		sprintf(acErrMsg,"��ǩ����֤����=[%lf],bail_amt=[%lf]",dYqBzamt,sMoBankAccPo.bail_amt);
        	WRITEMSG
        	
		if( pub_base_CompDblVal(dTempAmt,sMoBankAccPo.bail_amt - dYqBzamt) > 0 )
		{
			sprintf( acErrMsg, "�˳ж�Э��ȡ�������![%.2f][%.2f]",dTempAmt,dYqBzamt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B342" );
			goto ErrExit;
		}
        	
		if( pub_base_CompDblVal(dTempAmt,sMoBankAccPo.bail_amt) > 0 )
		{
			sprintf( acErrMsg, "�˳ж�Э��ȡ�������![%.2f][%.2f]",dTempAmt,sMoBankAccPo.bail_amt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B342" );
			goto ErrExit;
		}
		**************end by chenchao ���Ŵ����� ���Ҵ��ڶ�������� Ҫȫ��ȡ�� ��У�� 20110908**************************/
		pub_base_strpack(sMoBankAccPo.bail_ac_no);
		pub_base_strpack(bail_ac_no);
		/*	���Ƚ��ˣ���Ϊ�ۿ��Ǵ�����ı�֤�𻧿�Ǯ�� 2006-12-11 liuxuan*//*���н����ж� 2009/12/23 12:03:27 modify by martin */
		if( strcmp(sMoBankAccPo.bail_ac_no,bail_ac_no) )
		{
			sprintf( acErrMsg, "Э���Ŷ�Ӧ�ı�֤���ʺ���¼�뱣֤���ʺŲ�һ��!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B329" );
			goto ErrExit;
		}
		
		/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
		if( pub_base_CompDblVal(dTempAmt,0.00) > 0 )
			sMoBankAccPo.bail_amt=sMoBankAccPo.bail_amt-dTempAmt;
        	
		if(pub_base_CompDblVal(sMoBankAccPo.bail_amt,0.00) < 0)
		{
			sMoBankAccPo.bail_amt=0.00;
		}else{
			sMoBankAccPo.bail_amt_ratio=pub_base_PubDround_1(sMoBankAccPo.bail_amt*100/sMoBankAccPo.sign_amt);
		}
		ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Mo_bank_acc_po_Clo_Upd();
	}else if (cBail_type[0] == '2')
	{
		/***jk20120615
			if(strlen(sMoBankAccPo.pact_no)==0 )
		{
			ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply, "bail_ac_no='%s' and sts='1' and amt=%lf",bail_ac_no ,dTempAmt );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			ret = Mo_bail_rate_Fet_Upd(&sMoBailRate_cc,g_pub_tx.reply);
			if( ret ) 
			{
				sprintf( acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			sMoBailRate_cc.sts[0]='*';
			ret = Mo_bail_rate_Upd_Upd(sMoBailRate_cc,g_pub_tx.reply);
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
			Mo_bail_rate_Clo_Upd();
			ret = sql_sum_double("mo_bail_rate","amt",&dSum_bail_amt,"bail_ac_no='%s' and sts='1'",bail_ac_no);
			if(ret)
			{
				sprintf( acErrMsg, "ִ��sql_sum_double��![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "AT03" );
				goto ErrExit;
			}
			if(sDd_mst.bal-dSum_bail_amt-dTempAmt<-0.005)
			{
				sprintf( acErrMsg, "ȡ�������![%d][%s]",ret,bail_ac_no);
				WRITEMSG
				sprintf( g_pub_tx.reply, "A030" );
				goto ErrExit;
			}
		}jk20120615***/
	}else{
		sprintf( acErrMsg, "��־����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** ��������Ҫ��Э���� **/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
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
		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Dec_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret ) 
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Fet_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
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
			/** ������Ϣ **/
			if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; ����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * dTempAmt * HQday / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
				sMoBailRate.amt=dTempAmt;
			}
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
						/** ������Ϣ **/
			if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; ����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;����Ҫ��ֻ������������� 2011-8-9 16:23:29 **/
				if(sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * dTempAmt * day / L360 / L100;
				}else{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
				sMoBailRate.amt=dTempAmt;
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
	
		/** ���±�֤���Ϣ��� **/
		sMoBailRate.intst_date=g_pub_tx.tx_date;
		sMoBailRate.end_trace_no=g_pub_tx.trace_no;
		sMoBailRate.intst_amt=g_pub_intst.dealval;
		strcpy(sMoBailRate.sts,"*");
		ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_bail_rate_Upd_Upd��![%d][%s]",ret,sMoBankAccPo.pact_no);
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
	else if( intst_flag[0]=='N' )
	{
		g_pub_intst.dealval=0.00;
		g_pub_intst.tax_intst=0.00;					/** Ӧ˰��Ϣ **/
		g_pub_intst.factval=0.00;					/** ʵ����Ϣ **/
		g_pub_intst.val=0.00;						/** ��Ϣ˰ **/
		g_pub_intst.keepval=0.00;					/** ��ֵ��Ϣ **/
		set_zd_data("0153","52101");				/** ��֤����Ϣ **/
		set_zd_double("102P",g_pub_intst.dealval);
	}else{
		sprintf(acErrMsg,"��Ϣ��־����!!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D999");
		goto ErrExit;
	}
	Mo_bail_rate_Clo_Upd();
	set_zd_double("0410",dTempAmt);
	
	
OkExit:
	if(strlen(sMoBankAccPo.pact_no)>0)
	{/*20110811*/
		ret=sql_execute("update ln_auth set sts='1' where pact_no='%s' and sts='X' and type='13' ",sMoBankAccPo.pact_no);
	}else{
		ret=sql_execute("update ln_auth set sts='1' where pay_ac_no='%s' and sts='X' and type='13' ",bail_ac_no);
	}
	if(ret){
		sprintf(acErrMsg,"����ln_auth״̬ʧ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}
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
