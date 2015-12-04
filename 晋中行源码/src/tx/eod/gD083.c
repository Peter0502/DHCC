/*************************************************************
* 文 件 名: gD083.c
* 功能描述：日终－贷款结息
*
* 作    者: lance
* 完成日期: 2003年3月31日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if(ret) {\
		sprintf(acErrMsg,"SQLERROR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "com_sys_parm_c.h"
#include "cif_basic_inf_c.h"

gD083()
{
	int    ac_seqn;
	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
	long   time_year=0,time_month=0,time_day=0,time_flag=0;
	int	month_tmp=0;
	int	day_tmp=0;
	long ly_cnt=0;
	char ln_ac_no[20],dd_ac_no[20],flag[2];  
	char time_list[201];
	double vbnlx=0,vbwlx=0,vfllx=0,vbjlx=0;
	double amt1=0,amt2=0,amt3=0,amt4=0;
	double tx_amt1=0,tx_amt2=0,tx_amt3=0,tx_amt4=0;	
	struct S_ln_intst vln_intst[LN_INTST_TYPS];	
	struct com_holiday_c com_holiday;
	struct ln_mst_c ln_mst_tmp;	
	struct ln_parm_c ln_parm;
	struct ln_mst_c ln_mst;
	struct cif_basic_inf_c S_cif_basic_inf;
    double bj_acm=0.00;
	double bn_acm=0.00;
	double bw_acm=0.00;
	double fl_acm=0.00;
	double bj_sect_acm=0.00;
	double bn_sect_acm=0.00;
	double bw_sect_acm=0.00;
	double fl_sect_acm=0.00;
	struct com_sys_parm_c s_com_sys_parm;
	
   	char acm_type[2];
	int    coninue_flag=0;
	char type[2];			  
	amt1=amt2=amt3=amt4=0.00;	
	vbnlx=vbwlx=vfllx=vbjlx=0.00;	
	memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
	memset(&vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst));
	memset(&ln_parm, 0x00, sizeof(struct ln_parm_c));
	memset(&ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset(&S_cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	set_zd_double("0400",0.00);
	set_zd_double("0410",0.00);
	set_zd_double("1089",0.00);
	set_zd_double("108F",0.00);
	set_zd_double("108H",0.00);
	set_zd_double("108I",0.00);
	set_zd_double("102F",0.00);
		
	sprintf(acErrMsg,"贷款结息开始DKJX");
	WRITEMSG

	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

   	/* 初始化全局变量 */
   	pub_base_sysinit();

	/**xxx****3line********/
	ret = Com_sys_parm_Sel( g_pub_tx.reply,&s_com_sys_parm, "1=1" );
	if( ret ) goto ErrExit;
	g_pub_tx.tx_date=s_com_sys_parm.sys_date;

	strcpy(g_pub_tx.sub_tx_code,"G083");
	strcpy( g_pub_tx.tx_code, "G083");
	/*日期条件*/
	month_tmp=g_pub_tx.tx_date%10000/100;
	day_tmp=g_pub_tx.tx_date%100;
	
	/**郭强要求银行承兑汇票逾期贷款不计息 delete by jk 20120213 begin
	strcpy( time_list, " intst_type not in('0','1','6') and repay_type not in ('3','4') and prdt_no !='3Y1' and "); 
	delete by jk 20120213 end**/
	/**郭强要求银行承兑汇票逾期贷款不计息 add by jk 20120213 begin**/
	/**加了3Y2，3Y3。。。等多个委托贷款产品 20150525 wudawei**/
	strcpy( time_list, " intst_type not in('0','1','6') and repay_type not in ('3','4') and prdt_no not in('3Y1','3Y2','3Y3','3Y4','3Y5','3Y6','3Y7','3Y8','3Y9','3YA','3YB','3Z1','3Z2','3Z3') and ");
	/**郭强要求银行承兑汇票逾期贷款不计息 add by jk 20120213 end**/
	/* 判断是月末、季末或者年末 */
	/* time_flag:0:普通1:旬初2:旬末 3:月初4:月末5:季初6:季末7:半年初
												8:半年末 9:年初10:年末 */	
	/****
	ret = pub_base_get_ymd(&time_year , &time_month , &time_day , 
											g_pub_tx.tx_date , &time_flag);
	if( ret )    goto ErrExit;
	
	if( time_flag==0 )	*普通日期*
	{
		strcat( time_list, " intst_type='2' and " );
	}	
	if( time_flag==4 )* 月末 *
	{		
		strcat( time_list," intst_type='3' and ");
	}
	if( time_flag==6 )* 季末 *
	{
		strcat( time_list," intst_type='4' and ");
	}		
	if( time_flag==10 )* 年末 *
	{
		strcat( time_list," intst_type='5' and ");
	}
	*****/
	if( day_tmp==21 )
	{
		strcat( time_list, " (intst_type='3' or " );
		if( month_tmp==3|| month_tmp==6|| month_tmp==9|| month_tmp==12 )
		{
			strcat( time_list," intst_type='4' or ");
		}
		if( month_tmp==12 )
		{
			strcat( time_list," intst_type='5' or ");
		}
		strcat ( time_list , " intst_type='2' ) and " );
	}else{
		strcat ( time_list , " intst_type='2' and " );
	}
		strcat(time_list," ac_sts in('1','2','3','4','5') and 1=1");		
	sprintf(acErrMsg,"条件语句 [%s]",time_list);
	WRITEMSG
	
	ret = Eod_ln_mst_Dec_Upd(g_pub_tx.reply,time_list);
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Eod_ln_mst_Fet_Upd(&g_ln_mst, g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			break;
		}
		
		/* 取流水号 xxx 7 lines*/
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"Get trace_no error [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );

		ly_cnt++;
		/***
		if( ly_cnt>100 )
		{
			break;
		}
		***/
		vtcp_log(" DO WITH id='%ld' seqn='%d'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		TRACE
		sprintf(acErrMsg,"lnly第【【【【 %ld 】】】】户",ly_cnt);
		WRITEMSG		
		
   		/* 取贷款参数信息 */
		ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm , " prdt_no='%s' " ,
				        g_ln_mst.prdt_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"贷款参数文件中无[%s]产品",ln_mst_tmp.prdt_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L001" );
			goto ErrExit;
		}else if( ret ){
   			sprintf(acErrMsg,"取贷款参数文件异常 [%s][%d]",ln_mst_tmp.prdt_no,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L001" );
   			goto ErrExit;
   		}
		TRACE
		/* 取介质信息 */
		ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld and ac_seqn=%d ",
															g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
		strcpy( ln_ac_no, g_mdm_ac_rel.ac_no);
		
		/*交易机构赋值*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);
		sprintf(acErrMsg,"~~~~初始时in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           	WRITEMSG
	   	sprintf(acErrMsg,"~~~~初始时out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           	WRITEMSG
	   	sprintf(acErrMsg,"~~~~初始时cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           	WRITEMSG
   	sprintf(acErrMsg,"~~~~初sssssscmpd_lo_intst[%s]", g_ln_mst.intst_type);
           	WRITEMSG
		/* 算息过程 */
		if( g_ln_mst.intst_type[0]=='0' || g_ln_parm.intst_type[0]=='0' )	/*不计息*/
		{
			vbjlx=0.00; bj_acm=0.00;
			vbnlx=0.00; bn_acm=0.00;
			vbwlx=0.00; bw_acm=0.00;
			vfllx=0.00; fl_acm=0.00;
		}
		else
		{  	sprintf(acErrMsg,"~~~~fffffff[%s]", g_ln_mst.intst_type);
           	WRITEMSG
			ret = pub_ln_CalIntst( g_ln_mst,g_ln_parm,"0",&vbjlx,0 );
			if( ret ) goto ErrExit;
			
			/* 积数计算类型（1每日累计2变动累计3变动累计倒算）*/
			strcpy( acm_type , "2" ); 
			bj_acm = g_ln_mst.intst_acm ;/* 利息积数 */
        	sprintf(acErrMsg,"~~~~mmmmmmm[%s]", g_ln_mst.intst_type);
           	WRITEMSG
			ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
								g_ln_parm.intst_days_type , acm_type , 
								g_ln_mst.bal ,g_ln_mst.bal , &bj_acm , "0",0,0);
			if( ret )
			{
				sprintf(acErrMsg,"调用函数计算积数错误!");
				WRITEMSG
				goto ErrExit;
			}
			  	sprintf(acErrMsg,"~~~~初fffffffcmpd_lo_intst[%s]", g_ln_mst.intst_type);
           	WRITEMSG
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
	
		/* 判断本金利息 */
		bj_flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );/* 1表内  2表外 */
		TRACE
		
		/*写贷款登记簿  0登记欠款 1回收已登记欠款 2登记并回收*/
		/*********
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,"4",vbnlx+vbnlx+vfllx,"0",0);
		if( ret ) goto ErrExit;
		*******/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbnlx,bn_acm+bn_sect_acm,"1",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vbwlx,bw_acm+bw_sect_acm,"1",0);
		if( ret ) goto ErrExit;
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",vfllx,fl_acm+fl_sect_acm,"1",0);
		if( ret ) goto ErrExit;				
		if( bj_flag==1 )
		{
	  		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",vbjlx,bj_acm+bj_sect_acm,"1",0);
	  		if(ret) goto ErrExit;
		}else{
	  		ret=pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",vbjlx,bj_acm+bj_sect_acm,"1",0);
	  		if(ret) goto ErrExit;
		}
		TRACE
      	/*****************add by ly 分列的各种积数结息 **************/
  		/*判断本金积数入表内还是表外*/
  		if(bj_flag==1)
  		{
   			g_pub_tx.tx_amt2=vbjlx;
   		}else{
   			g_pub_tx.tx_amt3=vbjlx;
   		}
	   	g_pub_tx.tx_amt4=vbnlx+vbwlx+vfllx;	
	   		
		/* 对结出来的利息记账--表内利息 */
		if( pub_base_CompDblVal( g_pub_tx.tx_amt2 ,0.00 ) )
		{
			/** 记流水和记账(结出的利息入相应的科目) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/***不读取相关表20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");	
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
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
			/***不读取相关表20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");
			
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0400",0.00); 
			
/**JYW add end***/
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
		g_pub_tx.tx_amt4=0.00;
		if( pub_base_CompDblVal( g_pub_tx.tx_amt4 ,0.00 ) )
		{
			/** 记流水和记账(结出的利息入相应的科目) **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/***不读取相关表20050918******/
			strcpy( g_pub_tx.ac_get_ind,"111");
			
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
/**JYW add***/
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           	set_zd_double("0400",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
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
	ret = sql_execute( "UPDATE ln_mst SET intst_acm=0.00,in_lo_acm=0.00,out_lo_acm=0.00,cmpd_lo_acm=0.00  \
								WHERE ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
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
	sprintf(acErrMsg,"~~~~qianin_lo_intst[%lf]",g_ln_mst.in_lo_intst);
   	WRITEMSG
   	sprintf(acErrMsg,"~~~~qianout_lo_intst[%lf]",g_ln_mst.out_lo_intst);
   	WRITEMSG
   	sprintf(acErrMsg,"~~~~qiancmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
   	WRITEMSG
	/*重取贷款主文件*/
	/*memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c)); JYW*/
	ret=Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	if(ret)
	{
		goto ErrExit;
	}
	/* 若存在负数,不处理 */
	if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 ||pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0
			||pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 ||pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)<0 )
	{
       		sprintf(acErrMsg,"存在负数!!!!!!!!!!!!!!!!!!");
       		WRITEMSG			
			TRACE
			continue;
	}
		 
    /** 还息过程 **/
	/* 若自动还款 */
	coninue_flag=0;
sprintf(acErrMsg,"~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
WRITEMSG
sprintf(acErrMsg,"~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
WRITEMSG
sprintf(acErrMsg,"~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
WRITEMSG
	memset( vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst) );

	if ( g_ln_parm.auto_pay_ind[0]=='Y')
	{
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
				/****
				goto ErrExit;
				*****/
				coninue_flag=1;
	   		}else{
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
			}
			
			/** 对公不处理 ***
			if( g_ln_mst.cif_no==0 )
			{
				coninue_flag++;
			}else{
				ret= Cif_basic_inf_Sel(g_pub_tx.reply,&S_cif_basic_inf,"cif_no=%ld",g_ln_mst.cif_no);
				if(ret)
				{
					sprintf(acErrMsg,"客户号不存在!![%ld]",g_ln_mst.cif_no);
					WRITEMSG
					coninue_flag++;
				}else{
					if( S_cif_basic_inf.type[0]!='1' && S_cif_basic_inf.type[0]!='5')
					{
						coninue_flag++;
					}
				}
			}
		长治对公自动扣******************/
			if( coninue_flag )
			{
				sprintf(acErrMsg,"存款账户状态不正确![%d]",coninue_flag);
				WRITEMSG
				g_pub_tx.tmp_amt1=0.00;
			}				
			TRACE			
			
			vbjlx=0.00;
			vbnlx=0.00;
			vbwlx=0.00;
			vfllx=0.00;	
			/* 取贷款信息 *
			ret = pub_ln_lndis_file(ln_ac_no,0,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); 
			if( ret ) { goto ErrExit; }
			***/

			TRACE
							
			
		sprintf(acErrMsg,"~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
		WRITEMSG
		sprintf(acErrMsg,"~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
		WRITEMSG
		sprintf(acErrMsg,"~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
		WRITEMSG

			memset( vln_intst,0x00,LN_INTST_TYPS*sizeof(struct S_ln_intst) );
			/*确定各项欠息的还款金额  0本金、1本金利息、2表内欠息、3表内复利、
									4表外欠息、5表外复利、6复利欠息、7复利复利*/
			i=1;
			vln_intst[i].intst_amt=g_ln_mst.in_lo_intst;   vln_intst[i].type=2;
			i++;
			vln_intst[i].intst_amt=g_ln_mst.out_lo_intst;  vln_intst[i].type=4;
			i++;	  
			/***被GXJ改
			vln_intst[i].intst_amt=g_ln_mst.cmpd_lo_intst; vln_intst[i].type=6;
			****/
			vln_intst[i].intst_amt=0; vln_intst[i].type=6;
			i++;

	sprintf(acErrMsg,"JJJJ~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
 WRITEMSG
  sprintf(acErrMsg,"JJJJ~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
   WRITEMSG
	sprintf(acErrMsg,"JJJJ~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
	WRITEMSG

			/* 根据还款顺序表顺序还款 */	  
			ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
														&g_pub_tx.tmp_amt1 );
			if( ret ) goto ErrExit;										
		}	
		sprintf(acErrMsg,"KKKK~~~~vln_intst[1].pay_amt[%lf]",vln_intst[1].pay_amt);
		 WRITEMSG
		  sprintf(acErrMsg,"KKKK~~~~vln_intst[2].pay_amt[%lf]",vln_intst[2].pay_amt);
		   WRITEMSG
			sprintf(acErrMsg,"KKKK~~~~vln_intst[3].pay_amt[%lf]",vln_intst[3].pay_amt);
			WRITEMSG


		/* 计算可归还金额和需挂账金额 */
		dc_flag=0;
		tx_amt1=tx_amt2=tx_amt3=tx_amt4=0.00;
		amt1=amt2=amt3=amt4=0.00;
		TRACE
		/* 表内欠息 */
		tx_amt2= vln_intst[1].pay_amt; /*记账金额*/
		if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,vln_intst[1].pay_amt) )
		{
	  		amt2= g_ln_mst.in_lo_intst-vln_intst[1].pay_amt; /*挂账金额*/
	  		dc_flag++;
		}
		/* 表外欠息 */
		tx_amt3= vln_intst[2].pay_amt; /*记账金额*/
		if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,vln_intst[2].pay_amt) )
		{
	  		amt3= g_ln_mst.out_lo_intst-vln_intst[2].pay_amt; /*挂账金额*/
	  		dc_flag++;
		}		
		/* 复利欠息 */
		tx_amt4= vln_intst[3].pay_amt; /*记账金额*/
		if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,vln_intst[3].pay_amt) )
		{
	  		amt4= g_ln_mst.cmpd_lo_intst-vln_intst[3].pay_amt; /*挂账金额*/
	 		dc_flag++;
		}
		sprintf(acErrMsg,"~~~~tx_amt4[%lf]",tx_amt4);
           	WRITEMSG	
		TRACE
	   		sprintf(acErrMsg,"~~~~2in_lo_intst[%lf]",g_ln_mst.in_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~2out_lo_intst[%lf]",g_ln_mst.out_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~2cmpd_lo_intst[%lf]",g_ln_mst.cmpd_lo_intst);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt2[%lf]",tx_amt2);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt3[%lf]",tx_amt3);
           		WRITEMSG
	   		sprintf(acErrMsg,"~~~~tx_amt4[%lf]",tx_amt4);
           		WRITEMSG

		/*写贷款登记簿  0登记欠款 1回收已登记欠款 2登记并回收*/
		/* 注：回收时，日期无作用 */
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",tx_amt2,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"2 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt2);
			WRITEMSG
			goto ErrExit;
		}
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",tx_amt3,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"3 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt3);
			WRITEMSG
			goto ErrExit;
		}
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",tx_amt4,0.00,"1",1 );
		if( ret ) 
		{
			sprintf(acErrMsg,"4 ac_id[%d-%d],[%.2lf]",
				g_ln_mst.ac_id,g_ln_mst.ac_seqn,tx_amt4);
			WRITEMSG
			goto ErrExit;
		}
	
	TRACE
		if( pub_base_CompDblVal( tx_amt2+tx_amt3+tx_amt4 ,0.00 ) )
		{
			/** 记流水和记账(可还利息部分) **/
        	/** 贷款部分 **/
			strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
			g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
			/****
			strcpy( g_pub_tx.ac_get_ind,"10");	*已经读取了mdm_ac_rel*
			*******/
			strcpy( g_pub_tx.ac_get_ind,"011");	/*未读取了mdm_ac_rel*/
			
			strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
	   		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/  
			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*为日终传票打印赋值ln_ac_no*/
          	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	  		g_pub_tx.svc_ind=3050;
	   		g_pub_tx.tx_amt2=tx_amt2;
	   		g_pub_tx.tx_amt3=tx_amt3;
	   		g_pub_tx.tx_amt4=tx_amt4;
	   		
           	strcpy ( g_pub_tx.brf,"" );
			
	   		/*** 交易记帐处理 ***/
	   		if( pub_acct_trance() )
	   		{
           			sprintf(acErrMsg,"交易记帐处理错误!");
           			WRITEMSG
           			goto ErrExit;
	   		}
	   		
	   		
	   		
				pub_mob_sendmail_ln("贷款结息",g_pub_tx.tx_code,g_ln_mst.prdt_no,"0",tx_amt2 + tx_amt3 + tx_amt4,tx_amt1,0.0);
				
	   		
	   		
	   		/*** 会计记帐 ***/
/**JYW add***/
           	set_zd_double("0400",0.00); 
           	set_zd_double("0410",0.00); 
			
/**JYW add end***/
           	set_zd_data("0210",g_ln_parm.cur_no); 
           	set_zd_double("108F",g_pub_tx.tx_amt2);    
			vtcp_log("108H监控[%.02lf][%.02lf][%.02lf]",g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
           	set_zd_double("108H",g_pub_tx.tx_amt3); 
           	set_zd_double("108I",g_pub_tx.tx_amt4);                                     
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
           	set_zd_double("108F",0.00);    
           	set_zd_double("108H",0.00); 
           	set_zd_double("108I",0.00); 
           		   
	   	/**********取款部分************/
           	strcpy( g_pub_tx.ac_no , dd_ac_no);
		g_pub_tx.ac_id = g_dd_mst.ac_id;
           	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
           	strcpy( g_pub_tx.ac_get_ind,"011");	/*未读取了mdm_ac_rel*/
	   		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
	   		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
	   		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
	   		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否*/
	   		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入
																3日终汇总入*/ 
           	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
			g_pub_tx.ac_wrk_ind[9]='1';     /*对于非结算户强行扣款*/
			g_pub_tx.ac_wrk_ind[8]='1';         /*日终记账标志 */

			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*为日终传票打印赋值ln_ac_no*/
	   		g_pub_tx.svc_ind=1001;
	   		g_pub_tx.tx_amt1=tx_amt2+tx_amt3+tx_amt4;		   
           	strcpy ( g_pub_tx.brf,"贷款还息" );
	
	   		/*** 交易记帐处理 ***/
	  		if( pub_acct_trance() )
	   		{
               		sprintf(acErrMsg,"交易记帐处理错误!");
               		WRITEMSG
               		goto ErrExit;
	   		}
	   		/*** 进行会计记帐 ***/
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
           	vtcp_log("aaaaaaaaaaa[%lf]\n",g_pub_tx.tx_amt1);	
           	/* 委托协议贷款(还息) */
           	ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "3" );
           	if( ret )	goto ErrExit;
	          		
        }
		TRACE
	   	g_pub_tx.tx_amt1=0.00;
	   	g_pub_tx.tx_amt2=0.00;
	   	g_pub_tx.tx_amt3=0.00;	   
	   	g_pub_tx.tx_amt4=0.00;
        set_zd_double("102F",0.00);	
            /* 更新贷款主文件 */ 
            g_ln_mst.lst_date = g_pub_tx.tx_date;    
            g_ln_mst.ic_date =  g_pub_tx.tx_date;                                        	               
            ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
            if (ret)
            {
                 sprintf(acErrMsg,"更新贷款主文件错误!!");
                 WRITEMSG
                 goto ErrExit;
            }         
    }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"贷款结息处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"AT12");/* 待修改 */
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"贷款结息处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
