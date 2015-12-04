/*************************************************************
* �� �� ��: gB001.c
* �����������г���ծȯͶ�ʼ�����Ϣ��̯���ۼ����
*
* ��    ��: jane
* �������: 20030610
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

struct	mo_invest_c	sMo_invest;

gB001()
{
	int ret = 0;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	strcpy(g_pub_tx.cur_no,RMB);
	strcpy(g_pub_tx.tx_code,"B001");
	strcpy(g_pub_tx.sub_tx_code,"B001");
	strcpy(g_pub_tx.chk,"000");
	strcpy(g_pub_tx.auth,"000");
	g_pub_tx.trace_cnt=0;
	strcpy(g_pub_tx.ct_ind,"2");

	/**------- ҵ���� --------------**/
	ret=pub_eod_invest();
	if( ret ) 
	{
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int pub_eod_invest()
{
	int mouth=0,day=0;
	int ret;
	memset(&sMo_invest,0x00,sizeof(struct mo_invest_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	/*** ��ĩ21��ִ�� ***/
	mouth=g_pub_tx.tx_date%10000/100;
	day=g_pub_tx.tx_date/100;
	if( mouth!=3 && mouth!=6 && mouth!=9 && mouth!=12 )
	{
		sprintf( acErrMsg, "δ����ĩ����ִ��[%d]!!!",mouth );
		WRITEMSG
		return 0;
	}
	if( g_pub_tx.tx_date%100!=21 )
	{
		sprintf( acErrMsg, "δ�����ڲ���ִ��[%d]!!!",day );
		WRITEMSG
		return 0;
	}

	ret=Mo_invest_Dec_Upd(g_pub_tx.reply,"bkrl_code='%s' and sts='2' ",INVEST_LONG);
	if( ret )
	{
		sprintf( acErrMsg, "��Ͷ�ʵǼǱ�����[%d]!!!",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B036");
		goto ErrExit;
	}

	while(1)
	{
		ret=Mo_invest_Fet_Upd(&sMo_invest,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf( acErrMsg, "��Ͷ�ʵǼǱ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( sMo_invest.abate_ac_id!=0 && sMo_invest.overflow_ac_id!=0 )
		{
			sprintf( acErrMsg, "�ۼ��˺�����˺�ͬʱ����[%d][%d]!!!",sMo_invest.abate_ac_id,sMo_invest.overflow_ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s' ",INVEST_LONG);
		if( ret )
		{
			sprintf( acErrMsg, "ȡ��Ʒ��������[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"W035");
			goto ErrExit;
		}

		vtcp_log("sMo_invest.pre_intst=[%lf]",sMo_invest.pre_intst);
		ret=pub_eod_invest_intst(sMo_invest);
		if( ret )
		{
			goto ErrExit;
		}
			
		vtcp_log("sMo_invest.pre_intst=[%lf]",sMo_invest.pre_intst);
		ret=Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "��Ͷ�ʵǼǱ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
	}
	Mo_invest_Clo_Upd();

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_eod_invest_intst()
{
	double	intst_amt=0.00,tmp_amt=0.00;	/*** Ӧ������Ϣ ***/
	double	over_amt=0.00;					/*** ���̯����� ***/
	double	abate_amt=0.00;					/*** �ۼ�̯����� ***/
	int		ret;
	int		txcs=0;							/*** ̯������ ***/

	if( sMo_invest.intst_type[0]=='1' )		/*** ����ծȯ ***/
		intst_amt=0.00;
	else									/*** ���ڸ�Ϣծȯ ***/
	{
		ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%d",sMo_invest.ac_id);
		if( ret )
		{
			sprintf( acErrMsg, "ȡͶ���˺����ļ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
		if( ret==100 ) { intst_amt=0.00; goto OVER_AMT; }
		if( ret )
		{
			sprintf( acErrMsg, "ȡͶ���˺����ļ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		/**----------- ������Ϣ���� ------------**/
		if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
			g_in_parm.cal_day_type,g_in_parm.acm_type,
			g_in_mst.bal,0.00,&g_in_mst.intst_acm,
			g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
		{
			sprintf(acErrMsg,"������Ϣ��������!");
			WRITEMSG
			goto ErrExit;
		}

		tmp_amt = sMo_invest.rate * g_in_mst.intst_acm / L360 / L100;
		intst_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		g_in_mst.lst_date=g_pub_tx.tx_date;
		g_in_mst.ic_date=g_pub_tx.tx_date;
		g_in_mst.intst_acm=0.00;		/*** �������� ***/
		ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "ȡͶ���˺����ļ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		In_mst_Clo_Upd();

		/**------------ ������Ϣ���� -------------**/
		strcpy(g_pub_tx.ac_no,"14204");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=intst_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"�����г���ծȯͶ�ʼ�����Ϣ");
		strcpy(g_pub_tx.ct_ind,"2");		/*** ת�� ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** ����� ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*������δ��ȡ�ֻ�*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*���˶�ƾ֤�ţ��������ˮ����ϸ*/
		strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}

OVER_AMT:
	sMo_invest.pre_intst=intst_amt;
	if( sMo_invest.overflow_ac_id==0 )			/*** ����� ***/
		over_amt=0.00;
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_invest.overflow_ac_id);
		if( ret==100 ) { over_amt=0.00; goto ABATE_AMT; }
		if( ret )
		{
			sprintf( acErrMsg, "ȡͶ������˺����ļ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( pub_base_CompDblVal(g_in_mst.bal,0.00)==0 || sMo_invest.mtr_date<g_pub_tx.tx_date )
		{ over_amt=0.00; goto ABATE_AMT; }

		/**--------- ����̯������ ----------**/
		txcs=pub_CallNum(sMo_invest.mtr_date);
		if( txcs<0 )
		{
			sprintf( acErrMsg, "����̯����������[%d]!!!",txcs );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		tmp_amt = g_in_mst.bal / txcs;
		over_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		/**------------ ���̯������ -------------**/
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.overflow_ac_id;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=over_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"�����г���ծȯͶ�����̯��");
		strcpy(g_pub_tx.ct_ind,"2");		/*** ת�� ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** ����� ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*������δ��ȡ�ֻ�*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*���˶�ƾ֤�ţ��������ˮ����ϸ*/
		strcpy(g_pub_tx.add_ind,"1");/*������־*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}

ABATE_AMT:
	sMo_invest.overflow_amt=sMo_invest.overflow_amt-over_amt;
	if( sMo_invest.abate_ac_id==0 )			/*** ���ۼ� ***/
		abate_amt=0.00;
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_invest.abate_ac_id);
		if( ret==100 ) { abate_amt=0.00; goto AMT_514; }
		if( ret )
		{
			sprintf( acErrMsg, "ȡͶ���ۼ��˺����ļ�����[%d]!!!",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}

		if( pub_base_CompDblVal(g_in_mst.bal,0.00)==0 || sMo_invest.mtr_date<g_pub_tx.tx_date )
		{ abate_amt=0.00; goto AMT_514; }

		/**--------- ����̯������ ----------**/
		txcs=pub_CallNum(sMo_invest.mtr_date);
		if( txcs<0 )
		{
			sprintf( acErrMsg, "����̯����������[%d]!!!",txcs );
			WRITEMSG
			strcpy(g_pub_tx.reply,"B036");
			goto ErrExit;
		}
		tmp_amt = g_in_mst.bal / txcs;
		abate_amt = pub_base_PubDround_1(tmp_amt * L100) / L100;

		/**------------ ���̯������ -------------**/
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.abate_ac_id;
		g_pub_tx.ac_seqn=0;
		g_pub_tx.tx_amt1=abate_amt;
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
		strcpy( g_pub_tx.end_note_no,"");
		strcpy(g_pub_tx.brf,"�����г���ծȯͶ���ۼ�̯��");
		strcpy(g_pub_tx.ct_ind,"2");		/*** ת�� ***/
		strcpy(g_pub_tx.cur_no,"01");		/*** ����� ***/
		strcpy( g_pub_tx.ac_id_type,"9" );	/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );	/*������δ��ȡ�ֻ�*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*���˶�ƾ֤�ţ��������ˮ����ϸ*/
		strcpy(g_pub_tx.add_ind,"2");/*������־*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
	}
AMT_514:
	sMo_invest.abate_amt=sMo_invest.abate_amt-abate_amt;
	/**------------ �ۼ�̯������ -------------**/
	strcpy(g_pub_tx.ac_no,"51401");
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=intst_amt+abate_amt-over_amt;
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy( g_pub_tx.end_note_no,"");
	strcpy(g_pub_tx.brf,"�����г���ծȯͶ�ʼ���̯��");
	strcpy(g_pub_tx.ct_ind,"2");		/*** ת�� ***/
	strcpy(g_pub_tx.cur_no,"01");		/*** ����� ***/
	strcpy( g_pub_tx.ac_id_type,"9" );	/*�˻�����Ϊ�ڲ�*/
	strcpy( g_pub_tx.ac_get_ind,"00" );	/*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); 	/*���˶�ƾ֤�ţ��������ˮ����ϸ*/
	strcpy(g_pub_tx.add_ind,"1");/*������־*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("1214",g_pub_tx.cur_no);
	set_zd_data("1215",g_pub_tx.ct_ind);
	set_zd_double("1218",g_pub_tx.tx_amt1);
	ret=pubf_acct_proc("A017");
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( acErrMsg, "�г���ծȯ������Ϣ̯��ģ��ɹ��˳�!!!" );
	WRITEMSG
	return 0;
ErrExit:
	sprintf( acErrMsg, "�г���ծȯ������Ϣ̯��ģ������˳�!!!" );
	WRITEMSG
	return 1;
}

int pub_CallNum(long date1)
{
	int Mouths=0;
	int	cs;

	Mouths=pub_base_CalMouths(g_pub_tx.tx_date,date1);
	cs=Mouths/3;
	if( cs<0 )
		return -1;
	else if( cs==0 ) cs=1;
	else if( cs>0 && date1%100>g_pub_tx.tx_date%100 )
		cs++;
	
	return cs;
}
