/*************************************************
* 文 件 名: sp1234.c
* 功能描述：向代理人信息表中插入数据 agent_info
* 作    者: wudawei
* 完成日期：2015年03月20日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "agent_info_c.h"

sp1234()  
{ 		
	int ret=0, i=0;
	struct agent_info_c sAgent_info;
	char ac_no[20],id_no[21],id_type[2],phone[30],tmp[60];
	char fldstr[2][30];
	/*
			$FD27=#AGENTNAME;
			$FD68=#ATCETYPE;
			$FD63=#ATCENUM;			
			$FD83=#ATCEPHONE;
	*/
	memset(&sAgent_info,0x00,sizeof(struct agent_info_c));
	pub_base_sysinit();
	memset(ac_no,0x00,sizeof(ac_no));
	memset(id_no,0x00,sizeof(id_no));
	memset(id_type,0x00,sizeof(id_type));
	memset(phone,0x00,sizeof(phone));
	memset(fldstr,0x00,sizeof(fldstr));
	memset(tmp,0x00,sizeof(tmp));
	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);						/*帐号*/
	/**get_zd_data("0270",agent_name); **/					/*代理人姓名*/
	get_zd_data("0680",id_type);					/*证件类型*/
	get_zd_data("0630",id_no);					    /*证件号码*/
	get_zd_data("0830",tmp);					    /*电话号码*/
	
	for( i=0; i<2; i++ )
	{
			ret=pub_base_cut_str( tmp,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
	}

	strcpy(sAgent_info.ac_no, ac_no);
	strcpy(sAgent_info.type, "1");    /*开IC卡 代理人*/
	strcpy(sAgent_info.agent_name, fldstr[1]); 
	strcpy(sAgent_info.id_type, id_type);
	strcpy(sAgent_info.id_no, id_no);
	strcpy(sAgent_info.phone, fldstr[0]);
	
	 vtcp_log("sAgent_info.ac_no=【%s】",sAgent_info.ac_no);
	 vtcp_log("sAgent_info.agent_name=【%s】",sAgent_info.agent_name);
	 vtcp_log("sAgent_info.id_type=【%s】",sAgent_info.id_type);
	 vtcp_log("sAgent_info.id_no=【%s】",sAgent_info.id_no);
	 vtcp_log("sAgent_info.phone=【%s】",sAgent_info.phone);

	strcpy(sAgent_info.br_no,  g_pub_tx.tx_br_no); 
	strcpy(sAgent_info.tel, g_pub_tx.tel);
	sAgent_info.tx_date = g_pub_tx.tx_date ;

	if(strlen(sAgent_info.agent_name)!=0){
		vtcp_log("进入代理人登记部分..." );
	 ret = Agent_info_Ins( sAgent_info,   g_pub_tx.reply );
                        if( ret )
                        {
                                sprintf(acErrMsg,"登记代理人信息表错误![%d]",ret);
                                WRITEMSG
                                goto ErrExit;
                        }
     vtcp_log("代理人登记成功..." );
	}

	
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
   
