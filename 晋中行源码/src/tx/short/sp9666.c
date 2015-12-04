/*************************************************
* 文  件名:  sp9666.c
* 功能描述: 获取客户图片信息
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

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;

sp9666()
{
        int ret = 0;
	char filename[100];

        /** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
	memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c));

	/** 取值、赋值 **/
        get_zd_long("0280",&cif_photo_inf.cif_no);		/* 客户号 */
        get_zd_data("0720",cif_photo_inf.photo_type);
        get_zd_long("0280",&cif_cop_photo.cif_no);
        get_zd_data("0720",cif_cop_photo.photo_type);
	/** 处理流程 **/

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );
	pub_base_strpack(filename);
       
        sprintf(acErrMsg,"文件路径:[%s]",filename);
        WRITEMSG
	
	if(cif_photo_inf.photo_type[0] =='1')
	{
            sprintf(acErrMsg,"取客户照片!");
            WRITEMSG
	    strcpy(cif_photo_inf.photo,filename);
	    ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
        }
        else if(cif_photo_inf.photo_type[0]=='2')
        {
            sprintf(acErrMsg,"取客户印件信息！");
            WRITEMSG
            strcpy(cif_cop_photo.photo,filename);	
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
            sprintf(acErrMsg,"开始取客户图片信息！");
            WRITEMSG   	
            /*生成客户图片文件*/
	    if(cif_photo_inf.photo_type[0] =='1')
	    {
                sprintf(acErrMsg,"获取客户[%ld]照片！",cif_photo_inf.cif_no);
                WRITEMSG
	        ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,
	              "cif_no=%ld",cif_photo_inf.cif_no);
	    }
	    else if(cif_photo_inf.photo_type[0]=='2')
	    {
                sprintf(acErrMsg,"获取客户[%ld]印件！",cif_cop_photo.cif_no);
                WRITEMSG
                sprintf(acErrMsg,"客户路径:[%s]",cif_cop_photo.photo);
                WRITEMSG
	        Cif_cop_photo_Debug(&cif_cop_photo);
	        ret = Cif_cop_photo_Sel(g_pub_tx.reply,&cif_cop_photo,
                      "cif_no=%ld",cif_photo_inf.cif_no);
                /*ret = Cif_photo_inf_Sel(g_pub_tx.reply,&cif_photo_inf,"cif_no=%ld",cif_photo_inf.cif_no);*/
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
    sprintf(acErrMsg,"客户图片信息获取成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"获取客户图片信息失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
