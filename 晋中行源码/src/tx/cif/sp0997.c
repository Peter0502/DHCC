/*************************************************
* 文 件 名: sp0997.c
* 功能描述：建立客户识别ID
* 作    者: andy
* 完成日期：2004年01月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#define TX_CODE "0990"
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

static struct cif_alias_inf_c    cif_alias_inf;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_id_code_rel_c  cif_id_code_rel;
static struct cif_addr_inf_c  	 cif_addr_inf;
static struct cif_email_inf_c  	 cif_email_inf;

sp0997()
{ 	
	int ret=0;
	int lkg_flag=0;
	long cif_no;
	long bir_date;
	char sex[2];
	char tmp_andy[11];

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;

	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	vtcp_log("[%s][%d] cif_basic_inf.name = [%s]\n",cif_basic_inf.name);
	vtcp_log("[%s][%d] cif_addr_inf.addr = [%s]\n",cif_addr_inf.addr);

	vtcp_log("%d@%s   取流水号 \n", __LINE__, __FILE__);
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("%d@%s   END 取流水号 \n", __LINE__, __FILE__);

	vtcp_log("[%s][%d] cif_basic_inf.name = [%s]\n",cif_basic_inf.name);
	if (iCharacterType(cif_addr_inf.addr,sizeof(cif_addr_inf.addr),'g'))
	{
		sprintf(acErrMsg,"客户地址含有非法字符![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C128");
		goto ErrExit;
	}
	vtcp_log("[%s][%d] cif_addr_inf.addr = [%s]\n",cif_addr_inf.addr);

	/** 处理流程 **/
	/* 检查客户证件类型＋证件号码是否已经存在 */
	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel, 
		" id_type='%s' and id_no='%s'",cif_id_code_rel.id_type ,
		cif_id_code_rel.id_no );
	if ( ret==0 )
	{
		sprintf(acErrMsg,"该客户已经拥有客户ID![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C001");
		goto ErrExit;
	}
	else if ( ret!=100 )
	{
		sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
	vtcp_log("%s,%d type=[%s]",__FILE__,__LINE__,cif_basic_inf.type);
	if (atoi(cif_basic_inf.type)!=1)
	{
		/*检查客户户名是否已经存在*/
		vtcp_log("%s,%d name=[%s]",__FILE__,__LINE__);
		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"name='%s'and type!='%s' and cif_no<60000000",cif_basic_inf.name,"1");
		if (ret==0)
		{
			sprintf(acErrMsg,"该户名已存在[%s]",g_pub_tx.reply);
			WRITEMSG
				strcpy(g_pub_tx.reply,"C220");
			goto ErrExit;
		}
		else if ( ret!=100 )
		{
			sprintf(acErrMsg,"取该户名信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
				strcpy(g_pub_tx.reply,"C221");
			goto ErrExit;
		}
	}
	/* 生成客户id */
	switch(atoi(cif_basic_inf.type))
	{
	case 9: /* 内部员工 */
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"生成客户id失败!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;	  	
	case 1: /* 个人 */
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"生成客户id失败!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;
	case 2: /* 公司 */
	case 3: /* 机构 */
	case 4: /* 金融机构 */
		ret = pub_cif_CreCom_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"生成客户id失败!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
		break;		
	}	
	cif_basic_inf.cif_no = cif_no ;
	cif_id_code_rel.cif_no = cif_no ;	

	/* 登记客户证件与客户号对照表 */
	ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
	vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
	if ( ret )
	{
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		sprintf(acErrMsg,"登记客户证件与客户号对照表错误![%d]",ret);
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		WRITEMSG
			vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"C004");
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
		goto ErrExit;
		vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);
	}
	vtcp_log("[%s][%d]ret===[%d]\n",__FILE__,__LINE__,ret);

	/* 登记客户基本信息表 */
	cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* 建立日期 */
	cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* 建立时间 */
	strcpy( cif_basic_inf.sts , "1" ) ;         /* 客户状态：1正常 2注销 */
	cif_basic_inf.cif_seqn = 0 ;                /* 客户序号 */

	ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"登记客户基本信息表错误![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
		goto ErrExit;
	}
	strcpy(cif_alias_inf.alias,cif_basic_inf.name);		/* 客户名称 */
	cif_alias_inf.cif_no = cif_basic_inf.cif_no ; 		/* 客 户 号 */
	cif_alias_inf.alias_seqn = 1 ;                		/* 别名序号 */
	ret=Cif_alias_inf_Ins(cif_alias_inf,g_pub_tx.reply);
	if ( ret )
	{
		sprintf(acErrMsg,"登记客户别名信息表错误![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C083");
		goto ErrExit;
	}
	/* 登记交易流水 */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
			goto ErrExit;
	}
	if (cif_basic_inf.type[0]=='1'||cif_basic_inf.type[0]=='5')
	{
		strcpy(cif_addr_inf.addr_type,"2");
		if (!strncmp(cif_email_inf.email,"13",2)&&strlen(cif_email_inf.email)==11)
			strcpy(cif_email_inf.addr_type,"5");
		else
			strcpy(cif_email_inf.addr_type,"1");
	}
	else if (cif_basic_inf.type[0]=='2'||cif_basic_inf.type[0]=='3'||cif_basic_inf.type[0]=='4')
	{
		strcpy(cif_addr_inf.addr_type,"1");
		strcpy(cif_email_inf.addr_type,"2");
	}
	cif_addr_inf.cif_no=cif_no;
	cif_email_inf.cif_no=cif_no;
	cif_addr_inf.addr_seqn=1;
	cif_email_inf.addr_seqn=1;
	strcpy(cif_addr_inf.country,"01");
	strcpy(cif_email_inf.link_man,cif_basic_inf.name);

	ret=Cif_email_inf_Ins(cif_email_inf,g_pub_tx.reply);
	if (ret)
	{
		Cif_email_inf_Debug(&cif_email_inf);
		sprintf(acErrMsg,"登记电子地址信息表错误![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C024");
		goto ErrExit;
	}
	ret=Cif_addr_inf_Ins(cif_addr_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"登记地址信息表错误![%d]",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"C023");
		goto ErrExit;
	}
	TRACE

	/* 输出变量 */
	set_zd_long( "0280", cif_no );
	set_zd_data("0160",TX_CODE);
	sprintf(acErrMsg,"客户号[%ld]",cif_no);
	WRITEMSG

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"建立客户识别ID成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("[%s][%d]FUCK!ret==[%d]\n",__FILE__,__LINE__,ret);
	sprintf(acErrMsg,"建立客户识别ID失败！[%s]",g_pub_tx.reply);
	vtcp_log("[%s][%d]FUCK!ret==[%d]\n",__FILE__,__LINE__,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int data_init_struct()
{
	memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf, 0x00, sizeof(struct cif_alias_inf_c));
	memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset(&cif_id_code_rel, 0 ,sizeof(struct cif_id_code_rel_c));
	memset(&cif_addr_inf   , 0 ,sizeof(struct cif_addr_inf_c));
	memset(&cif_email_inf  , 0 ,sizeof(struct cif_email_inf_c));
	return 0;
}

int data_set_struct()
{
	get_zd_data("0670",cif_basic_inf.type);		/* 客户类型 */
	get_zd_data("0680",cif_id_code_rel.id_type);/* 证件类型 */
	get_zd_data("0620",cif_id_code_rel.id_no);	/* 证件号码 */
	get_zd_data("0250",cif_basic_inf.name);		/* 客户名称 */
	strcpy(cif_basic_inf.local,"1");			/* 本地标志1-本地2-异地3-国外 */
	strcpy(cif_basic_inf.poten,"Y");			/* 潜在标志Y-潜在 N-非潜在*/
	strcpy(cif_basic_inf.shareholder_ind,"N");	/* 本行股东Y-是 N-否 */
	strcpy(cif_basic_inf.lvl,"4");
	/*客户级别：1 白金卡客户 2金卡客户 3银卡客户 4普通客户 */

	get_zd_data("0260",cif_email_inf.email);	/*联系信息*/
	get_zd_data("0270",cif_addr_inf.addr);		/*地址信息*/
	return 0;
}
