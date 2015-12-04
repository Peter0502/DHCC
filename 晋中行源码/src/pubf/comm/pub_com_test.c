#include "find_debug.h" 
/*************************************************
* 文 件 名:  pub_com_test.c
* 功能描述： 控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
* 作  者:    薛珂
* 完成日期： 2011-8-9 
*
* 函数列表：
*         1、pub_base_sysinit();初始化函数
*         2、pub_base_check_draw();检查密码函数
*         3、pubf_card_chk_msr();检查卡磁道信息函数
*         4、pub_com_ChkNoteUse();柜员凭证检查函数
*         5、pub_com_ChkAcCheq();客户凭证检查函数
*参数：
*     输入：
*     char * cCtrlcmd; 控制标志
*          检查标志(0不检查1检查)：
*          [0]:密码检查标志0不查1查
*          [1]:磁道信息检查标志0不查1查
*          [2]:证件检查标志0不查1查
*          [3]:户名检查标志0不查不查
*          [4]:帐号检查标志0不查不查
*          [5]:收款人挂帐标志0帐号必须相符不查户名1帐号户名必须都相符2帐号必须相符，户名不符挂帐3帐号或户名有一不符挂帐,9强制挂帐
*          [6]:通存通兑标志0允许1不允许
*          [7]:凭证检查标志0不查1查柜员凭证2查客户凭证
*     char *cCert_no; 证件类型(1位)+号码(19位) 证件类型：1：身份证；2：户口簿；
*          3：护照；4：军人证；5：回乡证；6：士兵证；7：港澳居民来往通行证
*          8：企业代码证；9：经营许可证；A: 营业执照；B: 事业法人证书；
*          C：工商核准号；D：其他（对公）；E：临时身份证；F：外国人居留证；
*          G：警官证；H：其他证件（对私）；F：台湾同胞来往通行证
*     char *cAc_name;户名
*     char *cAc_no;账号
*     char *cNote_type;凭证种类
*     char *cNote_no;凭证号码
*     char *cDc_ind;借贷标志 1借2贷
*     输出：
*     char *cGz_flag;挂账标志
*     char *sts;凭证状态
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "svrpub_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "com_item_c.h"
#include "com_parm_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "mo_loss_c.h"
char * get_bin_data(char *,char *);
int pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no, cNote_type,cNote_no,cDc_ind,cGz_flag)
    char *cCtrlcmd;
    char *cCert_no;
    char *cAc_name;
    char *cAc_no;
    char *cNote_type;
    char *cNote_no;
    char *cDc_ind;
    char *cGz_flag;
{
    int iRet;
    int lKeylen=0;
    int iPwdlen=0;
    char  cName_tmp[61];/** 比对用NAME **/
    char  cVal[17];   /*十六进制*/
    char  cPwd[17];
    char  cDraw_pwd[17];
    char sts[51];/*凭证状态*/
    struct  dd_mst_c    sDd_mst;
    struct  dd_parm_c   sDd_parm;
    struct  com_item_c  sCom_item;
    struct  prdt_ac_id_c sPrdt_ac_id;
    struct  in_mst_c     sIn_mst;
    struct  mo_loss_c    sMo_loss;
    struct  com_parm_c   sCom_parm;
    /*初始化变量*/
   
    memset(sts,0x00,sizeof(sts));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sMo_loss,0x00,sizeof(sMo_loss));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(cVal,0x00,sizeof(cVal));
    memset(cPwd,0x00,sizeof(cPwd));
    memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
    
    /*接收密码*/
    get_bin_data("0800",cDraw_pwd);
    /**20111113  CD
    get_fd_data("0800", cDraw_pwd);
    **/
    /*vtcp_log("[%s],[%d],cDraw_pwd=[%s]",__FILE__,__LINE__,cDraw_pwd);*/
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cNote_type=[%s],cNote_no=[%s],cDc_ind=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cNote_type,cNote_no,cDc_ind);
    pub_base_old_acno(cAc_no);  /** 新旧账号转换 **/
    memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
    zip_space(g_pub_tx.ac_no);
    if( strlen ( cAc_no ) >10) /*** 此种情况下,只能是账号 ***/
    {
      iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
      if (iRet)
      {
          vtcp_log("%s,%d,ac_no[%s]查找mdm_ac_rel错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
          strcpy(g_pub_tx.reply,"C114");
          goto ErrExit;
      }
      if(g_mdm_ac_rel.note_sts[0] != '0')
      {
          vtcp_log("%s,%d,ac_no[%s]介质状态不正常!",__FILE__,__LINE__,g_pub_tx.ac_no);
          if(g_mdm_ac_rel.note_sts[0] == '1') /*挂失*/
          {
              iRet = Mo_loss_Sel(g_pub_tx.reply,&sMo_loss,"ac_no ='%s' and unloss_ind ='0'",g_mdm_ac_rel.ac_no);
              if(iRet)
              {
                  vtcp_log("[%s][%d],查询挂失登记簿错！！",__FILE__,__LINE__);
                  strcpy(g_pub_tx.reply,"P189");
                  goto ErrExit;
              }
              if(sMo_loss.loss_ind[0] == '1')
              {
                 strcpy(g_pub_tx.reply,"P501");
              }
              else if(sMo_loss.loss_ind[0] == '2')
              {
                 strcpy(g_pub_tx.reply,"P502");
              }
              else if(sMo_loss.loss_ind[0] == '3')
              {
                 strcpy(g_pub_tx.reply,"P503");
              }
              else if(sMo_loss.loss_ind[0] == '4')
              {
                 strcpy(g_pub_tx.reply,"P504");
              }
              else
              { 
                    strcpy(g_pub_tx.reply,"P170");
              }              
          }
          else if(g_mdm_ac_rel.note_sts[0] == '2')/*挂失换证*/
          {
              strcpy(g_pub_tx.reply,"P171");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '*')/*销户*/
          {
              strcpy(g_pub_tx.reply,"P172");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '3')/*正常换证*/
          {
              strcpy(g_pub_tx.reply,"M022");
          }
          else if(g_mdm_ac_rel.note_sts[0] == '5')/*撤销*/
          {
              strcpy(g_pub_tx.reply,"M023");
          }
          else /*其他*/
          {
              strcpy(g_pub_tx.reply,"D140");
          }
          goto ErrExit;
      }
      vtcp_log("%s,%d,ac_no[%s]介质状态正常!",__FILE__,__LINE__,g_pub_tx.ac_no);
      iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
      if (iRet)
      {
          vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cAc_no);
          strcpy(g_pub_tx.reply,"W013");
          goto ErrExit;
      }
      if(sPrdt_ac_id.ac_id_type[0] == '1')
      {
          iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
          if (iRet)
          {
              vtcp_log("%s,%d,ac_no[%s]查找dd_mst错误!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"P102");
              goto ErrExit;
          }
          if(sDd_mst.ac_sts[0] != '1')
          {
              vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"O085");
              goto ErrExit;
          }
          memcpy(cName_tmp,sDd_mst.name,sizeof(cName_tmp)-1);
          iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
          if (iRet)
          {  
              vtcp_log("%s,%d,prdt_no[%s]查询dd_parm错误!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"W015");
              goto ErrExit;
          }
      }
      else if(sPrdt_ac_id.ac_id_type[0] == '9')
      {
          iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
          if (iRet)
          {
              vtcp_log("%s,%d,ac_no[%s]查找in_mst错误!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"A029");
              goto ErrExit;
          }
          if(sIn_mst.sts[0] != '1')
          {
              vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no);
              strcpy(g_pub_tx.reply,"O085");
              goto ErrExit;
          }
          memcpy(cName_tmp,sIn_mst.name,sizeof(cName_tmp)-1);
					zip_space(cName_tmp);
					
      }
       if ( cCtrlcmd[3] == '1' )    /** 检查户名信息 **/
       {
           zip_space(cAc_name);
           vtcp_log("%s,%d 户名=[%s]",__FILE__,__LINE__,cAc_name);
           if( cDc_ind[0] == '2')
           {
              if(cCtrlcmd[5] == '1' && memcmp (cName_tmp, cAc_name,sizeof(cName_tmp)-1))
              {
                vtcp_log("%s,%d,cName_tmp=[%s]",__FILE__,__LINE__,cName_tmp);
                 sprintf(acErrMsg,"户名不符!!!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "P042");
                 goto ErrExit;
              }
              if( cCtrlcmd[5] == '9' || ((cCtrlcmd[5] == '2' || cCtrlcmd[5] == '3' ) && memcmp (cName_tmp , cAc_name,sizeof(cName_tmp)-1)))
              {
                 cGz_flag[0] = '2';
              }
                 vtcp_log("%s,%d,cGz_flag[0]=[%s]",__FILE__,__LINE__,cGz_flag); 
           }
       }
    }
    else
    {    /**** 否则是科目  **/
         iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
         if(iRet)
         {
             sprintf(acErrMsg,"查询科目失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B172");
             goto ErrExit;
         }   
         if ( cCtrlcmd[3] == '1' )    /** 检查户名信息 **/
         {
             zip_space(cAc_name);
             vtcp_log("%s,%d 户名=%s",__FILE__,__LINE__,cAc_name);
             if( cDc_ind[0] == '2')
             {
                 if(cCtrlcmd[5] == '1' && memcmp (sCom_item.acc_name , cAc_name,sizeof(sCom_item.acc_name ) ))
                 {
                     sprintf(acErrMsg,"科目户名不符!!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "P042");
                     goto ErrExit;
                 }
                 if( cCtrlcmd[5] == '9' || ((cCtrlcmd[5] == '2' || cCtrlcmd[5] == '3' ) && memcmp (sCom_item.acc_name  , cAc_name,sizeof(sCom_item.acc_name ))))
                 {
                     cGz_flag[0] = '2';
                 } 
                 vtcp_log("%s,%d,22cGz_flag[0]=%s",__FILE__,__LINE__,cGz_flag);
             }
         }
    }    
    /**　开始检查各要素　**/
    if ( cCtrlcmd[0] == '1' ) /* 判断密码是否正确 */
    {   
        /*ADD BEGIN BY CHENGBO 20111020 14:47 解密处理*/
        iRet = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='PTMY' and parm_seqn=1");
        if( iRet )
        {
             sprintf(acErrMsg,"查询密钥失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "G001");
             goto ErrExit;
        }
        lKeylen=strlen(sCom_parm.val);
        vtcp_log("[%s][%d] lKeylen=[%d]",__FILE__,__LINE__,lKeylen);
        /*iPwdlen=strlen(cDraw_pwd);*/
        iPwdlen=8;
        vtcp_log("[%s][%d] iPwdlen=[%d]",__FILE__,__LINE__,iPwdlen);
        vtcp_log("[%s][%d] val=[%s],draw_pwd=[%s]",__FILE__,__LINE__,sCom_parm.val,cDraw_pwd);
        vtcp_log("[%s][%d] draw_pwd[0]=[%x]",__FILE__,__LINE__,cDraw_pwd[0]);
        vtcp_log("[%s][%d] draw_pwd[1]=[%x]",__FILE__,__LINE__,cDraw_pwd[1]);
        vtcp_log("[%s][%d] draw_pwd[2]=[%x]",__FILE__,__LINE__,cDraw_pwd[2]);
        vtcp_log("[%s][%d] draw_pwd[3]=[%x]",__FILE__,__LINE__,cDraw_pwd[3]);
        vtcp_log("[%s][%d] draw_pwd[4]=[%x]",__FILE__,__LINE__,cDraw_pwd[4]);
        vtcp_log("[%s][%d] draw_pwd[5]=[%x]",__FILE__,__LINE__,cDraw_pwd[5]);
        vtcp_log("[%s][%d] draw_pwd[6]=[%x]",__FILE__,__LINE__,cDraw_pwd[6]);
        vtcp_log("[%s][%d] draw_pwd[7]=[%x]",__FILE__,__LINE__,cDraw_pwd[7]);
        /*转十六进制*/
        pubf_str_hex2str(sCom_parm.val,cVal,lKeylen/2);
        
        /*解3des密码*/
        iRet = pubf_3des_dec(cVal,lKeylen/2,cDraw_pwd,cPwd,8); 
        if( iRet )
        {
             sprintf(acErrMsg,"解密失败!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "A035");
             goto ErrExit;
            
        }
        /*ADD END BY CHENGBO 20111020 14:47 解密处理*/
        vtcp_log("[%s][%d],draw_pwd=[%s]",__FILE__,__LINE__,cPwd);
        memcpy(g_pub_tx.draw_pwd,cPwd+2,sizeof(g_pub_tx.draw_pwd)-1);
        iRet = pub_base_check_draw("001");
        if(iRet)
        {
            sprintf(acErrMsg,"支取密码检验错误!");
            if(memcmp(g_pub_tx.reply,"M036",4))
            {
                sprintf(g_pub_tx.reply,"M005");
            }
            goto ErrExit;

        }
        else
        {
            vtcp_log("%s,%d,《《《密码校验完毕了》》》",__FILE__,__LINE__);
        }
    }
    if ( cCtrlcmd[1] == '1' ) /** 检查磁道信息 **/
    {
        iRet = pubf_card_chk_msr(3,cAc_no);
        if ( iRet )
        {
          sprintf(acErrMsg,"检查磁道信息失败!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "CU05");
          goto ErrExit;
        }
    }
    if ( cCtrlcmd[2] == '1' ) /** 检查证件信息 **/
    {
        if ( cCert_no[0] != g_mdm_ac_rel.id_type[0] )
        {
            sprintf(acErrMsg,"证件类型不符!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "P444");
            goto ErrExit;
        }
        else
        {
          vtcp_log("%s,%d,检查证件类型通过了",__FILE__,__LINE__);
        }
        iRet=memcmp (g_mdm_ac_rel.id_no , cCert_no+1,strlen(g_mdm_ac_rel.id_no) );
        vtcp_log("%s,%d,g_mdm_ac_rel.id_no=[%s],cCert_no+1=[%s]",__FILE__,__LINE__,g_mdm_ac_rel.id_no,cCert_no+1);
        if(iRet!=0)
        {
            /**modify by lw 20110917 银联发过来的身份证后六位**/
            if(cCert_no[0] == '1' && strlen(cCert_no) == 7 && nCheckID(g_mdm_ac_rel.id_no,cCert_no+1) == 0)
            {
                vtcp_log("[%s][%d]银联身份证号后六位验证通过！",__FILE__,__LINE__);
            }
            else
            {
             sprintf(acErrMsg,"证件号码不符!!!");
             WRITEMSG
             strcpy(g_pub_tx.reply, "P445");
             goto ErrExit;
            }
        }
        else
        {
            vtcp_log("%s,%d,检查证件号码通过了",__FILE__,__LINE__);
        }
    }
    /** if ( cCtrlcmd[4] == '1' )  检查账号信息 
    {
      if ( memcmp (g_mdm_ac_rel.ac_no , cAc_no,sizeof(g_mdm_ac_rel.ac_no) ))
      {
        sprintf(acErrMsg,"账号不符!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "YYL4");
        goto ErrExit;
      }else{
      if ( memcmp (sCom_item.acc_no , cAc_no,sizeof(sCom_item.acc_no) ))
      {
        sprintf(acErrMsg,"科目账号不符!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "YYL4");
        goto ErrExit;
       }
      }
    }**/
    if ( cCtrlcmd[6] == '0') /** 检查通存通兑标志 **/
    {
    	/*应届汇款户允许通存 Modefied by YangMeng 2014/7/11 9:23:25*/
        if (sDd_parm.thr_dpst_ind[0] == 'N' && memcmp(sDd_parm.prdt_no,"138",3))
        {
            sprintf(acErrMsg,"通存标志不正确！！！");
            strcpy(g_pub_tx.reply,"A102");
            WRITEMSG
            goto ErrExit;
        }
        if ( sDd_parm.thr_draw_ind[0] =='N')
        {
            sprintf(acErrMsg,"通兑标志不正确！！！");
            strcpy(g_pub_tx.reply,"A102");
            WRITEMSG
            goto ErrExit;
        }
    }
    /*凭证检查标志0不查1查柜员凭证2查客户凭证*/
    if ( cCtrlcmd[7] == '1' )
    {
        iRet = pub_com_ChkNoteUse(cNote_type);
        if (iRet)
        {
            sprintf(acErrMsg,"检查柜员凭证错误");
            WRITEMSG
            strcpy(g_pub_tx.reply, "M020");
            goto ErrExit;
        }
    }
    else if( cCtrlcmd[7] == '2' )
    {
      iRet = pub_com_ChkAcCheq( g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn,cNote_type, cNote_no, cNote_no, sts );
      if ( iRet )
      {
        sprintf(acErrMsg,"查询客户凭证出错");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N080");
        goto ErrExit;
      }
      vtcp_log("%s,%d,sts[0]=%s,iRET=%d,g_mdm_ac_rel.ac_id=%ld,",__FILE__,__LINE__,sts,iRet,g_mdm_ac_rel.ac_id);
      if ( sts[0] == '0' )
      {
        sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] 凭证种类=[%s] 凭证号码=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N080" );
        goto ErrExit;
      }
      else if ( sts[0] == '2' )
      {
        sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] 凭证种类=[%s] 凭证号码=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N081" );
        goto ErrExit;
      } else if ( sts[0] == '3' )
      {   
        sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] 凭证种类=[%s] 凭证号码=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N082" );
        goto ErrExit;
      } else if ( sts[0] == '4' )
      {
        sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] 凭证种类=[%s] 凭证号码=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N083" );
        goto ErrExit;
      } else if ( sts[0] == '5' )
      {
        sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);
        WRITEMSG
        vtcp_log("[%s][%d] 凭证种类=[%s] 凭证号码=[%s] RET=[%d]", __FILE__, __LINE__, cNote_type,cNote_no,iRet);
        strcpy( g_pub_tx.reply, "N084" );
        goto ErrExit;
      }
    }

OKExit:
    return 0;
ErrExit:
    return 1;
}
/**
函数名:
作  者: luowei
日  期: 2011-09-17
功  能: 验证身份证后六位
参  数:
        id_no 身份证号码
        no    身份证号后六位
返回值: 0/1
**/
int nCheckID(const char *id_no, const char *no)
{
	char sNo[7];
	int  len=0;
	
	memset(sNo,'0',sizeof(sNo));
	sNo[sizeof(sNo)-1]='\0';
	len=strlen(sNo)-1;
	
	for(int i=strlen(id_no); i>=0; i--)
	{
		if(id_no[i]>='0' && id_no[i]<='9')
		{
			sNo[len]=id_no[i];
			len--;
			if(len<0)
			{
				break;
			}
		}	
	}
	vtcp_log("[%s][%d]按银联方式取出身份证[%s]后六位[%s]",__FILE__,__LINE__,id_no,sNo);
	if(memcmp(sNo,no,strlen(sNo))!=0)
	{
		vtcp_log("[%s][%d]身份证后六位[%s][%s]",__FILE__,__LINE__,sNo,no);
		return(1);
	}
	return(0);
}



/**********************************************************************
* 函 数 名：  get_bin_data
* 函数功能：  取得指定数据编码的数据内容
* 作    者：  
* 完成时间：  2003年12月16日
*
* 参    数：
*     输  入：sjbm:    数据编码
*      
*     输  出：data:    返回的数据
*
*     返回值：取得的数据
*             NULL失败
*
* 修改历史：
*                   
********************************************************************/

char *get_bin_data( char *sjbm, char *data )
{
	char	tmpdata[1024];
	int i;

	memset(tmpdata, 0 , sizeof(tmpdata));
	data[0] = 0;
	vtcp_log("[%s][%d]get_bin_data*****************sjbm=[%s]",__FILE__,__LINE__,sjbm);
	i = pub_base_GetDataCode( sjbm );
	if ( i<0 ) 
	{
		vtcp_log("%s,%d i==%d ",__FILE__,__LINE__,i);TRACE;
		return NULL;
	}
	pub_base_Nctoc((char*)Pu_Val[i].val,tmpdata,Pu_Val[i].lx,Pu_Val[i].length,Pu_Val[i].prec);  
	
	memcpy(data,tmpdata,Pu_Val[i].length);

	return data;
}
