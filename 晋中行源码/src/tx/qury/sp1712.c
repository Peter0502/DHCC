/*********************************************************************************
***文 件 名:  sp1712.c                                                         ***
***功能描述： 个人客户资料查询                                                 ***
***                                                                            ***
***作    者:  Han Xichao                                                       ***
***完成日期： 2009-09-19                                                       ***
***                                                                            ***
***记    录： 根据证件类型和证件号码在cif_id_code_rel表中查询到客户的客户号，  ***
***           然后根据客户号到其他客户相关信息表中查询到客户相关资料信息       ***
**********************************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_wrk_hst_inf_c.h"
/**#include "cif_alias_inf_c.h"**/


int sp1712()
{
	struct cif_id_code_rel_c st_cif_id_code_rel;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct cif_email_inf_c st_cif_email_inf;
	struct cif_addr_inf_c st_cif_addr_inf;
	struct cif_per_inf_c st_cif_per_inf;
	struct cif_wrk_hst_inf_c st_cif_wrk_hst_inf;
	/**struct cif_alias_inf_c st_cif_alias_inf;**/
	
	int iCif_no=0;
	int ret=0;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
	memset(&st_cif_id_code_rel,0x00,sizeof(st_cif_id_code_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&st_cif_email_inf,0x00,sizeof(st_cif_email_inf));
	memset(&st_cif_addr_inf,0x00,sizeof(st_cif_addr_inf));
	memset(&st_cif_per_inf,0x00,sizeof(st_cif_per_inf));
	memset(&st_cif_wrk_hst_inf,0x00,sizeof(st_cif_wrk_hst_inf));
	/**memset(&st_cif_alias_inf,0x00,sizeof(st_cif_alias_inf));**/
		
	pub_base_sysinit();

	get_zd_data("0680",st_cif_id_code_rel.id_type );				/*证件类型*/
	get_zd_data("0620",st_cif_id_code_rel.id_no );				/*证件号码*/
		/**组成查询条件**/
	if(strlen(st_cif_id_code_rel.id_type)&&strlen(st_cif_id_code_rel.id_no))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&st_cif_id_code_rel, \
								"id_type='%s' and id_no='%s'", st_cif_id_code_rel.id_type, st_cif_id_code_rel.id_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else if(ret==0)
		{
			iCif_no=st_cif_id_code_rel.cif_no;
			vtcp_log( "[%s][%d] 客户号为：[%ld]",__FILE__,__LINE__,iCif_no);
			set_zd_long("0540",iCif_no);   /**客户号**/
		}
	}
	else 
	{
		sprintf(acErrMsg,"查询条件错误");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply,"BBCL" );
		goto ErrExit;
	}
	
	/**根据客户号到cif_basic_inf表中查询客户相关信息**/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&st_cif_basic_inf," cif_no=%ld ",iCif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else if(ret==0)
		{
			set_zd_data("0260",st_cif_basic_inf.name);           /**客户名**/
			set_zd_data("0340",st_cif_basic_inf.lvl);            /**信誉等级**/
		}
		/**
		char      type[2];
	char      name[61];
	char      local[2];
	char      poten[2];
	long      crt_date;
	long      crt_time;
	char      lvl[2];
	char      shareholder_ind[2];
	long      cif_seqn;
	char      sts[2];
**/
/**根据客户号到cif_per_inf表中查询客户相关信息**/
	ret=Cif_per_inf_Sel(g_pub_tx.reply,&st_cif_per_inf," cif_no=%ld ",iCif_no);
		/**if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else **/
			if(ret==0)
		{
			set_zd_data("0230",st_cif_per_inf.sex);              /**性别**/
			set_zd_long("0280",st_cif_per_inf.g_bir);            /**出生日期**/
			set_zd_double("0370",st_cif_per_inf.mon_income);     /**月收入**/
			set_zd_data("0240",st_cif_per_inf.marriage);         /**婚姻状况**/
			vtcp_log( "[%s][%d] st_cif_per_inf.sex[%s]",__FILE__,__LINE__,st_cif_per_inf.sex);
			vtcp_log( "[%s][%d] st_cif_per_inf.g_bir[%ld]",__FILE__,__LINE__,st_cif_per_inf.g_bir);
			vtcp_log( "[%s][%d] st_cif_per_inf.mon_income[%f]",__FILE__,__LINE__,st_cif_per_inf.mon_income);
			vtcp_log( "[%s][%d] st_cif_per_inf.marriage[%s]",__FILE__,__LINE__,st_cif_per_inf.marriage);
		}
		/**
	char      name[21];
	long      g_bir;
	long      n_bir;
	char      sex[2];
	char      marriage[2];
	long      mar_date;
	char      edu[2];
	char      religion[2];
	char      nationality[4];
	char      country[4];
	char      folk[3];
	char      language[2];
	char      clerk_no[6];
	char      health[2];
	char      technical_post[3];
	char      headship[3];
	char      birth_adr[41];
	double    mon_income;
	double    h_mon_income;
	long      home_cnt;
		**/

	/**根据客户号到cif_addr_inf表中查询客户相关信息**/
	ret=Cif_addr_inf_Sel(g_pub_tx.reply,&st_cif_addr_inf," cif_no=%ld and addr_seqn=1",iCif_no);
		/*if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else */
		if (ret==0)
		{
			set_zd_data("0250",st_cif_addr_inf.addr);                /**家庭住址**/
			set_zd_data("0290",st_cif_addr_inf.post_code);           /**家庭邮编**/
			vtcp_log( "[%s][%d] st_cif_addr_inf.addr[%s]",__FILE__,__LINE__,st_cif_addr_inf.addr);
			vtcp_log( "[%s][%d] st_cif_addr_inf.post_code[%s]",__FILE__,__LINE__,st_cif_addr_inf.post_code);
		}
		/**
		long      addr_seqn;
	char      addr_type[3];
	char      country[4];
	char      addr[61];
	char      post_code[7];
		**/
		/**根据客户号到cif_email_inf表中查询客户相关信息**/
	ret=Cif_email_inf_Sel(g_pub_tx.reply,&st_cif_email_inf," cif_no=%ld and addr_seqn=1",iCif_no);
		/*if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		else */
		if (ret==0)
		{
			set_zd_data("0300",st_cif_email_inf.email);                  /**手机号码**/
			vtcp_log( "[%s][%d] st_cif_email_inf.email[%s]",__FILE__,__LINE__,st_cif_email_inf.email);
			/**vtcp_log( "[%s][%d] st_cif_email_inf.link_man[%s]",__FILE__,__LINE__,st_cif_email_inf.link_man);**/
		}
		/**
		cif_no	integer
addr_seqn	smallint
addr_type	char(2)
住宅电话/公司电话/传真/传呼/手机/电子邮件
email	char(20)
link_man	char(60)
		**/
		
		/**根据客户号到cif_wrk_hst_inf表中查询客户相关信息**/
	ret=Cif_wrk_hst_inf_Sel(g_pub_tx.reply,&st_cif_wrk_hst_inf," cif_no=%ld ",iCif_no);
		if (ret==0)
		{
			set_zd_data("0270",st_cif_wrk_hst_inf.unit_name);                  /**单位名称**/
		}
		/**
		long      cif_no;
	long      wrk_no;
	char      industry_no[4];
	char      vocation_no[4];
	char      position_no[4];
	long      beg_date;
	char      wrk_ind[2];
	long      end_date;
	char      unit_name[51];
	double    income;
		**/
		
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
