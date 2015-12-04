/***************************************************************
* �� �� ��: spD099.c
* ��������������
* ��    ��: jack
* ������ڣ�2005��1��11��
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
#include "cif_id_code_rel_c.h"
#include "com_rate_c.h"

spD099()
{
	int ret;

	char tmpname[101],ct_name[5],memo[51],id_name[21];
	char br_name[31],br_tele[21],term[3],term_type[3];
	FILE *fp;
	char acc_no[8];
	struct com_rate_c sCom_rate;
	struct cif_id_code_rel_c g_cif_id_code_rel;

	memset(&sCom_rate,0x00,sizeof(struct com_rate_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("1011",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_int("1012",&g_pub_tx.ac_seqn);
	get_zd_data("101A",g_pub_tx.cur_no);
	get_zd_data("1014",g_pub_tx.note_type);
	get_zd_data("1015",g_pub_tx.beg_note_no);
	get_zd_data("1015",g_pub_tx.end_note_no);
	get_zd_data("101B",g_pub_tx.ct_ind);
	get_zd_data("1016",g_pub_tx.brf);				/***ժҪ***/
	
	vtcp_log("[%s][%d] beg_note_no=[%s]end_note_no=[%s] ",__FILE__,__LINE__,g_pub_tx.beg_note_no,g_pub_tx.end_note_no);
	
	/***add zgf 20131022 ��0630��ֵ��ȡժҪ��Ϣ****/
	char tmp_brf1[30];
	memset(tmp_brf1,'\0',sizeof(tmp_brf1));
	get_zd_data("0630",tmp_brf1);
	/***add zgf end***/
        sprintf(acErrMsg,"brf=[%s]",g_pub_tx.brf);
        WRITEMSG
	
	/*char tmp_brf[4];*/
	char tmp_brf[30]; /***add zgf 20131022 �޸Ķ����СΪ30**/
	memset(tmp_brf,'\0',sizeof(tmp_brf));
	get_zd_data("101C",tmp_brf);  
	vtcp_log("[%s][%d] ժҪ0000001=[%c] ",__FILE__,__LINE__,tmp_brf);
	if(strlen(tmp_brf)==0)
	{
		memset(tmp_brf,'\0',sizeof(tmp_brf));
		memcpy(tmp_brf,tmp_brf1,sizeof(tmp_brf));
	}
	if(g_pub_tx.ct_ind[0]=='1')
	{
		pub_base_dic_show_str(g_pub_tx.brf,"cash_imemo",tmp_brf);
		/*add by lwb 20150123*/
		if(memcmp(tmp_brf,"031",3)==0)	/* 031��ʾ����ժҪ */
		{
			get_zd_data("0330",g_pub_tx.brf);
		}
	}
	else
	{
		pub_base_dic_show_str(g_pub_tx.brf,"tran_memo",tmp_brf);
		if(memcmp(tmp_brf,"031",3)==0)	/* 031��ʾ����ժҪ */
		{
			get_zd_data("0330",g_pub_tx.brf);
		}
	}
	vtcp_log("[%s][%d] ժҪ=[%s] ",__FILE__,__LINE__,g_pub_tx.brf);
	if( strlen(g_pub_tx.brf)==0 )
	{
		strcpy( g_pub_tx.brf,"���" );
	}
	get_zd_double("1013",&g_pub_tx.tx_amt1);

	if(g_pub_tx.ct_ind[0]=='2')
	{
		strcpy( memo,"ת��" );
	}
	else strcpy( memo,"���" );

	sprintf(acErrMsg,"������[%lf],!!!brf[%s]",g_pub_tx.tx_amt1,g_pub_tx.brf);
	WRITEMSG
	
	if(!memcmp(g_pub_tx.tx_code,"2409",4))  /*add by lwb 20150508*/
			get_zd_data("1016",g_pub_tx.brf);
	sprintf(acErrMsg,"aaaa brf[%s]",g_pub_tx.brf);
	WRITEMSG

	get_zd_data("1017",g_pub_tx.prt_ind);
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ac_wrk_ind,"100");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=1001;
	get_zd_data("0191",g_pub_tx.using_ind);
	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
    if (g_pub_tx.ct_ind[0] != '1')
    {
        strcpy(g_pub_tx.ct_ind,"2");
    }
	sprintf(acErrMsg,"BEG D099[%s][%s][%s][%d]",g_pub_tx.ac_no,
		g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
	WRITEMSG
	/** ���� **/

	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("g_pub_tx.prdt_code=============[%s]",g_pub_tx.prdt_code);

	/***Part For Print By Azz***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
	if(g_pub_tx.prdt_code[0]=='1')
	{
		/*** ȡ֤����Ϣ ***/
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_dd_mst.cif_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ÿͻ�������֤����Ϣ!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ����");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}

		pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

		ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�޴˻��ڲ���!");
			WRITEMSG
			goto ErrExit;
		}

		pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
		pub_base_strpack(br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(g_dd_parm.title);
		pub_base_strpack(g_dd_mst.name);
		set_zd_data("1018", g_dd_mst.name);
		set_zd_double("1019", g_dd_mst.bal);
		if(  pub_base_prdt_accno( g_dd_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );
		
		/***�˴����ӷ��ؽ����к� add by wanglei 20060920***/
		vtcp_log("[%s][%d] pub_txbrno =%s,rate=%lf\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_dd_mst.rate);
		/***ȡ������***/
		ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
		if( ret)
		{
			sprintf(acErrMsg,"ȡ���ʴ���!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"CKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|\n",
		g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
		g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.cif_no,0,0,"",
		0.00,g_dd_mst.bal,"","","","","",memo,"",0.00,ct_name,
		g_pub_tx.ac_seqn,g_td_mst.ic_date,g_mdm_ac_rel.prt_line,
		g_dd_mst.prdt_no,g_td_mst.book_line,g_pub_tx.brf,"","","",
		id_name,g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
		"",acc_no,g_pub_tx.tx_br_no,"","",sCom_rate.rate_val);
	}
	else if(g_pub_tx.prdt_code[0]=='2')
	{
		
		/*** ȡ֤����Ϣ ***/
		ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
								  "cif_no=%ld", g_td_mst.cif_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�ÿͻ�������֤����Ϣ!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ����");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}

		pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret )
		{
			sprintf(acErrMsg,"�޴˶��ڲ���!");
			WRITEMSG
			goto ErrExit;
		}

		/***���ش��ںʹ��ڵ�λ  add by wanglei 20060920***/
		sprintf(term,"%02d",g_td_parm.term);
		
		if(g_td_parm.term_type[0] == 'Y')
			strcpy(term_type,"��");
		else if(g_td_parm.term_type[0] == 'M')
			strcpy(term_type,"��");
		else if(g_td_parm.term_type[0] == 'D')
			strcpy(term_type,"��");

		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(br_tele);
		pub_base_strpack(g_td_parm.title);
		pub_base_strpack(g_td_mst.name);
		if(  pub_base_prdt_accno( g_td_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );
		/***�˴����ӷ��ؽ����к� add by wanglei 20060920***/	
		vtcp_log("[%s][%d] pub_txbrno =%s,rate=%lf,term=%s,term_type=%s\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_dd_mst.rate,term,term_type);
		fprintf(fp,"CKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|\n",
		g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
		g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.cif_no,0,0,"",
		0.00,g_td_mst.bal,"","","","","",memo,"",0.00,ct_name,
		g_td_mst.ac_seqn,0,0,g_td_mst.prdt_no,0,g_pub_tx.brf,"","","",
		id_name,g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
		"", acc_no,g_pub_tx.tx_br_no,term,term_type,g_td_mst.rate);
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/***FINISH***/

	set_zd_data("101A",g_pub_tx.cur_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		strcpy(g_pub_tx.reply,"D103");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
