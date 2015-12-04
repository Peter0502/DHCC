/*************************************************
* 文 件 名: spC307.c
* 功能描述：
		客户联系方式信息维护
*
* 作    者:
* 完成日期：2004年01月26日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_inf_chg_log_c.h"

static struct cif_basic_inf_c		cif_basic_inf;
static struct cif_email_inf_c		cif_email_inf;
static struct cif_email_inf_c		g_cif_email_inf;
static struct cif_email_inf_c		st_cif_email_inf;
static struct cif_inf_chg_log_c		cif_inf_chg_log;
static struct cif_inf_chg_log_c		cif_inf_chg;

char old_alias[30],tbwhere[50],memo[41];
char sts[2];

spC307()
{
	int ret=0;
	int iRc=0;
	int iRet=0;
	int cmp_code=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&g_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&st_cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&cif_inf_chg_log,0x00,sizeof(struct cif_inf_chg_log_c));

	/** 取值、赋值 **/
	get_zd_data("0680",sts);
	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*1-修改*/	
				get_zd_long("0290",&g_cif_email_inf.cif_no);		/*客户号*/
				get_zd_long("0480",&g_cif_email_inf.addr_seqn);		/*地址序号*/
				get_zd_data("0270",g_cif_email_inf.link_man);		/*联系人*/
				get_zd_data("0670",g_cif_email_inf.addr_type);		/*联系方式*/
				get_zd_data("0620",g_cif_email_inf.email);			/*电子地址*/
				vtcp_log("g_cif_email_inf.email===[%s]\n",g_cif_email_inf.email);
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				vtcp_log("g_cif_email_inf.link_man===[%s]\n",g_cif_email_inf.link_man);
				vtcp_log("g_cif_email_inf.cif_no===[%ld]\n",g_cif_email_inf.cif_no);
				vtcp_log("g_cif_email_inf.email===[%s]\n",g_cif_email_inf.email);
				vtcp_log("sts=====[%c]\n",sts[0]);

				ret=Cif_email_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and addr_seqn=%d",g_cif_email_inf.cif_no,
					g_cif_email_inf.addr_seqn);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_email_inf_rel游标异常![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				ret=Cif_email_inf_Fet_Upd(&cif_email_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_email_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				vtcp_log("rowid==[%20s]\n",cif_email_inf.rowid);
				vtcp_log("g_rowid==[%20s]\n",g_cif_email_inf.rowid);
				memcpy(g_cif_email_inf.rowid, cif_email_inf.rowid,sizeof(g_cif_email_inf.rowid));
				ret=Cif_email_inf_Upd_Upd(g_cif_email_inf,g_pub_tx.reply);
				vtcp_log("[%s][%d]I am herer!\n",__FILE__,__LINE__);
				vtcp_log("cif_no==[%d]\n",g_cif_email_inf.cif_no);
				vtcp_log("addr_seqn=[%d]\n",g_cif_email_inf.addr_seqn);
				vtcp_log("link_amn=[%s]\n",g_cif_email_inf.link_man);
				vtcp_log("addr_type=[%c]\n",g_cif_email_inf.addr_type[0]);
				vtcp_log("email==[%s]\n",g_cif_email_inf.email);
				if (ret)
				{
    					sprintf(acErrMsg,"UPDATE Cif_email_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				Cif_email_inf_Clo_Upd( );
				
		  /* add begin 网银客户信息同步 lzy 20141204 */
			char today[9]={0};
			/*char nextday[9]={0};*/
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(g_cif_email_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);

			if ( cifno[0] == '1' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if ( cifno[0] == '5' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::客户号[%s],不需要与宇信同步.",__FILE__,__LINE__,cifno);
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

			if ( cifno[0] == '1' )  /**个人客户**/
			{
					if ( g_cif_email_inf.addr_type[0] == '1' ) /**电话**/
					{/**fprintf(fp, "客户号,证件类型,证件号码,姓名,地址,邮编,电话,手机,电邮,性别\n");**/
						fprintf(fp, "%ld,,,,,,%s,,,\n",g_cif_email_inf.cif_no,g_cif_email_inf.email);
					}
					else if (  g_cif_email_inf.addr_type[0] == '5' )  /**手机**/
					{
						fprintf(fp, "%ld,,,,,,,%s,,\n",g_cif_email_inf.cif_no,g_cif_email_inf.email);
					}
					else if ( g_cif_email_inf.addr_type[0] == '6' )  /**电邮**/
					{
						fprintf(fp, "%ld,,,,,,,,%s,\n",g_cif_email_inf.cif_no,g_cif_email_inf.email);
					}
					else
					{
						vtcp_log("%s %d::不需要与宇信同步.",__FILE__,__LINE__);
						fclose(fp);
						break;
					}
			}
			else if (cifno[0] == '5' && g_cif_email_inf.addr_type[0] == '2' )  /**公司客户电话**/
			{/**fprintf(fp, "客户号,证件类型,证件号码,名称,地址,邮编,电话,法人代表\n");**/
				fprintf(fp, "%ld,,,,,,%s,\n",g_cif_email_inf.cif_no,g_cif_email_inf.email);
			}
			else
			{
				vtcp_log("%s %d::不需要与宇信同步.",__FILE__,__LINE__);
				fclose(fp);
				break;
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
				
		case '2':									/*2-删除*/
				/*删除客户别名信息表内容*/
                                get_zd_long("0290",&g_cif_email_inf.cif_no);            /*客户号*/
                                get_zd_long("0480",&g_cif_email_inf.addr_seqn);          /*地址序号*/
                                get_zd_data("0270",g_cif_email_inf.link_man);           /*联系人*/
                                get_zd_data("0670",g_cif_email_inf.addr_type);          /*联系方式*/
                                get_zd_data("0620",g_cif_email_inf.email);                      /*电子地址*/
                                vtcp_log("g_cif_email_inf.email===[%s]\n",g_cif_email_inf.email);
                                vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
                                vtcp_log("g_cif_email_inf.link_man===[%s]\n",g_cif_email_inf.link_man);
                                vtcp_log("g_cif_email_inf.cif_no===[%ld]\n",g_cif_email_inf.cif_no);
                                vtcp_log("g_cif_email_inf.email===[%s]\n",g_cif_email_inf.email);
                                vtcp_log("sts=====[%c]\n",sts[0]);

				sprintf(tbwhere,"cif_no=%ld and addr_seqn=%d",
						g_cif_email_inf.cif_no,g_cif_email_inf.addr_seqn);
				ret =deldb("cif_email_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户别名信息表记录异常!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		case '3':  /*电话银行修改手机号码*/
				get_zd_long("0290",&g_cif_email_inf.cif_no);		/*客户号*/
				/*通过电话银行修改手机号码时,取系统中addr_seqn最大值*/
				if( 0 == g_cif_email_inf.addr_seqn )
                                {
                                        ret=sql_max_int( "cif_email_inf", "addr_seqn", "addr_type='5' and \
                                        cif_no=%ld ", g_cif_email_inf.cif_no );
                                        g_cif_email_inf.addr_seqn = ret ;
                                        vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
                                }
				ret=Cif_email_inf_Dec_Upd(g_pub_tx.reply,
					"cif_no=%ld and addr_seqn=%d",g_cif_email_inf.cif_no,
					g_cif_email_inf.addr_seqn);
				if (ret)
 				{
 					sprintf(acErrMsg,"DECLARE Cif_email_inf_rel游标异常![%s]",
					g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				ret=Cif_email_inf_Fet_Upd(&g_cif_email_inf,g_pub_tx.reply);
				if (ret==100 || ret==1403)	/**Insert**/
				{
					Cif_email_inf_Clo_Upd();
					
					iRet = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf ," cif_no=%ld " , g_cif_email_inf.cif_no );
    					if( iRet==100 || iRet==1403)
    					{
    					   	sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
    					   	WRITEMSG
    					   	strcpy(g_pub_tx.reply,"C007");
    					   	ERR_DEAL;
    					}
    					else if( iRet )
    					{
    					   	sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
    					   	WRITEMSG
    					   	strcpy(g_pub_tx.reply,"C006");
    					   	ERR_DEAL;
    					}
    					memcpy(st_cif_email_inf.link_man,cif_basic_inf.name,strlen(cif_basic_inf.name));
					st_cif_email_inf.addr_seqn=sql_max_int( "cif_email_inf", "addr_seqn", " cif_no=%ld ", g_cif_email_inf.cif_no );
                                        if(st_cif_email_inf.addr_seqn >= 0)
                                        {
                                        	st_cif_email_inf.addr_seqn = st_cif_email_inf.addr_seqn + 1;
                                	}
                                	else
                                	{
                                		st_cif_email_inf.addr_seqn = 1;
                                	}
                                        vtcp_log("[%s],[%d],st_cif_email_inf.addr_seqn=[%d]",__FILE__,__LINE__,st_cif_email_inf.addr_seqn);
                                        st_cif_email_inf.cif_no = g_cif_email_inf.cif_no;
                                        st_cif_email_inf.addr_type[0]='5';
                                        get_zd_data("0620",st_cif_email_inf.email);			/*电子地址*/
                                        
                                        iRc=Cif_email_inf_Ins(st_cif_email_inf,g_pub_tx.reply);
    					if( iRc && iRc!=-239 )
    					{
    					 	Cif_email_inf_Debug(st_cif_email_inf);
    					 	sprintf(acErrMsg,"登记客户电子地址信息表失败![%d]",iRc);         
    					    	WRITEMSG
    					    	strcpy(g_pub_tx.reply,"C024");
    					    	goto ErrExit;
    					}
    					else if( iRc ==-239 )
    					{
						Cif_email_inf_Debug(st_cif_email_inf);
						sprintf(acErrMsg,"登记客户电子地址信息表有重复记录![%d]",iRc);
						WRITEMSG
						strcpy(g_pub_tx.reply,"C089");
						goto ErrExit;
					}
					vtcp_log("[%s],[%d],登记客户电子地址信息成功!",__FILE__,__LINE__);
					break;
					
				}
				else if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_email_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				get_zd_data("0620",g_cif_email_inf.email);			/*电子地址*/
				vtcp_log("cif_no==[%d]\n",g_cif_email_inf.cif_no);
				vtcp_log("addr_seqn=[%d]\n",g_cif_email_inf.addr_seqn);
				vtcp_log("link_man=[%s]\n",g_cif_email_inf.link_man);
				vtcp_log("addr_type=[%c]\n",g_cif_email_inf.addr_type[0]);
				vtcp_log("email==[%s]\n",g_cif_email_inf.email);
				ret=Cif_email_inf_Upd_Upd(g_cif_email_inf,g_pub_tx.reply);
				if (ret)
				{
    					sprintf(acErrMsg,"UPDATE Cif_email_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					Cif_email_inf_Clo_Upd();
					ERR_DEAL;
				}
				vtcp_log("g_cif_email_inf.addr_seqn===[%d]\n",g_cif_email_inf.addr_seqn);
				Cif_email_inf_Clo_Upd();
				break;
		default:
				break;
	}

	/* 登记客户信息修改日志 */
	cif_inf_chg_log.cif_no=g_cif_email_inf.cif_no;				/*客户ID*/
	memcpy(cif_inf_chg_log.chg_tablename,"cif_email_inf",sizeof(cif_inf_chg_log.chg_tablename));     /*被修改的客户信息表名*/
	memcpy(cif_inf_chg_log.chg_columnname,"alias",sizeof(cif_inf_chg_log.chg_columnname));		/*被修改客户信息列名*/
	cif_inf_chg_log.chg_date=g_pub_tx.tx_date;					/*修改日期*/
	cif_inf_chg_log.chg_time=g_pub_tx.tx_time;					/*修改时间*/
	memcpy(cif_inf_chg_log.bef_content,cif_email_inf.link_man,sizeof(cif_inf_chg_log.bef_content));	/*修改前内容*/
	memcpy(cif_inf_chg_log.aft_content,g_cif_email_inf.link_man,sizeof(cif_inf_chg_log.aft_content));/*修改后内容*/
	memcpy(cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(cif_inf_chg_log.clerk_no));        		/*职员编号*/
  memcpy(cif_inf_chg_log.memo,memo,sizeof(cif_inf_chg_log.memo));   						/*更改说明*/

    ret = Cif_inf_chg_log_Ins( cif_inf_chg_log );
	if( cmp_code )
    {
         sprintf(acErrMsg,"登记客户信息修改日志失败![%d]",ret);
         WRITEMSG
         strcpy(g_pub_tx.reply,"C013");
         goto ErrExit;
    }

    /* 登记交易流水 */
    if( pub_ins_trace_log() )
    {
         sprintf(acErrMsg,"登记交易流水错误!");
         WRITEMSG
         goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户联系方式信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户联系方式信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

