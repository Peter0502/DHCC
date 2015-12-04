/*************************************************
* �� �� ��:  spB331.c
* ���������� ������гжһ�Ʊ��Ϣ¼�빦��
*
* ��    ��:  jane
* ������ڣ� 2003��8��8��
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
#include "mdm_ac_rel_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h"

int spB331()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc,sMoBankAcc1;		/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* ���ҲֵǼǱ� */
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* �жҲ��ԭʼƱ�ݵǼǲ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 			sDdMst;			/* �������ļ� */
	struct	impa_dep_reg_c 		sImpaDepReg;	/* ��Ѻ�浥�Ǽǲ� */
	struct	mo_dep_reg_c 		sMoDepReg;	/* ��Ʊ��Ѻ�浥�Ǽǲ� */
	struct  mo_po_co_c  sMoPoCo; 	/* �ж�Э�������ͬ�Ŷ��ձ� */
	struct  mo_bail_rate_c		mo_bail_rate; 	/* ��֤�����ʵǼǱ� */
	char    cLoadName[50];       		   		/* �ϴ��ļ��� */
	char	cDbActno[25];						/* �����ʺ� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	double	dTmpAmt;							/* ��֤���� */
	double	dTallAmt;							/* �жһ�Ʊ����޶� */
	double	dSumMortAmt;						/* ��Ѻ�浥�Ǽǲ�����Ѻ��Ŷ�Ӧ��� */
	double	dSumMortAmt1;						/* �жһ�����Ϣ�Ǽǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	double	dSumMortAmt2;						/* �жҵǼǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	int		ret, ret1, flag, num;
	char	Tmp_flag[2];

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	memset( cDbActno, 0x00, sizeof( cDbActno ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/* �������� */

	pub_base_strpack( sMoBankAcc.pact_no );
/***Э���ŵĳ��Ȳ�������
	if( strlen( sMoBankAcc.pact_no ) != 16 )
	{
		strcpy( g_pub_tx.reply, "B328" );
		sprintf( acErrMsg, "�ж�Э����ӦΪ16λ!" );
		WRITEMSG
		goto ErrExit;
	}
***/

	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret1 );
		WRITEMSG
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
				goto ErrExit;
			}
		}

		/* ��ǰ̨��Ļȡֵ */
		/*strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	 ���׻��� */
		/* 2008/12/9 17:01 add by martin ��˲�����¼��Ϊͬһ����*/
		strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tel );
		sMoBankAcc.req_date = g_pub_tx.tx_date;				/* �������� */
		get_zd_data( "0260", sMoBankAcc.payer_name );		/* ������ȫ�� */
		get_zd_data( "0910", sMoBankAcc.payer_br_no );		/* �����˿������к� */
		get_zd_data( "0250", sMoBankAcc.payer_br_name );	/* �����˿��������� */
		get_zd_data( "0270", sMoBankAcc.payee_name );		/* �տ���ȫ�� */
		get_zd_data( "0830", sMoBankAcc.payee_ac_no );		/* �տ����˺� */
		get_zd_data( "0810", sMoBankAcc.payee_br_name );	/* �տ��˿��������� */
		get_zd_double( "1001", &sMoBankAcc.sign_amt );		/* ǩ��Ʊ���� */
		get_zd_long( "0440", &sMoBankAcc.matr_date );		/* �������� */
		get_zd_data( "0630", sMoBankAcc.pact_num );			/* ��Ѻ��� */
		get_zd_double( "1002", &sMoBankAcc.tot_mort_amt );	/* ��Ѻ��� */
		get_zd_double( "0400", &sMoBankAcc.bail_amt_ratio );/* ��֤����� */
		strcpy( sMoBankAcc.acc_po_ind, "0" );				/* ��Ʊ״̬: 0δ���ڴ���*/
		get_zd_data( "0380", sMoBankAcc.payer_ac_no );		/* �������ʺ� */
		get_zd_double( "1003", &dTmpAmt );					/* ��֤���� */
		get_zd_data( "0670", sMoBankAcc.acc_type );			/* �ж����� */
		get_zd_data( "0320", cBailAcNo );					/* ��֤���ʺ� */
		get_zd_double( "0840", &sMoBankAcc.over_rate );		/* ��Ϣ���� */

		/** ���ʺŴ��� **/
		pub_base_old_acno( sMoBankAcc.payer_ac_no );
		pub_base_old_acno( cBailAcNo );

		/** �жϵ��������ʵ����պͳжһ�Ʊ�������Ƿ�һ�� **/
		/** ��Ϊ�޷�ȡ��ȫ�������ֶΣ����Բ��ܱ�֤�Ժ� **/
		/** �����ظ���¼ **/
		if( sMoBankAcc.acc_type[0] == '3' || sMoBankAcc.acc_type[0] == '1' )
		{
			memset(&mo_bail_rate,0,sizeof(mo_bail_rate));
			memcpy(mo_bail_rate.pact_no,sMoBankAcc.pact_no,sizeof(mo_bail_rate.pact_no)-1);
			zip_space(mo_bail_rate.pact_no);
			ret = Mo_bail_rate_Sel(g_pub_tx.reply,&mo_bail_rate," pact_no='%s' ",mo_bail_rate.pact_no);
			if(ret)
			{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
			}
			if(mo_bail_rate.end_date!=sMoBankAcc.matr_date)
			{
					sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B319" );
					goto ErrExit;
			}
		}

		/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
		/* Ʊ���ܽ��ܴ��ڵ���1100��Ԫ����� *
		*---- �Ǽǽ����� -------------------------------*
		ret = pub_base_GetParm_double( "CDZGE", 1, &dTallAmt );
		if( ret )
		{
			sprintf( acErrMsg, "ȡ�жһ�Ʊ����޶��Ʒ��Ŵ�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B269" );
			goto ErrExit;
		}

		ret = pub_base_CompDblVal( sMoBankAcc.sign_amt, dTallAmt );
		if( ret != -1 )
		{
			sprintf( acErrMsg, "�жһ�ƱƱ���ܽ�������1100��Ԫ�����![%lf]", sMoBankAcc.sign_amt );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B270" );
			goto ErrExit;
		}
		*----------------------------------------------------*/

		/* ��鸶���ʺŵ���Ч��:�����˺š��˻�״̬���� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
							sMoBankAcc.payer_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�������˺Ų�����![%s]", sMoBankAcc.payer_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D164" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
						sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�������˺Ų��ǻ����˺�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B179" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "�˸������˺�״̬������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B180" );
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "131" ) ==0 )
		{
			sprintf( acErrMsg, "��Ʊ���˺Ų�����Ϊ��֤���˺�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B295" );
			goto ErrExit;
		}

		/* �жϱ�֤�����Ѻ�浥�ܽ���Ƿ�Ϊ0 * modify by ligl 20100426
		if( sMoBankAcc.tot_mort_amt < 0.005 && dTmpAmt < 0.005 &&sMoBankAcc.acc_type[0] != '4')**/
		vtcp_log("%s,%d,aaaaaaaaaaaaa=%lf,%lf",__FILE__,__LINE__,dTmpAmt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00);
		if( sMoBankAcc.tot_mort_amt < 0.005 && pub_base_CompDblVal(dTmpAmt,sMoBankAcc.sign_amt*sMoBankAcc.bail_amt_ratio/100.00)<0 &&sMoBankAcc.acc_type[0] != '4')
		{
			sprintf( acErrMsg, "��֤������Ѻ������ȫ��Ϊ��" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B181" );
			goto ErrExit;
		}

		/* 1�����жң�2��100%��Ѻ�浥��3��100%��֤��4�������жң�5����Ʊ��СƱ */
		if( ret1 == 0 )
		{
			strcpy(Tmp_flag,"0");

			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5' || sMoBankAcc.acc_type[0] == '4')
			{
				sprintf( acErrMsg, "�ж�Э�����Ѵ���!" );
				WRITEMSG
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
		            	    	strcpy( g_pub_tx.reply, "B312" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dTmpAmt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��֤�������С�ڵ��ڵ�һ�νɴ�ʱ����Ľ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}

				if( strcmp( sMoBankAcc.pact_num, "" ) )
				{
					ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Sel����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
					if( ret == 0 )
					{
						if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
						{
							sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
							WRITEMSG
							strcpy( g_pub_tx.reply, "B319" );
							goto ErrExit;
						}
					}else if( ret == 100 )
					{
						sprintf( acErrMsg, "����Ѻ��Ų�����!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Sel����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}

					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, 
						"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "����Ѻ��Ų�����!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d]", 
							ret );
						WRITEMSG
						goto ErrExit;
					}
		
					/* �����Ѻ�浥����Ƿ��� */
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", 
						&dSumMortAmt,"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					/* modify by rob at 20031201 **
					ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", 
						&dSumMortAmt1,"pact_num='%s'", sMoBankAcc.pact_num );
					***/
					ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
						"a.tot_mort_amt", &dSumMortAmt1,\
						"pact_num='%s' and a.pact_no=b.pact_no and \
						b.po_sts!='3'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
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
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}
		
			/**	 �����ж�
			if( sMoBankAcc.acc_type[0] == '4' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "�ж�����Ϊ���Ҳ�ʱ����֤���˺ű�������!" );
		                	WRITEMSG
		            	    	strcpy( g_pub_tx.reply, "B260" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
		
				ret = pub_base_CompDblVal( dTmpAmt, sMoBankAcc.bail_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��֤�������С�ڵ��ڵ�һ�νɴ�ʱ����Ľ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B259" );
					goto ErrExit;
				}

				if( strcmp( sMoBankAcc.pact_num, "" ) )
				{
					ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Sel����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
					if( ret == 0 )
					{
						if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
						{
							sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
							WRITEMSG
							strcpy( g_pub_tx.reply, "B319" );
							goto ErrExit;
						}
					}else if( ret == 100 )
					{
						sprintf( acErrMsg, "����Ѻ��Ų�����!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Sel����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
	
					Mo_bank_acc_po_Clo_Sel();

					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "��Ѻ�浥��Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}

		
					ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
					if( ret == 100 )
					{
						sprintf( acErrMsg, "����Ѻ��Ų�����!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B308" );
						goto ErrExit;
					}else if( ret )
					{
						sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					* �����Ѻ�浥����Ƿ��� *
					ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
											"pact_no='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1,\
											"pact_num='%s'", sMoBankAcc.pact_num );
					if( ret )
					{
						sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
						WRITEMSG
						goto ErrExit;
					}
		
					ret = pub_base_CompDblVal( (dSumMortAmt1 + sMoBankAcc.tot_mort_amt), dSumMortAmt );
					if( ret == 1 )
					{
						sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ��������!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B310" );
						goto ErrExit;
					}
				}
			}
			**/
		
			if( sMoBankAcc.acc_type[0] == '3' )
			{
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "�ж�����Ϊ100����֤��ʱ����֤���˺ű�������!" );
		                	WRITEMSG
		            	    	strcpy( g_pub_tx.reply, "B314" );
		        	        goto ErrExit;
				}
		
				if( strcmp( sMoBankAcc.bail_ac_no, cBailAcNo ) != 0 )
				{
					sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B258" );
					goto ErrExit;
		    		}
vtcp_log( "zy1  sMoBankAcc.bail_amt[%lf], sMoBankAcc.sign_amt[%lf]",sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				ret = pub_base_CompDblVal( sMoBankAcc.bail_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "�ɴ汣֤���������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B313" );
					goto ErrExit;
				}
		
				ret = Mo_sg_cash_po_Sel( g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", \
												sMoBankAcc.pact_no );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "�ж�Э������ɴ汣֤��ʱ����Ĳ�һ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B306" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Sel��![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			}
		
			/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
			/****��Ϊֱ�Ӹ�ֵ   20110128
			strcpy( sMoBankAcc.acc_po_sts, "0" );
			****/
			sMoBankAcc.acc_po_sts[0]='0';		/* �ж�Э��״̬��0 ¼�� */
		
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		else if( ret1 == 100 )
		{
			strcpy(Tmp_flag,"1");
			if( sMoBankAcc.acc_type[0] == '1' || sMoBankAcc.acc_type[0] == '3' )
			{
				sprintf( acErrMsg, "���ѡ��֤��жң�����������ɴ汣֤����!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B264" );
				goto ErrExit;
			}

			if( !strcmp( sMoBankAcc.pact_num, "" ) && sMoBankAcc.acc_type[0] != '4' )
			{
				sprintf( acErrMsg, "��Ѻ��ű�������!" );
				WRITEMSG
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
						strcpy( g_pub_tx.reply, "E354" );
						goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
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
						strcpy( g_pub_tx.reply, "B355" );
						goto ErrExit;
				}


				ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
								sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "[%s][%d]�����ʻ����ǻ����ʻ�!",__FILE__,__LINE__ );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B356" );
					goto ErrExit;
				}
				else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%s][%d][%d]",__FILE__, __LINE__,ret );
					WRITEMSG
					goto ErrExit;
				}

				if(!memcmp(sDdMst.prdt_no,"133",sizeof(sDdMst.prdt_no)-1))
				{
					sprintf( acErrMsg, "��֤���ʻ����ܵ��������ʻ�!",__FILE__, __LINE__,ret );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B357" );
					goto ErrExit;
				}
				if(sDdMst.ac_sts[0] != '1')
				{
					sprintf( acErrMsg, "�����ʺ�״̬������!",__FILE__, __LINE__,ret );
					WRITEMSG
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
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_dep_reg_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/* �����Ѻ�浥����Ƿ��� */
				ret = sql_sum_double( "mo_dep_reg", "po_amt", &dSumMortAmt,\
								"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	

				/***
				ret = sql_sum_double( "mo_bank_acc_po a, mo_bank_po b", 
					"a.tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					a.pact_no=b.pact_no and b.po_sts!='3' and acc_type='5' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			   ***/

				ret = sql_sum_double( "mo_bank_acc_po",
						"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
						acc_type='5' and acc_po_sts in ('0','1','2') ",
						sMoBankAcc.pact_num );
				if( ret )
				{
					  sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					  WRITEMSG
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
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "��Ѻ��������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
/****************ԭ��״̬5����� 
				if( strcmp( cBailAcNo, "" ) == 0 )
				{
					sprintf( acErrMsg, "�ж�����Ϊ�жҲ��ʱ����֤���˺ű�������!" );
					WRITEMSG
			    		strcpy( g_pub_tx.reply, "B299" );
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}

vtcp_log( "zyzyzy1   sMoBankAcc1.bail_ac_no[%s], cBailAcNo[%s]",sMoBankAcc1.bail_ac_no, cBailAcNo );
					pub_base_strpack( sMoBankAcc1.bail_ac_no );
vtcp_log( "zyzyzy2   sMoBankAcc1.bail_ac_no[%s], cBailAcNo[%s]",sMoBankAcc1.bail_ac_no, cBailAcNo );
					if( strcmp( sMoBankAcc1.bail_ac_no, cBailAcNo ) != 0 )
					{
						sprintf( acErrMsg, "��֤���˺ű�������Ʊ�ݹ�����¼���һ��!" );
		       				WRITEMSG
						strcpy( g_pub_tx.reply, "B300" );
						goto ErrExit;
			    		}
				}else if( ret != 100 )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
				strcpy( sMoBankAcc.bail_ac_no, cBailAcNo );

				Mo_bank_acc_po_Clo_Sel();

				* ��ѯ�жҲ��ԭʼƱ�ݵǼǲ� *
				ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, 
					"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "���Ϊ�жҲ�֣������������Ʊ�ݹ�����!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B297" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_split_po_Sel��![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				if( sMoBankSplitPo.matr_date > sMoBankAcc.matr_date )
				{
					sprintf( acErrMsg, "ԭƱ�ݵ������ڲ���������³жҵ�������!ԭ����[%ld]������[%ld]",sMoBankSplitPo.matr_date, sMoBankAcc.matr_date );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B298" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.sign_amt, sMoBankSplitPo.par_amt );
				if( ret == 1 )
				{
					sprintf( acErrMsg, "��ֺ��¿��ж�Ʊ���ܽ�������ڱ����Ʊ�ݽ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B301" );
					goto ErrExit;
				}
********************/
			}

			/** ��Ʊ��СƱ����Ѻ�浥һ������ **/
/** ��Ʊ��СƱҪ��������
			if( sMoBankAcc.acc_type[0] == '2' || sMoBankAcc.acc_type[0] == '5')
**/
			if( sMoBankAcc.acc_type[0] == '2' )
			{
				ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, "pact_num='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				ret = Mo_bank_acc_po_Fet_Sel( &sMoBankAcc1, g_pub_tx.reply );
				if( ret == 0 )
				{
					if( sMoBankAcc1.matr_date != sMoBankAcc.matr_date )
					{
						sprintf( acErrMsg, "��Ѻ��Ŷ�Ӧ�ĳж�,�䵽���ձ���Ϊͬһ��!" );
						WRITEMSG
						strcpy( g_pub_tx.reply, "B319" );
						goto ErrExit;
					}
				}
				else if( ret != 100 )
				{
					sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}

				Mo_bank_acc_po_Clo_Sel();

				ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, "pact_no='%s'", sMoBankAcc.pact_num );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "����Ѻ��Ų�����!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B308" );
					goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/* �����Ѻ�浥����Ƿ��� */
				ret = sql_sum_double( "impa_dep_reg", "mort_amt", &dSumMortAmt,\
										"pact_no='%s'", sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
	
				/*** modify by rob at 20031201
				ret = sql_sum_double( "mo_bank_acc_po", "tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  acc_po_sts!='3'", sMoBankAcc.pact_num );**/

				ret = sql_sum_double( "mo_bank_acc_po", 
					"tot_mort_amt", &dSumMortAmt1, "pact_num='%s' and  \
					acc_type='2' and acc_po_sts in ('0','1','2') ", 
					sMoBankAcc.pact_num );
				if( ret )
				{
					sprintf( acErrMsg, "ִ��sql_sum_double����![%d]", ret );
					WRITEMSG
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
					strcpy( g_pub_tx.reply, "B310" );
					goto ErrExit;
				}
	
				ret = pub_base_CompDblVal( sMoBankAcc.tot_mort_amt, sMoBankAcc.sign_amt );
				if( ret == -1 )
				{
					sprintf( acErrMsg, "��Ѻ��������ڵ���Ʊ���ܽ��!" );
					WRITEMSG
					strcpy( g_pub_tx.reply, "B315" );
					goto ErrExit;
				}
			}
		
			/* �Ǽǻ�Ʊ�ǼǱ� */
			strcpy( sMoBankAcc.acc_po_sts, "0" );       /* �ж�Э��״̬��0 ¼�� */
		
			ret = pub_sttl_RegMoBankAccPo( sMoBankAcc );
			if ( ret )
		   	{
			       	sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankAccPo����!ret=[%d]", ret );
			       	WRITEMSG
			       	goto ErrExit;
			}
		}
	}

	Mo_bank_acc_po_Clo_Upd();

	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "���гжһ�Ʊ��Ϣ¼��" );	/* ��ע */

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,Tmp_flag);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	/*�Ǽ�Э����-��ͬ�Ŷ��յǼǲ� ΪMIS׼������*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�Ǽǳж�Э����-��ͬ�Ŷ��ձ����" );
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
