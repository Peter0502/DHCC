/*************************************************
* 文 件 名:  sp5380.c
* 功能描述： 通存通兑手续费冲正
*
* 作    者:  martin
* 完成日期： 
*
* 修改记录：
* 日   期:2007.11.16
* 修 改 人:
* 修改内容:非本网点或本网点下属网点帐户信息不能查询
*************************************************/
#include <stdio.h>
#include <math.h>
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "lv_pkgreg_c.h"
#include "hv_orno_brno_c.h"

int sp5380()
{
	struct lv_pkgreg_c f_lv_pkgreg;
	struct lv_pkgreg_c vlv_pkgreg;
	struct hv_orno_brno_c f_hv_orno_brno;
	
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		long WtDate=0;
		double TmpAmt=0.00;
		FILE *fp;
	char opn_br_no[6];
	char Flag[2];
	char LvSts[5];
	char ChrgInd[5];
	char Orderno[9];
	
	memset( Flag     ,'\0',sizeof(Flag) );
	memset( LvSts    ,'\0',sizeof(LvSts) );
	memset( ChrgInd  ,'\0',sizeof(ChrgInd) );
	memset( Orderno  ,'\0',sizeof(Orderno) );
	memset( &g_pub_tx,  0 ,sizeof(g_pub_tx) );
  memset( wherelist,  0 ,sizeof(wherelist) );
	memset( opn_br_no,'\0',sizeof(opn_br_no) );

	pub_base_sysinit();

	get_zd_data("0030",opn_br_no);
	get_zd_data("0670",Flag);

	/**组成查询条件**/
	ret=Hv_orno_brno_Sel(g_pub_tx.reply,&f_hv_orno_brno," br_no='%s' ",opn_br_no );
	TRACE
	
	if( strlen(opn_br_no) )
	{
		if(memcmp(opn_br_no,QS_BR_NO, 5) == 0||memcmp(opn_br_no, KJ_BR_NO, 5) == 0)
		{
		}
		else
		{
			sprintf( tmpstr," or_br_no='%s' and",f_hv_orno_brno.or_br_no );
			strcat( wherelist,tmpstr );
		}
	}
 if( Flag[0]=='0' )
 {
	/**组成查询**/
	strcat(wherelist," lw_ind='1' and pkgno in ('003','004')  and lv_sts in ('5','C') and tx_chrg_ind in ('1','2') ORDER BY wt_date ") ;

	ret=Lv_pkgreg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel( &f_lv_pkgreg, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			/**显示列标题**/
			fprintf( fp,"~委托日期|Pkg编号|交易序号|交易状态|付款人帐号|收款人帐号|手续费标志|$手续费金额|\n" );
		}
		
	if( f_lv_pkgreg.lv_sts[0] =='5' )
		{
				strcpy( LvSts , "拒绝" );		
		}else{
				strcpy( LvSts , "冲正" );
	  }
	if( f_lv_pkgreg.tx_chrg_ind[0] =='1' )
		{
				strcpy( ChrgInd , "现金" );		
		}else{
				strcpy( ChrgInd , "转帐" );
	  }
	  
 
	fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%.2lf|\n", \ 
		 			f_lv_pkgreg.wt_date,f_lv_pkgreg.pkgno,f_lv_pkgreg.orderno,LvSts,\
		 			f_lv_pkgreg.pay_ac_no,f_lv_pkgreg.cash_ac_no,ChrgInd,f_lv_pkgreg.chrg_amt); 
	 ttlnum++;
	}

	ret=Lv_pkgreg_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
 }else
 if(Flag[0]=='1')
 {
 	
 	get_zd_data("0280",Orderno);
 	get_zd_long("0440",&WtDate);
 	
 	if( strlen(opn_br_no) )
	{
		if(memcmp(opn_br_no,QS_BR_NO, 5) == 0||memcmp(opn_br_no, KJ_BR_NO, 5) == 0)
		{
			sprintf(acErrMsg,"该机构不能做此交易!opn_br_no=[%s]",opn_br_no);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		}
 	}
 	if( strlen(Orderno) )
 	{
			sprintf( tmpstr," orderno=%s and",Orderno );
			strcat( wherelist,tmpstr );
	}
	if( WtDate!=0 )
 	{
			sprintf( tmpstr," wt_date=%ld and",WtDate );
			strcat( wherelist,tmpstr );
	}
	
 	strcat(wherelist," lw_ind='1' and pkgno in ('003','004')  and lv_sts in ('5','C') and tx_chrg_ind in ('1','2','B','C') ") ;

	ret=Lv_pkgreg_Dec_Upd( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ret=Lv_pkgreg_Fet_Upd(&f_lv_pkgreg,g_pub_tx.reply);
	if(ret)
		{
			vtcp_log("[%s][%d]查询数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","查询数据错误");
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
	/** 状态检查 **/
	if( f_lv_pkgreg.tx_chrg_ind[0]=='B' || f_lv_pkgreg.tx_chrg_ind[0]=='C')
		{
			vtcp_log("[%s][%d]此笔已冲正tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
			strcpy( g_pub_tx.reply,"LS69" );
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
	
	vtcp_log("[%s][%d]更新数据库前f_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
	/** 更新已冲正 **/
	if( f_lv_pkgreg.tx_chrg_ind[0]=='1')
		{
			f_lv_pkgreg.tx_chrg_ind[0]='B';
		}else
		{
			f_lv_pkgreg.tx_chrg_ind[0]='C';
		}
	vtcp_log("[%s][%d]更新数据库后f_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
	
 	ret=Lv_pkgreg_Upd_Upd(f_lv_pkgreg,g_pub_tx.reply);
 	if(ret)
 		{
			vtcp_log("[%s][%d]更新数据库错误[%d]",__FILE__,__LINE__,ret);
			set_zd_data("0130","更新数据库错误");
			Lv_pkgreg_Clo_Upd();
			goto ErrExit;
		}
 	Lv_pkgreg_Clo_Upd();
 	
 	pub_base_old_acno(f_lv_pkgreg.pay_ac_no);
	pub_base_strpack(f_lv_pkgreg.pay_ac_no);
	pub_base_old_acno(f_lv_pkgreg.cash_ac_no);
	pub_base_strpack(f_lv_pkgreg.cash_ac_no);
	if( f_lv_pkgreg.chrg_amt>0.005 )
	 {
	   TmpAmt=-f_lv_pkgreg.chrg_amt;
	   vtcp_log("[%s][%d]手续费冲正金额！TmpAmt==[%.2lf]",__FILE__,__LINE__,TmpAmt);
   }
 	
 	/*** 冲账数据处理 ***/
 	if( f_lv_pkgreg.tx_chrg_ind[0]=='B')
		{
			set_zd_double("0400",TmpAmt);
			set_zd_data("0670","1");/*** 现金 ***/
			goto CashExit;
		}else
	if( f_lv_pkgreg.tx_chrg_ind[0]=='C')
		{
			if( memcmp(f_lv_pkgreg.pkgno,"003",3)==0 )
				{
					set_zd_data("0300",f_lv_pkgreg.pay_ac_no);
				}
				else
			if( memcmp(f_lv_pkgreg.pkgno,"004",3)==0 )
				{
					set_zd_data("0300",f_lv_pkgreg.cash_ac_no);
				}
			set_zd_double("0400",TmpAmt);
			set_zd_data("0670","2");/*** 转帐 ***/
			goto TrsfExit;
		}else
		{
			vtcp_log("[%s][%d]出错在这里！f_lv_pkgreg.tx_chrg_ind==[%s]",__FILE__,__LINE__,f_lv_pkgreg.tx_chrg_ind);
			goto ErrExit;
		}
 }

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
CashExit:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before Cash return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
TrsfExit:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before Transfers return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}