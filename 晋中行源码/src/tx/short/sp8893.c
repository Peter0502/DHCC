/*****************************************************************
*��������sp8893.c
*���ܣ�ʵ�ֶԹ�����Ƿ�Ѳ���
*���ߣ�lwb
*ʱ�䣺20141210
******************************************************************/

#include "public.h"
#include "mob_acct_type_c.h"
#include "ln_mst_c.h"
#include "com_item_c.h"
#include "mdm_ac_rel_c.h"
#include "mob_server_type_c.h"
#include "com_branch_c.h"
#include "mob_fee_mx_c.h"
#define  EXTERN

int sp8893()
{
	int ret = 0;
	int ttlnum = 0;
	double dAmt = 0.0;
	double bal = 0.0;
	int n=0;
	long date_tmp = 0;
	char cAcno[25];
	char in_br_no[8];
	struct mob_server_type_c sMob_server_type;
	struct mob_acct_type_c sMob_acct_type;
	struct dd_mst_c g_dd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mob_fee_mx_c sMob_fee;
	
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sMob_server_type,0x00,sizeof(sMob_server_type));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(cAcno,0x00,sizeof(cAcno));
	memset(in_br_no,0x00,sizeof(in_br_no));
	memset(&sMob_fee,0x00,sizeof(sMob_fee));
	
	pub_base_sysinit();
	
	get_zd_double("0410",&dAmt);
	get_zd_data("0300",cAcno);
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0580",g_pub_tx.beg_note_no);
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);

	
	ret = Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",cAcno);
	if(ret)
	{	
		sprintf(acErrMsg, "ȡ�����α����Mob_acct_type [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	while(1)
	{
		ret = Mob_acct_type_Fet_Upd(&sMob_acct_type,g_pub_tx.reply);
		if(ret == 100)
		{
			if(ttlnum)
				break;
			else{
			sprintf(acErrMsg, "Mob_acct_type���޴˼�¼ [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
			}				
		}	
		else if(ret)
		{
			sprintf(acErrMsg, "ȡ�����շ�ǩԼ��¼ʧ�� [%d][%s]", ret,g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
		}
		
		if(!ttlnum){
			/***��ȡ�շѽ��***/
		ret = Mob_server_type_Sel(g_pub_tx.reply,&sMob_server_type,"server_type='%s'",sMob_acct_type.server_type);  
	  if (ret)
	  {
	     vtcp_log("[%s][%d]��ѯ��_Sel����[%d]",__FILE__,__LINE__,ret);
	     strcpy(g_pub_tx.reply,"D104");
	     goto ErrExit;
	  }
	  	/**�շѴ���*/
		/**���ڻ�**/
		if(sMob_acct_type.ac_type[0]=='1')
		{
			/**������˻����**/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",sMob_acct_type.ac_id);
			if(ret)
			{
				sprintf(acErrMsg, "���ڱ����Dd_mst [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld",g_dd_mst.ac_id);
			if(ret)
			{
				sprintf(acErrMsg, "��ѯmdm_ac_rel [%d]����", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			memcpy(in_br_no,g_dd_mst.opn_br_no,5);
		}
		/**���**/
		if(sMob_acct_type.ac_type[0]=='3')
		{
			/**������˻���Ӧ���㻧ac_id**/
			ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"pact_no like '%%%s%%' ",sMob_acct_type.ac_no);
			if(ret)
			{
				sprintf(acErrMsg, "��ѯ��������Dd_mst [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			/**����㻧�˻����**/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_ln_mst.repay_ac_id);
			if(ret)
			{
				sprintf(acErrMsg, "���ڱ����Dd_mst [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld",g_dd_mst.ac_id);
			if(ret)
			{
				sprintf(acErrMsg, "��ѯmdm_ac_rel [%d]����", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			memcpy(in_br_no,g_dd_mst.opn_br_no,5);
		}
		if(sMdm_ac_rel.note_sts[0]!='0')
		{
			sprintf(acErrMsg,"�ý���״̬������ [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D140");
			goto ErrExit;
		}/**��ʧ**/
		if(g_dd_mst.ac_sts[0]!='1')
		{
			sprintf(acErrMsg,"���˺�״̬������ [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L016");
			goto ErrExit;
		}/**����**/
		if( (g_dd_mst.hold_sts[0]=='1')||(g_dd_mst.hold_sts[0]=='2'))
		{
			sprintf(acErrMsg,"���˺��ѱ����� [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L098");
			goto ErrExit;
		}
		bal=0.00;
		if(g_dd_mst.hold_sts[0]=='3')
		{
			bal=g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt;
		}
		else
			bal=g_dd_mst.bal-g_dd_mst.ctl_amt;
		
		if(pub_base_CompDblVal(dAmt, bal)>0)
		{
			sprintf(acErrMsg,"���˺����� [%d]", ret);
			WRITEMSG
			 strcpy(g_pub_tx.reply,"LS51");
			goto ErrExit;	
		}
		
		}
		sMob_acct_type.mob_sts[0] = '1';
		/*n=pub_base_CalMouths(sMob_acct_type.lst_fee_date,g_pub_tx.tx_date);
		pub_base_deadlineM(sMob_acct_type.lst_fee_date,n,&date_tmp);*/
		sMob_acct_type.lst_fee_date = g_pub_tx.tx_date;
		
		ret = Mob_acct_type_Upd_Upd(sMob_acct_type,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg, "����Mob_acct_type [%d]����", ret);
			WRITEMSG
			Mob_acct_type_Clo_Upd();
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;	
		}
		ttlnum++;
	}
		
		Mob_acct_type_Clo_Upd();
		vtcp_log("[%s][%d]��ʼ���пۿ�ac_id[%ld]ac_no[%s]!!!",__FILE__,__LINE__,sMob_acct_type.ac_id,sMob_acct_type.ac_no);
			/**�¿ͻ���*/
			ret=sub_tran_acct1(sMdm_ac_rel.ac_no,dAmt,"���ſ۷�",in_br_no,"2");
			if(ret)
			{
				if(!memcmp(g_pub_tx.reply,"P105",4))
				{
					vtcp_log("[%s][%d]�ñʿۿ�ʧ��,P105!!sMob_server_type.fee_standard=[%f],bal=[%f],ac_no=[%s],"\
					,__FILE__,__LINE__,sMob_server_type.fee_standard,bal,sMdm_ac_rel.ac_no);
					
				}else
				{
					vtcp_log("[%s][%d]�ñʿۿ�ʧ��,[%s]!!sMob_server_type.fee_standard=[%f],bal=[%f],ac_no=[%s],"\
					,__FILE__,__LINE__,g_pub_tx.reply,sMob_server_type.fee_standard,bal,sMdm_ac_rel.ac_no);
					
				}
				goto ErrExit;
			}
			/**�ǿ�Ŀ��*/
			ret=sub_cr_acct1("51110",dAmt,"���ſ۷�",in_br_no,"2");
			if(ret)
			{
				goto ErrExit;
			}

			/*�ǼǶ���������ˮ��*/
			memcpy(sMob_fee.tel,g_pub_tx.tel,6);
			memcpy(sMob_fee.tx_code,g_pub_tx.tx_code,4);
			memcpy(sMob_fee.ct_ind,"2",1);
			memcpy(sMob_fee.br_no,g_dd_mst.opn_br_no,sizeof(sMob_fee.br_no)-1);

			memcpy(sMob_fee.ac_no,sMdm_ac_rel.ac_no,sizeof(sMdm_ac_rel.ac_no));
			if(sMob_acct_type.ac_type[0]=='1')
			{
				memcpy(sMob_fee.sign_ac_no,sMob_acct_type.ac_no,sizeof(sMob_acct_type.ac_no)-1);
			}
			else
			{
				memcpy(sMob_fee.sign_ac_no,sMob_acct_type.ac_no,sizeof(sMob_acct_type.ac_no)-1);
			}
			memcpy(sMob_fee.name,g_dd_mst.name,sizeof(g_dd_mst.name)-1);
			memcpy(sMob_fee.server_type,sMob_acct_type.server_type,sizeof(sMob_fee.server_type)-1);
			sMob_fee.amt_should=sMob_server_type.fee_standard;
			sMob_fee.amt_fact=sMob_server_type.fee_standard;
			sMob_fee.tx_date=g_pub_tx.tx_date;
			sMob_fee.trace_no=g_pub_tx.trace_no;
			sMob_fee.print_cnt=0;
			sMob_fee.lst_fee_date=sMob_acct_type.lst_fee_date;
			memcpy(sMob_fee.memo,"���ŷ���ѿ۷�",20);
			memcpy(sMob_fee.sts,"0",1);

			ret=Mob_fee_mx_Ins(sMob_fee,g_pub_tx.reply);
			if(ret)
			{
				sprintf(acErrMsg, "����Mob_fee_Ins [%d]����", ret);
				WRITEMSG
				 strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]�ۿ�ɹ�ac_no=[%s]���=[%f]!!!",__FILE__,__LINE__,sMob_acct_type.ac_no,sMob_server_type.fee_standard);
		
OkExit:
	vtcp_log("���ſ۷����");
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	Mob_acct_type_Clo_Sel();
	vtcp_log("[%s][%d]���ſ۷Ѵ���ʧ��! tx_code=[DX09],prog_name=[gDX09]",__FILE__,__LINE__);
	set_zd_data("0120", g_pub_tx.reply);
	return 1;					
}


int sub_tran_acct1( char *dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	vtcp_log("�跽���ʿ�Ŀ[%s][%.2lf]",dr_ac_no,amt);
	g_pub_tx.ac_id = 0;	 /*test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy( g_pub_tx.tx_code, "DX09");
	strcpy( g_pub_tx.ac_no, dr_ac_no );
	memcpy(g_pub_tx.tx_br_no,br_no,5);
	strcpy(g_pub_tx.opn_br_no,br_no);
	strcpy( g_pub_tx.ac_wrk_ind,"000" );
	g_pub_tx.tx_amt1 = amt;
	if(dr_ac_no[0]=='9')
	{
			strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
			g_pub_tx.svc_ind=9001;
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "A016" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			set_zd_data( "1204", "01" );
			set_zd_data( "1205", "2" );
			set_zd_double( "1208", g_pub_tx.tx_amt1 );
	}
	else
	{
			strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
			g_pub_tx.svc_ind=1001;
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D003" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			strcpy( g_pub_tx.prt_ind, "0" );
			set_zd_data( "102O", "0" );
			set_zd_data( "102J", "01" );
			set_zd_data( "102K", "2" );
			set_zd_double( "102F", g_pub_tx.tx_amt1 );
			set_zd_double( "102P", 0.00 );
			set_zd_double( "102Q", 0.00 );
			vtcp_log("11111111111111111");
	}
	strcpy( g_pub_tx.brf,"���ſ۷�" );
	strcpy( g_pub_tx.no_show,"0" );
	strcpy(g_pub_tx.add_ind,"0");
vtcp_log("222222222222222222");
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"000000101");
	strcpy(g_pub_tx.hst_ind,"1");
	vtcp_log("3333333333333333333333");
	ret=pub_acct_trance();
	if( ret ) goto ErrExit;
  /* ���л�Ƽ��� */
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code );
	if (ret != 0)
	{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
	}
	vtcp_log("4444444444444444444444");
	set_zd_data("102O","" );
	set_zd_data("102J","" );
	set_zd_double("102F",0.00 );
	set_zd_data("102K","" );
	set_zd_double("102P",0.00 );
	set_zd_double("102Q",0.00 );
	set_zd_data( "1204", "" );
	set_zd_data( "1205", "" );
	set_zd_double( "1208", 0.00 );
	strcpy( g_pub_tx.add_ind,"" );
	return 0;
ErrExit:
	return 1;
}
int sub_cr_acct1(char * cr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	vtcp_log("�������ʿ�Ŀ[%s][%.2lf]",cr_ac_no,amt);
	strcpy( g_pub_tx.tx_code, "DX09");
	memcpy(g_pub_tx.tx_br_no,br_no,5);
	strcpy(g_pub_tx.opn_br_no,br_no);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=9001;
	strcpy( g_pub_tx.ct_ind,ct_ind );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,"" );
	strcpy( g_pub_tx.ac_no,cr_ac_no );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.brf,brf);
	strcpy( g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.sub_tx_code, "A017");

	/*** ���û�Ʒ�¼�Զ��� ***/
	ret=pub_acct_trance();
	if( ret ) goto ErrExit;
	/* ���л�Ƽ��� */
	set_zd_data("1214","01" );
	set_zd_data("1215",ct_ind );
	set_zd_double("1218",amt);
	ret = pubf_acct_proc("A017");
	if (ret != 0)
	{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		 strcpy(g_pub_tx.reply,"H034");
		goto ErrExit;
	}
	set_zd_data("1204","" );
	set_zd_data("1205","" );
	set_zd_double("1208",0.00 );
	set_zd_data("1214","" );
	set_zd_data("1215","" );
	set_zd_double("1218",0.00 );
  strcpy( g_pub_tx.add_ind,"" );

	return 0;
ErrExit:
	return 1;
}
