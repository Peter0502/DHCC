/***************************************************************
* �� �� ��: spD004.c
* �����������Զ��ⶳ����
* ��    ��: jack
* ������ڣ�2003��3��10��
*
* �޸ļ�¼:
* ��    ��: 2003��6��27��
* �� �� ��: rob
* �޸�����:
*
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "com_eod_exec_c.h"
#include "com_sys_parm_c.h"

struct com_sys_parm_c com_sys_parm;
/*extern struct com_eod_exec_c grz_com_eod_exec;ȫ��,�������ӳ��򴫵ݿ�������*/

gD004()
{
	int ret=0;
	int  icnt=0;
	char min_ht[2];             /***����Ȩ�� martin***/
	double  sum_amt=0.00;						/***�����ۼ� martin***/
	static struct mo_hold_c	sMo_hold;

	memset(&sMo_hold,0x00,sizeof(struct mo_hold_c));

	ret=sql_begin(); /*������*/
	MYSQLERR

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	ret=pub_base_GetSysparm(&com_sys_parm);
	MYERR
	
	if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
		goto ErrExit;

	if( grz_com_eod_exec.do_type[0]=='0' ) /*����ǰ*/
		g_pub_tx.tx_date=com_sys_parm.sys_date;
	else
		g_pub_tx.tx_date=com_sys_parm.lst_date;

	/*---- ��ѯ�����¼ -----*/
	ret=Mo_hold_Dec_Upd(g_pub_tx.reply,
		"auto_unhold_ind='Y' and hold_sts='0' and plan_unhold_date=%ld",
		g_pub_tx.tx_date);
	MYSQLERR
	
	while(1)
	{
		ret = Mo_hold_Fet_Upd(&sMo_hold,g_pub_tx.reply);
		if(ret==100) break;
		MYSQLERR
		/*û�ҵ���; 2008-11-20 18:43
		ret=sql_execute("update dd_mst set hold_sts='%s',hold_amt=%.2f \
			g_dd_mst.hold_amt"); */

		strcpy(g_pub_tx.opn_br_no,sMo_hold.hold_br_no);

   		ret=pub_base_idseqn_file( sMo_hold.ac_id,sMo_hold.ac_seqn, 
				g_pub_tx.ac_type);
		if( ret )
		{
			sprintf(acErrMsg,"�����˺�ID�����ȡ���ļ�ʧ��!![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("ac_type=[%s]",g_pub_tx.ac_type);
		vtcp_log("ac_id=[%d] [%d]",sMo_hold.ac_id,sMo_hold.ac_seqn);
		

		/****** 
		if(sMo_hold.hold_typ=="1" || sMo_hold.hold_typ=="2" || sMo_hold.hold_typ=="4" )
		{
			sMo_hold.hold_typ[0]='0';			* ��Ϊ���� *
			sMo_hold.hold_sts[0]='1';			* ��Ϊ���״̬ *
		}
		else if(sMo_hold.hold_typ=="3")
		{
			sMo_hold.hold_typ[0]='3';          *��Ϊ���ֶ���*
		 	sMo_hold.hold_sts[0]='0';			* ��Ϊδ���״̬ *
		}
		*******/
		sMo_hold.hold_sts[0]='1';				/* ��Ϊ���״̬ */
	
		/** 
		strcpy(sMo_hold.hold_brf,"");
		strcpy(sMo_hold.unhold_brf,"");
		**/
		strcpy(sMo_hold.unhold_brf,"�����Զ��ⶳ");
		strcpy(sMo_hold.unhold_br_no,g_pub_tx.tx_br_no);
		strcpy(sMo_hold.unhold_tel,g_pub_tx.tel);
		sMo_hold.wrk_date=g_pub_tx.tx_date;
		sMo_hold.trace_no=g_pub_tx.trace_no;
		sMo_hold.fct_unhold_date=g_pub_tx.tx_date;

		vtcp_log("ac_type=[%s]",g_pub_tx.ac_type);
		
		ret = Mo_hold_Upd_Upd(sMo_hold,g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"ִ��Mo_hold_Upd_Upd��[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*��ѯ�˻��Ƿ���ڶ����¼2008-11-20 18:40*/
		icnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'", \
						sMo_hold.ac_id,sMo_hold.ac_seqn);
		if( icnt<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'",sMo_hold.ac_id,sMo_hold.ac_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
	  ret=sql_sum_double("mo_hold","plan_hold_amt",&sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'",sMo_hold.ac_id,sMo_hold.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		/*��ѯ�˻��Ƿ���ڶ����¼2008-11-20 18:40*/
		switch( g_pub_tx.ac_type[0] )
		{
			case '1':
/*JYW				Dd_mst_Dec_Upd( g_pub_tx.reply , fmtstr , va_alist);*/

			vtcp_log("�����ʺ��Զ��ⶳ");
			/* del by martin 2008-11-20 18:42
			if(g_dd_mst.hold_sts[0]!='0' && sMo_hold.hold_typ[0]=='1')
			{
				g_dd_mst.hold_sts[0]='0';        *��ȫ�ⶳʱ��Ϊ����*
				g_dd_mst.hold_amt=0.00;
			}
			else if(g_dd_mst.hold_sts[0]!='0'&& sMo_hold.hold_typ[0]=='2')
			{
				g_dd_mst.hold_sts[0]='0';
				g_dd_mst.hold_amt=0.00;
			}
			else if(g_dd_mst.hold_sts[0]!='0' && sMo_hold.hold_typ[0]=='4')
			{
				g_dd_mst.hold_sts[0]='0';
			}
			else if(g_dd_mst.hold_sts[0]!='0'&& sMo_hold.hold_typ[0]=='3')
			{
				if( pub_base_CompDblVal(g_dd_mst.hold_amt,sMo_hold.plan_hold_amt)<=0 )
				{
					g_dd_mst.hold_sts[0]='0';
					g_dd_mst.hold_amt=0.00;
				}
				else
				{
					g_dd_mst.hold_sts[0]='3';		*���ֽⶳʱ��Ϊ���ֽⶳ*
					g_dd_mst.hold_amt=g_dd_mst.hold_amt-sMo_hold.plan_hold_amt;
				}
			}
			else
			{	
				sprintf(acErrMsg,"���˻��Ѵ�������״̬!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O018");
				goto ErrExit;
			}
			2008-11-20 18:42 */
				
			if( icnt==0 )							/*** �޶����¼ ***/
					{
					  g_dd_mst.hold_sts[0]='0'; 
					  g_dd_mst.hold_amt=0.00; 
				  }
				else 
				{
					strcpy(g_dd_mst.hold_sts,min_ht);
					g_dd_mst.hold_amt=sum_amt; 
				}

			vtcp_log("g_dd_mst.hold_amt=[%lf]",g_dd_mst.hold_amt);
			ret=sql_execute("update dd_mst set hold_sts='%s',hold_amt=%.2f \
				where ac_id=%ld and ac_seqn=%d", g_dd_mst.hold_sts, \
				g_dd_mst.hold_amt,g_dd_mst.ac_id,g_dd_mst.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"���»������ļ�ʧ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O012");
				goto ErrExit;
			}
			break;
		case '2':
			
			/* del by  martin 2008-11-20 18:44
	        if(g_td_mst.hold_sts[0]!='0' && sMo_hold.hold_typ[0]=='1')
			{
				 g_td_mst.hold_sts[0]='0';           *��ȫ�ⶳʱ��Ϊ����*
				 g_td_mst.hold_amt=0.00;
			}
			else if(g_td_mst.hold_sts[0]!='0' && sMo_hold.hold_typ[0]=='2')
			{
				g_td_mst.hold_sts[0]='0';
				 g_td_mst.hold_amt=0.00;
			}
			else if(g_td_mst.hold_sts[0]!='0'&& sMo_hold.hold_typ[0]=='4')
			{
				g_td_mst.hold_sts[0]='0';
				 g_td_mst.hold_amt=0.00;
			}
			else if(g_td_mst.hold_sts[0]!='0'&& sMo_hold.hold_typ[0]=='3')

			{	
				if( pub_base_CompDblVal(g_td_mst.hold_amt,sMo_hold.plan_hold_amt)<=0 )
				{
					g_td_mst.hold_sts[0]='0';
					g_td_mst.hold_amt=0.00;
				}
				else
				{
					g_td_mst.hold_sts[0]='3';
					g_td_mst.hold_amt=g_td_mst.hold_amt-sMo_hold.plan_hold_amt;
				}
			}
			else
			{
				sprintf(acErrMsg,"���˻��Ѵ�������״̬!!");
				WRITEMSG
				strcpy (g_pub_tx.reply,"O018");
				goto ErrExit;
			}
			2008-11-20 18:44*/
			
			if( icnt==0 )							/*** �޶����¼ ***/
				 {
					g_td_mst.hold_sts[0]='0'; 
					g_td_mst.hold_amt=0.00; 
				 }
				else 
				{
					strcpy(g_td_mst.hold_sts,min_ht);
					g_td_mst.hold_amt=sum_amt; 
				}

			ret=sql_execute("update td_mst set hold_sts='%s',hold_amt=%.2f where ac_id=%ld and ac_seqn=%d",g_td_mst.hold_sts,g_td_mst.hold_amt,g_td_mst.ac_id,g_td_mst.ac_seqn);
			if(ret)
			{
				sprintf(acErrMsg,"���¶������ļ�ʧ��!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"O013");
				goto ErrExit;
			}
			break;
		default:
			sprintf(acErrMsg,"���ʻ������ڽⶳ��Χ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O015");
			goto ErrExit;
			break;
		}

		/* ���¶���Ǽǲ�*
		ret=sql_execute("update mo_hold set hold_typ='%s' hold_sts='%s' where \
					ac_id=ld and ac_seqn=%d and hold_seqn=%d",   \
					sMo_hold.hold_typ,	sMo_hold.hold_sts,g_pub_tx.ac_id \
					,g_pub_tx.ac_seqn,sMo_hold.hold_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"���¶���Ǽǲ�!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		****************/

	}
	Mo_hold_Clo_Upd();

	/** �Ǽǽ�����ˮ **/
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Զ��ⶳ���׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�Զ��ⶳ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
