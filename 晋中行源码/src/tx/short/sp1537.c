/*************************************************
* 文 件 名:    sp1537.c
* 功能描述：   七天乐存款协议账号信息查询户名和余额等信息
* 作    者：    wanghongjie 
* 时    间：    20110426
* insert into tx_def values('1537','七天乐信息查询','
10000000000010000000000010000000000000011110000000000000001001110000000000000000011000001000000000000000000000000000000000000100','0','4','0');
* insert into tx_flow_def values('1537','0','1537','#$'); 
* insert into tx_sub_def values('1537','七天乐信息查询','sp1537','0','0');
* lj...mod it .
s*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mo_prdt_prot_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "dd_parm_c.h"
#include "mo_prot_ac_rel_c.h"

static struct mdm_ac_rel_c     g_mdm_ac_rel;
static struct dd_mst_c         sDd_mst;
static struct dd_mst_c         g_dd_mst;
static struct mo_prdt_prot_c   sMo_prdt_prot;
static struct cif_email_inf_c  g_cif_email_inf;
static struct cif_addr_inf_c   g_cif_addr_inf;
static struct dd_parm_c        g_dd_parm;
static struct mo_prot_ac_rel_c sMo_prot_ac_rel;
int sp1537()
{
    int      ret = 0, ac_seqn;
    char     netmark[6];
    char     ac_no[20], name[51];
    char     idtype[40];
    double   ac_bal=0.00, ky_bal=0.00;
    char     cif_no[20];
    double   bal=0.00;
    memset(netmark, 0x00, sizeof(netmark));
    memset(ac_no, 0x00, sizeof(ac_no));  
    memset(name, 0x00, sizeof(name));
    memset(idtype ,0x00,sizeof(idtype));
    memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot, 0x00, sizeof(struct mo_prdt_prot_c));
    memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00 , sizeof(struct dd_parm_c));
    memset(&g_dd_mst,       0x00, sizeof(struct dd_mst_c));
    memset(&sMo_prot_ac_rel,0x00, sizeof(struct mo_prot_ac_rel_c));
    
     /** 取值、赋值 **/     
    get_zd_data("0100", netmark); 
    get_zd_data("0300", ac_no);  /* 帐号 */
    pub_base_old_acno(ac_no);    /* 对旧帐号的处理 */

		if ( memcmp(netmark,"netb",4)==0 ) /**从网银来查,全新的返回内容和处理方式.LZY 20141120 **/
		{
				    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
				    if(ret == 100) 
				    {
				        sprintf(acErrMsg, "该账号不存在!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "该账号不存在!");  
				        goto ErrExit;
				    }
				    else if(ret) 
				    {
				        sprintf(acErrMsg, "Mdm_ac_rel_Sel error ret=[%d]", ret);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "D103");
				        goto ErrExit;
				    }
				    else if ( g_mdm_ac_rel.note_sts[0] !='0' )
				    {
								sprintf(acErrMsg, "该账号状态不正常!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "该账号状态不正常!");  
				        goto ErrExit;
						}
            /*查询活期主表获取户名、可用金额等*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if(ret == 100)
            {
                sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D104");
								set_zd_data("0130", "该账号不存在!");  
                goto ErrExit;
            }
            else if(ret) 
            {
                sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
				    else if ( sDd_mst.ac_sts[0] !='1' )
				    {
								sprintf(acErrMsg, "该账号状态不正常!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "该账号状态不正常!");  
				        goto ErrExit;
						}
				    else if ( sDd_mst.hold_sts[0] =='1' || sDd_mst.hold_sts[0] =='2' )
				    {
								sprintf(acErrMsg, "该账号已被完全冻结或只进不出!!ac_no=[%s]", ac_no);
				        WRITEMSG
				        strcpy(g_pub_tx.reply, "AT01");
								set_zd_data("0130", "该账号已被完全冻结或只进不出!");
				        goto ErrExit;
						}
						
						ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
            set_zd_data("0250", sDd_mst.name);   /*客户名称*/
            set_zd_double("0400",sDd_mst.bal);   /*账户余额*/
            set_zd_double("0410",ky_bal);        /*账户可用金额*/

        ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts <> '*'", g_mdm_ac_rel.ac_id);
        if(ret == 100)
        {
            sMo_prdt_prot.sts[0] = 'W';
            set_zd_data("0690",sMo_prdt_prot.sts);/*账户状态是未签约.*/
            goto OkExit;
        }
        else if(ret)
        {
					sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
					WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
					goto ErrExit;
        }
        else
        {
            set_zd_data("0690",sMo_prdt_prot.sts);/**账户状态 1：签约 2：临时解约.**/        	

            ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1' ",g_mdm_ac_rel.ac_id);
            if(ret)
            {
                sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "X013");
 								set_zd_data("0130", "查询划款协议账户对照表出错!");
                goto ErrExit;
            }
            vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
            /*查询虚拟账户余额*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
            if(ret)
            {
                sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "C115");
								set_zd_data("0130", "查询七天乐账户出错!");  
                goto ErrExit;
            }
            set_zd_double("0420",g_dd_mst.bal); /**七天乐账户余额**/
						goto OkExit;
        }
    }    
		else  /**生产前台的老处理方式,写的很乱啊.LZY 20141120**/
		{
	   /* 根据帐号查询账户序号和账户ID */
	    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
	    if(ret == 100) 
	    {
	        sprintf(acErrMsg, "不存在该帐号!!ac_no=[%s]", ac_no);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "AT01");
	        goto ErrExit;
	    }
	    else if(ret) 
	    {
	        sprintf(acErrMsg, "Mdm_ac_rel_Sel error ret=[%d]", ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "D103");
	        goto ErrExit;
	    }
	    set_zd_data("0620",  g_mdm_ac_rel.id_no);     /*证件号码*/
	    /**查询七天乐协议信息..**/
	    ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts = '1' ", g_mdm_ac_rel.ac_id);
	    if( ret==100 )
	    {
	        ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts = '2' ", g_mdm_ac_rel.ac_id);
	        if(ret == 100)
	        {
	            /**未签约..程序结束.回显到前台.**/
	            sMo_prdt_prot.sts[0] = 'W';
	            set_zd_data("0690",sMo_prdt_prot.sts);/*账户状态 协议状态.*/
	            goto OkExit;
	        }
	        else if(ret)
	        {
	            sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        else
	        {
	            /**取得所需信息.**/
	            /*查询活期主表获取户名、可用金额等*/
	            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
	            if(ret == 100)
	            {
	                sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            } 
	            else if(ret) 
	            {
	                sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0250", sDd_mst.name);   /*客户名称*/
	            set_zd_double("0430",sDd_mst.bal);   /*主账户可用金额*/
	            /*查询客户地址信息表获取联系地址*/
	            ret=Cif_addr_inf_Sel(g_pub_tx.reply,&g_cif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
	            if(ret == 100) 
	            {
	                sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            }
	            else if(ret)
	            {
	                sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0820", g_cif_addr_inf.addr);/*联系地址*/
	             
	            /*查询客户电子信息表获取联系电话*/
	            ret=Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
	            if(ret == 100) 
	            {
	                sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D104");
	                goto ErrExit;
	            }
	            else if(ret)
	            {
	                sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "D103");
	                goto ErrExit;
	            }
	            set_zd_data("0830", g_cif_email_inf.email);/*移动电话*/
	            vtcp_log("[%s][%d] 留存金额=[%17.2f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
	            set_zd_double("0410",sMo_prdt_prot.leave_amt);  /*留存金额*/
	            ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1'",g_mdm_ac_rel.ac_id);
	            if(ret)
	            {
	                sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "X013");
	                goto ErrExit;
	            }
	            vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
	            /*查询虚拟账户余额*/
	            ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
	            if(ret)
	            {
	                sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
	                WRITEMSG
	                strcpy(g_pub_tx.reply, "C115");
	                goto ErrExit;
	            }
	            set_zd_double("0420",g_dd_mst.bal);             /*化转金额*/
	        }
	    }
	    else if(ret)
	    {
	        sprintf(acErrMsg, "Mo_prdt_prot_Sel error ret=[%d]", ret);
	        WRITEMSG
	        strcpy(g_pub_tx.reply, "D103");
	        goto ErrExit;
	    }
	    else
	    {
	        /**取得所需信息.**/
	        /*查询活期主表获取户名、可用金额等*/
	        ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
	        if(ret == 100)
	        {
	            sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	        } 
	        else if(ret) 
	        {
	            sprintf(acErrMsg, "Dd_mst_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0250", sDd_mst.name);   /*客户名称*/
	        set_zd_double("0430",sDd_mst.bal);   /*主账户可用金额*/
	        /*查询客户地址信息表获取联系地址*/
	        ret=Cif_addr_inf_Sel(g_pub_tx.reply,&g_cif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
	        if(ret == 100) 
	        {
	            sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            /** 不报错.
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	            **/
	        }
	        else if(ret)
	        {
	            sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0820", g_cif_addr_inf.addr);/*联系地址*/
	         
	        /*查询客户电子信息表获取联系电话*/
	        ret=Cif_email_inf_Sel(g_pub_tx.reply,&g_cif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
	        if(ret == 100) 
	        {
	            sprintf(acErrMsg, "记录不存在.ac_id=[%ld]", g_mdm_ac_rel.ac_id);
	            WRITEMSG
	            /**不报错.
	            strcpy(g_pub_tx.reply, "D104");
	            goto ErrExit;
	            **/
	        }
	        else if(ret) 
	        {
	            sprintf(acErrMsg, "Cif_addr_inf_Sel error ret=[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "D103");
	            goto ErrExit;
	        }
	        set_zd_data("0830", g_cif_email_inf.email);/*移动电话*/
	        vtcp_log("[%s][%d] 留存金额=[%17.2f]",__FILE__,__LINE__,sMo_prdt_prot.leave_amt);
	        set_zd_double("0410",sMo_prdt_prot.leave_amt);  /*留存金额*/
	        ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts='1'",g_mdm_ac_rel.ac_id);
	        if(ret)
	        {
	            sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "X013");
	            goto ErrExit;
	        }
	        vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
	        /*查询虚拟账户余额*/
	        ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
	        if(ret)
	        {
	            sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
	            WRITEMSG
	            strcpy(g_pub_tx.reply, "C115");
	            goto ErrExit;
	        }
	        set_zd_double("0420",g_dd_mst.bal);             /*化转金额*/        
	        
	    }
	    vtcp_log("[%s][%d] zyl-sts = [%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
	    /**add by jk 20130225 签约后未划转情况下,主账户取款使主账户金额不足51000 此时.显示为临时解约**/
	    if(pub_base_CompDblVal(g_dd_mst.bal,0.00)<=0 && pub_base_CompDblVal(sDd_mst.bal-sDd_mst.ctl_amt-sDd_mst.hold_amt,51000.00)<0 && sMo_prdt_prot.sts[0]=='1')
	    {
	        sMo_prdt_prot.sts[0] = 'P';
	    }
	    /**
	    sts=1 签约.
	    sts=2 临时解约.
	    sts=* 解约.不传送到前台.
	    sts=W 解约或未签约,不传送到前台.
	    sts=P 签约状态,视为临时解约状态.
	    **/
	    /**end by zyl 20110513**/
	    vtcp_log("[%s][%d] zyl-sts = [%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
	    set_zd_data("0690",sMo_prdt_prot.sts);          /*账户状态 协议状态.*/ 
	    bal=sDd_mst.bal+g_dd_mst.bal;
	    set_zd_double("0400", bal);                     /*总余额*/
		}
    
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
