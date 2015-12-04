/*************************************************
* 文 件 名:  spF080.c
* 功能描述： ATM清机
*
* 作    者:  robbit
* 完成日期： 
*
* 修改记录： 
* 日   期:   20080416
* 修 改 人:  
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "mo_rmny_smny_c.h"
#include "seqctl_c.h"

spF080()
{
	struct com_tel_c com_tel;
	struct cash_mst_c cash_mst;
	struct cash_mst_c Vcash_mst;
	struct cash_mst_c Scash_mst;
	struct cash_mst_hst_c VCashHst;
	struct cash_mst_hst_c SCashHst;
	struct mo_rmny_smny_c Mormnysmny;
	struct seqctl_c       Seqctl;
	char Tel[TEL_LENGTH+1];
	
	double txamt=0.00;
    
	char type[2];
	char type1[2];
	int ret=0;
	int ret1=0;
	int ret2=0;
	
	memset(&com_tel,0x00,sizeof(struct com_tel_c));
	memset(&cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Vcash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Scash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&VCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&SCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&Mormnysmny,0x00,sizeof(struct mo_rmny_smny_c));
	memset(&Seqctl,0x00,sizeof(struct seqctl_c));
	memset(Tel,0x00,sizeof(Tel));
	memset(type,0x00,sizeof(type));
	memset(type1,0x00,sizeof(type1)); 
	
	pub_base_sysinit();
	get_zd_data("1280",com_tel.name);
	memcpy(com_tel.tel,com_tel.name,sizeof(com_tel.tel)-1);
	get_zd_data("0670",type);
	get_zd_data("0680",type1);
	get_zd_double("0390",&txamt);

	/**------- 初始化全局变量 --------**/
	
	ret = Com_tel_Sel( g_pub_tx.reply,&com_tel," tel='%s' ",com_tel.name );
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",com_tel.name );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	/*****add by chengbo 非同一机构的柜员不允许此操作  20090515 开封*******/
	if(strcmp(g_pub_tx.tx_br_no,com_tel.br_no)!=0){
		
		sprintf(acErrMsg,"非本机构ATM柜员");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L255");
		goto ErrExit;
	}
  /*****end 20090515**************/
	ret = Cash_mst_Sel(g_pub_tx.reply,&cash_mst,"tel='%s'",com_tel.tel);
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",com_tel.name );
		WRITEMSG
		strcpy( g_pub_tx.reply,"M008" );
		goto ErrExit;
	}
	
	/***修改柜员现金台帐***/
	ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",g_pub_tx.tel);
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}
	ret = Cash_mst_Fet_Upd(&Vcash_mst,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M008");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	
	Vcash_mst.bal=Vcash_mst.bal+cash_mst.bal;	
	ret = Cash_mst_Upd_Upd(Vcash_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg,"修改现金台帐错误[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"P153" );
		goto ErrExit;
	} 
	Cash_mst_Clo_Upd();
	
	/***修改AIM现金台帐***/
	ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel.tel);
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",com_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}
	ret = Cash_mst_Fet_Upd(&Scash_mst,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M008");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Scash_mst.bal=Scash_mst.bal-cash_mst.bal;	
	ret = Cash_mst_Upd_Upd(Scash_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf( acErrMsg,"修改现金台帐错误[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"P153" );
		goto ErrExit;
	} 
	Cash_mst_Clo_Upd();
	
	/**插入现金台帐明细***/
	strcpy( g_pub_tx.brf,"收入现金");
	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.ac_no,Vcash_mst.ac_no  );
	
	strcpy( g_pub_tx.cur_no,"01" );
	g_pub_tx.tx_amt1=cash_mst.bal;
     
     
        VCashHst.trace_no = g_pub_tx.trace_no;	/*主机流水号*/
        VCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*交易流水序号*/
	strcpy( VCashHst.br_no , g_pub_tx.tx_br_no );	/*机构*/
	strcpy( VCashHst.ac_no , Vcash_mst.ac_no );	/*账号*/
	strcpy( VCashHst.tel, g_pub_tx.tel );		/*柜员*/
	strcpy( VCashHst.cur_no , g_pub_tx.cur_no );	/*币种*/
	sprintf( VCashHst.add_ind , g_pub_tx.ct_ind ); 	/*收付标志*/
	VCashHst.amt =	cash_mst.bal;		        /*发生额*/
	VCashHst.tx_date = g_pub_tx.tx_date;		/*流水日期*/
	VCashHst.tx_time = g_pub_tx.tx_time;		/*交易时间*/
	strcpy( VCashHst.brf ,	g_pub_tx.brf );		/*摘要*/
	strcpy( VCashHst.cash_code , g_pub_tx.cash_code );	/*现金代码*/
	get_zd_long("1261",&VCashHst.tt_no);/*add by cgx 20090818 nxhhyh*/
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
	strcpy( g_pub_tx.brf,"付出现金");
        strcpy( g_pub_tx.ct_ind,"1" );
        strcpy( g_pub_tx.ac_no,Scash_mst.ac_no  );
        
        SCashHst.trace_no = g_pub_tx.trace_no;	/*主机流水号*/
        SCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*交易流水序号*/
	strcpy( SCashHst.br_no , Scash_mst.br_no );	/*机构*/
	strcpy( SCashHst.ac_no , Scash_mst.ac_no );	/*账号*/
	strcpy( SCashHst.tel, Scash_mst.tel );		/*柜员*/
	strcpy( SCashHst.cur_no , g_pub_tx.cur_no );	/*币种*/
	sprintf( SCashHst.add_ind , g_pub_tx.ct_ind ); 	/*收付标志*/
	SCashHst.amt =	cash_mst.bal;		        /*发生额*/
	SCashHst.tx_date = g_pub_tx.tx_date;		/*流水日期*/
	SCashHst.tx_time = g_pub_tx.tx_time;		/*交易时间*/
	strcpy( SCashHst.brf ,	g_pub_tx.brf );		/*摘要*/
	strcpy( SCashHst.cash_code , g_pub_tx.cash_code );	/*现金代码*/         
	get_zd_long("1261",&SCashHst.tt_no);/*add by cgx 20090818 nxhhyh*/
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
	vtcp_log("%s %d dc_code = %s",__FILE__,__LINE__,g_pub_tx.dc_code);
	set_zd_data(DC_CODE,g_pub_tx.dc_code);
	if(type[0]=='0') 
	{
	goto OkExit;	
	}
	else
	{
		if(type1[0]=='0')  /***长款****/
		{
			/*** 插入长短款登记簿 ***/
			set_zd_double("0400",txamt);
			strcpy( Mormnysmny.br_no, com_tel.br_no );
			Mormnysmny.occ_date=g_pub_tx.tx_date;
			strcpy( Mormnysmny.tx_opmd, "0" );
			strcpy( Mormnysmny.cur_no, "01" );
			Mormnysmny.amt=txamt;
			Mormnysmny.tx_date=g_pub_tx.tx_date;
			Mormnysmny.trace_no=g_pub_tx.trace_no;
			strcpy( Mormnysmny.occ_tel, cash_mst.tel);
			strcpy( Mormnysmny.brf, "长款登记");
			strcpy( Mormnysmny.tel, g_pub_tx.tel);
			strcpy( Mormnysmny.occ_br_no,g_pub_tx.tx_br_no);
			
			ret = Seqctl_Dec_Upd(g_pub_tx.reply,"ac_ind='%s' and br_no='00000'",CDK_IND);
			if(ret)
			{
				vtcp_log("%s,%d定义游标错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			ret = Seqctl_Fet_Upd(&Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d该记录不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			Mormnysmny.seqn=Seqctl.seqno;
			Seqctl.seqno++;
			ret = Seqctl_Upd_Upd(Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d更新序号表错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			
			Seqctl_Clo_Upd();
			get_zd_long("1261",&Mormnysmny.tt_no);/* add by cgx 20090818 nxhhyh */
			ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);         
			if( ret )
			{
				sprintf(acErrMsg,"登记长短款登记簿出错![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"CD29");
				goto ErrExit;
			}
			
			set_zd_data("121A","ATM清机长款");
			set_zd_int("0280",Mormnysmny.seqn);
			goto OkExit1;
	        }
		else if (type1[0]=='1') /**短款***/
		{
			/*** 插入长短款登记簿 ***/
			set_zd_double("0400",txamt);
			strcpy( Mormnysmny.br_no, com_tel.br_no );
			Mormnysmny.occ_date=g_pub_tx.tx_date;
			strcpy( Mormnysmny.tx_opmd, "3" );
			strcpy( Mormnysmny.cur_no, "01" );
			Mormnysmny.amt=txamt;
			Mormnysmny.tx_date=g_pub_tx.tx_date;
			Mormnysmny.trace_no=g_pub_tx.trace_no;
			strcpy( Mormnysmny.occ_tel, cash_mst.tel);
			strcpy( Mormnysmny.brf, "短款登记");
			strcpy( Mormnysmny.tel, g_pub_tx.tel);
			strcpy( Mormnysmny.occ_br_no,g_pub_tx.tx_br_no);
			ret = Seqctl_Dec_Upd(g_pub_tx.reply,"ac_ind='%s' and br_no='00000'",CDK_IND);
			if(ret)
			{
				vtcp_log("%s,%d定义游标错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			ret = Seqctl_Fet_Upd(&Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d该记录不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			Mormnysmny.seqn=Seqctl.seqno;
			Seqctl.seqno++;
			ret = Seqctl_Upd_Upd(Seqctl,g_pub_tx.reply);
			if(ret)
			{
				vtcp_log("%s,%d更新序号表错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D105");
				goto ErrExit;
			}
			
			Seqctl_Clo_Upd();
			get_zd_long("1261",&Mormnysmny.tt_no);/* add by cgx 20090818 nxhhyh */
			ret=Mo_rmny_smny_Ins(Mormnysmny,g_pub_tx.reply);         
	                if( ret )
		        {
		          	sprintf(acErrMsg,"登记钱箱明细出错![%d]",ret);
		           	WRITEMSG
		           	strcpy(g_pub_tx.reply,"P155");
		           	goto ErrExit;
		        }
			set_zd_data("121A","ATM清机短款");
			set_zd_int("0280",Mormnysmny.seqn);
			goto OkExit2;
		}	
        }
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return(长款): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return(短款): reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
