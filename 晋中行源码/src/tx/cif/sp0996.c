/*************************************************
* 文 件 名:  sp0996.c
* 功能描述： 平台要查询客户ID
*
* 作    者:  xyy
* 完成日期： 2007-08-03
*
* 修改记录：martin
* 日   期:  2009-12-22
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
        sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
        }
#define EXTERN
#define TX_CODE "0990"
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"

int sp0996()
{
    int i=0;
    int j=0;
    int iNum=0;
    int ret=0;
    long lCifno=0;
    char cTxtype[2];
    char cZjtype[2];
    char cCif_type[2];
    char cDGtmp_type[2];
    char cDGtmp_idno[21];
    char cId_no[21];
    char cCifname[61];
    char cEnt_attri[2];
    struct cif_id_code_rel_c cif_id_code_rel;
    struct cif_id_code_rel_c f_cif_id_code_rel;
    struct cif_basic_inf_c g_cif_basic_inf;
    
    memset(cDGtmp_idno ,0,sizeof(cDGtmp_idno));
    memset(cDGtmp_type ,0,sizeof(cDGtmp_type)); 
    memset(cTxtype     ,0,sizeof(cTxtype));
    memset(cZjtype     ,0,sizeof(cZjtype));
    memset(cCif_type   ,0,sizeof(cCif_type));
    memset(cId_no      ,0,sizeof(cId_no));
    memset(cCifname    ,0,sizeof(cCifname));
    memset(cEnt_attri  ,0,sizeof(cEnt_attri));
    memset(&g_pub_tx   ,0,sizeof(g_pub_tx) );
    memset(&cif_id_code_rel  ,0,sizeof(cif_id_code_rel));
    memset(&f_cif_id_code_rel,0,sizeof(f_cif_id_code_rel));
    memset(&g_cif_basic_inf  ,0,sizeof(struct cif_basic_inf_c));
    vtcp_log("[%s][%d]开始调用sp0996\n",__FILE__,__LINE__);
    
    pub_base_sysinit();
    get_zd_data("0670",cTxtype); /**1:个人 2:公司**/
    vtcp_log("[%s][%d]cTxtype==[%s]\n",__FILE__,__LINE__,cTxtype);
    /***开始对平台传来的域进行判断***/
    if(cTxtype[0]=='1')/**1:个人**/
    {
    	get_zd_data("0680",cCif_type );
      vtcp_log("[%s][%d]cCif_type==[%s]\n",__FILE__,__LINE__,cCif_type);
					/*1身份证2户口簿3护照4军官证5回乡证6士兵证7港澳居民来往通行证         
					E临时身份证F外国人居留证H:其他G警官证
					*/
    }
    else if(cTxtype[0]=='2')/**2:公司**/
    {
			get_zd_data("0680",cDGtmp_type);
			get_zd_data("0620",cDGtmp_idno);
			vtcp_log("%s,%d对公的证件类型[%s]",__FILE__,__LINE__,cDGtmp_type);
			vtcp_log("%s,%d对公的证件号码[%s]",__FILE__,__LINE__,cDGtmp_idno);
    }
    get_zd_data("0250",cCifname);	/* 客户名称 */
    vtcp_log("[%s][%d]cCifname==[%s]\n",__FILE__,__LINE__,cCifname);
    if(cTxtype[0]=='1')/***查询个人的客户id***/
    {
			get_zd_data("0250",cCifname);	/* 客户名称 */
    	  switch(cCif_type[0])
    	  {
    	     case '1':/*核对已有的证件类型 2009/12/22 17:38:40 martin case*/
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case 'E':
             case 'F':
             case 'G':
             case 'H':
                 get_zd_data("0620",cId_no );  /*证件号码-个人*/
                 get_zd_data("0250",cCifname);
                 pub_base_strpack(cCifname);
                 vtcp_log("[%s][%d]id_no==[%s]\n",__FILE__,__LINE__,cId_no);
                 vtcp_log("[%s][%d]cCifname=[%s]\n",__FILE__,__LINE__,cCifname);
                 if(strlen(cId_no))
                 {
                    ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel,"id_type='%c' and id_no='%s'", \
                                           cCif_type[0], cId_no);
                   if(ret)
                   {
                       sprintf(acErrMsg,"无匹配的纪录!! ");
                       WRITEMSG
                       strcpy(g_pub_tx.reply,"0001");
                       goto OKExit;/*去0997开客户 2009/12/22 17:40:49 martin */
                   }
                   else
                   {
                     ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld",f_cif_id_code_rel.cif_no);
                     if(ret)
                     {
                      sprintf(acErrMsg,"查找客户基本信息表错误!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P135");
                      goto ErrExit;
                     }       
                     if(!memcmp(cCifname,g_cif_basic_inf.name,strlen(g_cif_basic_inf.name)))
                     {
                         vtcp_log("[%s][%d]lCifno====[%ld]\n",__FILE__,__LINE__,f_cif_id_code_rel.cif_no); 
                         set_zd_long("0280",f_cif_id_code_rel.cif_no);
                         goto GoodExit; /*已经找到记录返回成功 2009/12/22 17:40:49 martin */
                     }else 
                     {
                           	sprintf(acErrMsg,"客户名称不一致!");
                      	WRITEMSG
                      	strcpy(g_pub_tx.reply,"CD26");
                      	goto ErrExit;
                     }
                   }
                }
                break;
             default:
                   vtcp_log("[%s][%d]error ! cCif_type=[%s]\n",__FILE__,__LINE__,cCif_type);
                   sprintf(acErrMsg,"错误的个人业务类型!! ");
                   WRITEMSG
                   strcpy(g_pub_tx.reply,"0999");
                   goto ErrExit;
    	  }
    }
    else if(cTxtype[0]=='2')/**查询对公的客户id**/
    {
        if(cDGtmp_type[0]=='8' ||cDGtmp_type[0]=='A')
        {
        	vtcp_log("[%s][%d]cZjtype==[%s]\n",__FILE__,__LINE__,cZjtype);
        }
        else
        {
          sprintf(acErrMsg,"错误的公司业务类型!! ");
          WRITEMSG
          strcpy(g_pub_tx.reply,"0999");
          goto ErrExit;
    	  }
    	  
    	  if(strlen(cDGtmp_idno)==0)
    	  {
    	    vtcp_log("[%s][%d]MS传入id_no=[%s]长度为零\n",__FILE__,__LINE__,cId_no);
    	    strcpy(g_pub_tx.reply,"0999");
    	    goto ErrExit;
    	  }
        else/*ID_NO判断长度不为零*/
        {
             vtcp_log("[%s][%d]id_no=[%s],cDGtmp_type=[%c]\n",__FILE__,__LINE__,cDGtmp_idno,cDGtmp_type[0]);
             ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel,"id_type='%c' and id_no='%s'", \
                                 cDGtmp_type[0], cDGtmp_idno);
             if(ret!=100 && ret!=0)
             {
             	sprintf(acErrMsg,"您输入的帐号和证件类型错误!! ");
             	WRITEMSG
             	strcpy(g_pub_tx.reply,"0999");
             	goto ErrExit;
             }
             else if(ret==100)
             {
             	vtcp_log("[%s][%d]没有找到对应的记录",__FILE__,__LINE__,ret);
             	sprintf(acErrMsg,"无匹配的纪录!! ");
             	strcpy(g_pub_tx.reply,"0001");
             	goto OKExit;/*去0997开客户 2009/12/22 17:40:49 martin */
             }
             else
             {
             	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld",f_cif_id_code_rel.cif_no);
             	if(ret){
             		sprintf(acErrMsg,"查找客户基本信息表错误!");
             		WRITEMSG
             		strcpy(g_pub_tx.reply,"P135");
             		goto ErrExit;
             	}
             	if(g_cif_basic_inf.sts[0]=='2')
             	{
             		vtcp_log("[%s][%d]没有找到对应的记录",__FILE__,__LINE__,ret);
             		sprintf(acErrMsg,"客户已为注销状态!! ");
             		strcpy(g_pub_tx.reply,"C225");
             		goto ErrExit;
             	}
             	vtcp_log("%s,%d,看看客户号码到底是什么[%s]",__FILE__,__LINE__,cCifname);
             	if(!memcmp(cCifname,g_cif_basic_inf.name,strlen(g_cif_basic_inf.name)))/*比较客户名称**/
             	{
                 vtcp_log("[%s][%d]lCifno====[%ld]\n",__FILE__,__LINE__,f_cif_id_code_rel.cif_no); 
                 set_zd_long("0280",f_cif_id_code_rel.cif_no);
                 set_zd_data("0160",TX_CODE);
                 goto GoodExit;/*已经找到记录返回成功 2009/12/22 17:40:49 martin */
              }else 
              {
                  sprintf(acErrMsg,"客户名称不一致!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply,"CD26");
                  goto ErrExit;
              }
             }
         vtcp_log("[%s][%d]跟新客户证件与客户号对照表成功\n",__FILE__,__LINE__);
     }
     vtcp_log("[%s][%d]id_no====[%s]\n",__FILE__,__LINE__,cId_no);
    }
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OKExit:
    sprintf(acErrMsg,"Before OK1 return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit:
    sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 2;
}
