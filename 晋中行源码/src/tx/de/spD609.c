/***************************************************************
* 文 件 名: spD609.c
* 功能描述: 帐户证件信息维护
* 说    明: 在更改帐户证件号码的同时,也将帐户的客户号更改
* 作    者: wanglei
* 完成日期: 2007年5月22日
*
* 修改记录:
* 日    期:2008/12/8 13:33
* 修 改 人:martin
* 修改内容:更新主表客户号，户名  帐户证件号码
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"

spD609()
{   
	int ret; 
	char ac_no[20],ac_type[2];
	char name[61];
	char cId_type[2];
	char cId_no[21];
	char cAc_id_type[2];
	long lCif_no;

	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_c     g_dd_mst;
	struct cif_id_code_rel_c  g_cif_id_code_rel;
	struct cif_basic_inf_c    g_cif_basic_inf;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0200",cId_type);  					     /* 证件类型 */
	get_zd_data("0250",name);                    /* 帐户名称 */
	get_zd_long("0280",&lCif_no);                /* 客户号 */
	get_zd_data("0300",g_pub_tx.ac_no);          /* 账号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			 /* 账户序号 */
	get_zd_data("0620",cId_no);         			   /* 证件号码 */

	/*根据账号查找账号ID*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'", \
						 g_pub_tx.ac_no);
  	if( ret==100 )
  	{
	   sprintf(acErrMsg,"介质账户对照表中不存在该记录");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"W010");
	   goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	
	/*检查账户状态是否正常*/
	if( g_mdm_ac_rel.note_sts[0]=='*'||g_in_mst.sts[0]=='*' )
	{
	 	sprintf(acErrMsg,"该账户已销户!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"P172");
	    goto ErrExit;
	}
	/****得到客户表中的客户号
	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,"id_type = '%s' and id_no = '%s'", \
			cId_type,cId_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"证件号与证件类型错误! id_type=[%s] id_no=[%s]",cId_type,cId_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找证件客户号对照表出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	lCif_no = g_cif_id_code_rel.cif_no;
	vtcp_log("[%s][%d] cif.cif_no=[%ld] lCif_no=[%ld]\n",__FILE__,__LINE__,g_cif_id_code_rel.cif_no,lCif_no);
	
	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no = %ld", lCif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户号不存在! cif_no=[%ld]",lCif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找客户基本信息表记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}****/

	ret=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply,"ac_no= '%s'", g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中无此记录");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W010");
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
	strcpy(g_mdm_ac_rel.id_no,cId_no);
	strcpy(g_mdm_ac_rel.id_type,cId_type);

	ret = Mdm_ac_rel_Upd_Upd( g_mdm_ac_rel,g_pub_tx.reply );
	if( ret )
	{
		sprintf(acErrMsg,"Update Error!");
		WRITEMSG
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();
	
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐户与产品对照表记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	strcpy(cAc_id_type,g_prdt_ac_id.ac_id_type);
	vtcp_log("[%s][%d] ac_id_type = [%s] cAc_id_type = [%s]\n",__FILE__,__LINE__, \
		g_prdt_ac_id.ac_id_type,cAc_id_type);
	/*根据账户类型到相应的表中作更新*/
	switch( cAc_id_type[0] )
	{
		case '1':                                          /*1-活期*/
				ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id = %ld  and ac_seqn = %d", \
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"帐户不存在!");
	  				WRITEMSG
					strcpy(g_pub_tx.reply,"M003");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找活期主表出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				
				ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				
				ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"活期文件中无此记录");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_dd_mst.ac_id;
				g_dd_mst.cif_no=lCif_no;
				strcpy(g_dd_mst.name,name);
				vtcp_log("[%s][%d] dd.cif_no = [%ld] lCif_no =[%ld] \n",__FILE__,__LINE__,g_dd_mst.cif_no,lCif_no);
	/*根据账户类型到相应的表中作更新*/

				ret = Dd_mst_Upd_Upd( g_dd_mst,g_pub_tx.reply );
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}

				Dd_mst_Clo_Upd();
				break;
	case '2':												/*定期*/
				ret=Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id = %ld  and ac_seqn = %d", \
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"帐户不存在!");
	  				WRITEMSG
					strcpy(g_pub_tx.reply,"M003");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找定期主表出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				
				ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				ret = Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"活期文件中无此记录");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_td_mst.ac_id;
				g_td_mst.cif_no=lCif_no;
				strcpy(g_td_mst.name,name);
				vtcp_log("ic_date=[%ld]",g_td_mst.ic_date);
				vtcp_log("[%s][%d] td.cif_no = [%ld] lCif_no =[%ld] \n",__FILE__,__LINE__,g_dd_mst.cif_no,lCif_no);

				ret = Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}

				Td_mst_Clo_Upd();
				break;
		case '9':
				ret=In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id = %ld  and ac_seqn = %d", \
								g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"帐户不存在!");
	  				WRITEMSG
					strcpy(g_pub_tx.reply,"M003");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找内部主表出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				
				ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
									g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
				if( ret )
				{
	 				sprintf(acErrMsg,"DECLARE CURSOR ERROR!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				ret = In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"活期文件中无此记录");
	  				WRITEMSG
	    			strcpy(g_pub_tx.reply,"P172");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				g_pub_tx.ac_id=g_in_mst.ac_id;
				strcpy(g_in_mst.name,name);
				vtcp_log("ic_date=[%ld]",g_in_mst.ic_date);
				vtcp_log("[%s][%d] in.cif_no = [%ld] lCif_no =[%ld] \n",__FILE__,__LINE__,g_dd_mst.cif_no,lCif_no);
				ret = In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
				if( ret )
				{
	 				sprintf(acErrMsg,"Update Error!");
	  				WRITEMSG
	    			goto ErrExit;
				}
				In_mst_Clo_Upd();
				break;
		default:
				break;
	} 
	/*switch结束*/


    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}

	 /* add begin lzy 20150108 */
			char today[9]={0};
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);

			if ( g_pub_tx.ac_no[0]=='5' )
				{
					sprintf( filename,"CorpAcctSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::账户为[%s],不需要与宇信同步.",__FILE__,__LINE__, g_pub_tx.ac_no );
					goto OkExit;
				}
			
			sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
			vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
			fp=fopen(pcfilename,"at");
			if( fp==NULL )
			{
				vtcp_log("[%s],[%d] 创建新文件[%s]",__LINE__,__FILE__,pcfilename);
				fp = fopen(pcfilename, "wt");
				if (fp == NULL)
				{
					vtcp_log("[%s],[%d] open file[%s] error",__LINE__,__FILE__,pcfilename);
					strcpy(g_pub_tx.reply,"SM83");
					goto ErrExit;
				}
			}
				/**fprintf(fp, "账号,户名\n");**/
				fprintf(fp, "%s,%s\n",g_pub_tx.ac_no,name);
				fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp文件[%s]到网银中台出错[%d]", __FILE__, __LINE__,filename,ret);
				}	
			vtcp_log("[%s][%d] ftp文件[%s]到网银中台成功", __FILE__, __LINE__,filename);				
		  /* add end lzy 20150108 */


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"帐户信息维护成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"帐户信息维护失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
