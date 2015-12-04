/***************************************************************
* �� �� ��:     pubf_acct_rollback.c
* ����������
*��Ա���ӣ����ջ�
*ƾ֤������ƾ֤����
*�ͻ��Ǽǡ�ά������ά����ɾ��
**��־�÷���
**��Ҫ�˶�ƾ֤��** g_pub_tx.ac_wrk_ind[0]='1'
**g_pub_tx.hst_ind[0]=='1'�ռ�����ϸ
**g_pub_tx.prt_ind[0]!='1'�Ƿ����
*
* ��    ��:	jack
* ������ڣ�    2003��12��27��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR%d@%s", __LINE__, __FILE__);WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_unprt_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "dc_log_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "tx_dc_br_rel_c.h"
#include "com_rollback_ctl_c.h"
#include "card_consum_lmt_c.h"
#include "ln_pay_pln_rol_c.h"
#include "tx_sub_def_c.h"
#include "note_mst_hst_c.h"
#include "note_mst_c.h"

struct mdm_attr_c v_mdm_attr;

pub_acct_rollback( long p_trace_no )
{
	struct dc_log_c vdc_log;
	struct trace_log_c vtrace_log;
	struct trace_log_c T_trace_log;
	struct tx_sub_def_c	T_tx_sub_def;
	struct note_mst_hst_c s_note_mst_hst;
	struct note_mst_c sNoteMst;
	int ret=0;
	int doflag=0;

	/***�������׼�¼***/
	ret=Trace_log_Dec_Sel( g_pub_tx.reply , "trace_no=%ld \
		order by add_ind,svc_ind desc" , p_trace_no );
	ERR_DEAL

	while(1)
	{
		ret = Trace_log_Fet_Sel( &vtrace_log, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		memcpy(&T_trace_log,&vtrace_log,sizeof(struct trace_log_c));

    sprintf( acErrMsg, "T_trace_log.svc_ind=[%d]", T_trace_log.svc_ind);
		WRITEMSG


		if(  vtrace_log.svc_ind==7021 && vtrace_log.tx_code[0]!='5')/**��Աƾ֤ʹ�ò�����**/
		{
			ret=Note_mst_hst_Dec_Upd( g_pub_tx.reply,
				"tx_date=%ld and trace_no=%ld and sts_ind='D' ",
				vtrace_log.tx_date,vtrace_log.trace_no );
			ERR_DEAL
			ret=Note_mst_hst_Fet_Upd( &s_note_mst_hst,g_pub_tx.reply );
			vtcp_log("[%s][%d]���������� ret==[%d]\n",__FILE__,__LINE__,ret);
			if( ret==100 ) continue;
			ERR_DEAL
			s_note_mst_hst.sts_ind[0]='E';
			ret=Note_mst_hst_Upd_Upd( s_note_mst_hst,g_pub_tx.reply );
			ERR_DEAL
			Note_mst_hst_Clo_Upd();

			memset( &sNoteMst,0,sizeof(sNoteMst) );
			strcpy( sNoteMst.tel , vtrace_log.tel );
			strcpy( sNoteMst.br_no, vtrace_log.tx_br_no );
			strcpy( sNoteMst.note_type, vtrace_log.note_type );
			strcpy( sNoteMst.beg_note_no, vtrace_log.note_no );
			strcpy( sNoteMst.end_note_no, vtrace_log.note_no );
			sNoteMst.cnt = 1;
			/* �ֶδ���(�޸�ƾ֤״̬) */
			ret = pub_com_NoteApart( "e", sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "�ֶδ���(�޸�ƾ֤״̬)��" );
				WRITEMSG
				goto ErrExit;;
			}

			continue;
		}

		if( vtrace_log.sts[0]=='1' )
		{
			sprintf(acErrMsg,"�Ѿ�����[%d]",p_trace_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S052" );
			goto ErrExit;
		}

		ret=pub_acct_Rol_chk( &vtrace_log ) ;
		ERR_DEAL
		vtcp_log("rollback [%s][%ld]",vtrace_log.ac_no,vtrace_log.ac_id);

		/**--- xxx ---- @20050818@ ----- 2505�ڲ�������,2501�ڲ��ʿ���**/
		memset(&T_tx_sub_def,0x00,sizeof(struct tx_sub_def_c));
		ret=Tx_sub_def_Sel(g_pub_tx.reply,&T_tx_sub_def," sub_tx_code='%s' ",vtrace_log.sub_tx_code);
		ERR_DEAL
		/* modify begin by wzs 20120321 reason:���Ӷ����˴���Ķ�Ӧ���� */
		/* if (vtrace_log.no_show[0] != '1' &&
			(T_tx_sub_def.acct_ind[0]!='0' || !strcmp(vtrace_log.sub_tx_code,"D505")
			|| !strcmp(vtrace_log.tx_code,"2501") ) ) */
		if (vtrace_log.no_show[0] != '1' && strcmp(vtrace_log.sub_tx_code,"L542") && (T_tx_sub_def.acct_ind[0]!='0'
			|| !strcmp(vtrace_log.sub_tx_code,"D505") || !strcmp(vtrace_log.tx_code,"2501") || !strcmp(vtrace_log.tx_code,"6701")))
		{
			ret=pub_acct_rollback_tx( &vtrace_log ) ;
			ERR_DEAL
		}
		/* else if( !strncmp(vtrace_log.sub_tx_code,"N1",2) ) */
		else if ( !strncmp(vtrace_log.sub_tx_code,"N1",2) && strcmp(vtrace_log.sub_tx_code,"L542") )
		/* modify end by wzs 20120321 */
		{
			ret=pub_acct_rollback_tx( &vtrace_log ) ;
			ERR_DEAL
		}
		/* add begin by wzs 20120322 reason:���Ӷ����˴����Ӧ�ĳ��� */
		else if((!strcmp(vtrace_log.tx_code,"2508") || !strcmp(vtrace_log.tx_code,"2212") \
			|| !strcmp(vtrace_log.tx_code,"6615") || !strcmp(vtrace_log.tx_code,"2214") \
			|| !strcmp(vtrace_log.tx_code,"2217")) && vtrace_log.svc_ind != 1201 && vtrace_log.svc_ind != 1202)
		{
			ret=pub_acct_rollback_tx( &vtrace_log ) ;
			ERR_DEAL
		}
		/* add end by wzs 20120322 */

		ret=pub_acct_Rol_reg( &vtrace_log ) ;
		ERR_DEAL

		ret=pub_acct_Rol_special( &vtrace_log ) ;
		ERR_DEAL

		TRACE
		strcpy( vtrace_log.sts , "1" );
		TRACE


		ret=sql_execute("update trace_log set sts='1' where trace_no=%ld and trace_cnt=%d",
			T_trace_log.trace_no,T_trace_log.trace_cnt);
		ERR_DEAL

		doflag++;
	}
	Trace_log_Clo_Upd();

	ret=pub_acct_Rol_spe( &vtrace_log ) ;
	ERR_DEAL

	if( doflag==0 )
	{
		strcpy( g_pub_tx.reply,"Z106" );
		TRACE
		goto ErrExit;
	}

	/***������Ƽ�¼***/
	ret=Dc_log_Dec_Upd( g_pub_tx.reply , "trace_no=%d "
		, p_trace_no );
	ERR_DEAL

	while(1)
	{
		ret = Dc_log_Fet_Upd( &vdc_log, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		/***������ʹ��ƾ֤����***/
		if( !strcmp(vdc_log.entry_code,"N00100") ) continue;

		if( vdc_log.sts[0]=='1' )
		{
			sprintf(acErrMsg,"�Ѿ�����[%d]",p_trace_no);
			WRITEMSG
			strcpy( g_pub_tx.reply,"S052" );
			goto ErrExit;
		}

		ret=pub_acct_rollback_dc( &vdc_log ) ;
		ERR_DEAL

		strcpy( vdc_log.sts , "1" );

		ret=Dc_log_Upd_Upd( vdc_log, g_pub_tx.reply );
		ERR_DEAL

		doflag++;
	}
	Dc_log_Clo_Upd();

	return 0;
ErrExit:
	return 1;
}
int pub_acct_rollback_tx( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	struct note_parm_c note_parm_c;
	struct trace_log_c vtrace_log;
	struct prdt_ac_id_c vprdt_ac_id;
	int ret=0;

	memcpy( &vtrace_log,ptrace_log,sizeof(vtrace_log) );
	
	vtcp_log("77777777777777[%s]--[%s]--",g_pub_tx.brf,vtrace_log.brf);
	/***mod zgf 20130725 ע�͵�����Ϊ����֪ͨҪ��ȡԭ���׵ı�ע��Ϣ+����***/
	/*strcpy( g_pub_tx.brf,"����" );*/
	strcpy( g_pub_tx.brf,vtrace_log.brf );
	/***mod zgf end***/
	strcpy( g_pub_tx.ac_no, vtrace_log.ac_no );
	pub_base_strpack( g_pub_tx.ac_no );
	g_pub_tx.ac_id=vtrace_log.ac_id;
	g_pub_tx.ac_seqn=vtrace_log.ac_seqn;

	/**��Ʒ**/
	if( vtrace_log.ac_id>=100000000 )
	{
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&vprdt_ac_id,
			"ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		ERR_DEAL

		g_pub_tx.ac_id_type[0]=vprdt_ac_id.ac_id_type[0];
	}
	else
	{
		g_pub_tx.ac_id_type[0]='9';
		vtrace_log.ac_id=0;
	}

	g_pub_tx.tx_amt1=ptrace_log->amt;
	g_pub_tx.svc_ind=ptrace_log->svc_ind;
	ptrace_log->trace_cnt--;
	g_pub_tx.trace_cnt = ptrace_log->trace_cnt;
	set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);

	strcpy( g_pub_tx.sub_tx_code,ptrace_log->sub_tx_code);
	strcpy( g_pub_tx.cur_no,ptrace_log->cur_no);

	sprintf(acErrMsg, " ---------> cur_no = [%s],svc_ind[%d]", g_pub_tx.cur_no,g_pub_tx.svc_ind);
	WRITEMSG
	sprintf(acErrMsg, " ---------> tx_amt1 = [%lf]", g_pub_tx.tx_amt1);
	WRITEMSG
	strcpy( g_pub_tx.add_ind,ptrace_log->add_ind);
	strcpy( g_pub_tx.ct_ind,ptrace_log->ct_ind);
	strcpy( g_pub_tx.hst_ind,ptrace_log->hst_ind);

		/*** ��鴦��ֻ���,��ϸ�� ***/
		switch( g_pub_tx.ac_id_type[0] )
		{
			case '1':
				ret=Rol_dd_mst(&vtrace_log);
				ERR_DEAL
				break;
			case '2':
				ret=Rol_td_mst(&vtrace_log);
				ERR_DEAL
				break;
			case '3':
				ret=Rol_ln_mst(&vtrace_log);
				ERR_DEAL
				break;
			case '9':
	sprintf(acErrMsg, " ---------> cur_no = [%s]", g_pub_tx.cur_no);
	WRITEMSG
				ret=strlen(g_pub_tx.ac_no);
				if(ret==3||ret==5||ret==7)
					ret=Rol_acchrt_mst(&vtrace_log);
				else if( !strncmp(vtrace_log.ac_no,"91011",5) )
					ret=Rol_cash(&vtrace_log);
				else
					ret=Rol_in_mst(&vtrace_log);
				ERR_DEAL
				break;
			default:
				sprintf(acErrMsg,"�޴��˺�����![%s]",g_pub_tx.ac_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
		}

	memcpy( ptrace_log,&vtrace_log,sizeof(vtrace_log) );

	sprintf(acErrMsg,"Rollback [%s]",vtrace_log.note_type,vtrace_log.note_no);

vtcp_log("%s,%d TX[%s][%s][%s]",__FILE__,__LINE__,vtrace_log.tx_code,vtrace_log.sub_tx_code,vtrace_log.ac_no);
	if( !strcmp(vtrace_log.tx_code,"5105") &&
		!strcmp(vtrace_log.sub_tx_code,"N105") &&
		!strncmp(vtrace_log.ac_no,"702",3) )
	{
vtcp_log("%s,%d Tx[%s][%s][%s]",__FILE__,__LINE__,vtrace_log.tx_code,vtrace_log.sub_tx_code,vtrace_log.ac_no);
		if( pub_com_NoteCX( vtrace_log.trace_no, 0, g_pub_tx.tx_date, "N" ) )
			goto ErrExit;
	}
	else if( !strncmp(vtrace_log.sub_tx_code,"N1",2) &&
		!strncmp(vtrace_log.ac_no,"702",3) )
	{
vtcp_log("%s,%d Tx[%s][%s][%s]",__FILE__,__LINE__,vtrace_log.tx_code,vtrace_log.sub_tx_code,vtrace_log.ac_no);
		if( pub_com_NoteCX( vtrace_log.trace_no, 0, g_pub_tx.tx_date, "N" ) )
			goto ErrExit;
	}
	else
	{
vtcp_log("%s,%d Tx[%s][%s][%s]",__FILE__,__LINE__,vtrace_log.tx_code,vtrace_log.sub_tx_code,vtrace_log.ac_no);
	/***�ͻ�ʹ��ƾ֤����**/
	vtcp_log("%s,%d �ͻ�ʹ��ƾ֤������ʼ",__FILE__,__LINE__);
	pub_base_strpack( vtrace_log.note_type );
		g_reply_int=Note_parm_Sel(g_pub_tx.reply,&note_parm_c,
			"note_type='%s' ",vtrace_log.note_type);
		if( g_reply_int==100 )
		{
			;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡƾ֤��������![%s,%d]",g_pub_tx.note_type,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N018");
			goto ErrExit;
		}
		else if( note_parm_c.sell_ind[0]=='Y' )
		{
			pub_com_CheqNoCX( vtrace_log.trace_no, vtrace_log.trace_cnt,
					vtrace_log.tx_date, "Y" );
		}
	}
	return 0;
ErrExit:
	return 1;
}
int pub_acct_rollback_dc( pdc_log )
 struct dc_log_c *pdc_log;
{
	int ret=0;
	double amt;
	char wherelist[200];
	int i=0;

	struct gl_mst_c v_gl_mst;
	struct com_item_c v_com_item;
	struct com_item_c u_com_item;
	struct tx_dc_br_rel_c v_tx_dc_br_rel;

	memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&v_com_item,0x00,sizeof(struct com_item_c));

	strcpy( g_pub_tx.cur_no,pdc_log->cur_no );
	pub_base_strpack( pdc_log->acc_hrt );
	g_pub_tx.tx_amt1=-pdc_log->amt;
	if( pdc_log->dc_ind[0]=='1' )
		g_pub_tx.add_ind[0]='0';
	else
		g_pub_tx.add_ind[0]='1';

	/*����Ŀ����*/
	ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_hrt='%s'",
		pdc_log->acc_hrt);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿ�Ŀ������[%s]",pdc_log->acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	pdc_log->sts[0]='2';

	/*��Ŀ����Ϊ���ʻ�����*/
	if(v_com_item.acc_knd[0]=='3')
	{
		sprintf(acErrMsg,"�ÿ�Ŀ���������ʻ���Ŀ!");
		WRITEMSG
	}
	else
	{
		sprintf(acErrMsg,"�ÿ�Ŀ���Ͳ������ʻ���Ŀ!");
		WRITEMSG
		goto OkExit;
	}

	/**ȡ�ö�Ӧ�Ļ�ƻ���**/
	ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&v_tx_dc_br_rel,"tx_br_no='%s'",
		g_pub_tx.tx_br_no );
	if( ret )
	{
		sprintf(acErrMsg,"���׻����޶�Ӧ��ƻ���[%s]",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S029");
		goto ErrExit;
	}

	memset( wherelist,0,sizeof(wherelist) );
	memcpy( &u_com_item,&v_com_item,sizeof(u_com_item) );
	pub_base_strpack( u_com_item.acc_hrt );
	pub_base_strpack( u_com_item.up_acc_hrt );

	strcpy( g_pub_tx.dc_code,"ACHR" );
	set_zd_data("0152",v_com_item.acc_hrt );

	sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
		v_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no, u_com_item.acc_hrt );
	/**ȡ���ϼ���Ŀ**/
	for ( i=0;i<2;i++ )
	{
		if( !strcmp(u_com_item.acc_hrt,u_com_item.up_acc_hrt) )
			break;

		ret=Com_item_Sel(g_pub_tx.reply,&u_com_item,"acc_hrt='%s'",
			u_com_item.up_acc_hrt);
		if( ret )
		{
			sprintf(acErrMsg,"�ÿ�Ŀ�����ֲ�����[%s]",u_com_item.up_acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		pub_base_strpack( u_com_item.acc_hrt );
		pub_base_strpack( u_com_item.up_acc_hrt );
		strcat( wherelist,",'" );
		strcat( wherelist,u_com_item.acc_hrt );
		strcat( wherelist,"'" );
	}

	strcat( wherelist,")" );

	sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
	WRITEMSG
OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_acct_rollback_dc_old( pdc_log )
 struct dc_log_c *pdc_log;
{
	int ret=0;
	double amt;
	char wherelist[200];
	int i=0;

	struct gl_mst_c v_gl_mst;
	struct com_item_c v_com_item;
	struct com_item_c u_com_item;
	struct tx_dc_br_rel_c v_tx_dc_br_rel;

	memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&v_com_item,0x00,sizeof(struct com_item_c));

	strcpy( g_pub_tx.cur_no,pdc_log->cur_no );
	pub_base_strpack( pdc_log->acc_hrt );
	g_pub_tx.tx_amt1=-pdc_log->amt;
	if( pdc_log->dc_ind[0]=='1' )
		g_pub_tx.add_ind[0]='0';
	else
		g_pub_tx.add_ind[0]='1';

	/*����Ŀ����*/
	ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_hrt='%s'",
		pdc_log->acc_hrt);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿ�Ŀ������[%s]",pdc_log->acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

	pdc_log->sts[0]='2';

	/*��Ŀ����Ϊ���ʻ�����*/
	if(v_com_item.acc_knd[0]=='3')
	{
		sprintf(acErrMsg,"�ÿ�Ŀ���������ʻ���Ŀ!");
		WRITEMSG
	}
	else
	{
		sprintf(acErrMsg,"�ÿ�Ŀ���Ͳ������ʻ���Ŀ!");
		WRITEMSG
		goto OkExit;
	}

	/**ȡ�ö�Ӧ�Ļ�ƻ���**/
	ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&v_tx_dc_br_rel,"tx_br_no='%s'",
		g_pub_tx.tx_br_no );
	if( ret )
	{
		sprintf(acErrMsg,"���׻����޶�Ӧ��ƻ���[%s]",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S029");
		goto ErrExit;
	}

	memset( wherelist,0,sizeof(wherelist) );
	memcpy( &u_com_item,&v_com_item,sizeof(u_com_item) );
	pub_base_strpack( u_com_item.acc_hrt );
	pub_base_strpack( u_com_item.up_acc_hrt );

	strcpy( g_pub_tx.dc_code,"ACHR" );
	set_zd_data("0152",v_com_item.acc_hrt );

	sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
		v_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no, u_com_item.acc_hrt );
	/**ȡ���ϼ���Ŀ**/
	for ( i=0;i<2;i++ )
	{
		if( !strcmp(u_com_item.acc_hrt,u_com_item.up_acc_hrt) )
			break;

		ret=Com_item_Sel(g_pub_tx.reply,&u_com_item,"acc_hrt='%s'",
			u_com_item.up_acc_hrt);
		if( ret )
		{
			sprintf(acErrMsg,"�ÿ�Ŀ�����ֲ�����[%s]",u_com_item.up_acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		pub_base_strpack( u_com_item.acc_hrt );
		pub_base_strpack( u_com_item.up_acc_hrt );
		strcat( wherelist,",'" );
		strcat( wherelist,u_com_item.acc_hrt );
		strcat( wherelist,"'" );
	}

	strcat( wherelist,")" );

	sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
	WRITEMSG

	/*�޸�����*/
	ret=Gl_mst_Dec_Upd(g_pub_tx.reply,wherelist);
	if( ret )
	{
		sprintf(acErrMsg,"��ѯ�����쳣!![%s]",v_com_item.acc_hrt);
		WRITEMSG
		goto ErrExit;
	}

	i=0;
	while( 1 )
	{
		ret=Gl_mst_Fet_Upd(&v_gl_mst,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"��ѯ�����쳣!![%s]",v_com_item.acc_hrt);
			WRITEMSG
			goto ErrExit;
		}
		i++;

		switch(g_pub_tx.add_ind[0])					/*���׽������*/
		{
			case '0': /**�跽����**/
				if(v_com_item.amt_dc_ind[0]=='2')
				{
					sprintf(acErrMsg,"�˿�Ŀֻ�ܼǴ�����![%s]",g_pub_tx.ac_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"A025");
					goto ErrExit;
				}

				if(v_com_item.dc_ind[0]=='1')				/*�账��*/
				{
					v_gl_mst.dr_bal+=g_pub_tx.tx_amt1;
				}
				else if(v_com_item.dc_ind[0]=='2')			/*������*/
				{
					v_gl_mst.cr_bal-=g_pub_tx.tx_amt1;
				}
				else if(v_com_item.dc_ind[0]=='0')			/*˫����*/
				{
					amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal+g_pub_tx.tx_amt1;
					ret=pub_base_CompDblVal(amt,0.00);
					if(ret==1)
					{
						v_gl_mst.dr_bal=amt;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
					}
					else if(ret==-1)
					{
						v_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=-amt;		/*��ǰ�������*/
					}
					else
					{
						v_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
					}
				}
				v_gl_mst.rdd_cnt-=1;					/*���ս����*/
				v_gl_mst.rdd_amt+=g_pub_tx.tx_amt1;		/*���ս跢����*/
				v_gl_mst.tddr_cnt-=1;					/*��Ѯ�����*/
				v_gl_mst.tddr_amt+=g_pub_tx.tx_amt1;	/*��Ѯ�跢����*/
				v_gl_mst.mdr_cnt-=1;					/*���½����*/
				v_gl_mst.mdr_amt+=g_pub_tx.tx_amt1;		/*���½跢����*/
				v_gl_mst.qdr_cnt-=1;					/*���������*/
				v_gl_mst.qdr_amt+=g_pub_tx.tx_amt1;		/*�����跢����*/
				v_gl_mst.ydr_cnt-=1;					/*��������*/
				v_gl_mst.ydr_amt+=g_pub_tx.tx_amt1;		/*����跢����*/
				break;
			case '1': /*��������*/
				if(v_com_item.amt_dc_ind[0]=='1')
				{
					sprintf(acErrMsg,"�˿�Ŀֻ�ܼǽ跽��![%s]",g_pub_tx.ac_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"A024");
					goto ErrExit;
				}

				if(v_com_item.dc_ind[0]=='1')				/*�账��*/
				{
					v_gl_mst.dr_bal-=g_pub_tx.tx_amt1;
				}
				else if(v_com_item.dc_ind[0]=='2')			/*������*/
				{
					v_gl_mst.cr_bal+=g_pub_tx.tx_amt1;
				}
				else if(v_com_item.dc_ind[0]=='0')			/*˫����*/
				{
					amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal-g_pub_tx.tx_amt1;
					ret=pub_base_CompDblVal(amt,0.00);
					if(ret==1)
					{
						v_gl_mst.dr_bal=amt;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
					}
					else if(ret==-1)
					{
						v_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=-amt;		/*��ǰ�������*/
					}
					else
					{
						v_gl_mst.dr_bal=0.00;		/*��ǰ�跽���*/
						v_gl_mst.cr_bal=0.00;		/*��ǰ�������*/
					}
				}
				v_gl_mst.rcd_cnt-=1;					/*���մ�����*/
				v_gl_mst.rcd_amt+=g_pub_tx.tx_amt1;		/*���մ�������*/
				v_gl_mst.tdcr_cnt-=1;					/*��Ѯ������*/
				v_gl_mst.tdcr_amt+=g_pub_tx.tx_amt1;	/*��Ѯ��������*/
				v_gl_mst.mcr_cnt-=1;					/*���´�����*/
				v_gl_mst.mcr_amt+=g_pub_tx.tx_amt1;		/*���´�������*/
				v_gl_mst.qcr_cnt-=1;					/*����������*/
				v_gl_mst.qcr_amt+=g_pub_tx.tx_amt1;		/*������������*/
				v_gl_mst.ycr_cnt-=1;					/*���������*/
				v_gl_mst.ycr_amt+=g_pub_tx.tx_amt1;		/*�����������*/
				break;
			default:
				sprintf(acErrMsg,"��ȡ��־����=[%s]!",g_pub_tx.add_ind);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P106");
				goto ErrExit;
				break;
		}

		ret=Gl_mst_Upd_Upd(v_gl_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"Update ���ʳ���!![%s]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
	}
	Gl_mst_Clo_Upd();

	if( i==0 )
	{
		sprintf(acErrMsg,"�����в����ڸÿ�Ŀv_gl_mst.acc_hrt=[%s]", \
			v_gl_mst.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O075");
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
asdad()
{
	int ret=0;
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c mdm_attr_c;
	struct note_parm_c note_parm_c;
    int factday,lastday;

	memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
	memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
	memset(&note_parm_c,0x00,sizeof(struct note_parm_c));

	/**��ʼ�����ļ�����**
	ret=pub_sub_ac();
	if( ret==100 ) return 0; ***TEMP�����˺�***/
	ERR_DEAL

	/**ƾ֤����**/
	ret=pub_sub_note();
	ERR_DEAL

	/***��鴦��ֻ���***/
	ret=pub_sub_mst();
	ERR_DEAL

	/***�����۴���***/
	ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
	ERR_DEAL

	/***�����۴���***/
	ret=pub_sub_prt_book();
	ERR_DEAL

	/***ͨ��ͨ�Ҵ���***/
	/**
	ret=pub_sub_liqu();
	ERR_DEAL
	**/

	/***�Ǽǽ�����ˮ***/
	if( g_pub_tx.ac_id_type[0]!='3' && g_pub_tx.hst_ind[0]!='0' )
	{
		/***�����ر���***/
		ret=pub_sub_spe_do();
		ERR_DEAL
		TRACE
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);
OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"ҵ����ϵͳ���˴���ɹ�!");
	WRITEMSG
	return 0;

ErrExit:
	sprintf(acErrMsg,"ҵ����ϵͳ���˴���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
Rol_cash( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));

	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.cur_no,ptrace_log->cur_no );

	sprintf(acErrMsg,"--------> cur_no = [%s]", ptrace_log->cur_no);
	WRITEMSG

	g_pub_tx.tx_amt1=-ptrace_log->amt;

	/*** ���ݽ��׻����ź͹�Ա����ȡ�ֽ��˺�ID ***/
	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	/* �������Ƿ���ڣ��Ƿ����� */
	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "�����ִ�" );
		WRITEMSG
		goto ErrExit;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"�ֽ��˺�δ���û�״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no,cash_mst_c.ac_no);

	if( g_pub_tx.add_ind[0]=='0' )
		cash_mst_c.bal=cash_mst_c.bal-g_pub_tx.tx_amt1;
	else if( g_pub_tx.add_ind[0]=='1' )
		cash_mst_c.bal=cash_mst_c.bal+g_pub_tx.tx_amt1;
	else
	{
		sprintf(acErrMsg,"������־����![%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( cash_mst_c.bal<=-0.005)
	{
		sprintf(acErrMsg,"�ֽ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P152");
		goto ErrExit;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Cash_mst_Clo_Upd();

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	/*** �Ǽ�Ǯ����ϸ ***/
	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	strcpy(cash_mst_hst_c.add_ind,g_pub_tx.add_ind);
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,g_pub_tx.brf);
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}
	TRACE
	/* ������������Ǽ���ˮ
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	***********/

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OkExit:
	return 0;
ErrExit:
	return 1;
}
int Rol_dd_mst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret;
	double tmpje=0.00;
	struct card_consum_lmt_c v_card_consum_lmt;

	strcpy( g_pub_tx.cur_no,ptrace_log->cur_no );
	g_pub_tx.tx_amt1=-ptrace_log->amt;

	/*** ���û��ڲ�Ʒ���������麯�� ***/
	/*** �������ļ���� ***/
	ret=Dd_mst_Dec_Upd(g_pub_tx.reply,
		"ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**��ȡ��Ʒ����**/
	ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
			" prdt_no='%s' ",g_dd_mst.prdt_no );
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    /****��Ȫadd**/
    strcpy( g_pub_tx.opn_br_no,g_dd_mst.opn_br_no );
	/****end***/
	strcpy(g_pub_tx.name,g_dd_mst.name);
	g_pub_tx.cif_no=g_dd_mst.cif_no;

	/*ȡ����š�Э������*/
	if( g_pub_tx.add_ind[0]=='0' )
	{
		/*** Э������� ***/
		if( g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_mst.hold_amt
		|| g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_parm.min_bal )
		{
			ret=pub_base_CompDblVal(g_dd_mst.hold_amt,g_dd_parm.min_bal);
			if( ret>=0 )
		tmpje=g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_pub_tx.tx_amt1-g_dd_mst.bal;
			else
		tmpje=g_dd_mst.ctl_amt+g_dd_parm.min_bal+g_pub_tx.tx_amt1-g_dd_mst.bal;

			if( pub_sub_prot(tmpje) )
			{
				sprintf(acErrMsg,"Э����������!");
				WRITEMSG
				goto ErrExit;
			}
		}

		/*** ��������Ϣ�����ĺ��� ***/
		sprintf(acErrMsg, "xxx -------> svc_ind = [%ld]", g_pub_tx.svc_ind);
		WRITEMSG

		if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012)
		{
			sprintf(acErrMsg, "xxx ---------->���׽��:[%lf]",
				g_pub_tx.tx_amt1);
			WRITEMSG

			if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
		}

		switch( g_pub_tx.svc_ind )
		{
			case 1006: 		/*Ԥ��Ȩ*/
				g_dd_mst.ctl_amt=g_dd_mst.ctl_amt+g_pub_tx.tx_amt1;
				break;
			case 1007: 		/*Ԥ��Ȩ���*/
				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				break;
			case 1003: 		/*����*/
                /* ���ü�Ϣ���� */

				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				if( g_dd_mst.ac_sts[0]!='*' )
				{
					sprintf(acErrMsg,"�˻�״̬��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O156");
					goto ErrExit;
				}

				strcpy(g_dd_mst.ac_sts,"1");
				break;
			case 1004: 		/*Э������*/
				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				if( g_dd_mst.ac_sts[0]!='*' )
				{
					sprintf(acErrMsg,"�˻�״̬��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O156");
					goto ErrExit;
				}

				strcpy(g_dd_mst.ac_sts,"1");
				break;
			/*
			default :
				sprintf( acErrMsg,"�������ʹ���[%d]",g_pub_tx.svc_ind );
				WRITEMSG
				strcpy( g_pub_tx.reply,"D999" );
				goto ErrExit;
			case 1012: * ������Ϣ *
			case 1001: 		*��ȡ��˴�Ϊȡ��*
			case 1008: 		*Э��*
			*/
			default:
				/***ȡ��ʱ��鿨�޶�ǼǱ�������***/
				/***ADD BY Azz.05.03.26***/
				ret=Card_consum_lmt_Dec_Upd(g_pub_tx.reply,"ac_no='%s'", \
											g_pub_tx.ac_no);
				if(ret)
				{
					sprintf(acErrMsg,"DECLARE ���޶�ǼǱ��쳣!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P121");
					goto ErrExit;
				}

				ret=Card_consum_lmt_Fet_Upd(&v_card_consum_lmt,g_pub_tx.reply);
				if(ret&&ret!=100)
				{
					sprintf(acErrMsg,"FETCH ���޶�ǼǱ��쳣!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P120");
					goto ErrExit;
				}
				else if(ret==0)
				{
					if( g_pub_tx.ct_ind[0]=='1' ) /**ȡ��**/
					{
						/***ȡ�ּ�鿨�޶��****/
						memset(&v_card_consum_lmt,0x0, \
								sizeof(struct card_consum_lmt_c));
						ret=Card_consum_lmt_Sel(g_pub_tx.reply, \
							&v_card_consum_lmt,"ac_no='%s'",g_pub_tx.ac_no);
						if(!ret)
						{
							if(g_pub_tx.tx_amt1+v_card_consum_lmt.da_all_cash_lmt>v_card_consum_lmt.da_cash_lmt)
							{
								sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"P231");
								goto ErrExit;
							}
							if(g_pub_tx.tx_amt1+v_card_consum_lmt.mo_all_cash_lmt>v_card_consum_lmt.mo_cash_lmt)
							{
								sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
								WRITEMSG
								strcpy(g_pub_tx.reply,"P232");
								goto ErrExit;
							}

							v_card_consum_lmt.da_all_cash_lmt = \
							g_pub_tx.tx_amt1+v_card_consum_lmt.da_all_cash_lmt;

							v_card_consum_lmt.mo_all_cash_lmt = \
							g_pub_tx.tx_amt1+v_card_consum_lmt.mo_all_cash_lmt;
						}
					}

					if(g_pub_tx.tx_amt1+v_card_consum_lmt.da_all_amt_lmt> \
						v_card_consum_lmt.da_amt_lmt)
					{
						sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"P233");
						goto ErrExit;
					}
					if(g_pub_tx.tx_amt1+v_card_consum_lmt.mo_all_amt_lmt> \
						v_card_consum_lmt.mo_amt_lmt)
					{
						sprintf(acErrMsg,"����ÿ�����ȡ���޶�!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"P234");
						goto ErrExit;
					}

					v_card_consum_lmt.da_all_amt_lmt = \
					g_pub_tx.tx_amt1+v_card_consum_lmt.da_all_amt_lmt;

					v_card_consum_lmt.mo_all_amt_lmt = \
					g_pub_tx.tx_amt1+v_card_consum_lmt.mo_all_amt_lmt;

					ret=Card_consum_lmt_Upd_Upd(v_card_consum_lmt,g_pub_tx.reply);
					ERR_DEAL
				}
				Card_consum_lmt_Clo_Upd();
				/***FINISH***/

				g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
				break;
		}

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
		}

		if( g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"���˺��ѱ�ֻ����������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}

		if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt )
		{
			sprintf(acErrMsg,"���˺��ѱ����ֶ���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P111");
			goto ErrExit;
		}

		/* add by xxx at 20030801 **/
		if (g_pub_tx.svc_ind != 1012)
		{
			if( g_dd_mst.bal<g_dd_mst.ctl_amt+g_dd_mst.hold_amt )
			{
				sprintf(acErrMsg,"���˺������ڿ��ƽ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"A030");
				goto ErrExit;
			}
		}
	}
	else if( g_pub_tx.add_ind[0]=='1' )		/*���*/
	{
		if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012)
		{
			if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
				g_dd_mst.bal,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
		}
		switch( g_pub_tx.svc_ind )
		{
			case 1000: 		/*����*/
				g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
				if( g_dd_mst.ac_sts[0]!='1' )
				{
					sprintf(acErrMsg,"�˻�״̬��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P109");
					goto ErrExit;
				}

				strcpy(g_dd_mst.ac_sts,"4");
				break;
			case 1012: /* ��Ϣ */
			case 1001: 		/*���*/
				g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
				break;
			case 1006: 		/*Ԥ��Ȩ*/
				g_dd_mst.ctl_amt=g_dd_mst.ctl_amt-g_pub_tx.tx_amt1;
				if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0 )
				{
					sprintf(acErrMsg,"Ԥ��Ȩ��ɽ���[%.2lf]<[%.2lf]!",
						g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
					WRITEMSG
					strcpy( g_pub_tx.reply,"A031" );
					goto ErrExit;
				}
				break;
			default :
				g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
				g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
				/*
				sprintf( acErrMsg,"�������ʹ���[%d]",g_pub_tx.svc_ind );
				WRITEMSG
				strcpy( g_pub_tx.reply,"D999" );
				goto ErrExit;
				*/
		}
		/*** ��������Ϣ�����ĺ��� ***/

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
		}
	}
	else
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_dd_mst.lst_date=g_pub_tx.tx_date;

	/*** modify by xxx at 20050801 **/
	if (g_pub_tx.svc_ind != 1012)
	{
		if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.min_bal)<0
			&& pub_base_CompDblVal(g_dd_mst.bal,0.00) )
		{
			sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
				g_dd_mst.bal,g_dd_parm.min_bal );
			WRITEMSG
			strcpy(g_pub_tx.reply,"A022" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_bal)>0
			&& pub_base_CompDblVal(g_dd_parm.max_bal,0.00) )
		{
			sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
				g_dd_mst.bal,g_dd_parm.max_bal );
			WRITEMSG
			strcpy(g_pub_tx.reply,"A008" );
			goto ErrExit;
		}
	}

	if( g_dd_mst.ac_sts[0]=='4' && pub_base_CompDblVal(g_dd_mst.bal,0.00) )
	{
		sprintf(acErrMsg,"bal[%.2lf] sts[%s]",g_dd_mst.bal,g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D191" );
		goto ErrExit;
	}
	if( !strcmp(ptrace_log->sub_tx_code,"E702") )
	{
		;
	}
		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			if( g_dd_mst.ac_type[0]=='5' || g_dd_mst.ac_type[0]=='7' )
			{
				ret=pub_ins_dd_hst( );
				ERR_DEAL
			}
			else
			{
				ret=pub_del_dd_hst( ptrace_log );
				if( ret==100 )
				{
					ret=pub_ins_dd_hst( );
					ERR_DEAL
				}
				else
				{
				ERR_DEAL
				}
			}
		}

   pub_mob_sendmail( g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );

	ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Dd_mst_Clo_Upd();

	ret=pub_acct_ac_rec( g_dd_mst.ac_id,g_dd_mst.ac_seqn,
		g_pub_tx.add_ind,g_pub_tx.ct_ind,ptrace_log->tx_code,g_pub_tx.tx_amt1,"0000" );
	if( ret ) goto ErrExit;

OkExit:
	return 0;
ErrExit:
	return 1;
}
pub_del_dd_hst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;
	int ret1 = 0;
	int iCount = 0;
	double dAcbl = 0.00;

	double dTram = 0.00;	/* ������ˮ��Ӧ��� */
	char cAdd_ind[2];	/* 0����;1���� */

	struct dd_mst_hst_c sDd_mst_hst;

	memset( cAdd_ind, 0x0, sizeof(cAdd_ind) );

	iCount=sql_count( "dd_mst_hst" , "tx_date=%d and trace_no>%d and ac_id=%d",
		ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
	if( iCount<0 )
	{
		goto ErrExit;
	}
	/*--------------------------------------------------*
	else if ( iCount>0 )
	{
		return 100;
	}
	*-------------------------------------------------*/
	/*-------add by xxx begin ---at 20051107----------------------------*/
	else if( iCount>0 )
	{
		ret = Dd_mst_hst_Dec_Sel( g_pub_tx.reply, "tx_date=%d \
			and trace_no=%d and ac_id=%d", \
			ptrace_log->tx_date, ptrace_log->trace_no, \
			ptrace_log->ac_id );
		if ( ret )
		{
			strcpy( g_pub_tx.reply,"D103" );
			sprintf(acErrMsg,"ret[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		while (1)
		{
			memset( &sDd_mst_hst, 0x0, sizeof(sDd_mst_hst) );
			ret = Dd_mst_hst_Fet_Sel( &sDd_mst_hst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				Dd_mst_hst_Clo_Sel( );
				goto ErrExit;
			}

			pub_base_strpack( sDd_mst_hst.add_ind );
			if ( sDd_mst_hst.add_ind[0] == '1' )	/*--����������Ϊ���--*/
			{
				dTram = dTram + sDd_mst_hst.tx_amt;
			}
			else
			{
				dTram = dTram - sDd_mst_hst.tx_amt;
			}
		}

		Dd_mst_hst_Clo_Sel();

		ret = pub_base_CompDblVal( dTram, 0.00 );	/*�жϸñ���ˮ���ղ��������仯���*/
		if ( ret < 0 )
		{
			strcpy( cAdd_ind, "0" );
			dTram = 0 - dTram;
		}
		else
		{
			strcpy( cAdd_ind, "1" );
		}

		if ( cAdd_ind[0] == '1' )	/*--����������Ϊ�����Ҫ�ж��޸ĺ�������--*/
		{
			ret = Dd_mst_hst_Dec_Sel( g_pub_tx.reply , "tx_date=%d and trace_no>%d and ac_id=%d",
				ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
			if ( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}

			while (1)
			{
				memset( &sDd_mst_hst, 0x0, sizeof(sDd_mst_hst) );
				ret = Dd_mst_hst_Fet_Sel( &sDd_mst_hst, g_pub_tx.reply );
				if ( ret==100 )
				{
					break;
				}
				else if ( ret )
				{
					strcpy( g_pub_tx.reply,"D103" );
					sprintf(acErrMsg,"ret[%d]",ret);
					WRITEMSG
					Dd_mst_hst_Clo_Sel( );
					goto ErrExit;
				}

				/*-----------����---����������С����������ɾ������--------*/
				dAcbl = 0.00;
				dAcbl = sDd_mst_hst.bal - sDd_mst_hst.tx_amt;
				ret = pub_base_CompDblVal( dAcbl, 0.00 );
				if ( ret < 0 )
				{
					Dd_mst_hst_Clo_Sel( );
					return 100;	/*��ϸ����ֻ�ܲ���һ��*/
				}
			}

			Dd_mst_hst_Clo_Sel( );
		}

		if ( cAdd_ind[0] == '0' )	/*---���������״�����ǽ�����---*/
		{
			ret=sql_execute( "update dd_mst_hst set bal = bal + %lf \
				where tx_date=%d and trace_no>%d and ac_id=%d", dTram, \
				ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
			if( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( cAdd_ind[0] == '1' )	/*---���������״�����ǽ������---*/
		{
			ret=sql_execute( "update dd_mst_hst set bal = bal - %lf \
				where tx_date=%d and trace_no>%d and ac_id=%d", dTram, \
				ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
			if( ret )
			{
				strcpy( g_pub_tx.reply,"D103" );
				sprintf(acErrMsg,"ret[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	/*-------add by xxx end----at 20051107---------------------------------*/

	ret=sql_execute( "delete from dd_mst_hst where tx_date=%d and trace_no=%d and ac_id=%d",
		ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
	if( ret )
	{
		strcpy( g_pub_tx.reply,"D103" );
		sprintf(acErrMsg,"ret[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
vtcp_log("HST CNT [%d]",g_dd_mst.hst_cnt);
	g_dd_mst.hst_cnt--;

	return 0;
ErrExit:
	return 1;
}

int Rol_td_mst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret;
    long l_date;
    long num;

	/*** ���ö��ڲ�Ʒ���������麯�� ***/

	strcpy( g_pub_tx.add_ind,ptrace_log->add_ind );
	g_pub_tx.tx_amt1=-ptrace_log->amt;

	/*** �������ļ���� ***/
	ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

    /* �����ڲ�Ʒ�ṹ��ֵ */
    ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm," prdt_no='%s' ",
          g_td_mst.prdt_no);
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
    /******��Ȫ**/
    strcpy( g_pub_tx.opn_br_no,g_td_mst.opn_br_no );
	/****end***/
    strcpy(g_pub_tx.name,g_td_mst.name);
	g_pub_tx.cif_no=g_td_mst.cif_no;

    /*ȡ�������Э������*/
    if ( g_pub_tx.add_ind[0]=='0' )
	{
		/*** ���ü�����Ϣ�����ĺ��� **** ������ȡ */
    	if ( g_td_parm.td_type[0] == '2' )
    	{
			if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
				g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
				g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
				g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
    	}

        if( g_td_parm.td_type[0]=='3' && g_pub_tx.svc_ind==1001 )
		{
			;
		}
		else
		{
		    g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
		    g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;
	    	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	    	{
	        	g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
	    	}
        }

        switch(g_pub_tx.svc_ind)
        {
            case 1012: 	/*ȡ��*/
            case 1011: 	/*ȡ��*/
            case 1001: 	/*ȡ��*/
				if ( g_pub_tx.svc_ind!=1012 )
				{
                /* ����Ǽ���Ƿ������� */
                if (g_td_parm.td_type[0] == '0' )
                /* ��ͨ������ȡ */
                {
                    g_td_mst.tx_cnt = g_td_mst.tx_cnt - 1; /* ������� */

                    if (  g_td_mst.tx_cnt < 0 )
                    {
						g_td_mst.tx_cnt=0;
                    }
                }

                if (g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0] == '2')
                /* ������ȡ ����ȡϢ */
                {
					g_td_mst.tx_cnt--;
                    /**********/
                }
				}

                /* ֪ͨȡ��ļ�� */

                /* ������Ϣ ** ������ȡ �汾ȡϢ ���ü�����Ϣ */
                break;
		 	case 1003:					/*����*/
                /* ����Ƿ���ǰ֧ȡ */
                /* ����������������� */

                /* ������Ϣ */
                sprintf(acErrMsg,"ȡ����ϢΪ:,[%lf]",g_pub_intst.factval);
                WRITEMSG

				if( g_td_mst.ac_sts[0]!='*' )
				{
					sprintf(acErrMsg,"�˻�״̬��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"O156");
					goto ErrExit;
				}

				strcpy(g_td_mst.ac_sts,"1");
                break;
            default:
				sprintf( acErrMsg,"�������ʹ���[%d]",g_pub_tx.svc_ind );
				WRITEMSG
				strcpy( g_pub_tx.reply,"D999" );
				goto ErrExit;
		}

        /* ����ʻ���� */
		if(  g_pub_tx.svc_ind!=1012 )
		{
        if ( pub_base_CompDblVal(g_td_mst.bal,0.00)<0 )
        {
            sprintf(acErrMsg,"ȡ�������ʻ����!!bal=[%lf] amt1=[%lf]",
                g_td_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A030");
            goto ErrExit;
        }
		}
	}
	else if( g_pub_tx.add_ind[0]=='1' )		/*���*/
	{
		/*** ���ü�����Ϣ�����ĺ��� **** �������� */
    	if ( g_td_parm.td_type[0] == '1' )
    	{
			if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
				g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
				g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
				g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
    	}

		g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
		g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;

		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
			g_td_mst.hst_cnt++;
		}

		g_td_mst.tx_cnt--;
		if( g_td_mst.tx_cnt<0 ) g_td_mst.tx_cnt=0;
		if( g_pub_tx.svc_ind==1000 )
		{
			if( g_td_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�˻�״̬��������!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P109");
				goto ErrExit;
			}

			strcpy(g_td_mst.ac_sts,"4");
		}
	}
	else
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	g_td_mst.lst_date=g_pub_tx.tx_date;

	if ( g_pub_tx.svc_ind!=1012 )
	{
    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_bal)<0
		&& g_td_mst.ac_sts[0]=='1' )
	{
		sprintf(acErrMsg,"������[%.2lf]��������[%.2lf]",
			g_td_mst.bal,g_td_parm.min_bal );
		WRITEMSG
		strcpy(g_pub_tx.reply,"A022" );
		goto ErrExit;
	}
	}
    if (g_pub_tx.hst_ind[0]== '1') /*�ռ�����ϸ*/
    {
		strcpy(g_pub_tx.name,g_td_mst.name);
		g_pub_tx.cif_no=g_td_mst.cif_no;
		if( 1==1 )
			ret=pub_ins_td_hst();
		else
		{
				ret=pub_del_td_hst( ptrace_log );
				if( ret==100 )
				{
					ret=pub_ins_td_hst( );
					ERR_DEAL
				}
				else
				{
				ERR_DEAL
				}
		}
		ERR_DEAL
    }
	ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	Td_mst_Clo_Upd();

	ret=pub_acct_ac_rec( g_td_mst.ac_id,g_td_mst.ac_seqn,
		g_pub_tx.add_ind,g_pub_tx.ct_ind,ptrace_log->tx_code,g_pub_tx.tx_amt1,"0000" );
	if( ret ) goto ErrExit;

OkExit:
	return 0;
ErrExit:
	return 1;
}
pub_del_td_hst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;

	ret=sql_count( "td_mst_hst" , "tx_date=%d and trace_no>%d and ac_id=%d",
		ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
	if( ret<0 )
	{
		goto ErrExit;
	}
	else if( ret>0 )
	{
		return 100;
	}

	ret=sql_execute( "delete from td_mst_hst where tx_date=%d and trace_no=%d and ac_id=%d",
		ptrace_log->tx_date,ptrace_log->trace_no,ptrace_log->ac_id );
	if( ret )
	{
		strcpy( g_pub_tx.reply,"D103" );
		sprintf(acErrMsg,"ret[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	g_td_mst.hst_cnt--;
	return 0;
ErrExit:
	return 1;
}

int Rol_ln_mst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret;
	int do_flag=0;
	char acm_type[2];
	double acm=0;

	struct	trace_log_c	s_trace_log;
	struct	ln_mst_c	ln_mst_acm;
	struct	ln_pay_pln_rol_c	ln_pay_pln_rol_ic;
	memcpy( &s_trace_log,ptrace_log,sizeof(s_trace_log) );
	memset( &ln_mst_acm,0x00,sizeof(struct	ln_mst_c) );
	memset( &ln_pay_pln_rol_ic,0x00,sizeof(struct ln_pay_pln_rol_c) );

	strcpy( g_pub_tx.cur_no,ptrace_log->cur_no );
	memset( &acm_type , 0x0 , sizeof( acm_type ) );

	sprintf(acErrMsg,"--------> cur_no = [%s]", ptrace_log->cur_no);
	WRITEMSG

	pub_base_strpack( ptrace_log->brf );

	g_pub_tx.tx_amt1=-ptrace_log->amt;

	/*********
	strcpy( g_pub_tx.cur_no,ptrace_log->cur_no );
	g_pub_tx.tx_amt1=-ptrace_log->amt;
	********/
	TRACE
	sprintf(acErrMsg,"Rol_ln_mst�з�����ͱ���[%.2lf][%s]",
					g_pub_tx.tx_amt1,g_pub_tx.cur_no );
	WRITEMSG


	/* ȡ������ϸ����Ϣ */
	ret = get_ln_mst_intst( &s_trace_log , acm_type ,&acm );
	if( ret ) ERR_DEAL

	TRACE
	sprintf(acErrMsg,"������ϸ�л�������[%s]����[%.2lf]",acm_type,acm );
	WRITEMSG
	/*Added by gujy ShengShui 200706614*/
        switch(acm_type[0])
    {
    case '2': /*����*/
        g_pub_tx.tx_amt2=g_pub_tx.tx_amt1;
        break;
    case '3': /*����*/
        g_pub_tx.tx_amt3=g_pub_tx.tx_amt1;
        break;
    case '4': /*����*/
        g_pub_tx.tx_amt4=g_pub_tx.tx_amt1;
        break;
    }

	/*** ��ȡ������Ϣ�����������Ϣ ***/
	ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
			g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	ERR_DEAL

	ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG
	TRACE

	ret = Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,
			" prdt_no='%s' ",g_ln_mst.prdt_no );
	ERR_DEAL
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG
	strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );
	strcpy( g_pub_tx.opn_br_no,g_ln_mst.opn_br_no );
	strcpy( g_pub_tx.name,g_ln_mst.name );
	g_pub_tx.cif_no=g_ln_mst.cif_no;
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG

	/**�˻�״̬���**/
	if( g_ln_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�����˺�������![%s]",g_ln_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P120");
		goto ErrExit;
	}
	TRACE

	if( g_ln_mst.ac_sts[0]=='1' )/* ���� */
	{
	TRACE
		strcpy(g_pub_tx.dc_code,g_ln_parm.dc_code);
	}
	if( g_ln_mst.ac_sts[0]=='2' )/* ����90�� */
	{
	TRACE
		strcpy(g_pub_tx.dc_code,g_ln_parm.over_dc_code);
	}
	if( g_ln_mst.ac_sts[0]=='3' )/* ���� */
	{
	TRACE
		strcpy(g_pub_tx.dc_code,g_ln_parm.sla_dc_code);
	}
	if( g_ln_mst.ac_sts[0]=='4' )/* ���� */
	{
	TRACE
		strcpy(g_pub_tx.dc_code,g_ln_parm.bad_dc_code);
	}
 	if( g_ln_mst.ac_sts[0]=='5')/*����90��*/
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG

	/*** ���ü�����Ϣ�����ĺ��� ***
		ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
			"2",g_ln_mst.bal,g_pub_tx.tx_amt1 ,
			&g_ln_mst.intst_acm ,"0",0,0 );
		ERR_DEAL
		ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
			"2",g_ln_mst.in_lo_intst,g_pub_tx.tx_amt2,
			&g_ln_mst.in_lo_acm ,"0",0,0 );
		ERR_DEAL
		ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
			"2" , g_ln_mst.out_lo_intst , g_pub_tx.tx_amt3,
			&g_ln_mst.out_lo_acm ,"0",0,0 );
		ERR_DEAL
		ret = pub_base_CalAcm( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
			"2" , g_ln_mst.cmpd_lo_intst , g_pub_tx.tx_amt4,
			&g_ln_mst.cmpd_lo_acm ,"0",0,0 );
		ERR_DEAL
		g_ln_mst.lst_date=g_pub_tx.tx_date;
	sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	WRITEMSG
	**********/

	/**����ǻ�����Ϣ�����������ע���ֶλ����ɸ������Լ���**/
	if( g_pub_tx.svc_ind==3011 || g_pub_tx.svc_ind==3012 )
	{
		/* �ع����ļ��л��� */
		TRACE
		ret = pub_ln_mst_acm_Back( &s_trace_log , &ln_mst_acm );
		if( ret )	goto ErrExit;
		sprintf(acErrMsg,"ǰintst_acm[%.2lf]",
					g_ln_mst.intst_acm );
		g_ln_mst.intst_acm=ln_mst_acm.intst_acm;
		g_ln_mst.ic_date=ln_mst_acm.ic_date;

		sprintf(acErrMsg,"��intst_acm[%.2lf]",
					g_ln_mst.intst_acm );

		/* �ع��ֶλ��������� */
		ret = pub_ln_sect_acm_Back( &s_trace_log );
		if( ret )	goto ErrExit;
	}
	if( g_pub_tx.svc_ind==3021 || g_pub_tx.svc_ind==3051 )
	{
		/* �ع����ļ��л��� */
		TRACE
		ret = pub_ln_mst_acm_Back( &s_trace_log , &ln_mst_acm );
		if( ret )	goto ErrExit;
		sprintf(acErrMsg,"ǰin_lo_acm[%.2lf]",
					g_ln_mst.in_lo_acm );
		g_ln_mst.in_lo_acm=ln_mst_acm.in_lo_acm;
		g_ln_mst.ic_date=ln_mst_acm.ic_date;

		/* �ع��ֶλ��������� */
		ret = pub_ln_sect_acm_Back( &s_trace_log );
		if( ret )	goto ErrExit;
		sprintf(acErrMsg,"��in_lo_acm[%.2lf]",
					g_ln_mst.in_lo_acm );
	}
	if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 )
	{
		/* �ع����ļ��л��� */
		ret = pub_ln_mst_acm_Back( &s_trace_log , &ln_mst_acm );
		if( ret )	goto ErrExit;
		sprintf(acErrMsg,"ǰout_lo_acm[%.2lf]",
					g_ln_mst.out_lo_acm );
		g_ln_mst.out_lo_acm=ln_mst_acm.out_lo_acm;
		g_ln_mst.ic_date=ln_mst_acm.ic_date;
		sprintf(acErrMsg,"��out_lo_acm[%.2lf]",
					g_ln_mst.out_lo_acm );
		/* �ع��ֶλ��������� */
		ret = pub_ln_sect_acm_Back( &s_trace_log );
		if( ret )	goto ErrExit;

	}
	if( g_pub_tx.svc_ind==3041 || g_pub_tx.svc_ind==3051 )
	{
		/* �ع����ļ��л��� */
		ret = pub_ln_mst_acm_Back( &s_trace_log , &ln_mst_acm );
		if( ret )	goto ErrExit;
		sprintf(acErrMsg,"ǰcmpd_lo_acm[%.2lf]",
					g_ln_mst.cmpd_lo_acm );
		g_ln_mst.cmpd_lo_acm=ln_mst_acm.cmpd_lo_acm;
		g_ln_mst.ic_date=ln_mst_acm.ic_date;
		sprintf(acErrMsg,"��cmpd_lo_acm[%.2lf]",
					g_ln_mst.cmpd_lo_acm );
		/* �ع��ֶλ��������� */
		ret = pub_ln_sect_acm_Back( &s_trace_log );
		if( ret )	goto ErrExit;
	}

	do_flag=0;

vtcp_log("%d@%s,   g_pub_tx.add_ind(%c)\n", __LINE__, __FILE__, g_pub_tx.add_ind[0]);
	TRACE
	if( g_pub_tx.add_ind[0]=='1' ) /*����*/
	{
		if( g_pub_tx.svc_ind==3010 ) /*��������ӣ����ſ�*/
		{
			/* ������Ǽǲ��м�¼ ������¼���Ƿ��ѻ���,��ɾ���ü�¼ */
			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"1" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			g_ln_mst.bal+=g_pub_tx.tx_amt1;
			g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
			strcpy( g_pub_tx.brf , "����ſ��" );
			ret=pub_ins_ln_hst( 0 ,1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3091 ) /*���Ǵ����*/
		{
	TRACE
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG

			if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
				g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
			{
				sprintf(acErrMsg,
					"���������޶�ѷ�[%.2lf]��[%.2lf]>�޶�[%.2lf]",
					g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
				WRITEMSG
				strcpy( g_pub_tx.reply,"P122" );
				goto ErrExit;
			}
			g_ln_mst.bal+=g_pub_tx.tx_amt1;
			g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
			ret=pub_ins_ln_hst( 0 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3092 ) /*���Ǵ����ѺƷ���*/
		{
			TRACE
			strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
			g_ln_mst.gage_amt+=g_pub_tx.tx_amt1;
			ret=pub_ins_ln_hst( 8 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3001 ) /*�����*/
		{
			TRACE
			strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
			if( pub_base_CompDblVal( g_ln_mst.ttl_prvd_amt , 0.00 ) )
	 		{
	 			sprintf( acErrMsg,"�����ѷſ�,���ȳ����ſ��!");
	 			WRITEMSG
	 			strcpy( g_pub_tx.reply ,"L173" );
				goto ErrExit;
			}
			strcpy( g_ln_mst.ac_sts , "9" );
			strcpy( g_pub_tx.brf , "���������" );
			ret=pub_ins_ln_hst( 0 , 1 );
			ERR_DEAL

			/* �޸Ĵ��������Ǽǲ���״̬ */
			/**--- xxx ---- @20050818@ ----- ���ֿ������ǼǸñ� ----**/
			if( strcmp(ptrace_log->tx_code,"4913") && strcmp(ptrace_log->tx_code,"4922")
				&& strcmp(ptrace_log->tx_code,"4924") )
			{
				ret=sql_execute("update ln_auth set sts='0' where pact_no='%s'",g_ln_mst.pact_no );
				ERR_DEAL

	 			sprintf( acErrMsg,"�����ݺ�[%s]",g_ln_mst.pact_no);
	 			WRITEMSG
			}
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3011 || g_pub_tx.svc_ind==3012 ) /*����������������Ϣ����*/
		{
			strcpy( g_pub_tx.brf,"���������Ϣ����" );
			ret=pub_ins_ln_hst( 11 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*���������Ϣ���ӣ�����Ϣ*/
		if( g_pub_tx.svc_ind==3020 || g_pub_tx.svc_ind==3050 )
		{
			g_ln_mst.in_lo_intst+=g_pub_tx.tx_amt2;
			ret=pub_ins_ln_hst( 1 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*������ڻ�����������Ϣ*/
		if( g_pub_tx.svc_ind==3021 || g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"���ڻ�����Ϣ����brf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "���ڻ�����Ϣ" )==0 )
			{
				g_pub_tx.tx_amt2 = g_pub_tx.tx_amt1;
				strcpy ( g_pub_tx.brf,"���ڻ�����Ϣ����" );
				ret=pub_ins_ln_hst( 12 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/*���������Ϣ���ӣ�����Ϣ*/
		if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 )
		{
			g_ln_mst.out_lo_intst+=g_pub_tx.tx_amt3;
			ret=pub_ins_ln_hst( 2 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*������������������Ϣ*/
		if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"���������Ϣ����brf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "���������Ϣ" )==0 )
			{
				g_pub_tx.tx_amt3 = g_pub_tx.tx_amt1;
				strcpy ( g_pub_tx.brf,"���������Ϣ����" );
				ret=pub_ins_ln_hst( 13 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/*��������ӣ�����Ϣ*/
		if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050 )
		{
			g_ln_mst.cmpd_lo_intst+=g_pub_tx.tx_amt4;
			ret=pub_ins_ln_hst( 3 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*�����������������Ϣ*/
		if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"����������Ϣ����brf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "����������Ϣ" )==0 )
			{
				g_pub_tx.tx_amt4 = g_pub_tx.tx_amt1;
				strcpy ( g_pub_tx.brf,"����������Ϣ����" );
				ret=pub_ins_ln_hst( 14 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/* �����弶�϶��ͽ�ת */
		if( g_pub_tx.svc_ind==3060 )
		{
			 ret=pub_ins_ln_hst( 0 , 1 );
			 ERR_DEAL
		  	 do_flag=1;
		}
		/* ���ɰ��һ���ƻ� */
		if( g_pub_tx.svc_ind==3062 )
		{
			 ret=pub_ins_ln_hst( 0 , 1 );
			 ERR_DEAL
		  	 do_flag=1;
		}
		if( g_pub_tx.svc_ind==3059 )
		{
			ret=pub_ins_ln_hst( 1 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			if( g_pub_tx.svc_ind==3010 )
			{
				g_ln_mst.hst_bal=g_ln_mst.hst_bal+g_pub_tx.tx_amt1;
			}
		}
	}
	else if( g_pub_tx.add_ind[0]=='0' ) /*����*/
	{
		if( g_pub_tx.svc_ind==3019 ) /*�������٣�������*/
		{
			if( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)>=0 )
			{
				g_ln_mst.bal-=g_pub_tx.tx_amt1;
			}else{
				g_pub_tx.tx_amt1=g_ln_mst.bal;
				g_ln_mst.bal=0.00;
			}

			ret=pub_ins_ln_hst( 0 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3092 ) /*���������ѺƷ���*/
		{
			strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
			g_ln_mst.gage_amt-=g_pub_tx.tx_amt1;
			ret=pub_ins_ln_hst( 8 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*���������Ϣ���٣�����ǷϢ*/
		if( g_pub_tx.svc_ind==3059 )
		{
			ret=pub_ins_ln_hst( 1 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3071 )
		{
			/*����Ƿ����һ�ʰ���ҵ��*/
		  	ret= check_ln_mst_no(ptrace_log);
			ERR_DEAL

			g_pub_tx.tx_amt2 = g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
			{
				g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
				if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0 )
				{
					sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
						g_pub_tx.tx_amt2,g_ln_mst.in_lo_intst );
					WRITEMSG
					strcpy( g_pub_tx.reply,"L039" );
					goto ErrExit;
				}

				sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG
				ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
				ERR_DEAL
				sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				/* �ع�����Ƿ��� */
				ret = Reback_ln_lo( ptrace_log->trace_no );
				ERR_DEAL

				/* �ع����һ���ƻ��� */
				ret = Reback_ln_pay_pln( ptrace_log->trace_no );
				ERR_DEAL

				strcpy ( g_pub_tx.brf , "�����ұ�����Ϣ����(��)����" );
				ret=pub_ins_ln_hst( 1 , 1 );
				ERR_DEAL
				do_flag=1;
			}
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3072 )
		{
			/*����Ƿ����һ�ʰ���ҵ��*/
		  	ret= check_ln_mst_no(ptrace_log);
		    ERR_DEAL

			g_pub_tx.tx_amt4 = g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
			{
				g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt4;
				if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 )
				{
					sprintf(acErrMsg,"��Ϣ������[%.2lf]>[%.2lf]",
						g_pub_tx.tx_amt2,g_ln_mst.out_lo_intst );
					WRITEMSG
					strcpy( g_pub_tx.reply,"L039" );
					goto ErrExit;
				}

				sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG
				ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
				ERR_DEAL
				sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				/* �ع�����Ƿ��� */
				ret = Reback_ln_lo( ptrace_log->trace_no );
				ERR_DEAL

				/* �ع����һ���ƻ��� */
				ret = Reback_ln_pay_pln( ptrace_log->trace_no );
				ERR_DEAL

				strcpy ( g_pub_tx.brf , "�����ұ�����Ϣ����(��)����" );
				ret=pub_ins_ln_hst( 3 , 1 );
				ERR_DEAL
				do_flag=1;
			}
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3068 ) /*���Ҵ����Ϣ*/
		{
			/*����Ƿ����һ�ʰ���ҵ��*/
			ret= check_ln_mst_no(ptrace_log);
			ERR_DEAL
			g_pub_tx.tx_amt2 = g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal(g_pub_tx.tx_amt2 , 0.00 ) )
			{
				sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
				ERR_DEAL
				sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				/* �ع�����Ƿ��� */
				ret = Reback_ln_lo( ptrace_log->trace_no );
				ERR_DEAL

				/* �ع����һ���ƻ��� */
				ret = Reback_ln_pay_pln( ptrace_log->trace_no );
				ERR_DEAL

				strcpy ( g_pub_tx.brf,"��������Ϣ����" );
				ret=pub_ins_ln_hst( 1 , 1 );
				ERR_DEAL
				do_flag=1;

			}

		}
		if( g_pub_tx.svc_ind==3069 ) /*���Ҵ����Ϣ*/
		{
			/*����Ƿ����һ�ʰ���ҵ��*/
			ret= check_ln_mst_no(ptrace_log);
			ERR_DEAL
			g_pub_tx.tx_amt4 = g_pub_tx.tx_amt1;
			if( pub_base_CompDblVal(g_pub_tx.tx_amt4 , 0.00 ) )
			{
				sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
				ERR_DEAL
				sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
				WRITEMSG

				/* �ع�����Ƿ��� */
				ret = Reback_ln_lo( ptrace_log->trace_no );
				ERR_DEAL

				/* �ع����һ���ƻ��� */
				ret = Reback_ln_pay_pln( ptrace_log->trace_no );
				ERR_DEAL

				strcpy ( g_pub_tx.brf,"�����ҷ�Ϣ����" );
				ret=pub_ins_ln_hst( 3 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}

		/****
		if( g_pub_tx.svc_ind==3069 ) *���Ҵ����Ϣ*
		{
			*����Ƿ����һ�ʰ���ҵ��*
			ret= check_ln_mst_no(ptrace_log);
			ERR_DEAL

			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG

			* �ع�����Ƿ��� *
			ret = Reback_ln_lo( ptrace_log->trace_no );
			ERR_DEAL

			* �ع����һ���ƻ��� *
			ret = Reback_ln_pay_pln( ptrace_log->trace_no );
			ERR_DEAL

			strcpy ( g_pub_tx.brf,"���һ�Ϣ����" );
			ret=pub_ins_ln_hst( 9 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		*********/
		if( g_pub_tx.svc_ind==3011 || g_pub_tx.svc_ind==3012 ) /*����������������Ϣ*/
		{
			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			strcpy ( g_pub_tx.brf,"�������������" );
			ret=pub_ins_ln_hst( 15 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3010 ) /*�������٣�������*/
		{
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
			WRITEMSG
			g_ln_mst.bal=g_ln_mst.bal-g_pub_tx.tx_amt1;

			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
			WRITEMSG

			/* �޸Ĵ���Ǽǲ� */
			ret = pub_ln_Udp_LnReg_Back ( ptrace_log->ac_id ,
						ptrace_log->ac_seqn , acm_type , ptrace_log->amt );
			ERR_DEAL

			strcpy( g_pub_tx.brf , "���������" );
			if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				/*����Ƿ����һ�ʰ���ҵ��*/
				ret= check_ln_mst_no(ptrace_log);
				ERR_DEAL

				/* �ع�����Ƿ��� */
				ret = Reback_ln_lo( ptrace_log->trace_no );
				ERR_DEAL

				/* �ع����һ���ƻ��� */
				ret = Reback_ln_pay_pln( ptrace_log->trace_no );
				ERR_DEAL

				strcpy( g_pub_tx.brf , "���һ�������" );
			}

			ret=pub_ins_ln_hst( 0 , 1 );
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.svc_ind==3018 ) /*����ƻ����*/
		{
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
			/*����Ƿ����һ�ʰ���ҵ��*/
			ret= check_ln_mst_no(ptrace_log);
			ERR_DEAL

			g_ln_mst.bal = g_ln_mst.bal - g_pub_tx.tx_amt1;
			g_ln_mst.amt_lmt = g_ln_mst.amt_lmt - g_pub_tx.tx_amt1;
			WRITEMSG
			/* �޸Ĵ���Ǽǲ� */
			ret= pub_ln_Udp_LnReg_Back(ptrace_log->ac_id, ptrace_log->ac_seqn,
										acm_type, ptrace_log->amt );
			ERR_DEAL

			/* �ع����һ���ƻ��� */
			ret = Reback_ln_pay_pln( ptrace_log->trace_no );
			ERR_DEAL

			/* �ع�ic_date */
			ret= Ln_pay_pln_rol_Sel(g_pub_tx.reply, &ln_pay_pln_rol_ic,
					"tx_date=%ld and trace_no=%ld", g_pub_tx.tx_date,
					ptrace_log->trace_no);
			ERR_DEAL
			g_ln_mst.ic_date= ln_pay_pln_rol_ic.ic_date_aj;

			strcpy( g_pub_tx.brf , "������ǰ��������" );
			ret=pub_ins_ln_hst( 0 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		/*���������Ϣ���٣�����ǷϢ*/
		if( g_pub_tx.svc_ind==3020 || g_pub_tx.svc_ind==3050 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"������ǷϢbrf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "������ǷϢ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt2 = g_pub_tx.tx_amt1;
				g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
				/* �޸Ĵ���Ǽǲ� */
				ret = pub_ln_Udp_LnReg_Back(ptrace_log->ac_id,
							ptrace_log->ac_seqn, acm_type, ptrace_log->amt);
				ERR_DEAL
				strcpy ( g_pub_tx.brf , "������ǷϢ����" );
				ret=pub_ins_ln_hst( 1 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/*���������Ϣ���٣�����ǷϢ*/
		if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"������ǷϢbrf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "������ǷϢ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt3 = g_pub_tx.tx_amt1;
				g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt3;
				/* �޸Ĵ���Ǽǲ� */
				ret = pub_ln_Udp_LnReg_Back ( ptrace_log->ac_id , ptrace_log->ac_seqn , acm_type , ptrace_log->amt );
				ERR_DEAL
				strcpy ( g_pub_tx.brf , "������ǷϢ����" );
				ret=pub_ins_ln_hst( 2 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/*��������٣�����ǷϢ*/
		if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"������ǷϢbrf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "������ǷϢ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt4 = g_pub_tx.tx_amt1;
				g_ln_mst.cmpd_lo_intst-=g_pub_tx.tx_amt4;
				/* �޸Ĵ���Ǽǲ� */
				ret = pub_ln_Udp_LnReg_Back ( ptrace_log->ac_id , ptrace_log->ac_seqn , acm_type , ptrace_log->amt );
				ERR_DEAL
				sprintf ( g_pub_tx.brf ,  "%s����", ptrace_log->brf );
				ret=pub_ins_ln_hst( 3 , 1 );
				ERR_DEAL
				do_flag=1;
			}
		}
		/*������ڻ�����������Ϣ*/
		if( g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"���ڻ���brf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "�����ڻ�����Ϣ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt2 = g_pub_tx.tx_amt1;
			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
				strcpy ( g_pub_tx.brf,"�����ڻ�������" );
				ret=pub_ins_ln_hst( 5 , 1 );
				ERR_DEAL
			}
			do_flag=1;
		}
			vtcp_log("%d@%s  ret=[%d] do_flag=[%d]\n", __LINE__, __FILE__, ret, do_flag);

		/*������������������Ϣ*/
		if( g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );

			sprintf(acErrMsg,"�������brf[%s]",ptrace_log->brf );
			WRITEMSG

			if( strcmp( ptrace_log->brf , "�����������Ϣ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt3 = g_pub_tx.tx_amt1;
			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
				strcpy ( g_pub_tx.brf,"�������������" );
				ret=pub_ins_ln_hst( 6 , 1 );
				ERR_DEAL
			}
			do_flag=1;
		}
			vtcp_log("%d@%s  ret=[%d] do_flag=[%d]\n", __LINE__, __FILE__, ret, do_flag);
		/*�����������������Ϣ*/
		if( g_pub_tx.svc_ind==3051 )
		{
			pub_base_strpack( ptrace_log->brf );
			sprintf(acErrMsg,"��������brf[%s]",ptrace_log->brf );
			WRITEMSG
			if( strcmp( ptrace_log->brf , "������������Ϣ" )==0 )
			{
				TRACE
				g_pub_tx.tx_amt4 = g_pub_tx.tx_amt1;
			sprintf(acErrMsg,"ǰ��ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
			ret = pub_ln_Del_LnReg( &s_trace_log ,"2" );
			ERR_DEAL
			sprintf(acErrMsg,"����ˮ��![%ld]",ptrace_log->trace_no);
			WRITEMSG
				strcpy ( g_pub_tx.brf,"��������������" );
				ret=pub_ins_ln_hst( 7 , 1 );
				ERR_DEAL
			}
			do_flag=1;
		}
			vtcp_log("%d@%s  ret=[%d] do_flag=[%d]\n", __LINE__, __FILE__, ret, do_flag);

	    /* �����弶�϶��ͽ�ת */
		if( g_pub_tx.svc_ind==3060 )
		{
			ret=pub_ins_ln_hst( 0 , 1 );
			ERR_DEAL
			do_flag=1;
		}
	    /* ��ѺƷע�� */
		if( g_pub_tx.svc_ind==3061 )
		{
			ret=pub_ins_ln_hst( 8 , 1 );
			ERR_DEAL
			do_flag=1;
		}
		if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
		{
			if( g_pub_tx.svc_ind==3019 || g_pub_tx.svc_ind==3010 ||g_pub_tx.svc_ind==3018)
			{
				g_ln_mst.hst_bal=g_ln_mst.hst_bal-g_pub_tx.tx_amt1;
			}
		}
			vtcp_log("%d@%s  ret=[%d] do_flag=[%d]\n", __LINE__, __FILE__, ret, do_flag);
	}
	else if( g_pub_tx.svc_ind==2 )
	{
		do_flag=1;
	}
	else
	{
		sprintf(acErrMsg,"������־����[%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	if( do_flag==0 )
	{
		sprintf(acErrMsg,"��־����[%s][%d]",g_pub_tx.add_ind,g_pub_tx.svc_ind);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S048" );
		goto ErrExit;
	}

	/**����������0�������**/
	if( g_pub_tx.svc_ind==2 )
	{
		if( pub_base_CompDblVal(g_ln_mst.bal,0.00)
			||  pub_base_CompDblVal(g_ln_mst.intst_acm,0.00)
			||  pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)
			||  pub_base_CompDblVal(g_ln_mst.in_lo_acm,0.00)
			||  pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)
			||  pub_base_CompDblVal(g_ln_mst.out_lo_acm,0.00)
			||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)
			||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_acm,0.00) )
		{
			sprintf(acErrMsg,"����δ����");
			WRITEMSG
			strcpy( g_pub_tx.reply,"L068" );
			goto ErrExit;
		}
	}
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
	Ln_mst_Clo_Upd();
			sprintf(acErrMsg,"������ʹ������[%.2lf][%.2lf]",
					g_pub_tx.tx_amt1,g_ln_mst.bal );
OkExit:
	return 0;
ErrExit:
	return 1;
}


int Rol_in_mst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	struct com_item_c v_com_item;
	int ret;
	/*** �����ڲ���Ʒ���������麯�� ***/

	strcpy( g_pub_tx.add_ind,ptrace_log->add_ind );
		sprintf(acErrMsg,"����ȡ��ʱ�Ĵ�ȡ��־g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
	g_pub_tx.tx_amt1=-ptrace_log->amt;
	/**--- xxx ----- @20050907@ ------**/
	g_pub_tx.svc_ind=ptrace_log->svc_ind;
	g_pub_tx.ac_id=ptrace_log->ac_id;
	g_pub_tx.ac_seqn=ptrace_log->ac_seqn;

	/*** �ڲ����ļ���� ***/
	ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
		g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	/**��ȡ��Ʒ����**/
	ret = In_parm_Sel( g_pub_tx.reply,&g_in_parm,
			" prdt_no='%s' ",g_in_mst.prdt_no );
	ERR_DEAL

	strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
	strcpy(g_pub_tx.dc_code,g_in_parm.dc_code); 			/*��ƴ���*/
	/****��Ȫ***********/
	strcpy( g_pub_tx.opn_br_no,g_in_mst.opn_br_no );
	/****end***/
	/**��ȡ��Ŀ����**/
	ret = Com_item_Sel( g_pub_tx.reply,&v_com_item,
			" acc_hrt='%s' ",g_in_parm.acc_hrt );
	ERR_DEAL

sprintf(acErrMsg,"yefx[%s][%s]",v_com_item.dc_ind,g_in_parm.acc_hrt);
WRITEMSG
	if( g_pub_tx.svc_ind==9002 ) /**����**/
	{
		if( g_in_mst.sts[0]!='*' )
		{
			sprintf(acErrMsg,"�˻�״̬��������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O156");
			goto ErrExit;
		}
		strcpy(g_in_mst.sts,"1");
	}

	if( g_in_mst.sts[0]!='1' && g_in_mst.sts[0]!='2' )
	{
		sprintf(acErrMsg,"�˺�״̬������״̬![%ld,%s]",g_in_mst.ac_id,g_in_mst.sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P109");
		goto ErrExit;
	}

	g_in_mst.lst_date=g_pub_tx.tx_date;

	strcpy( g_pub_tx.add_ind,ptrace_log->add_ind );

	/* add by xxx at 20030715  ������ת���ɽ�� */
	if (g_pub_tx.add_ind[0] == '0') /* �� */
	{
		if (v_com_item.dc_ind[0] == '2')
		{
			strcpy(g_pub_tx.add_ind, "0"); /* �� */
		}
		else
		{
			strcpy(g_pub_tx.add_ind, "1"); /* �� */
		}
	}
	else if (g_pub_tx.add_ind[0] == '1') /* �� */
	{
		if (v_com_item.dc_ind[0] == '2')
		{
			strcpy(g_pub_tx.add_ind, "1"); /* �� */
		}
		else
		{
			strcpy(g_pub_tx.add_ind, "0"); /* �� */
		}
	}
	else
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",
				g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}
	/* add by xxx end ***/

	sprintf(acErrMsg,"���������Ĵ�ȡ��־g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
	WRITEMSG

	if( g_pub_tx.add_ind[0]=='0' )	/*��*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='2' )
		{
			sprintf(acErrMsg,"��Ŀ����������");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A025");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='2' )
		{
			sprintf(acErrMsg,"��Ʒ����������");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A027");
			goto ErrExit;
		}

		if( v_com_item.dc_ind[0]=='2' )
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;
		}
		else
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}

			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
		}
	}
	else if ( g_pub_tx.add_ind[0]=='1' )	/*��*/
	{
		if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"���˻��Ѿ����򶳽�!sts[%s],add_ind[%s],dc[%s]",
				g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P125");
			goto ErrExit;
		}
		if( v_com_item.amt_dc_ind[0]=='1' )
		{
			sprintf(acErrMsg,"��Ŀ�������跽");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A024");
			goto ErrExit;
		}
		if( g_in_parm.amt_dc[0]=='1' )
		{
			sprintf(acErrMsg,"��Ʒ�������跽");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A026");
			goto ErrExit;
		}
		if( v_com_item.dc_ind[0]=='2' )
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
		}
		else
		{
			/*** ���ü�����Ϣ�������� ***/
			if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
				g_in_parm.cal_day_type,g_in_parm.acm_type,
				g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
				g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
			{
				sprintf(acErrMsg,"������Ϣ��������!");
				WRITEMSG
				goto ErrExit;
			}
			g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
			if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
				g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;
		}
	}
	else
	{
		sprintf(acErrMsg,"��ȡ��־����g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	if( pub_base_CompDblVal(g_in_mst.bal,0.00)<0
		&& g_in_mst.od_ind[0]=='N')
	{
		sprintf(acErrMsg,"���˻�������͸֧![%.2f][%.2f]",
			g_in_mst.bal,g_pub_tx.tx_amt1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P124");
		goto ErrExit;
	}
	if( g_in_mst.od_ind[0]=='Y'
		&& pub_base_CompDblVal(g_in_parm.od_amt,0.00)
		&& pub_base_CompDblVal(-g_in_mst.bal,g_in_parm.od_amt)>0 )
	{
		sprintf(acErrMsg,"͸֧����![%.2f][%.2f]",
			g_in_mst.bal,g_in_parm.od_amt );
		WRITEMSG
		strcpy(g_pub_tx.reply,"T004");
		goto ErrExit;
	}

	if( g_pub_tx.hst_ind[0]=='1' )		/*�ռ�����ϸ*/
	{
		ret=pub_ins_in_hst();
		ERR_DEAL
	}

	/**--- xxx ---- @20050913@ ---- �����ڲ��˿���������Ƿ�Ϊ�� ----*/
	if( !strncmp(ptrace_log->tx_code,"2501",4) && pub_base_CompDblVal(g_in_mst.bal,0.00) )
	{
		sprintf(acErrMsg,"bal[%.2lf] ",g_in_mst.bal);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D191" );
		goto ErrExit;
	}
	if( !strncmp(ptrace_log->tx_code,"2501",4) )
	{
		strcpy(g_in_mst.sts,"4");	/** �������� **/
	}

	ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	In_mst_Clo_Upd();

OkExit:
	return 0;
ErrExit:
	return 1;
}

int Rol_acchrt_mst( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;
	double amt;

	struct gl_mst_c g_gl_mst;
	struct com_item_c g_com_item;

	memset(&g_gl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&g_com_item,0x00,sizeof(struct com_item_c));

	strcpy( g_pub_tx.add_ind,ptrace_log->add_ind );
	g_pub_tx.tx_amt1=-ptrace_log->amt;

	/*����Ŀ����*/
	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"��ѯ������Ŀ���쳣[%s]",g_pub_tx.ac_no);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
pub_acct_Rol_chk( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	struct com_rollback_ctl_c vcom_rollback_ctl;
	int ret;

	ret=Com_rollback_ctl_Sel( g_pub_tx.reply,&vcom_rollback_ctl,
		"sub_tx_code='%s'",ptrace_log->sub_tx_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ܳ���[%s]",ptrace_log->sub_tx_code);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S053" );
		goto ErrExit;
	}
	ERR_DEAL

	if( vcom_rollback_ctl.reg_ind[0]=='#' )
	{
		sprintf(acErrMsg,"���ܳ���[%c]",vcom_rollback_ctl.reg_ind[0]);
		WRITEMSG
		strcpy( g_pub_tx.reply,"S053" );
		goto ErrExit;
	}
OkExit:
	return 0;
ErrExit:
	return 1;
}
/**�Ǽǲ��ĳ���**/
pub_acct_Rol_reg( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	struct com_rollback_ctl_c vcom_rollback_ctl;
	int ret=0;
	int i=0;
	int len=0;
	char c;

	sprintf(acErrMsg,"�Ǽǲ�[%s]",ptrace_log->sub_tx_code);
	WRITEMSG

	ret=Com_rollback_ctl_Sel( g_pub_tx.reply,&vcom_rollback_ctl,
		"sub_tx_code='%s'",ptrace_log->sub_tx_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ܳ���");
		WRITEMSG
		strcpy( g_pub_tx.reply,"S053" );
		goto ErrExit;
	}
	ERR_DEAL

	pub_base_strpack( vcom_rollback_ctl.reg_ind );
	len = strlen( vcom_rollback_ctl.reg_ind );
	sprintf(acErrMsg,"!!!com_rollback_ctl����[%d]",len);
	WRITEMSG
	for( i=0; i<len; i++ )
	{
		c=vcom_rollback_ctl.reg_ind[i];
		if( c=='0' ) continue;
		/* c = '1';	* ���� */
		sprintf(acErrMsg,"------------>�ǼǱ����[%d]",i+1);
		WRITEMSG
		ret=pub_acct_Rol_reg_sub( ptrace_log,c,i+1 );
		ERR_DEAL

		sprintf( acErrMsg, "------------>!!!�ǼǱ��������" );
		WRITEMSG
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}


