/*************************************************
* 文 件 名: spC105.c
* 功能描述：客户联系方式信息录入
* 作    者: andy
* 完成日期：2004年01月15日
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
#include "cif_email_inf_c.h"
#include "cif_basic_inf_c.h"

static struct cif_email_inf_c   cif_email_inf;
static struct cif_email_inf_c   g_cif_email_inf;
static struct cif_basic_inf_c   cif_basic_inf;  

spC105()
{
	int i=0;
 	int ret=0;
 	long cif_no;

    /** 初始化全局变量 **/
    pub_base_sysinit();

    /** 数据初始化 **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
    memset (&g_cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
    memset (&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));

    /** 取值、赋值 **/
   	get_zd_data("0670",cif_email_inf.addr_type);   		/*联系方式*/
	get_zd_long("0290",&cif_email_inf.cif_no);     		/*客户ID*/
    get_zd_data("0250",cif_email_inf.email);       		/*电子地址*/
    get_zd_data("0270",cif_email_inf.link_man);    		/*联系人*/
    get_zd_long("0480",&cif_email_inf.addr_seqn);   		/*地址序号*/

    /** 处理流程 **/
    /* 检查客户号是否已经存在 */
    ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf ,
           					" cif_no=%ld " , cif_email_inf.cif_no );
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

	/*检查输入的联系方式和已有的联系明细是否重复*/
	ret=Cif_email_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld",cif_email_inf.cif_no);
    if( ret )
    {
       	sprintf(acErrMsg,"DECLARE Cif_email_inf_Dec_Sel异常![%s]",
				g_pub_tx.reply);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"C039");
       	ERR_DEAL;
    }

	while(1)
	{
       	sprintf(acErrMsg,"cif_email_inf.cif_no!!==[%ld]",cif_email_inf.cif_no);
       	WRITEMSG
		ret =Cif_email_inf_Fet_Sel(&g_cif_email_inf,g_pub_tx.reply);
    	if( ret && ret!=100 )
    	{
       		sprintf(acErrMsg,"FETCH Cif_email_inf_Dec_Sel游标异常![%s]",
					g_pub_tx.reply);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"C038");
       		ERR_DEAL;
    	}else if(ret==100)
		{
			TRACE
			break;
		}
		TRACE

		i++;
		if(g_cif_email_inf.addr_type[0]!=cif_email_inf.addr_type[0])continue;
		sprintf(acErrMsg,"!!!!!!![%s][%s]", g_cif_email_inf.addr_type,cif_email_inf.addr_type);
	 	WRITEMSG

		pub_base_strpack(g_cif_email_inf.email);	/*空格压缩*/
		switch(atoi(cif_email_inf.addr_type))
		{
		TRACE
			case 1:									/*住宅电话*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此住宅电话!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C032");
       					ERR_DEAL;
					}
			case 2:									/*公司电话*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此公司电话!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C033");
       					ERR_DEAL;
					}
			case 3:									/*传真*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此传真号码!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C034");
       					ERR_DEAL;
					}
			case 4:									/*传呼*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此传呼号码!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C035");
       					ERR_DEAL;
					}
			case 5:									/*手机*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此手机号码!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C036");
       					ERR_DEAL;
					}
			case 6:									/*电子邮件*/
					if(!strcmp(cif_email_inf.email,g_cif_email_inf.email))
    				{
       					sprintf(acErrMsg,"该客户已存在此电子邮件!!");
       					WRITEMSG
       					strcpy(g_pub_tx.reply,"C037");
       					ERR_DEAL;
					}
			default:
					break;
		}
		TRACE
	}

	Cif_email_inf_Clo_Sel ();
    sprintf(acErrMsg,"!11111!!!![%d]",i);
	WRITEMSG


   	/* 登记客户电子地址信息表 */
	if(i) 
	{
		cif_email_inf.addr_seqn=g_cif_email_inf.addr_seqn+1;
		TRACE
	}else{ 
		TRACE
		cif_email_inf.addr_seqn=1;
	}
    ret=Cif_email_inf_Ins(cif_email_inf,g_pub_tx.reply);
    if( ret && ret!=-239 )
    {
     	sprintf(acErrMsg,"登记客户电子地址信息表失败![%d]",ret);         
        WRITEMSG
        strcpy(g_pub_tx.reply,"C024");
        goto ErrExit;
    }else if( ret ==-239 ){
		sprintf(acErrMsg,"登记客户电子地址信息表有重复记录![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C089");
		goto ErrExit;
	}


    /* 登记交易流水 */
 	if( pub_ins_trace_log() )
 	{
 		sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
 vtcp_log("arrrrrrrrrrrr");
    set_zd_long("0480",cif_email_inf.addr_seqn);   		/*地址序号*/
    
    vtcp_log("aaaaaaaaaaaaa");
		  /* add begin 网银客户信息同步 lzy 20141216 */
			char today[9]={0};
			/*char nextday[10]={0};*/
			long nextday = 0;
			char cifno[9]={0};
			char filename[60]={0};
			char pcfilename[256]={0};
			char cFtpfile[256]={0};
			FILE *fp = NULL;
			
			apitoa(cif_email_inf.cif_no,8,cifno);
			apitoa(g_pub_tx.tx_date,8,today);
			/*memcpy(nextday,pub_base_daynumLAT(today,1),8);*/
			nextday = pub_base_daynumLAT(today,1);
			 vtcp_log("bbbbbbbbbbb");

			if ( cifno[0] == '1' )
				{
					sprintf( filename,"PersCustSyn%ld.txt",nextday);
					 vtcp_log("cccccccccccccc");
				}
			else if ( cifno[0] == '5' )
				{
					sprintf( filename,"CorpCustSyn%ld.txt",nextday);
				}
			else
				{
					vtcp_log("%s %d::客户号[%s],不需要与宇信同步.",__FILE__,__LINE__,cifno);
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
 vtcp_log("ddddddddddddddd");
			if ( cifno[0] == '1' )  /**个人客户**/
			{
					if ( g_cif_email_inf.addr_type[0] == '1' ) /**电话**/
					{/**fprintf(fp, "客户号,证件类型,证件号码,姓名,地址,邮编,电话,手机,电邮,性别\n");**/
						fprintf(fp, "%ld,,,,,,%s,,,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else if (  g_cif_email_inf.addr_type[0] == '5' )  /**手机**/
					{
						fprintf(fp, "%ld,,,,,,,%s,,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else if ( g_cif_email_inf.addr_type[0] == '6' )  /**电邮**/
					{
						fprintf(fp, "%ld,,,,,,,,%s,\n",cif_email_inf.cif_no,cif_email_inf.email);
					}
					else
					{
						vtcp_log("%s %d::不需要与宇信同步.",__FILE__,__LINE__);
						fclose(fp);
						goto OkExit;
					}
			}
			else if (cifno[0] == '5' && g_cif_email_inf.addr_type[0] == '2' )  /**公司客户电话**/
			{/**fprintf(fp, "客户号,证件类型,证件号码,名称,地址,邮编,电话,法人代表\n");**/
				fprintf(fp, "%ld,,,,,,%s,\n",cif_email_inf.cif_no,cif_email_inf.email);
			}
			else
			{
				vtcp_log("%s %d::不需要与宇信同步.",__FILE__,__LINE__);
				fclose(fp);
				goto OkExit;
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
     vtcp_log("aeeeeeeeeeeeeee");
    

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    WRITEMSG
    sprintf(acErrMsg,"客户联系方式信息录入成功![%s]",g_pub_tx.reply);
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户联系方式信息录入失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
