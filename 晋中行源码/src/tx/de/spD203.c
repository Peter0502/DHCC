/***************************************************************
* 文 件 名: spD203.c
* 功能描述：销户
* 作    者: jack
* 完成日期：2004年1月6日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "cif_id_code_rel_c.h"
#include "com_rate_c.h"
#include "com_parm_c.h"
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/

spD203()
{
    int ret=0;
    char tmpname[100],intst_type[17],ct_name[5],term[3],term_type[3],memo[5],id_name[21];
    char br_name[31],br_tele[31];
    FILE *fp;
    char acc_no[8];
    char cAcno[20];
    double val=0.00 , dealval=0.00;
    double AC_BAL=0.00; /*电子现金账户余额*/ 
    double tx_amt=0.00;

    struct com_rate_c sCom_rate;
    struct cif_id_code_rel_c g_cif_id_code_rel;
    struct com_parm_c sCom_parm;
    struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/

    memset(&sCom_rate,0x00,sizeof(struct com_rate_c));
    memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
    memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
    memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    memset(term,0x00,sizeof(term));
    memset(cAcno,0x00,sizeof(cAcno));
    g_reply_int=0;

    /*** 初始化全局变量 ***/
    pub_base_sysinit();

    /*YW 20111212 IC卡账户销户时 销附属子账户*/
		
		get_zd_double("0420",&AC_BAL);
    get_zd_data("1021",g_pub_tx.ac_no);
    get_zd_data("1021",cAcno);
    get_zd_int("1022",&g_pub_tx.ac_seqn);
    get_zd_data("1023",g_pub_tx.note_type);
    get_zd_double("102I",&g_pub_tx.tx_amt1);
    get_zd_double("102I",&tx_amt);
/***    晋中商行IC卡电子账户不在核心
    if(!strcmp(g_pub_tx.note_type,ICCARDNOTETYPE))
    {
    	ret=pubf_base_IC(0,3);
    	if( ret )
    	{
    		sprintf(acErrMsg,"销IC卡子账户出错!");
    		WRITEMSG
    		goto ErrExit;
    	}
    	g_pub_tx.tx_amt1=g_dd_mst.bal;前台返回的值不包含子账户余额
    	vtcp_log("g_pub_tx.tx_amt1=%lf",g_pub_tx.tx_amt1);
    }***/
    
    /*增加控制短信存在签约信息不能进行账户销户,必须先进行解约----add by zgf 2013-07-25*/
	  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s'",g_pub_tx.ac_no);
	  if(ret==100){
	          vtcp_log("[%s][%d]账号[%s]未查询到短信签约记录,可以销户!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	  }else if(ret==0){
	          vtcp_log("[%s][%d]账号[%s]存在短信签约记录!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "该账号存在短信通知业务，尚未解约!!");
	          strcpy(g_pub_tx.reply,"F037");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	
	  }else{
	          vtcp_log("[%s][%d]查询短信银行签约表错误!!!",__FILE__,__LINE__,g_pub_tx.ac_no);
	          sprintf(acErrMsg, "查询短信银行签约表错误!!");
	          strcpy(g_pub_tx.reply,"XXXX");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	  }
	  /*end add by zgf*/
    
    /*IC卡销户时，先把电子现金余额圈提到主账户中*/
    if(!strcmp(g_pub_tx.note_type,ICCARDNOTETYPE))
    {
    	ret=Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='%s' and parm_seqn=11", g_pub_tx.opn_br_no);
    	if( ret )
 			{
 				sprintf(acErrMsg,"error");
 				WRITEMSG
 				goto ErrExit;
 			}
 			/*借方记账*/
 			ret=sub_cr_acct(sCom_parm.val,AC_BAL,"IC卡圈提销户" ,g_pub_tx.opn_br_no,"2");
 			if( ret )
 			{
 				sprintf(acErrMsg,"error");
 				WRITEMSG
 				goto ErrExit;
 			}
      if ( pub_base_acno_acid(&g_pub_tx.ac_id, cAcno) )
			{
			  goto ErrExit;
			}
 			/*贷方记账*/
 			ret=sub_dr_acct(cAcno,AC_BAL,"IC卡圈提销户" ,g_pub_tx.opn_br_no,"2");
 			if( ret )
 			{
 				sprintf(acErrMsg,"error");
 				WRITEMSG
 				goto ErrExit;
 			}
/*			ret=pubf_acct_IC(AC_BAL,sCom_parm.val);
 			if( ret )
 			{
 				sprintf(acErrMsg,"error");
 				WRITEMSG
 				goto ErrExit;
 			}*/
			g_pub_tx.tx_amt1=tx_amt+AC_BAL; /*主卡金额包含电子账户余额*/
    	vtcp_log("g_pub_tx.tx_amt1=%lf",g_pub_tx.tx_amt1); 
		}
		get_zd_data("1021",g_pub_tx.ac_no);
    get_zd_data("1024",g_pub_tx.beg_note_no);
    get_zd_data("1024",g_pub_tx.end_note_no);
    get_zd_data("1025",g_pub_tx.draw_uncon_yn); 
    get_zd_data("1026",g_pub_tx.draw_pwd_yn);   
    get_zd_data("1027",g_pub_tx.qry_pwd);
    get_zd_data("1028",g_pub_tx.draw_pwd);
    get_zd_data("1029",g_pub_tx.draw_id_yn);    
    get_zd_data("102G",g_pub_tx.id_type);
    get_zd_data("102A",g_pub_tx.id_no); 
    get_zd_data("102B",g_pub_tx.draw_seal_yn);  
    get_zd_data("102C",g_pub_tx.pwd_mach_yn);   
    get_zd_data("102D",g_pub_tx.draw_mach); 
    get_zd_long("102E",&g_pub_tx.mach_date);
    get_zd_data("102X",g_pub_intst.edu_ind); /**教育证明 1有**/
    get_zd_data("102J",g_pub_tx.cur_no);
    get_zd_data("102K",g_pub_tx.ct_ind);
    get_zd_data("102O",g_pub_tx.prt_ind);   /*是否打折标志*/
    get_zd_data("102Y",g_pub_tx.brf);       /*摘要内容*/
    pub_base_old_acno( g_pub_tx.ac_no);

    /* add by xxx **/
    get_zd_double("102P", &dealval);
    get_zd_double("102Q", &val);

    /** 修改了销户走帐方式,不把利息当做本金销户 
    g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 + dealval - val;
    ***/
    sprintf(acErrMsg, "**********CGH  【%lf】 【%lf】", val, dealval);
    WRITEMSG

    strcpy(g_pub_tx.add_ind,"0");
    g_pub_tx.svc_ind = 1003;
    strcpy(g_pub_tx.hst_ind,"1"); /* 日间入明细 */

    if(g_pub_tx.ct_ind[0]=='2')
        strcpy( memo,"转销" );
    else 
        strcpy( memo,"销户" );

    get_zd_data("0191",g_pub_tx.using_ind);
    if( g_pub_tx.using_ind[0]=='1' )
    {
        get_zd_data("0192",g_pub_tx.ac_wrk_ind);
    		vtcp_log("[%s][%d]aaaaaatx_code[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
        if(!strcmp(g_pub_tx.tx_code,"2409"))  /*mod by zgf 20140919*/
        {
        	g_pub_tx.svc_ind = 1005;
        }
        else
        {
        	get_zd_int("0193",&g_pub_tx.svc_ind);
        }
    }
    /*增加扣划特殊处理 2011/11/13 13:48:51*/
    if (memcmp(g_pub_tx.brf, "扣划业务", 8) == 0)
    {   
        memcpy(g_pub_tx.ac_wrk_ind, "0000", 4);
        g_pub_tx.ac_wrk_ind[9]='1';
        set_zd_data("1265","扣划交易");
        g_pub_tx.prt_ind[0]='0';
        sprintf(acErrMsg, "BEG D201LM2[%s][%s][%s][%d][%s]", g_pub_tx.ac_no,
        g_pub_tx.using_ind, g_pub_tx.ac_wrk_ind, g_pub_tx.svc_ind, g_pub_tx.brf);
    }   

    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }

    /******add by liuyue  20070208卡销户的时候需要首先销有的定期户*******/
    pub_base_strpack(g_pub_tx.ac_no);
    vtcp_log("[%s][%d]前台传入的帐号==[%s],帐户序号==[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
    if((memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) ==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) ==0) && !memcmp(g_pub_tx.ac_no+6,CARD_UNION_AREA,2))
    {
        vtcp_log("[%s][%d]!!!!!!这里进入卡销户的检测程序!\n",__FILE__,__LINE__);
        if(g_pub_tx.ac_seqn != 1)
        {
            sprintf(acErrMsg,"选择的销户不是销卡的活期主帐号!程序不理，继续执行!");
            WRITEMSG
        }
        else
        {
            vtcp_log("[%s][%d]您选择的是销卡主帐号  帐号==[%s],帐户序号==[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
            g_reply_int =   sql_count("td_mst","ac_id=%ld and ac_sts<>'*' and ac_seqn>1",g_mdm_ac_rel.ac_id) +
                            sql_count("dd_mst","ac_id=%ld and ac_sts<>'*' and ac_seqn>1",g_mdm_ac_rel.ac_id);   
            vtcp_log("[%s][%d]该卡号中序号大于1的记录条数===[%d]\n",__FILE__,__LINE__,g_reply_int);
            if(g_reply_int)
            {
                vtcp_log("[%s][%d]卡号中除了主帐号外还有[%d]个帐号没有销户!\n",__FILE__,__LINE__,g_reply_int);
                sprintf(acErrMsg,"卡[%s]中除主帐号外还有[%d]个帐号没有销户!\n",g_mdm_ac_rel.ac_no,g_reply_int);
                set_zd_data("0130",acErrMsg);
                strcpy(g_pub_tx.reply,"A033");
                goto ErrExit;
            }
                
        }
        vtcp_log("[%s][%d]卡销户检测成功!程序继续!\n",__FILE__,__LINE__);
        
    }
    
    /********************add end*****************************************/

    /***Add By Azz.Part For Print***/
    pub_base_AllDwnFilName( tmpname );
    fp=fopen(tmpname,"a");
    if( fp==NULL )
    {
        strcpy( g_pub_tx.reply,"B080" );
        sprintf(acErrMsg,"调用失败[%s]!",tmpname);
        WRITEMSG
        goto ErrExit;
    }

    pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
    if(g_pub_tx.prdt_code[0]=='1')
    {
        /*** 取证件信息 ***/
        g_reply_int = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
                                          "cif_no=%ld", g_dd_mst.cif_no);
        if( g_reply_int==100 )
        {
            sprintf(acErrMsg,"该客户不存在证件信息!");
            WRITEMSG
            strcpy( g_pub_tx.reply,"M003" );
            goto ErrExit;
        }
        else if( g_reply_int )
        {
            sprintf(acErrMsg,"取客户证件信息出错");
            WRITEMSG
            strcpy( g_pub_tx.reply,"D103" );
            goto ErrExit;
        }
        ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
        if( ret)
        {
            sprintf(acErrMsg,"取利率错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);

        g_reply_int = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }

        pub_base_getbrname(g_dd_mst.opn_br_no,br_name);
        pub_base_getbrtele(g_dd_mst.opn_br_no,br_tele);
        pub_base_strpack(br_tele);
        pub_base_strpack(br_name);
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_dd_mst.name);
        pub_base_strpack(g_dd_parm.title);
        if(  pub_base_prdt_accno( g_dd_mst.prdt_no,acc_no ) )
            strcpy( acc_no,"" );

        if (g_dd_mst.cif_no < 20000000)
        {
            /* 储蓄 */
        /***给这张凭证返回利率，存期设为空 add by wanglei 20060927***/
    sprintf(acErrMsg, "**********CGH  【%lf】 【%lf】", val, dealval);
    WRITEMSG
            fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%s|%s|%lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
            g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,
            g_dd_parm.title,g_dd_mst.opn_date,g_dd_mst.opn_br_no,
            g_dd_mst.cif_no, 0,"","",sCom_rate.rate_val,g_dd_mst.bal,"","","","","",
            memo,"",0.00,ct_name,g_pub_tx.ac_seqn,0,0,g_dd_mst.prdt_no,0,
            g_pub_tx.brf,"","","",id_name,g_cif_id_code_rel.id_no,
            br_name,br_tele, "","",acc_no);
        }
        else
        {
            /* 对公 */
            fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
            g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
            g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.cif_no,
            0,0,"",0.00,g_dd_mst.bal,"","","","","",memo,"",0.00,
            ct_name,g_pub_tx.ac_seqn,0,0,g_dd_mst.prdt_no,0,g_pub_tx.brf,"","","",
            id_name,g_cif_id_code_rel.id_no,br_name,br_tele,
            "","",acc_no);
        }
    }
    else if(g_pub_tx.prdt_code[0]=='2')
    {
    TRACE
        /*** 取证件信息 ***/
        g_reply_int = Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
                                  "cif_no=%ld", g_td_mst.cif_no);
        if( g_reply_int==100 )
        {
            sprintf(acErrMsg,"该客户不存在证件信息!");
            WRITEMSG
            strcpy( g_pub_tx.reply,"M003" );
            goto ErrExit;
        }
        else if( g_reply_int )
        {
            sprintf(acErrMsg,"取客户证件信息出错");
            WRITEMSG
            strcpy( g_pub_tx.reply,"D103" );
            goto ErrExit;
        }

        pub_base_dic_show_str(id_name,"id_type",g_cif_id_code_rel.id_type);
        vtcp_log("[%s][%d] term = %s\n",__FILE__,__LINE__,term);
        sprintf(term,"%02d",g_td_parm.term);
        vtcp_log("[%s][%d] term = %s\n",__FILE__,__LINE__,term);
        /***返回存期 add by wanglei 20060927***/
        if(g_td_parm.term_type[0] == 'Y')
            strcpy(term_type,"年");
        else if(g_td_parm.term_type[0] == 'M')
            strcpy(term_type,"月");
        else if(g_td_parm.term_type[0] == 'D')
            strcpy(term_type,"日");

        g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }

        pub_base_getbrname(g_td_mst.opn_br_no,br_name);
        pub_base_getbrtele(g_td_mst.opn_br_no,br_tele);
        pub_base_strpack(br_tele);
        pub_base_strpack(br_name);
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_td_mst.name);
        pub_base_strpack(g_td_parm.title);
        if(  pub_base_prdt_accno( g_td_mst.prdt_no,acc_no ) )
            strcpy( acc_no,"" );
        /***定期存在提前支取、到期销户和逾期销户三种情况,在此摘要修改一下.  added by wanglei 20070129***/
        if(g_pub_tx.ct_ind[0]=='2')
        {
            if(g_td_parm.atterm_date_ind[0] == 'Y' && g_td_parm.fwd_draw_ind[0] == 'Y' && g_pub_tx.tx_date < g_td_mst.mtr_date)
            {
                strcpy( memo,"转帐提前支取" );
            }
            else
            {
                strcpy( memo,"转帐到期销户" );
            }
        }
        else 
        {
            if(g_td_parm.atterm_date_ind[0] == 'Y' && g_td_parm.fwd_draw_ind[0] == 'Y' && g_pub_tx.tx_date < g_td_mst.mtr_date)
            {
                strcpy( memo,"提前支取" );
            }
            else
            {
                strcpy( memo,"到期销户" );
            }
        }
        /***ended by wanglei 20070129***/

        if (g_td_mst.cif_no < 20000000)
        {
            /* 储蓄 */
    sprintf(acErrMsg, "**********CGH  【%lf】 【%lf】", val, dealval);
    WRITEMSG
            fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
            g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,
            g_td_parm.title,
            g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.cif_no,
            g_td_mst.mtr_date,term,term_type,g_td_mst.rate,g_td_mst.bal,"","","","","",
            memo,"",dealval-val,
            ct_name,g_td_mst.ac_seqn,g_td_mst.ic_date,
            g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,g_td_mst.book_line,
            g_pub_tx.brf,"","","",id_name,g_cif_id_code_rel.id_no,
            br_name,br_tele,"","",acc_no);
        }
        else
        {
            fprintf(fp,"QKPT%s|%s|%.2lf|%s|%ld|%s|%ld|%ld|%d|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%d|%ld|%d|%s|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
            g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
            g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.cif_no,
            g_td_mst.mtr_date,0,"",0.00,g_td_mst.bal,"","","","","",
            memo,"",dealval,
            ct_name,g_td_mst.ac_seqn,g_td_mst.ic_date,
            g_mdm_ac_rel.prt_line,g_td_mst.prdt_no,g_td_mst.book_line,g_pub_tx.brf,
            "","","",id_name,g_cif_id_code_rel.id_no,br_name,br_tele,
            "","",acc_no);
        }
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");

    sprintf(acErrMsg, "*********** 交易金额=[%lf]",g_pub_tx.tx_amt1 );
    WRITEMSG

    set_zd_double("102I",g_pub_tx.tx_amt1);                         
    set_zd_double("102S",g_pub_tx.tx_amt1);
                                                                    /*应付总额*/     

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"销户成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    /*
    strcpy(g_pub_tx.reply,"A033");
    */
    sprintf(acErrMsg,"销户失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int sub_dr_acct(char * dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	/*贷方记账*/
	strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(g_pub_tx.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(g_pub_tx.ac_no,dr_ac_no);
	g_pub_tx.tx_amt1=amt;
	g_pub_tx.ac_seqn = g_pub_tx.ac_seqn ;
	g_pub_tx.ac_id= g_pub_tx.ac_id;
	strcpy(g_pub_tx.sub_tx_code,"D099");		
	strcpy( g_pub_tx.tx_code,"2204");
	strcpy( g_pub_tx.cur_no,"01");
	strcpy( g_pub_tx.ac_get_ind,"000");	
	strcpy( g_pub_tx.ac_id_type,"1" );	
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.prt_ind,"0" );
	strcpy( g_pub_tx.hst_ind,"1" );        
	strcpy( g_pub_tx.intst_type , "1" );	
	g_pub_tx.svc_ind=1001;	   
	strcpy ( g_pub_tx.brf,brf);
	if( pub_acct_trance() )
	{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
	}
	
	set_zd_double("1013",g_pub_tx.tx_amt1);
	set_zd_data("101A","01");
	set_zd_data("101B","2");                     
	strcpy(g_pub_tx.prdt_code,"");
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (ret != 0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
		return 0;
ErrExit:
	return 1;
}
int sub_cr_acct(char * cr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{	
	int ret=0;
	/*IC卡电子账户借方记账*/
	strcpy( g_pub_tx.tx_code, "2204");
	strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(g_pub_tx.opn_br_no,g_pub_tx.opn_br_no);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_wrk_ind,"000000011");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=9001;
	strcpy( g_pub_tx.ct_ind,"2" );
	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.prdt_code,"" );
	strcpy( g_pub_tx.ac_no,cr_ac_no );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=amt;
	strcpy( g_pub_tx.brf,brf );
	strcpy( g_pub_tx.no_show,"0" );
	strcpy( g_pub_tx.sub_tx_code, "A016");
	
	/*** 调用会计分录自动记 ***/
	ret=pub_acct_trance();
	if( ret ) goto ErrExit;
	
	/* 进行会计记帐 */
	set_zd_data("1204","01" );
	set_zd_data("1205","2" );
	set_zd_double("1208",amt);
	ret = pubf_acct_proc("A016");
	if (ret != 0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("1204","" );
	set_zd_data("1205","" );
	set_zd_double("1208",0.00 );
	set_zd_data("1214","" );
	set_zd_data("1215","" );
	set_zd_double("1218",0.00 );
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy( g_pub_tx.add_ind,"" );
		
	return 0;
ErrExit:
	return 1;
}
