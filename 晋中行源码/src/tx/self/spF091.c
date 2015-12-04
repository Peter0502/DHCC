/*************************************************
* 文 件 名:  spF091.c
* 功能描述： ATM加钞 
*
* 作    者:  lifei
* 完成日期： 
*
* 修改记录： 
* 日   期:   2009-7-30 9:17:27
* 修 改 人:  
* 修改内容:
insert into tx_def values ('8621','ATM自动加钞','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','3','2','3',null);
insert into tx_flow_def values('8621',0,'F091','#$');
insert into tx_sub_def values('F091','ATM自动加钞','spF091','0','0');

*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "com_parm_c.h"


spF091()
{
     struct com_tel_c com_tel;
     struct cash_mst_c cash_mst;
     struct cash_mst_c Vcash_mst;
     struct cash_mst_c Scash_mst;
     struct cash_mst_hst_c VCashHst;
     struct cash_mst_hst_c SCashHst;
     char cTel[TEL_LENGTH+1];
     char tel58[10];
     char cTmp[7];
     double txamt=0.00;
     int ret=0;
     int ret1=0;
     
     memset(cTel,0x00,sizeof(cTel));
     memset(cTmp,0x00,sizeof(cTmp));
     memset(&com_tel,0x00,sizeof(struct com_tel_c));
     memset(&cash_mst,0x00,sizeof(struct cash_mst_c));
     memset(&Vcash_mst,0x00,sizeof(struct cash_mst_c));
     memset(&Scash_mst,0x00,sizeof(struct cash_mst_c));
     memset(&VCashHst,0x00,sizeof(struct cash_mst_hst_c));
     memset(&SCashHst,0x00,sizeof(struct cash_mst_hst_c));
     memset(tel58,0x00,sizeof(tel58));
     pub_base_sysinit();
     
     get_zd_data("0580",tel58);
     memcpy(com_tel.tel,tel58,sizeof(com_tel.tel)-1);
     get_zd_double("0390",&txamt);
     get_zd_data("0920",cTel);
     
     vtcp_log("%s %d atm=[%s],tel=[%s]",__FILE__,__LINE__,tel58,cTel);
     
	/**------- 初始化全局变量 --------**/
	
		ret = Com_tel_Sel( g_pub_tx.reply,&com_tel," tel='%s' ",com_tel.tel );
		if( ret )
		{
			sprintf( acErrMsg,"柜员不存在[%s]",com_tel.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		}
		/*
		if(strcmp(g_pub_tx.tx_br_no,com_tel.br_no)!=0)
		{
		
			sprintf(acErrMsg,"非本机构ATM柜员");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L255");
			goto ErrExit;
		}*/
		if( com_tel.csts[0]=='4' )
		{
			sprintf( acErrMsg,"柜员状态错[%s]O[%s]",com_tel.tel,com_tel.csts );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O062" );
			goto ErrExit;
		}
		if( com_tel.csts[0]=='3' )
		{
			sprintf( acErrMsg,"柜员状态错[%s]i[%s]",com_tel.tel,com_tel.csts );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O063" );
			goto ErrExit;
		}
		
		ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst,"tel='%s'",com_tel.tel);
		if( ret )
		{
			sprintf( acErrMsg,"柜员不存在[%s]",com_tel.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		}
		if(cash_mst.bal!=0.00)
		{
			sprintf( acErrMsg,"ATM机没有进行清机处理" );
     	WRITEMSG
     	strcpy( g_pub_tx.reply,"P333");
     	goto ErrExit;
		}
		
		ret = pub_base_GetParm_Str(com_tel.br_no,9800,cTmp);
	
		strcpy(g_pub_tx.tel,cTel);		
		
		/***修改柜员现金台帐***/
		ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
		if( ret )
		{
			sprintf( acErrMsg,"柜员不存在[%s]",g_pub_tx.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		}
		ret = Cash_mst_Fet_Upd(&Vcash_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			goto ErrExit;
		}
		ret1 = pub_base_CompDblVal(Vcash_mst.bal,txamt);
		if (ret1<0)
		{
			sprintf( acErrMsg,"该柜员现金不足[%d]",ret1 );
     	WRITEMSG
     	strcpy( g_pub_tx.reply,"P152");
     	goto ErrExit;
		}
		Vcash_mst.bal=Vcash_mst.bal-txamt;	
		ret = Cash_mst_Upd_Upd(Vcash_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg,"修改现金台帐错误[%s]",g_pub_tx.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		} 
		Cash_mst_Clo_Upd();
     
		/***修改AIM现金台帐***/
		ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel.tel);
		if( ret )
		{
			sprintf( acErrMsg,"柜员不存在[%s]",com_tel.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		}
		ret = Cash_mst_Fet_Upd(&Scash_mst,g_pub_tx.reply);
		if( ret==100 )
		{
			sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P154");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P153");
			goto ErrExit;
		}
		Scash_mst.bal=Scash_mst.bal+txamt;	
		ret = Cash_mst_Upd_Upd(Scash_mst,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg,"修改现金台帐错误[%s]",g_pub_tx.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply,"O120" );
			goto ErrExit;
		} 
		Cash_mst_Clo_Upd();
     
		/**插入现金台帐明细***/
		strcpy( g_pub_tx.brf,"付出现金");
		strcpy( g_pub_tx.ct_ind,"0" );
		strcpy( g_pub_tx.ac_no,Vcash_mst.ac_no  );
		strcpy( g_pub_tx.cur_no,"01" );
		g_pub_tx.tx_amt1=txamt;
     
     
    VCashHst.trace_no = g_pub_tx.trace_no;	/*主机流水号*/
    VCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*交易流水序号*/
		strcpy( VCashHst.br_no , g_pub_tx.tx_br_no );	/*机构*/
		strcpy( VCashHst.ac_no , Vcash_mst.ac_no );	/*账号*/
		strcpy( VCashHst.tel, g_pub_tx.tel );		/*柜员*/
		strcpy( VCashHst.cur_no , g_pub_tx.cur_no );	/*币种*/
		sprintf( VCashHst.add_ind , g_pub_tx.ct_ind ); 	/*收付标志*/
		VCashHst.amt =	txamt;		                /*发生额*/
		VCashHst.tx_date = g_pub_tx.tx_date;		/*流水日期*/
		VCashHst.tx_time = g_pub_tx.tx_time;		/*交易时间*/
		strcpy( VCashHst.brf ,	g_pub_tx.brf );		/*摘要*/
		strcpy( VCashHst.cash_code , g_pub_tx.cash_code );	/*现金代码*/
		/*get_zd_long("1261",&VCashHst.tt_no);add by cgx 20090818 nxhhyh*/
    ret=Cash_mst_hst_Ins(VCashHst,g_pub_tx.reply);         
    if( ret )
		{
			sprintf(acErrMsg,"登记钱箱明细出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P155");
			goto ErrExit;
		}
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"登记交易流水出错!");
			WRITEMSG
			goto ErrExit;
		}
		strcpy( g_pub_tx.brf,"收入现金");
    strcpy( g_pub_tx.ct_ind,"1" );
    strcpy( g_pub_tx.ac_no,Scash_mst.ac_no  );
    
    SCashHst.trace_no = g_pub_tx.trace_no;	/*主机流水号*/
    SCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*交易流水序号*/
		strcpy( SCashHst.br_no , Scash_mst.br_no );	/*机构*/
		strcpy( SCashHst.ac_no , Scash_mst.ac_no );	/*账号*/
		strcpy( SCashHst.tel, Scash_mst.tel );		/*柜员*/
		strcpy( SCashHst.cur_no , g_pub_tx.cur_no );	/*币种*/
		sprintf( SCashHst.add_ind , g_pub_tx.ct_ind ); 	/*收付标志*/
		SCashHst.amt =	txamt;		                /*发生额*/
		SCashHst.tx_date = g_pub_tx.tx_date;		/*流水日期*/
		SCashHst.tx_time = g_pub_tx.tx_time;		/*交易时间*/
		strcpy( SCashHst.brf ,	g_pub_tx.brf );		/*摘要*/
		strcpy( SCashHst.cash_code , g_pub_tx.cash_code );	/*现金代码*/
		/*get_zd_long("1261",&SCashHst.tt_no);add by cgx 20090818 nxhhyh*/
   
    ret=Cash_mst_hst_Ins(SCashHst,g_pub_tx.reply);         
    if( ret )
		{
			sprintf(acErrMsg,"登记钱箱明细出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P155");
			goto ErrExit;
		}
		if( pub_ins_trace_log() )
		{
			sprintf(acErrMsg,"登记交易流水出错!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data(DC_CODE,g_pub_tx.dc_code);
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
