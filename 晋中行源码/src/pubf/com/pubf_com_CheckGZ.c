/*************************************************
* 文 件 名:  pubf_com_chkGZ.c
* 功能描述： 挂账检查
* 作  者:    商永超
* 完成日期： 2012-2-25 
*
* 参数：ac_no 账号
        ac_name 户名
        gz_flag 0帐号必须相符不查户名1帐号户名必须都相符2帐号必须相符，户名不符挂帐3帐号或户名有一不符挂帐,9强制挂帐
        gz_br_no 平台传送 挂账机构
        gz_reason 挂账原因 本函数返回：1：账号不存在 2：户名不符 3：账户状态非法 4：介质状态非法 5：冻结标志非法 6：止付标志非法 7：其他原因 
* 修改记录： 
* 日    期:
* 修 改 人:Modefied by YangMeng 20140704
* 修改内容:
	修改挂账检查条件
**************************************************/
#include "find_debug.h" 
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
int pubf_com_CheckGZ(char *ac_no,char *ac_name,char *gz_flag,char *gz_br_no,char *gz_reason,int parm_seqn)
{
    int ret;
    char cGz_yn[2];/*是否需要挂账 Y是 N否*/
    char sMzCode[5] = {0};
    char sSysCode[5] = {0};
    char cTmp_ac_name[61];
    char cGz_ac_name[61];/*挂账账号的户名*/
    char cOpn_br_no[BRNO_LEN+1];/*账号开户机构*/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct dd_mst_c    sDd_mst;
    struct com_parm_c   sCom_parm;
    struct in_mst_c sIn_mst;
    struct com_item_c sCom_item;
    /*初始化变量*/
    memset(cGz_yn,0x00,sizeof(cGz_yn));
    memset(cOpn_br_no,0x00,sizeof(cOpn_br_no));
    memset(cTmp_ac_name,0x00,sizeof(cTmp_ac_name));
    memset(cGz_ac_name,0x00,sizeof(cGz_ac_name));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    
    vtcp_log("[%s][%d],ac_no=[%s],ac_name=[%s],gz_flag=[%s],gz_br_no=[%s],parm_seqn=[%d]",\
    __FILE__,__LINE__,ac_no,ac_name,gz_flag,gz_br_no,parm_seqn);
    if(gz_flag[0] == '9')/*退汇强制挂账*/
    {
    	/*退汇业务强制挂原交易机构*/
    	gz_reason[0] = '7';/*强制挂账,added by YangMeng 20130312*/
    	memset(cOpn_br_no, 0x00, sizeof(cOpn_br_no));
        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
        if(ret)
        {
            sprintf(acErrMsg,"挂账账号选择函数错误");
            WRITEMSG
            return 1;
        }
    }
    else
    {
        if(strlen(ac_no) >10)/*是账号时*/
        {
            ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",ac_no);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"L015");
                sprintf(acErrMsg,"查询mdm_ac_rel出错！");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"无此账号，进行挂账判断");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"挂账账号选择函数错误");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"账号不存在，并且不挂账");
                    WRITEMSG
                    return 1;
                }
            }
            vtcp_log("[%s][%d],开始检查介质状态",__FILE__,__LINE__);
            memcpy(cOpn_br_no,sMdm_ac_rel.opn_br_no,sizeof(cOpn_br_no)-1);
            /*Modified by YangMeng 20131024*/
            get_zd_data("0100",sSysCode);
            get_zd_data("0270",sMzCode);
            if ((!memcmp(sSysCode,"HVPS",4) || !memcmp(sSysCode,"BEPS",4)) && !memcmp(sMzCode,"1000",4))
            {
            	if(sMdm_ac_rel.note_sts[0] == '*'){
            		vtcp_log("[%s][%d],账户已销户，进行挂账判断！",__FILE__,__LINE__);
            		if(gz_flag[0] == '3'||gz_flag[0] == '8'){
            			gz_reason[0] = '4'; 
            			ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            	        if(ret)
            	        {
            	            sprintf(acErrMsg,"挂账账号选择函数错误");
            	            WRITEMSG
            	            return 1;
            	        }
            	        return 0;
            		}
            		else
            	    {
            	        strcpy(g_pub_tx.reply,"M013");
            	        sprintf(acErrMsg,"介质状态不正常，并且不挂账");
            	        WRITEMSG
            	        return 1;
            	    }
            	}else{
            		vtcp_log("[%s][%d],除已销户账户外，不做挂账判断！",__FILE__,__LINE__);
            	}
            	
            }else{
            	if(sMdm_ac_rel.note_sts[0] != '0')
            	{
            	    sprintf(acErrMsg,"账号介质状态[%s]不正常，进行挂账判断",sMdm_ac_rel.note_sts);
            	    WRITEMSG
            	    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
            	    {
            	        gz_reason[0] = '4';
            	        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            	        if(ret)
            	        {
            	            sprintf(acErrMsg,"挂账账号选择函数错误");
            	            WRITEMSG
            	            return 1;
            	        }
            	        return 0;
            	    }
            	    else
            	    {
            	        strcpy(g_pub_tx.reply,"M013");
            	        sprintf(acErrMsg,"介质状态不正常，并且不挂账");
            	        WRITEMSG
            	        return 1;
            	    }
            	}
            }/*Modified by YangMeng 20131024*/
            ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id = %ld order by ac_seqn",sMdm_ac_rel.ac_id);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"W013");
                sprintf(acErrMsg,"查询prdt_ac_id出错！");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"prdt_ac_id无此账户记录，进行挂账判断");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"挂账账号选择函数错误");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"账号不存在，并且不挂账");
                    WRITEMSG
                    return 1;
                }
            }
            if(sPrdt_ac_id.ac_id_type[0] == '1')
            {
                ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel.ac_id,sPrdt_ac_id.ac_seqn);
                if(ret !=0 && ret != 100)
                {
                    strcpy(g_pub_tx.reply,"X003");
                    sprintf(acErrMsg,"查询dd_mst 错误");
                    WRITEMSG
                    return 1;
                }
                else if(ret == 100)
                {
                    sprintf(acErrMsg,"无此活期账户，进行挂账判断");
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                    {
                        gz_reason[0] = '1';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"挂账账号选择函数错误");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        sprintf(acErrMsg,"账号不存在，并且不挂账");
                        WRITEMSG
                        return 1;
                    }
                }
                vtcp_log("[%s][%d],开始检查账户状态、冻结和止付状态",__FILE__,__LINE__);
                /*Modified by YangMeng 20131024*/
                if ((!memcmp(sSysCode,"HVPS",4) || !memcmp(sSysCode,"BEPS",4)) && !memcmp(sMzCode,"1000",4))
                {
                	if(sDd_mst.ac_sts[0] == '5' || sDd_mst.ac_sts[0] == '*'){
                		vtcp_log("[%s][%d],账户已销户，进行挂账判断！",__FILE__,__LINE__);
                		if(gz_flag[0] == '3'||gz_flag[0] == '8'){
                			gz_reason[0] = '3';
                			ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                			if(ret)
                			{
                				sprintf(acErrMsg,"挂账账号选择函数错误");
                				WRITEMSG
                				return 1;
                			}
                			return 0;
                		}else{
                			strcpy(g_pub_tx.reply,"O085");
                			sprintf(acErrMsg,"账户状态不正常，并且不挂账");
                			WRITEMSG
                			return 1;
                		}
                	}else{
                		/*add by lwb 20141121 当账号为保证金账号时也挂账*/
                		if(!strcmp(sDd_mst.prdt_no,"131")||!strcmp(sDd_mst.prdt_no,"142"))
                		{
                		  gz_reason[0] = '7';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"挂账账号选择函数错误");
                            WRITEMSG
                            return 1;
                        }
                        return 0;		
                		}
                		/*add end */
                		vtcp_log("[%s][%d],除已销户账户外，不做挂账判断！",__FILE__,__LINE__);
                	}
                	
                }else{
                	if(sDd_mst.ac_sts[0] != '1')
                	{
                	    sprintf(acErrMsg,"账户状态不正常ac_sts=【%s】，进行挂账判断",sDd_mst.ac_sts);
                	    WRITEMSG
                	    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                	    {
                	        gz_reason[0] = '3';
                	        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                	        if(ret)
                	        {
                	            sprintf(acErrMsg,"挂账账号选择函数错误");
                	            WRITEMSG
                	            return 1;
                	        }
                	        return 0;
                	    }
                	    else
                	    {
                	        strcpy(g_pub_tx.reply,"O085");
                	        sprintf(acErrMsg,"账户状态不正常，并且不挂账");
                	        WRITEMSG
                	        return 1;
                	    }
                	}
                }/*Modified by YangMeng 20131024*/
                if(sDd_mst.hold_sts[0] == '1')
                {
                    sprintf(acErrMsg,"账户被全部冻结hold_sts=【%s】，进行挂账判断",sDd_mst.hold_sts);
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                    {
                        gz_reason[0] = '5';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"挂账账号选择函数错误");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P110");
                        sprintf(acErrMsg,"账户被全部冻结，并且不挂账");
                        WRITEMSG
                        return 1;
                    }
                }
                strcpy(cTmp_ac_name,sDd_mst.name);
                
            }
            else if(sPrdt_ac_id.ac_id_type[0] == '9')
            {
                ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel.ac_id,sPrdt_ac_id.ac_seqn);
                if(ret !=0 && ret != 100)
                {
                    strcpy(g_pub_tx.reply,"D103");
                    sprintf(acErrMsg,"查询in_mst出错！");
                    WRITEMSG
                    return 1;
                }
                else if(ret == 100)
                {
                    sprintf(acErrMsg,"无此内部账户，进行挂账判断");
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                    {
                        gz_reason[0] = '1';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"挂账账号选择函数错误");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        sprintf(acErrMsg,"无此内部账号，并且不挂账");
                        WRITEMSG
                        return 1;
                    }
                }
                vtcp_log("[%s][%d],开始检查账户状态、冻结和止付状态",__FILE__,__LINE__);
                if(sIn_mst.sts[0] != '1')
                {
                    sprintf(acErrMsg,"账户状态不正常ac_sts=【%s】，进行挂账判断",sIn_mst.sts);
                    WRITEMSG
                    if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                    {
                        gz_reason[0] = '3';
                        ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                        if(ret)
                        {
                            sprintf(acErrMsg,"挂账账号选择函数错误");
                            WRITEMSG
                            return 1;
                        }
                        return 0;
                    }
                    else
                    {
                        strcpy(g_pub_tx.reply,"O085");
                        sprintf(acErrMsg,"内部账号状态不正常，并且不挂账");
                        WRITEMSG
                        return 1;
                    }
                }
                strcpy(cTmp_ac_name,sIn_mst.name);
            }
            else
            {
                strcpy(g_pub_tx.reply,"P104");
                sprintf(acErrMsg,"该账号不是内部账号，不是活期账号,进行挂账判断");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"挂账账号选择函数错误");
                        WRITEMSG
                        return 1;
                    }
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P104");
                    sprintf(acErrMsg,"该账号不是内部账号，不是活期账号，并且不挂账");
                    WRITEMSG
                    return 1;
                }
            }
        }
        else
        {
            ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",ac_no);
            if(ret != 0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"D103");
                sprintf(acErrMsg,"查询科目主表出错");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                sprintf(acErrMsg,"无此科目，进行挂账判断");
                WRITEMSG
                if(gz_flag[0] == '3'||gz_flag[0] == '8')/*3 账号户名有一个不符就挂账*/
                {
                    gz_reason[0] = '1';
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    if(ret)
                    {
                        sprintf(acErrMsg,"挂账账号选择函数错误");
                        WRITEMSG
                        return 1;
                    }
                   /* memcpy(g_pub_tx.reply, "0000", 4);*/
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"P102");
                    sprintf(acErrMsg,"无此科目，并且不挂账");
                    WRITEMSG
                    return 1;
                }
            }
            strcpy(cTmp_ac_name,sCom_item.acc_name);
        }
        dbc_zipspace(ac_name);
        dbc_zipspace(cTmp_ac_name);
        sprintf(acErrMsg,"开始检查户名 cTmp_ac_name=[%s],ac_name=[%s]",cTmp_ac_name,ac_name);
        WRITEMSG
        /*挂账标志gz_flag为0不要求检查户名时不查*/
        sprintf(acErrMsg,"开始检查户名 gz_flag=[%s]",gz_flag);
        WRITEMSG
        if(gz_flag[0] != '0')
        {
            if(memcmp(cTmp_ac_name,ac_name,sizeof(cTmp_ac_name)-1) != 0)
            {
                sprintf(acErrMsg,"户名不符，进行挂账判断");
                WRITEMSG
                if(gz_flag[0] == '2'||gz_flag[0] == '3'||gz_flag[0] == '8') /*户名不符 要求挂账*/
                {
                    gz_reason[0] = '2';
                    /*账号存在，户名不符，账务挂在该账户的开户机构***20130114**YangMeng*/
                    ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
                    /* ret = gz_ac_no_sel(parm_seqn,cOpn_br_no,cOpn_br_no,ac_no,ac_name);*/
                    if(ret)
                    {
                        sprintf(acErrMsg,"挂账账号选择函数错误");
                        WRITEMSG
                        return 1;
                    }
                    sprintf(acErrMsg,"挂账机构gz_br_no【%s】",gz_br_no);
                    WRITEMSG
                    return 0;
                }
                else
                {
                    strcpy(g_pub_tx.reply,"NXD5");
                    sprintf(acErrMsg,"户名不符，并且不挂账");
                    WRITEMSG
                    return 1;
                }
            }
        }
        
        /*挂账标志gz_flag为8 如果账号存在强制挂账到开户机构 20130322 xuhaisheng*/
        if(gz_flag[0] == '8')/*退汇强制挂账，如果账户正确 挂账到开户结构，不正确挂到前台传来的挂账机构，一般为清算中心*/
        {
            gz_reason[0] = '7';/*其他错误*/
            ret = gz_ac_no_sel(parm_seqn,gz_br_no,cOpn_br_no,ac_no,ac_name);
            if(ret)
            {
                sprintf(acErrMsg,"挂账账号选择函数错误");
                WRITEMSG
                return 1;
            }
        }
    }
    sprintf(acErrMsg,"挂账检查结束了");
    WRITEMSG
    sprintf(acErrMsg,"ac_no=[%s],ac_name[%s],gz_flag=[%s],gz_br_no=[%s],gz_reason=[%s]",ac_no,ac_name,gz_flag,gz_br_no,gz_reason);
    WRITEMSG
  
    return 0;
}
/**********************************************************
    挂账账号选择 
    parm_seqn : com_parm 参数序号
    opn_br_no ：开户机构
    gz_br_no : 平台传送的挂账机构
    gz_ac_no :本函数返回挂账账号
    gz_ac_name :本函数返回挂账账号户名
修改：先查开户机构对应挂账账号。挂账顺序默认为 先开户机构，然后挂账机构，最后清算中心。
	其他情况如退汇，签名，密码错等根据前面前置挂账标志处理。chenggx 2013/3/15 11:38:12
**********************************************************/
int gz_ac_no_sel(int parm_seqn,char *gz_br_no,char *opn_br_no,char *gz_ac_no,char *gz_ac_name)
{
    int ret = 0;
    struct com_parm_c sCom_parm;
    struct mdm_ac_rel_c sMdm_ac_rel_gz;
    struct prdt_ac_id_c sPrdt_ac_id_gz;
    struct dd_mst_c sDd_mst_gz;
    struct com_item_c sCom_item_gz;
    struct in_mst_c sIn_mst_gz;
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(&sMdm_ac_rel_gz,0x00,sizeof(sMdm_ac_rel_gz));
    memset(&sPrdt_ac_id_gz,0x00,sizeof(sPrdt_ac_id_gz));
    memset(&sDd_mst_gz,0x00,sizeof(sDd_mst_gz));
    memset(&sCom_item_gz,0x00,sizeof(sCom_item_gz));
    memset(&sIn_mst_gz,0x00,sizeof(sIn_mst_gz));
    sprintf(acErrMsg,"开始进行挂账账号的选择,opn_br_no =[%s],gz_br_no=[%s]",opn_br_no,gz_br_no);
    WRITEMSG
    ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",opn_br_no,parm_seqn);
    if(ret !=0 && ret != 100)
    {
        strcpy(g_pub_tx.reply,"O005");
        sprintf(acErrMsg,"查询com_parm 错误");
        WRITEMSG
        return 1;
    }
    else if(ret == 100)
    {
        sprintf(acErrMsg,"平台传送的机构号没找到挂账账号，选择账号的开户机构");
        WRITEMSG
        ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",gz_br_no,parm_seqn);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"O005");
            sprintf(acErrMsg,"查询com_parm 错误");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            sprintf(acErrMsg,"平台传送的账号不存在，选择清算中心");
            WRITEMSG
            ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn = %d",QS_BR_NO,parm_seqn);
            if(ret)
            {
                strcpy(g_pub_tx.reply,"O005");
                sprintf(acErrMsg,"查询com_parm 错误,或不存在清算中心挂账账号ret=[%d]",ret);
                WRITEMSG
                return 1;
            }
        }
    }
    sprintf(acErrMsg,"开始对取出的挂账账号进行检查scom_parm.val【%s】",sCom_parm.val);
    WRITEMSG
    sprintf(acErrMsg,"挂账机构scom_parm.parm_code【%s】",sCom_parm.parm_code);
    WRITEMSG
    strcpy(gz_ac_no,sCom_parm.val);
    strcpy(gz_br_no,sCom_parm.parm_code);
    sprintf(acErrMsg,"挂账机构gz_br_no【%s】",gz_br_no);
    WRITEMSG
    /*set_zd_data("#sys77",gz_br_no);返回的挂账机构*/
    if(strlen(gz_ac_no) >10)
    {
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_gz,"ac_no='%s'",gz_ac_no);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"L015");
            sprintf(acErrMsg,"查询mdm_ac_rel出错！");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"挂账账号不存在");
            WRITEMSG
            return 1;
        }
        vtcp_log("[%s][%d],开始检查账号介质状态",__FILE__,__LINE__);
        if(sMdm_ac_rel_gz.note_sts[0] != '0')
        {
            strcpy(g_pub_tx.reply,"M013");
            sprintf(acErrMsg,"挂账账号介质状态不正常");
            WRITEMSG
            return 1;
        }
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id_gz,"ac_id = %ld order by ac_seqn",sMdm_ac_rel_gz.ac_id);
        if(ret !=0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"W013");
            sprintf(acErrMsg,"查询prdt_ac_id出错！");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"挂账账号不存在");
            WRITEMSG
            return 1;
        }
        if(sPrdt_ac_id_gz.ac_id_type[0] == '1')
        {
            ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_gz,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel_gz.ac_id,sPrdt_ac_id_gz.ac_seqn);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"X003");
                sprintf(acErrMsg,"查询dd_mst 错误");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                strcpy(g_pub_tx.reply,"P102");
                sprintf(acErrMsg,"挂账账号不存在");
                WRITEMSG
                return 1;
            }
            vtcp_log("[%s][%d],开始检查账户状态、冻结和止付状态",__FILE__,__LINE__);
            if(sDd_mst_gz.ac_sts[0] != '1')
            {
                strcpy(g_pub_tx.reply,"O085");
                sprintf(acErrMsg,"挂账账号状态不正常");
                WRITEMSG
                return 1;
            }
            if(sDd_mst_gz.hold_sts[0] == '1')
            {
                strcpy(g_pub_tx.reply,"P110");
                sprintf(acErrMsg,"挂账账号被全部冻结");
                WRITEMSG
                return 1;
            }
            strcpy(gz_ac_name,sDd_mst_gz.name);
        }
        else if(sPrdt_ac_id_gz.ac_id_type[0] == '9')
        {
            ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_gz,"ac_id=%ld and ac_seqn =%d",sMdm_ac_rel_gz.ac_id,sPrdt_ac_id_gz.ac_seqn);
            if(ret !=0 && ret != 100)
            {
                strcpy(g_pub_tx.reply,"D103");
                sprintf(acErrMsg,"查询in_mst出错！");
                WRITEMSG
                return 1;
            }
            else if(ret == 100)
            {
                strcpy(g_pub_tx.reply,"P102");
                sprintf(acErrMsg,"挂账账号不存在");
                WRITEMSG
                return 1;
            }
            vtcp_log("[%s][%d],开始检查账户状态、冻结和止付状态",__FILE__,__LINE__);
            if(sIn_mst_gz.sts[0] != '1')
            {
                strcpy(g_pub_tx.reply,"O085");
                sprintf(acErrMsg,"挂账账号状态不正常");
                WRITEMSG
                return 1;
            }
            strcpy(gz_ac_name,sIn_mst_gz.name);
        }
        else
        {
            strcpy(g_pub_tx.reply,"P104");
            sprintf(acErrMsg,"挂账账号类型错误");
            WRITEMSG
            return 1;
        }
    }
    else
    {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item_gz,"acc_no='%s'",gz_ac_no);
        if(ret != 0 && ret != 100)
        {
            strcpy(g_pub_tx.reply,"D103");
            sprintf(acErrMsg,"查询科目主表出错");
            WRITEMSG
            return 1;
        }
        else if(ret == 100)
        {
            strcpy(g_pub_tx.reply,"P102");
            sprintf(acErrMsg,"挂账科目不存在");
            WRITEMSG
            return 1;
        }
        strcpy(gz_ac_name,sCom_item_gz.acc_name);
    }
    return 0;
}
