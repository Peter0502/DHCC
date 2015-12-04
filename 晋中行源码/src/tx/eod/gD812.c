/*************************************************************
* 文 件 名: gD812.c
* 功能描述：一户通户计息程序
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_eod_exec_c.h"
#include "ac_yht_rel_c.h"
gD812()
{
		int ret = 0;
    struct dd_mst_c dd_mst1;
    struct dd_mst_c dd_mst2;
    struct dd_parm_c dd_parm_c;
    struct ac_yht_rel_c ac_yht_rel;
    double lmt_amt=0.0;
    double zz_lmt=0.0;
    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
    memset(&ac_yht_rel,0x0,sizeof(struct ac_yht_rel_c));
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
    vtcp_log("[%s][%d],++++++++++++\n",__FILE__,__LINE__);
    ret=sql_begin(); /*打开事务*/
		MYSQLERR
    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
	}
	set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
	set_zd_long( DC_TRACE_CNT,1 );
	strcpy(g_pub_tx.tx_code,"D812");

    ret = Ac_yht_rel_Dec_Sel(g_pub_tx.reply,"1=1");
		MYSQLERR
    while(1)
    {
        memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
        memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
        lmt_amt=0.0;
        zz_lmt=0.0;
        double tmpamt=0.0;
        ret = Ac_yht_rel_Fet_Sel(&ac_yht_rel,g_pub_tx.reply);
        if(ret==100)
        {
          break;
        }
        MYERR
        vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
        ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",ac_yht_rel.ac_id);
        MYERR
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        if(dd_mst1.ac_sts[0]!='1') /***活期户***/
        {
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
          continue;
        }
        if(dd_mst1.hold_sts[0]!='0')
        {
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst2,"ac_id=%ld",ac_yht_rel.yht_ac_id);
        if(dd_mst2.ac_sts[0]!='1') /***一户通***/
        {
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        if(dd_mst2.hold_sts[0]!='0' && dd_mst2.hold_sts[0]!='2')
        {
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
      
        tmpamt=ac_yht_rel.lmt_amt+ac_yht_rel.zz_lmt;
    
        vtcp_log("[%s][%d],%%%%****$$$$$$$$$$$$++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
        if(pub_base_CompDblVal(dd_mst1.bal,tmpamt)<0)
        {
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	continue;
        }
        else
        {
        	/**进行转存**/
         vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);

		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
        	ret=sub_change_acct(dd_mst1,dd_mst2,dd_mst1.bal - ac_yht_rel.lmt_amt );
        	MYERR
        }
		vtcp_log("[%s][%d] 冻1=[%s] 冻2=[%s]",__FILE__,__LINE__,dd_mst1.hold_sts,dd_mst2.hold_sts);
             vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);
   
    }
    Ac_yht_rel_Clo_Sel();
    set_zd_data(DC_CODE,"");
		set_zd_double("102P",0.00 );
		set_zd_double("102Q",0.00 );
    set_zd_data("102J","");
            vtcp_log("[%s][%d],++++++++++++dd_mst1=[%ld],dd_mst2=[%ld],lmt_amt=[%lf],zz_lmt=[%lf]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,ac_yht_rel.lmt_amt,ac_yht_rel.zz_lmt);

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"对私活期户计息程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"对私活期户计息程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
} 
int sub_change_acct(struct dd_mst_c dd_mst1,struct dd_mst_c dd_mst2,double amt)
{
		int ret=0;
		struct mdm_ac_rel_c vmdm_ac_rel;
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		/*记帐*/
		/**借存折户**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		g_pub_tx.tx_amt1=amt;
		strcpy(g_pub_tx.note_type,"812");
		strcpy(g_pub_tx.opn_br_no,dd_mst1.opn_br_no);
		strcpy(g_pub_tx.tx_br_no,dd_mst1.opn_br_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst1.ac_id);
		if(ret) goto ErrExit;
		vtcp_log("aaaaaaaaaaa[%s][%lf]\n",vmdm_ac_rel.ac_no,amt);
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst1.ac_id;
		g_pub_tx.ac_seqn =1;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转*/	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"一户通转" );	
		strcpy(g_pub_tx.sub_tx_code,"D003");
		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}
		/* 进行会计记帐 */
		set_zd_data("102J","01");         
		set_zd_double("102F",g_pub_tx.tx_amt1);                     

		
		strcpy(g_pub_tx.prdt_code,dd_mst1.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		
		/**贷一户通户**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst2.ac_id);
		if(ret) goto ErrExit;
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst2.ac_id;
		g_pub_tx.ac_seqn = 1 ;
		strcpy(g_pub_tx.opn_br_no,dd_mst2.opn_br_no);
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0加 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转*/	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"一户通转" );	
		strcpy(g_pub_tx.sub_tx_code,"D099");

		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}
		/* 进行会计记帐 */
		set_zd_data("101A","01");         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.prdt_code,dd_mst2.prdt_no);
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
 
