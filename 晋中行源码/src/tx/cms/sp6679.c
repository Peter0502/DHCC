/*************************************************
* �� �� ��: sp6679.c
* �����������˽�������Ŵ�ת����ת������
*
* ��    ��: chenchao
* ������ڣ�2011-7-20 9:50:43
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6679','�Ŵ�ת����ת��','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6679','0','6679','#$');
insert into tx_sub_def values ('6679','�Ŵ�ת����ת��','sp6679','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "mo_discnt_c.h"

int sp6679()
{
	int	iRet = 0;
	char	cGet_data[201];	
	double	dReal_amt = 0.00;
	long	lPnDays = 0;
	
	struct cms_rollback_c	sCms_rollback;
	struct mo_discnt_c	sMo_discnt;
	struct mo_discnt_c	sMoDiscnt; /**/
	
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(&sCms_rollback,0x00,sizeof( sCms_rollback));
	memset(&sMo_discnt,0x00,sizeof( sMo_discnt));
	memset(&sMoDiscnt,0x00,sizeof( sMoDiscnt));
	
	pub_base_sysinit();
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0370", cGet_data );		/* ת����Э���� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.tpact_no,cGet_data,sizeof(sMo_discnt.tpact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
		
	memcpy( sMo_discnt.br_no,g_pub_tx.tx_br_no,sizeof(sMo_discnt.br_no)-1);	/* �������� */
	sprintf( acErrMsg, "line 49���ڵ�br_no[%s]",sMo_discnt.br_no );/*20110817 ������������۵Ļ����Ż��������������ģ�������*/
	WRITEMSG

	get_zd_double( "0400", &sMo_discnt.par_amt );	/* ��Ʊ��� */
	get_zd_double( "0850", &sMo_discnt.tpn_rate );	/* ת������ */
	get_zd_long("0470",&sMo_discnt.tpn_date);	/* ת������ */
	sMo_discnt.wrk_date = g_pub_tx.tx_date;	/* ת������Ϣ¼��ʱ�������� */
	get_zd_long( "0460",&sMo_discnt.not_matr_date );/* ��Ʊ������ */
	get_zd_data( "0310", cGet_data );		/* ��Ʊ���ʺ� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.rem_ac_no,cGet_data,sizeof(sMo_discnt.rem_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0260", cGet_data);		/* ��Ʊ��ȫ�� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.remitter,cGet_data,sizeof(sMo_discnt.remitter)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data( "0250", cGet_data );		/* ������ȫ�� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payer_brno_name,cGet_data,sizeof(sMo_discnt.payer_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0320", cGet_data);		/* �տ����ʺ� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_ac_no,cGet_data,sizeof(sMo_discnt.payee_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	
	get_zd_data( "0270",cGet_data);			/* �տ���ȫ�� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee,cGet_data,sizeof(sMo_discnt.payee)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data( "0810", cGet_data );		/* �տ��˿�����ȫ�� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.payee_brno_name,cGet_data,sizeof(sMo_discnt.payee_brno_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));

	get_zd_data( "0670", cGet_data);		/* Ʊ������ */
	zip_space(cGet_data);
	if(cGet_data[0] == '1')
	{
		memcpy(sMo_discnt.note_type,"112",sizeof(sMo_discnt.note_type)-1);
	}else if(cGet_data[0] == '2')
	{
		memcpy(sMo_discnt.note_type,"111",sizeof(sMo_discnt.note_type)-1);
	}else{
		sprintf( acErrMsg, "Ʊ�����ʹ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B248" );
		goto ErrExit;
	}
	memset(cGet_data, 0x00, sizeof(cGet_data));
	get_zd_data( "0750",cGet_data);				/* Ʊ�ݺ� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.pnote_no,cGet_data,sizeof(sMo_discnt.pnote_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data( "0700", sMo_discnt.local_ind );		/* ���ͬ�Ǳ�־ */
	
	get_zd_long( "0520", &sMo_discnt.way_days );		/* ��;���� */
	/*get_zd_data( "0810", sMo_discnt.tpn_name );*//*������д���˰�*/	
	get_zd_data( "0810", cGet_data );	/* ���������� */
	zip_space(cGet_data);
	memcpy(sMo_discnt.tpn_name,cGet_data,sizeof(sMo_discnt.tpn_name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	sMo_discnt.tranx_bal = sMo_discnt.par_amt;				/* ��� */
   	/* ����Ʊ�����ͺ�Ʊ�ݺ��ҳ���¼ */
	iRet = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s' and sts='3' and br_no='%s' ",sMo_discnt.note_type, sMo_discnt.pnote_no,g_pub_tx.tx_br_no );
	if ( iRet )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	iRet = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( iRet == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ![%s],[%s]" ,sMo_discnt.note_type, sMo_discnt.pnote_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
	}else if( iRet )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	if(sMoDiscnt.pn_type[0] == '4')
	{
		sprintf( acErrMsg, "��Ʊ������ת����ת��!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��Ʊ������ת����ת��!"); 
		strcpy( g_pub_tx.reply, "B248" );
		goto ErrExit;
	}
	/* ���Ʊ��״̬�Ƿ�Ϊ����������������Ϊ���ֻ������ת�� */
	if( sMoDiscnt.pn_type[0] != '1' && sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "��Ʊ�ݷ����ֻ����ת�����ã�������ת����ת��!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"��Ʊ�ݷ����ֻ����ת�����ã�������ת����ת��!"); 
		strcpy( g_pub_tx.reply, "B154" );
		goto ErrExit;
	}

	if( sMoDiscnt.pn_type[0] == '1' )	/* ���� */
	{
		if( sMoDiscnt.sts[0] == '1' )/*Mo_discnt_Dec_Upd���������� sts='3'�������״̬�϶���3�� ��� */
		{
			sprintf( acErrMsg, "�˱�����δ���!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�˱�����δ���!"); 
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}
		else if( sMoDiscnt.sts[0] == '2' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�˱�����δ����!"); 
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	else					/* ���ת�� */
	{
		if( sMoDiscnt.sts[0] == '1' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			set_zd_data(DC_GET_MSG,"�˱�����δ����!"); 
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱�������������!" );
		WRITEMSG
		set_zd_data(DC_GET_MSG,"�˱�������������!"); 
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}
	strcpy(sMoDiscnt.sts, "*");    /* ��־��* ����*/ /* ���ʲô��˼�� */
	sprintf(sCms_rollback.rolbk_sql," sts='3',wrk_date=%ld ",sMoDiscnt.wrk_date);
	sMoDiscnt.wrk_date=g_pub_tx.tx_date;
	iRet = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( iRet )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();
	
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'U';
	sprintf(sCms_rollback.beg_sql," pn_type='%s' and pnote_no='%s' and sts='*' and br_no='%s' ",sMoDiscnt.pn_type,sMoDiscnt.pnote_no,sMoDiscnt.br_no);
	sCms_rollback.sts[0] = '0';
	sCms_rollback.filler[0] = '#';
	get_zd_data("1230",sCms_rollback.filler+1);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
	
	
	/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
	iRet = pub_base_GetParm_Str( "TXCP", 5, sMo_discnt.bkrl_code );
	if( iRet )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}
	
	/*****����������Ϣ�����У��������� = Ʊ�ݵ����� -ת��������+ ��;����,
		������Ϣ = ��Ʊ��� * �������� * ����ת������/30��/1000���� ****/
	iRet = pub_base_CalDays( sMo_discnt.tpn_date, sMo_discnt.not_matr_date, "0", &lPnDays );
	if( iRet )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}
	
	sMo_discnt.tpn_int = sMo_discnt.par_amt * (lPnDays + sMo_discnt.way_days) * (sMo_discnt.tpn_rate/30/1000);
	
	/* �Ǽ����ֵǼǱ���״̬Ϊ¼�� */
	strcpy( sMo_discnt.sts, "1" );				/* ��־��1¼�� */
	sMo_discnt.po_knd[0] = sMoDiscnt.po_knd[0] ;
	
	strcpy(sMo_discnt.pn_type, "4");                     /* ��������: 4���ת�� */
	iRet = Mo_discnt_Ins( sMo_discnt, g_pub_tx.reply );
	if ( iRet )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!iRet=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table,"mo_discnt",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," tpact_no='%s' and pnote_no='%s' and pn_type='4' and sts='1' ",sMo_discnt.tpact_no,sMo_discnt.pnote_no );
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
	/*************************
	*����Ϊֹ�����������������
	*������8��10�ţ�����������һ��A��¼��6��10�����ĵ�һ�ʽ��ס�Ʊ����123��Ȼ��
	*A��¼������״̬��    sts='*',pn_type='2', pact_no='A1',ac_id='1234' pnote_no='123' ת��
	                      sts='*',pn_type='4',tpact_no='A2',ac_id=null,  pnote_no='123' ת��
	*����������һ��B����  sts='*',pn_type='2', pact_no='B1',ac_id='5678' pnote_no='123' �µ�ת��
	                      sts='1',pn_type='4',tpact_no='B2',ac_id=null,  pnote_no='123' �µ�ת��   
	*����ת����������Ʊ�ݺŻ�����Ϣ��ʱ�򣬵���sp9934���ף���ѯ��������    
	    ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' \  //������ֵǼǱ����Ƿ���ڴ�Ʊ�� 
			  and pnote_no='%s' and pn_type='4' and br_no ='%s'", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no ,cKinbr); 
			ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' \  //�ټ�����ֵǼǱ��д�Ʊ�ݵĻ�����Ϣ 
			and pnote_no='%s' and pn_type in ('1','2') and br_no='%s'  ",sMoDiscnt1.note_type, sMoDiscnt1.pnote_no,cKinbr );
	*��Ϊû�ж�sts���жϣ����ԾͰ�A��¼��������ˡ���ΪA��¼��B��¼��Ʊ�ţ�pn_type ���кŶ�������һģһ����
	*�������ڻ����ܿ�sts���ж����µĽ��׻����Ͻ��ס�		                             
	****************************/
OkExit:
	sprintf( acErrMsg, "�Ŵ�ת����ת���ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ�ת����ת��ʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
