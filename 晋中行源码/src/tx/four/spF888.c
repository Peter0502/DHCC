/*************************************************************
* 文 件 名: spF888.c
* 功能描述：六市业务对帐(包括ATM,POS,六市通兑业务)
* 作    者: jack
* 完成日期：2003年8月18日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

#define MAX 10000
#include "four_tradelog_c.h"
#include "chnl_self_trade_c.h"
#include "six_dz_log_c.h"

long lDate;
char cFlag[2];
char cMessage[81];

char   shmx[MAX][81], ylmx[MAX][81];
char   hdbz[MAX] ;
char   sh_corr[MAX][81];

spF888()
{
	int ret = 0;
	char buf[128];
	char filename[128];
	
	ret = dz_process();
	
	set_zd_data( "0760", cMessage );
	set_zd_data( "0700", cFlag );
	set_zd_long( "0440", lDate );

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "%ld.txt", lDate );

	memset( buf, 0x00, sizeof(buf) );
	sprintf( buf, "%s/bin/ftp_to_tfiof.sh %s %s", getenv("HOME"), filename );
	system( buf );

	if ( ret )
	{
		goto ErrExit;
	}


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"六市业务对帐交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"六市业务对帐交易出错[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int dz_process()
{
	FILE    *fr, *fw;
	int     flag=0,i=0,j=0,n=0, len=0 ; /* 0:t_lc_trans_hist   1:t_lc_transflow */
	int     num_bd=0, num_yl=0, num_sh=0, num_corr=0, num_bs=0; /* 本地笔数，银联笔数 */
	char    lsbuf[51], ret_code[3];
	
	char   buf[1024],  filename[61], jdbz[2];
	char   trn_date[9], date[9], srsn[5], otsn[5], seq_dl[11];
	char   seq_kh[11], seq_zx[11], czbz[3], oper[9];
	char   trcd[5], amcr[20], idno[21],wsno[11], time[9], trn_flag[2];
	double tram , zje_sxf ,zje_sh, zje_yd, zje_yl;
	double  sxfje, tzje;
	int     dl_kh ; /* 0-其他行 1-代理行 2-开户行 */
	int     bdt_bs_sh, tdb_bs_sh ;
	double  bdt_jffs_sh, bdt_dffs_sh, tdb_jffs_sh, tdb_dffs_sh ;
	int     bdt_bs_zx, tdb_bs_zx ;
	double  bdt_jffs_zx, bdt_dffs_zx, tdb_jffs_zx, tdb_dffs_zx ;
	int     bdt_bs_hj, tdb_bs_hj ;
	double  bdt_jffs_hj, bdt_dffs_hj, tdb_jffs_hj, tdb_dffs_hj , qsje_hj;
	
	char fldstr[35][128];	/*存放循环拆出对帐记录*/
	int  ret = 0;
	char tmpstr[128];

	char filename1[128];
	char filename2[128];
	
	struct four_tradelog_c sFour_tradelog;
	struct chnl_self_trade_c sChnl_self_trade;
	struct six_dz_log_c sSix_dz_log;
	
	memset( cFlag, 0x0, sizeof(cFlag) );

	get_zd_long( "0440", &lDate );

	sprintf( acErrMsg, "－－－－－－－－>对帐日期为[%ld]", lDate );
	WRITEMSG
	
	ret = sql_execute( "delete from six_dz_log where tx_date = %ld", lDate );
	
	/*--------------判断是否已经对过帐---------------*/
	sprintf( trn_date, "%ld", lDate );
	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/%s.txt", trn_date );
	fw = fopen( filename, "r" );	
	if(fw != NULL)
	{
		fclose(fw);
		sprintf( cMessage, "今天已经对帐![%s]", trn_date );
		sprintf( acErrMsg, "今天已经对帐![%s]", trn_date );
		WRITEMSG
		strcpy( cFlag, "0" );
		return(0);
	}	
	
	/*-------取对帐文件,并判断取对帐文件是否成功----------*/
	memset( filename1, 0x0, sizeof(filename1) );
	memset( filename2, 0x0, sizeof(filename2) );
	sprintf( filename1, "ZXLS_4302_%s.txt", trn_date );
	sprintf( filename2, "HZQC_4302_%s.txt", trn_date );
	
	memset( buf, 0x00, sizeof(buf) );
	sprintf( buf, "%s/bin/ftp_lc_dz.sh %s %s %s", getenv("HOME"), filename1, filename2, trn_date );
	system( buf );

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/HZQC_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if(fr==NULL)
	{
		sprintf( cMessage, "该日没有生成对帐文件 %s !", filename );
		sprintf( acErrMsg, "该日没有生成对帐文件[%s]", filename );
		WRITEMSG
		goto ErrExit;
	}
	fclose(fr);	

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/ZXLS_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "该日没有生成对帐文件 %s !", filename );
		sprintf( acErrMsg, "该日没有生成对帐文件 %s !", filename );
		WRITEMSG
		goto ErrExit;
	}
	fclose(fr);	
	
	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "/backup/data/%s.txt", trn_date );
	fw = fopen( filename, "w" );
	fprintf( fw, "交易日期: %s\n", trn_date );	
		
	tdb_bs_sh = bdt_bs_sh = 0 ;
	tdb_jffs_sh = tdb_dffs_sh = 0.0 ;
	bdt_jffs_sh = bdt_dffs_sh = 0.0 ;
	memset( jdbz, 0x0, sizeof(jdbz) );	
	
	/*--------------柜面六市异地存取款交易--------------*/
	ret = Four_tradelog_Dec_Sel( g_pub_tx.reply , "tx_date = %ld and tx_stat = '0' \
		and tx_code in ('7701', '7702', '7801', '7802', '7711', '7713') order by trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "执行Four_tradelog_Dec_Sel错[%s]!!", g_pub_tx.reply );
		sprintf( acErrMsg, "执行Four_tradelog_Dec_Sel错[%s]!!", g_pub_tx.reply );
		WRITEMSG
		return 1;
	}
		
	while(1)
	{
		memset( &sFour_tradelog, 0x0, sizeof(sFour_tradelog) );
		ret = Four_tradelog_Fet_Sel( &sFour_tradelog, g_pub_tx.reply ); 
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "执行Four_tradelog_Fet_Sel错[%s]!!", g_pub_tx.reply );
			sprintf( acErrMsg, "执行Four_tradelog_Fet_Sel错[%s]!!", g_pub_tx.reply );
			WRITEMSG
			return 1;
		}
		
		pub_base_strpack( sFour_tradelog.tx_code );
		
	 	if( sFour_tradelog.tx_code[1] == '7' )	/* 本代他 */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sFour_tradelog.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sFour_tradelog.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4302" );
	 		strcpy( sSix_dz_log.mb_bank, "4301" );
	 		sSix_dz_log.tx_amt = sFour_tradelog.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sFour_tradelog.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sFour_tradelog.chang_no );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );	 		
	 		
	 		if ( strcmp( sFour_tradelog.tx_code, "7711" ) && strcmp( sFour_tradelog.tx_code, "7713" ) )
	 		{	/* 7711 隔日冲开户行帐户  7713 隔日补开户行帐户 */
	 			bdt_bs_sh++ ;
	 	  	
	 	  		if ( memcmp( sFour_tradelog.tx_code, "7701", 4 ) == 0 )
	 	    			bdt_jffs_sh += sFour_tradelog.tx_amt ;
	 	  		else
	 	  			bdt_dffs_sh+= sFour_tradelog.tx_amt;
	 		}
	 	}
	 	else	/* 他代本 */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sFour_tradelog.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sFour_tradelog.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4301" );
	 		strcpy( sSix_dz_log.mb_bank, "4302" );
	 		sSix_dz_log.tx_amt = sFour_tradelog.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sFour_tradelog.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sFour_tradelog.chang_no );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 		
	 		tdb_bs_sh++;
	 		
	 		if( memcmp( sFour_tradelog.tx_code, "7801", 4) ==0 )
	 	 		tdb_dffs_sh += sFour_tradelog.tx_amt;
	 	  	else
	 	  		tdb_jffs_sh += sFour_tradelog.tx_amt;
	 	}	 	
	 	
	 	++num_sh;
	 	
	 	if ( num_sh >= MAX )
	 	{
	 		sprintf( cMessage, "数据空间太小!" );
	 		sprintf( acErrMsg, "数据空间太小!" );
	 		WRITEMSG
	 	  	goto ErrExit;
	 	}	  
	}
	Four_tradelog_Clo_Sel();
	
	/*-----------------ATM取款,POS消费交易----------------*/
	ret = Chnl_self_trade_Dec_Sel( g_pub_tx.reply , "tx_date = %ld and stat = '0' \
		and ( ( tx_code = '6203' and info_source in ('1', '2') ) or \
		tx_code = '6301' )  order by trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "执行Four_tradelog_Dec_Sel错[%s]!!", g_pub_tx.reply );
		sprintf( acErrMsg, "执行Four_tradelog_Dec_Sel错[%s]!!", g_pub_tx.reply );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset( &sChnl_self_trade, 0x0, sizeof(sChnl_self_trade) );
		ret = Chnl_self_trade_Fet_Sel( &sChnl_self_trade, g_pub_tx.reply ); 
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "执行 chnl_self_trade错[%s]!!", g_pub_tx.reply );
			sprintf( acErrMsg, "执行 chnl_self_trade错[%s]!!", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		/*-------------ATM,POS不存在存款--------------*/
		if( !strcmp( sChnl_self_trade.tx_code, "6203") && sChnl_self_trade.info_source[0] == '2' )	/* 本代他 */
	 	{
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sChnl_self_trade.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sChnl_self_trade.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4302" );
	 		strcpy( sSix_dz_log.mb_bank, "4301" );
	 		sSix_dz_log.tx_amt = sChnl_self_trade.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sChnl_self_trade.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sChnl_self_trade.stra );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 			
	 	  	bdt_bs_sh++ ;
	 	  	bdt_dffs_sh+= sChnl_self_trade.tx_amt;	/*贷方*/
	 	}
	 	else	/* 他代本 */
	 	{ 
	 		 			
	 		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "0" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, sChnl_self_trade.tx_code );
	 		strcpy( sSix_dz_log.ac_no, sChnl_self_trade.ac_no );
	 		strcpy( sSix_dz_log.sl_bank, "4301" );
	 		strcpy( sSix_dz_log.mb_bank, "4302" );
	 		sSix_dz_log.tx_amt = sChnl_self_trade.tx_amt;
	 		sprintf( sSix_dz_log.sl_trace_no, "%010ld", sChnl_self_trade.trace_no );
	 		sprintf( sSix_dz_log.center_trace_no, "%010ld", sChnl_self_trade.stra );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", " " );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
	 		
	 		tdb_bs_sh++;
	 	  	tdb_jffs_sh += sChnl_self_trade.tx_amt;	/*借方*/
	 	}
	 		 	
	 	++num_sh;
	 	
	 	if ( num_sh >= MAX )
	 	{
	 		sprintf( cMessage, "数据空间太小!" );
	 		sprintf( acErrMsg, "数据空间太小!" );
	 		WRITEMSG
	 	  	goto ErrExit;
	 	}		
	}
	Chnl_self_trade_Clo_Sel();
		
	/*-------------读取总帐文件--------------*/
	sprintf( filename, "/backup/data/HZQC_4302_%s.txt",  trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "Can't open file %s !", filename );
		sprintf( acErrMsg, "Can't open file %s !", filename );
		WRITEMSG
		fclose(fw);
		goto ErrExit;
	}
	
	while( fgets( buf, 1023, fr) != NULL)
	{
		/*--------判断是否本行记录--------*/
		if( memcmp( buf+9, "4302", 4 ) !=0 ) 
		{
			continue;
		}
		else
		{
	    		if( memcmp( buf, trn_date, 8 ) !=0 )
	    		{
	      			buf[8]=0;
	      			sprintf( cMessage,"日期不正确! %s" , buf);
	      			sprintf( acErrMsg, "日期不正确! %s", buf );
	      			WRITEMSG
	      			fclose( fr );
	      			goto ErrExit;
	    		}
	    		
	    		memset(fldstr,0x0,sizeof(fldstr));
			for( i=0;i<15;i++ )
			{
				ret=pub_base_cut_str( buf, fldstr[i], "|", i+1 );
					vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
				pub_base_strpack( fldstr[i] );
			}
	    		  		
	    		/* 本代他笔数 */
	    		bdt_bs_hj = atoi(fldstr[6]);
	    		/* 本代他借方发生额 */
	    		bdt_jffs_hj = atof(fldstr[7]);
	    		/* 本代他贷方发生额 */
	    		bdt_dffs_hj = atof(fldstr[8]);
	    		
	    		/* 他代本笔数 */
	    		tdb_bs_hj = atoi(fldstr[9]);
	    		/* 他代本借方发生额 */
	    		tdb_jffs_hj = atof(fldstr[10]);
	    		/* 他代本贷方发生额 */
	    		tdb_dffs_hj = atof(fldstr[11]);
	    		
	    		/* 借贷标志 */
	    		jdbz[0] = fldstr[13][0];
	    		/* 清算差额 */
	    		qsje_hj = atof(fldstr[14]);
	    		
	    		break;
		}
	}
	fclose(fr);
	
	tdb_bs_zx = bdt_bs_zx = 0 ;
	tdb_jffs_zx = tdb_dffs_zx = 0.0 ;
	bdt_jffs_zx = bdt_dffs_zx = 0.0 ;
	
	sprintf( filename, "/backup/data/ZXLS_4302_%s.txt", trn_date );
	fr = fopen( filename, "r" );
	if( fr == NULL )
	{
		sprintf( cMessage, "Can't open file %s !", filename );
		sprintf( acErrMsg, "Can't open file %s !", filename );
		WRITEMSG
		fclose(fw);
		goto ErrExit;
	}
	
	num_yl = 0 ;
	while( fgets( buf, 1023, fr) != NULL )
	{
		n = strlen(buf);
		dl_kh = 0 ; /* 设置为其他行 */
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<35;i++ )
		{
			ret=pub_base_cut_str( buf, fldstr[i], "|", i+1 );
				/*vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);*/
			pub_base_strpack( fldstr[i] );
		}		
		
		/*01: 交易码 */
		strcpy( trcd, fldstr[0] );
		
		/*---------异地卡查询余额记录去掉----------*/
		if( memcmp(trcd,"7795",4) == 0  || memcmp(trcd,"7895",4) == 0 )
		{
			continue;
		}
		
		/* 02: 交易日期 */
		strcpy(date , fldstr[1] );
		
		/* 03: 交易时间 */ 
		strcpy(time , fldstr[2] );
		
		/* 04：受理行 */
		strcpy(srsn , fldstr[3] );
		if( memcmp(srsn , "4302",4) == 0 ) 
		{
			dl_kh = 1;
		}
		
		/* 05：发卡行 */
		strcpy( otsn , fldstr[4] );
		if ( memcmp( otsn ,"4302",4 ) == 0 ) 
		{
			dl_kh = 2 ;
		}
		
		/* 其他行则继续循环, 不往下处理 */
		if(dl_kh == 0)  
		{
			continue; 
		}
		
		/* 如为开户行，则将交易码改为78XX */
		if(dl_kh==2) 
		{
			trcd[1]='8';  
		}
		
		/* 06：交易柜员 */
		strcpy(oper, fldstr[5]);
		
		/* 07：交易机构 */
		
		/* --- 08：帐户号 */
		
		/* --- 09：凭证代号 */
		
		/*10：卡号 16位或19位 */
		strcpy( amcr, fldstr[9] );
		if( memcmp( lsbuf, "69430", 5) == 0 )
		{
			memcpy( amcr, lsbuf+2, 16 ); 
			amcr[16]=0;
		}
		
		/* 11：交易金额 */
		tram = atof(fldstr[10])/1.00 ;
		
		/* 12：交易方式  0-折 1-卡 */
		
		/* 13：代理(受理)行流水 长度为10位，前补零 */
		strcpy(seq_dl, fldstr[12]);
		
		/* 14：开户(处理)行流水 长度为10位，前补零 */
		strcpy(seq_kh, fldstr[13]);
		
		/* 15：中心流水 长度为10位，前补零 */
		strcpy(seq_zx, fldstr[14]);
		
		/* 16：备用帐号 POS交易时存放商户号 */
		
		/* 17：备用金额 存放余额信息*/
		
		/* 18：运行标志 1-表示中心接收到请求 2-表示交易成功完成  */
		
		/* --- 19：受理行对帐标志 -此字段停止使用 */
		
		/* --- 20：发卡行对帐标志 此字段停止使用 */
		
		/* 21：借贷标志 0-表示借方  1-表示贷方 */
		strcpy(jdbz, fldstr[20]);
		
		/* ---22：手续费用 */
		
		/* ---23：手续费率 */
		
		/* 24：原交易中心流水  冲正交易时由代理行提交的原交易中心流水 */
		
		/* 25：授权代码 存放代理行的授权代码 */
		
		/* 26：客户代码  开户行返回的客户号信息 */
		
		/* 27：货币代号 01-人民币 */
		
		/* 28：交易种类  0-代理行发出 1-中心发出 */
		
		/* --- 29：备注1 */
		
		/* --- 30：备注2 */
		
		/* 31：响应码 00-成功 */
		if(fldstr[30][0]==10) 
			fldstr[30][0]=' '; /* 将换行改为空格 */
		
		strcpy( ret_code, fldstr[30] );
		
		/* 32：被冲帐标志  0-冲正交易 1-被冲正交易 */
		strcpy( czbz, fldstr[31] );
		
		/* 33：终端号 */
		if( memcmp( ret_code, "00",2)==0 && czbz[0]!='1' && czbz[0]!='0' )
		{
						
			memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
	 		strcpy( sSix_dz_log.inf_source, "1" );
	 		sSix_dz_log.tx_date = lDate;
	 		strcpy( sSix_dz_log.tx_code, trcd );
	 		strcpy( sSix_dz_log.ac_no, amcr );
	 		strcpy( sSix_dz_log.sl_bank, srsn );
	 		strcpy( sSix_dz_log.mb_bank, otsn );
	 		sSix_dz_log.tx_amt = tram;
	 		sprintf( sSix_dz_log.sl_trace_no, "%s", seq_dl );
	 		sprintf( sSix_dz_log.center_trace_no, "%s", seq_zx );
	 		sprintf( sSix_dz_log.opn_trace_no, "%s", seq_kh );
	 		
	 		ret = Six_dz_log_Ins( sSix_dz_log, g_pub_tx.reply );
				
			++num_yl;
			if( dl_kh == 1 )	/* 本代他 */
			{ 
		    		bdt_bs_zx++ ;
		    		if( memcmp( trcd, "7701", 4 ) == 0 )
		      			bdt_jffs_zx += tram ;
		    		else 
		    			bdt_dffs_zx+= tram;
		  	}
		  	else 	/* 他代本 */
		  	{
		    		tdb_bs_zx++ ;
		    		if(memcmp(trcd,"7801",4)==0)
		      			tdb_dffs_zx += tram ;
		    		else
		    			tdb_jffs_zx += tram;
		  	}
		}
		
		if( num_yl >= MAX )
		{
			sprintf( cMessage, "数据空间分配小!" );
		  	sprintf( acErrMsg, "数据空间分配小!" );
		  	WRITEMSG
		  	fclose(fr);
		  	goto ErrExit;
		}
	}
		
	fclose(fr);

	fprintf(fw,"中心总帐:\n");
	fprintf(fw,"    本代他笔数 %4d   借方:%12.2f   贷方:%12.2f\n",\
	      bdt_bs_hj, bdt_jffs_hj, bdt_dffs_hj );
	fprintf(fw,"    他代本笔数 %4d   借方:%12.2f   贷方:%12.2f\n",\
	      tdb_bs_hj, tdb_jffs_hj, tdb_dffs_hj );
	fprintf(fw,"    清算金额:%.2f\n", qsje_hj);
	
	/*----------------中心总帐与中心明细对比----------------*/
	if (bdt_bs_zx != bdt_bs_hj || tdb_bs_zx!=tdb_bs_hj)
	{
		fprintf(fw,"中心明细:\n");
		fprintf(fw,"    本代他笔数 %4d   借方:%12.2f   贷方:%12.2f\n",\
		   bdt_bs_zx, bdt_jffs_zx, bdt_dffs_zx );      
		fprintf(fw,"    他代本笔数 %4d   借方:%12.2f   贷方:%12.2f\n", \
		   tdb_bs_zx, tdb_jffs_zx, tdb_dffs_zx );
	}
	
	/*-----------------中心总帐与商行总帐对比---------------*/
	if (bdt_bs_sh != bdt_bs_hj || tdb_bs_sh!=tdb_bs_hj ||
	fabs(bdt_jffs_sh-bdt_jffs_hj)>0.001||fabs(bdt_dffs_sh-bdt_dffs_hj)>0.001||
	fabs(tdb_jffs_sh-tdb_jffs_hj)>0.001||fabs(tdb_dffs_sh-tdb_dffs_hj)>0.001)
	{
		fprintf(fw,"商行汇总:\n");
		fprintf(fw,"    本代他笔数 %4d   借方:%12.2f   贷方:%12.2f\n",\
		   bdt_bs_sh, bdt_jffs_sh, bdt_dffs_sh );
		fprintf(fw,"    他代本笔数 %4d   借方:%12.2f   贷方:%12.2f\n", \
		   tdb_bs_sh, tdb_jffs_sh, tdb_dffs_sh );
	}
	else
	{
		fprintf( fw, "今日帐务核对正确!\n" );
	}
	
	/*---------------商行有中心无-----------------*/
	ret =  Six_dz_log_Dec_Sel( g_pub_tx.reply , "inf_source = '0' and tx_date = %ld \
		order by center_trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "执行Six_dz_log_Dec_Sel错!" );
		sprintf( acErrMsg, "执行Six_dz_log_Dec_Sel错!" );
		WRITEMSG
		fclose(fr);
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
		ret = Six_dz_log_Fet_Sel( &sSix_dz_log, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "执行Six_dz_log_Fet_Sel错!" );
			sprintf( acErrMsg, "执行Six_dz_log_Fet_Sel错!" );
			WRITEMSG
			fclose(fr);
			goto ErrExit;
		}
		
		ret = sql_count( "six_dz_log", "inf_source = '1' and tx_date = %ld \
			and ac_no = '%s' and tx_amt = %lf and center_trace_no = '%s'", \
			lDate, sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		if ( ret != 1 )
		{
			fprintf( fw, "商行 %4s %4s %4s %-19s %10.2f %s\n",\
	 			sSix_dz_log.tx_code, sSix_dz_log.sl_bank, sSix_dz_log.mb_bank, \
	 			sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		}
	}
	
	Six_dz_log_Clo_Sel();
	
	/*---------------中心有商行无-----------------*/
	ret =  Six_dz_log_Dec_Sel( g_pub_tx.reply , "inf_source = '1' and tx_date = %ld \
		order by center_trace_no", lDate );
	if ( ret )
	{
		sprintf( cMessage, "执行Six_dz_log_Dec_Sel错!" );
		sprintf( acErrMsg, "执行Six_dz_log_Dec_Sel错!" );
		WRITEMSG
		fclose(fr);
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sSix_dz_log, 0x0, sizeof(sSix_dz_log) );
		ret = Six_dz_log_Fet_Sel( &sSix_dz_log, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( cMessage, "执行Six_dz_log_Fet_Sel错!" );
			sprintf( acErrMsg, "执行Six_dz_log_Fet_Sel错!" );
			WRITEMSG
			fclose(fr);
			goto ErrExit;
		}
		
		ret = sql_count( "six_dz_log", "inf_source = '0' and tx_date = %ld \
			and ac_no = '%s' and tx_amt = %lf and center_trace_no = '%s'", \
			lDate, sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		if ( ret != 1 )
		{
			fprintf( fw, "中心 %4s %4s %4s %-19s %10.2f %s\n",\
	 			sSix_dz_log.tx_code, sSix_dz_log.sl_bank, sSix_dz_log.mb_bank, \
	 			sSix_dz_log.ac_no, sSix_dz_log.tx_amt, sSix_dz_log.center_trace_no );
		}
	}
	
	Six_dz_log_Clo_Sel();	
	
	fclose(fw);
		
OkExit:
	strcpy( cFlag, "0" );
	strcpy( cMessage, "对帐完毕!" );
	return 0;
ErrExit:
	fclose( fw );
	strcpy( cFlag, "1" );
	return 0;
}
