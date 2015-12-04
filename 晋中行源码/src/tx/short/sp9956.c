/*************************************************
* �� �� ��: sp9956.c
* ���������������ʺŻ����˻���ż���Ϣ[�Թ����ڲ��ύ��]
* ��    ��: rob
* ������ڣ�2003��06��10��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#include "public.h"
#include "prdt_ac_id_c.h"

#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "mo_infrm_c.h"

sp9956()  
{ 		
	int i=0,ret=0,ac_seqn;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61];
	char ac_type[2],tmp_memo[100],tmpname[100],prdt_name[31];
	char spe_ind[21];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct mo_infrm_c g_mo_infrm;

	memset( &g_mo_infrm, 0x00, sizeof(struct mo_infrm_c) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);						/*�ʺ�*/
	get_zd_data("102M",ac_type);					/*�ʻ�����*/
	pub_base_old_acno( ac_no );                 	/**�Ծ��ʺŵĴ���**/
	
	/*** �Թ������ ***/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����������ᣬ���������˺�!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M017");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý�����������!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	vtcp_log("CCCCC[%s]",g_mdm_ac_rel.mdm_code);
	if(!memcmp(g_mdm_ac_rel.mdm_code,"0008",4))
	{
		sprintf(acErrMsg,"�����붨��֤ʵ���˻�!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O225");
		goto ErrExit;
	}

	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
					     g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** ���ڻ� ***/
	if( prdt_ac_id_c.prdt_no[0]=='2')
	{
		ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
						 prdt_ac_id_c.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"read td_mst [%d]",prdt_ac_id_c.ac_id);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"�����ʺų���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if(g_td_mst.ac_sts[0]=='*')
		{
			sprintf(acErrMsg,"���˺�������!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}

		ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
						  g_td_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"�����ʺų���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		/*** ֪ͨ�����֪ͨ�ǼǱ� ***/
		if( g_td_parm.td_type[0]=='4' )
		{
			ret = Mo_infrm_Sel(g_pub_tx.reply,&g_mo_infrm,"ac_id=%ld and \
							   ac_seqn=%d and sts='0'", g_td_mst.ac_id, \
							   g_td_mst.ac_seqn );
			if( ret==100 )
				strcpy( spe_ind ,"0001" );			/*** δ�Ǽ�֧ȡ ***/
			else if( ret==0 )
			{
				if( g_pub_tx.tx_date < g_mo_infrm.prm_draw_date )
				strcpy( spe_ind , "0002");			/*** ��ǰ֧ȡ ***/
				if( g_pub_tx.tx_date > g_mo_infrm.prm_draw_date )
				strcpy( spe_ind , "0003");			/*** ����֧ȡ ***/
			}
			else if( ret )
			{
				sprintf(acErrMsg,"read dd_mst error" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
		}

		strcpy(name,g_td_mst.name);
		cif_no=g_td_mst.cif_no;
		bal=g_td_mst.bal;
		strcpy(prdt_name,g_td_parm.title);
		strcpy(cur_no,g_td_parm.cur_no);
		strcpy(cif_type,g_td_parm.cif_type);
	}
	else
	{
		sprintf(acErrMsg,"���ʻ�������������ҵ��![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O193");
		goto ErrExit;
	}

	set_zd_data("102M",cif_type);

	if( cif_type[0]=='1' )
	{
		sprintf(acErrMsg,"������Թ��˺�!! [%d]",cif_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel,"cif_no=%ld", \
								cif_no);
	if( ret==100 )
	{	
		sprintf(acErrMsg,"��ѯcif_id_code_rel����![%d][%ld]",ret,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
						g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{	
		sprintf(acErrMsg,"��ѯ���ʲ��������![%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply, "G102");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/***��Ĭ����ת��־��Ϊ'2' by wanglei 20061017***/
	pub_base_strpack(name);
   	set_zd_data("102C",g_mdm_ac_rel.pwd_mach_yn);	/*֧������*/
   	set_zd_data("102H",name);						/*����*/
   	set_zd_data("102V",prdt_name);					/*��Ʒ����*/
	set_zd_data("102J",cur_no);						/*����*/
	set_zd_data("102K","2");						/*Ĭ����ת*/
	set_zd_double("102I",bal);						/*���*/
	set_zd_data("1023",g_mdm_attr.note_type);		/*ƾ֤����*/
	set_zd_data("1024",g_mdm_ac_rel.note_no);		/*ƾ֤����*/
	set_zd_data("1026",g_mdm_ac_rel.draw_pwd_yn);	/*����֧ȡ*/
	set_zd_data("102U",spe_ind);					/*�����־*/

	/***��ʼ���ز˵�***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	/***���ضԹ�ժҪ�б�***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='tran_memo'");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);
		if(i==0)
		{
			set_zd_data("102L",vdic_data.pr_seq);
			strcpy(tmp_memo,vdic_data.pr_seq);
		}

		fprintf(fp,"102L|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/***ѡ���б��������***/

	set_zd_data("1021",ac_no);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
