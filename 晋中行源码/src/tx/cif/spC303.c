/*************************************************
* 文 件 名: spC303.c
* 功能描述：客户基本信息维护
*
* 作    者: 
* 完成日期：2004年02月05日
* 修改记录：   
*     1. 日    期:	2004-05-31
*        修 改 人:	andy
*        修改内容:	增加修改别名、联系电话、联系地址
*     2. 
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_inf_chg_log_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_alias_inf_c.h"

static struct cif_basic_inf_c    g_cif_basic_inf;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_inf_chg_log_c  cif_inf_chg_log;
static struct cif_email_inf_c    cif_email_inf;
static struct cif_addr_inf_c    cif_addr_inf;
static struct cif_alias_inf_c    cif_alias_inf;
static struct cif_email_inf_c    cif_email_inf_tmp;
static struct cif_addr_inf_c    cif_addr_inf_tmp;
static struct cif_alias_inf_c    cif_alias_inf_tmp;

char memo[41],tbwhere[50];

		
spC303()  
{ 	
	int ret=0;
	int cmp_code=0;
	char sts[2];	
	char cName[61];
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 数据初始化 **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&g_cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));	
	memset (&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
	memset (&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));
	memset (&cif_email_inf_tmp, 0x00, sizeof(struct cif_email_inf_c));
	memset (&cif_addr_inf_tmp, 0x00, sizeof(struct cif_addr_inf_c));
	memset (&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset (&cif_alias_inf_tmp, 0x00, sizeof(struct cif_alias_inf_c));
	strcpy( sts , "" );
	memset(cName,0x00,sizeof(cName));
				
	/** 取值、赋值 **/
	get_zd_data("0930",g_cif_basic_inf.sts);			/* 客户当前状态*/
	g_cif_basic_inf.cif_seqn=0;							/* 客户账户序号*/
	get_zd_long("0280",&g_cif_basic_inf.cif_no);		/* 客户号 */
	
	get_zd_data("0720",sts);							/* 操作类型 */
	get_zd_data("0670",g_cif_basic_inf.type);			/* 客户类型 */
	/****
	get_zd_data("0260",g_cif_basic_inf.name);			
	****/
	get_zd_data("0260",cName);			/* 客户名称 */

	get_zd_data("0680",g_cif_basic_inf.local);			/* 本地标志 */
	get_zd_data("0690",g_cif_basic_inf.poten);			/* 潜在标志 */
	get_zd_data("0700",g_cif_basic_inf.shareholder_ind);/* 本行股东 */
	get_zd_data("0710",g_cif_basic_inf.lvl);			/* 客户级别 */
	
	get_zd_data( "0810",cif_email_inf_tmp.email);		/*客户联系电话*/
	get_zd_data( "0980",cif_email_inf_tmp.addr_type);	/*联系方式*/
	get_zd_data( "0820",cif_addr_inf_tmp.addr);			/*客户联系地址*/		

	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*修改*/
				/* 修改客户基本信息表 */
				ret = Cif_basic_inf_Dec_Upd( g_pub_tx.reply,"cif_no=%ld", \
											g_cif_basic_inf.cif_no);
			    if( ret )
	            {
				   sprintf(acErrMsg,"DECLARE Cif_id_code_rel异常[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret = Cif_basic_inf_Fet_Upd(&cif_basic_inf,g_pub_tx.reply);
			    if( ret )
			    {
				   sprintf(acErrMsg,"FETCH Cif_id_code_rel异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }

				g_cif_basic_inf.crt_date=cif_basic_inf.crt_date;
				g_cif_basic_inf.crt_time=cif_basic_inf.crt_time;
				g_cif_basic_inf.cif_seqn=cif_basic_inf.cif_seqn;
				
				
				if( strcmp(g_cif_basic_inf.sts ,cif_basic_inf.sts) && strlen(g_cif_basic_inf.sts)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"sts" ,cif_basic_inf.sts, g_cif_basic_inf.sts, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.sts ,cif_basic_inf.sts,sizeof(g_cif_basic_inf.sts));
					
				if( strcmp(g_cif_basic_inf.type ,cif_basic_inf.type)  && strlen(g_cif_basic_inf.type)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"type" ,cif_basic_inf.type, g_cif_basic_inf.type, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.type ,cif_basic_inf.type,sizeof(g_cif_basic_inf.type));
					

				if( strcmp(cName ,cif_basic_inf.name)  && strlen(cName)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no,"cif_basic_inf" ,"name" ,cif_basic_inf.name, cName, "" );
					memcpy(g_cif_basic_inf.name ,cName,sizeof(g_cif_basic_inf.name));

					if(ret)	ERR_DEAL
				}else
				{
					memcpy(g_cif_basic_inf.name ,cif_basic_inf.name,sizeof(g_cif_basic_inf.name));
				}
				if( strcmp(g_cif_basic_inf.local ,cif_basic_inf.local)  && strlen(g_cif_basic_inf.local)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"local" ,cif_basic_inf.local, g_cif_basic_inf.local, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.local ,cif_basic_inf.local,sizeof(g_cif_basic_inf.local));
				
				if( strcmp(g_cif_basic_inf.poten ,cif_basic_inf.poten)  && strlen(g_cif_basic_inf.poten)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"poten" ,cif_basic_inf.poten, g_cif_basic_inf.poten, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.poten ,cif_basic_inf.poten,sizeof(g_cif_basic_inf.poten));
					
				if( strcmp(g_cif_basic_inf.shareholder_ind ,cif_basic_inf.shareholder_ind) && strlen(g_cif_basic_inf.shareholder_ind)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf( g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"shareholder_ind" ,cif_basic_inf.shareholder_ind, g_cif_basic_inf.shareholder_ind, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.shareholder_ind ,cif_basic_inf.shareholder_ind,sizeof(g_cif_basic_inf.shareholder_ind));
					
				if( strcmp(g_cif_basic_inf.lvl ,cif_basic_inf.lvl)  && strlen(g_cif_basic_inf.lvl)!=0 )
				{
					ret=pub_cif_Ins_Chg_inf(g_cif_basic_inf.cif_no ,"cif_basic_inf" ,"lvl" ,cif_basic_inf.lvl, g_cif_basic_inf.lvl, "" );
					if(ret)	ERR_DEAL
				}else
					memcpy(g_cif_basic_inf.lvl ,cif_basic_inf.lvl,sizeof(g_cif_basic_inf.lvl));
					
			   strcpy(g_cif_basic_inf.rowid,cif_basic_inf.rowid);	

				ret = Cif_basic_inf_Upd_Upd(g_cif_basic_inf,g_pub_tx.reply);
				if( ret )
				{
					sprintf(acErrMsg,"UPDATE Cif_id_code_rel游标异常![%s]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}

				Cif_basic_inf_Clo_Upd( );
				
				ret = sql_count("cif_alias_inf","cif_no=%ld and alias_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT异常![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					/* 修改客户名称(别名信息表) */
					ret = Cif_alias_inf_Dec_Upd(g_pub_tx.reply,
						"cif_no=%ld and alias_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_alias_inf异常![%ld]",ret);
				  	 	WRITEMSG
				 	  	ERR_DEAL;
			    	}

				    ret = Cif_alias_inf_Fet_Upd(&cif_alias_inf,g_pub_tx.reply);
				    if( ret )
			    	{
					   	sprintf( acErrMsg,"FETCH Cif_alias_inf异常![%ld]",ret );
				  		WRITEMSG
				   		ERR_DEAL;
			    	}

					strcpy( cif_alias_inf.alias , cName);/*名称*/

					ret = Cif_alias_inf_Upd_Upd(cif_alias_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_alias_inf 异常![%ld]",ret);
						WRITEMSG
						ERR_DEAL;
					}

					Cif_alias_inf_Clo_Upd( );				
				}
					
				/* 修改客户联系电话 */
				ret = sql_count("cif_email_inf","cif_no=%ld and addr_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT异常![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					ret=Cif_email_inf_Dec_Upd(g_pub_tx.reply,
				   		"cif_no=%ld and addr_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_email_inf异常![%ld]",ret);
				   		WRITEMSG
				   		ERR_DEAL;
			    	}

				    ret = Cif_email_inf_Fet_Upd(&cif_email_inf,g_pub_tx.reply);
				    if( ret )
				    {
					   sprintf(acErrMsg,"FETCH cif_email_inf异常![%ld]",ret);
					   WRITEMSG
					   ERR_DEAL;
			    	}

					strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
					strcpy( cif_email_inf.addr_type , cif_email_inf_tmp.addr_type);	
					ret = Cif_email_inf_Upd_Upd(cif_email_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_email_inf异常![%ld]",ret );
						WRITEMSG
						ERR_DEAL;
					}

					Cif_email_inf_Clo_Upd( );				
				}
				
				/* 修改客户联系地址 */
				ret = sql_count("cif_addr_inf","cif_no=%ld and addr_seqn=1", \
								 g_cif_basic_inf.cif_no );
				if( ret<0 )
	            {
				   sprintf(acErrMsg,"TRANSFER SQL_COUNT异常![%ld]", ret);
				   WRITEMSG
				   ERR_DEAL;
			    }
				else if( ret>0 )
				{
					ret = Cif_addr_inf_Dec_Upd(g_pub_tx.reply,
					   	"cif_no=%ld and addr_seqn=1",g_cif_basic_inf.cif_no);
			    	if( ret )
	            	{
				   		sprintf(acErrMsg,"DECLARE Cif_email_inf异常[%ld]",ret );
				   		WRITEMSG
				   		ERR_DEAL;
			    	}
			    	ret = Cif_addr_inf_Fet_Upd(&cif_addr_inf,g_pub_tx.reply);
			    	if( ret )
			    	{
				   		sprintf(acErrMsg,"FETCH cif_email_inf异常![%ld]", ret);
				   		WRITEMSG
				   		ERR_DEAL;
			    	}

					strcpy( cif_addr_inf.addr , cif_addr_inf_tmp.addr );

					ret = Cif_addr_inf_Upd_Upd(cif_addr_inf,g_pub_tx.reply);
					if( ret )
					{
						sprintf(acErrMsg,"UPDATE cif_addr_inf异常![%ld]",ret);
						WRITEMSG
						ERR_DEAL;
					}
					Cif_addr_inf_Clo_Upd( );
				}

		  /* add begin 网银客户信息同步 lzy 20141204 */
			char today[9]={0};
			long nextday = 0;
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);
			/**vtcp_log("%s %d:: nextday[%ld]  ",__FILE__,__LINE__,nextday);**/
			
			if (cif_basic_inf.type[0] == '1' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if (cif_basic_inf.type[0] == '2' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::客户类型为[%s],不需要与宇信同步.",__FILE__,__LINE__,cif_basic_inf.type);
					break;
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

			if (cif_basic_inf.type[0] == '1' )  /**个人客户**/
				{
					/**fprintf(fp, "客户号,证件类型,证件号码,姓名,地址,邮编,电话,手机,电邮,性别\n");**/
					fprintf(fp, "%ld,,,%s,%s,,,,,\n",g_cif_basic_inf.cif_no,cName,cif_addr_inf_tmp.addr);
				}
			else if (cif_basic_inf.type[0] == '2' )  /**公司客户**/
				{
					/**fprintf(fp, "客户号,证件类型,证件号码,名称,地址,邮编,电话,法人代表\n");**/
					fprintf(fp, "%ld,,,%s,%s,,,\n",g_cif_basic_inf.cif_no,cName,cif_addr_inf_tmp.addr);
				}	
				fclose(fp);
				
			sprintf(cFtpfile,"%s/bin/ftp_to_wyzt.sh %s",getenv("WORKDIR"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
				{
					vtcp_log("[%s][%d] ftp文件[%s]到网银中台出错[%d]", __FILE__, __LINE__,filename,ret);
					strcpy(g_pub_tx.reply,"S047");
					goto ErrExit;
				}	
			vtcp_log("[%s][%d] ftp文件[%s]到网银中台成功", __FILE__, __LINE__,filename);				
		  /* add end 网银客户信息同步 lzy 20141204 */

				break;
				
		case '2':									/*删除*/
				/* 检查客户号是否在主文件存在 */
				/*检查该客户若存在台帐则不允许删除*/
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"该客户号存在活期台账,不能删除!![%s],\
							g_dd_mst.cif_no=[%ld],g_cif_basic_inf.cif_no=[%ld]",
							g_pub_tx.reply,g_dd_mst.cif_no, \
							g_cif_basic_inf.cif_no);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C048");
				 	ERR_DEAL;
				}
				else if( ret && ret!=100)
			 	{
				 	sprintf(acErrMsg,"读取活期主文件信息异常![%ld]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W015");
					ERR_DEAL;
				}

				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"该客户号存在定期台账,不能删除![%ld]",ret);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C049");
				 	ERR_DEAL;
				 }
				 else if( ret && ret!=100)
				 {
				 	sprintf(acErrMsg,"读取定期主文件信息异常![%ld]",ret );
					WRITEMSG
					strcpy(g_pub_tx.reply,"W017");
					ERR_DEAL;
				 }

				ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"cif_no=%ld", \
								 g_cif_basic_inf.cif_no);
				if( ret==0 )
				{
				 	sprintf(acErrMsg,"该客户号存在贷款台账,不能删除![%s]",ret);
				 	WRITEMSG
				 	strcpy(g_pub_tx.reply,"C050");
				 	ERR_DEAL;
				}
				else if( ret && ret!=100 )
				{
				 	sprintf(acErrMsg,"读取贷款主文件信息异常![%ld]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W019");
					ERR_DEAL;
				}

				/*取出客户类型*/
				ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,
							 		"cif_no=%ld",g_cif_basic_inf.cif_no);
				if( ret )
				{
				 	sprintf(acErrMsg,"读取客户基本信息表异常![%ld]",ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"W019");
					ERR_DEAL;
				}

				memset(tbwhere,0,sizeof(tbwhere));
				sprintf(tbwhere,"cif_no=%ld",g_cif_basic_inf.cif_no);

				if(g_cif_basic_inf.type[0]=='1')
				{
					ret =deldb("cif_per_inf",tbwhere);
					if( ret )
					{
				   		sprintf(acErrMsg,"删除个人客户基本信息表异常[%ld]",ret);
				  		WRITEMSG
				   		strcpy(g_pub_tx.reply,"C006");
				   		ERR_DEAL;
					}
				}
				else if(g_cif_basic_inf.type[0]=='2')
				{
					ret =deldb("cif_cop_inf",tbwhere);
					if( ret )
					{
				   		sprintf(acErrMsg,"删除公司客户基本信息表异常[%ld]",ret);
				  		WRITEMSG
				   		strcpy(g_pub_tx.reply,"C006");
				   		ERR_DEAL;
					}
				}

				/*删除客户证件与客户号对照表内容*/
				ret = deldb("cif_id_code_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户证件与客户号对照表异常[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户基本信息表内容*/
				ret = deldb("cif_basic_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户基本信息表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户地址信息表内容*/
				ret = deldb("cif_addr_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户地址信息表记录异常!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户别名信息表内容*/
				ret = deldb("cif_alias_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除别名信息表记录异常!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户黑名单信息表内容*/
				ret = deldb("cif_blklst_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户黑名单信息表记录异常!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户-客户关系表内容*/
				ret = deldb("cif_cif_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户-客户关系表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除电子地址信息表内容*/
				ret = deldb("cif_email_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除电子地址信息表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户生活方式信息表内容*/
				ret = deldb("cif_lifsty_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户生活方式表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户-客户经理关系表内容*/
				ret = deldb("cif_mger_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户-客户经理关系表异常[%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户其他金融产品信息表内容*/
				ret = deldb("cif_oth_prdt_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户其他金融产品信息表异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户产品关系表内容*/
				ret = deldb("cif_prdt_rel",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户产品关系表记录异常!![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户工作历史信息表内容*/
				ret = deldb("cif_wrk_hst_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户工作历史信息表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}

				/*删除客户受信代号对照表内容*/
				ret = deldb("cif_crdt_code",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除客户授信代号对照表记录异常![%ld]",ret);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
				}
				break;
		default:
				sprintf(acErrMsg,"操作类型异常,请联系中心机房![%s]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C108");
				ERR_DEAL;				
				break;
	}

	/* 登记交易流水 */
	g_pub_tx.ac_id = g_cif_basic_inf.cif_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户基本信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"客户基本信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
/**插入客户别名信息
Ins_cif_alias_inf()
{
		int iRc = 0;
		if(strlen(g_cif_basic_inf.name)!=0)
			{
				cif_alias_inf.cif_no=g_cif_basic_inf.cif_no;
				cif_alias_inf.alias_seqn=1;
				strcpy( cif_alias_inf.alias , g_cif_basic_inf.name);  *名称*
				iRc=Cif_alias_inf_Ins(&cif_alias_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,插入数据库错误【%d】",__FILE__, __LINE__,iRc);
					}
			}
		return iRc;
}
**修改联系电话**
Ins_cif_email_inf()
{
	int iRc = 0;
	if(strmcp(cif_email_inf_tmp.email) != 0)
		{
			strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
			strcpy(cif_email_inf.link_man, g_cif_basic_inf.name);
			cif_email_inf.cif_no=g_cif_basic_inf.cif_no;
			cif_email_inf.addr_seqn=1;
			iRc=Cif_email_inf_Ins(&cif_email_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,插入数据库错误【%d】",__FILE__, __LINE__,iRc);
					}
		}
	return iRc;
}
**插入客户地址**
Ins_cif_addr_inf()
{
	int iRc = 0;
	if(strmcp(cif_addr_inf_tmp.email) != 0)
		{
			strcpy( cif_email_inf.email , cif_email_inf_tmp.email);
			strcpy(cif_addr_inf.link_man, g_cif_basic_inf.name);
			cif_addr_inf.cif_no=g_cif_basic_inf.cif_no;
			cif_addr_inf.addr_seqn=1;
			iRc=Cif_addr_inf_Ins(&cif_email_inf,g_pub_tx.reply);
				if(iRc)
					{
						vtcp_log("%s,%d,插入数据库错误【%d】",__FILE__, __LINE__,iRc);
					}
		}
	return iRc;
}**/

