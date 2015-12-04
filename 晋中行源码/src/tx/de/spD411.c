/***************************************************************
* �� �� ��: spD411.c
* ����������֪ͨ�Ǽ�/����/�������
* ��    ��: mike
* ������ڣ�2004��1��15��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"

spD411()
{
	char ac_no[20];
	char tmp_ind1[2],tmp_ind2[2],flag[2];
	char tmp_name[51],tmp_phone[20],tmp_id_knd[2],tmp_id_no[21];
	long tmp_date1,tmp_date2,deadline;
	double tmp_amt,tmp_bal;

	struct mo_infrm_c	g_mo_infrm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));

	g_reply_int=0;
	flag[0]='0';

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

        get_zd_data( "0670", tmp_ind1 );                /* �������ͱ�־ */
	get_zd_data( "0310", g_pub_tx.ac_no );          /* �ʺ� */
	get_zd_int( "0340",  &g_pub_tx.ac_seqn );       /* �˻���� */
	get_zd_long( "0440", &tmp_date1 );              /* ֧ȡ���� */
	get_zd_double( "0410", &g_pub_tx.tx_amt1 );     /* ֧ȡ��� */
	get_zd_long( "0450", &tmp_date2 );              /* �����֧ȡ���� */
	get_zd_double( "0420", &tmp_amt );              /* �����֧ȡ��� */
	get_zd_double( "0430", &tmp_bal );              /* �ʻ���� */
	get_zd_data( "0690", tmp_ind2 );                /* ΥԼ��־ */
	get_zd_data( "0250", tmp_name );         		/* �˻����� */
	get_zd_data( "0610", tmp_phone );        		/* �绰���� */
	get_zd_data( "0680", tmp_id_knd );       		/* ֤������ */
	tmp_id_knd[1]='\0';
	get_zd_data( "0620", tmp_id_no );        		/* ֤������ */
	tmp_id_no[20]='\0';

	vtcp_log(" tmp_date1 firs2 is [%ld],tmp_id_no is [%s]",tmp_date1,tmp_id_no);
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf(acErrMsg,"������ʾ�ʺŲ�ѯ�����˻����ձ�ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P147");
		goto ErrExit;
	}

	g_reply_int = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
							 g_mdm_ac_rel.ac_id);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�ʺŲ����� !");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	vtcp_log("------------------------prdt_no==[%s]",g_td_mst.prdt_no);

	g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
							  g_td_mst.prdt_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"��Ʒ������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O215");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ��֪ͨ��������ʾ ***/
	if( g_td_parm.td_type[0]!='4' )
	{
		sprintf(acErrMsg,"���ʻ���֪ͨ�������������룡");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O204");
		goto ErrExit;
	}

	switch( tmp_ind1[0] )
	{
		case '1':												/*1-�Ǽ�*/
				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)<=0 )
				{
					sprintf(acErrMsg,"�Ǽǽ���ȷ������������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D207");
					goto ErrExit;
				}

				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)>0 )
				{
					sprintf(acErrMsg,"�Ǽǽ��ô����˻�������������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D202");
					goto ErrExit;
				}

				/*** һ�졢����֪ͨ������Ǽ����� ***/
				if( !strncmp(g_td_mst.prdt_no,"242",3) || \
					!strncmp(g_td_mst.prdt_no,"281",3) )
				{
					g_reply_int = pub_base_deadlineD( g_pub_tx.tx_date,1,&deadline );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"�������ں�������");
						WRITEMSG
						goto ErrExit;
					}
					vtcp_log(" tmp_date1 is [%ld],deadline [%ld]",tmp_date1,deadline);
					if( tmp_date1<deadline || tmp_date1>deadline )
					{
						sprintf(acErrMsg,"�Ǽ����ڳ����涨��������");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D205");
						goto ErrExit;
					}
				}
				else if( !strncmp(g_td_mst.prdt_no,"243",3) || \
			 			 !strncmp(g_td_mst.prdt_no,"282",3) )
				{
					g_reply_int = pub_base_deadlineD( g_pub_tx.tx_date,7,&deadline );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"�������ں�������");
						WRITEMSG
						goto ErrExit;
					}

					if( tmp_date1<deadline || tmp_date1>deadline )
					{
						sprintf(acErrMsg,"�Ǽ����ڳ����涨��������");
						WRITEMSG
						strcpy(g_pub_tx.reply,"D205");
						goto ErrExit;
					}
				}

				if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)>0 )
				{
					sprintf(acErrMsg,"�Ǽǽ������˻�������������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D202");
					goto ErrExit;
				}

				/*** ����Ǽ�ȡ�����С��5��Ԫ,�Թ������õ���10��Ԫ ***/
				if( g_td_parm.cif_type[0]=='1' )
				{
					if( pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,50000.00)<0 
						&& pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,0.00)!=0 )
					{
						sprintf(acErrMsg,"�������С��5��Ԫ!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"O222");
						goto ErrExit;
					}
				}
				else 
				{
					if( pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,500000.00)<0
						&& pub_base_CompDblVal(g_td_mst.bal-g_pub_tx.tx_amt1,0.00)!=0 )
					{
						sprintf(acErrMsg,"�������С��50��Ԫ!");
						WRITEMSG
						strcpy(g_pub_tx.reply,"O223");
						goto ErrExit;
					}
				}

				g_mo_infrm.ac_id = g_mdm_ac_rel.ac_id;
				g_mo_infrm.ac_seqn = g_pub_tx.ac_seqn;
				g_mo_infrm.infrm_date = g_pub_tx.tx_date;
				g_mo_infrm.prm_draw_date = tmp_date1;
				g_mo_infrm.prm_draw_amt = g_pub_tx.tx_amt1;
				g_mo_infrm.fct_wd_amt = 0.00;
				g_mo_infrm.occ_date = 0;
				strcpy (g_mo_infrm.sts,"0");
				strcpy (g_mo_infrm.name,tmp_name);
				strcpy (g_mo_infrm.phone,tmp_phone);
				strcpy (g_mo_infrm.id_knd,tmp_id_knd);
				strcpy (g_mo_infrm.id_no,tmp_id_no);
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				/*** �����˻��Ƿ��ѵǼ�δ֧ȡ ***/
				g_reply_int = Mo_infrm_Sel(g_pub_tx.ac_no,&g_mo_infrm,
								"ac_id=%ld and ac_seqn=%d and sts='0'",
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( g_reply_int==0 )
				{
				  	sprintf(acErrMsg,"���ʻ�����֪ͨδȡ��¼��������Ǽ�!");
				  	WRITEMSG
				  	strcpy (g_pub_tx.reply,"D203");
				 	goto ErrExit;
				}
				else if( g_reply_int==100 )
				{
					/*�Ǽ�֪ͨ�ǼǱ�*/
					g_reply_int = Mo_infrm_Ins(g_mo_infrm,g_pub_tx.reply);
					if ( g_reply_int )
					{
						sprintf(acErrMsg,"�Ǽ�֪ͨ�ǼǱ�����[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D107" );
						goto ErrExit;
					}
				}
				else 
				{
					sprintf(acErrMsg,"��ѯ֪ͨ�ǼǱ��쳣!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"W056");
					goto ErrExit;
				}
			 	break;
		case '2':									/*2-����*/
				if( g_pub_tx.tx_date!=g_mo_infrm.infrm_date )
				{
					g_reply_int = Td_mst_Dec_Upd(g_pub_tx.reply, \
										"ac_id=%ld and ac_seqn=%d", \
										g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
					if( g_reply_int )
					{
						sprintf(acErrMsg,"���嶨���ļ��α��[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D101" );
						goto ErrExit;
					}

					g_reply_int = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
					if( g_reply_int==100 )
					{
						sprintf(acErrMsg,"�����ļ����޴˼�¼[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D104" );
						goto ErrExit;
					}
					else if( g_reply_int )
					{
						sprintf(acErrMsg,"ȡ�����ļ�����[%d]",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}

					g_td_mst.ttl_cnt++;				/*ΥԼ������һ*/

					g_reply_int=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    				if ( g_reply_int )
					{
						sprintf(acErrMsg,"UPDATE td_mst error %d",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D105" );
						goto ErrExit;	
					}

					Td_mst_Clo_Upd( );
				}	

				/*�޸�֪ͨ�ǼǱ�*/
				g_reply_int=Mo_infrm_Dec_Upd(g_pub_tx.reply,
							"ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld \
							and prm_draw_amt=%lf",g_mdm_ac_rel.ac_id, \
							g_pub_tx.ac_seqn,tmp_date1,g_pub_tx.tx_amt1 );
				if(g_reply_int)
				{
					sprintf(acErrMsg,"����֪ͨ�ǼǱ��α��[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D101" );
					goto ErrExit;
				}

				g_reply_int=Mo_infrm_Fet_Upd(&g_mo_infrm,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"֪ͨ�ǼǱ������ڸü�¼[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D104" );
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"ȡ֪ͨ�ǼǱ��α����[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D103" );
					goto ErrExit;
				}

				strncpy(g_mo_infrm.sts,"4",1);
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				g_reply_int = Mo_infrm_Upd_Upd(g_mo_infrm,g_pub_tx.reply);
    			if( g_reply_int )
				{
					sprintf(acErrMsg,"UPDATE Mo_Infrm error %d",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D105" );
					goto ErrExit;	
				}

				Mo_infrm_Clo_Upd( );
				break;
		case '3':									/*3-���*/
				g_reply_int = Mo_infrm_Dec_Upd(g_pub_tx.reply,
							"ac_id=%ld and ac_seqn=%d and prm_draw_date=%ld \
							 and prm_draw_amt=%lf",g_mdm_ac_rel.ac_id,
							 g_pub_tx.ac_seqn,tmp_date1,g_pub_tx.tx_amt1);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"����������ļ��α����[%d]",
							g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D101" );
					goto ErrExit;
				}

				g_reply_int = Mo_infrm_Fet_Upd(&g_mo_infrm,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"֪ͨ�ǼǱ������ڸü�¼[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W057" );
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"ȡ֪ͨ�ǼǱ��α����[%d]",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D103" );
					goto ErrExit;
				}

				if(g_pub_tx.tx_date!=g_mo_infrm.infrm_date && (g_mo_infrm.prm_draw_date!=tmp_date2 || g_mo_infrm.prm_draw_amt!=tmp_amt))
				{
					strncpy(flag,"1",1);				/*ΥԼ��־*/
				}

				g_mo_infrm.prm_draw_date = tmp_date2;	/*���ĺ�Լ��֧ȡ����*/
				g_mo_infrm.prm_draw_amt = tmp_amt;		/*���ĺ�Լ��֧ȡ���*/
				g_mo_infrm.fct_wd_amt = 0.00;			/*���ĺ�ʵ�ʷ������*/
				g_mo_infrm.occ_date = 0;				/*���ĺ�ʵ�ʷ�������*/
				strcpy (g_mo_infrm.name,tmp_name);		/*���ĺ�����*/	
				strcpy (g_mo_infrm.phone,tmp_phone);	/*���ĺ�绰����*/	
				strcpy (g_mo_infrm.id_knd,tmp_id_knd);	/*���ĺ�֤������*/
				strcpy (g_mo_infrm.id_no,tmp_id_no);	/*���ĺ�֤������*/
				g_mo_infrm.wrk_date = g_pub_tx.tx_date;
				g_mo_infrm.trace_no = g_pub_tx.trace_no;

				g_reply_int=Mo_infrm_Upd_Upd(g_mo_infrm,g_pub_tx.reply);
    			if ( g_reply_int )
				{
					sprintf(acErrMsg,"UPDATE mo_infrm error %d",g_reply_int);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "D105" );
					goto ErrExit;	
				}

				Mo_infrm_Clo_Upd( );

				/*�Ƿ�ΥԼ*/
				if(flag[0]=='1')
				{
					sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",
							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG

					g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply,
								"ac_id=%ld and ac_seqn=%d",
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
					if(g_reply_int)
					{
						sprintf(acErrMsg,"����������ļ��α����[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D101" );
						goto ErrExit;
					}

					g_reply_int = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
					if( g_reply_int==100 )
					{
						sprintf(acErrMsg,"�����ļ����޴˼�¼[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}
					else if(g_reply_int)
					{
						sprintf(acErrMsg,"�򿪸������ļ��α����[%d]",
								g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D103" );
						goto ErrExit;
					}

					g_td_mst.ttl_cnt++;				/*ΥԼ������һ*/

					g_reply_int = Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    				if ( g_reply_int )
					{
						sprintf(acErrMsg,"UPDATE td_mst error %d",g_reply_int);
						WRITEMSG
						strcpy ( g_pub_tx.reply , "D105" );
						goto ErrExit;	
					}

					Td_mst_Clo_Upd( );
				}	
				break;
		default :
				 sprintf(acErrMsg,"��������ѡ���쳣!!");
				 WRITEMSG
				 strcpy(g_pub_tx.reply,"W010");
				 goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����֪ͨ�ǼǱ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"����֪ͨ�ǼǱ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
