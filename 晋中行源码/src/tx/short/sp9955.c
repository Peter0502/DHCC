/*************************************************
* �� �� ��: sp9955.c
* ���������������ʺŻ��ԶԹ��ʻ���Ϣ[�Թ�ȡ���]
* ��    ��: rob
* ������ڣ�2003��02��19��
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

sp9955()  
{ 		
	int i=0,ret=0,flag=0,ac_seqn;
	double bal;
	long cif_no=0;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char tmp_memo[100],tmpname[100],prdt_name[31];
	FILE *fp;
	
	vtcp_log("Oooooooooooooooooooooooo");
	struct prdt_ac_id_c prdt_ac_id_c;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	pub_base_sysinit();
	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);					/*�ʺ�*/
 	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/

	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXXac_no=[%s]",ac_no);
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

	/****����Ϊ����Ļ�������ر�־����ʾ��Ա add by wanglei 20070621****/
	ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"repay_ac_id = %ld and ac_sts in('1','2','3','4','5') \
		and (bal >0 or in_lo_intst >0 or out_lo_intst >0 or cmpd_lo_intst >0) ", g_mdm_ac_rel.ac_id);
	if(ret == 0)
	{
		sprintf(acErrMsg,"������ʺ����б������Ϣδ��!!");
		WRITEMSG
		set_zd_data("0710","1"); /****������ʾ��־****/
	}
	/****ended 20070621****/
	
	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXX");
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
					   g_mdm_ac_rel.ac_id);
	if(ret==100)
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

	vtcp_log("XXXXXXXXXXXXXXXXXXXXXXXX");
	if(prdt_ac_id_c.prdt_no[0]=='1')
	{
		ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",prdt_ac_id_c.ac_id);
		if( ret )
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

		strcpy(name,g_dd_mst.name);
		cif_no = g_dd_mst.cif_no;
		bal = g_dd_mst.bal;

		if(g_dd_mst.ac_sts[0]=='*')
		{
			sprintf(acErrMsg,"���˺�������!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P245");
			goto ErrExit;
		}
		/*****add by ligl 2007-1-29 13:09******/
		int ret1=0,ret2=0;
		long days=0;
		ret=pub_base_CalDays(g_dd_mst.opn_date,g_pub_tx.tx_date,"0",&days);
		/******  ���ݹ���Ҫ��  �Թ�����ȥ�� ����3���ڲ���ȡ�������
		if(days<3 && g_dd_mst.bal>0)
		{
  			 ret2=sql_count("ln_mst","repay_ac_id=%ld or save_ac_id=%ld",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_id);
  			 if(ret2==0)
  			 	{
						sprintf(acErrMsg,"���˺ſ�������3�ղ�����ȡ��!! [%d]",ret2);
						WRITEMSG
						strcpy(g_pub_tx.reply,"T067");
						goto ErrExit;
				}
  		
  	}
	********/
  	/********end**********/
    /*if(g_dd_mst.ac_type[0]=='2')
    {
    	sprintf(acErrMsg,"һ����㻧����ȡ��!![%d]",ret);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"T047");
    	goto ErrExit;
    }*/
		ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
						  g_dd_mst.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"read dd_mst error" );
			WRITEMSG
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"���ҳ���!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		strcpy(prdt_name,g_dd_parm.title);
		strcpy(cur_no,g_dd_parm.cur_no);
		strcpy(cif_type,g_dd_parm.cif_type);
	}
	else if(prdt_ac_id_c.prdt_no[0]=='2')	
	{
		sprintf(acErrMsg,"���ʻ���������ҵ��ֻ�ܲ���! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O197");
		goto ErrExit;
	}
	else
	{
		sprintf(acErrMsg,"�Ǵ�ȡ�����˺�! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O166");
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

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply,&g_cif_id_code_rel, \
							   "cif_no=%ld",cif_no);
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

	pub_base_strpack(name);
   	set_zd_data("102C","N");						/*֧������*/
   	set_zd_data("102H",name);						/*����*/
   	set_zd_data("102V",prdt_name);					/*��Ʒ����*/
	set_zd_data("102J",cur_no);						/*����*/
	set_zd_data("102K","1");						/*Ĭ����ת*/
	set_zd_double("102I",bal);						/*���*/
	set_zd_data("1024",g_mdm_ac_rel.note_no);		/*ƾ֤����*/
	set_zd_data("102O",g_dd_mst.ac_type);						/*�ʻ�����*/

	/***��ʼ���ز˵�***/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"w");

	/***���ضԹ�ժҪ�б�  �����ˣ�ͳһͨ����ת��־��ѡժҪ
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
	*/

	/***ƾ֤����ѡ���б�***/
	i=0;
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='pub_taty' order by pr_seq");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		else if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )  
		{
			strcpy(note_no,vdic_data.pr_seq);
			set_zd_data("1023",vdic_data.pr_seq);
		}
		fprintf(fp,"1023|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	/***ѡ���б��������***/

	set_zd_data("1021",ac_no);
  set_zd_data("0680",g_dd_mst.ac_type);
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
   
