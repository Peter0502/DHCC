/*************************************************
* �� �� ��: sp6675.c
* ��������������Ŵ����Ŵ����֪ͨ
*
* ��    ��: chenchao
* ������ڣ�2011-7-21 15:54:14
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6675','�Ŵ����Ŵ���֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6675','0','6675','#$');
insert into tx_sub_def values ('6675','�Ŵ����Ŵ���֪ͨ','sp6675','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"
#include "mo_po_co_c.h"
#include "draw_loan_limit_c.h"

int sp6675()
{
	int	iRet = 0;
	long	cif_no = 0;
	char	cGet_data[201];
	char	cLn_type[3];
	
	struct	cms_rollback_c		sCms_rollback;
	struct	ln_auth_c		sLn_auth;
	struct	mo_po_co_c		sMo_po_co;
	struct	draw_loan_limit_c	sDraw_loan_limit;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(cLn_type,0x00,sizeof(cLn_type));
	memset(&sMo_po_co,0x00,sizeof(sMo_po_co));
	memset(&sDraw_loan_limit,0x00,sizeof(sDraw_loan_limit));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);	/** ��ݺ� **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0220",cLn_type);	/** ����:1:��ͨ 2:���� **/
	
	get_zd_data("0250",cGet_data);	/** ����   **/
	zip_space(cGet_data);
	memcpy(sLn_auth.name,cGet_data,sizeof(sLn_auth.name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_long("0530",&sLn_auth.time_lmt);	/** ����  **/
	get_zd_double("0390",&sLn_auth.amt);	/** ��ݽ�� **/
	
	get_zd_double("0840",&sLn_auth.rate);	/** ��ͬ���� **/
	get_zd_double("1005",&sLn_auth.fine_rate);	/** ��Ϣ���� **/
	get_zd_data("0230",sLn_auth.prdt_no);	/** ��Ʒ��� **/
	get_zd_data("0300",cGet_data);		/** �����ʺ� **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pay_ac_no,cGet_data,sizeof(sLn_auth.pay_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	pub_base_old_acno(sLn_auth.pay_ac_no);
	get_zd_data("0310",cGet_data);	/** �տ��˺� **/
	zip_space(cGet_data);
	memcpy(sLn_auth.save_ac_no,cGet_data,sizeof(sLn_auth.save_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack( sLn_auth.save_ac_no );
	pub_base_old_acno(sLn_auth.save_ac_no);
	/** ����˺ŵĺϷ��� **/
	if( strlen( sLn_auth.pay_ac_no ) )
	{
		iRet = pub_ln_AcCheck( sLn_auth.pay_ac_no,1,&cif_no,&sLn_auth.pay_ac_id );
		if( iRet )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}
	}
	/** ����˺ŵĺϷ��� **/
	if( strlen( sLn_auth.save_ac_no ) )
	{
		iRet = pub_ln_AcCheck( sLn_auth.save_ac_no,1,&cif_no,&sLn_auth.save_ac_id );
		if( iRet )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}
	}
	memcpy( sLn_auth.save_mdm_code , g_mdm_ac_rel.mdm_code ,sizeof(sLn_auth.save_mdm_code)-1);	/** ���� **/
	sLn_auth.sts[0] = 'X';	/** ״̬ **/
	get_zd_double("0850",&sLn_auth.over_rate );	/** �������� **/
	get_zd_long("0450",&sLn_auth.mtr_date);		/** �������� **/
	get_zd_long("0280",&sLn_auth.cif_no);		/** �ͻ���  **/
	memcpy(sLn_auth.type,"01",2);
	get_zd_data("0690",sLn_auth.ln_type);		/** �������� **/
	get_zd_double("0970",&sLn_auth.flt_rate);	/** �������� **/
	get_zd_long("0440",&sLn_auth.beg_date);		/** �������� **/
	get_zd_data("0680",sLn_auth.intst_type);	/** ��Ϣ���� **/
	get_zd_data("0890",sLn_auth.repay_type);	/** ���ʽ **/

	get_zd_data("0830",cGet_data);			/** �����ͻ��� **/
	zip_space(cGet_data);
	memcpy(sLn_auth.bail_cif_no,cGet_data,sizeof(sLn_auth.bail_cif_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0380",cGet_data);			/** ί��Э����**/
	zip_space(cGet_data);
	memcpy(sLn_auth.trust_no,cGet_data,sizeof(sLn_auth.trust_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0810",cGet_data);			/** ��Ʒ����**/
	zip_space(cGet_data);
	memcpy(sLn_auth.filler1,cGet_data,sizeof(sLn_auth.filler1)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0820",cGet_data);			/** ������;**/
	zip_space(cGet_data);
	memcpy(sLn_auth.filler2,cGet_data,sizeof(sLn_auth.filler2)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0430",&sLn_auth.intst_amt);	/** ��ͬ�ܽ��**/
	
	
	/*memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);*/
	/* ����ѻ����Ÿ�Ϊʵ�ʷ����Ļ��� ԭ����10000�� */
	char sCms_tx_br_no[11];
	memset(sCms_tx_br_no, 0x00, sizeof(sCms_tx_br_no));
	get_zd_data("0880",sCms_tx_br_no); /*�Ŵ�ʵ�ʻ�����*/
	memcpy(sLn_auth.cms_tx_br_no,sCms_tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]���������ǼǱ�ʧ��.����",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"���������ǼǱ�ʧ��!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table,"ln_auth",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and type='01' and sts='X' ",sLn_auth.pact_no);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{ 
		Cms_rollback_Debug(&sCms_rollback);
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
	memcpy(sDraw_loan_limit.pact_no,sLn_auth.pact_no,sizeof(sDraw_loan_limit.pact_no)-1);
	memcpy(sDraw_loan_limit.ac_no,sLn_auth.save_ac_no,sizeof(sDraw_loan_limit.ac_no)-1);
	sDraw_loan_limit.ac_id = sLn_auth.save_ac_id;
	sDraw_loan_limit.save_amt = sLn_auth.amt;
	get_zd_double("0420",&sDraw_loan_limit.trust_amt);
	get_zd_double("0410",&sDraw_loan_limit.self_amt);
	sDraw_loan_limit.sts[0] = 'X';
	memcpy(sDraw_loan_limit.br_no,g_pub_tx.tx_br_no,sizeof(sDraw_loan_limit.br_no)-1);
	sDraw_loan_limit.tx_date = g_pub_tx.tx_date;
	
	iRet = Draw_loan_limit_Ins(sDraw_loan_limit,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����֧���ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����֧���ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table," draw_loan_limit ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and sts='X' ",sLn_auth.pact_no);
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
	get_zd_data("0330",cGet_data);	/** ��ͬ�� **/
	zip_space(cGet_data);
	memcpy(sMo_po_co.contract_no,cGet_data,sizeof(sMo_po_co.contract_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sMo_po_co.pact_no,sLn_auth.pact_no,sizeof(sMo_po_co.pact_no)-1);
	iRet = Mo_po_co_Ins(sMo_po_co,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]�����ͬ��ݹ�ϵ�����",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"�����ͬ��ݹ�ϵ�����!!!"); 
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	memcpy(sCms_rollback.beg_table," mo_po_co",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and contract_no='%s' ",sMo_po_co.pact_no,sMo_po_co.contract_no);
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
	sprintf( acErrMsg, "�Ŵ����Ŵ����֪ͨ�ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ����Ŵ����֪ͨʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
