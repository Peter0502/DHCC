/***************************************************************************
 *** 程序作者 : wangxd                                                   ***
 *** 日    期 : 2010-12-25                                               ***
 *** 所属模块 :                                                          ***
 *** 程序名称 : pubf_mob_data_prco.c                                     ***
 *** 程序作用 : 短信签约解约的相关函数,供处理短信签约相关问题使用        ***
 ***                                                                     ***
 *** 使用注意 :                                                          ***
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"
#include "mob_acct_type_c.h"
#include "mob_server_type_c.h"
#include "ln_mst_c.h"
#include "dd_mst_c.h"
#include "com_sys_parm_c.h"
#include "trace_log_c.h"
#include "mob_acct_type_hst_c.h"
#include "mob_fee_mx_c.h"



/*处理签约历史信息--新签约*/
/*参数说明
	只需要传入签约时插入mob_acct_type数据库的表结构即可
	struct mob_acct_type_c st_mob_acct_type 原签约信息结构体
*/
int proc_mob_hst_ins(struct mob_acct_type_c st_mob_acct_type)
{
	struct mob_acct_type_hst_c st_mob_acct_type_hst;

	memset(&st_mob_acct_type_hst,0x00,sizeof(st_mob_acct_type_hst));


	/*开始赋值*/
	memcpy(st_mob_acct_type_hst.ac_no,st_mob_acct_type.ac_no,sizeof(st_mob_acct_type.ac_no));
	st_mob_acct_type_hst.ac_id=st_mob_acct_type.ac_id;
	st_mob_acct_type_hst.ac_seqn=st_mob_acct_type.ac_seqn;
	memcpy(st_mob_acct_type_hst.ac_type,st_mob_acct_type.ac_type,sizeof(st_mob_acct_type.ac_type));
	memcpy(st_mob_acct_type_hst.server_type,st_mob_acct_type.server_type,sizeof(st_mob_acct_type.server_type));
	memcpy(st_mob_acct_type_hst.mobile,st_mob_acct_type.mobile,sizeof(st_mob_acct_type.mobile));
	st_mob_acct_type_hst.ori_tx_date=st_mob_acct_type.tx_date;
	st_mob_acct_type_hst.lst_fee_date=st_mob_acct_type.lst_fee_date;
	memcpy(st_mob_acct_type_hst.lst_tel,st_mob_acct_type.tel,sizeof(st_mob_acct_type.tel));
	memcpy(st_mob_acct_type_hst.sfbz,st_mob_acct_type.sfbz,sizeof(st_mob_acct_type.sfbz));
	memcpy(st_mob_acct_type_hst.mob_sts,st_mob_acct_type.mob_sts,sizeof(st_mob_acct_type.mob_sts));
	memcpy(st_mob_acct_type_hst.phones,st_mob_acct_type.phones,sizeof(st_mob_acct_type.phones));
	memcpy(st_mob_acct_type_hst.lst_br_no,st_mob_acct_type.br_no,sizeof(st_mob_acct_type.br_no));
	memcpy(st_mob_acct_type_hst.filler,st_mob_acct_type.filler,sizeof(st_mob_acct_type.filler));
	st_mob_acct_type_hst.tx_date=g_pub_tx.tx_date;
	memcpy(st_mob_acct_type_hst.tx_code,g_pub_tx.tx_code,sizeof(st_mob_acct_type_hst.tx_code));
	st_mob_acct_type_hst.trace_no=g_pub_tx.trace_no;
	memcpy(st_mob_acct_type_hst.opr_sts,"0",1);
	memcpy(st_mob_acct_type_hst.br_no,g_pub_tx.tx_br_no,sizeof(st_mob_acct_type_hst.br_no));
	memcpy(st_mob_acct_type_hst.tx_tel,g_pub_tx.tel,sizeof(g_pub_tx.tel));
	st_mob_acct_type_hst.fee=0.0;
	memcpy(st_mob_acct_type_hst.auth_tel	,g_pub_tx.auth,sizeof(g_pub_tx.auth));
	memcpy(st_mob_acct_type_hst.memo,"短信签约",sizeof(st_mob_acct_type_hst.memo));
	memcpy(st_mob_acct_type_hst.sts	,"0",1);	
Mob_acct_type_hst_Debug(&st_mob_acct_type_hst);

	g_reply_int = Mob_acct_type_hst_Ins(st_mob_acct_type_hst,g_pub_tx.reply );

    if(g_reply_int)
    {
        vtcp_log("[%s][%d]数据库Mob_acct_type_hst插入记录异常[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D107");
        goto  ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}

/*处理签约历史信息--签约变更*/
/*参数说明
struct mob_acct_type_c st_mob_acct_type 原签约信息的结构体
char	*cOpt							操作类型 1--变更 2--解约 3--暂停 4--暂停解除
char	*new_ac_no						签约变更的新帐号
char	*new_mobile						签约变更的新手机号
double	 fee							签约变更的手续费
	
*/
int proc_mob_hst_modify(struct mob_acct_type_c st_mob_acct_type,char *cOpt,char *new_ac_no,char *new_mobile,double fee)
{
	struct mob_acct_type_hst_c st_mob_acct_type_hst;

	memset(&st_mob_acct_type_hst,0x00,sizeof(st_mob_acct_type_hst));

	vtcp_log("[%s][%d]进入公函cOpt=[%s],new_ac_no=[%s],new_mobile=[%s],fee=[%f]",__FILE__,__LINE__,cOpt,new_ac_no,new_mobile,fee);

	/*开始赋值*/
	memcpy(st_mob_acct_type_hst.ac_no,st_mob_acct_type.ac_no,sizeof(st_mob_acct_type.ac_no));
	st_mob_acct_type_hst.ac_id=st_mob_acct_type.ac_id;
	st_mob_acct_type_hst.ac_seqn=st_mob_acct_type.ac_seqn;
	memcpy(st_mob_acct_type_hst.ac_type,st_mob_acct_type.ac_type,sizeof(st_mob_acct_type.ac_type));
	memcpy(st_mob_acct_type_hst.server_type,st_mob_acct_type.server_type,sizeof(st_mob_acct_type.server_type));
	memcpy(st_mob_acct_type_hst.mobile,st_mob_acct_type.mobile,sizeof(st_mob_acct_type.mobile));
	st_mob_acct_type_hst.ori_tx_date=st_mob_acct_type.tx_date;
	st_mob_acct_type_hst.lst_fee_date=st_mob_acct_type.lst_fee_date;
	memcpy(st_mob_acct_type_hst.lst_tel,st_mob_acct_type.tel,sizeof(st_mob_acct_type.tel));
	memcpy(st_mob_acct_type_hst.sfbz,st_mob_acct_type.sfbz,sizeof(st_mob_acct_type.sfbz));
	memcpy(st_mob_acct_type_hst.mob_sts,st_mob_acct_type.mob_sts,sizeof(st_mob_acct_type.mob_sts));
	memcpy(st_mob_acct_type_hst.phones,st_mob_acct_type.phones,sizeof(st_mob_acct_type.phones));
	memcpy(st_mob_acct_type_hst.lst_br_no,st_mob_acct_type.br_no,sizeof(st_mob_acct_type.br_no));
	memcpy(st_mob_acct_type_hst.filler,st_mob_acct_type.filler,sizeof(st_mob_acct_type.filler));
	st_mob_acct_type_hst.tx_date=g_pub_tx.tx_date;
	memcpy(st_mob_acct_type_hst.tx_code,g_pub_tx.tx_code,sizeof(st_mob_acct_type_hst.tx_code));
	st_mob_acct_type_hst.trace_no=g_pub_tx.trace_no;
	memcpy(st_mob_acct_type_hst.opr_sts,cOpt,1);
	memcpy(st_mob_acct_type_hst.br_no,g_pub_tx.tx_br_no,sizeof(st_mob_acct_type_hst.br_no));
	memcpy(st_mob_acct_type_hst.tx_tel,g_pub_tx.tel,sizeof(g_pub_tx.tel));
	memcpy(st_mob_acct_type_hst.new_ac_no,new_ac_no,sizeof(st_mob_acct_type_hst.new_ac_no));
	memcpy(st_mob_acct_type_hst.new_mobile,new_mobile,sizeof(st_mob_acct_type_hst.new_mobile));
	st_mob_acct_type_hst.fee=fee;
	memcpy(st_mob_acct_type_hst.auth_tel	,g_pub_tx.auth,sizeof(g_pub_tx.auth));
	if(cOpt[0]=='1'){
		memcpy(st_mob_acct_type_hst.memo,"短信签约变更",sizeof(st_mob_acct_type_hst.memo));
	}else if(cOpt[0]=='2'){
		memcpy(st_mob_acct_type_hst.memo,"短信解约",sizeof(st_mob_acct_type_hst.memo));
	}else if(cOpt[0]=='3'){
		memcpy(st_mob_acct_type_hst.memo,"短信签约暂停",sizeof(st_mob_acct_type_hst.memo));
	}else if(cOpt[0]=='4'){
		memcpy(st_mob_acct_type_hst.memo,"短信签约暂停解除",sizeof(st_mob_acct_type_hst.memo));
	}else if(cOpt[0]=='5'){
		memcpy(st_mob_acct_type_hst.memo,"短信签约自动解约",sizeof(st_mob_acct_type_hst.memo));
	}

	memcpy(st_mob_acct_type_hst.sts	,"0",1);	

	g_reply_int = Mob_acct_type_hst_Ins(st_mob_acct_type_hst,g_pub_tx.reply );

    if(g_reply_int)
    {
        vtcp_log("[%s][%d]数据库Mob_acct_type_hst插入记录异常[%d]",__FILE__,__LINE__,g_reply_int);
        strcpy(g_pub_tx.reply,"D107");
        goto  ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;

}
/*
短信银行手续费计费公函
struct mob_acct_type_c st_mob_acct_type   签约信息结构体
int counttype   0 --按日计费
				1 --按月计费
				3 --按季计费
				4 --按年计费
int  feetype	0--预算
				1--实际计费
double fee		计算结果
*/
int count_mob_fee(struct mob_acct_type_c st_mob_acct_type,int feetype,int counttype,double fee)
{
	double true_fee_standard=0.0;
	int days=0;
	double months=0.0;
	double season=0.0;
	double year=0.0;
	char phones[90+1];
	struct mob_server_type_c st_mob_server_type;

	memset(&st_mob_server_type,0x00,sizeof(st_mob_server_type));
	memset(phones,0x00,sizeof(phones));
	fee=0.0;

	vtcp_log("[%s][%d]进入公函feetype=[%d],counttype=[%d],fee=[%f]",__FILE__,__LINE__,feetype,counttype,fee);

	/**收费处理**/
	g_reply_int=Mob_server_type_Sel(g_pub_tx.reply,&st_mob_server_type,"server_type='%s'",st_mob_acct_type.server_type);
	if(g_reply_int)
	{
			sprintf(acErrMsg, "查询服务类型表Mob_server_type错误 [%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
	}
	if(st_mob_acct_type.ac_type[0]=='A')
		true_fee_standard=st_mob_server_type.fee_standard*2.0;
	else
		true_fee_standard=st_mob_server_type.fee_standard;

	vtcp_log("[%s][%d]该笔签约信息的基准费率为true_fee_standard=[%f]!!!",__FILE__,__LINE__,true_fee_standard);
	vtcp_log("st_mob_acct_type.lst_fee_date=[%ld]!!!",st_mob_acct_type.lst_fee_date);
	vtcp_log("g_pub_tx.tx_date=[%ld]!!!",g_pub_tx.tx_date);
	days=pub_base_CalTrueDays(st_mob_acct_type.lst_fee_date,g_pub_tx.tx_date);
	vtcp_log("[%s][%d]该笔签约的欠费天数days=[%d],手机号数目[%d]!!!",__FILE__,__LINE__,days,strlen(st_mob_acct_type.phones)/11);
	fee=0.00;
	if(counttype == 0){
		fee=true_fee_standard*((double)days)*(strlen(st_mob_acct_type.phones)/11 +1)/30;
	}else if(counttype == 1){
		months=days/30;
		fee=true_fee_standard*months*(strlen(st_mob_acct_type.phones)/11 +1);
	}else if(counttype == 2){
		season=days/90;
		fee=true_fee_standard*season*(strlen(st_mob_acct_type.phones)/11 +1)*3;
	}else if(counttype == 3){
		year=days/360;
		fee=true_fee_standard*year*(strlen(st_mob_acct_type.phones)/11 +1)*12;
	}else {
		vtcp_log("[%s][%d]传入公函错误!!!",__FILE__,__LINE__);
	
	}

	vtcp_log("[%s][%d]手续费计算完成,fee=[%f]",__FILE__,__LINE__,fee);
	if(fee<0){
		fee=0.0;
		vtcp_log("[%s][%d]手续费为负值,进行处理,fee=[%f]",__FILE__,__LINE__,fee);
	}
	
	set_zd_double( "0410", fee);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}

/*
检查是否短信银行签约公函

*/
int check_mob_sign(char *Acno,char *check,char *result)
{
	memset(result,0x00,sizeof(result));

	struct mob_acct_type_c st_mob_acct_type;

	memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));

	vtcp_log("[%s][%d]进入签约检查公函Acno=[%s],check=[%s],result=[%s]",__FILE__,__LINE__,Acno,check,result);

	/**判断处理**/
	zip_space(Acno);
	if(check[0]=='1'){
		g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"filler='%s' or ac_no='%s' ",Acno,Acno);
		if(g_reply_int==100){

			memcpy(result,"0",1);
			vtcp_log("[%s][%d]账号或借据号[%s]未查询到签约记录!!!",__FILE__,__LINE__,Acno);
			goto OkExit;
		
		}else if(g_reply_int==0){
			memcpy(result,"1",1);
			vtcp_log("[%s][%d]账号或借据号[%s]存在签约记录!!!",__FILE__,__LINE__,Acno);
		}else{
			memcpy(result,"1",1);
			sprintf(acErrMsg, "查询签约表错误 [%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
		}

	}
	vtcp_log("[%s][%d]签约表检查完成",__FILE__,__LINE__,result);	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}

/*
短信银行手续费预缴费计算公函
*/
int count_mob_fee_pre(struct mob_acct_type_c st_mob_acct_type,int counttype,int month,double fee)
{
	double true_fee_standard=0.0;
	char phones[90+1];
	struct mob_server_type_c st_mob_server_type;

	memset(&st_mob_server_type,0x00,sizeof(st_mob_server_type));
	memset(phones,0x00,sizeof(phones));
	fee=0.0;

	vtcp_log("[%s][%d]进入公函feetype=[%d],counttype=[%d],fee=[%f]",__FILE__,__LINE__,month,counttype,fee);

	/**收费处理**/
	g_reply_int=Mob_server_type_Sel(g_pub_tx.reply,&st_mob_server_type,"server_type='%s'",st_mob_acct_type.server_type);
	if(g_reply_int)
	{
			sprintf(acErrMsg, "查询服务类型表Mob_server_type错误 [%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
	}
	if(st_mob_acct_type.ac_type[0]=='A')
		true_fee_standard=st_mob_server_type.fee_standard*2.0;
	else
		true_fee_standard=st_mob_server_type.fee_standard;

	vtcp_log("[%s][%d]该笔签约信息的基准费率为true_fee_standard=[%f]!!!",__FILE__,__LINE__,true_fee_standard);

	vtcp_log("[%s][%d]该笔签约的欠费天数days=[%d],手机号数目[%d]!!!",__FILE__,__LINE__,month,strlen(st_mob_acct_type.phones)/11);
	fee=0.00;
	if(counttype == 0){
		/*日*/
		fee=true_fee_standard*((double)month)*(strlen(st_mob_acct_type.phones)/11+1)/30;
	}else if(counttype == 1){
		/*月*/
		fee=true_fee_standard*month*(strlen(st_mob_acct_type.phones)/11 +1);
	}else if(counttype == 2){
		/*季度*/
		fee=true_fee_standard*month*(strlen(st_mob_acct_type.phones)/11 +1)*3;
	}else if(counttype == 3){
		/*年*/
		fee=true_fee_standard*month*(strlen(st_mob_acct_type.phones)/11 +1)*12;
	}else if(counttype == 9) {
		/*计算50天的扣费金额*/
		fee=true_fee_standard*50*(strlen(st_mob_acct_type.phones)/11 +1)/30;
	}else{
		vtcp_log("[%s][%d]传入公函错误!!!",__FILE__,__LINE__);
	
	}

	vtcp_log("[%s][%d]手续费计算完成,fee=[%f]",__FILE__,__LINE__,fee);
	if(fee<0){
		fee=0.0;
		vtcp_log("[%s][%d]手续费为负值,进行处理,fee=[%f]",__FILE__,__LINE__,fee);
	}
	
	set_zd_double( "0410", fee);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}

/*
短信银行交易撤销是更新登记薄使用
struct mob_acct_type_c st_mob_acct_type   签约信息结构体
int counttype   0 --按日计费
				1 --按月计费
				3 --按季计费
				4 --按年计费
int  feetype	0--预算
				1--实际计费
double fee		计算结果
*/
int rollback_mob_fee(long tx_date,long trace_no)
{
	double true_fee_standard=0.0;
	int update_mob_sts=0;
	char phones[90+1];
	char tmpstr[40+1];
	struct mob_acct_type_c st_mob_acct_type;
	struct mob_acct_type_hst_c st_mob_acct_type_hst;
	struct mob_fee_mx_c st_mob_fee_mx;

	memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	memset(&st_mob_acct_type_hst,0x00,sizeof(st_mob_acct_type_hst));
	memset(&st_mob_fee_mx,0x00,sizeof(st_mob_fee_mx));
	memset(tmpstr,0x00,sizeof(tmpstr));



	vtcp_log("[%s][%d]进入公函tx_date=[%ld],trace_no=[%ld]",__FILE__,__LINE__,tx_date,trace_no);

	/**查询收费明细**/
	g_reply_int=Mob_fee_mx_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld and sts='0' ",tx_date,trace_no);
	if(g_reply_int)
	{
			sprintf(acErrMsg, "查询原短信交易流水错误[%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
	}

	g_reply_int=Mob_fee_mx_Fet_Upd(&st_mob_fee_mx,g_pub_tx.reply);
	if(g_reply_int)
	{
			if(g_reply_int==100){
				sprintf(acErrMsg, "未找到短信银行服务缴费信息[%d],程序直接返回成功!", g_reply_int);
				WRITEMSG
				goto OkExit;
			}
			sprintf(acErrMsg, "打开原短信交易流水错误[%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
	}

	if(!memcmp(st_mob_fee_mx.tx_code,"8891",4)){
		/*为解约交易*/
		g_reply_int=Mob_acct_type_hst_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld  and sts='0' ",tx_date,trace_no);
		if(g_reply_int)
		{
				sprintf(acErrMsg, "查询原短信交易流水错误[%d]", g_reply_int);
				WRITEMSG
				goto ErrExit;
		}

		g_reply_int=Mob_acct_type_hst_Fet_Upd(&st_mob_acct_type_hst,g_pub_tx.reply);
		if(g_reply_int)
		{
				sprintf(acErrMsg, "打开原短信交易流水错误[%d]", g_reply_int);
				WRITEMSG
				goto ErrExit;
		}

		if(!memcmp(st_mob_acct_type_hst.opr_sts,"1",1)){
			vtcp_log("[%s][%d]原交易为解约opr_sts=[%s]!!!",__FILE__,__LINE__,st_mob_acct_type_hst.opr_sts);
			st_mob_acct_type.tx_date=st_mob_acct_type_hst.ori_tx_date;
			memcpy(st_mob_acct_type.ac_no,st_mob_acct_type_hst.ac_no,sizeof(st_mob_acct_type.ac_no));
			memcpy(st_mob_acct_type.mobile,st_mob_acct_type_hst.mobile,sizeof(st_mob_acct_type.mobile));
			st_mob_acct_type.ac_id=st_mob_acct_type_hst.ac_id;
			st_mob_acct_type.lst_fee_date=st_mob_acct_type_hst.lst_fee_date;
			st_mob_acct_type.ac_seqn=st_mob_acct_type_hst.ac_seqn;

			memcpy(st_mob_acct_type.server_type,st_mob_acct_type_hst.server_type,sizeof(st_mob_acct_type.server_type));
			memcpy(st_mob_acct_type.ac_type,st_mob_acct_type_hst.ac_type,sizeof(st_mob_acct_type.ac_type));
			memcpy(st_mob_acct_type.br_no,st_mob_acct_type_hst.lst_br_no,sizeof(st_mob_acct_type.br_no));

			memcpy(st_mob_acct_type.tel,st_mob_acct_type_hst.lst_tel,sizeof(st_mob_acct_type.tel));
			memcpy(st_mob_acct_type.mob_sts,st_mob_acct_type_hst.mob_sts,sizeof(st_mob_acct_type.mob_sts));
			memcpy(st_mob_acct_type.sfbz,st_mob_acct_type_hst.sfbz,sizeof(st_mob_acct_type.sfbz));
			memcpy(st_mob_acct_type.phones,st_mob_acct_type_hst.phones,sizeof(st_mob_acct_type.phones));
			memcpy(st_mob_acct_type.filler,st_mob_acct_type_hst.filler,sizeof(st_mob_acct_type.filler));


			update_mob_sts=3;	/*不需要进行更新,已经还原原数据了*/

			g_reply_int=Mob_acct_type_Ins(st_mob_acct_type,g_pub_tx.reply);
			if(g_reply_int)
			{
				sprintf(acErrMsg, "插入mob_acct_type [%d]错误", g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
		}

		st_mob_acct_type_hst.mob_sts[0]='1';
		sprintf(st_mob_acct_type_hst.filler1,"%ld",g_pub_tx.trace_no);
		g_reply_int=Mob_acct_type_hst_Upd_Upd(st_mob_acct_type_hst,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg, "更新Mob_fee_mx [%d]错误", g_reply_int);
			WRITEMSG
			Mob_acct_type_Clo_Sel();
			goto ErrExit;
		}
	
	}else if(!memcmp(st_mob_fee_mx.tx_code,"8886",4)){
		/**原交易为续费或者为预缴费**/
		if(!memcmp(st_mob_fee_mx.memo,"短信服务续约",12)){

			update_mob_sts=1;
			vtcp_log("[%s][%d]原交易为短信续约,需要修改原签约状态,memo=[%s],update_mob_sts=[%d]!!!",\
				__FILE__,__LINE__,st_mob_fee_mx.memo,update_mob_sts);
		
		}
	}

	if(update_mob_sts!=3){
		/*更新原签约信息*/
		vtcp_log("[%s][%d]开始更新原签约信息update_mob_sts=[%d]!!!",__FILE__,__LINE__,update_mob_sts);
		zip_space(st_mob_fee_mx.sign_ac_no);
		if(strlen(st_mob_fee_mx.sign_ac_no)>19)
			g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"filler='%s' and mob_sts='1' ",st_mob_fee_mx.sign_ac_no);
		else
			g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and mob_sts='1' ",st_mob_fee_mx.sign_ac_no);
		if(g_reply_int)
		{
				sprintf(acErrMsg, "查询签约信息错误[%d]", g_reply_int);
				WRITEMSG
				goto ErrExit;
		}

		g_reply_int=Mob_acct_type_Fet_Upd(&st_mob_acct_type,g_pub_tx.reply);
		if(g_reply_int)
		{
				sprintf(acErrMsg, "打开签约信息数据错误[%d]", g_reply_int);
				WRITEMSG
				goto ErrExit;
		}
		st_mob_acct_type.lst_fee_date=st_mob_fee_mx.lst_fee_date;
		if(update_mob_sts==1){
			st_mob_acct_type.mob_sts[0]='2';
		}
		g_reply_int=Mob_acct_type_Upd_Upd(st_mob_acct_type,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg, "更新Mob_fee_mx [%d]错误", g_reply_int);
			WRITEMSG
			Mob_acct_type_Clo_Sel();
			goto ErrExit;
		}

	}

	st_mob_fee_mx.sts[0]='1';
	sprintf(st_mob_fee_mx.filler1,"%ld",g_pub_tx.trace_no);
	zip_space(st_mob_fee_mx.memo);
	sprintf(tmpstr,"已被流水%ld撤销",g_pub_tx.trace_no);
	strcat(st_mob_fee_mx.memo,tmpstr);
    g_reply_int=Mob_fee_mx_Upd_Upd(st_mob_fee_mx,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "更新Mob_fee_mx [%d]错误", g_reply_int);
		WRITEMSG
		Mob_acct_type_Clo_Sel();
		goto ErrExit;
	}

	Mob_fee_mx_Clo_Sel();
	Mob_acct_type_hst_Clo_Sel();
	Mob_acct_type_Clo_Sel();


	vtcp_log("[%s][%d]短信撤销完成!!!",__FILE__,__LINE__);


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    return (0);
ErrExit:
    return g_reply_int;
}