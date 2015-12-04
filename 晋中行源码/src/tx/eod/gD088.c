/*************************************************************
* 文 件 名: gD088.c
* 功能描述：日终－贷款到期
*
* 作    者: lance
* 完成日期: 2003年3月24日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"

gD088()
{
    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    int    ac_seqn=0;
    int    coninue_flag=0;
    double vbnlx,vbwlx,vfllx,vbjlx;
    double amt1,amt2,amt3,amt4;
    double tx_amt1,tx_amt2,tx_amt3,tx_amt4;  
    char ln_ac_no[20],dd_ac_no[20],flag[2];
	long dd_ac_id=0;
    char type[2];
	
    char pact_no_tmp[20];
    memset(pact_no_tmp,0x00,sizeof(pact_no_tmp));
      
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    double bj_acm=0.00;
	double bn_acm=0.00;
	double bw_acm=0.00;
	double fl_acm=0.00;
	double bj_sect_acm=0.00;
	double bn_sect_acm=0.00;
	double bw_sect_acm=0.00;
	double fl_sect_acm=0.00;
	
    	char acm_type[2];
    vbnlx=vbwlx=vfllx=vbjlx=0.00;
    amt1=amt2=amt3=amt4=0.00;
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    
  	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /* 初始化全局变量 */
    pub_base_sysinit();
	TRACE
	set_zd_double("0400",0.00);
	set_zd_double("0410",0.00);
	set_zd_double("0440",0.00);
	set_zd_double("1089",0.00);
	set_zd_double("107F",0.00);
	set_zd_double("102F",0.00);
	set_zd_double("108F",0.00);
	set_zd_double("108H",0.00);
	set_zd_double("108I",0.00);
	set_zd_double("1074",0.00);
	set_zd_double("1075",0.00);		
	set_zd_double("1076",0.00);	
	set_zd_double("1077",0.00);
			
	/** dong 20030819 add ln_ac_sts!='*' **/
	TRACE
	/** lance 20030829 ln_ac_sts=='1' ****/
   /** ret = Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"((mtr_date>=20030826 AND mtr_date<=%ld ) OR \
								(exp_ind<>'0' AND exp_mtr_date>=20030826 AND exp_mtr_date<=%ld)) \ 
								AND ac_sts='1' and \
								prdt_no not in('3Z1','3Z2','3Z3','3Z4')",g_pub_tx.tx_date,g_pub_tx.tx_date);
    **/
    /*****modfied by ligl 2006-9-25 15:46*****/
    /**加了3Y2，3Y3。。。等多个委托贷款产品 20150616 wudawei**/
    ret=Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"((mtr_date=%ld) or (exp_ind<>'0' AND exp_mtr_date=%ld)) and ac_sts in('1','2') and prdt_no not in('3Z1','3Z2','3Z3','3Z4','3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB')",g_pub_tx.tx_date,g_pub_tx.tx_date);
    /******end********/
    if (ret)
    {
    	sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {

		/*** 数据初始化 ***/
   	memset( vln_intst,0,sizeof(vln_intst) );    
		memset( dd_ac_no, 0x00, sizeof(dd_ac_no));
		memset( ln_ac_no, 0x00, sizeof(ln_ac_no));
		memset( flag, 0x00, sizeof(flag));
		dc_flag=0;
		tx_amt1=tx_amt2=tx_amt3=tx_amt4=0.00;
    amt1=amt2=amt3=amt4=0.00;
    bj_acm=0.00;
		bn_acm=0.00;
		bw_acm=0.00;
		fl_acm=0.00;
		bj_sect_acm=0.00;
		bn_sect_acm=0.00;
		bw_sect_acm=0.00;
		fl_sect_acm=0.00;
		coninue_flag=0;
		strcpy( g_pub_tx.tx_code, "G088");		
		TRACE
      	ret = Eod_ln_mst_Fet_Upd(&g_ln_mst, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if (ret == 100)
        {
            break;
        }
			if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
			{
            sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
			}
      g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;
			strcpy(g_pub_tx.tx_br_no,g_ln_mst.opn_br_no);
      ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",g_ln_mst.ac_id);
      if ( ret == 100 )
      {
            sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
      }
      else if ( ret && ret!=100)
      {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
      }
      strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
      memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
			TRACE  
        /* 取贷款信息 * hao 
        ret = pub_ln_lndis_file(ln_ac_no,g_pub_tx.ac_seqn,
					&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); *读取贷款户信息*
		if( ret ) {return ret;}
		***************/
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
		if( ret ) 
		{
			strcpy(acErrMsg,"取贷款产品参数表错误!");
			WRITEMSG
			return ret;
		}
	/***
    if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
    {	
			TRACE
        	continue;
		}
	***/
		TRACE
		sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG
		/* 判断节假日是否处理 */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ){continue;}
		TRACE
    /* 取贷款状态 */
    chg_flag = pub_ln_check_stsdate();
		if( chg_flag==-1 )	/* 贷款不需要做到期处理 */
		{
			TRACE
			/**对已经展期的并且当前日期为到期日的，进行插入分段积数表操作 add by ligl 2006-12-15 23:34**/
			if(g_ln_mst.exp_ind[0]!='0' && g_ln_mst.mtr_date==g_pub_tx.tx_date)
			{
				double acm_rate=0.00;
				acm_rate=g_ln_mst.rate;
				ret=pub_ln_Ins_sect_rate( g_ln_mst, g_ln_parm, acm_rate );
				if( ret )	
				{
						sprintf(acErrMsg,"登记分段积数表失败!");
						WRITEMSG
						goto ErrExit;
				}	
				/**更新贷款主表的lst_date**/
				g_ln_mst.lst_date=g_pub_tx.tx_date;
				g_ln_mst.ic_date=g_pub_tx.tx_date;
				g_ln_mst.intst_acm=0.00;
				ret=Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
				if ( ret == -239 )
				{
					sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
					WRITEMSG
					strcpy(g_pub_tx.reply,"L031");
					goto ErrExit;
				}else if(ret)
				{
					sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
					WRITEMSG
					goto ErrExit;
				}
				/****/	
			}
			/***end**/
			continue;
		}
		/* 算息 */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_parm.intst_type[0]=='0' )	/*不计息*/
		{
			vbjlx=0.00; bj_acm=0.00;
			vbnlx=0.00; bn_acm=0.00;
			vbwlx=0.00; bw_acm=0.00;
			vfllx=0.00; fl_acm=0.00;
		}
		else
		{
			ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"0",&vbjlx,0 );
			if( ret ) goto ErrExit;
			
			strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
			bj_acm = g_ln_mst.intst_acm ;/* 利息积数 */

			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
								g_ln_parm.intst_days_type , acm_type , 
								g_ln_mst.bal ,g_ln_mst.bal , &bj_acm , "0",0,0);
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算积数错误!");
				WRITEMSG
				goto ErrExit;
			}
			sprintf(acErrMsg,"调用函数计算积数PASS为 [%lf]",bj_acm );
			WRITEMSG
			/*统计分段积数表中积数*/
			/* 计算分段积数利息 */
			strcpy( type , "1" ); /* 1-本金积数 */	
			ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
				type , &bj_sect_acm );
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",bj_sect_acm);
			WRITEMSG			
			if( g_ln_mst.cmpd_intst_ind[0]=='N' || g_ln_parm.cmpd_intst_ind[0]=='N')
			{
				vbnlx=0.00;
				vbwlx=0.00;
				vfllx=0.00;
			}else
			{
				ret = pub_ln_CalIntst ( g_ln_mst,g_ln_parm,"1",&vbnlx,0 );
				if( ret ) goto ErrExit;

				strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
				bn_acm = g_ln_mst.in_lo_acm ;/* 利息积数 */
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.in_lo_intst ,g_ln_mst.in_lo_intst , &bn_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算积数错误!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"调用函数计算表内积数PASS为 [%lf]",bn_acm );
				WRITEMSG
				/* 计算分段积数利息-表内 */
				strcpy( type , "2" ); /* 2-表内积数 */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &bn_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",bn_sect_acm);
				WRITEMSG
				
				
				
				ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"2",&vbwlx,0 );
				if( ret ) goto ErrExit;
				strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
				bw_acm = g_ln_mst.out_lo_acm ;/* 利息积数 */
	
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.out_lo_intst ,g_ln_mst.out_lo_intst , &bw_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算积数错误!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"调用函数计算表外积数PASS为 [%lf]",bw_acm );
				WRITEMSG
				/* 计算分段积数－表外 */
				strcpy( type , "3" ); /* 3-表外积数 */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &bw_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",bw_sect_acm);
				WRITEMSG
				
				
				
				ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"3",&vfllx,0 );
				if( ret ) goto ErrExit;
				strcpy( acm_type , "2" ); /* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
				fl_acm = g_ln_mst.cmpd_lo_acm ;/* 利息积数 */
	
				ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
									g_ln_parm.intst_days_type , acm_type , 
									g_ln_mst.cmpd_lo_intst ,g_ln_mst.cmpd_lo_intst , &fl_acm , "0",0,0);
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算积数错误!");
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"调用函数计算复利积数PASS为 [%lf]",fl_acm );
				WRITEMSG
				/* 计算分段积数－复利 */
				strcpy( type , "4" ); /* 4-复利积数 */	
				ret = pub_ln_Get_SectAcm( g_ln_mst.ac_id , g_ln_mst.ac_seqn , 
					type , &fl_sect_acm );
				if( ret )
				{
					sprintf(acErrMsg,"调用函数计算分段积数利息错误!");
					WRITEMSG
					return -1;	
				}	
				sprintf(acErrMsg,"调用函数计算分段积数利息PASS[%lf]!",fl_sect_acm);
				WRITEMSG				
				TRACE
			}
			TRACE
		}
vtcp_log("JJJJJ2222222222222222");	
		/* 判断本金利息 */
		bj_flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );/* 1表内  2表外 */
		TRACE
	
		/*写贷款登记簿  0登记欠款 1回收已登记欠款 2登记并回收*/
		/*********
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,"4",vbnlx+vbnlx+vfllx,"0",0);
		if( ret ) goto ErrExit;
		*******/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbnlx,bn_acm+bn_sect_acm,"0",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbwlx,bw_acm+bw_sect_acm,"0",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vfllx,fl_acm+fl_sect_acm,"0",0);
		if( ret ) goto ErrExit;				
		if( bj_flag==1 )
		{
	  		ret= pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",vbjlx,bj_acm+bj_sect_acm,"0",0);
	  		if( ret ) goto ErrExit;
		}else{
	  		ret= pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",vbjlx,bj_acm+bj_sect_acm,"0",0);
	  		if( ret ) goto ErrExit;
		}
		TRACE
		/* 对结出来的利息记账 * del by ly 不汇总记账
		if( pub_base_CompDblVal( vbnlx+vbwlx+vfllx+vbjlx ,0.00 ) )
		{
			** 记流水和记账(结出的利息入相应的科目) **
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	* 账号类型 3-贷款 *
	   		strcpy( g_pub_tx.add_ind,"1" );		* 增减:0减1加2销户 *	
	   		strcpy( g_pub_tx.ct_ind,"2" );		* 现转:1现2转 *	
	   		strcpy( g_pub_tx.prt_ind,"0" );*是否打印存折(有折无折标志):1是0否*
	   		strcpy( g_pub_tx.hst_ind,"1" );*入明细:1日间入2日终单笔入
																3日终汇总入* 
			strcpy ( g_pub_tx.intst_type , "1" );	* 计息类型 *
	  		g_pub_tx.svc_ind=3052;	* 积数清零,欠息增加 *
	  		if(bj_flag==1)
	  		{
	   			g_pub_tx.tx_amt2=vbjlx;
	   		}else{
	   			g_pub_tx.tx_amt3=vbjlx;
	   		}
	   		g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
			strcpy ( g_pub_tx.brf,"贷款结息" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		*** 交易记帐处理 ***
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"交易记帐处理错误!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
	   		*** 会计记帐 ***
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0400",g_pub_tx.tx_amt2);    
			set_zd_double("0410",g_pub_tx.tx_amt3); 
			set_zd_double("1089",g_pub_tx.tx_amt4);                                     
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"会计记帐不成功!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt2=0.00;
           	g_pub_tx.tx_amt3=0.00;
           	g_pub_tx.tx_amt4=0.00;
			set_zd_double("0400",0.00);    
			set_zd_double("0410",0.00); 
			set_zd_double("1089",0.00);         	
      }
      ***************************/
      		/*****************add by ly 分列的各种积数结息 **************/
  		/*判断本金积数入表内还是表外*/
  		if(bj_flag==1)
  		{
   			g_pub_tx.tx_amt2=vbjlx;
   		}else{
   			g_pub_tx.tx_amt3=vbjlx;
   		}
	   	g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
	   		
vtcp_log("JJJJJ333333333333333333");	
		/* 对结出来的利息记账--表内利息 */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt2 ,0.00 ) )
		{
			/** 记流水和记账(结出的利息入相应的科目) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
															3日终汇总入*/  
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
          		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
			strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
			g_ln_mst.in_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET in_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;	  		
	  		g_pub_tx.svc_ind=3020;	/* 积数清零,欠息增加 */	
			strcpy ( g_pub_tx.brf,"本金积数结息入表内" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** 交易记帐处理 ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"交易记帐处理错误!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
          		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
	   		/*** 会计记帐 ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0400",g_pub_tx.tx_amt2);                                    
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"会计记帐不成功!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt2=0.00;
			set_zd_double("0400",0.00);           	
      }
      
 		/* 对结出来的利息记账--表外利息 */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt3 ,0.00 ) )
		{
			/** 记流水和记账(结出的利息入相应的科目) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
			 													3日终汇总入*/  
			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
           		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG			
			strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
			
	  		g_ln_mst.out_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET out_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;
			
	  		g_pub_tx.svc_ind=3030;	/* 积数清零,欠息增加 */	
			strcpy ( g_pub_tx.brf,"本金积数结息入表外" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** 交易记帐处理 ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"交易记帐处理错误!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
           		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
           		WRITEMSG
	   		/*** 会计记帐 ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("0410",g_pub_tx.tx_amt3);                                
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"会计记帐不成功!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt3=0.00;
			set_zd_double("0410",0.00);         	
      }     
      
		/* 对结出来的利息记账--复利利息 */
		g_pub_tx.tx_amt4=0.00;/**被GXJ增加***/
		if( pub_base_CompDblVal( g_pub_tx.tx_amt4 ,0.00 ) )
		{
			/** 记流水和记账(结出的利息入相应的科目) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	   		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/  
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
			strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	  		g_ln_mst.cmpd_lo_acm=0.00;
			ret = sql_execute( "UPDATE ln_mst SET cmpd_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;
	  		g_pub_tx.svc_ind=3040;	/* 积数清零,欠息增加 */	
			strcpy ( g_pub_tx.brf,"其他积数结息入复利" );
			sprintf(acErrMsg,"~~~~backin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~backcmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG	
	   		/*** 交易记帐处理 ***/
	   		if( pub_acct_trance() )
	   		{
           		sprintf(acErrMsg,"交易记帐处理错误!");
           		WRITEMSG
           		goto ErrExit;
	   		}
	   		sprintf(acErrMsg,"~~~~1in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   		sprintf(acErrMsg,"~~~~1cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
	   		/*** 会计记帐 ***/
			set_zd_data("0210",g_ln_parm.cur_no); 
			set_zd_double("1089",g_pub_tx.tx_amt4);                                 
			strcpy(g_pub_tx.sub_tx_code,"G083");
           	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
           	if (ret != 0)
           	{
           		sprintf(acErrMsg,"会计记帐不成功!!");
           		WRITEMSG
           		goto ErrExit;
           	}
           	g_pub_tx.tx_amt4=0.00;
			set_zd_double("1089",0.00);         	
      }    
      /***  add by lance  20030902 清理主文件中积数 ****/
	ret = sql_execute( "UPDATE ln_mst SET intst_acm=0.00,in_lo_acm=0.00,out_lo_acm=0.00,cmpd_lo_acm=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
					g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret ) goto ErrExit;  
      	/*数据清理*/
	bj_acm=0.00;
	bn_acm=0.00;
	bw_acm=0.00;
	fl_acm=0.00;
	bj_sect_acm=0.00;
	bn_sect_acm=0.00;
	bw_sect_acm=0.00;
	fl_sect_acm=0.00;
	
	  /*重取贷款主文件*/
	  ret= Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	  if(ret)
	  {
			goto ErrExit;
	  }
		/* 判断自动还款标志 */
		if ( g_ln_parm.auto_pay_ind[0]=='Y')
		{
		TRACE
	   		/* 根据币种生成活期户的账号ID */ 
	   		ret=pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
	   		if ( ret )
	   		{
				sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
				WRITEMSG
				goto ErrExit;	
	   		}	  
	   		/* 取还款账户信息 */
	   		ret=pub_base_currentid_file(g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst);
	   		if( ret )
	   		{
				sprintf(acErrMsg,"调用根据币种、显示账号取活期主文件函数错误");
				WRITEMSG
				goto ErrExit;
	   		}
           	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld order by note_sts ", 
															g_dd_mst.ac_id);
           	if ( ret == 100 )
           	{
                sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
                WRITEMSG
                strcpy (g_pub_tx.reply,"W010");
                goto ErrExit;
           	}else if ( ret && ret!=100){
                sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
                WRITEMSG
                strcpy (g_pub_tx.reply,"W011");
                goto ErrExit;
           	}
           	strcpy( dd_ac_no ,g_mdm_ac_rel.ac_no );
           	dd_ac_id =g_mdm_ac_rel.ac_id ; /*保存*/
	        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
	        	
	        ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
																g_dd_mst.prdt_no);
	        if( ret==100 )
	        {
	            sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%s]",g_dd_mst.prdt_no);
	            WRITEMSG
	            strcpy(g_pub_tx.reply,"P145");
	            goto ErrExit;
	        }else if( ret ){
	            sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
	            WRITEMSG
	            goto ErrExit;
	        }		
			TRACE
			if( g_dd_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"账号状态非正常状态!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P109");
				coninue_flag++;
			}
			/* 账号完全冻结或者只进不出 */
			if( g_dd_mst.hold_sts[0]=='1'||g_dd_mst.hold_sts[0]=='2' )
			{
				sprintf(acErrMsg,"该账号已被冻结!");
				WRITEMSG
				coninue_flag++;			
			}
			if( g_dd_parm.draw_ind[0]=='N' )
			{
				sprintf(acErrMsg,"不可续取[%s]!",g_dd_parm.draw_ind);
				WRITEMSG
				strcpy(g_pub_tx.reply,"A023");
				coninue_flag++;
			}
			/* 金额检查 */
			ret = pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal,0.00);
			if( ret<=0 )
			{	
				coninue_flag++;
			}else{
				g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
			}
		
			/* 考虑部分冻结状态 */
			if( g_dd_mst.hold_sts[0]=='3' )
			{
				ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1-g_dd_mst.hold_amt , 0.00);
				if( ret<=0 )
				{
					coninue_flag++;
				}else{
					g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	
				}
			}
			sprintf(acErrMsg,"原始可用存款户余额【%lf】!",g_pub_tx.tmp_amt1);
			WRITEMSG
	
			if( coninue_flag )
			{
				sprintf(acErrMsg,"存款账户状态不正确![%d]",coninue_flag);
				WRITEMSG
				g_pub_tx.tmp_amt1=0.00;
			}				
			TRACE	            	   

	   		/*确定各项欠息的还款金额  0本金、1本金利息、2表内欠息、3表内复利、
			4表外欠息、5表外复利、6复利欠息、7复利复利*/
	   		i=0;
	   		vln_intst[i].intst_amt=g_ln_mst.bal;           vln_intst[i].type=7;/*原数0*/
	   		i++;
			/****
	   		vln_intst[i].intst_amt=vbjlx;                  vln_intst[i].type=1;
			***/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=6;/*原数1*/
	   		i++;
	   		vln_intst[i].intst_amt=g_ln_mst.in_lo_intst;   vln_intst[i].type=5;/*原数2*/
	   		i++;
			/****
	   		vln_intst[i].intst_amt=vbnlx;                  vln_intst[i].type=3;
			****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=4;/*原数3*/
	   		i++;
	   		vln_intst[i].intst_amt=g_ln_mst.out_lo_intst;  vln_intst[i].type=3;/*原数4*/
	   		i++;	  
			/****
	   		vln_intst[i].intst_amt=vbwlx;                  vln_intst[i].type=5;
			****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=2;/*原数5*/
	   		i++;
	/***被GXJ修改   		
	vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=1;***/
    vln_intst[i].intst_amt=0.00; vln_intst[i].type=1;/*原数6*/
	   		i++;
			/*****
	   		vln_intst[i].intst_amt=vfllx;                  vln_intst[i].type=7;
			*****/
	   		vln_intst[i].intst_amt=0.00;                  vln_intst[i].type=0;/*原数7*/
	   		i++;
			TRACE
	  		/* 根据还款顺序表顺序还款 */	  
	   		ret=pub_ln_OrderPay(&vln_intst,g_ln_parm.pay_seq_code,
															&g_pub_tx.tmp_amt1);
	  		if( ret ) goto ErrExit;
	  				
				sprintf(acErrMsg,"存款账号[%ld]",g_dd_mst.ac_id);
				WRITEMSG
				sprintf(acErrMsg,"贷款账号[%ld]",g_ln_mst.ac_id);
				WRITEMSG
			 	sprintf(acErrMsg,"存款余额[%lf]",g_pub_tx.tmp_amt1);
				WRITEMSG
			 	sprintf(acErrMsg,"贷款余额:本金[%lf]表内[%lf]表外[%lf]复利[%lf]",
					g_ln_mst.bal,g_ln_mst.in_lo_intst,g_ln_mst.out_lo_intst,g_ln_mst.cmpd_lo_intst);
				WRITEMSG
			 	sprintf(acErrMsg,"贷款积数:本金[%lf]表内[%lf]表外[%lf]复利[%lf]",
						vbjlx,vbnlx,vbwlx,vfllx);
				WRITEMSG
       }
    TRACE
	
vtcp_log("JJJJJ4444444444444444444");	
	/* 本金 */
	tx_amt1= vln_intst[0].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(g_ln_mst.bal,vln_intst[0].pay_amt) )
	{
	 	amt1= g_ln_mst.bal-vln_intst[0].pay_amt;	  /*挂账金额*/
	 	dc_flag++;
	}	
	TRACE
	/* 本金利息 */
	if ( bj_flag==1 )
	{
	 	tx_amt2= vln_intst[1].pay_amt; /*记账金额*/
	}else{
	 	tx_amt3= vln_intst[1].pay_amt; /*记账金额*/
	}	  
	if( pub_base_CompDblVal(vbjlx,vln_intst[1].pay_amt) )
	{
		if ( bj_flag==1 )
	  	{	  	
	    	amt2= vbjlx-vln_intst[1].pay_amt;	  /*挂账金额*/
	  	}else{
	    	amt3= vbjlx-vln_intst[1].pay_amt;	  /*挂账金额*/
	  	}
	  	dc_flag++;
	}
	TRACE
	/* 表内欠息 */
	tx_amt2+= vln_intst[2].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,vln_intst[2].pay_amt) )
	{
		amt2+= g_ln_mst.in_lo_intst-vln_intst[2].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}
	/* 表内复利 */
	tx_amt4+= vln_intst[3].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(vbnlx,vln_intst[3].pay_amt) )
	{
		amt4+= vbnlx-vln_intst[3].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}	
	/* 表外欠息 */
	tx_amt3+= vln_intst[4].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,vln_intst[4].pay_amt) )
	{
		amt3+= g_ln_mst.out_lo_intst-vln_intst[4].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}	
	/* 表外复利 */
	tx_amt4+= vln_intst[5].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(vbwlx,vln_intst[5].pay_amt) )
	{
		amt4+= vbwlx-vln_intst[5].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}	
	/* 复利欠息 */
	tx_amt4+= vln_intst[6].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,vln_intst[6].pay_amt) )
	{
		amt4+= g_ln_mst.cmpd_lo_intst-vln_intst[6].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}	
	/* 复利利息 */
	tx_amt4+= vln_intst[7].pay_amt; /*记账金额*/
	if( pub_base_CompDblVal(vfllx,vln_intst[7].pay_amt) )
	{
		amt4+= vfllx-vln_intst[7].pay_amt;	  /*挂账金额*/
		dc_flag++;
	}
	TRACE

	/*写贷款登记簿  0登记欠款 1回收已登记欠款 2登记并回收*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"1",tx_amt1,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"1PASS");
	WRITEMSG

	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"2",tx_amt2,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"2PASS");
	WRITEMSG
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"3",tx_amt3,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"3PASS");
	WRITEMSG
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"4",tx_amt4,0.00,"0",1 );
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"4PASS");
	WRITEMSG

	/*将积数产生的利息累加到分户账
	if( bj_flag==1 )
	{
		g_ln_mst.in_lo_intst+= vbjlx;
	}else if( bj_flag==2 ){
		g_ln_mst.out_lo_intst+= vbjlx;
	}else{
		goto ErrExit;
	}
	
	g_ln_mst.cmpd_lo_intst+= vbnlx+vbwlx+vfllx;
	***********/

vtcp_log("JZAdd_ac[%s]ln_ac[%s]tx_amt1[%lf]tx_amt2[%lf]tx_amt3=[%lf]tx_amt4[%lf]dc_flag=[%d]",
	dd_ac_no,ln_ac_no,tx_amt1,tx_amt2,tx_amt3,tx_amt4,dc_flag);
vtcp_log("JZBdd_id[%d]ln_id[%d]tx_amt1[%lf]tx_amt2[%lf]tx_amt3=[%lf]tx_amt4[%lf]dc_flag=[%d]",
	g_dd_mst.ac_id,g_ln_mst.ac_id,tx_amt1,tx_amt2,tx_amt3,tx_amt4,dc_flag);
vtcp_log("TRACE_NO=[%d]",g_pub_tx.trace_no);
	vtcp_log("ACID=[%ld]  AC_NO=[%s],%d",g_dd_mst.ac_id,dd_ac_no,__LINE__);
	
    if( pub_base_CompDblVal( tx_amt1 ,0.00 ) )
    {
	TRACE
    	/*************** 记流水和记账(可还本金部分) **************/
    	/******贷款部分********/
    	strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
    	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*未读取了mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		/****
		strcpy( ac_get_ind,"111" );			*不读取分户账，参数表，介质表*
		***/
		strcpy( g_pub_tx.note_type,"299");
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	   	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
        strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	   	g_pub_tx.svc_ind=3010;
	   	g_pub_tx.tx_amt1=tx_amt1;	
        strcpy ( g_pub_tx.brf,"日终到期贷款还本金" );
	
	   	/*** 交易记帐处理 ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	   	}
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	/* 进行会计记帐 */
        set_zd_data("0210",g_ln_parm.cur_no);  
        set_zd_double("107F",g_pub_tx.tx_amt1);       
        strcpy(g_pub_tx.sub_tx_code,"L201");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_double("107F",0.00);                       
        /* 委托协议贷款(还本) */
    /**委托贷款批量处理有问题，暂时封上不处理JYW * 
		ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "2" );   
        if( ret )  goto ErrExit;   
	*/
		vtcp_log("处理完ln_trust本金[%s][%d]\n",__FILE__,__LINE__);
	   	g_pub_tx.tx_amt1=0.00;
	    /**********取款部分************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*未读取了mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-活期 */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/ 
 		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*为日终传票打印赋值dd_ac_no*/
        strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.ac_wrk_ind[9]='1';     /*对于非结算户强行扣款*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */

	   	g_pub_tx.svc_ind=1001;
	   	g_pub_tx.tx_amt1=tx_amt1;	
        strcpy ( g_pub_tx.brf,"贷款还本" );
	
vtcp_log("JJJJJ666666666666666666");	
	   	/*** 交易记帐处理 ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	   	}
	    /* 进行会计记帐 */
        set_zd_data("102J",g_ln_parm.cur_no);      
        set_zd_double("102F",g_pub_tx.tx_amt1);             
        strcpy(g_pub_tx.sub_tx_code,"D003");/**取款子交易**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
             sprintf(acErrMsg,"会计记帐不成功!!");
             WRITEMSG
             goto ErrExit;
        }
        set_zd_double("102F",0.00);             
	   	g_pub_tx.tx_amt1=0.00;
    }
vtcp_log("JJJJJ77777777777777777777");	

		vtcp_log("ACID=[%ld]  AC_NO=[%s],%d",g_dd_mst.ac_id,dd_ac_no,__LINE__);
	
    if( pub_base_CompDblVal( tx_amt2+tx_amt3+tx_amt4 ,0.00 ) )
    {
	TRACE
     /*************** 记流水和记账(可还利息部分) **************/
     /******贷款部分********/
    	strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
    	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*未读取了mdm_ac_rel*/
   		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
   		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/ 
   		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
 		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/ 
  		g_pub_tx.svc_ind=3050;
  		g_pub_tx.tx_amt2=tx_amt2;
  		g_pub_tx.tx_amt3=tx_amt3;
  		g_pub_tx.tx_amt4=tx_amt4;	
        strcpy ( g_pub_tx.brf,"日终到期贷款还利息" );
	
	    /*** 交易记帐处理 ***/
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	    }
	    /* 进行会计记帐 */
        set_zd_data("0210",g_ln_parm.cur_no); 
        set_zd_double("108F",g_pub_tx.tx_amt2);    
        set_zd_double("108H",g_pub_tx.tx_amt3); 
        set_zd_double("108I",g_pub_tx.tx_amt4);                                     
        strcpy(g_pub_tx.sub_tx_code,"L202");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }

		vtcp_log("ACID=[%ld]  AC_NO=[%s]",g_dd_mst.ac_id,dd_ac_no);

        /* 委托协议贷款(还息) */
/************JYW*/
/*	if(g_ln_mst.mtr_date==g_pub_tx.tx_date){*/
        ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
        if( ret )  goto ErrExit;           
/*	}*/
/***************/
		vtcp_log("处理完ln_trust利息[%s][%d]\n",__FILE__,__LINE__);
           
	    g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;	   
        set_zd_double("108F",0.00);    
        set_zd_double("108H",0.00); 
        set_zd_double("108I",0.00);                                     
	   	/**********取款部分************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
	/*JYW	g_pub_tx.ac_id = g_dd_mst.ac_id ;***/
		g_pub_tx.ac_id = dd_ac_id ;
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;

		vtcp_log("ACID=[%ld]  AC_NO=[%s]",g_dd_mst.ac_id,dd_ac_no);

	    strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
	    strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
	    strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	    strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	    strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
	    strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
   		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*为日终传票打印赋值dd_ac_no*/
        strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	    g_pub_tx.svc_ind=1001;
		g_pub_tx.ac_wrk_ind[9]='1';     /*对于非结算户强行扣款*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */

	    /**** add by ly 2003-08-27 23:24  ***/
	    g_pub_tx.tx_amt1=tx_amt2+tx_amt3+tx_amt4;
	    
	    /**** del by ly 2003-08-27 23:24
	    g_pub_tx.tx_amt2=tx_amt2;	
	    g_pub_tx.tx_amt3=tx_amt3;	   
	    g_pub_tx.tx_amt4=tx_amt4;
	    *****/	   
        strcpy ( g_pub_tx.brf,"日终贷款到期还息" );
	
	    /*** 交易记帐处理 ***/
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	    }
	    /* 进行会计记帐 */
        set_zd_data("102J",g_ln_parm.cur_no);      
        set_zd_double("102F",g_pub_tx.tx_amt1);     
        strcpy(g_pub_tx.sub_tx_code,"D003");/**取款子交易**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
	   	g_pub_tx.tx_amt1=0.00;
	   	g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;
        set_zd_double("102F",0.00);
    }
   
    strncpy(pact_no_tmp,g_ln_mst.pact_no,strlen(g_ln_mst.pact_no)-3); 
    /********houyi20100620*********/
    
		if(pub_base_CompDblVal(tx_amt2 + tx_amt3 + tx_amt4,0.00)>0)  /*mod by lwb 20150413  利息和本金分开发短信*/
		{	
			/*pub_mob_sendmail_ln("归还本金","Z088",g_mdm_ac_rel.ac_no,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1);*/
			pub_mob_sendmail_ln("归还利息","Z088",pact_no_tmp,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1); /*mod by lwb 20150408*/
   	}
   	if(pub_base_CompDblVal(tx_amt1,0.00)>0)  /*mod by lwb 20150413  利息和本金分开发短信*/
		{	
			/*pub_mob_sendmail_ln("归还本金","Z088",g_mdm_ac_rel.ac_no,"1",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,amt1);*/
			pub_mob_sendmail_ln("归还本金","Z088",pact_no_tmp,"1",tx_amt1,tx_amt1,amt1); /*mod by lwb 20150408*/
   	}
    
vtcp_log("JJJJJ888888888888888888888");	
    /***********挂账处理************************	
    if( dc_flag!=0 )
    {
     	**** 挂账金额转出部分********
        strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	未读取了mdm_ac_rel
	    strcpy( g_pub_tx.ac_id_type,"3" );	 账号类型 3-贷款 
	    strcpy( g_pub_tx.add_ind,"1" );		增减:0减1加2销户 	
	    strcpy( g_pub_tx.ct_ind,"2" );		 现转:1现2转 	
	    strcpy( g_pub_tx.prt_ind,"0" );是否打印存折(有折无折标志):1是0否 
	    strcpy( g_pub_tx.hst_ind,"1" );入明细:1日间入2日终单笔入3日终汇总入
        strcpy ( g_pub_tx.intst_type , "1" );	 计息类型 	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);    为日终传票打印赋值ln_ac_no
	    g_pub_tx.svc_ind=3063;
	    g_pub_tx.tx_amt1=-amt1;
	    **
	    g_pub_tx.tx_amt2=amt2;
	    g_pub_tx.tx_amt3=amt3;
	    g_pub_tx.tx_amt4=amt4;
	    ***	   	  	   	
        strcpy ( g_pub_tx.brf,"日终贷款到期转出" );
	
	    ** 交易记帐处理 ***
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	    }
	    * 进行会计记帐 *
        set_zd_data("0210",g_ln_parm.cur_no);  
        set_zd_double("1074",g_pub_tx.tx_amt1);  
        ***   
        set_zd_double("1075",g_pub_tx.tx_amt2);    
        set_zd_double("1076",g_pub_tx.tx_amt3); 
        set_zd_double("1077",g_pub_tx.tx_amt4);    
        ***                        
        strcpy(g_pub_tx.sub_tx_code,"L30B");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_double("1074",0.00);     
        set_zd_double("1075",0.00);    
        set_zd_double("1076",0.00); 
        set_zd_double("1077",0.00);                             
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=0.00;
		g_pub_tx.tx_amt3=0.00;
		g_pub_tx.tx_amt4=0.00;
	    **********转入部分************
        strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ;
        g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
        strcpy( g_pub_tx.ac_get_ind,"011"); *未读取了mdm_ac_rel*
	    strcpy( g_pub_tx.ac_id_type,"3" );	* 账号类型 3-贷款 *
	    strcpy( g_pub_tx.add_ind,"1" );		* 增减:0减1加2销户 *
	    strcpy( g_pub_tx.ct_ind,"2" );		* 现转:1现2转 *
	    strcpy( g_pub_tx.prt_ind,"0" );*是否打印存折(有折无折标志):1是0否 *
	    strcpy( g_pub_tx.hst_ind,"1" );*入明细:1日间入2日终单笔入3日终汇总入*        
        strcpy ( g_pub_tx.intst_type , "1" );	* 计息类型 *	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);    *为日终传票打印赋值ln_ac_no*
	    g_pub_tx.svc_ind=3060;
	    g_pub_tx.tx_amt1=amt1;	   	  	   	
        strcpy ( g_pub_tx.brf,"日终贷款到期转入" );
	    sprintf( flag , "%d" , chg_flag );
	  	strcpy( g_ln_mst.ac_sts,flag );
		*******
		g_ln_mst.ac_sts[0]='2';				*变为转逾期了修改标志*
		*******

	    *** 交易记帐处理 ***
	    if( pub_acct_trance() )
	    {
             sprintf(acErrMsg,"交易记帐处理错误!");
             WRITEMSG
             goto ErrExit;
	    }
	    
		*	if(g_ln_mst.ac_sts[0]=='2')
			{	
				*pub_mob_sendmail_ln("逾期","Z089",g_mdm_ac_rel.ac_no,"1",g_pub_tx.tx_amt1,0.0,0.0);*
		*		pub_mob_sendmail_ln("逾期","Z089",pact_no_tmp,"1",g_pub_tx.tx_amt1,0.0,0.0); /*mod by lwb 20150408 *
		 * }*
	    
	    * 进行会计记帐 *
        set_zd_data("0210",g_ln_parm.cur_no); 
        set_zd_double("107F",g_pub_tx.tx_amt1);                       
        strcpy(g_pub_tx.sub_tx_code,"L302");
        strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
	    g_pub_tx.tx_amt1=0.00;
	    g_pub_tx.tx_amt2=0.00;
	    g_pub_tx.tx_amt3=0.00;
	    g_pub_tx.tx_amt4=0.00;
        set_zd_double("107F",0.00);                       
	}lpj20151124正常转换后不在有逾期的记账，全部为正常或者减值产品*/
vtcp_log("JJJJJ9999999999999999999");	
	if( dc_flag==0 )
	{
	   if( g_ln_parm.auto_cls[0]=='Y' )
	   {
	       g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户id */
	       g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
		   strcpy( g_pub_tx.ac_get_ind,"011"); /*未读取了mdm_ac_rel*/
	       strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	       strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	       strcpy ( g_pub_tx.add_ind , "2" );		/* 增减:0减1加2销户 */	
	       strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	       strcpy ( g_pub_tx.prt_ind , "0" );/*是否打印存折(有无折标志):1是0否*/
	       strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入
																3日终汇总入*/
	       strcpy ( g_pub_tx.brf,"贷款销户" );
		   strcpy( g_pub_tx.beg_note_no, ln_ac_no);    /*为日终传票打印赋值ln_ac_no*/
	       g_pub_tx.svc_ind = 2;				/* 销户 */
	
           /*** 交易记帐处理 ***/
	       if( pub_acct_trance() )
	       {
		    	sprintf(acErrMsg,"交易记帐处理错误!");
		    	WRITEMSG
		    	goto ErrExit;
	       }
           /* 更新贷款主文件 */
           strcpy(g_ln_mst.ac_sts,"*");
           g_ln_mst.lst_date = g_pub_tx.tx_date;               	               
           ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
           if (ret != 0)
           {
                sprintf(acErrMsg,"更新贷款主文件错误!!");
                WRITEMSG
                goto ErrExit;
           }
      }
   }else{
           /* 更新贷款主文件 */
           sprintf( flag , "%d" , chg_flag );
	sprintf(acErrMsg,"更新贷款主文件[%s]!!",flag);
	WRITEMSG           
           strcpy( g_ln_mst.ac_sts,flag );  
           g_ln_mst.lst_date = g_pub_tx.tx_date;              	               
           ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
           if (ret != 0)
           {
                sprintf(acErrMsg,"更新贷款主文件错误!!");
                WRITEMSG
                goto ErrExit;
           }
         }           
vtcp_log("JJJJJaaaaaaaaaaaaaaaaaaaaa");	
		if(g_ln_mst.ac_sts[0]=='2'&&pub_base_CompDblVal(g_ln_mst.bal,0.00)>0)
			{	
				/*pub_mob_sendmail_ln("逾期","Z089",g_mdm_ac_rel.ac_no,"1",g_pub_tx.tx_amt1,0.0,0.0);*/
				pub_mob_sendmail_ln("逾期","Z089",pact_no_tmp,"1",g_pub_tx.tx_amt1,0.0,0.0); /*mod by lwb 20150408 */
		  }
  }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终贷款到期处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* 待修改 */
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终贷款到期处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
