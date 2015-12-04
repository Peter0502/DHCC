/***************************************************************************
* 文 件 名：spEB06.c
* 功能描述：网银账户验证(密码、证件)
* 作    者：Han Xichao
* 完成日期：2010-10-19
* 所属模块：鹤壁网银
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "com_branch_c.h"

int spEB06()
{
	const	char * trs_desc="网银账户验证";
	int	  ret=0;
	char	cAc_no[25]={0};
	char	cDraw_pwd[7]={0};	
	char	cType[2]={0};
	char  Id_type[2];
	char  Id_no[21];
	char  nochkpin[10];
	char  Name[60+1];
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c	g_dd_mst;
	struct td_mst_c	g_td_mst;
	struct com_branch_c	g_com_branch;
	
	memset(Name,0x00, sizeof(Name));
	memset(cAc_no,0x00, sizeof(cAc_no));
	memset(cDraw_pwd,0x00, sizeof(cDraw_pwd));
	memset(cType,0x00, sizeof(cType));
	memset(Id_type,0x00, sizeof(Id_type));
	memset(Id_no,0x00, sizeof(Id_no));
	memset(nochkpin,0x00, sizeof(nochkpin));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	
	vtcp_log("[%s],[%d],[%s]处理开始...",__FILE__,__LINE__,trs_desc);
	pub_base_sysinit();              /*初始化公共变量*/	
	get_zd_data("0300",cAc_no);      /*账号*/
	get_zd_data("0790",cDraw_pwd);   /*密码*/
	get_zd_data("0700",cType);       /*密码类型0:查询密码 1:帐户密码*/
	get_zd_data("0630",Id_no);       /**证件号码**/
	get_zd_data("0680",Id_type);     /**证件类型**/
	get_zd_data("0280",nochkpin);     /**密码检查标志**/
	get_zd_data("0250",Name);     /**客户姓名**/
	
	pub_base_strpack(cAc_no);
	vtcp_log("[%s][%d],账号为[%s],密码类型[%c],证件类型[%s],证件号码[%s],密码检查标志[%s]",__FILE__,__LINE__,cAc_no,cType[0],Id_type,Id_no,nochkpin);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
	if( 100==ret ) {
		sprintf(acErrMsg,"该账户不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if( ret ) {
		sprintf(acErrMsg,"查询介质和帐户关系表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	/**证件类型证件号码校验**/
	if(memcmp(Id_type,g_mdm_ac_rel.id_type,strlen(Id_type)))
		{
			sprintf(acErrMsg,"[%s],[%d]证件类型不符!输入Id_type=[%s],表中Id_type=[%s]",__FILE__,__LINE__,Id_type,g_mdm_ac_rel.id_type);
		  WRITEMSG
		  strcpy(g_pub_tx.reply,"P210");
		  goto ErrExit;
		}
	if(memcmp(Id_no,g_mdm_ac_rel.id_no,strlen(Id_no)))
		{
		  sprintf(acErrMsg,"[%s],[%d]证件号码不符!输入Id_no=[%s],表中Id_no=[%s]",__FILE__,__LINE__,Id_no,g_mdm_ac_rel.id_no);
		  WRITEMSG
		  strcpy(g_pub_tx.reply,"M004");
		  goto ErrExit;
		}
		
	if(memcmp(nochkpin,"nochkpin",8) ==0)   /**网银内管上送时不检查账户密码**/
		{
			vtcp_log("[%s][%d],账号为[%s],密码检查标志[%s],网银内管上送时不检查账户密码!",__FILE__,__LINE__,cAc_no,nochkpin);
		}
		else
		{
	      /**开始检查密码，准备条件(有些凭证件支取的账户会检查证件)**/
	      memcpy(g_pub_tx.ac_no,	cAc_no,	sizeof(g_pub_tx.ac_no));
	      memcpy(g_pub_tx.draw_pwd,cDraw_pwd,sizeof(g_pub_tx.draw_pwd));
	      strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	      strcpy(g_pub_tx.id_no,	g_mdm_ac_rel.id_no);
	      /**********************************************************************
	      输入：mode[4]
	      第一位是否读取mdm_ac_rel，对g_mdm_ac_rel结构体进行赋值:	0-否；1-是
	      第二位如果留密校验查询密码还是支取密码:	0-支取；1-查询 
	      第三位是否需要加密传入的密码后再进行比较:0-否；1-是
	      **********************************************************************/
	
	     if ( '0'== cType[0] ) {            /*查询密码*/
	    	ret = pub_base_check_draw("111");
	    	if( ret ) {
	    		sprintf( acErrMsg, "[%s],[%d],账户密码错误!",__FILE__,__LINE__);
	    		WRITEMSG
	    		goto ErrExit;
	    	}
	     } else {                          /*支取密码*/
	    	ret = pub_base_check_draw("101");
	    	if( ret ) {
	    		sprintf( acErrMsg, "[%s],[%d],账户密码错误!",__FILE__,__LINE__);
	    		WRITEMSG
	    		goto ErrExit;
	    	}
	    }
   }
	/**活期定期区分**/
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret ) {
	   sprintf(acErrMsg,"该账户号不存在!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"D104");
	   goto ErrExit;
	  }
	else if( ret ) {
	   sprintf(acErrMsg,"查询产品账号对照表错误!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"AT06");
	   goto ErrExit;
	  }
	if (sPrdt_ac_id.ac_id_type[0] == '1')                   /**查询活期产品!***/
		{
	     /**获取账户客户名、账户状态、账户类型**/
	     ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) {
	     	sprintf(acErrMsg,"该账户号不存在!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	goto ErrExit;
	     }
	     else if( ret ) {
	     	sprintf(acErrMsg,"查询活期存款主文件表错误!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	     if(strcmp(Name,g_dd_mst.name)!= 0)
	     {
	       	sprintf(acErrMsg,"[%s],[%d]客户姓名不符!输入name=[%s],表中g_dd_mst.name=[%s]",__FILE__,__LINE__,Name,g_dd_mst.name);
		  		WRITEMSG
		  		strcpy(g_pub_tx.reply,"D249");
		 			goto ErrExit;
	     }
	     sprintf(acErrMsg,"[%s],[%d]Name=[%s],账户客户名=[%s],账户类型=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,Name,g_dd_mst.name,g_dd_mst.prdt_no,g_dd_mst.ac_sts,g_dd_mst.hold_sts);
       WRITEMSG
       set_zd_data("0250", g_dd_mst.name);
	     set_zd_data("0340", g_dd_mst.prdt_no);
       set_zd_data("0660", g_dd_mst.ac_sts);
	     set_zd_data("0690", g_dd_mst.hold_sts);
    }
  else if (sPrdt_ac_id.ac_id_type[0] == '2')               /**查询定期产品!***/
		{
	     /**获取账户客户名、账户状态、账户类型**/
	     ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) {
	     	sprintf(acErrMsg,"该账户号不存在!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	goto ErrExit;
	     }
	     else if( ret ) {
	     	sprintf(acErrMsg,"查询活期存款主文件表错误!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	     
	     if(strcmp(Name,g_td_mst.name)!= 0)
	     {
	       	sprintf(acErrMsg,"[%s],[%d]客户姓名不符!输入name=[%s],表中g_td_mst.name=[%s]",__FILE__,__LINE__,Name,g_td_mst.name);
		  		WRITEMSG
		  		strcpy(g_pub_tx.reply,"D249");
		 			goto ErrExit;
	     }
	     
	     sprintf(acErrMsg,"[%s],[%d]Name=[%s],账户客户名=[%s],账户类型=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,Name,g_td_mst.name,g_td_mst.prdt_no,g_td_mst.ac_sts,g_td_mst.hold_sts);
       WRITEMSG
       set_zd_data("0250", g_td_mst.name);
	     set_zd_data("0340", g_td_mst.prdt_no);
       set_zd_data("0660", g_td_mst.ac_sts);
	     set_zd_data("0690", g_td_mst.hold_sts);
    }
    set_zd_double("0280",sPrdt_ac_id.cif_no);/**客户号**/
	/**获取开户行行名行号**/
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if( 100==ret ) {
		sprintf(acErrMsg,"该机构号不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if( ret ) {
		sprintf(acErrMsg,"查询公共机构码表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	sprintf(acErrMsg,"[%s],[%d]开户行号=[%s],开户行名=[%s]，账号类型=[%s]",__FILE__,__LINE__,g_com_branch.br_no,g_com_branch.br_name,sPrdt_ac_id.ac_id_type);
  WRITEMSG
	set_zd_data("0240", sPrdt_ac_id.ac_id_type);
	set_zd_data("0350", g_mdm_ac_rel.mdm_code);
	set_zd_data("0670", g_mdm_ac_rel.note_sts);
	set_zd_data("0770", g_mdm_ac_rel.opn_br_no);
	set_zd_data("0820", g_com_branch.br_name);
	
OkExit:
	vtcp_log("[%s],[%d],[%s]成功!",__FILE__,__LINE__,trs_desc);
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;

ErrExit:
	vtcp_log("[%s],[%d],[%s]失败!",__FILE__,__LINE__,trs_desc);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
