/*************************************************
* �� �� ��: sp9730.c
* ���������������ʺŻ����˻���ż���Ϣ[ת�ʷ����]
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
#include "com_item_c.h"
#include "gl_mst_c.h"

sp9730()  
{ 		
	int i=0;
	int ret=0,tag=0;
	long ac_seqn = 0;
	double bal;
	char ac_no[20],cur_no[5],cif_type[2],note_no[20],name[51];
	char ac_type[2],tmp_memo[100],prdt_name[31],tmpname[100];
	FILE *fp;
	
	struct prdt_ac_id_c prdt_ac_id_c;
	struct mdm_attr_c g_mdm_attr;
	struct dic_data_c vdic_data;
	struct com_item_c g_com_item;
	struct gl_mst_c     g_gl_mst;
	memset(ac_type,'\0',sizeof(ac_type));memset(tmp_memo,'\0',sizeof(tmp_memo));
	memset(prdt_name,'\0',sizeof(prdt_name));memset(tmpname,'\0',sizeof(tmpname));
	memset(note_no,0x00,sizeof(note_no));
	/** ȡֵ����ֵ **/	
	get_zd_data("1011",ac_no);					/*�ʺ�*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/
	set_zd_data("1011",ac_no);
vtcp_log("old_new[%s]",ac_no);
	
	/** ��Ŀ���� **/
		/** ȡֵ����ֵ **/	               /**�Ծ��ʺŵĴ���**/
vtcp_log("�ʺ�Ϊ��[%s]",ac_no);	
  i=strlen(ac_no);
	if(i==3 || i==5 || i==7)							/*��Ŀ���ڲ��ʺ�*/
	{
        vtcp_log("�Կ�Ŀ�Ĵ���");
                set_zd_data("0690","0");
	 	/*���ݿ�Ŀ�Ų�ѯ��Ŀ����*/
	 	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_no='%s'",
					  	 ac_no);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]", g_in_parm.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O074");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf( acErrMsg, "��ѯ������Ŀ���쳣!!ret=[%d]", ret );
		 	WRITEMSG
		 	goto ErrExit;
	 	}

		if( g_com_item.acc_knd[0]!='3' )
		{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�����˻������ӿ�Ŀ!!acc_knd=[%s]",g_com_item.acc_knd);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"P406");
		 	goto ErrExit;
		}

	 	/*���ݿ�Ŀ�Ų�ѯ���������*/
	 	ret=Gl_mst_Sel(g_pub_tx.reply,&g_gl_mst,"acc_hrt='%s'",
					   g_com_item.acc_hrt);
	 	if( ret==100 )
	 	{
		 	sprintf(acErrMsg,"�ÿ�Ŀ�Ų�����!!acc_hrt=[%s]",g_com_item.acc_hrt);
		 	WRITEMSG
		 	strcpy(g_pub_tx.reply,"O075");
		 	goto ErrExit;
	 	}
	 	else if( ret )
	 	{
		 	sprintf(acErrMsg,"��ѯ������Ŀ���쳣!!ret=[%d]",ret);
		 	WRITEMSG
		 	goto ErrExit;
	 	}
                if(g_gl_mst.dr_bal<0.001)	g_gl_mst.dr_bal=0-g_gl_mst.dr_bal;/* ��ֹǰ̨core */
                set_zd_double("1203",g_gl_mst.dr_bal);   /*���*/
		/*��Ŀ�������*/
		if(g_com_item.dc_ind[0]=='1')
		{
			set_zd_data("0220","��");			
			set_zd_double("0410",g_gl_mst.dr_bal);

                        set_zd_double("0390",g_gl_mst.dr_bal);  /*��ǰ�跽���*/			
		}
		else if(g_com_item.dc_ind[0]=='2')
		{
			set_zd_data("0220","��");			
			set_zd_double("0410",g_gl_mst.cr_bal);			
	               
                        set_zd_double("0390",g_gl_mst.cr_bal);  /*��ǰ�������*/
         	}
		else if(g_com_item.dc_ind[0]=='0')		/*˫��*/
		{										
			ret=pub_base_CompDblVal(g_gl_mst.dr_bal,g_gl_mst.cr_bal);
			if(ret==1)
			{
				set_zd_data("0220","��");			
				set_zd_double("0410",g_gl_mst.dr_bal);			
			}
			else if(ret==-1)
			{
				set_zd_data("0220","��");
				set_zd_double("0410",g_gl_mst.cr_bal);			
			}
		}
		set_zd_data("0250",g_com_item.acc_name);       /*����*/
		set_zd_data("0210",g_gl_mst.cur_no);           /*����*/
      
                
   /**������FD120�ĸ�ֵ**/
                set_zd_data("1201",ac_no);
                set_zd_data("1202",g_com_item.acc_name); /*����*/
                                                         /*1203������������*/
                set_zd_data("1204",g_gl_mst.cur_no);     /*����*/
                set_zd_data("1205",  "2");          /*��ת��־2��ת 1 ��*/

		/** add.begin.gyf.20060920 Ϊ�˺ͼ���ʻ�ʱ����һ�£��������Ӷ�102�򲿷���Ϣ��ֵ **/
		set_zd_data("1018",g_com_item.acc_name);
if(g_gl_mst.dr_bal<0.001)
	g_gl_mst.dr_bal=0-g_gl_mst.dr_bal;	/* ��������ǰ̨core�� */
    set_zd_double("1019",g_gl_mst.dr_bal);   /*���*/
    set_zd_data("101A",g_gl_mst.cur_no);     /*����*/
    set_zd_data("101B",g_gl_mst.cur_no);     /*��ת��־2��ת 1 ��*/
		strcpy(cif_type,"2");	/** ���Թ������ʹ��� **/
		strcpy(ac_type,cif_type);
		set_zd_data("101D",ac_type);
		strcpy(prdt_name,"��Ŀ��:");
		strcat(prdt_name,g_com_item.acc_name);
		set_zd_data("101F",prdt_name);
vtcp_log("��Ʒ����:[%s] [%s][%d]",prdt_name,__FILE__,__LINE__);
		/** add.end.gyf.20060920 **/
	}else{
	/** �ʻ����� **/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
TRACE

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
	
	if(!strncmp(g_mdm_ac_rel.mdm_code,"0016",4))
	{
		sprintf(acErrMsg,"����һ��ͨ�����������ҵ��!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O209");
		goto ErrExit;
	}

TRACE
	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld order by ac_seqn", \
						  g_mdm_ac_rel.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret ) goto ErrExit;

TRACE
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
		{
TRACE
			ac_seqn=prdt_ac_id_c.ac_seqn;
		}
		else 
		{
TRACE
			ac_seqn=prdt_ac_id_c.ac_seqn;
		/**
			set_zd_long("101E",prdt_ac_id_c.ac_seqn);
		 	ac_seqn = 0;
			tag = 1;
			**/
		}
	}
	else if(g_mdm_ac_rel.ac_seqn)
		ac_seqn = g_mdm_ac_rel.ac_seqn;

TRACE
	/*ȡ��Ϣ*/
	if(tag==0)
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
			/** ����ת�ʶ��ڲ��ʵ�֧�� gyf.20060920 **/
			case '9':	ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,
										"ac_id=%ld and ac_seqn=%d", 
										prdt_ac_id_c.ac_id,ac_seqn);
						strcpy(name,g_in_mst.name);
						bal=g_in_mst.bal;

						if(g_in_mst.sts[0]=='*')	
						{
							sprintf(acErrMsg,"���˺�������!! [%d]",ret);
	 						WRITEMSG
	  						strcpy(g_pub_tx.reply,"P245");
	   						goto ErrExit;
						}

						ret = In_parm_Sel(g_pub_tx.reply,&g_in_parm, \
							  			 "prdt_no='%s'",g_in_mst.prdt_no);

						strcpy(prdt_name,g_in_parm.prdt_mo);
						strcpy(cur_no,g_in_parm.cur_no);
						strcpy(cif_type,"2");	/** ���Թ������� **/
						break;
			default:
						sprintf(acErrMsg,"�Ǵ�ȡ�����ʺ�![%d]",ret);
						WRITEMSG
						strcpy(g_pub_tx.reply, "O166");
						goto ErrExit;
		}
TRACE

		strcpy(ac_type,cif_type);
		set_zd_data("101D",cif_type);
  }
		/*��ʼ���ز˵�*/
		pub_base_AllDwnFilName( tmpname );
		fp=fopen(tmpname,"a");

TRACE
		if(ac_type[0]=='2')
		{
			/***ժҪѡ���б�***/
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
				strcpy(tmp_memo,vdic_data.pr_seq);

				fprintf(fp,"101C|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
TRACE

		if(strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE))
		{
			ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,
						  	"mdm_code='%s'",g_mdm_ac_rel.mdm_code);
			if( ret )
			{
				 sprintf(acErrMsg,"��ѯ�������Ա����![%d]",ret);
				 WRITEMSG
				 strcpy(g_pub_tx.reply, "B004");
				 goto ErrExit;
			}
			strcpy(note_no,g_mdm_attr.note_type);

			/***ƾ֤����ѡ���б�***/
			ret=Dic_data_Dec_Sel( g_pub_tx.reply, \
								"pr_code='per_dety' and pr_seq='%s'",note_no);
			if( ret ) goto ErrExit;

			while(1)
			{
				ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
				if( ret==100 ) break;
				if( ret ) goto ErrExit;

				pub_base_strpack(vdic_data.pr_seq);
				pub_base_strpack(vdic_data.content);

				fprintf(fp,"1014|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
				i++;
			}
			Dic_data_Clo_Sel();
		}
		else 
		{
			if(ac_type[0]=='2')			/*�Թ�ƾ֤��������*/
			{
				/***ѡ���б�***/
				i=0;
				ret=Dic_data_Dec_Sel( g_pub_tx.reply, "pr_code='pubcomdety'");
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

					if( !strncmp(vdic_data.pr_seq,"148",3) )  
					{
						memset(note_no,0x00,sizeof(note_no));
						strcpy(note_no,vdic_data.pr_seq);
					}
					if(i==0)
					set_zd_data("1014",vdic_data.pr_seq);

					fprintf(fp,"1014|%-20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
					i++;
				}
				Dic_data_Clo_Sel();
			}
		}
TRACE
		fclose(fp);
		set_zd_data(DC_FILE_SND,"2");
		/*��ϸ�������*/

		/* ������� */
		pub_base_strpack(name);
		vtcp_log("name=[%s],bal=[%lf]",name,bal);
   		set_zd_data("1018",name); 					/*����*/
   		set_zd_data("101A",cur_no); 				/*����*/
   		set_zd_data("101B","1"); 					/*Ĭ����ת*/
if(bal<0.001)
	bal=0-bal;											/* ��ֹǰ̨down�� */
   		set_zd_double("1019",bal); 					/*���*/
   		set_zd_data("1014",note_no); 				/*ƾ֤����*/
   		set_zd_data("1015",g_mdm_ac_rel.note_no); 	/*ƾ֤����*/
   		set_zd_data("101C",tmp_memo);			 	/*ժҪ*/
   		set_zd_data("101F",prdt_name);			 	/*��Ʒ����*/
		set_zd_long("1012",ac_seqn);					/*�ʻ����*/
vtcp_log("��Ʒ����:[%s] [%s][%d]",prdt_name,__FILE__,__LINE__);
	}
TRACE

	double tmp_bal=0;
	get_zd_double("0410",&tmp_bal);
	if(tmp_bal<0){
		vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n",tmp_bal);
		tmp_bal=0;
		set_zd_double("0410",tmp_bal);
	}
	tmp_bal=0;
	get_zd_double("0390",&tmp_bal);
	if(tmp_bal<0){
		vtcp_log("!!!!!!ע��  ���[%.2f]С��0\n",tmp_bal);
		tmp_bal=0;
		set_zd_double("0390",tmp_bal);
	}


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
   
