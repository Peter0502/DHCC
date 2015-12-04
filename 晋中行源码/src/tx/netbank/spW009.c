/*************************************************
* 文 件 名:    spW009.c
* 功能描述：   子账户（卡）信息查询
* 作    者：    dulaichun
* 时    间：    20130617

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
static struct mdm_ac_rel_c     f_mdm_ac_rel;
static struct dd_mst_c         sDd_mst;
static struct dd_mst_c         g_dd_mst;
static struct mo_prdt_prot_c   sMo_prdt_prot;
static struct cif_email_inf_c  g_cif_email_inf;
static struct cif_addr_inf_c   g_cif_addr_inf;
static struct dd_parm_c        g_dd_parm;
static struct mo_prot_ac_rel_c sMo_prot_ac_rel;
int spW009()
{
	
		char cFtpfile[200];
		FILE *fp;
		char filename[100];
		char tmpname[512],tmpstr[150];
    int      ret = 0, ac_seqn;
    int 			iNum = 0;
    char     ac_no[20], name[51];
    char     idtype[40];
    double   ac_bal=0.0;
    char     cif_no[20];
    double   bal=0.00;
  	char     tx_date[8+1];
		char   	 sw_traceno[10+1];
    memset(filename, 0x00, sizeof(filename));  
    memset(tmpname, 0x00, sizeof(tmpname));
    memset(cFtpfile, 0x00, sizeof(cFtpfile));   
    memset(ac_no, 0x00, sizeof(ac_no));  
    memset(name, 0x00, sizeof(name));
    memset(idtype ,0x00,sizeof(idtype));
    memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&f_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sMo_prdt_prot, 0x00, sizeof(struct mo_prdt_prot_c));
    memset(&sDd_mst, 0x00, sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00 , sizeof(struct dd_parm_c));
    memset(&g_dd_mst,       0x00, sizeof(struct dd_mst_c));
    memset(&sMo_prot_ac_rel,0x00, sizeof(struct mo_prot_ac_rel_c));
    memset(cFtpfile, 0, sizeof(cFtpfile));
    memset(tx_date, 0, sizeof(tx_date));
		memset(sw_traceno, 0, sizeof(sw_traceno));
    
     /** 取值、赋值 **/
    get_zd_data("0300", ac_no);  /* 帐号 */
    pub_base_old_acno(ac_no);    /* 对旧帐号的处理 */

		get_zd_data("0050",tx_date);
		get_zd_data("0520",sw_traceno);
   
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
		else
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		    if(ret == 100) 
		    {
		       sprintf(acErrMsg, "活期账号不存在!!!g_mdm_ac_rel.ac_no=[%s]", g_mdm_ac_rel.ac_no);
		       WRITEMSG
		       strcpy(g_pub_tx.reply, "W014");
		       goto ErrExit;
		    }
		    else if(ret)
		    {
			     sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
			     WRITEMSG
			     strcpy(g_pub_tx.reply, "C115");
			     goto ErrExit;
		    }
		      vtcp_log("[%s][%d] sDd_mst.name[%s]",__FILE__,__LINE__,sDd_mst.name);
		      vtcp_log("[%s][%d] sDd_mst.opn_br_no[%s]",__FILE__,__LINE__,sDd_mst.opn_br_no);
  				set_zd_data("0250",sDd_mst.name);
  				set_zd_data("0770",sDd_mst.opn_br_no);
    /**查询七天乐协议信息..**/
  	  ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &sMo_prdt_prot,"main_ac_id=%ld and main_ac_seqn=%ld and prot_type='TK7' and sts in ('1','2') ", g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
  	  if( ret==100 )
 	    {
        
            /**未签约..程序结束.回显到前台.**/
            sMo_prdt_prot.sts[0] = '*';
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
        		iNum=1;
            ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld  and main_ac_seqn=%ld and sts='1'",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
            if(ret)
            {
                sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "X013");
                goto ErrExit;
            }
            vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
            /*查询虚拟账户余额*/
              memset(&g_dd_mst,0x00, sizeof(struct dd_mst_c));
            ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%ld",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
            if(ret == 100) 
				    {
				       sprintf(acErrMsg, "活期虚拟子账号不存在!!!g_mdm_ac_rel.ac_no=[%s]", g_mdm_ac_rel.ac_no);
				       WRITEMSG
				       strcpy(g_pub_tx.reply, "W014");
				       goto ErrExit;
				    }
            if(ret)
            {
               sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
               WRITEMSG
               strcpy(g_pub_tx.reply, "C115");
               goto ErrExit;
            }
         /*查询虚拟账户账号*/
            ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &f_mdm_ac_rel, "ac_id=%ld and ac_seqn=%ld", g_dd_mst.ac_id,g_dd_mst.ac_seqn);
				    if(ret == 100) 
				    {
				       sprintf(acErrMsg, "不存在该子帐号!!ac_no=[%s]", g_mdm_ac_rel.ac_no);
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
        
  	  }
  	}
    /*开始写文件*/


  
  sprintf( filename,"1005netbank%s%s",tx_date,sw_traceno );
	sprintf( tmpname,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("[%s][%d] 要写的文件名tmpname[%s]",__FILE__,__LINE__,tmpname);
	fp=fopen(tmpname,"w");

   /**显示列标题**/
 	fprintf(fp,"~子账号|@币种|$钞汇标志|@储种|@存期|@利率|@到息日|@到期日|@可用余额|@余额|@状态|@开户日期|@开户网点|\n");

	vtcp_log("XXXXXXXXXXXXXX");
	vtcp_log("[%s][%d] g_mdm_ac_rel.ac_no[%s]",__FILE__,__LINE__,g_mdm_ac_rel.ac_no);
	vtcp_log("[%s][%d] sMo_prot_ac_rel.sts[%s]",__FILE__,__LINE__,sMo_prot_ac_rel.sts);
	vtcp_log("[%s][%d] g_dd_mst.ac_id[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
	vtcp_log("[%s][%d] g_dd_mst.ac_sts[%s]",__FILE__,__LINE__,g_dd_mst.ac_sts);
  vtcp_log("[%s][%d] g_dd_mst.cif_no[%d]",__FILE__,__LINE__,g_dd_mst.cif_no);
  vtcp_log("[%s][%d] g_dd_mst.odtt_ind[%s]",__FILE__,__LINE__,g_dd_mst.odtt_ind);
  vtcp_log("[%s][%d] g_dd_mst.rate[%.3lf]",__FILE__,__LINE__,g_dd_mst.rate);
  vtcp_log("[%s][%d] g_dd_mst.acbo_bal[%.2lf]",__FILE__,__LINE__,g_dd_mst.acbo_bal);
  vtcp_log("[%s][%d] g_dd_mst.opn_date[%d]",__FILE__,__LINE__,g_dd_mst.opn_date);
  vtcp_log("[%s][%d] g_dd_mst.opn_br_no[%s]",__FILE__,__LINE__,g_dd_mst.opn_br_no);
  
 	fprintf(fp,"%20s|%s|%1s|%s|%s|%.3lf|%s|%s|%.2lf|%.2lf|%2s|%6d|%6s|\n",f_mdm_ac_rel.ac_no,"CNY",g_dd_mst.odtt_ind,"5","7",g_dd_mst.rate," "," ",g_dd_mst.acbo_bal,g_dd_mst.acbo_bal,g_dd_mst.ac_sts,g_dd_mst.opn_date,f_mdm_ac_rel.opn_br_no);

	vtcp_log("XXXXXXXXXXXXXX1");
	fclose(fp);
	/*set_zd_data(DC_FILE_SND,"1");*/
	 vtcp_log("[%s][%d] iNum[%d]",__FILE__,__LINE__,iNum);
	 iNum=1;
		set_zd_int("0490",iNum);
			set_zd_data("0260",filename);
			sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
			{
			sprintf(acErrMsg,"ftp文件[%s]到平台出错[%d]",filename,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P157");
			goto ErrExit;
			}
			sprintf(acErrMsg,"ftp文件[%s]到平台成功",filename);
			WRITEMSG
	vtcp_log("XXXXXXXXXXXXXX2");
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
