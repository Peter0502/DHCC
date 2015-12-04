/****************************************************
* 文 件 名:    sp9740.c
* 功能描述：   
*              根据帐号和序号回显相关信息
*
* 作    者:    lance
* 完成日期：   2003年03月05日
* 修改记录：   
*     1. 日    期: 20070521
*        修 改 人: wanglei
*        修改内容: 修改户名时,以客户表中的户名为基准
*                  将主表中的名字修改掉
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
	char cId_type[2];   /****证件类型****/
	char cId_no[21];    /****证件号码****/
	char cOld_name[61]; /****原客户名字****/
	char cCif_name[61]; /****客户名字****/
	long lCif_no;       /****客户号****/
	char cRate[9];      /****利率,为前台3511交易返回,且用char型****/
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

	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);			/*帐号*/
	get_zd_int("0340",&ac_seqn);			/*帐号序号*/
	
	/* 调用根据帐号返回账户序号函数 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	strcpy(kt_flag,g_mdm_ac_rel.draw_pwd_chg);
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	if(g_pub_tx.ac_id<900000000)
	{
	    /****帐户修改户名时，校验证件类型、证件号和客户号 add by wanglei 20070521 ****/
	    strcpy(cId_type,g_mdm_ac_rel.id_type);
	    strcpy(cId_no,g_mdm_ac_rel.id_no);
	vtcp_log("[%s][%d] mdm.cif_no = [%s] cid_no = [%s]\n", \
	    		__FILE__,__LINE__,g_mdm_ac_rel.id_no,cId_no); 
	    ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,"id_type = '%s' and id_no = '%s'",cId_type,cId_no);
	    if( ret==100 )
	    {
	    	sprintf(acErrMsg,"证件号与证件类型错误!!id_type=[%s],id_no=[%s]", cId_type,cId_no);
	    	WRITEMSG
	    	strcpy(g_pub_tx.reply,"C020");
	    	goto ErrExit;
	    }
	    lCif_no = g_cif_id_code_rel.cif_no;/****客户号以客户表中为准****/
	    vtcp_log("[%s][%d] id_code.cif_no = [%ld] lCif_no = [%ld]\n", \
	    		__FILE__,__LINE__,g_cif_id_code_rel.cif_no,lCif_no);
	    
	    ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf," cif_no = '%ld'",lCif_no);
	    if( ret==100 )
	    {
	    	sprintf(acErrMsg,"客户号错误!!cif_no=[%ld]", lCif_no);
	    	WRITEMSG
	    	strcpy(g_pub_tx.reply,"C007");
	    	goto ErrExit;
	    }
	    strcpy(cCif_name,g_cif_basic_inf.name);/****户名以客户表中为准****/ 
	    
	    g_pub_tx.cif_no=lCif_no;					/*客户号*/
	    strcpy(g_pub_tx.name,cCif_name);   				/*户名*/
	    vtcp_log("[%s][%d] cifname =[%d]\n",__FILE__,__LINE__,cCif_name);
	    /**** end 2007521****/
	  }

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,ac_seqn);
   	if( ret==100 )
	{
		 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
		 		 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		 case '1':                               			/*1-活期*/
				 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
					 			"ac_id=%ld and ac_seqn=%d",
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 g_pub_tx.tx_amt1=g_dd_mst.bal;						/*余额*/
				 strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);			/*币种*/
				 strcpy(g_pub_tx.prdt_code,g_dd_parm.prdt_no);		/*产品编号*/
				 strcpy(prdt_name,g_dd_parm.title);					/*产品名称*/
				 strcpy(cOld_name,g_dd_mst.name);					/*原户名*/
	vtcp_log("[%s][%d] oldname = [%s] cifname =[%s]\n",__FILE__,__LINE__,cOld_name,cCif_name);
				 strcpy(g_pub_tx.intst_type,g_dd_mst.intst_type);	/*计息类型*/
				struct com_rate_c g_com_rate;
				memset(&g_com_rate,'\0',sizeof(g_com_rate));
				ret=Com_rate_Sel(g_pub_tx.reply,&g_com_rate,"cur_no='01' and rate_code='%s' and end_date>=99999999",g_dd_parm.rate_no);
					if( ret ) goto ErrExit;
				 sprintf(cRate,"%8.5f",(1+g_dd_mst.flt_ratio)*g_com_rate.rate_val); /*返回利率*/
				 
				 vtcp_log(" rate_val[%.5f],cRate = [%s] ",g_com_rate.rate_val,cRate);
				
				 set_zd_data("0880",cRate);   /****传给前台char型利率****/
				char jzrate[10];
				 sprintf(jzrate,"%8.5f",g_com_rate.rate_val); /*返回基准利率*/
				 set_zd_data("0870",jzrate);   /****传给前台char型利率****/
				 set_zd_double( "0560", g_dd_mst.intst_acm );    /* 本金积数 add by martin 2008/12/10 15:18 */	 		
				set_zd_data("0890",kt_flag); /* 开通标志 */

				/***开始下载菜单***/
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
				 /***选项列表下载完毕***/

				 break;
		 case '2':                               				/*2-定期*/
				 ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
					 			"ac_id=%ld and ac_seqn=%d", \
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }

				 g_pub_tx.tx_amt1=g_td_mst.bal;                     /*余额*/
				 strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);          /*币种*/
				 strcpy(g_pub_tx.prdt_code,g_td_parm.prdt_no);		/*产品编号*/
				 strcpy(prdt_name,g_td_parm.title);					/*产品名称*/
				 strcpy(cOld_name,g_td_mst.name);					/*原户名*/
				 strcpy(g_pub_tx.ac_type,"1");				    	/*帐户类型*/
		   	     strcpy(g_pub_tx.intst_type,g_td_parm.intst_type);  /*计息类型*/ 
				 break;
		case '9':
			   ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,
					 			"ac_id=%ld and ac_seqn=%d", \
								g_mdm_ac_rel.ac_id,ac_seqn);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
							 g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
					 WRITEMSG
		 			 strcpy(g_pub_tx.reply,"M003");
		 			 goto ErrExit;
			 	  }

				 ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,
								 "prdt_no='%s'",g_prdt_ac_id.prdt_no);
				 if( ret==100 )
				 {
					 sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
					 		 g_prdt_ac_id.prdt_no);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"P145");
					 goto ErrExit;
				 }
				 g_pub_tx.tx_amt1=g_in_mst.bal;                     /*余额*/
				 strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);          /*币种*/
				 strcpy(g_pub_tx.prdt_code,g_in_parm.prdt_no);		/*产品编号*/
				 strcpy(prdt_name,g_in_parm.prdt_mo);					/*产品名称*/
				 strcpy(g_pub_tx.ac_type,"1");				    	/*帐户类型*/
		   	 strcpy(g_pub_tx.intst_type,g_in_parm.intst_knd);  /*计息类型*/
         /*g_pub_tx.cif_no=g_in_mst.cif_no; */                  /*客户号*/              
         strcpy(cOld_name,g_in_mst.name);                       /*户名*/
         break;
		/* add by yanqq 20120110 贷款户不可做此交易 */
		case '3':
			sprintf(acErrMsg,"此为贷款账户，请到4508交易进行信息维护！");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L412");
			goto ErrExit;
		/* add end yanqq 20120110 */
		default:
				 break;
	}

	vtcp_log("[%s][%d] oldname = [%s] cifname =[%d]\n",__FILE__,__LINE__,cOld_name,cCif_name);
	/* 输出变量 */
	set_zd_data("0250",g_pub_tx.name);          	/*户名*/
	set_zd_data("0260",cOld_name);          	    /*原户名*/
	set_zd_data("0210",g_pub_tx.cur_no);       		/*币种*/ 
	set_zd_data("0230",g_pub_tx.prdt_code);    		/*产品编号*/ 
	set_zd_data("0270",prdt_name); 			   		/*产品名称*/ 
	set_zd_double("0410",g_pub_tx.tx_amt1);         /*余额*/
	set_zd_double("0560",g_dd_mst.intst_acm);         /*积数*/
	set_zd_data("0670",g_pub_tx.intst_type);    	/*计息类型*/
	set_zd_long("0280",g_pub_tx.cif_no);        	/*客户号*/
	set_zd_double( "0850", g_dd_mst.rate);        /*分户利率*/

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




