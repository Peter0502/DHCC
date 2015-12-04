/**************************************************************
* �� �� ��: spDEDZ.c
* �������������ܼ���
* ��    ��: 
* ������ڣ�2006��10��18��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: ChenMing
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spDEDZ()
{
	char flag[2],trance_type[2];
	int ret=0;
	struct	prdt_ac_id_c	sPrdt_ac_id;
	vtcp_log("%s,%d��������ϸ���������� =======[%s]",__FILE__,__LINE__,g_pub_tx.cur_no);
	vtcp_log("%s,%d��������ϸ���������� =======[%s]",__FILE__,__LINE__,g_pub_tx.cur_no);
	vtcp_log("%s,%d��������ϸ���������� =======[%s]",__FILE__,__LINE__,g_pub_tx.cur_no);

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	memset(&g_od_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct in_parm_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

    /* For Test  */   
    double dTx_amt=0.0;
    get_zd_double("0390",&dTx_amt); 
  	vtcp_log("%s,%d 0390  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);
    dTx_amt = 0.0;
    get_zd_double("0400",&dTx_amt); 
  	vtcp_log("%s,%d 0400  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);    
	  char dc_ind[2];
	  memset(dc_ind,0,sizeof(dc_ind));
  	get_zd_data("0660",dc_ind);				/** 1-�裻2-�� **/  	
  	vtcp_log("%s,%d 0660  dc_ind=======[%s]",__FILE__,__LINE__,dc_ind);    

	  memset(dc_ind,0,sizeof(dc_ind));
  	get_zd_data("0720",dc_ind);				/** 1-�裻2-�� **/  	
  	vtcp_log("%s,%d 0720  dc_ind=======[%s]",__FILE__,__LINE__,dc_ind);    

	  memset(flag,0,sizeof(flag));
	  memset(trance_type,0,sizeof(trance_type));
	  

	get_zd_data("0680",trance_type);
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0210",g_pub_tx.cur_no);
	
	vtcp_log("%s,%d trance_type=======[%s]",__FILE__,__LINE__,trance_type);	
	vtcp_log("%s,%d ����=======[%s]",__FILE__,__LINE__,g_pub_tx.cur_no);
	vtcp_log("%s,%d ac_no=======[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	
	/**get_zd_int("0340",&g_pub_tx.ac_seqn);**/
	if( trance_type[0]=='1' )
		pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
	else
		g_pub_tx.ac_seqn=0;
	vtcp_log("%s,%d �ʺ����=======[%d]",__FILE__,__LINE__,g_pub_tx.ac_seqn);
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	get_zd_data("0660",flag);				/** 1-�裻2-�� **/
	
	vtcp_log("%s,%d note_type=======[%s]",__FILE__,__LINE__,g_pub_tx.note_type);
	vtcp_log("%s,%d beg_note_no=======[%s]",__FILE__,__LINE__,g_pub_tx.beg_note_no);
	vtcp_log("%s,%d flag=======[%s]",__FILE__,__LINE__,flag);
	
	
	if( flag[0]!='1' && flag[0]!='2' )
	{
		sprintf(acErrMsg,"�����־����!! [%s]",flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P404");
		goto ErrExit;
	}
	get_zd_data("0880",g_pub_tx.brf);
	get_zd_data("0670",g_pub_tx.ct_ind);

	vtcp_log("%s,%d brf=======[%s]",__FILE__,__LINE__,g_pub_tx.brf);
	vtcp_log("%s,%d ct_ind=======[%s]",__FILE__,__LINE__,g_pub_tx.ct_ind);
	
	
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	if( trance_type[0]=='7' )					/*** �ֽ���� ***/
	{
		get_zd_double("0400",&g_pub_tx.tx_amt1);
  	vtcp_log("%s,%d 0400  tx_amt1=======[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		
		strcpy(g_pub_tx.cash_code,"");
		if( flag[0]=='1' )
			strcpy( g_pub_tx.add_ind, "1" );	/*** �跽���� ***/
		else
			strcpy( g_pub_tx.add_ind, "0" );	/*** �������� ***/
		if( g_pub_tx.ct_ind[0]!='1' )
		{
			sprintf(acErrMsg,"�ֽ�ת�˱�־����!! [%s]",flag);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P156");
			goto ErrExit;
		}
		ret=pub_acct_cash();
		if( ret )
		{
			sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_double("100E",g_pub_tx.tx_amt1);
	}
	else
	{
		if( strlen(g_pub_tx.ac_no)==3 || strlen(g_pub_tx.ac_no)==5
		|| strlen(g_pub_tx.ac_no)==7 )			/*** ��Ŀ���� ***/
		{
			get_zd_double("0400",&g_pub_tx.tx_amt1);
  	  vtcp_log("%s,%d 0400  tx_amt1=======[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			
			if( flag[0]=='1' )
				strcpy(g_pub_tx.add_ind,"0");		/*** �跽 ***/
			else
				strcpy(g_pub_tx.add_ind,"1");		/*** ���� ***/
			g_pub_tx.svc_ind=9001;  				/*** �ڲ��ʴ�ȡ ***/
			set_zd_double("1001",g_pub_tx.tx_amt1);
		}
		else 
		{
			/*** ȡ��Ʒ���룬���ʺ���� ***/
			ret=pub_base_ac_prdt(&sPrdt_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ȡ��Ʒ�������!");
				WRITEMSG
				goto ErrExit;
			}
			strcpy( g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type );
			if( sPrdt_ac_id.ac_id_type[0]=='2' )
			{
				sprintf(acErrMsg,"�����ʺŲ������˽���!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P403");
				goto ErrExit;
			}

			if( sPrdt_ac_id.ac_id_type[0]=='1' )		/*** �Թ����� ***/
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
  	   vtcp_log("%s,%d 0400  tx_amt1=======[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				
				if( trance_type[0]!='1' )
				{
					sprintf(acErrMsg,"�����˻�����ѡ����ڽ�������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P409");
					goto ErrExit;
				}
				ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",sPrdt_ac_id.prdt_no);
				if( ret )
				{
					sprintf(acErrMsg,"ȡ���ڲ�Ʒ��������!! [%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W013");
					goto ErrExit;
				}
				if( flag[0]=='1' )	strcpy(g_pub_tx.add_ind,"0"); /*** �跽���� ***/
				else strcpy(g_pub_tx.add_ind,"1");
				g_pub_tx.svc_ind=1001;  				/*���ڴ�ȡ*/
	
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='3' 
				|| sPrdt_ac_id.ac_id_type[0]=='4' )		/*** ����͸֧ ***/
			{
				if( flag[0]=='1' ) 
					strcpy(g_pub_tx.add_ind,"1");		/*** ����跽���� ***/
				else
					strcpy(g_pub_tx.add_ind,"0");
			
				if( trance_type[0]=='2' )
				{
					get_zd_double("0400",&g_pub_tx.tx_amt1);
					g_pub_tx.svc_ind=3010;  			/*������ȡ*/
					set_zd_double("1001",g_pub_tx.tx_amt1);
				}
				else if( trance_type[0]=='3' )
				{
					get_zd_double("0400",&g_pub_tx.tx_amt2);
					g_pub_tx.svc_ind=3020;  			/*������ڴ�ȡ*/
					set_zd_double("1002",g_pub_tx.tx_amt2);
				}
				else if( trance_type[0]=='4' )
				{
					get_zd_double("0400",&g_pub_tx.tx_amt3);
					g_pub_tx.svc_ind=3030;  			/*��������ȡ*/
					set_zd_double("1003",g_pub_tx.tx_amt3);
				}
				else if( trance_type[0]=='5' )
				{
					get_zd_double("0400",&g_pub_tx.tx_amt4);
					g_pub_tx.svc_ind=3040;  			/*��������ȡ*/
					set_zd_double("1004",g_pub_tx.tx_amt4);
				}
				else
				{
					sprintf(acErrMsg,"�����˻�����ѡ����������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"P407");
					goto ErrExit;
				}
  	    vtcp_log("%s,%d 0400  tx_amt1=======[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
									
			}
			else if( sPrdt_ac_id.ac_id_type[0]=='9' )	/*** �ڲ��� ***/
			{
				get_zd_double("0400",&g_pub_tx.tx_amt1);
  	    vtcp_log("%s,%d 0400  tx_amt1=======[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
									
				if( flag[0]=='1' ) strcpy(g_pub_tx.add_ind,"0");
				else	strcpy(g_pub_tx.add_ind,"1");
				g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
				set_zd_double("1001",g_pub_tx.tx_amt1);
			}
			else
			{
				sprintf(acErrMsg,"�ʺ����ʹ���![%s]",g_pub_tx.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P410");
				goto ErrExit;
			}
		}
		vtcp_log("%s,%d into pub_acct_trance functions",__FILE__,__LINE__);
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}

		ret=pub_ln_reg(trance_type);
		if( ret )
		{
			sprintf(acErrMsg,"����ǼǱ�����ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
	}
	set_zd_data("1204",g_pub_tx.cur_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ܼ��ʼ��ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ܼ��ʼ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int pub_ln_reg(char trance_type[2])
{
	int  ret=0;
	char type[2];

	if( trance_type[0]!='2' && trance_type[0]!='3' 
		&& trance_type[0]!='4' && trance_type[0]!='5' )
		return 0;

	if( trance_type[0]=='2' ) strcpy(type,"1");
	else if( trance_type[0]=='3' ) 
	{
		strcpy(type,"2");
		g_pub_tx.tx_amt1=g_pub_tx.tx_amt2;
	}
	else if( trance_type[0]=='4' ) 
	{
		strcpy(type,"3");
		g_pub_tx.tx_amt1=g_pub_tx.tx_amt3;
	}
	else 
	{
		strcpy(type,"4");
		g_pub_tx.tx_amt1=g_pub_tx.tx_amt3;
	}

	if( g_ln_mst.intst_type[0]!='6' && g_pub_tx.add_ind[0]=='1' )	/* ���� */
	{
		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,type,g_pub_tx.tx_amt1,g_ln_mst.intst_acm,0,0);
		if( ret )
		{
			sprintf(acErrMsg,"����ǼǱ�����ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( g_ln_mst.intst_type[0]!='6' )							/*����*/
	{
		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,type,g_pub_tx.tx_amt1,g_ln_mst.intst_acm,0,1);
		if( ret )
		{
			sprintf(acErrMsg,"����ǼǱ�����ʧ��!");
			WRITEMSG
			goto ErrExit;
		}
	}

	if( g_ln_mst.intst_type[0]=='6' && ( trance_type[0]=='3' 
		|| trance_type[0]=='4' || trance_type[0]=='5' ) )
	{
		sprintf(acErrMsg,"���Ҵ�����ð��һ������!");
		WRITEMSG
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}
