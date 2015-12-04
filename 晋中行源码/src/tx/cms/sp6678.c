/*************************************************
* �� �� ��: sp6678.c
* ��������������Ŵ�����֪ͨ
*
* ��    ��: chenchao
* ������ڣ�2011-7-25 15:16:49
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6678','�Ŵ�����֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6678','0','6678','#$');
insert into tx_sub_def values ('6678','�Ŵ�����֪ͨ','sp6678','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_discnt_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

int sp6678()
{
	int	iRet = 0;
	char	cPo_type[2];
	char	cGet_data[201];
	char	cPnTyep[2];
	long	lPnDays=0;
	
	struct	cms_rollback_c		sCms_rollback;
	struct	mo_discnt_c		sMo_discnt;
	struct	mo_discnt_c		sMo_discnt1;
	struct	mo_discnt_c		sMoDiscnt;
	struct	mdm_ac_rel_c		sMdmAcRel;
	struct	dd_mst_c		sDdMst;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sMo_discnt,0x00,sizeof(sMo_discnt));
	memset(&sMo_discnt1,0x00,sizeof(sMo_discnt1));
	memset(&sMoDiscnt,0x00,sizeof(sMoDiscnt));
	memset(&sMdmAcRel,0x00,sizeof(sMdmAcRel));
	memset(&sDdMst,0x00,sizeof(sDdMst));
	memset(cPo_type,0x00,sizeof(cPo_type));
	memset(cPnTyep,0x00,sizeof(cPnTyep));
	memset(cGet_data,0x00,sizeof(cGet_data));
	
	pub_base_sysinit();
	
	get_zd_data("0620",cGet_data);				/** ����Э�����  **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pact_no,cGet_data,sizeof(sMo_discnt.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sMo_discnt.pact_no);
	memcpy( sMo_discnt.br_no, g_pub_tx.tx_br_no,sizeof(sMo_discnt.br_no)-1 );	/* �������� */
	get_zd_double("0400",&sMo_discnt.par_amt);		/** ��Ʊ���      **/
	get_zd_double("0840",&sMo_discnt.pn_rate);		/** ������        **/
	get_zd_long("0460",&sMo_discnt.pn_date);		/** ��������      **/
	sMo_discnt.wrk_date = g_pub_tx.tx_date;			/* ������Ϣ¼��ʱ�������� */
	get_zd_long("0450",&sMo_discnt.not_matr_date);		/** ��Ʊ������    **/
	
	get_zd_data("0300",cGet_data);				/** ��Ʊ���˺�    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.rem_ac_no,cGet_data,sizeof(sMo_discnt.rem_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0250",cGet_data);				/** ��Ʊ������    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.remitter,cGet_data,sizeof(sMo_discnt.remitter)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0270",cGet_data);				/** ����������    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payer_brno_name,cGet_data,sizeof(sMo_discnt.payer_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0310",cGet_data);				/** �տ����˺�    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_ac_no,cGet_data,sizeof(sMo_discnt.payee_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0260",cGet_data);				/** �տ�������    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee,cGet_data,sizeof(sMo_discnt.payee)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0830",cGet_data);				/** �տ�������    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_brno_name,cGet_data,sizeof(sMo_discnt.payee_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0670",cPo_type);				/** Ʊ������   1-��ҵ�жһ�Ʊ  2-���гжһ�Ʊ  **/
	if(cPo_type[0] == '1')
	{
		memcpy(sMo_discnt.note_type,"112",sizeof(sMo_discnt.note_type)-1);
	}else if(cPo_type[0] == '2')
	{
		memcpy(sMo_discnt.note_type,"111",sizeof(sMo_discnt.note_type)-1);
	}
	
	get_zd_data("0590",cGet_data);				/** Ʊ�ݺ���      **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pnote_no,cGet_data,sizeof(sMo_discnt.pnote_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	pub_base_strpack(sMo_discnt.pnote_no);

	get_zd_data("0320",cGet_data);				/** �����������ʺ�**/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pn_ac_no,cGet_data,sizeof(sMo_discnt.pn_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
		
	get_zd_data("0810",cGet_data);				/** ����������    **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pn_name,cGet_data,sizeof(sMo_discnt.pn_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data( "0700", sMo_discnt.local_ind );		/* ���ͬ�Ǳ�־ */
	get_zd_long("0520",&sMo_discnt.way_days);		/** ��;����      **/
	

	get_zd_data("0680",sMo_discnt.pn_type);				/** ���뷽ʽ      **/
	
	get_zd_data("0620",cGet_data);				/** ����Э�����  **/
	zip_space(cGet_data);
	memcpy(sMo_discnt.pact_no,cGet_data,sizeof(sMo_discnt.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sMo_discnt.tranx_bal = sMo_discnt.par_amt;		/* �����˻���� */
	sMoDiscnt.po_knd[0] = 'P';				/** ֽƱ��־ **/
	
		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�ת����Э���� */
	iRet = Mo_discnt_Sel( g_pub_tx.reply, &sMo_discnt1, "pact_no='%s'", sMo_discnt.pact_no );
	if ( iRet == 0 )
	{
		sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�ת����Э����!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"���ֵǼǱ����Ѿ����ڴ�ת����Э����!");
		strcpy( g_pub_tx.reply, "B132" );
		goto ErrExit;
	}
	else if ( iRet != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", iRet );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
		goto ErrExit;
	}
	/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�Ʊ�����ͺ�Ʊ�ݺ� */
	iRet = Mo_discnt_Sel( g_pub_tx.reply, &sMo_discnt1, "note_type='%s' and pnote_no='%s' and sts<>'*' and pn_type='%s' ",sMo_discnt.note_type, sMo_discnt.pnote_no ,sMo_discnt.pn_type);
	if ( iRet == 0 )
	{
		sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�Ʊ�����ͺ�Ʊ�ݺ�!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"���ֵǼǱ����Ѿ����ڴ�Ʊ�����ͺ�Ʊ�ݺ�!");
		strcpy( g_pub_tx.reply, "B174" );
		goto ErrExit;
	}
	else if ( iRet != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", iRet );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
		goto ErrExit;
	}
	if(sMo_discnt.pn_type[0] == '1')/** ����  **/
	{
		sMo_discnt.sts[0] = '1';				/** ¼��״̬ **/
		/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
		iRet = pub_base_GetParm_Str( "TXCP", 1, sMo_discnt.bkrl_code );
		if ( iRet )
		{
			sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ȡ���ֲ�Ʒ��Ŵ�!");
			strcpy( g_pub_tx.reply, "B126" );
			goto ErrExit;
		}
		/* ��������������˺�Ϊ���е�,��������������˺���Ч��:�����ǻ������˻�״̬��������,�����������Ƿ����*/
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMo_discnt.pn_ac_no);
		if (iRet) 
		{
			sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", iRet);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
			goto ErrExit;
		}
		else 
		{
			/* ���ݿͻ��ʺ�ID���ʻ���š��������ʻ�״̬Ϊ1���� ��ѯ���ڴ�����ļ� */
			iRet = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld \
				and ac_seqn=%d and ac_sts='%s'", sMdmAcRel.ac_id, sMdmAcRel.ac_seqn, "1");
			if (iRet == 100) 
			{
				sprintf(acErrMsg, "�˿ͻ��ʺŷǻ��ڻ��ʻ�״̬������![%s]", sMo_discnt.pn_ac_no);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"�˿ͻ��ʺŷǻ��ڻ��ʻ�״̬������!");
				strcpy(g_pub_tx.reply, "B124");
				goto ErrExit;
			}
			else if (iRet) 
			{
				sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", iRet);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"ִ�����ݿ��ѯʧ�ܣ�");
				goto ErrExit;
			}
			if (memcmp(sMo_discnt.pn_name , sDdMst.name ,sizeof(sMo_discnt.pn_name)-1)!=0)
			{
				sprintf(acErrMsg, "��������![%s][%s]", sMo_discnt.pn_name,sDdMst.name);
				WRITEMSG
				set_zd_data(DC_GET_MSG,"��������!");
				strcpy(g_pub_tx.reply, "NXD5");
				goto ErrExit;
			}
		}

	
	}else if (sMo_discnt.pn_type[0] == '2')/** ת����  **/
	{
		sMo_discnt.sts[0] = '2';				/* ת���ֱ�־û������ֱ�Ӽ���*/
		/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
		iRet = pub_base_GetParm_Str( "TXCP", 2, sMo_discnt.bkrl_code );
		if ( iRet )
		{
			sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"ȡ���ֲ�Ʒ��Ŵ�!");
			strcpy( g_pub_tx.reply, "B126" );
			goto ErrExit;
		}
	
	}
	/** ������Ϣ  **/
	iRet = pub_base_CalDays( sMo_discnt.pn_date, sMo_discnt.not_matr_date, "0", &lPnDays );
	if ( iRet )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��������������!");
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}
	sMo_discnt.pn_int = sMo_discnt.par_amt * (lPnDays + sMo_discnt.way_days) * (sMo_discnt.pn_rate/30/1000);
		
	iRet = Mo_discnt_Ins(sMo_discnt,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]�������ֵǼǱ�ʧ��.����ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"�������ֵǼǱ�ʧ��!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and pn_type='%s' and sts='%s' ",sMo_discnt.pact_no,sMo_discnt.pn_type,sMo_discnt.sts);
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
	sprintf( acErrMsg, "�Ŵ�����֪ͨ�ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ�����֪ͨʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
