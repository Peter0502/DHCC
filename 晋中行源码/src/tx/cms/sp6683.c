/*************************************************
* �� �� ��: sp6683.c
* �����������˽�������Ŵ��������
*
* ��    ��: chenchao
* ������ڣ�2011-8-4 15:20:22
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6683','�Ŵ��������','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6683','0','6683','#$');
insert into tx_sub_def values ('6683','�Ŵ��������','sp6683','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_gage_reg_c.h"
int sp6683()
{
	int	iRet = 0;
	char	cGet_data[201];
	
	
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	ln_gage_reg_c		sLnGageReg;
	struct	td_mst_c		sTd_mst;
	struct	cms_rollback_c		sCms_rollback;
	
	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	memset(&sLnGageReg,0x00,sizeof(sLnGageReg));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sTd_mst,0x00,sizeof(sTd_mst));
	
	pub_base_sysinit();
	
	get_zd_data("0380",cGet_data);			/** �����ͬ�� || �ж�Э���� **/
	zip_space(cGet_data);
	memcpy(sLn_gage_reg.pact_no,cGet_data,sizeof(sLn_gage_reg.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	iRet = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sLn_gage_reg.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "�������ǼǱ��α����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��ѯ����ǼǱ�����!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	iRet = Ln_gage_reg_Fet_Upd(&sLnGageReg,g_pub_tx.reply);
	if(iRet && iRet != 100)
	{
		sprintf( acErrMsg, "��ѯ����ǼǱ�����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��ѯ����ǼǱ�����!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if (iRet == 100)
	{
		get_zd_data("0670",sLn_gage_reg.gage_type);	/** ��Ѻ������ 1:��Ѻ 2:��Ѻ **/
		sLn_gage_reg.sts[0] = 'X';			/** �Ŵ�������� **/
		sLn_gage_reg.ac_id=0;
		iRet = Ln_gage_reg_Ins(sLn_gage_reg,g_pub_tx.reply);
		if(iRet)
		{
			sprintf( acErrMsg, "�������Ѻ��ǼǱ�����!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�������Ѻ��ǼǱ�����!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
		get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'I';
		sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' ",sLn_gage_reg.pact_no);
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
	
	}else
	{
		vtcp_log("[%s][%d]��ȥ���½׶�",__FILE__,__LINE__);
		sprintf(sCms_rollback.rolbk_sql," sts='%s' and gage_type='%s' ",sLnGageReg.sts,sLnGageReg.gage_type);
		get_zd_data("0670",sLnGageReg.gage_type);	/** ��Ѻ������ 1:��Ѻ 2:��Ѻ **/
		sLnGageReg.sts[0] = 'X';			/** �Ŵ�������� **/
		iRet = Ln_gage_reg_Upd_Upd(sLnGageReg);
		if(iRet)
		{
			vtcp_log("[%s][%d]���´���ǼǱ�����Ret=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,"���´���ǼǱ�����!!!"); 
			goto ErrExit;
		}
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));

		get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'U';
		sCms_rollback.sts[0] = '0';
		sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' and gage_type='%s'",sLnGageReg.pact_no,sLnGageReg.gage_type);
		get_zd_data("1230",sCms_rollback.filler);
		iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
			strcpy( g_pub_tx.reply, "D107" );
			set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
			goto ErrExit;
		}
	}
	Ln_gage_reg_Clo_Upd();
	
	
	/*** ��ʼ���� pact_gaga_rel **/
	memcpy(sPact_gaga_rel.pact_no,sLn_gage_reg.pact_no,sizeof(sPact_gaga_rel.pact_no)-1);	/** ��ͬ�� **/
	get_zd_data("0680",sPact_gaga_rel.pact_type);		/** ����:1:���� 2:�ж� **/
	get_zd_data("0370",cGet_data);				/** ����Ѻ���� **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.gaga_id,cGet_data,sizeof(sPact_gaga_rel.gaga_id)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_double("0390",&sPact_gaga_rel.gaga_bal);		/** ����Ѻ��� **/
	get_zd_double("0400",&sPact_gaga_rel.gaga_val);		/** ����Ѻ��� **/	
	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);

	get_zd_data("0250",cGet_data);				/** ���� **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.name,cGet_data,sizeof(sPact_gaga_rel.name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data("0300",cGet_data);				/** ���д浥���� **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.bill_no,cGet_data,sizeof(sPact_gaga_rel.bill_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sPact_gaga_rel.bill_no);

	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);
	sPact_gaga_rel.stsvar[0] = 'X';				/** �Ŵ�������� **/
	if(sLn_gage_reg.gage_type[0] == '2' && strlen(sPact_gaga_rel.bill_no) >0 )
	{
		iRet = Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s' )",sPact_gaga_rel.bill_no);
		if(iRet)
		{
			sprintf( acErrMsg, "��ѯ�������ļ�ʧ��!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"��ѯ�������ļ�ʧ��!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		memcpy(sPact_gaga_rel.io_ind,"120",3);
	}
	
	vtcp_log("[%s][%d]gaga_bal=[%lf]gaga_val=[%lf]",__FILE__,__LINE__,sPact_gaga_rel.gaga_bal,sPact_gaga_rel.gaga_val);
	iRet = Pact_gaga_rel_Ins(sPact_gaga_rel,g_pub_tx.reply);
	if(iRet)
	{
		sprintf( acErrMsg, "��ͬ�ź͵���ѺƷ��ϸ������ϵ��ʧ��!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����ͬ�ź͵���ѺƷ��ϸ������ϵ��ʧ��!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," pact_gaga_rel ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," stsvar='X' and pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
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
	sprintf( acErrMsg, "�Ŵ��������ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ��������ʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
