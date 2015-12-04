/*************************************************
* 文 件 名:  spF090.c
* 功能描述： ATM自动清机  
*
* 作    者:  lifei
* 完成日期： 
*
* 修改记录： 
* 日   期:   2009-7-30 9:08:53
* 修 改 人:  
* 修改内容:

insert into tx_def values ('8620','ATM自动清机','10000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','3','2','3',null);

insert into tx_flow_def values('8620',0,'F090','#=0001@1@#=0002@3@#$');
insert into tx_flow_def values('8620',1,'M001','#=0000@2@#$');
insert into tx_flow_def values('8620',2,'A017','#$');
insert into tx_flow_def values('8620',3,'A016','#=0000@4@#$');
insert into tx_flow_def values('8620',4,'M002','#$');

insert into tx_sub_def values('F090','ATM自动清机','spF090','0','0');



insert into tx_sub_rel values('8620','A016','1','190104','1201','账号');
insert into tx_sub_rel values('8620','A016','1','01','1204','币种');
insert into tx_sub_rel values('8620','A016','1','1','1205','现转标识');
insert into tx_sub_rel values('8620','A016','0','0400','1208','金额');

insert into tx_sub_rel values('8620','A017','1','290201','1211','账号');
insert into tx_sub_rel values('8620','A017','1','01','1214','币种');
insert into tx_sub_rel values('8620','A017','1','1','1215','现转标识');
insert into tx_sub_rel values('8620','A017','0','0400','1218','交易金额');

insert into tx_sub_rel values('8620','M001','1','01','1181','币种');
insert into tx_sub_rel values('8620','M001','0','0400','1184','交易金额');
insert into tx_sub_rel values('8620','M001','1','ATM长款','1187','摘要');

insert into tx_sub_rel values('8620','M002','1','01','1191','币种');
insert into tx_sub_rel values('8620','M002','0','0400','1194','交易金额');
insert into tx_sub_rel values('8620','M002','1','ATM短款','1197','摘要');


*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"
#include "mo_rmny_smny_c.h"
#include "seqctl_c.h"
#include "com_parm_c.h"

spF090()
{
	struct com_tel_c com_tel;
	struct cash_mst_c cash_mst;
	struct cash_mst_c Vcash_mst;
	struct cash_mst_c Scash_mst;
	struct cash_mst_hst_c VCashHst;
	struct cash_mst_hst_c SCashHst;
	struct mo_rmny_smny_c Mormnysmny;
	struct seqctl_c       Seqctl;
	char cTel[TEL_LENGTH+1];
	
	double txamt=0.00;
	double txamt1=0.00;
  char cTmp[7];
	char type[2];
	char type1[2];
	int ret=0;
	int ret1=0;
	int ret2=0;
	
	memset(cTel,0x00,sizeof(cTel));
	memset(cTmp,0x00,sizeof(cTmp));
	memset(&com_tel,0x00,sizeof(struct com_tel_c));
	memset(&cash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Vcash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&Scash_mst,0x00,sizeof(struct cash_mst_c));
	memset(&VCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&SCashHst,0x00,sizeof(struct cash_mst_hst_c));
	memset(&Mormnysmny,0x00,sizeof(struct mo_rmny_smny_c));
	memset(&Seqctl,0x00,sizeof(struct seqctl_c));

	memset(type,0x00,sizeof(type));
	memset(type1,0x00,sizeof(type1)); 
	
	pub_base_sysinit();
	memset(cTmp,0x00,sizeof(cTmp));
	get_zd_data("0580",cTmp);
	memcpy(com_tel.tel,cTmp,4);
	vtcp_log("%s %d cTmp=[%s],com_tel.tel=[%s]",__FILE__,__LINE__,cTmp,com_tel.tel);
	/*memcpy(com_tel.tel,com_tel.name,4);*/
	get_zd_double("0390",&txamt1);
	get_zd_data("0920",cTel);
	vtcp_log("%s %d cTel=[%s],amt=[%f]",__FILE__,__LINE__,cTel,txamt1);

	/**------- 初始化全局变量 --------**/
	
	ret = Com_tel_Sel( g_pub_tx.reply,&com_tel," tel='%s' ",com_tel.tel );
	if( ret )
	{
		sprintf( acErrMsg,"柜员不存在[%s]",com_tel.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	/*****add by chengbo 非同一机构的柜员不允许此操作  20090515 开封*****
	if(strcmp(g_pub_tx.tx_br_no,com_tel.br_no)!=0){
		
		sprintf(acErrMsg,"非本机构ATM柜员");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L255");
		goto ErrExit;
	}**/
  /*****end 20090515**************/
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
		strcpy( g_pub_tx.reply,"M008" );
		goto ErrExit;
	}
	
	vtcp_log("%s %d qtxamt1=%f,kamt=%f",__FILE__,__LINE__,txamt1,cash_mst.bal);
	
	if (pub_base_CompDblVal(txamt1,cash_mst.bal)==0)
	{
		type[0]='0';	
	}
	else if (pub_base_CompDblVal(txamt1,cash_mst.bal)<0)
	{
		txamt = cash_mst.bal - txamt1;
		type[0]='1';
		type1[0]='0';		
	}
	else
	{
		txamt = txamt1 - cash_mst.bal;
		type[0]='1';
		type1[0]='1';			
	}
	
	ret = pub_base_GetParm_Str(com_tel.br_no,9800,cTmp);
	vtcp_log("%s %d tel=%s",__FILE__,__LINE__,cTmp);
	strcpy(g_pub_tx.tel,cTel);
	vtcp_log("%s %d tel=%s",__FILE__,__LINE__,g_pub_tx.tel);
	
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
			/*get_zd_long("1261",&Mormnysmny.tt_no);add by cgx 20090818 nxhhyh*/
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
			/*get_zd_long("1261",&Mormnysmny.tt_no);add by cgx 20090818 nxhhyh*/
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
