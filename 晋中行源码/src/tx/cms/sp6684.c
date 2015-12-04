/*************************************************
* �� �� ��: sp6684.c
* �����������˽�������Ŵ���������
*
* ��    ��: chenchao
* ������ڣ�2011-8-4 17:50:31
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6684','�Ŵ���������','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6684','0','6684','#$');
insert into tx_sub_def values ('6684','�Ŵ���������','sp6684','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "pact_gaga_rel_c.h"
#include "ln_gage_reg_c.h"
#include "ln_mst_c.h"

int sp6684()
{
	int	iRet = 0;
	int	iUpd_flag = 0;
	char	cGet_data[201];
	
	
	struct	ln_gage_reg_c		sLn_gage_reg;
	struct	cms_rollback_c		sCms_rollback;
	struct	ln_mst_c		sLn_mst;
	struct	pact_gaga_rel_c		sPact_gaga_rel;
	
	memset(&sLn_mst,0x00,sizeof(sLn_mst));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sPact_gaga_rel,0x00,sizeof(sPact_gaga_rel));
	
	get_zd_data("0380",cGet_data);			/** �����ͬ�� || �ж�Э���� **/
	zip_space(cGet_data);
	memcpy(sLn_gage_reg.pact_no,cGet_data,sizeof(sLn_gage_reg.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0670",sLn_gage_reg.gage_type);	/** ��Ѻ������ 1:��Ѻ 2:��Ѻ **/
	
	get_zd_data("0370",cGet_data);			/** ����Ѻ���� **/
	zip_space(cGet_data);
	memcpy(sPact_gaga_rel.gaga_id,cGet_data,sizeof(sPact_gaga_rel.gaga_id)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	/*************************************************************/
	/***�����ͬ�����Ѻ���ϵ��Pact_gaga_rel *****************/
	/*************************************************************/
	
	iRet = Pact_gaga_rel_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
	if(iRet)
	{
		sprintf( acErrMsg, "�����α����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����α����!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	iRet = Pact_gaga_rel_Fet_Upd(&sPact_gaga_rel,g_pub_tx.reply);
	if(iRet == 100)
	{
		sprintf( acErrMsg, "û�иñʺ�ͬ��Э������ĵ�Ѻ�������¼!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"û�иñʺ�ͬ��Э������ĵ�Ѻ�������¼!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if(iRet)
	{
		sprintf( acErrMsg, "ȡ�α����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"ȡ�α����!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	if(sPact_gaga_rel.stsvar[0] != '1')
	{
		sprintf( acErrMsg, "�ú�ͬ��Э���µ�Ѻ������״̬!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�ú�ͬ��Э���µ�Ѻ������״̬!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;		
	}
	sPact_gaga_rel.stsvar[0] = 'Y';		/**�Ŵ�������� **/
	iRet = Pact_gaga_rel_Upd_Upd(sPact_gaga_rel,g_pub_tx.reply);
	if(iRet == 100)
	{
		sprintf( acErrMsg, "���µ���Ѻ�ǼǱ�����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"���µ���Ѻ�ǼǱ�����!!!");
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}
	Pact_gaga_rel_Clo_Upd();
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," pact_gaga_rel ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.beg_sql," stsvar='Y' and pact_no='%s' and gaga_id='%s' ",sLn_gage_reg.pact_no,sPact_gaga_rel.gaga_id);
	sCms_rollback.sts[0] = '0';
	memcpy(sCms_rollback.rolbk_sql," stsvar='1' ",sizeof(sCms_rollback.rolbk_sql)-1);
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
	
	/*************************************************************/
	/*******   �����ͬ��ݹ�ϵ�� Ln_gage_reg    *****************/
	/*************************************************************/
	
	iRet = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sLn_gage_reg.pact_no);
	if(iRet)
	{
		sprintf( acErrMsg, "�����α����!!!");
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�����α����!!!");
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}
	while(1)		/** �п���һ�ʺ�ͬ���Զ�Ƚ��(����),��������Ҫȫ������ **/
	{
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		iRet = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);
		if(iRet == 100)
		{
			if(iUpd_flag)
			{
				break;
			}
			sprintf( acErrMsg, "û�иñʺ�ͬ��Э�������¼!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"û�иñʺ�ͬ��Э�������¼!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}else if(iRet)
		{
			sprintf( acErrMsg, "ȡ�α����!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ȡ�α����!!!");
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
		if(sPact_gaga_rel.pact_type[0] == '1') /*�˴��е����Ⱑ*/
		{
			iRet = Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"ac_id = %ld ",sLn_gage_reg.ac_id);
			if(iRet)
			{
				sprintf( acErrMsg, "û�иñʴ�����Ϣ!!!");
				WRITEMSG
				set_zd_data(DC_GET_MSG,"û�иñʴ�����Ϣ!!!");
				strcpy( g_pub_tx.reply, "B130" );
				goto ErrExit;
			}
			if(pub_base_CompDblVal(sLn_mst.bal+sLn_mst.in_lo_intst+sLn_mst.out_lo_intst+sLn_mst.cmpd_lo_intst,0.00)>0)
			{
				sprintf( acErrMsg, "ac_id[%ld]δ����,���������!!!",sLn_mst.ac_id);
				WRITEMSG
				sprintf( acErrMsg, "[%s]δ����,���������!!",sLn_mst.name);
				WRITEMSG
				set_zd_data(DC_GET_MSG,acErrMsg);
				strcpy( g_pub_tx.reply, "B130" );
				goto ErrExit;
			}
		}
	
		if(sLn_gage_reg.sts[0] != '0' && sLn_gage_reg.sts[0] != 'Y')
		{
			sprintf( acErrMsg, "�ñʺ�ͬ��Э��ķ����״̬!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�ñʺ�ͬ��Э��Ϊ�����״̬!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;		
		}
		sprintf(sCms_rollback.rolbk_sql," sts='%s'  ",sLn_gage_reg.sts);
		sLn_gage_reg.sts[0] = 'Y';	/**�Ŵ�������� **/
		iRet = Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
		if(iRet == 100)
		{
			sprintf( acErrMsg, "���µ���Ѻ�ǼǱ�����!!!");
			WRITEMSG
			set_zd_data(DC_GET_MSG,"���µ���Ѻ�ǼǱ�����!!!");
			strcpy( g_pub_tx.reply, "B130" );
			goto ErrExit;
		}
		
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
		get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
		zip_space(cGet_data);
		memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
		memset(cGet_data, 0x00, sizeof(cGet_data));
		
		memcpy(sCms_rollback.beg_table," ln_gage_reg ",sizeof(sCms_rollback.beg_table)-1);
		sCms_rollback.do_type[0] = 'U';
		sprintf(sCms_rollback.beg_sql," sts='Y' and pact_no='%s' ",sLn_gage_reg.pact_no);
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
		iUpd_flag ++;
		memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	}
	Ln_gage_reg_Clo_Upd();
	
	

	
OkExit:
	sprintf( acErrMsg, "�Ŵ���������ɹ�!!!");
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
