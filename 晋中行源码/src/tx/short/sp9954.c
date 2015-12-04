/*************************************************
* �� �� ��: sp9954.c
* ���������������ʺŻ����˻���ż���Ϣ[�Թ�����]
*
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "com_branch_c.h"

sp9954()  
{
	int i=0;
	int ret=0;
	long ac_seqn=0;
	double bal;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[61];
	char tmp_memo[100],prdt_name[31];
	char filename[100],tmpname[100];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct com_branch_c g_com_branch;

	/** ȡֵ����ֵ **/	
	get_zd_data("1011",ac_no);					/*�ʺ�*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/
	
	if(strlen(ac_no)==19)
	{
		sprintf(acErrMsg,"������Թ��ʺ�!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
		goto ErrExit;
	}

	/* add by rob at 20031205 */
	/***
	ret = sql_count("sub_dd_mst", "ac_no='%s' and ac_sts='1'", ac_no);
	if ( ret < 0 )
	{
		sprintf(acErrMsg, "ͳ�����ʺ���Ŀ����!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0 )
	{
		sprintf(acErrMsg, "���ʺ������ʺţ�����2215����");
		WRITEMSG
		strcpy(g_pub_tx.reply, "O264");
		goto ErrExit;
	}
	***/

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	/****Modified by SSH,2006.10.10,�жϵ����ϸ�һЩ****/
	if(g_mdm_ac_rel.mdm_code[2]=='1')
	{
		sprintf(acErrMsg,"������Թ��ʺ�!! [%s]",g_mdm_ac_rel.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O165");
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
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if(g_mdm_ac_rel.ac_seqn==9999)
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret<0)
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==1) 
			ac_seqn=prdt_ac_id_c.ac_seqn;
		else 
		{
			set_zd_long("101E",prdt_ac_id_c.ac_seqn);
		 	ac_seqn=0;

			if(prdt_ac_id_c.prdt_no[0]!='1'&&prdt_ac_id_c.prdt_no[0]!='2')
			{
				sprintf(acErrMsg,"�Ǵ�����˺�!! [%d]",ret);
	 			WRITEMSG
	  			strcpy(g_pub_tx.reply,"O166");
	   			goto ErrExit;
			}
		}
	}
	else if(g_mdm_ac_rel.ac_seqn)
		ac_seqn=g_mdm_ac_rel.ac_seqn;

	/*ȡ��Ϣ*/
	if(ac_seqn||(g_mdm_ac_rel.ac_seqn==0&&prdt_ac_id_c.prdt_no[0]=='2'))
	{
		switch(prdt_ac_id_c.prdt_no[0])
		{
			case '1':	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
										"ac_id=%ld and ac_seqn=%d", 
										prdt_ac_id_c.ac_id,ac_seqn);
						strcpy(name,g_dd_mst.name);
						bal=g_dd_mst.bal;

						if(g_dd_mst.ac_sts[0]=='*')	
						{
							sprintf(acErrMsg,"���˺�������!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"P245");
	   						goto ErrExit;
						}
            if(g_dd_mst.ac_sts[0]=='0')
            {
            	sprintf(acErrMsg,"���˺Ż�ûȷ��!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"T049");
	   						goto ErrExit;
            }
						ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
										"prdt_no='%s'",g_dd_mst.prdt_no);
						strcpy(prdt_name,g_dd_parm.title);
						strcpy(cur_no,g_dd_parm.cur_no);
						strcpy(cif_type,g_dd_parm.cif_type);
						break;
			case '2':	ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
								"ac_id=%ld and ac_seqn=%d", 
								prdt_ac_id_c.ac_id,ac_seqn);
						strcpy(name,g_td_mst.name);
						bal=g_td_mst.bal;

						if(g_td_mst.ac_sts[0]=='*')	
						{
							sprintf(acErrMsg,"���˺�������!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"P245");
	   						goto ErrExit;
						}

						ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
							  "prdt_no='%s'",g_td_mst.prdt_no);

						if(g_td_parm.dpst_ind[0]=='N')	
						{
							sprintf(acErrMsg,"�ö��ڻ����������ҵ��![%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"O181");
	   						goto ErrExit;
						}
						strcpy(prdt_name,g_td_parm.title);
						strcpy(cur_no,g_td_parm.cur_no);
						strcpy(cif_type,g_td_parm.cif_type);
						break;
			default:
						sprintf(acErrMsg,"��Ʒ���ʹ���![%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply, "G102");
						goto ErrExit;
		}

		set_zd_data("101D",cif_type);

		if( cif_type[0]=='1' )
		{
			sprintf(acErrMsg,"������Թ��˺�!! [%d]",cif_type);
		 	WRITEMSG
		  	strcpy(g_pub_tx.reply,"O165");
		   	goto ErrExit;
		}
	}

	/*��ʼ���ز˵�*/
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");

	/***ժҪѡ���б�***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='cash_imemo'");/* �ֽ������ժҪ */
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);
		if(i==0)
		strcpy(tmp_memo,vdic_data.pr_seq);

		fprintf(fp,"101C|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	/*�Թ�ƾ֤��������*/
	i=0;
	ret=Dic_data_Dec_Sel( g_pub_tx.reply, "pr_code='pub_dety'");
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( i==0 && ret==100 ) 
		{	
			strcpy(note_no,"");
			break;
		}
		else if(ret==100) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( !strncmp(vdic_data.pr_seq,"119",3) )  
		{
			vtcp_log("XXXXXvdic_data.pr_seq===[%s]",vdic_data.pr_seq);
			memset(note_no,0x00,sizeof(note_no));
			strcpy(note_no,vdic_data.pr_seq);
		}
		fprintf(fp,"1014|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
		i++;
	}
	Dic_data_Clo_Sel();

	fclose(fp);
	set_zd_data(DC_FILE_SND,"2");
	/*��ϸ�������*/

	/* ������� */
	pub_base_strpack(name);
	vtcp_log("name=[%s],bal=[%lf]",name,bal);
	  set_zd_data("1017",g_dd_mst.ac_type); /*�ʻ�����*/
   	set_zd_data("1018",name); 					/*����*/
   	set_zd_data("101A",cur_no); 				/*����*/
   	set_zd_data("101B","1"); 					/*Ĭ����ת*/
   	set_zd_double("1019",bal); 					/*���*/
   	set_zd_data("1014",note_no); 				/*ƾ֤����*/
   	set_zd_data("1015",g_mdm_ac_rel.note_no); 	/*ƾ֤����*/
   	set_zd_data("101C",tmp_memo);			 	/*ժҪ*/
   	set_zd_data("101F",prdt_name);			 	/*��Ʒ����*/
	set_zd_long("1012",ac_seqn);

	memset(&g_com_branch,0,sizeof(g_com_branch));
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch, "br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"�޴˿�������!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S019");
		goto ErrExit;
	}
	set_zd_data("0250",g_com_branch.br_name);	/* �������� */

	set_zd_data("1011",ac_no);
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
   
