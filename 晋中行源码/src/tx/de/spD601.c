/***************************************************************
* 文 件 名: spD601.c
* 功能描述：帐户信息维护
* 作    者: mike
* 完成日期：2003年3月9日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"

spD601()
{
	int ret;
	char ac_no[20],ac_type[2],intst_type[2];
	char name[61];
	long tmp_date1,tmp_date2;
	double tmp_amt;
	char kt_flag[2];

	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct dd_mst_c     g_dd_mst;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(kt_flag,0x00,sizeof(kt_flag));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0300",g_pub_tx.ac_no);          	/* 账号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);  			/* 账户序号 */
	get_zd_data("0230",g_pub_tx.prdt_code);  		/* 产品名称 */
	get_zd_data("0680",ac_type);         			/* 账户类型 */
	get_zd_data("0670",intst_type);         		/* 计息类型 */
	get_zd_data("0250",name);         				/* 账户名称 */
	get_zd_data("0710",kt_flag);                    /* 是否开通二级账户标志 */

	pub_base_strpack( name );

	sprintf(acErrMsg,"新的户名是-[%s]",name);
	WRITEMSG

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

	/*** del by xxxxx 2003-12-20
	if( g_pub_tx.ac_no[0]!='5' )
	{
		sprintf(acErrMsg,"查找记录出错[%s]",g_pub_tx.ac_no);
		WRITEMSG
		set_zd_data( DC_GET_MSG,"不可以修改个人户户名，如需要只能销户重开");
		strcpy(g_pub_tx.reply,"BBCL");
		goto ErrExit;
	}
	********/

	/*检查账户状态是否正常*/
	if( g_mdm_ac_rel.note_sts[0]=='*'||g_in_mst.sts[0]=='*' )
	{
	 	sprintf(acErrMsg,"该账户已销户!!");
	  	WRITEMSG
	    strcpy(g_pub_tx.reply,"P172");
	    goto ErrExit;
	}
	
	strcpy(g_mdm_ac_rel.draw_pwd_chg,kt_flag);
	ret=sql_execute("update mdm_ac_rel set draw_pwd_chg='%s' where ac_no='%s'",\
		g_mdm_ac_rel.draw_pwd_chg,g_pub_tx.ac_no);
	if(ret)
	{
		vtcp_log("更新表mdm_ac_rel失败 帐号:[%s],[%s] [%d]",g_pub_tx.ac_no,__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O087");
		goto ErrExit;
	}

	/*根据账户类型到相应的表中作更新*/
	switch( g_pub_tx.prdt_code[0] )
	{
		case '1':                                          /*1-活期*/
				ret=Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", \
								g_pub_tx.prdt_code);
				if( ret==100 )
				{
	 				sprintf(acErrMsg,"产品编号不存在!");
	  				WRITEMSG
					strcpy(g_pub_tx.reply,"M003");
	    			goto ErrExit;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"查找记录出错[%d]",ret);
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
				strcpy( g_dd_mst.name, name );

				if(g_dd_parm.cif_type[0]=='1')
					strcpy(g_dd_mst.sttl_type,ac_type);
				else
				{
					strcpy(g_dd_mst.ac_type,ac_type);

					if(ac_type[0]=='1')							/*** 基本户 ***/
						strcpy(g_dd_mst.cash_ind,"Y");
					else if(ac_type[0]=='2')					/*** 一般户 ***/
						strcpy(g_dd_mst.cash_ind,"N");
					else
						strcpy(g_dd_mst.cash_ind,"Y");
				}
				if(g_mdm_ac_rel.ac_no[0]=='5'){			/* 对公的改储蓄的不改,别柜员做错了 */
					g_dd_mst.intst_type[0]=intst_type[0];
				}

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
				/*定期的也 能改计息方式 wudawei 20140715*/
				strcpy(g_td_mst.intst_type,intst_type);
				/***strcpy( g_td_mst.name, name );del by martin 2008/12/8 14:28***/
				vtcp_log("ic_date=[%ld]",g_td_mst.ic_date);

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
				strcpy( g_in_mst.name, name );/***del by martin 2008/12/8 14:28***/
				vtcp_log("ic_date=[%ld]",g_in_mst.ic_date);
				g_in_mst.intst_knd[0]=intst_type[0];
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
	strcpy( g_pub_tx.brf,"帐户信息维护" );


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
			/*memcpy(nextday,atol(pub_base_daynumLAT(today,1)),8);*/
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
