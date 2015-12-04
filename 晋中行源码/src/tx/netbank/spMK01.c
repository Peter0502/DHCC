/*************************************************
* 版 本 号 :  
* 程序作者 :  吴海帆
* 日    期 :  2015/01/06
* 所属模块 :  手机银行自助注册
* 程序名称 :  spMK01.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5021', '手机银行自助注册', '10000000000000000000001111110100000111100000110000010000001000001111111100001010010001000000100000011000000000000000000000000000', '0','4', '0');
insert into tx_flow_def values ('5021', 0, 'MK01', '#$');
insert into tx_sub_def values ('MK01', '手机银行自助注册', 'spMK01', '0', '0');
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "cif_per_inf_c.h"		/*个人客户信息表*/
#include "cif_addr_inf_c.h"		/*客户地址信息表*/
#include "cif_email_inf_c.h"		/*客户电子地址信息表*/
#include "mob_acct_type_c.h"		/*短信通知签约解约表*/
#include "dd_mst_c.h"			/*活期存款主文件*/
#include "td_mst_c.h"			/*定期存款主文件*/
#include "mdm_ac_rel_c.h"		/*介质与账户关系表*/
#include "prdt_ac_id_c.h"		/*产品账号对照表*/
#include "com_branch_c.h"		/*公共机构码表*/

int spMK01()
{
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c g_dd_mst;
	struct td_mst_c g_td_mst;
	struct com_branch_c g_com_branch;
	struct cif_per_inf_c cif_per_inf;
	struct cif_addr_inf_c cif_addr_inf;
	struct cif_email_inf_c cif_email_inf;
	struct mob_acct_type_c sMob_acct_type;	
	
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	memset(&g_com_branch,0x00,sizeof(g_com_branch));
	memset(&cif_per_inf,0x00,sizeof(cif_per_inf));
	memset(&cif_addr_inf,0x00,sizeof(cif_addr_inf));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	
	int ret=-1;
	char ac_id_str[20]={0};
	char Ac_id_str[2000]={0};
	char sTemp[10]={0};
	
	char accountNo[32+1]; /*账号*/
	char password[16+1];  /*密码*/
	char cType[10];       /*密码类型0:查询密码 1:帐户密码*/
	char nochkpin[10];    /**密码检查标志**/
	
	memset(accountNo,0x00,sizeof(accountNo));
	memset(password,0x00,sizeof(password));
	memset(cType,0x00,sizeof(cType));
	memset(nochkpin,0x00,sizeof(nochkpin));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0300",accountNo);
	get_zd_data("0790",password);
	get_zd_data("0700",cType);
	get_zd_data("0280",nochkpin);
	
	/*检查账户*/
	pub_base_strpack(accountNo);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",accountNo);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::该账户不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","该账户不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询介质和帐户关系表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询介质和帐户关系表错误");
		goto ErrExit;
	}	
	/*vtcp_log("[%s],[%d]::证件类型=[%s],证件号码=[%s],介质状态=[%s]",__FILE__,__LINE__,g_mdm_ac_rel.id_type,g_mdm_ac_rel.id_no,g_mdm_ac_rel.note_sts);*/
	set_zd_data("0680", g_mdm_ac_rel.id_type);  /*证件类型*/
	set_zd_data("0380", g_mdm_ac_rel.id_no);    /*证件号码*/
	set_zd_data("0670", g_mdm_ac_rel.note_sts); /*介质状态*/
	
	/*核心验密*/
	if(memcmp(nochkpin,"nochkpin",8) ==0)   /**网银内管上送时不检查账户密码**/
	{
		vtcp_log("[%s][%d],账号为[%s],密码检查标志[%s],网银内管上送时不检查账户密码!",__FILE__,__LINE__,accountNo,nochkpin);
	}
	else
	{
	 	/**开始检查密码，准备条件(有些凭证件支取的账户会检查证件)**/
	 	memcpy(g_pub_tx.ac_no,	accountNo,	sizeof(g_pub_tx.ac_no));
	 	memcpy(g_pub_tx.draw_pwd,password,sizeof(g_pub_tx.draw_pwd));
	 	strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	 	strcpy(g_pub_tx.id_no,	g_mdm_ac_rel.id_no);
	 	/**********************************************************************
	 	输入：mode[4]
	 	第一位是否读取mdm_ac_rel，对g_mdm_ac_rel结构体进行赋值:	0-否；1-是
	 	第二位如果留密校验查询密码还是支取密码:	0-支取；1-查询 
	 	第三位是否需要加密传入的密码后再进行比较:0-否；1-是
	 	**********************************************************************/
		if ( '0'== cType[0] ) 
		{/*查询密码*/
			ret = -1;
			ret = pub_base_check_draw("111");
			if( ret ) 
			{
				vtcp_log("%s %d::账户密码错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"AT04");
				set_zd_data("0130","账户密码错误");
				goto ErrExit;
			}
	 	}
		else 
		{/*支取密码*/
			ret = -1;
			ret = pub_base_check_draw("101");
			if( ret ) 
			{
				vtcp_log("%s %d::账户密码错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"AT04");
				set_zd_data("0130","账户密码错误");
				goto ErrExit;
			}
		}
	}
  	 
	/**活期定期区分**/
	ret = -1;
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret || 1403 == ret ) 
	{
		vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","该账户号不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询产品账号对照表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询产品账号对照表错误");
		goto ErrExit;
	}
	
	/**查询活期产品!***/
	if (sPrdt_ac_id.ac_id_type[0] == '1')
	{/**获取账户客户名、账户状态**/
		ret = -1;
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","该账户号不存在");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::查询活期存款主文件表错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","查询活期存款主文件表错误");
			goto ErrExit;
		}
		/*vtcp_log("[%s],[%d]::客户号=[%ld],客户名=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,g_dd_mst.cif_no,g_dd_mst.name,g_dd_mst.ac_sts,g_dd_mst.hold_sts);*/
		set_zd_data("0370", g_dd_mst.name);/*客户名*/
		set_zd_data("0660", g_dd_mst.ac_sts);/*账户状态*/
		set_zd_data("0690", g_dd_mst.hold_sts);/*冻结状态*/
	}
	/**查询定期产品!***/
	else if (sPrdt_ac_id.ac_id_type[0] == '2')               
	{/**获取账户客户名、账户状态**/
		ret = -1;
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","该账户号不存在");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::查询定期存款主文件表错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","查询定期存款主文件表错误");
			goto ErrExit;
		}
		/*vtcp_log("[%s],[%d]::客户号=[%ld],客户名=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,g_td_mst.cif_no,g_td_mst.name,g_td_mst.ac_sts,g_td_mst.hold_sts);*/
		set_zd_data("0370", g_td_mst.name);/*客户名*/
		set_zd_data("0660", g_td_mst.ac_sts);/*账户状态*/
		set_zd_data("0690", g_td_mst.hold_sts);/*冻结状态*/
	}
	/*vtcp_log("[%s],[%d]::客户号=[%ld],账户类型=[%s]",__FILE__,__LINE__,sPrdt_ac_id.cif_no,sPrdt_ac_id.ac_id_type);*/
	memset(sTemp,0x00,sizeof(sTemp));
	sprintf(sTemp,"%ld",sPrdt_ac_id.cif_no);
	set_zd_data("0860",sTemp);  /**客户号**/
	set_zd_data("0240", sPrdt_ac_id.ac_id_type);  /*账户类型*/
	
	/**获取开户行行名行号**/
	ret = -1;
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::该机构号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O116");
		set_zd_data("0130","该机构号不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询公共机构码表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O092");
		set_zd_data("0130","查询公共机构码表错误");
		goto ErrExit;
	}
	/*vtcp_log("[%s],[%d]开户行号=[%s],开户行名=[%s]",__FILE__,__LINE__,g_com_branch.br_no,g_com_branch.br_name);*/
	set_zd_data("0770", g_mdm_ac_rel.opn_br_no);  /*开户行号-开户网店*/
	set_zd_data("0820", g_com_branch.br_name);    /*网店名称*/
	
	/* 根据客户号查询客户性别*/
	ret = -1;
	ret = Cif_per_inf_Sel(g_pub_tx.reply, &cif_per_inf,"cif_no='%ld'",sPrdt_ac_id.cif_no);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::取客户证件信息异常!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","取客户证件信息异常");
		goto ErrExit;
	}
	/*vtcp_log("[%s],[%d]::性别=[%s]",__FILE__,__LINE__,cif_per_inf.sex);*/
	set_zd_data("0720", cif_per_inf.sex);/*性别*/
	
	/*根据客户号查询客户地址和邮政编码*/
	ret = -1;
	ret = Cif_addr_inf_Sel(g_pub_tx.reply, &cif_addr_inf,"cif_no='%ld' and addr_type='2'",sPrdt_ac_id.cif_no);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::取客户地址信息异常!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","取客户地址信息异常");
		goto ErrExit;
	}  
	vtcp_log("[%s],[%d]::联系地址=[%s],邮政编码=[%s]",__FILE__,__LINE__,cif_addr_inf.addr,cif_addr_inf.post_code);                     
	set_zd_data("0270", cif_addr_inf.addr);/*联系地址*/
	set_zd_data("0640", cif_addr_inf.post_code);/*邮政编码*/
	
	/*根据客户号查询客户联系电话*/
	memset(&cif_email_inf, 0x00, sizeof(cif_email_inf));
	ret = -1;
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,"cif_no ='%ld' and addr_type = '1'", sPrdt_ac_id.cif_no);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::取客户电子地址信息异常!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","取客户电子地址信息异常");
		goto ErrExit;
	}
	/*vtcp_log("[%s],[%d]::联系电话=[%s]",__FILE__,__LINE__,cif_email_inf.email);*/
	set_zd_data("0590", cif_email_inf.email);/*联系电话*/
	
	/* 根据客户号查询客户邮箱*/
	memset(&cif_email_inf, 0x00, sizeof(cif_email_inf));
	ret = -1;
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf," cif_no ='%ld' and addr_type = '6'", sPrdt_ac_id.cif_no);
	if (ret != 100 && ret != 0) 
	{
		vtcp_log("%s %d::取客户电子地址信息异常!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","取客户电子地址信息异常");
		goto ErrExit;
	}
	/*vtcp_log("[%s],[%d]::电子邮件=[%s]",__FILE__,__LINE__,cif_email_inf.email);*/
	set_zd_data("0260", cif_email_inf.email);/*电子邮件*/
	
	/*取客户建立id时保存的手机号*/
	memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
   ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf,
                           " cif_no = %ld and addr_type = '5'", sPrdt_ac_id.cif_no);
   if (ret != 100 && ret != 0)
   {
			sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			set_zd_data("0130","取客户电子地址信息异常");
			goto ErrExit;
   }       
   set_zd_data("0610",cif_email_inf.email);   /*手机号*/
	
        
	/* 根据客户号查询客户手机号码*/
	/*查询dd_mst表，客户号对应的账号*/
/*	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	ret = -1;
	ret = Dd_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld' ",sPrdt_ac_id.cif_no);
	if( ret )
	{
		vtcp_log("[%s][%d]查询dd_mst错误![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		set_zd_data("0130","查询活期存款主文件异常");
		goto ErrExit;
	}
	while(1)
	{
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if(ret==100 || ret == 1403)
		{
			break;
		}
		else if( 0 != ret  && 100 != ret)
		{
			vtcp_log("[%s],[%d] 查询dd_mst异常!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W015");
			set_zd_data("0130","查询活期存款主文件异常");
			goto ErrExit;
		}
		else 
		{
			sprintf(ac_id_str,"%d,",g_dd_mst.ac_id);
			strcat(Ac_id_str,ac_id_str);
		}
	}
	Dd_mst_Clo_Sel();

	Ac_id_str[strlen(Ac_id_str)-1]='\0';
	vtcp_log("[%s],[%d] Ac_id_str=[%s]!",__FILE__,__LINE__,Ac_id_str);
	
	查询短信银行客户账号与服务品种对应表
	ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_id in (%s) and mob_sts <> '3' ",Ac_id_str);
	if(ret == 100)
	{
		vtcp_log("[%s],[%d] 此客户未做过短信签约或已解约!",__FILE__,__LINE__);
	}
	else if( ret )
	{
		vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","查询短信签约表异常");
		goto ErrExit;
	}
	else
	{
		vtcp_log("[%s],[%d]::手机号码=[%s]",__FILE__,__LINE__,sMob_acct_type.mobile);  
		set_zd_data("0610",sMob_acct_type.mobile);  手机号码
	}
*/

OkExit:
        strcpy(g_pub_tx.reply, "0000");
        vtcp_log("[%s],[%d]::Before OK return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        vtcp_log("[%s],[%d]::Before return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}

