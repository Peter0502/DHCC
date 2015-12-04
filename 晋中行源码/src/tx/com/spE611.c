/*************************************************
* 文 件 名:  spE611.c
* 功能描述： 柜员扎帐
*
* 作    者:  martin
* 完成日期： 2006-10-20
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "com_item_c.h"
#include "com_tel_c.h"
#include "mo_dc_inf_c.h"
#include "dc_log_c.h"
#include "cash_mst_c.h"

int spE611()
{

		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char tmpname[5];

		char titstr[1024];
		char tel[5];
		char ntel[5];
		char old_acc_hrt[6];
		
		int ttlnum=0;			/**读取的总条数**/
		int ttlnum1=0;
		int num=0;	
		int i=0;
		int ret=0;
		
		double	dr_cash_amt=0.00;
		long	dr_cash_cnt=0;
		double	dr_ch_amt=0.00;
		long	dr_ch_cnt=0;
		double	dc_cash_amt=0.00;
		long	dc_cash_cnt=0;
		double	dc_ch_amt=0.00;
		long	dc_ch_cnt=0;
		double	br_no_amt=0.00;
		char amt_tmp[41];
		
		char old_acc_no_lvl1[8],old_acc_no_lvl2[8],old_acc_no_lvl3[8];
									
		FILE *fp;
	
	struct dc_log_c		v_dc_log;
	struct com_item_c	v_com_item;
	struct com_tel_c	v_com_tel;
	struct com_item_c	v_com_item_lvl1;
	struct com_item_c	v_com_item_lvl2;
	struct com_item_c	v_com_item_lvl3;
	struct mo_dc_inf_c	v_mo_dc_inf;
	struct cash_mst_c  v_cash_mst;
				
	memset( &v_dc_log , 0x00 , sizeof(struct trace_log_c));
	memset( &v_com_item , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
	memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
	memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
	memset( &v_cash_mst , 0x00 , sizeof(struct cash_mst_c));
						
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( old_acc_no_lvl1,0,sizeof(old_acc_no_lvl1) );
	memset( old_acc_no_lvl2,0,sizeof(old_acc_no_lvl2) );
	memset( old_acc_no_lvl3,0,sizeof(old_acc_no_lvl3) );
		
	strcpy( old_acc_hrt , "*****" );
		
	pub_base_sysinit();

	get_zd_data("0910",tel );
	get_zd_data("0070",ntel);

	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* 检查柜员号是否存在 */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"此柜员号不存在![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"读取柜员信息表异常出错!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}
		
		sprintf( tmpname,"柜员" );
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}else{
	       sprintf( tmpname,"网点" );
	     }
	
	/* 机构编码 */
	sprintf( tmpstr," br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
	
	/* 币种 */
	sprintf( tmpstr," cur_no='01' and" );
	strcat( wherelist,tmpstr );
	
	/**组成查询**/
	strcat(wherelist," sts='0' ") ;

	vtcp_log( "Cash_mst_Sel  WHERE [%s]",wherelist );
	
	/* 机构余额 */
	ret=Cash_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum1=0;
	
	while(1)
	{
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		ret=Cash_mst_Fet_Sel( &v_cash_mst, g_pub_tx.reply);
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"+++++++++++++++++++++ttlnum1====[%d]",ttlnum1);
		        WRITEMSG
			if( ttlnum1 )
			{
				vtcp_log("OOOOKKKK br_no_amt=[%12.2lf]",br_no_amt);
				break;
			}
		}	else if(ret)
		  {
		  	goto ErrExit;
			}
				/* 统计信息 */
		if( pub_base_CompDblVal( v_cash_mst.bal , 0.00 ) )
		{
					br_no_amt+=v_cash_mst.bal;
		}
	    ttlnum1++;
	}
	ret=Cash_mst_Clo_Sel();
	sprintf(acErrMsg,"Cash_mst_Clo_Sel hello");
  WRITEMSG
	
	memset( wherelist,0,sizeof(wherelist) );
	/**组成查询条件**/
	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* 检查柜员号是否存在 */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"此柜员号不存在![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"读取柜员信息表异常出错!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}
		
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}

	/* 机构编码 */
	sprintf( tmpstr," tx_tx_br_no='%s' and",g_pub_tx.tx_br_no );
/*
	sprintf( tmpstr," tx_opn_br_no='%s' and",g_pub_tx.tx_br_no );
*/
	strcat( wherelist,tmpstr );

	/* 交易日期 */
	sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
	strcat( wherelist,tmpstr );
		
	/**组成查询**/
	strcat(wherelist," sts='0' ORDER BY acc_hrt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = sql_execute( "delete from mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	vtcp_log("ret=[%d]",ret);
        if(ret==-1) ret=0;
	ERR_DEAL	
	
	/* 统计数据过程 */
	ret=Dc_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		memset( &v_dc_log , 0x00 , sizeof(struct trace_log_c)); /* add by LiuHuafeng 20070126 */
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		vtcp_log("%s,%d acc_hrt=[%s],v_dc_log.acc_hrt=[%s]",__FILE__,__LINE__,old_acc_hrt, v_dc_log.acc_hrt);
		ret=Dc_log_Fet_Sel( &v_dc_log, g_pub_tx.reply);
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"+++++++++++++++++++++ttlnum====[%d]",ttlnum);
		        WRITEMSG
			if( ttlnum )
			{
				/* 查出科目基本信息 */
				vtcp_log("%s,%d acc_hrt=[%s],v_dc_log.acc_hrt=[%s]",__FILE__,__LINE__,old_acc_hrt, v_dc_log.acc_hrt);
				ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
				ERR_DEAL
				
				v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* 借方现金金额 */				
				v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* 借方现金笔数 */
				v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* 借方转账金额 */
				v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* 借方转账笔数 */
				v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* 贷方现金金额 */
				v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* 贷方现金笔数 */
				v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* 贷方转账金额 */
				v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* 贷方转账笔数 */
				if( v_com_item.acc_no[0]>'5' )					/* 表内/表外标志 */
				{
					strcpy( v_mo_dc_inf.sts , "1" );
				}else{
					strcpy( v_mo_dc_inf.sts , "0" );
				}
				strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* 柜员编号 */
				strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* 机构编号 */	
				sprintf(acErrMsg,"科目号[%s]级别[%s]!",v_com_item.acc_no,v_com_item.acc_lvl);
				WRITEMSG				
				/* 三级科目 */
				if( v_com_item.acc_lvl[0]=='3' )
				{
					strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* 三级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* 三级科目名称 */
					
					/* 查该科目二级科目信息 */                 
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* 二级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* 二级科目名称 */
					
					/* 查该科目一级科目信息 */
					ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL
					sprintf(acErrMsg,"一级科目[%s]二级上级[%s]!",v_com_item_lvl1.acc_no,v_com_item_lvl2.up_acc_hrt);
					WRITEMSG
											
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* 一级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* 一级科目名称 */												
				
				}else if( v_com_item.acc_lvl[0]=='2' ){		/* 二级科目 */
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* 二级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* 二级科目名称 */					

					/* 查该科目一级科目信息 */
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* 一级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* 一级科目名称 */												
				}else if( v_com_item.acc_lvl[0]=='1' ){		/* 一级科目 */
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* 一级科目号 */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* 一级科目名称 */
				}else{
					sprintf(acErrMsg,"无此级别科目号!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"S056" );			
					goto ErrExit;		
				}								
								
				/* 登记统计表 */

				
				ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
				if( ret )
				{
					sprintf(acErrMsg,"登记轧账科目统计表异常出错,请与中心机房联系!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"G106" );			
					goto ErrExit;					
				}
				
				strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* 科目控制字 */ 	
				vtcp_log("%s,%d acc_hrt=[%s],v_dc_log.acc_hrt=[%s]",__FILE__,__LINE__,old_acc_hrt, v_dc_log.acc_hrt);
				
				/* 数据清零 */
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
				memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));				
			}
			sprintf(acErrMsg,"-----------------------------");
			WRITEMSG
			break;
			
		}else if(ret){
			vtcp_log("%s,%d ret=[%d], to errexit ",__FILE__,__LINE__,ret);
			goto ErrExit;
		}
		
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		/* 若科目控制字为空 */		
		if( strlen(v_dc_log.acc_hrt)==0 )
		{
			strcpy( v_dc_log.acc_hrt , "临" );
		}
		
		/* 第一条记录 */
		if( strcmp( old_acc_hrt , "*****")==0 )
		{
			TRACE
			strcpy( old_acc_hrt, v_dc_log.acc_hrt );
		}		
		
		
		if( strcmp( v_dc_log.acc_hrt , old_acc_hrt ) )	/* 科目控制字变化时 */
		{
		
	sprintf(acErrMsg,"科目控制字变化时");
	WRITEMSG
			/* 查出科目基本信息 */
			vtcp_log("before inf old_acc_hrt[%s]\n", old_acc_hrt);
			ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
			ERR_DEAL
						
			v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* 借方现金金额 */				
			v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* 借方现金笔数 */
			v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* 借方转账金额 */
			v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* 借方转账笔数 */
			v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* 贷方现金金额 */
			v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* 贷方现金笔数 */
			v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* 贷方转账金额 */
			v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* 贷方转账笔数 */
			if( v_com_item.acc_no[0]>'5' )					/* 表内/表外标志 */
			{
				strcpy( v_mo_dc_inf.sts , "1" );
			}else{
				strcpy( v_mo_dc_inf.sts , "0" );
			}
			strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* 柜员编号 */
			strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* 机构编号 */	
			
			/* 三级科目 */
			if( v_com_item.acc_lvl[0]=='3' )
			{
				strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* 三级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* 三级科目名称 */
				
				/* 查该科目二级科目信息 */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* 二级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* 二级科目名称 */
				
				/* 查该科目一级科目信息 */
				ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* 一级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* 一级科目名称 */												
			
			}else if( v_com_item.acc_lvl[0]=='2' ){		/* 二级科目 */
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* 二级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* 二级科目名称 */					

				/* 查该科目一级科目信息 */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* 一级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* 一级科目名称 */												
			}else if( v_com_item.acc_lvl[0]=='1' ){		/* 一级科目 */
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* 一级科目号 */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* 一级科目名称 */
			}else{
				sprintf(acErrMsg,"无此级别科目号!lvl[%s]acc_hrt[%s]",v_com_item.acc_lvl,old_acc_hrt);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"S056" );			
				goto ErrExit;		
			}								
							
			/* 登记统计表 */

			ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
			if( ret )
			{
				sprintf(acErrMsg,"登记轧账科目统计表异常出错,请与中心机房联系!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"G106" );			
				goto ErrExit;					
			}
			
			strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* 科目控制字 */ 	
			
			/* 数据清零 */
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
			memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));						
		}
		
		
		/* 统计信息 */
		if( pub_base_CompDblVal( v_dc_log.amt , 0.00 ) )
		{
			if( v_dc_log.ct_ind[0]=='1' )		/* 现金 */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* 借方 */
				{
					dr_cash_amt+=v_dc_log.amt;
					dr_cash_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* 贷方 */
				{
					dc_cash_amt+=v_dc_log.amt;
					dc_cash_cnt++;
				}
			}
			if( v_dc_log.ct_ind[0]=='2' )		/* 转账 */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* 借方 */
				{
					dr_ch_amt+=v_dc_log.amt;
					dr_ch_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* 贷方 */
				{
					dc_ch_amt+=v_dc_log.amt;
					dc_ch_cnt++;
				}
			}			
		}	
		
		ttlnum++;
	}

	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG
		sprintf(acErrMsg,"hello");
WRITEMSG
	ret=Dc_log_Clo_Sel();
	sprintf(acErrMsg,"hello");
WRITEMSG
	ERR_DEAL
	sprintf(acErrMsg,"hello");
WRITEMSG
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
/***JYW		fclose(fp); **/
		set_zd_data( DC_FILE_SND,"1" );
	}
		
	
	/** 写显示文件 **/
	
	ret=Mo_dc_inf_Dec_Sel( g_pub_tx.reply,"br_no='%s' and tel='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel );
	ERR_DEAL

		num=0;
	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG	
	while(1)
	{
		ret=Mo_dc_inf_Fet_Sel( &v_mo_dc_inf, g_pub_tx.reply );
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"<><><><><><><><><><><>");
			WRITEMSG
			if( num )
			{
				/* 分别写表内/表外科目统计信息 */
				/* 表内信息 */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
																
				fprintf( fp,"             表内汇总  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  \n",dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);  
				
				/* 数据清零 */				
				
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				/* 表外信息 */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL

				fprintf( fp,"             表外汇总  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  \n",dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"\n" );
				
				/* 数据清零 */			
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;							
			
			}
			
			break;
			
		}else if(ret){
			goto ErrExit;
		}						
		if( !num )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log("filename=[%s]",filename);
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			sprintf( amt_tmp, "%.2lf" , br_no_amt );
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			fprintf( fp,"             \n" );
			fprintf( fp,"              日期:[%ld] 时间:[%ld]        %s轧帐    %s余额:[%s]  操作员:[%s] \n",g_pub_tx.tx_date,g_pub_tx.tx_time,tmpname,tmpname,amt_tmp,ntel );
			fprintf( fp,"                                                                                                \n" );
			fprintf( fp,"            ==========================================机构号码:[%s]================================\n",g_pub_tx.tx_br_no );
			fprintf( fp,"                                     现          金                             转          帐        \n" );
			fprintf( fp,"            =================================================   ======================================\n" );
      fprintf( fp,"                科目           借方  笔数          贷方  笔数          借方  笔数          贷方  笔数 \n" );
      fprintf( fp,"            --------- ------------- ----- ------------- ----- ------------- ----- ------------- ----- \n" );
          
		}
		
		
		/* 统计一级科目信息 */
		if( strcmp(  v_mo_dc_inf.acc_no_lvl1 , old_acc_no_lvl1 ) && strlen(v_mo_dc_inf.acc_no_lvl1)!=0 )
		{
			ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
			
			ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
															
			
			fprintf( fp,"              %7s  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  \n" , v_mo_dc_inf.acc_no_lvl1,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
			
			/* 数据清零 */		
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			
			strcpy( old_acc_no_lvl1 , v_mo_dc_inf.acc_no_lvl1 );
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
		}else{
			continue;
		}
		
		/* 查询二级科目信息 */
		ret=Mo_dc_inf_Dec_Sel1( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1 );
		ERR_DEAL				
		while(1)
		{
			ret=Mo_dc_inf_Fet_Sel1( &v_mo_dc_inf, g_pub_tx.reply );
			if( ret==100 ) 
			{
				break;			
			}else if(ret){
				goto ErrExit;
			}
			/* 打印二级科目信息 */
			if( strcmp(  v_mo_dc_inf.acc_no_lvl2 , old_acc_no_lvl2 ) && strlen(v_mo_dc_inf.acc_no_lvl2)!=0 )
			{
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
																
				
				fprintf( fp,"              %7s  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  \n" , v_mo_dc_inf.acc_no_lvl2,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt );
				/* 数据清零 */		
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				strcpy( old_acc_no_lvl2 , v_mo_dc_inf.acc_no_lvl2 );
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
			}else{
				continue;
			}
			
			/* 查询三级科目信息 */
			ret=Mo_dc_inf_Dec_Sel2( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' and acc_no_lvl2='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1,old_acc_no_lvl2 );
			ERR_DEAL				
			while(1)
			{
				ret=Mo_dc_inf_Fet_Sel2( &v_mo_dc_inf, g_pub_tx.reply );
				if( ret==100 ) 
				{
					break;			
				}else if(ret){
					goto ErrExit;
				}
				/* 打印三级科目信息 */
				if( strcmp(  v_mo_dc_inf.acc_no_lvl3 , old_acc_no_lvl3 ) && strlen(v_mo_dc_inf.acc_no_lvl3)!=0)
				{
					ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
					
					ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
					
					fprintf( fp,"              %7s  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  %12.2lf  %4d  \n" , v_mo_dc_inf.acc_no_lvl3,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt );
					/* 数据清零 */		
					dr_cash_amt=0.00;
					dr_cash_cnt=0;
					dr_ch_amt=0.00;
					dr_ch_cnt=0;
					dc_cash_amt=0.00;
					dc_cash_cnt=0;
					dc_ch_amt=0.00;
					dc_ch_cnt=0;
					
					strcpy( old_acc_no_lvl3 , v_mo_dc_inf.acc_no_lvl3 );
					memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
				}else{
					continue;
				}			
			num++;
			}
			ret=Mo_dc_inf_Clo_Sel2( );
			ERR_DEAL
		num++;
		}					
		ret=Mo_dc_inf_Clo_Sel1( );
		ERR_DEAL
	num++;
	}
	ret=Mo_dc_inf_Clo_Sel( );
	ERR_DEAL
		
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
	

GoodExit:
	
	ret = sql_execute( "delete from  mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	ERR_DEAL
	
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
