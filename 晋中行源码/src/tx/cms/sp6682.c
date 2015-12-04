/*************************************************
* �� �� ��: sp6682.c
* �����������˽�������Ŵ�������������
*
* ��    ��: chenchao
* ������ڣ�2011-8-3 14:58:29
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6682','�Ŵ���������','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6682','0','6682','#$');
insert into tx_sub_def values ('6682','�Ŵ���������','sp6682','0','0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_parm_c.h"

int sp6682()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	struct	mdm_ac_rel_c	sMdm_ac_rel;
	struct	ln_mst_c	sLn_mst;
	struct	cms_rollback_c	sCms_rollback;
	struct	ln_auth_c	sLn_auth;
	struct	ln_parm_c	sLn_parm;
	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_mst,0x00,sizeof(sLn_mst));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	memset(&sLn_parm,0x00,sizeof(sLn_parm));
	
	pub_base_sysinit();
	
	get_zd_data("0300",cGet_data);
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0400",&sLn_auth.amt);		/** ���������� **/
	get_zd_double("0410",&sLn_auth.intst_amt);	/** ������Ϣ��� **/
	
	iRet = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst," pact_no ='%s' ",sLn_auth.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "��ѯ�����������!pact_no[%s]ret=[%d]" ,sLn_auth.pact_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��ѯ�����������!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	iRet = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no='%s'",sLn_mst.prdt_no);
	if(iRet)
	{
		sprintf( acErrMsg, "��ѯ������������!pact_no[%s]ret=[%d]" ,sLn_mst.prdt_no ,iRet);
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��ѯ������������!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	if(pub_base_CompDblVal(sLn_auth.amt,0.00)>0)		/** ������� **/
	{
		/* ������ļ��н���Ƿ�Ϊ0 */	
		if ( !pub_base_CompDblVal(sLn_mst.bal , 0.00) )
		{
			sprintf(acErrMsg,"�����Ϊ0,���ܺ�������![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�����Ϊ0,���ܺ�������!");
			strcpy( g_pub_tx.reply,"L057" );
			goto ErrExit;
		}		
		/* ������״̬�Ƿ�Ϊ���� 	*/
		if( sLn_mst.ac_sts[0]!='4' )
		{
			sprintf(acErrMsg,"����״̬�Ǵ���,���ܺ���![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"����״̬�Ǵ���,���ܺ���!!!!");
			strcpy( g_pub_tx.reply,"L056" );
			goto ErrExit;
		}
		/* ����Ƿ������ȫ�黹���� */
		if (-1 == pub_base_CompDblVal(sLn_mst.bal , sLn_auth.amt))	/* ��������� */ 
		{
			sprintf(acErrMsg,"����[%.2f],ʵ�����[%.2f]",g_pub_tx.tx_amt1,sLn_mst.bal);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"���������!!!");
			strcpy( g_pub_tx.reply,"L039" );
			goto ErrExit;				
		}
	}
	if ( sLn_parm.ln_pay_type[0]!='3' && sLn_parm.ln_pay_type[0]!='4' && pub_base_CompDblVal(sLn_auth.amt,0.00)>0)/** ��Ϣ���� **/
	{
		/* ������ļ���ǷϢ���֮���Ƿ�Ϊ0 del 20120628 ����˵��������ж���
		if ( !pub_base_CompDblVal(sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst , 0.00) )
		{
			sprintf(acErrMsg,"����ǷϢΪ0,���ܺ���![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"����ǷϢΪ0,���ܺ���!");
			strcpy( g_pub_tx.reply,"L074" );
			goto ErrExit;
		}*/	
		if ( pub_base_CompDblVal(sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst ,sLn_auth.intst_amt) )
		{
			sprintf(acErrMsg,"��Ƿ��Ϣ����һ���Ժ���![%s]",g_ln_parm.ac_sts);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"��Ƿ��Ϣ����һ���Ժ���!");
			strcpy( g_pub_tx.reply,"L232" );
			goto ErrExit;
		}
	}
	sLn_auth.sts[0] = 'X';
	memcpy(sLn_auth.type,"11",2);/** ����+��Ϣ���� **/
	memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����������ʧ��.����ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����������ʧ��!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," ln_auth ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' and type='11' ",sLn_auth.pact_no);
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
OkExit:
	sprintf( acErrMsg, "�Ŵ����������ɹ�!!!1");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ���������ʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
