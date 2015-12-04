/***************************************************************
* �� �� ��: spD406.c
* ��������������ⶳ����
* ��    ��: mike
* ������ڣ�2004��1��18��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*update tx_def set bit_map = '10000000000000000001000000000000000000011000000010000000000000000000000000000000000000000000000000000000000000000000000000000000' where tx_code = '2406' *****����41��ֵ�����ڷ����˻����յ���� zgf 20140220**
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_hold_c.h"

spD406()
{
	/*��������*/
	FILE *fp;
	int	 i=0;  
	int  cnt=0; 
	int	 ret=0;
	int  icnt=0;
	int	 unlock_flag=0;						/***�Ƿ�����ǰ���������־***/
	char tmp_ind1[2];						/***1-���ᡢ�ⶳ����***/
	char tmp_ind2[2];						/**���᷽ʽ 1-��ȫ 2-ֻ�� 3-����**/
	char tmp_ind3[2];						/***�Ƿ��Ϣ***/
	char tmp_ind4[2];						/***�Զ��ⶳ��־ Y/N***/
	char tmp_ind5[2];						/***�ⶳ��ʽ***/
	char tmp_ind6[2];						/***�Ƿ��Ϣ Y/N***/
	char tmp_memo1[100];					/***���ᱸע***/
	char tmp_memo2[100];	
	long tmp_date;							/***��������***/
	int  hold_seqn;							/***�������***/		
	double ac_bal;	
        double  hold_amt;						/***�ʻ����***/	
   
  double bal_tmp = 0.00;     
   	
	long beg_hold_date;
	char tmpname[100];
	char min_ht[2];             /***����Ȩ�� martin***/
	double  sum_amt=0.00;						/***�����ۼ� martin***/		
	struct mo_hold_c g_mo_hold;
	struct mo_hold_c f_mo_hold;
	struct dd_mst_c  f_dd_mst;
	struct td_mst_c  f_td_mst;
	
	struct dd_mst_c  dd_mst_tmp;
	struct td_mst_c  td_mst_tmp;

	/*��սṹ*/
	g_reply_int=0;
	memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&f_mo_hold,0x00,sizeof(struct mo_hold_c));
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&f_td_mst,0x00,sizeof(struct td_mst_c));
	
	memset(&dd_mst_tmp,0x00,sizeof(struct dd_mst_c));
	memset(&td_mst_tmp,0x00,sizeof(struct td_mst_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*** !!!!���ϼ�Ϣ����Ϣ��־�Ŀ��� ***/
	/*** ȡֵ ***/
	get_zd_data("0310",g_pub_tx.ac_no);			/*ȡ�ʺ�*/
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/*ȡ���*/
	get_zd_data("0670",tmp_ind1);				/*�������� 1-���� 2-�ⶳ */
	get_zd_double("1001",&ac_bal);				/*�ʻ����*/

	get_zd_data("0680",tmp_ind2);				/*���᷽ʽ*/
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/*������*/
	get_zd_data("0690",tmp_ind3);				/*�Ƿ��Ϣ(����)*/
	get_zd_data("0810",tmp_memo1);				/*���ᱸע*/
	get_zd_long("0440",&tmp_date);				/*��������*/
	get_zd_data("0710",tmp_ind5);				/*�ⶳ��ʽ*/
	get_zd_int("0350",&hold_seqn);				/*�������*/
	get_zd_double("0420",&g_pub_tx.tx_amt2);	/*�ⶳ���*/
	get_zd_data("0720",tmp_ind6);				/*�Ƿ��Ϣ(�ⶳ)*/
	get_zd_data("0820",tmp_memo2);				/*�ⶳ��ע*/
	
	/*��������*/
	if(tmp_date) strcpy(tmp_ind4,"Y");
	else strcpy(tmp_ind4,"N");
		
	g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
								 g_pub_tx.ac_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"�ʺŲ�����!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.ac_id = g_mdm_ac_rel.ac_id;

	g_reply_int=pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
									g_pub_tx.ac_type);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"������ʾ�ʺź����ȡ���ļ�ʧ��!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O017");
		goto ErrExit;
	}

	/* zy�����2005.7.27 begin */	
	switch( g_pub_tx.ac_type[0] )
	{
		case '1':				
				if( strcmp( g_pub_tx.tx_br_no, g_dd_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "���ύ�ײ�����ͨ��!opn_br_no=[%s]", \
							g_dd_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"���˺ſ�������Ϊ��%s��", \
							g_dd_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;
		case '2':
				if( strcmp( g_pub_tx.tx_br_no, g_td_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "���ύ�ײ�����ͨ��!opn_br_no=[%s]", \
							g_td_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"���˺ſ�������Ϊ��%s��", \
							g_td_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;
		case '3':
				if( strcmp( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "���ύ�ײ�����ͨ��!opn_br_no=[%s]", \
							 g_ln_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"���˺ſ�������Ϊ��%s��", \
							g_ln_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;		
		case '9':
				if( strcmp( g_pub_tx.tx_br_no, g_in_mst.opn_br_no ) != 0 )
				{
					sprintf( acErrMsg, "���ύ�ײ�����ͨ��!opn_br_no=[%s]", \
							 g_in_mst.opn_br_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "D208" );
					sprintf( acErrMsg,"���˺ſ�������Ϊ��%s��", \
							 g_in_mst.opn_br_no );
					set_zd_data( DC_GET_MSG, acErrMsg );
					goto ErrExit;
				}
				break;							
		default	:
				sprintf(acErrMsg,"�˺Ŷ�Ӧ��Ʒ���ʹ���,�������Ļ�����ϵ!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
	}
	/* zy�����2005.7.27 end */	
	
	/**if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
	 	sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}	------- hao --------- @20050716@---
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý��ʴ��ڹ�ʧ��֤״̬,��������ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M010");
		goto ErrExit;
	}                 -----**/
/* del by martin 2008-11-19 15:55  lhr ���
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѻ�֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M011");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"���˻���������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M017");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='5' )
	{
		sprintf(acErrMsg,"���˻���������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M019");
		goto ErrExit;
	}                       
*/
	if( tmp_ind1[0]=='1' )							/*1-���ᴦ��*/
	{
		i=1;
		/*** ȡ�ǼǶ���ǼǱ���Ϣ ***/
		g_mo_hold.ac_id = g_pub_tx.ac_id;
		g_mo_hold.ac_seqn = g_pub_tx.ac_seqn;
		strcpy(g_mo_hold.hold_typ,tmp_ind2);

		g_mo_hold.plan_hold_amt = g_pub_tx.tx_amt1;
		strcpy(g_mo_hold.auto_unhold_ind,tmp_ind4);
		strcpy(g_mo_hold.hold_sts,"0");
		g_mo_hold.beg_hold_date = g_pub_tx.tx_date;
		g_mo_hold.plan_unhold_date = tmp_date;
		strcpy(g_mo_hold.hold_br_no,g_pub_tx.tx_br_no);
		strcpy(g_mo_hold.hold_tel,g_pub_tx.tel);
		strcpy(g_mo_hold.hold_brf,tmp_memo1);

		strcpy(g_mo_hold.unhold_br_no,"");
		strcpy(g_mo_hold.unhold_tel,"");
		strcpy(g_mo_hold.unhold_brf,"");
		g_mo_hold.wrk_date = g_pub_tx.tx_date;
		g_mo_hold.trace_no = g_pub_tx.trace_no;

		ret = sql_count("mo_hold","ac_id=%ld and ac_seqn=%d", g_pub_tx.ac_id,
						g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!![%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==0 )
			i = 1;
		else 
			i = ret + 1 ;

		g_reply_int=Mo_hold_Dec_Upd(g_pub_tx.reply, \
									"ac_id=%ld and ac_seqn=%d and hold_sts='0'",
									g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!![%d]",g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		while(1)
		{
			g_reply_int=Mo_hold_Fet_Upd(&f_mo_hold,g_pub_tx.reply); 
			if( g_reply_int==100 )break;
			else if( g_reply_int )
			{
				sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!![%d]",g_reply_int);
				WRITEMSG
				strcpy (g_pub_tx.reply,"O016");
				goto ErrExit;
			}
/***  del by martin 2008-11-19 15:50  �����˲��� lhr ���
			if( f_mo_hold.hold_typ[0]=='1' )			*** ����ȫ���� ***
			{
				sprintf(acErrMsg,"���˻�����ȫ����!");
				WRITEMSG
				strcpy (g_pub_tx.reply,"P110");
				goto ErrExit;
			}
			else if( f_mo_hold.hold_typ[0]=='2' )		*** ��ֻ������ ***
			{
				*** ��ѡ����ֻ�������򲿷ֶ��� ***
				if(  tmp_ind2[0]=='2' || tmp_ind2[0]=='3' )
				{
					sprintf(acErrMsg,"���˻���ֻ������f_mo_hold.hold_typ=[%s]", 
							f_mo_hold.hold_typ);
					WRITEMSG
					strcpy (g_pub_tx.reply,"O161");
					goto ErrExit;
				}
			}

			if( tmp_ind2[0]=='1' && \
			  ( f_mo_hold.hold_typ[0]=='2' || f_mo_hold.hold_typ[0]=='3') )
			{
				strcpy(f_mo_hold.hold_sts,"1");		*�������*
				unlock_flag=1;
				sprintf(f_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
				sprintf(f_mo_hold.unhold_tel,g_pub_tx.tel);
				sprintf(f_mo_hold.unhold_brf,"׷����ȫ����");
			}

			if( tmp_ind2[0]=='2' && f_mo_hold.hold_typ[0]=='3' )
			{
				strcpy(f_mo_hold.hold_sts,"1");		*�������*
				unlock_flag=1;
				sprintf(f_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
				sprintf(f_mo_hold.unhold_tel,g_pub_tx.tel);
				sprintf(f_mo_hold.unhold_brf,"׷��ֻ����������");
			}

			g_reply_int=Mo_hold_Upd_Upd(f_mo_hold,g_pub_tx.reply);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!![%d]",g_reply_int);
				WRITEMSG
				strcpy (g_pub_tx.reply,"O016");
				goto ErrExit;
			}***/
		}

		Mo_hold_Clo_Upd();
		
		/*�ǼǶ���ǼǱ�*/
		g_mo_hold.hold_seqn = i;
		g_reply_int = Mo_hold_Ins(g_mo_hold,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"�ǼǶ���ǼǱ��쳣!!g_reply_int=[%d]", \
					g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O015");
			goto ErrExit;
		}
		
		/*��ѯ�˻��Ƿ���ڶ����¼*/
		icnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( icnt<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
	  ret=sql_sum_double("mo_hold","plan_hold_amt",&sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

 	 	switch(g_pub_tx.ac_type[0])
  		{
    		case '1':									/*1-����*/
				/*** ��ȫ���᲻���ٶ��� ***/
		 		/* del by lhr 2008-11-19 16:23
		 		if( g_dd_mst.hold_sts[0]=='1' )
		 		{
					sprintf(acErrMsg,"���˻��Ѵ�����ȫ����״̬!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"P110");
					goto ErrExit;
		 		}
		 		*/

				g_reply_int=Dd_mst_Dec_Upd(g_pub_tx.reply, \
					"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	    		if(g_reply_int)
	   			{
	   				sprintf(acErrMsg,"�����α����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				
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

				g_reply_int = Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"�˺Ų�����!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"ȡ��¼����![%d][%ld][%d]", \
							g_reply_int,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				if( f_dd_mst.ac_sts[0]=='*' )
				{
					sprintf(acErrMsg,"���˻�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P245");
					goto ErrExit;
				}
				
				strcpy(f_dd_mst.hold_sts,g_dd_mst.hold_sts);
				f_dd_mst.hold_amt=g_dd_mst.hold_amt; 
				/* if(atoi(f_dd_mst.hold_sts)>atoi(tmp_ind2))add by lhr 2008-11-19 16:27 
					{
						strcpy(f_dd_mst.hold_sts,tmp_ind2);
					}*/

				/*** ���ֶ��� 
				if( tmp_ind2[0]=='3' )		
					f_dd_mst.hold_amt=f_dd_mst.hold_amt+g_mo_hold.plan_hold_amt;***/

				/*** ȫ�����ᡢֻ������ del by lhr  2008-11-19 16:26
				if( tmp_ind2[0]=='1' || tmp_ind2[0]=='2' )
					f_dd_mst.hold_amt = f_dd_mst.bal;
					***/
				g_reply_int = Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
				if( g_reply_int )
				{	
		 			sprintf(acErrMsg,"�޸ļ�¼����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Dd_mst_Clo_Upd( );
				set_zd_double("0410", f_dd_mst.bal);/**���ڷ����˻����յ���� zgf 20140220**/
		 		break;
			case '2':									/*2-����*/
				/*** ��ȫ���᲻���ٶ��� ***/
				/* del by lhr 2008-11-19 16:23
		 		if( g_td_mst.hold_sts[0]=='1' )
		 		{
					sprintf(acErrMsg,"���˻��Ѵ�����ȫ����״̬!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"P110");
					goto ErrExit;
		 		}*/

				g_reply_int=Td_mst_Dec_Upd(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id, \
							g_pub_tx.ac_seqn);
	    		if(g_reply_int)
	   			{
	   				sprintf(acErrMsg,"�����α����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				
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

				g_reply_int=Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
				if(g_reply_int&&g_reply_int!=100)
				{
					sprintf(acErrMsg,"ȡ��¼����!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if(g_reply_int==100)
				{
					sprintf(acErrMsg,"�˺Ų�����!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "M003");
					goto ErrExit;
				}

				if( f_td_mst.ac_sts[0]=='*' )
				{
					sprintf(acErrMsg,"���˻�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P245");
					goto ErrExit;
				}
				
				strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);
				f_td_mst.hold_amt=g_td_mst.hold_amt; 
        /*if(atoi(f_td_mst.hold_sts)>atoi(tmp_ind2)) add by lhr 2008-11-19 16:27 
					{
						strcpy(f_td_mst.hold_sts,tmp_ind2);
					}*/
				

				/*** ���ֶ��� 
				if( tmp_ind2[0]=='3' )						
					f_td_mst.hold_amt=f_td_mst.hold_amt+g_mo_hold.plan_hold_amt;***/

				/*** ȫ�����ᡢֻ������ del by lhr  2008-11-19 16:26
				if( tmp_ind2[0]=='1' || tmp_ind2[0]=='2' )
					f_td_mst.hold_amt = f_td_mst.bal;
				***/

				g_reply_int=Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
				if(g_reply_int)
				{	
		 			sprintf(acErrMsg,"�޸ļ�¼����!");
		 			WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Td_mst_Clo_Upd( );
				set_zd_double("0410", f_td_mst.bal);/**���ڷ����˻����յ���� zgf 20140220**/
		 		break;
			default :
		 		sprintf(acErrMsg,"���˻������ڶ����˻���Χ!!");
		 		WRITEMSG
		 		strcpy (g_pub_tx.reply,"O014");
		 		goto ErrExit;
		 		break;
 	 	}	/*switch����*/

		/*�ǼǶ���ǼǱ� 2008-11-19 18:13 del by martin
		g_mo_hold.hold_seqn = i;
		g_reply_int = Mo_hold_Ins(g_mo_hold,g_pub_tx.reply);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"�ǼǶ���ǼǱ��쳣!!g_reply_int=[%d]", \
					g_reply_int);
			WRITEMSG
			strcpy (g_pub_tx.reply,"O015");
			goto ErrExit;
		}*/
 
                set_zd_int("0490",i);


                cnt = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
                      /********* 2008-11-19 15:57 lhr
                      if( cnt<0)
                      {    sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
                           WRITEMSG
                           strcpy (g_pub_tx.reply,"O021");
                           goto ErrExit;
                      } 
                      else if( ret==0 )
                           cnt = 1;
                      else
                           cnt = ret + 1 ;
                      **********/
               pub_base_AllDwnFilName( tmpname );
                      fp=fopen(tmpname,"w");
                      if( fp==NULL )
                       {
                        sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
                        WRITEMSG
                        strcpy( g_pub_tx.reply,"P157" );
                        goto ErrExit;
                       }
                    fprintf(fp,"CKPT%d|\n",cnt);
                    fclose(fp);
                    set_zd_data(DC_FILE_SND,"1"); 

		/***���ض������,��ʼΪ1***/
		set_zd_int("0490",i);	
	}
	else if( tmp_ind1[0]=='2' )							/*2-�ⶳ����*/	
	{
		/*���¶���ǼǱ�*/
		g_reply_int = Mo_hold_Dec_Upd(g_pub_tx.reply, \
							"ac_id=%ld and ac_seqn=%d and hold_seqn=%d", \
							g_pub_tx.ac_id,g_pub_tx.ac_seqn,hold_seqn);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ��쳣!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O016");
			goto ErrExit;
		}

		g_reply_int = Mo_hold_Fet_Upd(&f_mo_hold,g_pub_tx.reply);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"--------->[%ld][%d][%d]",g_pub_tx.ac_id, g_pub_tx.ac_seqn, hold_seqn );
			WRITEMSG
			sprintf(acErrMsg,"����ǼǱ��в����ڸñʼ�¼!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O019");
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg,"����ǼǱ��в����ڸñʼ�¼!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(g_mo_hold.rowid,f_mo_hold.rowid);
		beg_hold_date = f_mo_hold.beg_hold_date;
		g_mo_hold.ac_id = g_pub_tx.ac_id;
		g_mo_hold.ac_seqn = g_pub_tx.ac_seqn;
		g_mo_hold.hold_seqn = hold_seqn;
		strcpy(g_mo_hold.hold_typ,f_mo_hold.hold_typ);
		/**-------- hao ---------- **/
		g_mo_hold.plan_hold_amt = f_mo_hold.plan_hold_amt;

		/*** �����ѽ�� ***/
		if( f_mo_hold.hold_sts[0]=='1' )
		{
			sprintf(acErrMsg,"���˻��ѽ������!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"D225");
			goto ErrExit;
		}

		/*** ��ȫ���ᡢδ���������ȫ�ⶳ ***/
		if( f_mo_hold.hold_typ[0]=='1' && f_mo_hold.hold_sts[0]=='0' \
			&& tmp_ind5[0]!='1' )
		{
			sprintf(acErrMsg,"���˻�Ϊ��ȫ���ᣬ������ȫ�ⶳ!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O162");
			goto ErrExit;
		}
		else if(tmp_ind5[0]=='1' && f_mo_hold.hold_sts[0]=='0')	/*��ȫ�ⶳδ��*/
			g_mo_hold.hold_sts[0]='1';						/*��Ϊ���״̬*/
		else if( tmp_ind5[0]=='2' )							/*���ֽⶳ*/
		{
		if(f_mo_hold.hold_typ[0]=='3' )/* 2008-11-19 16:47 ���˲��ֶ����ж�*/
		 {
		 	vtcp_log("[%s][%d],hold_typ=[%s],tmp_ind5=[%s],f_mo_hold.hold_sts=[%s]\n",__FILE__,__LINE__,f_mo_hold.hold_typ,tmp_ind5,f_mo_hold.hold_sts);
			if(pub_base_CompDblVal(g_mo_hold.plan_hold_amt,g_pub_tx.tx_amt2)>0)
			{
				g_mo_hold.hold_typ[0]='3';				    /*��Ϊ���ֶ���*/
				g_mo_hold.hold_sts[0]='0';					/*��Ϊδ���״̬*/
				g_mo_hold.plan_hold_amt = \
					f_mo_hold.plan_hold_amt - g_pub_tx.tx_amt2;
				g_pub_tx.tx_amt1 = f_mo_hold.plan_hold_amt;
			}
			else if(pub_base_CompDblVal( g_mo_hold.plan_hold_amt, \
										 g_pub_tx.tx_amt2 )==0 )
			{
				g_mo_hold.hold_sts[0]='1';					/*��Ϊ���״̬*/
				g_pub_tx.tx_amt1 = g_pub_tx.tx_amt2;
			}
			else
			{
				sprintf(acErrMsg,"�ⶳ�����ڶ������˶�!![%.2f][%.2f]", \
						g_mo_hold.plan_hold_amt,g_pub_tx.tx_amt2);
				WRITEMSG
				strcpy (g_pub_tx.reply,"P418");
				goto ErrExit;
			}
		 }else/*2008-11-19 16:49 */
		 	{
		 		vtcp_log("[%s][%d],hold_typ=[%s],tmp_ind5=[%s],f_mo_hold.hold_sts=[%s]\n",__FILE__,__LINE__,f_mo_hold.hold_typ,tmp_ind5,f_mo_hold.hold_sts);
		 		sprintf(acErrMsg,"ȫ�������ֻ��������֧�ֲ��ֽⶳ!!" );
				WRITEMSG
				strcpy (g_pub_tx.reply,"P313");
				goto ErrExit;
		 	}
		 /*2008-11-19 16:49 end*/
		}

		strcpy(g_mo_hold.auto_unhold_ind,f_mo_hold.auto_unhold_ind);
		g_mo_hold.beg_hold_date = f_mo_hold.beg_hold_date;
		g_mo_hold.plan_unhold_date = f_mo_hold.plan_unhold_date;
		strcpy(g_mo_hold.hold_br_no,f_mo_hold.hold_br_no);
		strcpy(g_mo_hold.hold_tel,f_mo_hold.hold_tel);
		strcpy(g_mo_hold.hold_brf,f_mo_hold.hold_brf);

		strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
		strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
		strcpy(g_mo_hold.unhold_brf,tmp_memo2);
		g_mo_hold.wrk_date = g_pub_tx.tx_date;
		g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
		g_mo_hold.trace_no = g_pub_tx.trace_no;

    	/*���¶���ǼǱ�*/
		g_reply_int = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
	vtcp_log("g_reply_int[%d]",g_reply_int);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"���¶���ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}

		Mo_hold_Clo_Upd( );

		/*��ѯ�˻��Ƿ���ڶ����¼*/
		i = sql_count( "mo_hold","ac_id=%ld and ac_seqn=%d and hold_sts='0'", \
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( i<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		ret = sql_min_string("mo_hold","hold_typ", min_ht, sizeof(min_ht)-1,\
                         "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
     if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
	  ret=sql_sum_double("mo_hold","plan_hold_amt",&sum_amt,\
                   "ac_id=%ld and ac_seqn=%d and hold_sts='0'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
		if( ret<0 )
		{
			sprintf(acErrMsg,"��ѯ����ǼǱ�ʧ��!");
			WRITEMSG
			strcpy (g_pub_tx.reply,"O021");
			goto ErrExit;
		}
		
		/*** �޸����ļ� ***/
 		switch( g_pub_tx.ac_type[0] )
  		{
			case '1':
				if( g_dd_mst.hold_sts[0]=='0' )
				{
					sprintf(acErrMsg,"���˻��Ѵ�������״̬!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"O018");
					goto ErrExit;
				}

				if( i==0 )							/*** �޶����¼ ***/
					{
					  g_dd_mst.hold_sts[0]='0'; 
					  g_dd_mst.hold_amt=0.00; 
				  }
				else 
				{
					strcpy(g_dd_mst.hold_sts,min_ht);
					g_dd_mst.hold_amt=sum_amt; 
					/* if( tmp_ind5[0]=='2' )			���ֽⶳʱ��Ϊ���ֶ��� 
						g_dd_mst.hold_sts[0]='3';*/
				}

				g_reply_int = Dd_mst_Dec_Upd(g_pub_tx.reply, \
											"ac_id=%ld and ac_seqn=%d", \
											g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	   			if( g_reply_int )
	   			{
	   				sprintf(acErrMsg,"�����α����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				g_reply_int = Dd_mst_Fet_Upd(&f_dd_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"�˺Ų�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
/***  del by martin 2008-11-19 17:43
				strcpy( f_dd_mst.hold_sts,g_dd_mst.hold_sts );

				if( tmp_ind5[0]=='2' )
					f_dd_mst.hold_amt = f_dd_mst.hold_amt - g_pub_tx.tx_amt2;
				else if( tmp_ind5[0]=='1' )
					f_dd_mst.hold_amt = 0.00;
				else
					f_dd_mst.hold_amt = \
					f_dd_mst.hold_amt - f_mo_hold.plan_hold_amt;
***/
        strcpy(f_dd_mst.hold_sts,g_dd_mst.hold_sts);
				f_dd_mst.hold_amt=g_dd_mst.hold_amt; 

				g_reply_int = Dd_mst_Upd_Upd(f_dd_mst,g_pub_tx.reply);
				if( g_reply_int )
				{	
	 				sprintf(acErrMsg,"�޸ļ�¼����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Dd_mst_Clo_Upd( );
	 			break;
			case '2':
				if( g_td_mst.hold_sts[0]=='0' )
				{
					sprintf(acErrMsg,"���˻��Ѵ�������״̬!!");
					WRITEMSG
					strcpy (g_pub_tx.reply,"O018");
					goto ErrExit;
				}

				if( i==0 )							/*** �޶����¼ ***/
				 {
					g_td_mst.hold_sts[0]='0'; 
					g_td_mst.hold_amt=0.00; 
				 }
				else 
				{
					strcpy(g_td_mst.hold_sts,min_ht);
					g_td_mst.hold_amt=sum_amt; 
					/* if( tmp_ind5[0]=='2' )			���ֽⶳʱ��Ϊ���ֶ��� 
						g_td_mst.hold_sts[0]='3';*/
				}

				g_reply_int = Td_mst_Dec_Upd(g_pub_tx.reply, \
											"ac_id=%ld and ac_seqn=%d", \
											g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    			if( g_reply_int )
   				{
   					sprintf(acErrMsg,"�����α����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}

				g_reply_int = Td_mst_Fet_Upd(&f_td_mst,g_pub_tx.reply);
				if( g_reply_int==100 )
				{
					sprintf(acErrMsg,"�˺Ų�����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				else if( g_reply_int )
				{
					sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}

				/* 2008-11-19 17:46 by martin
				strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);

				if(tmp_ind5[0]=='2')
					f_td_mst.hold_amt = f_td_mst.hold_amt - g_pub_tx.tx_amt2;
				else
					f_td_mst.hold_amt=f_td_mst.hold_amt-f_mo_hold.plan_hold_amt;
	
	vtcp_log("hao--look[%d]",f_td_mst.ac_id);*/
	
	      strcpy(f_td_mst.hold_sts,g_td_mst.hold_sts);
				f_td_mst.hold_amt=g_td_mst.hold_amt; 
	      
				g_reply_int = Td_mst_Upd_Upd(f_td_mst,g_pub_tx.reply);
	vtcp_log("hao--look1");
				if( g_reply_int )
				{	
	 				sprintf(acErrMsg,"�޸ļ�¼����!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "P162");
					goto ErrExit;
				}
				Td_mst_Clo_Upd( );
	 			break;
		default:
			 	sprintf(acErrMsg,"���˻������ڽⶳ�˻���Χ!!");
	 			WRITEMSG
	 			strcpy (g_pub_tx.reply,"O015");
	 			goto ErrExit;
	 			break;
      }/*switch����*/

		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");
		if( fp==NULL )
		{
			sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
			WRITEMSG
			strcpy( g_pub_tx.reply,"P157" );
			goto ErrExit;
		}

		fprintf(fp,"%ld|\n",beg_hold_date);
		fclose(fp);
		set_zd_data(DC_FILE_SND,"1");
	}
	else 
	{
		sprintf(acErrMsg,"����ⶳ��������ѡ����ȷ!!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"O020");
		goto ErrExit;
	}

    /*** �Ǽǽ�����ˮ ***/
	WRITEMSG
	vtcp_log("ac_wrk_ind=[%s]",g_pub_tx.ac_wrk_ind);
	get_zd_data("0670",tmp_ind1);				/*�������� 1-���� 2-�ⶳ */
	if (tmp_ind1[0] == '1')
		strcpy(g_pub_tx.brf, "����");
	else
		strcpy(g_pub_tx.brf, "�ⶳ");

	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
	  	WRITEMSG
		strcpy (g_pub_tx.reply,"S024");
	    goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ⶳ�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ⶳ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
