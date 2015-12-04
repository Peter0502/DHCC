
/*************************************************
* 文 件 名: spC113.c
* 功能描述：公司高管信息录入
*
* 作    者: lwb
* 完成日期：2014年10月14日
*
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
#include "cif_manage_inf_c.h"
#include "cif_basic_inf_c.h"

int spC113()
{
		int iRet = 0;
		int ret = 0;
		struct cif_manage_inf_c sCif_manage_inf;
		struct cif_basic_inf_c sCif_basic_inf;
		
		pub_base_sysinit();
		
		memset(&sCif_manage_inf,0x00,sizeof(sCif_manage_inf));
		memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
			
		get_zd_data("0260",sCif_manage_inf.name);
		get_zd_long("0280",&sCif_manage_inf.cif_no);
		get_zd_long("0290",&sCif_manage_inf.manage_no);
		get_zd_data("0670",sCif_manage_inf.type);
		get_zd_data("0680",sCif_manage_inf.sex);
		get_zd_data("0690",sCif_manage_inf.id_type);
		get_zd_data("0620",sCif_manage_inf.id_no);
		
		strcpy(sCif_manage_inf.br_no,g_pub_tx.tx_br_no);
    strcpy(sCif_manage_inf.tel,g_pub_tx.tel);
		
		ret=Cif_basic_inf_Sel( g_pub_tx.reply,&sCif_basic_inf,
							"cif_basic_inf.cif_no=%ld",sCif_manage_inf.cif_no);
		if( ret==100)
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
	
	/*检查客户类型是个人还是公司*/
	if(sCif_basic_inf.type[0]=='1')
	{
 		sprintf(acErrMsg,"该客户为个人户，不得登记!![%s]",g_pub_tx.reply);
    WRITEMSG
    strcpy(g_pub_tx.reply,"O165");
    ERR_DEAL;
  }

	sCif_manage_inf.tx_date = g_pub_tx.tx_date;
	sCif_manage_inf.up_date = g_pub_tx.tx_date;
	
	ret=sql_count("cif_manage_inf","cif_no=%ld ",sCif_manage_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		sCif_manage_inf.manage_no=1;

		/* 登记客户高管信息表 */
		iRet = Cif_manage_inf_Ins(sCif_manage_inf,g_pub_tx.reply);
		if(iRet)
		{
			sprintf(acErrMsg,"登记客户高管信息表失败！");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data(DC_GET_MSG,acErrMsg);
			ERR_DEAL
		}
	}
	else
	{
		ret=sql_max_int("cif_manage_inf","manage_no",\
						"cif_no=%ld ",sCif_manage_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			sCif_manage_inf.manage_no=ret+1;

		
			iRet = Cif_manage_inf_Ins(sCif_manage_inf,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"登记客户高管信息表失败！");
				strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				ERR_DEAL
			}

		}
	}
	set_zd_int("0350",sCif_manage_inf.manage_no);
		/*登记交易流水*/
	g_pub_tx.ac_id = sCif_manage_inf.cif_no;
	strcpy(g_pub_tx.brf,"公司高管信息录入");
	g_pub_tx.ac_seqn = sCif_manage_inf.manage_no;
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }


		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户高管工作信息录入成功!!%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"客户工作高管录入失败!!%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;		
}
