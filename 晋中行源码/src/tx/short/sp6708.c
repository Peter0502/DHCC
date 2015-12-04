/****************************************************
* 文 件 名:    sp6708.c
* 功能描述:    根据帐号回显证件证件类型和证件号
*
* 作    者:    wanglei
* 完成日期：   2007年05月22日
* 修改记录：   
*     1. 日    期: 2008/12/8 9:56
*        修 改 人: MARTIN
*        修改内容: 新增户名及客户号 增加操作根据客户号查询证件及客户名称
*                 
*****************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"

sp6708()  
{
	int  ret=0;
	long cif_no=0;
	char do_typ[2];
	char cif_typ[2];
	char ac_no[20];
	char cId_type[2];   /****证件类型****/
	char cId_no[21];    /****证件号码****/
	char name[61];      /*客户名称*/
	
	struct dd_mst_c               g_dd_mst;
	struct td_mst_c               g_td_mst;
	struct in_mst_c               g_in_mst;
	struct prdt_ac_id_c           sPrdt_ac_id;
	struct cif_basic_inf_c        sCif_basic_inf;
	struct cif_id_code_rel_c      sCif_id_code_rel;

	/** 取值、赋值 **/	
	get_zd_data("0670",do_typ);							/*操作 1: 帐号查询  ; 2: 客户号查询*/
	if(do_typ[0]=='1')/*根据帐号查询*/
	{
	   get_zd_data("0300",ac_no);							/*帐号*/
	   get_zd_int ("0340",&g_pub_tx.ac_seqn); /*账户序号*/
	   
	   ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	   if( ret==100 )
	   {
	       sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	       WRITEMSG
	    	strcpy(g_pub_tx.reply,"M003");
	   	goto ErrExit;
	   }
	   else if(ret)
	   {
	   	sprintf(acErrMsg,"查找介质帐户关系表出错! ret=[%d]",ret);
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
	   	goto ErrExit;
	   }
	   
	   memset( &sPrdt_ac_id, 0x00, sizeof(sPrdt_ac_id) );
	   ret = Prdt_ac_id_Sel( g_pub_tx.reply, &sPrdt_ac_id, \
		 "ac_id = '%ld' order by ac_id_type", g_mdm_ac_rel.ac_id );
	   if( ret != 0 )
	   {
	   	strcpy( g_pub_tx.reply,"W011" );
	   	goto ErrExit;
	   }
	   
	   /* 帐户信息 */
	   /*根据账户类型到相应的表中作更新*/
	   switch( sPrdt_ac_id.ac_id_type[0] )
	   {
	   	case '1':                                          /*1-活期*/
	   			/**ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);**/
	   			ret=Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", g_mdm_ac_rel.ac_id);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"帐户不存在!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"查找活期主表出错[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_dd_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_dd_mst.name=[%s] \n",__FILE__,__LINE__,g_dd_mst.name);
	   			break;
	   case '2':												/*2-定期*/
	   			ret=Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"帐户不存在!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"查找定期主表出错[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_td_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_td_mst.name=[%s] \n",__FILE__,__LINE__,g_td_mst.name);
	   			break;
	   	case '9':
	   			ret=In_mst_Sel(g_pub_tx.reply, &g_in_mst, "ac_id = %ld  and ac_seqn = %d", \
	   							g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	   			if( ret==100 )
	   			{
	    				sprintf(acErrMsg,"帐户不存在!");
	     				WRITEMSG
	   				strcpy(g_pub_tx.reply,"M003");
	       			goto ErrExit;
	   			}
	   			else if( ret )
	   			{
	   				sprintf(acErrMsg,"查找内部主表出错[%d]",ret);
	   				WRITEMSG
	   				strcpy(g_pub_tx.reply,"D103");
	   				goto ErrExit;
	   			}
	   			strcpy(name,g_in_mst.name);
	   			
	   			vtcp_log("[%s][%d] g_in_mst.name=[%s] \n",__FILE__,__LINE__,g_in_mst.name);
	   			break;
	   	default:
	   			break;
	   } 
	   /*switch结束*/
	   
	   
     /* 客户基本信息 */
	   memset( &sCif_basic_inf, 0x00, sizeof(sCif_basic_inf) );
	   ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCif_basic_inf, \
	   	"cif_no = '%ld' and sts='1'", sPrdt_ac_id.cif_no );
	   if( ret != 0 )
	   {
	   	strcpy( g_pub_tx.reply,"W011" );
	   	goto ErrExit;
	   }
     
     /* 根据客户号查询客户证件 */	
	   ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		 "cif_no=%ld", sPrdt_ac_id.cif_no);
	   if (ret != 0 && ret != 100)
	   {
	   	sprintf(acErrMsg, "客户证件与客户号对照表错误!! [%d]", ret);
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply, "C002");
	   	goto ErrExit;
	   }
	   else if (ret == 100)
	   {
	   	sprintf(acErrMsg, "客户证件与客户号对照表没有此记录!!");
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply, "B115");
	   	goto ErrExit;
	   }
	   
	   sprintf(acErrMsg,"ac_id is [%ld];cif_no is [%ld]-------------", g_mdm_ac_rel.ac_id, sPrdt_ac_id.cif_no );
	   WRITEMSG
     
	   cif_no = sPrdt_ac_id.cif_no;
	
	   /****
	   if( pub_cif_GetCifType ( cif_no , &cif_typ ) )
	   {
	   	sprintf(acErrMsg,"查找客户类型失败!");
	   	WRITEMSG
	   	goto ErrExit;
	   }
	   ****/
	   strcpy(cif_typ,sCif_basic_inf.type);  /****在此处给客户类型赋值，上面函数不起作用****/
	   
	   strcpy(cId_type,g_mdm_ac_rel.id_type);
	   strcpy(cId_no,g_mdm_ac_rel.id_no);
	   
	   /* 输出变量 */
	   set_zd_data("0200",sCif_id_code_rel.id_type);/*证件类型*/
	   set_zd_data("0250",sCif_basic_inf.name);     /*客户名称*/
	   set_zd_data("0260",name);                    /*帐户客户名称*/
	   set_zd_long("0290",cif_no);                  /*客户号*/
	   set_zd_data("0620",sCif_id_code_rel.id_no);  /*证件号码*/
	   set_zd_data("0630",cId_no);                  /*帐户证件号码*/
	   set_zd_data("0680",cif_typ);                 /*客户类型*/
	   set_zd_data("0690",cId_type);                /*帐户证件类型*/
	   
	}else
	if(do_typ[0]=='2')/*根据客户号查询*/
	{
		get_zd_long("0280",&cif_no);							/*客户号*/
		get_zd_data("0680",cif_typ);							/*客户类型*/
		
		memset (&sCif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		memset (&sCif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	  sprintf(acErrMsg,"客户号[%d]",cif_no);
	  WRITEMSG
    
	  					  
	  /* 根据客户号查询客户类型和名称 */			  
	  ret = Cif_basic_inf_Sel( g_pub_tx.reply , &sCif_basic_inf , 
	  				" cif_no=%ld and sts='1' " , cif_no );
	  if( ret==100 )
	  {
	  	sprintf(acErrMsg,"该客户号不存在[%s][%ld]",g_pub_tx.reply,cif_no);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply,"C007");
	  	goto ErrExit;
	  }
	  else if( ret )
	  {
	  	sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply,"C006");
	  	goto ErrExit;
	  }
	  
	  /*判断客户类型是否一致*/
	  if(sCif_basic_inf.type[0]!=cif_typ[0])
	  	{
	  		sprintf(acErrMsg,"该客户类型不符合[%s]cif_no=[%ld]cif_type=[%s]",g_pub_tx.reply,cif_no,sCif_basic_inf.type);
	  	  WRITEMSG
	  		strcpy(g_pub_tx.reply,"D112");
	  	  goto ErrExit;
	  	}
	  
	  /* 根据客户号查询客户证件 */	
	  ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", cif_no);
	  if (ret != 0 && ret != 100)
	  {
	  	sprintf(acErrMsg, "客户证件与客户号对照表错误!! [%d]", ret);
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply, "C002");
	  	goto ErrExit;
	  }
	  else if (ret == 100)
	  {
	  	sprintf(acErrMsg, "客户证件与客户号对照表没有此记录!!");
	  	WRITEMSG
	  	strcpy(g_pub_tx.reply, "B115");
	  	goto ErrExit;
	  }
	  
	   set_zd_data("0200",sCif_id_code_rel.id_type);       /*证件类型*/
	   set_zd_data("0250",sCif_basic_inf.name);           /*客户名称*/
	   set_zd_data("0620",sCif_id_code_rel.id_no);         /*证件号码*/
		
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

