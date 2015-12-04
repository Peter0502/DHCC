/*************************************************************
* �� �� ��: gD085.c
* �������������գ������ƻ���ÿ�ռ��
*
* ��    ��: lance
* �������: 2003��3��31��
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
#include "com_holiday_c.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"

gD085()
{
	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0,pay_flag=0;
	int    ac_seqn=0,parm_seqn=0,t_cnt=0;

	double amt_reg=0.00,amt=0.00,pun_intst=0.00,t_intst=0.00,t_pun_intst=0.00;
	char ln_ac_no[20],dd_ac_no[20],flag[2],parm_code[6];  
	long day_tmp=0,be_date=0;
	double now_ln_bal=0.00;
	double lo_tol_amt=0.00;
	double lo_pay_amt=0.00;
	long ttl_cnt=0;
	long curr_cnt=0;
	struct S_ln_intst vln_intst[LN_INTST_TYPS];
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;
	struct ln_lo_c ln_lo; 
	struct ln_lo_c ln_lo_tmp;       
	struct ln_pay_pln_c ln_pay_pln; 
	struct ln_pay_pln_c t_ln_pay_pln;   
	struct ln_pay_pln_c ln_pay_pln_curr;  
	struct ln_pay_pln_c ln_pay_pln_next;   
	struct ln_pay_pln_c ln_pay_pln_temp;   
	int first=0;

	memset( vln_intst,0,sizeof(vln_intst) );    
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
	memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 
	memset(&ln_lo_tmp, 0x00, sizeof(struct ln_lo_c));    
	memset(&ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&t_ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_curr, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_next, 0x00, sizeof(struct ln_pay_pln_c)); 
	memset(&ln_pay_pln_temp, 0x00, sizeof(struct ln_pay_pln_c)); 

	ret=sql_begin(); /*������*/
	if (ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
			goto ErrExit;
	}
	TRACE

	/* ��ʼ��ȫ�ֱ��� */
		pub_base_sysinit();

	show_log( " BEGIN [ pay_ind='0' and beg_date<=%ld and end_date >=%ld ]",
		g_pub_tx.tx_date,g_pub_tx.tx_date);

	/* ����ǰ����ƻ�������ʱ���� */
	ret=sql_execute("delete from ln_pay_pln_tmp");
	if ( ret) goto ErrExit;

	ret=sql_execute("insert into ln_pay_pln_tmp select * from ln_pay_pln");
	if ( ret) goto ErrExit;

	/* �������ƻ���(���ڻ�����������״̬Ϊδ����) */    
	ret = Ln_pay_pln_Dec_Upd1(g_pub_tx.reply,
		" pay_ind='0' and beg_date<=%ld and end_date >=%ld ",
	/*rem by LiuHuafeng replace by next line 20070120
		" pay_ind='0' and beg_date<=%ld and end_date >=%ld and ac_id in (select ac_id from ln_mst where ac_sts<>'*') ",
	****************************************/
		g_pub_tx.tx_date,g_pub_tx.tx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	TRACE

	while (1)
	{
		/*���ݳ�ʼ��*/
		ret=0;bj_flag=0;chg_flag=0;i=0;dc_flag=0;pay_flag=0;
		ac_seqn=0;parm_seqn=0;t_cnt=0;

		amt_reg=0.00;amt=0.00;pun_intst=0.00;t_intst=0.00;t_pun_intst=0.00;

		now_ln_bal=0.00;
		lo_tol_amt=0.00;
		lo_pay_amt=0.00;
		ttl_cnt=0;
		curr_cnt=0;
		memset( vln_intst,0,sizeof(vln_intst) );    
		strcpy( g_pub_tx.tx_code, "G085");

		ret = Ln_pay_pln_Fet_Upd1(&ln_pay_pln, g_pub_tx.reply);
		if (ret && ret!=100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
				goto ErrExit;
			}else if (ret==100)
			{
				vtcp_log("����Ӧ��¼[%d]ret=[%d]",g_pub_tx.tx_date,ret);
				break;
			}   
			first++;
			show_log( "gD085[%d]-[%d]",first,ln_pay_pln.ac_id );

			/* ȡ��ˮ�� */
			if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
			{
				sprintf(acErrMsg,"Get trace_no error [%d]",g_pub_tx.trace_no);
				WRITEMSG
					goto ErrExit;
			}
			set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
			set_zd_long( DC_TRACE_CNT,1 );

			/* ȡ������Ϣ�˺� */
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
			if (ret)
			{
				sprintf(acErrMsg,"��ȡ������ļ�ʧ��[%d-%d]",
					ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
				WRITEMSG
					goto ErrExit;   
			}
			ret= Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'",
				g_ln_mst.prdt_no);
			if ( ret )
			{
				sprintf(acErrMsg,"��ȡ�����������Ϣʧ��[%s]",g_ln_mst.prdt_no);
				WRITEMSG
					goto ErrExit;   
			}  
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_id=%ld and ac_seqn=%d",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
			if (ret)
			{
				sprintf(acErrMsg,"��ȡ�����˺Ŷ��ձ�ʧ��[%d-%d]",
					ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
				WRITEMSG
					goto ErrExit;
			}
			strcpy( ln_ac_no,g_mdm_ac_rel.ac_no );

			sprintf(acErrMsg,"SS [%ld][%d][%s]",
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn,ln_ac_no);
			WRITEMSG

			/*�������ݲ�׼ȷ,���¼��㻹��ƻ�*/
			/*ȡǷ�������*/
				ret= sql_sum_double("ln_lo", "lo_amt", &lo_tol_amt, 
				"ac_id=%ld and ac_seqn=%d and pay_type='0'",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if (ret)	goto ErrExit;

			ret= sql_sum_double ("ln_lo", "pay_lo_amt", &lo_pay_amt,
				"ac_id=%ld and ac_seqn=%d and pay_type='0'",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if (ret)	goto ErrExit;	

			now_ln_bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);

			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*������*/

			curr_cnt= ln_pay_pln.curr_cnt;		/*��ǰ����*/

			ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
				now_ln_bal, &ln_pay_pln_curr );
			if (ret)
			{
				sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
				WRITEMSG
					goto ErrExit;
			}
			sprintf(acErrMsg,"�������ɵĻ���ƻ�!��ǰ[%d]��",ln_pay_pln.curr_cnt);
			WRITEMSG

				sprintf(acErrMsg,"�˺����[%d]",ln_pay_pln_curr.ac_seqn);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln_curr.curr_cnt);
			WRITEMSG
				sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln_curr.wrk_date);
			WRITEMSG
				sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln_curr.pay_ind);
			WRITEMSG

				sprintf(acErrMsg,"������[%d]",ln_pay_pln_curr.ttl_cnt);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln_curr.curr_amt);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln_curr.curr_intst);
			WRITEMSG
				sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln_curr.beg_date);
			WRITEMSG
				sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln_curr.end_date);
			WRITEMSG

				sprintf(acErrMsg,"ԭ����ƻ�!!!!!!!!!!!!!!!!!!!");
			sprintf(acErrMsg,"�˺����[%d]",ln_pay_pln.ac_seqn);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln.curr_cnt);
			WRITEMSG
				sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln.wrk_date);
			WRITEMSG
				sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln.pay_ind);
			WRITEMSG

				sprintf(acErrMsg,"������[%d]",ln_pay_pln.ttl_cnt);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln.curr_amt);
			WRITEMSG
				sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln.curr_intst);
			WRITEMSG
				sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln.beg_date);
			WRITEMSG
				sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln.end_date);
			WRITEMSG

      Ln_pay_pln_Debug(&ln_pay_pln_curr);
			/*�滻ԭ�л���ƻ�*/
			memcpy(&ln_pay_pln, &ln_pay_pln_curr, sizeof( struct ln_pay_pln_c ));
      Ln_pay_pln_Debug(&ln_pay_pln);
			/*���׻�����ֵ*/
			strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

			/* �Ƿ��Զ����� */
			if ( g_ln_parm.auto_pay_ind[0]!='Y')
			{
				goto NoPay;
			}
			/**xxx*6lines*/
			if ( g_ln_mst.repay_ac_id<=0 )
			{
				sprintf(acErrMsg,"�޹黹�˺�[%ld]",g_ln_mst.repay_ac_id);
				WRITEMSG
					goto NoPay;
			}
			if ( g_dd_parm.draw_ind[0]=='N' )
			{
				sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
				WRITEMSG
					goto NoPay;
			}
			/* �������˺�״̬ */
			/* ���ݱ������ɻ��ڻ����˺�ID */
			ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
			if ( ret )
			{
				sprintf(acErrMsg,"���ݱ��������˻���Ŵ���[%s]!",g_ln_parm.cur_no);
				WRITEMSG
					goto ErrExit;	
			}
			ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst );
			if ( ret )
			{
				sprintf(acErrMsg,"�˺Ų�����[%ld|%d]",g_ln_mst.repay_ac_id,ac_seqn);
				WRITEMSG
					goto NoPay;
			}
			sprintf(acErrMsg,"�˺�[%ld]",g_dd_mst.ac_id);
			WRITEMSG
			/* ������ʾ�˺� */
				ret = pub_base_acid_acno( g_dd_mst.ac_id,ac_seqn,&dd_ac_no );	
			if( ret ) {	goto ErrExit; }

			sprintf(acErrMsg,"�˺�[%ld|%d][%s]",g_dd_mst.ac_id,ac_seqn,dd_ac_no);
			WRITEMSG

				ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
				g_dd_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"�����ڸò�Ʒ���!prdt_no=[%s]",g_dd_mst.prdt_no);
				WRITEMSG
					strcpy(g_pub_tx.reply,"P145");
				goto ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
				WRITEMSG
					goto ErrExit;
			}
			if ( g_dd_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�˺�״̬������״̬!");
				WRITEMSG
					strcpy(g_pub_tx.reply,"P109");
				goto NoPay;
			}
			/* ����� */
			g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;

			ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00);
			if ( ret<=0 )
			{	
				sprintf(acErrMsg,"���㹻���[%.2lf]",g_dd_mst.bal);
				WRITEMSG
					goto NoPay;
			}
			/* ���Ƕ���״̬ */
			if ( g_dd_mst.hold_sts[0]!='0' )
			{
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

				/**��ȫ�����ֻ�������ӿ���Ϊ0.00**/
				if ( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
					g_pub_tx.tmp_amt1=0.00;
				else
					/*g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt; ��һ�δ����У�g_pub_tx.tmp_amt1 �����ظ�����g_dd_mst.hold_amtһ�Σ���ȷ��Ӧ��ɾ�� 20140428_wjl*/	

				ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1 , 0.00);
				if ( ret<=0 )
				{
					sprintf(acErrMsg,"���㹻���[%s][%.2lf]",
						g_dd_mst.hold_sts,g_pub_tx.tmp_amt1);
					WRITEMSG
						goto NoPay;
				}
			}
			TRACE
			/* ���´������ļ� */
				ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1, 
				ln_pay_pln.curr_amt+ln_pay_pln.curr_intst );
			if ( ret<0 )
			{
				/*������*/
				/*���ǵ�����*/
				if (g_pub_tx.tx_date < ln_pay_pln.end_date)
				{
					goto NoPay;
				}
				else
				{
					/* ������ */
					/*���û���ƻ�����ȷ������ǷϢ�Ļ�����*/
					i=0;
					vln_intst[i].intst_amt=ln_pay_pln.curr_amt; 
					vln_intst[i].type=0;
					i++;
					vln_intst[i].intst_amt=ln_pay_pln.curr_intst; 
					vln_intst[i].type=1;
					i++;
					ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
						&g_pub_tx.tmp_amt1 );
					if ( ret ) goto ErrExit;
					g_pub_tx.tx_amt1=vln_intst[0].pay_amt;
					g_pub_tx.tx_amt2=vln_intst[1].pay_amt;
					/****
					ln_pay_pln.curr_amt-=g_pub_tx.tx_amt1;
					ln_pay_pln.curr_intst-=g_pub_tx.tx_amt2;
					****/

					sprintf(acErrMsg,"��Ƿ����:����=[%lf],��Ϣ=[%lf] ",
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst);
					WRITEMSG
						sprintf(acErrMsg,"�ɹ黹���:����tx_amt1=[%lf],��Ϣtx_amt2=[%lf] ",
						g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
					WRITEMSG

					/* �Ǽǰ���Ƿ��� */
						ret = pub_ln_Ins_ln_lo( g_ln_mst , ln_pay_pln , "0" ,
						ln_pay_pln.curr_amt,ln_pay_pln.curr_intst,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2 ); 	
					if (ret)
					{
						sprintf(acErrMsg,"�ǼǴ���Ƿ���ʧ��");
						WRITEMSG
							strcpy(g_pub_tx.reply,"L083");
						goto ErrExit;
					}
					/* �ж��Ƿ��ǰ��� */      
					if (g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
					{
						/* �Ƿ������һ�� */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							/* �����µĻ���ƻ� */    
							now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*������*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*��ǰ����*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
								now_ln_bal, &ln_pay_pln_next );
							if (ret)
							{
								sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
								WRITEMSG
									goto ErrExit;
							}
							sprintf(acErrMsg,"���ɵ���һ�ڻ���ƻ�!��һ�ڵ�[%d]��",ln_pay_pln.curr_cnt);
							WRITEMSG

								sprintf(acErrMsg,"�˺����[%d]",ln_pay_pln_next.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln_next.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln_next.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln_next.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"������[%d]",ln_pay_pln_next.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln_next.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln_next.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln_next.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln_next.end_date);
							WRITEMSG

								sprintf(acErrMsg,"��ǰ�ڴλ���ƻ�!!!!!!!!!!!!!!!!!!!");
							sprintf(acErrMsg,"�˺����[%d]",ln_pay_pln.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"������[%d]",ln_pay_pln.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln.end_date);
							WRITEMSG															

							/* ɾ��������¼ *
							ret = sql_execute("delete from ln_pay_pln where \
							ac_id=%ld and ac_seqn=%d",\
							ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if(ret)
							{
							sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
							WRITEMSG
							strcpy(g_pub_tx.reply,"L077");
							goto ErrExit;
							}
							***********************/
								vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* ��ӻ���ƻ��� *
							ret = Ln_pay_pln_Ins( ln_pay_pln_next , g_pub_tx.reply );
							if(ret)
							{
							sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
							WRITEMSG
							strcpy(g_pub_tx.reply,"L077");
							goto ErrExit;
							}
							*************************/
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln_next.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_next.rowid)-1);
							 
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln_next, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							}else
							{
								strcpy( ln_pay_pln.pay_ind , "1" );    /* �ѻ� */
								
								/**add by xyy 2006-12-1 22:28**/
							  memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							  ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							  if ( ret )
							  {
							  	sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
							  	WRITEMSG
							  		goto ErrExit;
							  }
							  else
							    memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
								
								vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
								
								ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
								if ( ret )
								{
									sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
									WRITEMSG
										goto ErrExit;
								}
							}
						}
						else	/**hao @20050817@ �Ѿ��Ǽ�Ƿ����ˣ�����ƻ����ѻ� **/
						{
							strcpy( ln_pay_pln.pay_ind , "1" );    /* �ѻ� */
							
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							  
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}	
						}
					}
				}
				else
				{
					/* �������� */
					g_pub_tx.tx_amt1 = ln_pay_pln.curr_amt;
					g_pub_tx.tx_amt2 = ln_pay_pln.curr_intst;           
					g_pub_tx.tmp_amt1=ln_pay_pln.curr_amt+ln_pay_pln.curr_intst;
					strcpy( ln_pay_pln.pay_ind , "1" );    /* �ѻ� */
					ln_pay_pln.de_ac_id = g_dd_mst.ac_id;
					ln_pay_pln.de_ac_seqn = ac_seqn;

					sprintf( acErrMsg,"��������[%s]",g_ln_parm.ln_pay_type );
					WRITEMSG

					/* ���ǰ��Ҵ��� */
					if (g_ln_parm.ln_pay_type[0]!='3'&&g_ln_parm.ln_pay_type[0]!='4') 
					{
						if ( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)<0 )
						{
							ln_pay_pln.curr_amt = g_ln_mst.bal ;
						}
					}
					/* �ж��Ƿ��ǰ��ң������Ҵ��� */      
					if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
					{
						/* �Ƿ������һ�� */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							/* �����µĻ���ƻ� */                        		
							now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*������*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*��ǰ����*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,
								now_ln_bal, &ln_pay_pln_next );
							Ln_pay_pln_Debug(&ln_pay_pln_next);
							if (ret)
							{
								sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
								WRITEMSG
									goto ErrExit;
							}
							Ln_pay_pln_Debug(&ln_pay_pln_next);
							sprintf(acErrMsg,"���ɵ���һ�ڻ���ƻ�!��һ�ڵ�[%d]��",ln_pay_pln.curr_cnt);
							WRITEMSG

								vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* ���±��ڻ���ƻ�Ϊ�ѻ� */
							
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln_next.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_next.rowid)-1);
							  
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);  
							  
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln_next, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							Ln_pay_pln_Debug(&ln_pay_pln_next);
						}
						else
						{
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							vtcp_log("[%s][%d] ln_pay_pln.ac_id==[%ld]  ln_pay_pln.ac_seqn=[%ld]",__FILE__,__LINE__,ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							
							/* ���±��ڻ���ƻ�Ϊ�ѻ� */
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;	
							}	
						}
					}
					else
					{
						/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
						
						vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
						
						/* ���±��ڻ���ƻ�Ϊ�ѻ� */
						ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
						if ( ret )
						{
							sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
							WRITEMSG
								goto ErrExit;	
						}	
					}
				}
				goto YOUPay;
				NoPay:
					g_pub_tx.tmp_amt1=0.00;
				/*������*/
				if (g_pub_tx.tx_date < ln_pay_pln.end_date)
				{
					continue;
				}
				else
				{
					/* ������ */
					TRACE
					/*���û���ƻ�����ȷ������ǷϢ�Ļ�����*/
						i=0;
					vln_intst[i].intst_amt=ln_pay_pln.curr_amt; 
					vln_intst[i].type=0;
					i++;
					vln_intst[i].intst_amt=ln_pay_pln.curr_intst; 
					vln_intst[i].type=1;
					i++;
					ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
						&g_pub_tx.tmp_amt1 );
					if ( ret ) goto ErrExit;
					g_pub_tx.tx_amt1=vln_intst[0].pay_amt;
					g_pub_tx.tx_amt2=vln_intst[1].pay_amt;

					sprintf(acErrMsg,"��Ƿ����:����=[%lf],��Ϣ=[%lf] ",
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst);
					WRITEMSG
					/* �Ǽǰ���Ƿ��� */
						ret = pub_ln_Ins_ln_lo( g_ln_mst , ln_pay_pln , "0" ,
						ln_pay_pln.curr_amt , ln_pay_pln.curr_intst , g_pub_tx.tx_amt1 , g_pub_tx.tx_amt2 ); 	
					if (ret)
					{
						sprintf(acErrMsg,"�ǼǴ���Ƿ���ʧ��");
						WRITEMSG
							strcpy(g_pub_tx.reply,"L083");
						goto ErrExit;
					}
					TRACE
					/* �ж��Ƿ��ǰ��� */      
					if (g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
					{
						/* �Ƿ������һ�� */
						if ( ln_pay_pln.curr_cnt != ln_pay_pln.ttl_cnt )
						{
							TRACE
							/* �����µĻ���ƻ� */    
								now_ln_bal= now_ln_bal-ln_pay_pln.curr_amt;
							ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1-1;		/*������*/
							curr_cnt= ln_pay_pln.curr_cnt+1;		/*��ǰ����*/
							ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, 
								curr_cnt, ttl_cnt, now_ln_bal, &ln_pay_pln_next );
							if (ret)
							{
								sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
								WRITEMSG
									goto ErrExit;
							}
							sprintf(acErrMsg,"���ɵ���һ�ڻ���ƻ�!��һ�ڵ�[%d]��",ln_pay_pln.curr_cnt);
							WRITEMSG

								sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln_next.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln_next.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln_next.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"������[%d]",ln_pay_pln_next.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln_next.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln_next.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln_next.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln_next.end_date);
							WRITEMSG

								sprintf(acErrMsg,"��ǰ�ڴλ���ƻ�!!!!!!!!!!!!!!!!!!!");
							sprintf(acErrMsg,"�˺����[%d]",ln_pay_pln.ac_seqn);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�[%d]",ln_pay_pln.curr_cnt);
							WRITEMSG
								sprintf(acErrMsg,"�Ǽ�����[%ld]",ln_pay_pln.wrk_date);
							WRITEMSG
								sprintf(acErrMsg,"����״̬0[%s]",ln_pay_pln.pay_ind);
							WRITEMSG

								sprintf(acErrMsg,"������[%d]",ln_pay_pln.ttl_cnt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",ln_pay_pln.curr_amt);
							WRITEMSG
								sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",ln_pay_pln.curr_intst);
							WRITEMSG
								sprintf(acErrMsg,"��ʼ�黹����[%ld]",ln_pay_pln.beg_date);
							WRITEMSG
								sprintf(acErrMsg,"���ڹ黹����[%ld]",ln_pay_pln.end_date);
							WRITEMSG															
							/* ɾ��������¼ */
								ret = sql_execute("delete from ln_pay_pln where \
								ac_id=%ld and ac_seqn=%d",\
								ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if (ret)
							{
								sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
								WRITEMSG
									strcpy(g_pub_tx.reply,"L077");
								goto ErrExit;
							}
							vtcp_log("DELE[%d-%d]",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn); TRACE
							/* ��ӻ���ƻ��� */	
								ret = Ln_pay_pln_Ins( ln_pay_pln_next , g_pub_tx.reply );
							if (ret)
							{
								sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
								WRITEMSG
									strcpy(g_pub_tx.reply,"L077");
								goto ErrExit;
							}		
							}else
							{
								strcpy( ln_pay_pln.pay_ind , "1" );    /* �ѻ� */
								
								/**add by xyy 2006-12-1 22:28**/
							  memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							  ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							  if ( ret )
							  {
							  	sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
							  	WRITEMSG
							  		goto ErrExit;
							  }
							  else
							    memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
								
								vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
								
								ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
								if ( ret )
								{
									sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
									WRITEMSG
										goto ErrExit;
								}
							}
						}
						else	/**hao @20050817@ �Ѿ��Ǽ�Ƿ����ˣ�����ƻ����ѻ� **/
						{
							strcpy( ln_pay_pln.pay_ind , "1" );    /* �ѻ� */
							
							/**add by xyy 2006-12-1 22:28**/
							memset(&ln_pay_pln_temp,'\0',sizeof(struct ln_pay_pln_c));
							ret =  Ln_pay_pln_Sel1(g_pub_tx.reply,&ln_pay_pln_temp,"ac_id='%ld' and ac_seqn='%ld'",ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}
							else
							  memcpy(ln_pay_pln.rowid,ln_pay_pln_temp.rowid,sizeof(ln_pay_pln_temp.rowid)-1);
							
							vtcp_log("[%s][%d] ln_pay_pln.rowid==[%s]  ln_pay_pln.rowid=[%s]",__FILE__,__LINE__,ln_pay_pln.rowid,ln_pay_pln_temp.rowid);
							
							ret = Ln_pay_pln_Upd_Upd(ln_pay_pln, g_pub_tx.reply  );
							if ( ret )
							{
								sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
								WRITEMSG
									goto ErrExit;
							}	
						}
					}
					continue;

					YOUPay:
					/*�ǼǴ���Ǽǲ�*/
					/* ����Ǽǲ�--���� */
						ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"1",g_pub_tx.tx_amt1,0.00,"0",1 );
					if ( ret ) 	goto ErrExit;
					/* ����Ǽǲ�--��Ϣ */
					ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
					if ( ret ) 	goto ErrExit;	

					TRACE
					/* ����ˮ�ͼ��� */
					/* ������𲿷֣���ˮ */
					strcpy( g_pub_tx.ac_no , ln_ac_no);
					g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
					g_pub_tx.ac_id = g_ln_mst.ac_id;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
					strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
					strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
					strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
					strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
					g_pub_tx.svc_ind=3019;	
					strcpy ( g_pub_tx.brf,"���ҹ黹--����" );
					strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/

					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"���׼��ʴ������!");
						WRITEMSG
							goto ErrExit;
					}
					TRACE
					/* �����Ϣ���֣���ˮ */
						strcpy( g_pub_tx.ac_no , ln_ac_no);
					g_pub_tx.ac_id = g_ln_mst.ac_id;
					g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
					strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
					strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
					strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/ 
					strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
					g_pub_tx.svc_ind=3059;	
					strcpy ( g_pub_tx.brf,"���ҹ黹--��Ϣ�ͷ�Ϣ" );
					strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"���׼��ʴ������!");
						WRITEMSG
							goto ErrExit;
					}
					/* ������֣���Ƽ��� */
					set_zd_data("0210",g_ln_parm.cur_no);    
					set_zd_double("0430",g_pub_tx.tx_amt2); 
					set_zd_double("0400",g_pub_tx.tx_amt1);                                               
					strcpy(g_pub_tx.sub_tx_code,"G085");
					strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
					ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
					if (ret != 0)
					{
						sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
						WRITEMSG
							goto ErrExit;
					}
					/* ί��Э�����(����) 
					ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "2" );
					if ( ret )  goto ErrExit;*/
					/* ί��Э�����(��Ϣ) 
					ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
					if ( ret )  goto ErrExit;*/

					/* ȡ�����ˮ */
					strcpy( g_pub_tx.ac_no , dd_ac_no );
					g_pub_tx.ac_id = g_dd_mst.ac_id;
					g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;

					sprintf(acErrMsg,"!![%s],[%ld][%d]",
						g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG

						g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+g_pub_tx.tx_amt2;
					strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/
					strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-���� */
					strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
					strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
					strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
					strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
					strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
					g_pub_tx.ac_wrk_ind[9]='1';		/*���ڷǽ��㻧ǿ�пۿ�*/
					g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */
					g_pub_tx.svc_ind=1001;	
					strcpy ( g_pub_tx.brf,"���ҿۿ�" );
					strcpy( g_pub_tx.beg_note_no, dd_ac_no);    /*Ϊ���մ�Ʊ��ӡ��ֵdd_ac_no*/

					if ( pub_acct_trance() )
					{
						sprintf(acErrMsg,"���׼��ʴ������!");
						WRITEMSG
							goto ErrExit;
					}   
					/* ȡ�����Ƽ��� */
					set_zd_data("102J",g_ln_parm.cur_no);    
					set_zd_double("102F",g_pub_tx.tx_amt1);                     
					strcpy(g_pub_tx.sub_tx_code,"D003");
					strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
					ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
					if (ret != 0)
					{
						sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
						WRITEMSG
							goto ErrExit;
					}            
				}
				Ln_pay_pln_Clo_Upd1( );    		                         
				sprintf(acErrMsg,"���չ黹����Ƿ����¼ PASS");
				WRITEMSG	

					OkExit:
					sql_commit();	/*--�ύ����--*/
				strcpy(g_pub_tx.reply,"0000");
				sprintf(acErrMsg,"���չ黹����Ƿ����¼�������ɹ�!!!!![%s]",g_pub_tx.reply);
				WRITEMSG
					set_zd_data("0120",g_pub_tx.reply);
				return 0;
				ErrExit:
				if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
				{
					strcpy(g_pub_tx.reply,"G009");/* ���޸� */
				}
				sql_rollback();	/*--����ع�--*/
				sprintf(acErrMsg,"���չ黹����Ƿ����¼�������ʧ��!!!!![%s]",g_pub_tx.reply);
				WRITEMSG
					set_zd_data("0120",g_pub_tx.reply);
				return 1;
			}

