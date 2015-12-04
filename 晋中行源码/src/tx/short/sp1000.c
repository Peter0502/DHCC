
/*************************************************
* 文 件 名: sp1000.c
* 功能描述：验印系统查询接口
* 作    者: zgf
* 完成日期：20131113

*交易配置：
insert into tx_def values('1000','验印系统查询接口','10000000000000010000000011111111000010000001110100000000000000000001111100000000000101000011000000000000000000000000000000000100','1','4','0');

insert into tx_sub_def values('1000','验印系统查询接口','sp1000','0','1');

insert into tx_flow_def values ('1000', 0, '1000', '#$');


接收报文域值
	客户账号      0300
	请求机构号		0020

返回报文域值
	账号					0310
	户名					0260
	地址					0250
	邮政编码			0290
	联系人				0270
	联系电话			0320
	开户日期			0440
	销户日期			0450
	启用日期			0460
	账户类型    ac_type  0680
	通兑方式	  td_kt_flag(tctd_ktzh)  0690
	网点机构号		0910
条件：
	若只有开户日期，则启用日期等于开户日期
	本机构才可查询本机构账号

*************************************************/

#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"  /**由账号，机构获取（账户ID，启用日期，止用日期）*/
#include "dd_mst_c.h"				/**由账户ID获取（开户机构，账户类型，开户日期，起息日期，有效日期，客户号，户名）**/
#include "td_mst_c.h"
#include "od_mst_c.h"
#include "ln_mst_c.h"
#include "dd_parm_c.h"				/**由账户ID获取（开户机构，账户类型，开户日期，起息日期，有效日期，客户号，户名）**/
#include "td_parm_c.h"
#include "od_parm_c.h"
#include "ln_parm_c.h"
#include "cif_addr_inf_c.h" /**由客户号获取（地址，邮政编码）***/
#include "prdt_ac_id_c.h"

sp1000()
{
    struct mdm_ac_rel_c sMdm;
    struct dd_mst_c sDd;
    struct td_mst_c sTd;
    struct od_mst_c sOd;
    struct ln_mst_c sLn;
    struct dd_parm_c sDd_parm;
    struct td_parm_c sTd_parm;
    struct od_parm_c sOd_parm;
    struct ln_parm_c sLn_parm;
    struct cif_addr_inf_c sCif;
    struct prdt_ac_id_c sPrdt;
		char cAcno[20];
		char br_no[6];
		int g_reply_int;
    
    memset(&sMdm,0x00,sizeof(sMdm));
    memset(&sDd,0x00,sizeof(sDd));
    memset(&sTd,0x00,sizeof(sTd));
    memset(&sOd,0x00,sizeof(sOd));
    memset(&sLn,0x00,sizeof(sLn));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sTd_parm,0x00,sizeof(sTd_parm));
    memset(&sOd_parm,0x00,sizeof(sOd_parm));
    memset(&sLn_parm,0x00,sizeof(sLn_parm));
    memset(&sCif,0x00,sizeof(sCif));
    memset(&sPrdt,0x00,sizeof(sPrdt));
    memset(cAcno,0x00,sizeof(cAcno));
    memset(br_no,0x00,sizeof(br_no));
    
    pub_base_sysinit();
    
 		/*获取前台数据**/
    get_zd_data("0300",cAcno);   
		if(cAcno[0]>='0' && cAcno[0]<='9'){
			pub_base_old_acno(cAcno);
		}
		
    get_zd_data("0020",br_no); 
vtcp_log("aaaaaaaaaaaaaaaaac_no[%s]--br_no[%s]!!!!!",cAcno,br_no);    
		g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm,"ac_no = '%s' and opn_br_no = '%s'",cAcno,br_no);
		if (g_reply_int)
    {
       vtcp_log("%s,%d,验证用户信息失败!",__FILE__,__LINE__);
       strcpy(g_pub_tx.reply,"CU14");
       goto ErrExit;
    }
              
  	g_reply_int=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt,"ac_id = %ld ",sMdm.ac_id);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d]查询表Prdt_ac_id_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"F039");
        goto ErrExit;
    }
		if(sPrdt.ac_id_type[0] == '1')/**活期账户**/
		{
			g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&sDd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int || sDd.ac_sts[0]=='0')
	    {
	        vtcp_log("[%s][%d]查询表Dd_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no = '%s' ",sDd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Dd_parm_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sDd.name);	/**户名**/
			set_zd_data("0270",sDd.name);	/**联系人**/
			set_zd_data("0320","");	/**联系电话**/
			set_zd_long("0440",sDd.opn_date);	/**开户日期**/
			set_zd_data("0680",sDd.ac_type);	/**账户类型ac_type**/
			set_zd_data("0690",sDd_parm.thr_draw_ind);	/**通兑方式thr_draw_ind**/
			set_zd_data("0910",sDd.opn_br_no);	/**网点机构号**/
			if(sDd.ac_sts[0] == '*')
			{
				set_zd_long("0450",sDd.lst_date);
			}
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		else if(sPrdt.ac_id_type[0] == '2')/**定期账户**/
		{
			g_reply_int=Td_mst_Sel(g_pub_tx.reply,&sTd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Td_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no = '%s' ",sTd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Td_parm_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sTd.name);	/**户名**/
			set_zd_data("0270",sTd.name);	/**联系人**/
			set_zd_data("0320","");	/**联系电话**/
			set_zd_long("0440",sTd.opn_date);	/**开户日期**/
			set_zd_data("0680","d");	/**账户类型ac_type d 活期户**/
			set_zd_data("0690",sTd_parm.thr_draw_ind);	/**通兑方式thr_draw_ind**/
			set_zd_data("0910",sTd.opn_br_no);	/**网点机构号**/
			if(sTd.ac_sts[0] == '*')
			{	
				set_zd_long("0450",sTd.lst_date);
			}	
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		else if(sPrdt.ac_id_type[0] == '3')/**贷款账户**/
		{
			g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&sLn,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Ln_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no = '%s' ",sLn.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Ln_parm_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sLn.name);	/**户名**/
			set_zd_data("0270",sLn.name);	/**联系人**/
			set_zd_data("0320","");	/**联系电话**/
			set_zd_long("0440",sLn.opn_date);	/**开户日期**/
			set_zd_data("0680","");	/**账户类型ac_type**/
			set_zd_data("0690","");	/**通兑方式thr_draw_ind**/
			set_zd_data("0910",sLn.opn_br_no);	/**网点机构号**/
			if(sLn.ac_sts[0] == '*')
			{
				set_zd_long("0450",sLn.lst_date);
			}
			else 
			{
				set_zd_long("0450",99999999);
			}
		}
		/*
		else if(sPrdt.ac_id_type[0] == '4')
		{
			g_reply_int=Od_mst_Sel(g_pub_tx.reply,&sOd,"ac_id = %ld ",sPrdt.ac_id);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Od_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
	    g_reply_int=Od_parm_Sel(g_pub_tx.reply,&sOd_parm,"prdt_no = '%s' ",sOd.prdt_no);
	    if(g_reply_int)
	    {
	        vtcp_log("[%s][%d]查询表Od_parm_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	        strcpy(g_pub_tx.reply,"F039");
	        goto ErrExit;
	    }
			set_zd_data("0260",sOd.name);	
			set_zd_data("0270",sOd.name);	
			set_zd_data("0320","");	
			set_zd_long("0440",sOd.opn_date);	
			set_zd_data("0680",sOd.ac_type);	
			set_zd_data("0690",sOd_parm.thr_draw_ind);	
			set_zd_data("0910",sOd.opn_br_no);	
		}
		*/
		g_reply_int=Cif_addr_inf_Sel(g_pub_tx.reply,&sCif,"cif_no = %ld ",sPrdt.cif_no);
	if(g_reply_int==100)
	{
	     strcpy(sCif.addr, "");
	     strcpy(sCif.post_code,"030600");
	}
    else if(g_reply_int)
    {
        vtcp_log("[%s][%d]查询表Cif_addr_inf_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"F039");
        goto ErrExit;
    }
		/** set_zd_long("0450",sMdm.end_date);	**//**销户日期**/
		set_zd_long("0460",sMdm.beg_date);	/**启用日期**/
		set_zd_data("0310",cAcno); 	/**账号**/
		set_zd_data("0250",sCif.addr);	/**地址**/
		set_zd_data("0290",sCif.post_code);	/**邮政编码**/	
	        vtcp_log("[%s][%d]查询表Dd_parm_Sel错误[%ld]",__FILE__,__LINE__,sMdm.end_date);
	        vtcp_log("[%s][%d]aaaaaaaaa[%ld]",__FILE__,__LINE__,sMdm.beg_date);
    
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
