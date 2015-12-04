/*************************************************
* 文  件名:  sp9669.c
* 功能描述: 客户图片信息录入
* 作    者: liuyong
* 完成日期: 2009年08月14日
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
#define EXTERN
#include "public.h"
#include "pub_tx.h"
#include "cif_photo_inf_c.h"
#include "cif_cop_photo_c.h"
#include "cif_id_code_rel_c.h"

static struct cif_photo_inf_c cif_photo_inf;
static struct cif_cop_photo_c cif_cop_photo;
static struct cif_id_code_rel_c cif_id_code_rel;

sp9669()
{ 	
  int ret = 0;
  char flag[2];
  char filename[100];
  char brno[10];
  char ttyno[5];
  char tmpfile[100];
  char picflag[2];
  char txcode[21];
  FILE *fp;
  
  /** 初始化全局变量 **/
  pub_base_sysinit();

  /** 数据初始化 **/
  memset(filename,0x00,sizeof(filename));
  memset(tmpfile,0x00,sizeof(tmpfile));
  memset(brno,0x00,sizeof(brno));
  memset(ttyno,0x00,sizeof(ttyno));
  memset(txcode,0x00,sizeof(txcode));
  memset(picflag,0x00,sizeof(picflag));
  memset(&cif_photo_inf,0x00,sizeof(struct cif_photo_inf_c));
  memset(&cif_cop_photo,0x00,sizeof(struct cif_cop_photo_c)); 
  memset(&cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));

  /** 取值、赋值 **/
  get_zd_long("0280",&cif_photo_inf.cif_no);		/* 客户号 */
  get_zd_long("0280",&cif_id_code_rel.cif_no);
  get_zd_long("0280",&cif_cop_photo.cif_no);
  get_zd_data("0620",txcode);                    /*证件号码*/
  get_zd_data(DC_TX_BR_NO,brno);
  get_zd_data(DC_TTY,ttyno); 
  get_zd_data("1270",flag); 

  sprintf(acErrMsg,"客户号[%ld]",cif_photo_inf.cif_no);
  WRITEMSG
	
  sprintf(acErrMsg,"是否设置图片标识[%s]",flag);
  WRITEMSG
  
  sprintf(acErrMsg,"图片类型：[%s]",picflag);
  WRITEMSG
  
  /** 处理流程 **/
  if(strlen(flag) == 0)
  {
     goto OkExit;	
  }
  if(flag[0] !='1')
  {
    goto OkExit;
  } 
  sprintf(acErrMsg,"生成下传文件名");
  WRITEMSG

  /*if(strlen(txcode)==0)
  {*/
      /*取客户证件号码*/ 
      ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,
        "cif_no=%ld and rowid in (select min(rowid) from cif_id_code_rel where cif_no =%ld)",cif_photo_inf.cif_no,cif_photo_inf.cif_no); 
      if(ret ==100)
      {
          sprintf(acErrMsg,"客户号不存在！[%s],取客户证件号出错！",g_pub_tx.reply);
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
      memcpy(txcode,cif_id_code_rel.id_no,strlen(cif_id_code_rel.id_no));
  /*}*/

  /*pub_base_strpack(picflag);*/
  pub_base_strpack(txcode); 
  pub_base_strpack (brno);
  pub_base_strpack (ttyno);
  strcpy(filename,txcode);
  strcat(filename,brno);
  strcat(filename,ttyno);
  pub_base_strpack (filename);
  
  /*查找照片*/
  strcpy(picflag,"1");
  sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,picflag);
  pub_base_strpack ( tmpfile );
  memcpy(cif_photo_inf.photo,tmpfile,strlen(tmpfile));
  sprintf(acErrMsg,"照片路径[%s]",cif_photo_inf.photo);
  WRITEMSG
  memcpy(cif_photo_inf.photo_type,picflag,strlen(picflag));
  fp=fopen(tmpfile,"r");
  if(fp !=NULL)
  {
  	  fclose(fp);
      /*查询客户图片信息是否存在*/
      ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"查询客户图片信息出错![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
      	cif_photo_inf.photo_seqn=1;
        sprintf(acErrMsg,"开始插入照片![%d]",ret);
	      WRITEMSG
	      /* 登记客户图片信息表 */
	      ret=Cif_photo_inf_Ins(cif_photo_inf);
	      if(ret)
	      {
	        sprintf(acErrMsg,"登记客户照片信息表错误![%d]",ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C021");
	        ERR_DEAL;
	      }
      }
      else 
      {
	      /*更新客户图片信息表 */
        ret=Cif_photo_inf_Upd(cif_photo_inf,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"更新 Cif_photo_inf失败![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  else
  {
  	  sprintf(acErrMsg,"删除照片信息！");
  	  WRITEMSG
  	  
      /*查询客户图片信息是否存在*/
      ret=sql_count("cif_photo_inf","cif_no=%ld",cif_photo_inf.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"查询照片信息出错![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {

      }
      else 
      {
	      /*删除客户图片信息表 */
        ret=Cif_photo_inf_Del_Upd(cif_photo_inf,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"删除 Cif_photo_inf失败![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  /*查找印件*/
  strcpy(picflag,"2");
  sprintf(tmpfile,"%s/%s/%s%s",getenv("HOME"),"image",filename,picflag);
  pub_base_strpack ( tmpfile );
  memcpy(cif_cop_photo.photo,tmpfile,strlen(tmpfile));
  sprintf(acErrMsg,"图片路径[%s]",cif_cop_photo.photo);
  WRITEMSG
  memcpy(cif_cop_photo.photo_type,picflag,strlen(picflag));
  fp=fopen(tmpfile,"r");
  if(fp !=NULL)
  {
  	  fclose(fp);
  	  	
      /*查询客户图片信息是否存在*/
      ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"查询客户印件信息出错![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
      	cif_cop_photo.photo_seqn=1;
        sprintf(acErrMsg,"开始插入印件![%d]",ret);
	      WRITEMSG
	      /* 登记客户图片信息表 */
	      ret=Cif_cop_photo_Ins(cif_cop_photo);
	      if(ret)
	      {
	        sprintf(acErrMsg,"登记客户印件信息表错误![%d]",ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C021");
	        ERR_DEAL;
	      }
      }
      else 
      {
	      /*更新客户图片信息表 */
        ret = Cif_cop_photo_Upd(cif_cop_photo,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"更新 Cif_cop_photo失败![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  else
  {
    sprintf(acErrMsg,"删除印件信息！");
  	  WRITEMSG	
      /*查询客户图片信息是否存在*/
      ret=sql_count("cif_cop_photo","cif_no=%ld",cif_cop_photo.cif_no);
      if(ret<0)
      {
	      sprintf(acErrMsg,"查询客户印件信息出错![%d]",ret);
	      WRITEMSG
	      strcpy(g_pub_tx.reply,"C006");
	      ERR_DEAL;
      }
      else if(ret==0)
      {
 
      }
      else 
      {
	      /*删除客户印件信息表 */
        ret = Cif_cop_photo_Del_Upd(cif_cop_photo,g_pub_tx.reply);
        if (ret)
        {
	        sprintf(acErrMsg,"删除 Cif_cop_photo失败![%s]",g_pub_tx.reply);
	        WRITEMSG
	        strcpy(g_pub_tx.reply,"C006");
	        ERR_DEAL;
	      }
      }
  }
  
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户图片信息录入成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
	sprintf(acErrMsg,"客户图片信息录入失败！[%s]",g_pub_tx.reply);
	vtcp_log("[%s][%d] ret==[%d]\n",__FILE__,__LINE__,ret);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
