/****************************************************
* �� �� ��:    sp9740.c
* ����������   
*              �����ʺź���Ż��������Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��03��05��
* �޸ļ�¼��   
*     1. ��    ��: 20070521
*        �� �� ��: wanglei
*        �޸�����: �޸Ļ���ʱ,�Կͻ����еĻ���Ϊ��׼
*                  �������е������޸ĵ�
*****************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "com_rate_c.h"

sp9740()  
{ 		
	int i=0,ret=0,ac_seqn;
	char title[40];
	char ac_no[20];
	char prdt_name[40];
	char tmpname[40];
	char deac_type[40];
	char cId_type[2];   /****֤������****/
	char cId_no[21];    /****֤������****/
	char cOld_name[61]; /****ԭ�ͻ�����****/
	char cCif_name[61]; /****�ͻ�����****/
	long lCif_no;       /****�ͻ���****/
	char cRate[9];      /****����,Ϊǰ̨3511���׷���,����char��****/
	double bal;
	FILE *fp;
	char kt_flag[2];

	struct prdt_ac_id_c g_prdt_ac_id;
	struct dic_data_c 	vdic_data;
	struct cif_basic_inf_c g_cif_basic_inf;
	struct cif_id_code_rel_c g_cif_id_code_rel;

	memset(&vdic_data,0x00,sizeof(struct dic_data_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(cRate,0x00,sizeof(cRate));
	memset(kt_flag,0x00,sizeof(kt_flag));

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);			/*�ʺ�*/
	get_zd_int("0340",&ac_seqn);			/*�ʺ����*/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	strcpy(kt_flag,g_mdm_ac_rel.draw_pwd_chg);
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	if(g_pub_tx.ac_id<900000000)
	{
	    /****�ʻ��޸Ļ���ʱ��У��֤�����͡�֤���źͿͻ��� add by wanglei 20070521 ****/
	    strcpy(cId_type,g_mdm_ac_rel.id_type);
	    strcpy(cId_no,g_mdm_ac_rel.id_no);
	vtcp_log("[%s][%d] mdm.cif_no = [%s] cid_no = [%s]\n", \
	    		__FILE__,__LINE__,g_mdm_ac_rel.id_no,cId_no); 
	    ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,"id_type = '%s' and id_no = '%s'",cId_type,cId_no);
	    if( ret==100 )
	    {
	    	sprintf(acErrMsg,"֤������֤�����ʹ���!!id_type=[%s],id_no=[%s]", cId_type,cId_no);
	    	WRITEMSG
	    	strcpy(g_pub_tx.reply,"C020");
	    	goto ErrExit;
	    }
	    lCif_no = g_cif_id_code_rel.cif_no;/****�ͻ����Կͻ�����Ϊ׼****/
	    vtcp_log("[%s][%d] id_code.cif_no = [%ld] lCif_no = [%ld]\n", \
	    		__FILE__,__LINE__,g_cif_id_code_rel.cif_no,lCif_no);
	    
	    ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf," cif_no = '%ld'",lCif_no);
	    if( ret==100 )
	    {
	    	sprintf(acErrMsg,"�ͻ��Ŵ���!!cif_no=[%ld]", lCif_no);
	    	WRITEMSG
	    	strcpy(g_pub_tx.reply,"C007");
	    	goto ErrExit;
	    }
	    strcpy(cCif_name,g_cif_basic_inf.name);/****�����Կͻ�����Ϊ׼****/ 
	    
	    g_pub_tx.cif_no=lCif_no;					/*�ͻ���*/
	    strcpy(g_pub_tx.name,cCif_name);   				/*����*/
	    vtcp_log("[%s][%d] cifname =[%d]\n",__FILE__,__LINE__,cCif_name);
	    /**** end 2007521****/
	  }

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,ac_seqn);
   	if( ret==100 )
	{
		 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 		 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		 case '1':                               			/*1-����*/
				 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
					 			"ac_id=%ld and ac_seqn=%d",
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 g_pub_tx.tx_amt1=g_dd_mst.bal;						/*���*/
				 strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);			/*����*/
				 strcpy(g_pub_tx.prdt_code,g_dd_parm.prdt_no);		/*��Ʒ���*/
				 strcpy(prdt_name,g_dd_parm.title);					/*��Ʒ����*/
				 strcpy(cOld_name,g_dd_mst.name);					/*ԭ����*/
	vtcp_log("[%s][%d] oldname = [%s] cifname =[%s]\n",__FILE__,__LINE__,cOld_name,cCif_name);
				 strcpy(g_pub_tx.intst_type,g_dd_mst.intst_type);	/*��Ϣ����*/
				struct com_rate_c g_com_rate;
				memset(&g_com_rate,'\0',sizeof(g_com_rate));
				ret=Com_rate_Sel(g_pub_tx.reply,&g_com_rate,"cur_no='01' and rate_code='%s' and end_date>=99999999",g_dd_parm.rate_no);
					if( ret ) goto ErrExit;
				 sprintf(cRate,"%8.5f",(1+g_dd_mst.flt_ratio)*g_com_rate.rate_val); /*��������*/
				 
				 vtcp_log(" rate_val[%.5f],cRate = [%s] ",g_com_rate.rate_val,cRate);
				
				 set_zd_data("0880",cRate);   /****����ǰ̨char������****/
				char jzrate[10];
				 sprintf(jzrate,"%8.5f",g_com_rate.rate_val); /*���ػ�׼����*/
				 set_zd_data("0870",jzrate);   /****����ǰ̨char������****/
				 set_zd_double( "0560", g_dd_mst.intst_acm );    /* ������� add by martin 2008/12/10 15:18 */	 		
				set_zd_data("0890",kt_flag); /* ��ͨ��־ */

				/***��ʼ���ز˵�***/
				pub_base_AllDwnFilName( tmpname );
				fp=fopen(tmpname,"a");

				if(g_dd_parm.cif_type[0]=='1')
				{
					ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='deac_type'");
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
							set_zd_data("0680",g_dd_mst.sttl_type);
							strcpy(deac_type,vdic_data.pr_seq);
						}

						fprintf(fp,"0680|%20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
						i++;
					}
					Dic_data_Clo_Sel();
				 }
				 else
				 {
					ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='ac_type'");
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
							set_zd_data("0680",g_dd_mst.ac_type);
							strcpy(deac_type,vdic_data.pr_seq);
						}

						fprintf(fp,"0680|%20s|%s|\n",vdic_data.content,vdic_data.pr_seq);
						i++;
					}
					Dic_data_Clo_Sel();
				 }
				 fclose(fp);
				 set_zd_data(DC_FILE_SND,"2");
				 /***ѡ���б��������***/

				 break;
		 case '2':                               				/*2-����*/
				 ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
					 			"ac_id=%ld and ac_seqn=%d", \
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 g_pub_tx.tx_amt1=g_td_mst.bal;                     /*���*/
				 strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);          /*����*/
				 strcpy(g_pub_tx.prdt_code,g_td_parm.prdt_no);		/*��Ʒ���*/
				 strcpy(prdt_name,g_td_parm.title);					/*��Ʒ����*/
				 strcpy(cOld_name,g_td_mst.name);					/*ԭ����*/
				 strcpy(g_pub_tx.ac_type,"1");				    	/*�ʻ�����*/
		   	     strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);  /*��Ϣ����*/ 
				 break;
		case '9':
			   ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,
					 			"ac_id=%ld and ac_seqn=%d", \
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }
				 g_pub_tx.tx_amt1=g_in_mst.bal;                     /*���*/
				 strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);          /*����*/
				 strcpy(g_pub_tx.prdt_code,g_in_parm.prdt_no);		/*��Ʒ���*/
				 strcpy(prdt_name,g_in_parm.prdt_mo);					/*��Ʒ����*/
				 strcpy(g_pub_tx.ac_type,"1");				    	/*�ʻ�����*/
		   	 strcpy(g_pub_tx.intst_type,g_in_parm.intst_knd);  /*��Ϣ����*/
         /*g_pub_tx.cif_no=g_in_mst.cif_no; */                  /*�ͻ���*/              
         strcpy(cOld_name,g_in_mst.name);                       /*����*/
         break;
		/* add by yanqq 20120110 ����������˽��� */
		case '3':
			sprintf(acErrMsg,"��Ϊ�����˻����뵽4508���׽�����Ϣά����");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L412");
			goto ErrExit;
		/* add end yanqq 20120110 */
		default:
				 break;
	}

	vtcp_log("[%s][%d] oldname = [%s] cifname =[%d]\n",__FILE__,__LINE__,cOld_name,cCif_name);
	/* ������� */
	set_zd_data("0250",g_pub_tx.name);          	/*����*/
	set_zd_data("0260",cOld_name);          	    /*ԭ����*/
	set_zd_data("0210",g_pub_tx.cur_no);       		/*����*/ 
	set_zd_data("0230",g_pub_tx.prdt_code);    		/*��Ʒ���*/ 
	set_zd_data("0270",prdt_name); 			   		/*��Ʒ����*/ 
	set_zd_double("0410",g_pub_tx.tx_amt1);         /*���*/
	set_zd_double("0560",g_dd_mst.intst_acm);         /*����*/
	set_zd_data("0670",g_pub_tx.intst_type);    	/*��Ϣ����*/
	set_zd_long("0280",g_pub_tx.cif_no);        	/*�ͻ���*/
	set_zd_double( "0850", g_dd_mst.rate);        /*�ֻ�����*/

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




