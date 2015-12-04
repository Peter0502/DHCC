/*************************************************
* 文 件 名:    sp9939.c
* 功能描述：   
*              查询合同号是否存在
*
* 作    者:    lance
* 完成日期：   2003年06月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
Update  tx_def  set BIT_MAP='10000000000000000000000111100110100000100001100000000000000000000011001000000000000111000000000010000000000000000000000000001000'  where tx_code='9939';
20110811 BIT_MAP
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"
#include "cassreg_c.h"
#include "ln_parm_c.h"

char 	tmpbuf[256];


struct 	{
	char time_lmt[3]; 
	char amt[16];			/*3*/
	char rate[6];			/*19*/
	char pay_ac_no[19];		/*19*2+6*/
	char save_ac_no[19];		/*19*3+6*/
	struct {
		char have;
		char ac_no[19];	
		char bw_ac_id[9]; 
		char amt[16];
	}cassreg;
}fd125;
	
sp9939()  
{ 		
	int ret=0;
	char cTmp[7];
	struct ln_auth_c  ln_auth;
	struct cassreg_c  cassreg;
	/*20110728 晋中商行扩表，添加新的查询项*/
	struct ln_parm_c m_ln_parm;
	memset(cTmp,0x00,sizeof(cTmp));
	memset(&m_ln_parm,0,sizeof(m_ln_parm));
	char m_ln_type[2],m_intst_type[2];/*67,71域*/
	double m_rate,m_over_rate,m_fine_rate,m_amt;/*84,85,97,39域*/
	long m_beg_date,m_mtr_date;/*44,45域*/ 
	/*新变量定义结束*/
	
	memset( &ln_auth , 0x00 , sizeof(struct ln_auth_c) );
	memset( &g_ln_mst,   0x00 , sizeof(struct ln_mst_c)  );

	/** 取值、赋值 **/	
	get_zd_data( "0630" , g_ln_mst.pact_no );	/* 借据号码 */

	vtcp_log("[%s][%d] pack_no =[%s]\n",__FILE__,__LINE__,g_ln_mst.pact_no);
	/* 检查 合同号 */
	ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth , " pact_no='%s' " , g_ln_mst.pact_no );
	vtcp_log("[%s][%d] pack_no =[%s]\n",__FILE__,__LINE__,g_ln_mst.pact_no);
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"读取贷款主文件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L164");
		ERR_DEAL;
	}else if(ret==100){
		vtcp_log("[%s][%d] pack_no =[%s]\n",__FILE__,__LINE__,g_ln_mst.pact_no);
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"pact_no='%s'",g_ln_mst.pact_no);
		if(g_ln_mst.opn_date>=20070501){			/* 老贷款未导入审批表 */
		sprintf(acErrMsg,"借据号尚未经过审批!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L177");
		ERR_DEAL;		
		}
	}else if ( ret == 0 ){
		/*新变量的赋值*/
		m_beg_date=ln_auth.beg_date;
		m_mtr_date=ln_auth.mtr_date;
		m_rate=ln_auth.rate;
		m_over_rate=ln_auth.over_rate;
		m_fine_rate=ln_auth.fine_rate;
		m_amt=ln_auth.amt;
		strcpy(m_ln_type,ln_auth.ln_type);
		strcpy(m_intst_type,ln_auth.intst_type);
		/*新变量赋值结束*/
		vtcp_log("[%s][%d] pack_no =[%s]\n",__FILE__,__LINE__,g_ln_mst.pact_no);
		/*if ( ln_auth.sts[0]=='1' )
		{	
			sprintf(acErrMsg,"借据号已存在![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L409");			
			ERR_DEAL;
		}因为下边反回担保了, Gujy JinZ 070429*/
		memset(&fd125, ' ', sizeof(fd125)); 
		sprintf(fd125.time_lmt, "%3d", ln_auth.time_lmt);
		sprintf(fd125.amt, "%16.2f", ln_auth.amt);
		sprintf(cTmp, "%6.2f", ln_auth.rate);
		memcpy(fd125.rate,cTmp,strlen(cTmp));
		memcpy(fd125.pay_ac_no,  ln_auth.pay_ac_no, strlen(ln_auth.pay_ac_no));
		memcpy(fd125.save_ac_no,  ln_auth.save_ac_no, strlen(ln_auth.save_ac_no));
		ret = Cassreg_Sel( g_pub_tx.reply , &cassreg , " pact_no='%s'" , g_ln_mst.pact_no );
		
		if( ret!=0 && ret!=100)
		{
			sprintf(acErrMsg,"读担保关系表错误![%s]",g_pub_tx.reply); 
			WRITEMSG
			strcpy(g_pub_tx.reply,"L164");
			ERR_DEAL;
		}else if(ret==100){
			fd125.cassreg.have = '0';
		}else if ( ret == 0 ){
			fd125.cassreg.have = '1'; 
			memcpy(fd125.cassreg.ac_no, cassreg.db_ac_no, strlen(cassreg.db_ac_no));
			sprintf(fd125.cassreg.bw_ac_id, "%.09ld", cassreg.bw_ac_id);
			sprintf(tmpbuf, "%16.2f", cassreg.amt);
			vtcp_log("[%s][%d] tmpbuf =[%s]\n",__FILE__,__LINE__,tmpbuf);
			memcpy(fd125.cassreg.amt, tmpbuf, 16);
			sprintf(acErrMsg,"FD125[%s]",(char*)&fd125);
			WRITEMSG
		}
		vtcp_log("[%s][%d] pack_no =[%s]\n",__FILE__,__LINE__,g_ln_mst.pact_no);
		sprintf(acErrMsg,"FD125[%s]",(char*)&fd125);
		WRITEMSG

		vtcp_log("[%s][%d]sizeof(fd125)=[%d]",__FILE__,__LINE__,sizeof(fd125));
		set_zd_data( "1250" , (char*)&fd125 );
		set_zd_data( "0260" , cassreg.name);

	}
	/*新变量传输*/
	set_zd_long("0440",m_beg_date);
	/*set_zd_long("0450",m_mtr_date);*//*已经有了*/
	set_zd_double("0840",m_rate);
	set_zd_double("0850",m_over_rate);
	set_zd_double("0970",m_fine_rate);
	set_zd_double("0390",m_amt);
	set_zd_data("0670",m_ln_type);
	set_zd_data("0300",ln_auth.save_ac_no); /** add by chenchao 20110731 **/
	set_zd_data("0310",ln_auth.pay_ac_no);	/** add by chenchao 20110731 **/
	set_zd_data("0680",ln_auth.repay_type); /** add by chenchao 20110731 **/
	set_zd_data("0240",ln_auth.prdt_no);/*产品号码*/
	set_zd_data("0910",ln_auth.cms_tx_br_no); /** 机构号 **/
	ret=Ln_parm_Sel(g_pub_tx.reply,&m_ln_parm,"prdt_no='%s' ",ln_auth.prdt_no);
	if(ret){
		sprintf(acErrMsg,"查询产品号错误");
		WRITEMSG
	  goto ErrExit;
	}	
	set_zd_data("0270",m_ln_parm.title);/*产品描述*/
	set_zd_data("0710",m_intst_type); 
	/*新变量传输结束*/
	set_zd_data( "0250" , ln_auth.name);
	vtcp_log("[%s][%d] ln_auth.mtr_date =[%ld]\n",__FILE__,__LINE__,ln_auth.mtr_date); 
	set_zd_long( "0450" , ln_auth.mtr_date);/*增加到期日*/
	set_zd_data("0330", ln_auth.trust_no);	/*增加委托协议编号*/
	sprintf(acErrMsg,"检查 合同号 PASS!");
	WRITEMSG
	

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
