/***************************************************************
* �� �� ��: spD002.c
* �������������˺�
*
* ��    ��: jack
* ������ڣ�2004��1��6��
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
#include "card.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "hv_brno_orno_c.h"

spD002()
{
	int ret=0;
	FILE *fp;
	char tmpname[100],term_type[3],draw_type[21],deac_type[11],cur_name[11];
	char ct_name[5],term[3],memo[5],tmp_ac_no[20],tmp_tx_name[21],id_name[21];
	char br_name[31],br_tele[21],auto_depo[9],zc_falg[2];
	struct cif_email_inf_c  	g_cif_email_inf;
	struct cif_id_code_rel_c  	g_cif_id_code_rel;
	struct hv_brno_orno_c  	    g_hv_brno_orno;
	char acc_no[8],mbfebkno[13];
    double amt;		
  memset(zc_falg,0x0,sizeof(zc_falg));
	memset(auto_depo,0x0,sizeof(auto_depo));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
	memset(&g_hv_brno_orno,0x00,sizeof(g_hv_brno_orno));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_data("0670",zc_falg);/*ת�淽ʽ,1:�����Զ�ת���ڣ���Ϣת����;2�������Ϣ�Զ�ת����*/
	get_zd_data("1039",g_pub_tx.ac_no);
	vtcp_log("[%s]",g_pub_tx.ac_no);
	get_zd_data("1031",g_pub_tx.prdt_code);
	get_zd_data("1033",g_pub_tx.cur_no);
	get_zd_data("1034",g_pub_tx.ac_type);
	get_zd_data("1036",g_pub_tx.intst_type);
	get_zd_double("1037",&g_pub_tx.tx_amt1);
	get_zd_data("1038",g_pub_tx.name);
	get_zd_data("103G",g_pub_tx.tmp_ind);	/*** �Ƿ��Զ�ת���־ ***/
	get_zd_data("102O",g_pub_tx.prt_ind);
	strcpy(g_pub_tx.prt_ind,"1");
	vtcp_log("sssssssssss[%s]\n",g_pub_tx.prt_ind);
	vtcp_log("lj------lj[%.2lf]",amt);
	get_zd_long("0440",&g_td_mst.mtr_date);/**����Э��ͬҵ���� ������ add by martin 2009/7/22 11:55:51**/
	vtcp_log("martin -- look g_td_mst.mtr_date=[%ld]",g_td_mst.mtr_date);
	/*****modify by ligl 20060914 ����Э�����ʼ������ʺ�******/
	get_zd_double("0850",&g_pub_tx.rate);
	get_zd_data("0300",g_pub_tx.ac_no1);
	vtcp_log("jack -- look g_pub_tx.ac_no1=[%s]",g_pub_tx.ac_no1);
	/*****end****/
	vtcp_log("jack -- look g_pub_tx.rate=[%lf]",g_pub_tx.rate);
	vtcp_log("jack -- look g_pub_tx.name=[%s]",g_pub_tx.name);

	get_zd_data("103E",g_pub_tx.ct_ind);
	/* get_zd_data("103F",g_pub_tx.cal_code); **ָ����ϵ����**/
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.brf,"����");
	strcpy(g_pub_tx.hst_ind,"1");

	if(g_pub_tx.ct_ind[0]=='2')
		strcpy( memo,"ת��" );
	else 
		strcpy( memo,"����" );

	if( g_pub_tx.tmp_ind[0]=='Y' )
	{
		if(zc_falg[0]=='1')
		{
			strcpy( auto_depo,"�� �����Զ�ת�� ��" );
			strcpy(g_pub_tx.tmp_ind,"P"); /*һ��ͨ�����Զ�ת��tfr_ind��ΪP*/
		}		
		else
		  strcpy( auto_depo,"�� ��Ϣ�Զ�ת�� ��" );
	}
	else strcpy( auto_depo,"");

	get_zd_data("0191",g_pub_tx.using_ind);

	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}
	g_pub_tx.svc_ind=1000;

	vtcp_log("%s,%d ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"��������!");
		WRITEMSG
        if ((strcmp(g_pub_tx.reply,"0000")==0) \
              || (strcmp(g_pub_tx.reply,"")==0) || g_pub_tx.reply[0] == '\0')
        {
            strcpy(g_pub_tx.reply,"D114");
        }
		goto ErrExit;
	}
	
	set_zd_data("1039",g_pub_tx.ac_no);
	set_zd_int("103D",g_pub_tx.ac_seqn);
	if( g_td_parm.prt_no>0 ) set_zd_data("103F","Y");
	else set_zd_data("103F","N");
	sprintf(acErrMsg,"ac_no[%s] [%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG
	
	/*ADD BY AZZ. PART FOR PRINT*/
	get_zd_data("1161",g_pub_tx.mdm_code);	

	if(!strcmp(g_pub_tx.mdm_code,"0014"))
	{
		ret=pub_base_CompDblVal(g_pub_tx.tx_amt1,100000.00);
		if(ret==-1)
		{
			sprintf(acErrMsg,"����������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"A003");
			goto ErrExit;
		}
	}

	/****���ӷ��ش���к� add by wanglei 20071120****/
	ret = Hv_brno_orno_Sel(g_pub_tx.reply,&g_hv_brno_orno,"br_no = '%s' ",g_pub_tx.tx_br_no);
	if(ret )
	{
		sprintf(acErrMsg,"����[%s]��Ӧ��֧���кŲ�����!\n",g_pub_tx.tx_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P099");
		goto ErrExit;
	}
	strcpy(mbfebkno,  g_hv_brno_orno.or_br_no);
	vtcp_log("[%s][%d] ��������Ϊ[%s],����к�Ϊ [%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no,mbfebkno);
	/****ended 20071120****/

   /******add by liuyue  20070208 ����ǿ������ڲ�Ʒ�����������ʻ��Ѿ��������������������****/
    pub_base_strpack(g_pub_tx.ac_no);
    vtcp_log("[%s][%d]ǰ̨������ʺ�==[%s],�ʻ����==[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
    if(!memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) && !memcmp(g_pub_tx.ac_no+6,CARD_UNION_AREA,2))
    {
            ret =  sql_count("dd_mst","ac_id=%ld and ac_sts<>'*' and ac_seqn=1",g_mdm_ac_rel.ac_id);
			if(!ret)
			{
				sprintf(acErrMsg,"��[%s]�����ʺ��Ѿ�����,����������������!\n",g_mdm_ac_rel.ac_no,ret);
				WRITEMSG
				sprintf(acErrMsg,"��[%s]�����ʺ��Ѿ�����,����������������!\n",g_mdm_ac_rel.ac_no,ret);
                set_zd_data("0130",acErrMsg);
                strcpy(g_pub_tx.reply,"HV01");
                goto ErrExit;
	
			}

	}

	
	/**** add by lance using for print �������� ****/

	strcpy( tmp_ac_no , "XXXXXXXXXXXXXXXXXXX" );
	if( g_pub_tx.prdt_code[0]=='1' )
		strcpy( tmp_tx_name , "���ڿ���" );
	else
		strcpy( tmp_tx_name , "���ڿ���" );
	
	/**** end --lance ****/
    pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	pub_base_dic_show_str(cur_name,"cur_name",g_pub_tx.cur_no);
	pub_base_dic_show_str(deac_type,"deac_type",g_pub_tx.ac_type);
	pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
    vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,ct_name);
	if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && g_mdm_ac_rel.draw_id_yn[0]=='N')
	 	strcpy(draw_type,"ƾ��֧ȡ");
	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N' && g_mdm_ac_rel.draw_id_yn[0]=='Y')
	 	strcpy(draw_type,"֤��֧ȡ");
	else if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y' && g_mdm_ac_rel.draw_id_yn[0]=='Y')
	 	strcpy(draw_type,"�����֤��֧ȡ");
	else strcpy(draw_type,"");

	/*���ڲ�Ʒ*/
	if(g_pub_tx.prdt_code[0]=='2'  )
	{
		if(g_td_parm.term_type[0]=='Y')
			strcpy(term_type,"��");
		else if(g_td_parm.term_type[0]=='M')
			strcpy(term_type,"��");
		else if(g_td_parm.term_type[0]=='D')
			strcpy(term_type,"��");
		vtcp_log("[%s] [%d] g_td_parm.term_type=%s term_type=%s\n",__FILE__,__LINE__,g_td_parm.term_type,term_type);
		g_pub_tx.tx_date = g_td_mst.mtr_date;
		g_pub_intst.edu_ind[0]='Y';

		if( g_td_parm.td_type[0]=='4' ) /**֪ͨ��Ԥ����Ϣ**/
		{
			g_pub_intst.dealval=0.00;
			g_pub_intst.factval=0.00;
			g_pub_intst.val=0.00;
			g_pub_intst.keepval=0.00;
			ret=0;
		}
		else
			ret = pub_base_CalIntstTd(&g_pub_intst.dealval,
				&g_pub_intst.factval,&g_pub_intst.val,&g_pub_intst.keepval,
				g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"ȡ������Ϣֵ����");
			WRITEMSG
			strcpy( g_pub_tx.reply,"O194" );
			goto ErrExit;
		}

		g_pub_tx.tx_date = g_td_mst.opn_date;

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

		pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
		pub_base_getbrname(g_td_mst.opn_br_no,br_name);
		pub_base_strpack(br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.name);
		pub_base_strpack(g_td_parm.title);
		pub_base_strpack(g_cif_email_inf.email);

		sprintf(term,"%02d",g_td_parm.term);
		vtcp_log("[%s] [%d] g_td_parm.term=%d term=%s\n",__FILE__,__LINE__,g_td_parm.term,term);
		/***���ӶԶ���ƾ֤�Ĵ��� ���������Ϊ�� add by wanglei 20060922***/
		if(!strcmp(g_td_parm.prdt_no,"241"))
		{
			strcpy(term,""); 
			strcpy(term_type,""); 
		}
		vtcp_log("[%s][%d] term =[%s] term_type=[%s]\n",__FILE__,__LINE__,term,term_type);

		if(  pub_base_prdt_accno( g_td_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );
			
    vtcp_log("[%s][%d],rate=[%lf][%s]",__FILE__,__LINE__,g_td_mst.rate,acc_no);
		fprintf(fp,
		"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
			,g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1
			,g_td_parm.title,g_td_mst.opn_date,g_td_mst.opn_br_no
			,g_td_mst.cif_no,g_td_mst.mtr_date,term,term_type
			,g_td_mst.rate,g_pub_tx.tx_amt1,g_cif_email_inf.email
			,draw_type,deac_type,cur_name,g_pub_tx.tx_code,memo
			,g_pub_tx.ac_no1,g_pub_intst.keepval+g_pub_intst.dealval
			,ct_name,g_pub_tx.ac_seqn,g_td_mst.ic_date
			,g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,0,g_pub_tx.brf
			,tmp_ac_no,tmp_tx_name,g_pub_tx.tel,id_name,
			g_cif_id_code_rel.id_no,br_name,br_tele,g_mdm_ac_rel.note_no,
			auto_depo,acc_no,mbfebkno);
	}	
	else if(g_pub_tx.prdt_code[0]=='1')
	{
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
		if(  pub_base_prdt_accno( g_dd_mst.prdt_no,acc_no ) )
			strcpy( acc_no,"" );

		pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

		pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
		pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
		pub_base_strpack(br_tele);
		pub_base_strpack(br_name);
		pub_base_strpack(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.name);
		pub_base_strpack(g_dd_parm.title);
		pub_base_strpack(g_cif_email_inf.email);
		/***�˴������˻������ʺͿ�Ŀ�ŵķ��� add by wanglei 20060918*******/
		vtcp_log("[%s][%d],rate=[%lf]",__FILE__,__LINE__,g_dd_mst.rate);
		fprintf(fp,
		"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
		,g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1
		,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no
		,g_dd_mst.cif_no,0,"","",g_dd_mst.rate,g_pub_tx.tx_amt1
		,g_cif_email_inf.email,draw_type,deac_type,cur_name
		,g_pub_tx.tx_code,memo,g_pub_tx.ac_no1,0.00,ct_name
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,g_pub_tx.tel,id_name,g_cif_id_code_rel.id_no,
		br_name,br_tele,g_mdm_ac_rel.note_no,"",acc_no,mbfebkno);
	}
	else
	{
		sprintf(acErrMsg,"g_pub_tx.prdt_code[%s]��",g_pub_tx.prdt_code);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/*Add Complete*/
	/*YW 20111028 ��IC���˻�ʱ ���������˻�*/
/**��������IC�������˻����ں���
	if(!strcmp(g_pub_tx.mdm_code,ICCARDMDMCODE))
	{
		ret=pubf_base_IC(1,1);
		if( ret )
		{
			sprintf(acErrMsg,"��IC�����˻�����!");
			WRITEMSG
			goto ErrExit;
		}
	}
**/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���˻��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���˻�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
