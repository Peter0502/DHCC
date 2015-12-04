/*************************************************
* 文 件 名: spC106.c
* 功能描述：客户地址信息录入
*
* 作    者: 
* 完成日期：2004年01月08日
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
#include "cif_addr_inf_c.h"

static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_addr_inf_c     cif_addr_inf;

spC106()  
{ 	
	int ret = 0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 数据初始化 **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));

	/** 取值、赋值 **/
	get_zd_long("0280",&cif_addr_inf.cif_no);		/* 客户号 */
	get_zd_data("0670",cif_addr_inf.addr_type);		/* 地址类型 */
	get_zd_data("0230",cif_addr_inf.country);		/* 所在国家 */
	get_zd_data("0810",cif_addr_inf.addr);			/* 地址 */
	get_zd_data("0650",cif_addr_inf.post_code);		/* 邮政编码 */

		sprintf(acErrMsg,"无此客户号，请检查![%s]", \
				cif_addr_inf.post_code);
		WRITEMSG
	/**处理流程**/
	/*检查客户号是否已经存在*/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf , \
						  "cif_no=%ld",cif_addr_inf.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	/*查询地址信息表是否存在记录*/
	ret=sql_count("cif_addr_inf","cif_no=%ld",cif_addr_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_addr_inf.addr_seqn=1;

		/* 登记客户地址信息表*/
		ret=Cif_addr_inf_Ins(cif_addr_inf);
		if(ret)
		{
			sprintf(acErrMsg,"登记客户地址信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
			goto ErrExit;
		}
	}
	else
	{
		ret = sql_max_int( "Cif_addr_inf", "addr_seqn", " cif_no=%ld ", 
							cif_addr_inf.cif_no);
		if ( ret < 0 )
		{
			sprintf(acErrMsg,"读取客户地址信息表地址最大序号异常![%s]", ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			ERR_DEAL;
		}
		else if(ret)
		{

  			cif_addr_inf.addr_seqn = ret + 1;
	
			/* 登记客户地址信息表 */
			ret = Cif_addr_inf_Ins( cif_addr_inf );
			if( ret )
			{
				sprintf(acErrMsg,"登记客户地址信息表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C023");
				goto ErrExit;
			}
		}
	}
	/* 登记交易流水 */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	/* 前台输出 */
	set_zd_long("0350",cif_addr_inf.addr_seqn);
		  /* add begin 网银客户信息同步 lzy 20141216 */
			char today[9]={0};
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(cif_addr_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			nextday = pub_base_daynumLAT(today,1);

			if ( cifno[0] == '1' && cif_addr_inf.addr_type[0] == '2' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
				}
			else if ( cifno[0] == '5' && cif_addr_inf.addr_type[0] == '1' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::客户号[%s],地址类型[%s],不需要与宇信同步.",__FILE__,__LINE__,cifno,cif_addr_inf.addr_type);
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

			if ( cifno[0] == '1' )  /**个人客户**/
				{
					/**fprintf(fp, "客户号,证件类型,证件号码,姓名,地址,邮编,电话,手机,电邮,性别\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,,,\n",cif_addr_inf.cif_no,cif_addr_inf.addr,cif_addr_inf.post_code);
				}
			else if (cifno[0] == '5' )  /**公司客户**/
				{
					/**fprintf(fp, "客户号,证件类型,证件号码,名称,地址,邮编,电话,法人代表\n");**/
					fprintf(fp, "%ld,,,,%s,%s,,\n",cif_addr_inf.cif_no,cif_addr_inf.addr,cif_addr_inf.post_code);
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
		  /* add end 网银客户信息同步 lzy 20141216 */

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户地址信息录入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"客户地址信息录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

