/*************************************************
* 文  件名:  sp9668.c
* 功能描述: 获取客户信息，包含图片信息�
* 作    者: liuyong
* 完成日期: 2009年08月15日
* 修改记录:   
*     1.日    期:
*       修 改 人:
*       修改内容:
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "stdlib.h" 
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#define  EXTERN
#include "public.h"
#include "pub_tx.h"
#include "cif_photo_inf_c.h"
#include "cif_cop_photo_c.h"
#include "cif_basic_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_cop_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "dic_data_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;
static struct cif_basic_inf_c cif_basic_inf;
static struct cif_per_inf_c   cif_per_inf;
static struct cif_cop_inf_c   cif_cop_inf;
static struct cif_id_code_rel_c cif_id_code_rel;
static struct cif_addr_inf_c cif_addr_inf;
static struct cif_email_inf_c cif_email_inf;
static struct dic_data_c dic_data;

sp9668()
{
        int ret = 0;
	char filename[100];
	char tmpfile[100];
	char cifno[20];
        char ttyno[5];
        char brno[10];
        char idno[20];
        char idtypes[51];
        char idtyp[2];  /*证件类型*/
  
        /** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
	memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
        memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));
        memset(&cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
        memset(&cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
        memset(filename,0x00,sizeof(filename));
	memset(tmpfile,0x00,sizeof(tmpfile));
	memset(cifno,0x00,sizeof(cifno));
	memset(ttyno,'\0',sizeof(ttyno));
        memset(brno,'\0',sizeof(brno));
        memset(idtypes,'\0',sizeof(idtypes));
        memset(idtyp,'\0',sizeof(idtyp));

        /** 取值、赋值 **/
        get_zd_data("0620",idno); /*有效证件号码*/
        get_zd_data("0620",cif_id_code_rel.id_no);
        get_zd_data("0670",idtyp);
        get_zd_data("0670",cif_id_code_rel.id_type);
        get_zd_long("0280",&cif_photo_inf.cif_no);	/* 客户号 */
        get_zd_data("0720",cif_photo_inf.photo_type);       /*图片类型*/
        get_zd_long("0280",&cif_cop_photo.cif_no);	/* 客户号 */
        get_zd_data("0720",cif_cop_photo.photo_type);       /*图片类型*/
	get_zd_data(DC_TTY,ttyno);            /*柜员号*/
        get_zd_data(DC_TX_BR_NO,brno);        /*机构号*/
        get_zd_data("0280",cifno);
       	
        /** 处理流程 **/
        sprintf(acErrMsg,"客户号：[%ld]",cif_photo_inf.cif_no);
        WRITEMSG
        sprintf(acErrMsg,"证件号码：[%s]",cif_id_code_rel.id_no);
        WRITEMSG
        sprintf(acErrMsg,"证件类型:[%s]",cif_id_code_rel.id_type);
        WRITEMSG
        /*如果客户号为空，则根据证件号码取客户号*/
        if(cif_photo_inf.cif_no==0)
        {
        	  sprintf(acErrMsg,"根据证件号码取客户号!");
            WRITEMSG
            ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "id_no='%s' and id_type='%s'",cif_id_code_rel.id_no,cif_id_code_rel.id_type);
            if(ret == 100)
            {
              sprintf(acErrMsg,"证件号码不存在![%s],取证件号出错！",g_pub_tx.reply);
              WRITEMSG
              strcpy(g_pub_tx.reply,"C007");
              ERR_DEAL;
            }
            else if(ret)
            {
              sprintf(acErrMsg,"读取客户证件信息异常！[%s]",g_pub_tx.reply);
              WRITEMSG
              strcpy(g_pub_tx.reply,"C006");
              ERR_DEAL;
            }
        }
        if(cif_id_code_rel.cif_no > 0)
        {
          cif_photo_inf.cif_no = cif_id_code_rel.cif_no;
          cif_cop_photo.cif_no = cif_id_code_rel.cif_no;
        }
	/* 生成下传全路经文件名(批量) */
        /*pub_base_strpack(cifno);
        pub_base_strpack(ttyno);
        pub_base_strpack(brno);
    
        strcpy(filename,cifno);
        pub_base_strpack(filename);
        strcat(filename,brno);
        strcat(filename,ttyno);
        pub_base_strpack(filename);
        sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,cif_photo_inf.photo_type);
        */
        pub_base_AllDwnFilName(tmpfile);
 
        pub_base_strpack(tmpfile);
        sprintf(acErrMsg,"文件名长度：[%d]",strlen(tmpfile));
        WRITEMSG
        if(cif_photo_inf.photo_type[0]=='1')
        {
            memcpy(cif_photo_inf.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"图片路径：[%s]",cif_photo_inf.photo);
            WRITEMSG
        }
        else if(cif_photo_inf.photo_type[0]=='2')
        {
            memcpy(cif_cop_photo.photo,tmpfile,strlen(tmpfile));
            sprintf(acErrMsg,"图片路径：[%s]",cif_cop_photo.photo);
            WRITEMSG
        }
        
        /*取客户姓名*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",cif_photo_inf.cif_no);
        if(ret==100)
        {
          sprintf(acErrMsg,"客户号不存在[%s],取客户姓名出错！",g_pub_tx.reply);
          WRITEMSG
          strcpy(g_pub_tx.reply,"C007");
          ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取客户姓名信息异常！[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        set_zd_data("0630",cif_basic_inf.name);
        
        /*取客户证件号码*/
        ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
              "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"客户号不存在![%s],取证件号出错！",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取客户证件信息异常！[%s]",g_pub_tx.reply);
            WRITEMSG
            strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;
        }
        
        strcpy(idtypes,"id_type");       
        pub_base_strpack(idtypes);
        ret = Dic_data_Sel(g_pub_tx.reply,&dic_data,"pr_code='%s' and pr_seq='%s'",idtypes,cif_id_code_rel.id_type);
        if(ret==100)
        {
            sprintf(acErrMsg,"取证件类型错误！[%s]",g_pub_tx.reply);
            WRITEMSG
        }
        else
        {
            sprintf(acErrMsg,"读取客户证件类型表错误![%s]",g_pub_tx.reply);
            WRITEMSG
        }
        if(strlen(dic_data.content)!=0)
        {
            set_zd_data("0830",dic_data.content);
        } 
        /*设置客户证件信息*/
        set_zd_data("0680",cif_id_code_rel.id_type);
        set_zd_data("0620",cif_id_code_rel.id_no);
        
        sprintf(acErrMsg,"证件类型:[%s]-证件号码:[%s]",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
        WRITEMSG
 
        ret = Cif_addr_inf_Sel(g_pub_tx.reply,&cif_addr_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"客户号不存在![%s]，取客户地址信息出错！",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取客户地址信息异常！[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        ret = Cif_email_inf_Sel(g_pub_tx.reply,&cif_email_inf,
                   "cif_no=%ld",cif_photo_inf.cif_no);
        if(ret == 100)
        {
            sprintf(acErrMsg,"客户号不存在![%s]，取客户联系信息出错！",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取客户联系信息异常！[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        set_zd_data("0260",cif_addr_inf.addr);
        set_zd_data("0580",cif_email_inf.email);
        set_zd_data("0250",cif_email_inf.link_man);
     
        sprintf(acErrMsg,"客户地址:[%s]-联系方式:[%s]-联系人:[%s]",cif_addr_inf.addr,cif_email_inf.email,cif_email_inf.link_man); 
        WRITEMSG
        /*取客户类型*/
        ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
              "cif_no=%ld",cif_photo_inf.cif_no);	
        if(ret == 100)
        {
            sprintf(acErrMsg,"客户号不存在![%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C007");
            ERR_DEAL;*/
        }
        else if(ret)
        {
            sprintf(acErrMsg,"读取客户信息异常！[%s]",g_pub_tx.reply);
            WRITEMSG
            /*strcpy(g_pub_tx.reply,"C006");
            ERR_DEAL;*/
        }
        
        /*个人客户*/
        if(cif_basic_inf.type[0]=='1')
        {
            ret = Cif_per_inf_Sel(g_pub_tx.reply,&cif_per_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
            if(ret == 100)
            {
                sprintf(acErrMsg,"客户号不存在![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"读取客户信息异常！[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
           
            /*设置客户个人信息*/
            /*set_zd_data("0630",cif_per_inf.name);*/
            set_zd_data("0700",cif_per_inf.sex);
            set_zd_data("0710","0"); 
            sprintf(acErrMsg,"客户姓名:[%s]-客户性别:[%s]",cif_per_inf.name,cif_per_inf.sex);
            WRITEMSG
        }
        else if(cif_basic_inf.type[0]=='2')
        {
            /*公司客户*/
            ret = Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf,
                  "cif_no=%ld",cif_photo_inf.cif_no);
               
            if(ret == 100)
            {
                sprintf(acErrMsg,"公司客户号不存在![%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C007");
                ERR_DEAL;*/
            }   
            else if(ret)
            {
                sprintf(acErrMsg,"读取公司客户信息异常！[%s]",g_pub_tx.reply);
                WRITEMSG
                /*strcpy(g_pub_tx.reply,"C006");
                ERR_DEAL;*/
            }
            /*设置公司客户信息*/
            set_zd_data("0810",cif_cop_inf.crd_no);
            set_zd_data("0820",cif_cop_inf.license);
            set_zd_long("0440",cif_cop_inf.reg_date);
            set_zd_double("0410",cif_cop_inf.reg_fund);
            set_zd_double("0420",cif_cop_inf.paicl_up_capital);
            set_zd_data("0710","1");

            sprintf(acErrMsg,"公司代码证:[%s]-公司营业执照:[%s]",cif_cop_inf.crd_no,cif_cop_inf.license);
            WRITEMSG
            /*sprintf(acErrMsg,"公司注册日期:[%s]",cif_cop_inf.reg_date);
            WRITEMSG
            sprintf(acErrMsg,"公司注册资金:[%f]",cif_cop_inf.reg_fund);
            WRITEMSG
            sprintf(acErrMsg,"公司实收资本:[%f]",cif_cop_inf.paicl_up_capital);
            WRITEMSG*/
        }
	strcpy(cif_photo_inf.photo,tmpfile);
	
	/*查询客户图片信息是否存在*/
	if(cif_photo_inf.photo_type[0]=='1')
	{
	     ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
	}
	else if(cif_photo_inf.photo_type[0]=='2')
	{
	    ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
	}
	if(ret<0)
	{
	    sprintf(acErrMsg,"查询客户图片信息出错![%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else if(ret == 0)
	{
	    sprintf(acErrMsg,"客户信息不存在！[%d]",ret);
	    WRITEMSG
	    /*strcpy(g_pub_tx.reply,"C006");
	    ERR_DEAL;*/
	}
	else
	{
		/*生成客户图片文件*/
		if(cif_photo_inf.photo_type[0]=='1')
		{
		  ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,
		    "cif_no=%ld",cif_photo_inf.cif_no);
	  }
	  else if(cif_photo_inf.photo_type[0]=='2')
	  {
	  	ret = Cif_cop_photo_Sel(g_pub_tx.reply,&cif_cop_photo,
		    "cif_no=%ld",cif_cop_photo.cif_no);
	  }
		if(ret ==100)
		{
		    sprintf(acErrMsg,"客户号不存在，请检查！[%s]",g_pub_tx.reply);
		    WRITEMSG
		    /*strcpy(g_pub_tx.reply,"C007");
		    ERR_DEAL;*/	
		}
		else if(ret)
		{
		    sprintf(acErrMsg,"读取客户图片信息异常！[%s]",g_pub_tx.reply);
		    WRITEMSG
		    /*strcpy(g_pub_tx.reply,"C006");
		    ERR_DEAL;*/
		}
		sprintf(acErrMsg,"检查客户号已经存在！");
		WRITEMSG
		set_zd_data(DC_FILE_SND,"2");
	}
	
	
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"获取客户信息成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"获取客户信息失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}


