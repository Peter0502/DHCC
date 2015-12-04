/*************************************************
* �� �� ��: sp6677.c
* ��������������Ŵ��жһ�Ʊ��ǩ��֪ͨ
*
* ��    ��: chenchao
* ������ڣ�2011-7-23 15:50:42
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6677','�Ŵ��жһ�Ʊǩ��֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6677','0','6677','#$');
insert into tx_sub_def values ('6677','�Ŵ��жһ�Ʊǩ��֪ͨ','sp6677','0','0'); 

*************************************************/ 

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h" 
#include "cms_rollback_c.h"
#include "mo_bank_po_fee_c.h"

int sp6677()
{ 
	int	iRet = 0;
	long	lNum_po = 0;
	long	lNum_fee = 0;
	char	cGet_data[201];
	
	struct	cms_rollback_c		sCms_rollback;
	struct	mo_bank_acc_po_c	sMoBankAcc;
	struct	mo_bank_acc_po_c	sMoBankAccPo;
	struct	mo_bank_po_fee_c	sMo_bank_po_fee;
	struct	mdm_ac_rel_c		sMdm_ac_rel;
	struct	com_branch_c		sCom_branch;
	struct	dd_mst_c		sDd_mst;
	struct	mo_bank_acc_po_c 	sMoBankAcc1;			/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;			/* ���ҲֵǼǱ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;			/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 		sDdMst;				/* �������ļ� */
	struct	impa_dep_reg_c 		sImpaDepReg;			/* ��Ѻ�浥�Ǽǲ� */
	struct	mo_dep_reg_c 		sMoDepReg;			/* ��Ʊ��Ѻ�浥�Ǽǲ� */
	struct  mo_po_co_c 		sMoPoCo; 			/* �ж�Э�������ͬ�Ŷ��ձ� */
	struct  mo_bail_rate_c		mo_bail_rate; 			/* ��֤�����ʵǼǱ� */
	char	cDbActno[25];						/* �����ʺ� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	double	dBail_amt = 0.00;					/* ��֤���� */
	double	dTallAmt;						/* �жһ�Ʊ����޶� */
	double	dSumMortAmt;						/* ��Ѻ�浥�Ǽǲ�����Ѻ��Ŷ�Ӧ��� */
	double	dSumMortAmt1;						/* �жһ�����Ϣ�Ǽǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	double	dSumMortAmt2;						/* �жҵǼǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	int	ret=0, ret1=0, flag=0, num=0;

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( cDbActno, 0x00, sizeof( cDbActno ) );
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sMoBankAcc,0x00,sizeof(sMoBankAcc));
	memset(&sMo_bank_po_fee,0x00,sizeof(sMo_bank_po_fee));
	memset(&sMoBankAccPo,0x00,sizeof(sMoBankAccPo));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(cBailAcNo,0x00,sizeof(cBailAcNo));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);		/** Э����� **/
	zip_space(cGet_data);
	memcpy(sMoBankAcc.pact_no,cGet_data,sizeof(sMoBankAcc.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data("0030",sMoBankAcc.tx_br_no);		/** ���׻��� **/
	
		/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret1 );
		WRITEMSG
		set_zd_data(DC_GET_MSG, "ִ�����ݿ���³���");
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret1 );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"ִ�����ݿ���³���");
		goto ErrExit;
	}
	else if( ret1 == 0 || ret1 == 100 )
	{
		if( ret1 == 0 )
		{
			if( sMoBankAcc.acc_po_sts[0]=='2' )
			{
				strcpy( g_pub_tx.reply,"B281" );
				sprintf( acErrMsg, "sMoBankAcc.acc_po_sts��![%s]",
					sMoBankAcc.acc_po_sts);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"�ж�Э��״̬����");
				goto ErrExit;
			}else if( sMoBankAcc.acc_po_sts[0]=='0' || sMoBankAcc.acc_po_sts[0]=='1')
			{
				goto CONTINUE;
			}
			/*** ���ûع��ֶ� ***/
			sprintf(sCms_rollback.rolbk_sql," tx_br_no='%s',req_date='%ld',payer_name='%s',payer_ac_no='%s',payer_br_no='%s',payer_br_name='%s',payee_name='%s',payee_ac_no='%s',payee_br_name='%s',sign_amt='%f',matr_date='%ld',bail_ac_no='%s',bail_amt='%f',bail_amt_ratio='%f',pact_num='%s',tot_mort_amt='%f',advance_amt='%f',over_ac_no='%s',over_rate='%f',acc_po_sts='%s',acc_po_ind='%s',acc_type='%s'",
			sMoBankAcc.tx_br_no,sMoBankAcc.req_date,
			sMoBankAcc.payer_name,sMoBankAcc.payer_ac_no,sMoBankAcc.payer_br_no,
			sMoBankAcc.payer_br_name,sMoBankAcc.payee_name,sMoBankAcc.payee_ac_no,
			sMoBankAcc.payee_br_name,sMoBankAcc.sign_amt,sMoBankAcc.matr_date,
			sMoBankAcc.bail_ac_no,sMoBankAcc.bail_amt,sMoBankAcc.bail_amt_ratio,
			sMoBankAcc.pact_num,sMoBankAcc.tot_mort_amt,sMoBankAcc.advance_amt,
			sMoBankAcc.over_ac_no,sMoBankAcc.over_rate,sMoBankAcc.acc_po_sts,
			sMoBankAcc.acc_po_ind,sMoBankAcc.acc_type);

			
		}
		get_zd_data("0300",cGet_data);				/** �������˺�**/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payer_ac_no,cGet_data,sizeof(sMoBankAcc.payer_ac_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		sMoBankAcc.req_date = g_pub_tx.tx_date;			/** ǩ������  **/
		get_zd_data("0250",cGet_data);				/** ���������� **/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payer_name,cGet_data,sizeof(sMoBankAcc.payer_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0310",cGet_data);				/** �տ����˺� **/
		zip_space(cGet_data);
		memcpy(sMoBankAccPo.payee_ac_no,cGet_data,sizeof(sMoBankAccPo.payee_ac_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		pub_base_old_acno(sMoBankAccPo.payee_ac_no);
		
		get_zd_data("0270",cGet_data);				/** �տ������� **/
		zip_space(cGet_data);
		memcpy(sMoBankAccPo.payee_name,cGet_data,sizeof(sMoBankAccPo.payee_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0260",cGet_data);				/** �տ��˿�������**/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.payee_br_name,cGet_data,sizeof(sMoBankAcc.payee_br_name)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		get_zd_data("0680",cGet_data);				/** �ж����� **/
		zip_space(cGet_data);
		memcpy(sMoBankAcc.acc_type,cGet_data,sizeof(sMoBankAcc.acc_type)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
        	
		get_zd_long("0450",&sMoBankAcc.matr_date);		/** �������� **/
		get_zd_double("1001",&sMoBankAcc.sign_amt);		/** Ʊ���� **/
        	
		get_zd_data("0320",cGet_data);				/** ��֤���˺�**/
		zip_space(cGet_data);
		memcpy(cBailAcNo,cGet_data,sizeof(cBailAcNo)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		
		get_zd_double("0410",&dBail_amt);			/** ��֤���� **/
		get_zd_double("0400",&sMoBankAcc.bail_amt_ratio);	/** ��֤����� **/
		get_zd_double("0970",&sMoBankAcc.over_rate);		/** ��Ϣ���� **/
		sMoBankAcc.acc_po_ind[0] = '0';				/* ��Ʊ״̬: 0δ���ڴ���*/
		
		get_zd_double("1002",&sMoBankAcc.tot_mort_amt);		/** ��Ѻ��� **/
		if(sMoBankAcc.tot_mort_amt > 0.005)
		{
			memcpy(sMoBankAcc.pact_num,sMoBankAcc.pact_no,sizeof(sMoBankAcc.pact_num)-1);
		}

		pub_base_old_acno(sMoBankAcc.payer_ac_no);
		
		/** ���ڽ��к��Ĳ�֧��һ��Э���¶���տ��˵����,�������������һ�� **/
		if(sMoBankAcc.acc_po_sts[0] == '1')
		{
			if(memcmp(sMoBankAccPo.payee_ac_no,sMoBankAcc.payee_ac_no,sizeof(sMoBankAcc.payee_ac_no)-1)
				|| memcmp(sMoBankAccPo.payee_name,sMoBankAcc.payee_name,sizeof(sMoBankAcc.payee_name)-1))
			{
				vtcp_log("[%s][%d]ͬһ��Э����ֻ��Ϊһ���տ���",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"һ��Э����ֻ��Ϊһ���տ���!!!");
				goto ErrExit;
			}
		}else{
			memcpy(sMoBankAcc.payee_ac_no,sMoBankAccPo.payee_ac_no,sizeof(sMoBankAcc.payee_ac_no)-1);
			memcpy(sMoBankAcc.payee_name,sMoBankAccPo.payee_name,sizeof(sMoBankAcc.payee_name)-1);
		}

			
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sMoBankAcc.payer_ac_no);
		if(iRet)
		{
			vtcp_log("[%s][%d]��ѯmdm_ac_relʧ��ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			set_zd_data(DC_GET_MSG,"��ѯ���ʹ�ϵ��ʧ��!!!");
			goto ErrExit;
		}
		iRet = Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sMdm_ac_rel.opn_br_no);
		if(iRet)
		{
			vtcp_log("[%s][%d]��ѯcom_branchʧ��ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,acErrMsg);
			set_zd_data(DC_GET_MSG,"��ѯ����ϵ��ʧ��!!!");
			goto ErrExit;
		}
		memcpy(sMoBankAcc.payer_br_no,sCom_branch.br_no,sizeof(sMoBankAcc.payer_br_no)-1);
		memcpy(sMoBankAcc.payer_br_name,sCom_branch.br_name,sizeof(sMoBankAcc.payer_br_name)-1);
	
		/** ���ʺŴ��� **/
		pub_base_old_acno( sMoBankAcc.payer_ac_no );
		pub_base_old_acno( cBailAcNo );

		if(sMoBankAcc.acc_type[0] == '3' || sMoBankAcc.acc_type[0] == '1' )
		{
			memset(&mo_bail_rate,0,sizeof(mo_bail_rate));
			memcpy(mo_bail_rate.pact_no,sMoBankAcc.pact_no,sizeof(mo_bail_rate.pact_no)-1);
			zip_space(mo_bail_rate.pact_no);
			ret = Mo_bail_rate_Sel(g_pub_tx.reply,&mo_bail_rate," pact_no='%s' ",mo_bail_rate.pact_no);
			if(ret)
			{
				if(ret==100){
					/*sprintf( acErrMsg, "�ж�Э�����뱣֤������ά��ʱ����Ĳ�һ��!" );*/
					sprintf( acErrMsg, "�óж�Э������δ���б�֤������ά������!" );
				  WRITEMSG
				  set_zd_data(DC_GET_MSG,"�óж�Э������δ���б�֤������ά������!");
				  strcpy( g_pub_tx.reply, "B306" );
				  goto ErrExit;
				}
				else{
					sprintf( acErrMsg, " ���ұ�mo_bail_rate����!" );
				  WRITEMSG
				  set_zd_data(DC_GET_MSG, "���ұ�mo_bail_rate����!");
				  strcpy( g_pub_tx.reply, "B306" );
				  goto ErrExit;
				}				
			}
			if(mo_bail_rate.end_date!=sMoBankAcc.matr_date)
			{
				sprintf( acErrMsg, "Э�鵽�����뱣֤�����ղ���ͬһ��!!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"Э�鵽�����뱣֤�����ղ���ͬһ��!!");
				strcpy( g_pub_tx.reply, "B319" );
				goto ErrExit;
			}
		}

		/* ��鸶���ʺŵ���Ч��:�����˺š��˻�״̬���� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBankAcc.payer_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�������˺Ų�����![%s]", sMoBankAcc.payer_ac_no );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�������˺Ų�����!");
			strcpy( g_pub_tx.reply, "D164" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
			goto ErrExit;
		}
	
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�������˺Ų��ǻ����˺�!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�������˺Ų��ǻ����˺�!");
			strcpy( g_pub_tx.reply, "B179" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
			goto ErrExit;
		}
	
		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "�˸������˺�״̬������!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�˸������˺�״̬������!");
			strcpy( g_pub_tx.reply, "B180" );
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "131" ) ==0 )
		{
			sprintf( acErrMsg, "��Ʊ���˺Ų�����Ϊ��֤���˺�!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"��Ʊ���˺Ų�����Ϊ��֤���˺�!");
			strcpy( g_pub_tx.reply, "B295" );
			goto ErrExit;
		}

		vtcp_log("%s,%d,Bail_amt=%lf,Ҫǩ���ı�֤����=%lf",__FILE__,__LINE__,dBail_amt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00);
		if( sMoBankAcc.tot_mort_amt < 0.005 && pub_base_CompDblVal(dBail_amt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00)<0 &&sMoBankAcc.acc_type[0] != '4')
		{
			sprintf( acErrMsg, "��֤������Ѻ������ȫ��Ϊ��" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"��֤������Ѻ������ȫ��Ϊ��");
			strcpy( g_pub_tx.reply, "B181" );
			goto ErrExit;
		}

		/* 1�����жң�2��100%��Ѻ�浥��3��100%��֤��4�������жң�5����Ʊ��СƱ */
		if( ret1 == 0 )
		{
			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5' || sMoBankAcc.acc_type[0] == '4')
			{
				sprintf( acErrMsg, "�ж�Э�����Ѵ���!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"�ж�Э�����Ѵ���!");
				strcpy( g_pub_tx.reply, "B191" );
				goto ErrExit;
			}

			pub_base_strpack( sMoBankAcc.bail_ac_no );
			if( sMoBankAcc.acc_type[0] == '1' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "�ж�����Ϊ���ж�ʱ����֤���˺ű�������!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�ж�����Ϊ���ж�ʱ����֤���˺ű�������!" );
					strcpy( g_pub_tx.reply, "B312" );
					goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!");
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'",sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!");
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dBail_amt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��֤�������С�ڵ��ڵ�һ�νɴ�ʱ����Ľ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��֤�������С�ڵ��ڵ�һ�νɴ�ʱ����Ľ��!");
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}
				pub_base_strpack( sMoBankAcc.pact_num );

				if( strcmp( sMoBankAcc.pact_num, "" ))
				{
					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "����Ѻ��Ų�����!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"����Ѻ��Ų�����!");
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d]",ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
						goto ErrExit;
					}
		
					/* �����Ѻ�浥����Ƿ��� */
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", 
						&dSumMortAmt,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
						goto ErrExit;
					}
					ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
						"a.tot_mort_amt", &dSumMortAmt1,\
						"pact_num='%s' and a.pact_no=b.pact_no and \
						b.po_sts!='3'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
						goto ErrExit;
					}

					sprintf(acErrMsg, "���ý��:[%lf], ��Ѻ�ܶ�:[%lf],���ν��:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
					WRITEMSG
		
					ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
					if( ret == 1 )
					{
						sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ��������!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"��Ѻ��Ŷ�Ӧ��������!" );
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}

			if( sMoBankAcc.acc_type[0] == '3' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "�ж�����Ϊ100����֤��ʱ����֤���˺ű�������!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�ж�����Ϊ100����֤��ʱ����֤���˺ű�������!");
					strcpy( g_pub_tx.reply, "B314" );
					goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!");
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
				vtcp_log( "[%s][%d],sMoBankAcc.bail_amt[%lf], sMoBankAcc.sign_amt[%lf]",__FILE__,__LINE__,sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				ret = pub_base_CompDblVal( sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "�ɴ汣֤���������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�ɴ汣֤���������ڵ���Ʊ���ܽ��!");
					strcpy( g_pub_tx.reply, "B313" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!");
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
					goto ErrExit;
				}
			}

			sMoBankAcc.acc_po_sts[0]='0';		/* �ж�Э��״̬��0 ¼�� */
		
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"�޸ĳжһ�Ʊ������Ϣ�ǼǱ���");
				goto ErrExit;
			}
			
			get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
			zip_space(cGet_data);
			memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
			memset(cGet_data, 0x00, sizeof(cGet_data));
			
			memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
			sCms_rollback.do_type[0] = 'U';
			sprintf(sCms_rollback.beg_sql,"pact_no='%s' ",sMoBankAcc.pact_no,sMoBankAcc.acc_po_sts);
			sCms_rollback.sts[0] = '0';
			sCms_rollback.filler[0]='#';
			get_zd_data("1230",sCms_rollback.filler+1);
			iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
			if(iRet)
			{
				vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
				goto ErrExit;
			}
		}
		else if( ret1 == 100 )
		{
			if( sMoBankAcc.acc_type[0] == '1' || sMoBankAcc.acc_type[0] == '3' )
			{
				sprintf( acErrMsg, "���ѡ��֤��жң�����������ɴ汣֤����!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"���ѡ��֤��жң�����������ɴ汣֤����!");
				strcpy( g_pub_tx.reply, "B264" );
				goto ErrExit;
			}

			if( !strcmp( sMoBankAcc.pact_num, "" ) && sMoBankAcc.acc_type[0] != '4' )
			{
				sprintf( acErrMsg, "��Ѻ��ű�������!" );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"��Ѻ��ű�������!" );
				strcpy( g_pub_tx.reply, "B317" );
				goto ErrExit;
			}

			if( sMoBankAcc.acc_type[0] == '4' )
			{
				ret = pub_base_CompDblVal( 0.00, sMoBankAcc.bail_amt );
				if( ret )
				{
					sprintf( acErrMsg, "�����жҵı�֤����Ӧ��Ϊ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�����жҵı�֤����Ӧ��Ϊ��!");
					strcpy( g_pub_tx.reply, "B358" );
					goto ErrExit;
				}

				memcpy(cDbActno,cBailAcNo,sizeof(cDbActno)-1);

				ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
								cDbActno );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]�����ʺŲ�����![%s]", __FILE__,__LINE__,cDbActno );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�����ʺŲ�����!");	
					strcpy( g_pub_tx.reply, "E354" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				switch(sMdmAcRel.note_sts[0])
				{
					case '0':	/** ���� **/
					case '2':	/** ��ʧ��֤ **/
					case '3':	/** ������֤ **/
					case '4':	/** �Ѿ����� **/
						break;
					default:
						sprintf( acErrMsg, "[%s][%d]�����ʺ�״̬������!" ,__FILE__,__LINE__);
						WRITEMSG
						set_zd_data(DC_GET_MSG,"�����ʺ�״̬������!");
						strcpy( g_pub_tx.reply, "B355" );
						goto ErrExit;
				}


				ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]�����ʻ����ǻ����ʻ�!",__FILE__,__LINE__ );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�����ʻ����ǻ����ʻ�!");
					strcpy( g_pub_tx.reply, "B356" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯ����");
					goto ErrExit;
				}

				if(!memcmp(sDdMst.prdt_no,"133",sizeof(sDdMst.prdt_no)-1))
				{
					sprintf( acErrMsg, "��֤���ʻ����ܵ��������ʻ�!",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��֤���ʻ����ܵ��������ʻ�!");
					strcpy( g_pub_tx.reply, "B357" );
					goto ErrExit;
				}
				if(sDdMst.ac_sts[0] != '1')
				{
					sprintf( acErrMsg, "�����ʺ�״̬������!",__FILE__, __LINE__,ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"�����ʺ�״̬������!");
					strcpy( g_pub_tx.reply, "B355" );
					goto ErrExit;
				}

				/** �����ʻ����������,��鵽�˽��� **/
				strcpy( sMoBankAcc.bail_ac_no, cBailAcNo );
			}

			if( sMoBankAcc.acc_type[0] == '5' )
			{

				ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "����Ѻ��Ʊ��Ų�����!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"����Ѻ��Ʊ��Ų�����!");
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_dep_reg_Sel����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}
	
				/* �����Ѻ�浥����Ƿ��� */
				ret = sql_sum_double( "mo_dep_reg", "po_amt", &dSumMortAmt,\
								"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po",
						"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
						acc_type='5' and acc_po_sts in ('0','1','2') ",
						sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b",
				"b.po_amt", &dSumMortAmt2, "pact_num='%s' and  \
				a.pact_no=b.pact_no and b.po_sts!='3' and a.acc_type='5' and a.acc_po_sts='3' ",
				sMoBankAcc.pact_num );
				if( ret )
				{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
						goto ErrExit;
				}
				dSumMortAmt1+=dSumMortAmt2;


				sprintf(acErrMsg, "���ý��:[%lf], ��Ѻ�ܶ�:[%lf],���ν��:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
				WRITEMSG
	
				ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ��������!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��Ѻ��Ŷ�Ӧ��������!" );
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "��Ѻ��������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��Ѻ��������ڵ���Ʊ���ܽ��!");
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}

			}

			if( sMoBankAcc.acc_type[0] == '2' )
			{
				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Sel����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
						WRITEMSG
						set_zd_data(DC_GET_MSG,"��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!");
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}
				}
				else if( ret != 100 )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Sel����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				Mo_bank_acc_po_Clo_Sel();

				ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "����Ѻ��Ų�����!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"����Ѻ��Ų�����!");
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}
	
				/* �����Ѻ�浥����Ƿ��� */
				ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
										"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					acc_type='2' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}

				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
					"b.po_amt", &dSumMortAmt2, "pact_num='%s' and  \
					a.pact_no=b.pact_no and b.po_sts!='3' and a.acc_type='2' and a.acc_po_sts='3' ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
					goto ErrExit;
				}
				dSumMortAmt1+=dSumMortAmt2;

				sprintf(acErrMsg, "���ý��:[%lf], ��Ѻ�ܶ�:[%lf],���ν��:[%lf]",
					dSumMortAmt1, dSumMortAmt, sMoBankAcc.tot_mort_amt);
				WRITEMSG
	
				ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ��������!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��Ѻ��Ŷ�Ӧ��������!");
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
				/*modify by gong 2011/9/17 11:07:12 �������� ��ʱ���
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				vtcp_log("[%s][%d] sMoBankAcc.tot_mort_amt=[%lf],sMoBankAcc.sign_amt[%lf]",__FILE__,__LINE__,sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt);
				if( ret == -1 )
				{
					sprintf( acErrMsg, "��Ѻ��������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					set_zd_data(DC_GET_MSG,"��Ѻ��������ڵ���Ʊ���ܽ��!");
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
				end by gong 2011/9/17 11:07:34*/
			}
		
			/* �Ǽǻ�Ʊ�ǼǱ� */
			strcpy( sMoBankAcc.acc_po_sts, "0" );       /* �ж�Э��״̬��0 ¼�� */
		
			ret = pub_sttl_RegMoBankAccPo( sMoBankAcc );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankAccPo����!ret=[%d]", ret );
				WRITEMSG
				set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
				goto ErrExit;
			}
			memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
			
			get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
			zip_space(cGet_data);
			memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
			memset(cGet_data, 0x00, sizeof(cGet_data));
			
			memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
			sCms_rollback.do_type[0] = 'I';
			sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoBankAcc.pact_no);
			sCms_rollback.sts[0] = '0';
			sCms_rollback.filler[0]='#';
			get_zd_data("1230",sCms_rollback.filler+1);
			iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
			if(iRet)
			{
				vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
				strcpy( g_pub_tx.reply, "D107" );
				set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
				goto ErrExit;
			}
		}
	}

	Mo_bank_acc_po_Clo_Upd();

	/*�Ǽ�Э����-��ͬ�Ŷ��յǼǲ� ΪMIS׼������*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�Ǽǳж�Э����-��ͬ�Ŷ��ձ����" );
		WRITEMSG
		set_zd_data(DC_GET_MSG, "�Ǽǳж�Э����-��ͬ�Ŷ��ձ����" );
		goto ErrExit;
	}

	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," mo_po_co ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoPoCo.pact_no);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0]='#'; /** ��ʾ��ѯʱ��ȥ����***/
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
CONTINUE:
	/***
	iRet = sql_max_long( "mo_bank_po", "pact_no_num", &lNum_po , "pact_no='%s' ",sMoBankAcc.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "ִ��sql_max_long��[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	iRet = sql_max_long( "mo_bank_po_fee", "num", &lNum_fee , "pact_no='%s' ",sMoBankAcc.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "ִ��sql_max_long��[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	sMo_bank_po_fee.num = (lNum_po > lNum_fee)?lNum_po:lNum_fee;
	***/

	memcpy(sMo_bank_po_fee.pact_no,sMoBankAcc.pact_no,sizeof(sMo_bank_po_fee.pact_no)-1);
	get_zd_long("0530",&sMo_bank_po_fee.num);		/** Э����� **/
	get_zd_double("0390",&sMo_bank_po_fee.po_amt );		/** Ʊ���� **/
	get_zd_double("0420",&sMo_bank_po_fee.fee_amt); 	/** ������ **/
	get_zd_double("0430",&sMo_bank_po_fee.prm_amt);		/** ��ŵ�� **/
	sMo_bank_po_fee.sts[0] = 'X';				/** �Ŵ����� **/
	sMo_bank_po_fee.new_num = 0;				/** **/
	iRet = Mo_bank_po_fee_Ins(sMo_bank_po_fee,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ж��շѵǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ж��շѵǼǱ�����!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_bank_po_fee",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and sts='X' and num=%ld ",sMoBankAcc.pact_no,sMo_bank_po_fee.num);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_bank_acc_po",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.rolbk_sql,"sign_amt=sign_amt-%lf",sMo_bank_po_fee.po_amt);
	sprintf(sCms_rollback.beg_sql," pact_no='%s' ",sMoBankAcc.pact_no,sMoBankAcc.acc_po_sts);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0]='#';
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
OkExit:
	sprintf( acErrMsg, "�Ŵ��жһ�Ʊǩ��֪ͨ�ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ��жһ�Ʊǩ��֪ͨʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
