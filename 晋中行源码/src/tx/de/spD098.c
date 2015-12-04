/***************************************************************
* �� �� ��: spD098.c
* ��������: ���ӿ��۽���
* ��    ��: jack
* ������ڣ�2004��1��11��
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
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "cif_email_inf_c.h"
#include "cif_id_code_rel_c.h"

spD098()
{
	int ret=0,ac_seqn;
	int i=0,tag=0;
	FILE *fp;
	char tmpname[101];
	char draw_type[15];
	char cur_name[15];
	char deac_type[15];
	char br_name[31];
	char br_tele[21];
	char tmp_mdm_code[5],tmp_ac_no[20],tmp_tx_name[21];
	struct mdm_attr_c old_mdm_attr;
	struct mdm_attr_c new_mdm_attr;
	struct mdm_attr_c tmp_mdm_attr;

	struct cif_email_inf_c  g_cif_email_inf;
	struct prdt_ac_id_c  	g_prdt_ac_id;
	struct cif_id_code_rel_c  	vcif_id_code_rel;

	int crd_num;
	int book_num;

	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&new_mdm_attr,0x00,sizeof(struct mdm_attr_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&vcif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	crd_num=0;
	book_num=0;

	/*** ��ʼ��ȫ�ֱ���***/
	pub_base_sysinit();

	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1168",g_pub_tx.beg_note_no);
	get_zd_data("1168",g_pub_tx.end_note_no);
	get_zd_data("1142",g_pub_tx.draw_pwd_yn);
	get_zd_data("1144",g_pub_tx.draw_pwd);
	get_zd_data("1145",g_pub_tx.draw_id_yn);
	get_zd_data("1148",g_pub_tx.id_type);
	get_zd_data("1146",g_pub_tx.id_no);

	/**** add by jack using for print �������� ****/

	strcpy( tmp_ac_no , g_pub_tx.ac_no );
	
	/**** end --lance ****/
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����˻����ձ��ȡ����");
		WRITEMSG
		goto ErrExit;
	}

	if(g_mdm_ac_rel.note_sts[0]!='0')
	{
		sprintf(acErrMsg,"�ý��ʴ��ڷ�����״̬!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O088");
		goto ErrExit;
	}

	/*���֧ȡ��ʽ�Ƿ�Ϸ�*/
	ret = pub_base_check_draw("001");
	if( ret )
	{
		sprintf(acErrMsg,"���ü��֧ȡ��ʽ��������!!");
   		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&old_mdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ʶ�����в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ʶ�����д���");
		WRITEMSG
		goto ErrExit;
	}

	strcpy(tmp_mdm_code,g_mdm_ac_rel.mdm_code);

	if( old_mdm_attr.mdm_type[0]=='3' || old_mdm_attr.mdm_type[0]=='4'
		|| old_mdm_attr.mdm_type[0]=='5' ) /**��**/
	{
		if( g_mdm_ac_rel.main_ind[0]=='2' )
		{
			sprintf(acErrMsg,"�������������˽���!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O179");
			goto ErrExit;
		}
	}

  	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld]",g_mdm_ac_rel.ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(g_dd_mst.ac_sts[0]!='1')
	{
		sprintf(acErrMsg,"�˻�״̬������!dd_mst.ac_sts='%s'",g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
		goto ErrExit;
	}

	if(g_dd_mst.hold_sts[0]!='0')
	{
		sprintf(acErrMsg,"���˻����ڶ���״̬!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
		goto ErrExit;
	}

	g_pub_tx.cif_no=g_dd_mst.cif_no;

vtcp_log("YYYYYYYYY[%d]",g_dd_mst.cif_no);
	strcpy( g_pub_tx.prdt_code,g_dd_mst.prdt_no );
	ac_seqn=g_dd_mst.ac_seqn;

	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	get_zd_data("1161",g_pub_tx.mdm_code);
	get_zd_data("1162",g_pub_tx.ac_no);
	get_zd_data("1152",g_pub_tx.draw_pwd_yn);
	get_zd_data("1154",g_pub_tx.draw_pwd);
	get_zd_data("1155",g_pub_tx.draw_id_yn);
	get_zd_data("1168",g_pub_tx.beg_note_no);
	strcpy(g_pub_tx.beg_note_no,g_pub_tx.end_note_no);

	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&vcif_id_code_rel,"cif_no=%ld", \
			  				  g_pub_tx.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ͻ��Ų�����!");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_pub_tx.id_type[0] = vcif_id_code_rel.id_type[0];
	g_pub_tx.id_type[1] = '\0';
	strcpy(g_pub_tx.id_no,vcif_id_code_rel.id_no);

sprintf(acErrMsg," ID_TYPE[%s],ID_NO[%s]",g_pub_tx.id_type,g_pub_tx.id_no);
WRITEMSG

	sprintf(acErrMsg,"g_pub_tx.beg_note_no=[%s],cif_no=[%ld]", \
			g_pub_tx.beg_note_no,g_pub_tx.cif_no);
	WRITEMSG

	/*����Ʒ-����-���ʹ�ϵ�Ƿ�ƥ��*/
	ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_pub_tx.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"Declare Error![%d]!",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret=Mdm_ac_rel_Fet_Sel(&g_mdm_ac_rel,g_pub_tx.reply);
		if(ret==100)break;
		else if( ret )
		{
			sprintf(acErrMsg,"Fetch Error![%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}

		if( g_mdm_ac_rel.note_sts[0]!='0' )
			continue;

		/*** ��ȡ�������� ***/
		ret=Mdm_attr_Sel(&g_pub_tx.reply,&tmp_mdm_attr,"mdm_code='%s'", \
					 g_mdm_ac_rel.mdm_code);
    	if( ret==100 )
		{
			sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",ret,g_mdm_ac_rel.mdm_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P132");
			goto ErrExit;
		}
		else if( ret )
		{
			TRACE
			goto ErrExit;
		}

		if( tmp_mdm_attr.mdm_type[0]=='1' )
			book_num++;
		else if( tmp_mdm_attr.mdm_type[0]=='3' 
			|| tmp_mdm_attr.mdm_type[0]=='4' 
			|| tmp_mdm_attr.mdm_type[0]=='5' )
			crd_num++;
vtcp_log(" BOOK OR CRD [%d] [%d]",book_num,crd_num);

		if( pub_base_check_promedi(g_pub_tx.prdt_code,
			g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code,NULL_MDM) )
		{
			sprintf(acErrMsg,"�ý��ʲ��������Ӵ������![%s]!", 
					g_mdm_ac_rel.mdm_code);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O176");
			goto ErrExit;
		}
		i++;
	}

	Mdm_ac_rel_Clo_Sel( );

	/*** ��ȡ�������� ***/
	ret = Mdm_attr_Sel( &g_pub_tx.reply,&new_mdm_attr,"mdm_code='%s'", \
					 	g_pub_tx.mdm_code);
    if( ret==100 )
	{
		sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",ret,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( new_mdm_attr.no_ind[0]=='Y' )					/*�¿�ʱ�Ƿ�ƾ����*/
	{
		strcpy(g_pub_tx.crd_no,g_pub_tx.ac_no);
	}

	/**���俨**/
	if( old_mdm_attr.mdm_type[0]=='1' &&
		(new_mdm_attr.mdm_type[0]=='3' || new_mdm_attr.mdm_type[0]=='4'
		|| new_mdm_attr.mdm_type[0]=='5' ) )
	{
		if( crd_num==0 ) 
		{
			tag=1; /**���������**/
			strcpy( tmp_tx_name , "���俨(����)" );
			strcpy( g_pub_tx.brf, "��������" );
		}else{
			tag=2; /**����Ǹ���**/
			strcpy( tmp_tx_name , "���俨(����)" );	
			strcpy( g_pub_tx.brf, "���丱��" );		
		}
vtcp_log(" TAG A [%d] ",tag);
	}
	/**������**/
	else if( (old_mdm_attr.mdm_type[0]=='3' || old_mdm_attr.mdm_type[0]=='4'
		|| old_mdm_attr.mdm_type[0]=='5' ) &&
		new_mdm_attr.mdm_type[0]=='1' )
	{
		if( book_num>0 ) 
		{
			strcpy( g_pub_tx.reply,"O176" );
			sprintf(acErrMsg,"����ʷ�ʽ����" );
			WRITEMSG
			goto ErrExit;
		}
		tag=0;
		strcpy( tmp_tx_name , "������" );
		strcpy( g_pub_tx.brf, "������" );
vtcp_log(" TAG B [%d] ",tag);
	}
	/**���俨**/
	else if( (old_mdm_attr.mdm_type[0]=='3' || old_mdm_attr.mdm_type[0]=='4'
		|| old_mdm_attr.mdm_type[0]=='5' ) &&
		(new_mdm_attr.mdm_type[0]=='3' || new_mdm_attr.mdm_type[0]=='4'
		|| new_mdm_attr.mdm_type[0]=='5' ) )
	{
		tag=2; /**����Ǹ���**/
		strcpy( tmp_tx_name , "�����丱��" );	
		strcpy( g_pub_tx.brf, "���丱��" );	
vtcp_log(" TAG C [%d] ",tag);
	}
	else
	{
		strcpy( g_pub_tx.reply,"O178" );
		sprintf(acErrMsg,"����ʷ�ʽ����" );
		WRITEMSG
		goto ErrExit;
	}

	/*�����ʺ���*/
	ret = pub_acct_opn_cif_mdm( );
	if( ret )
	{
	  	sprintf(acErrMsg,"����pub_acct_opn_cif_mdm�����ʺ�������!!");
		WRITEMSG
		goto ErrExit;
	}

TRACE
	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"�����α����!ret=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

TRACE
	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�˺Ų����� [%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	}

TRACE
	if( g_pub_tx.ac_seqn==ZERO_SEQN )
		g_mdm_ac_rel.ac_seqn=ac_seqn;
	else
		g_mdm_ac_rel.ac_seqn=g_pub_tx.ac_seqn;

	if( tag==1 )
		strcpy(g_mdm_ac_rel.main_ind,"1");
	else if( tag==2 )
		strcpy(g_mdm_ac_rel.main_ind,"2");
	else
		strcpy(g_mdm_ac_rel.main_ind,"1");

TRACE
	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret )
	{
	 	sprintf(acErrMsg,"�޸ļ�¼����!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd( );

	/***��ӡ�����***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"���ļ�[%s]��",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}

	pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
	pub_base_getbrname(g_dd_mst.opn_br_no,br_name);

	/*��ӡ�������*/
	ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
					  g_dd_mst.prdt_no);
	if( ret )
		goto ErrExit;

	if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='N')
  		strcpy(draw_type,"�� ƾ��֧ȡ ��");
	else if(g_pub_tx.draw_pwd_yn[0]=='N'&&g_pub_tx.draw_id_yn[0]=='Y')
  		strcpy(draw_type,"�� ֤��֧ȡ ��");
 	else if(g_pub_tx.draw_pwd_yn[0]=='Y'&&g_pub_tx.draw_id_yn[0]=='Y')
  		strcpy(draw_type,"�� �����֤��֧ȡ ��");
 	else
	  	strcpy(draw_type,"");

	pub_base_strpack(g_dd_parm.title);
	pub_base_strpack(g_cif_email_inf.email);

	/** del by jack **
	fprintf(fp,"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%d|%s|%lf|%lf|%s|%s|%s|%s|\n",
		g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,
		g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no,
		g_dd_mst.cif_no,0,0,"",0.00,g_pub_tx.tx_amt1,
		g_cif_email_inf.email,draw_type,deac_type,cur_name);
	del end **/
	
	fprintf(fp,
		"CZFY%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
		,g_pub_tx.ac_no,g_dd_mst.name,g_dd_mst.bal
		,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no
		,g_dd_mst.cif_no,0,"","",0.00,g_pub_tx.tx_amt1
		,g_cif_email_inf.email,draw_type,"",""
		,"","����",g_pub_tx.ac_no1,0.00,""
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,g_pub_tx.tel,"","",
		br_name,br_tele,g_mdm_ac_rel.note_no);
	fprintf(fp,
		"CPCZ%s|%s|%lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n"
		,g_pub_tx.ac_no,g_dd_mst.name,g_dd_mst.bal
		,g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no
		,g_dd_mst.cif_no,0,"","",0.00,g_pub_tx.tx_amt1
		,g_cif_email_inf.email,draw_type,"",""
		,"","����",g_pub_tx.ac_no1,0.00,""
		,g_pub_tx.ac_seqn,g_dd_mst.ic_date,g_mdm_ac_rel.prt_line
		,g_dd_mst.prdt_no,0,g_pub_tx.brf
		,tmp_ac_no,tmp_tx_name,g_pub_tx.tel,"","",
		br_name,br_tele,g_mdm_ac_rel.note_no);

	set_zd_data(DC_FILE_SND,"1");
	fclose(fp);
	/***��ӡ�����***/

	/** �������� **/
	set_zd_data("1167",g_pub_tx.ac_no);
	set_zd_int("103D",ac_seqn);

	/***�Ǽǽ�����ˮ***
	if( pub_ins_trace_log() )
	{
	  	sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ӿ��۽��׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ӿ��۽���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
