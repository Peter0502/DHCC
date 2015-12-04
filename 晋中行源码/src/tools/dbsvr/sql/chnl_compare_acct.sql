/*对账登记薄*/
drop  index chnl_compare_acct_1;
drop  table chnl_compare_acct;
create table chnl_compare_acct(
	detail_no      number(9)     not null,  /*明细编号:从1开始+1*/
	un_marking     varchar(32)   not null,	/*唯一标识*/
	syscode        varchar(5)    not null,  /*系统编号*/
	com_date       varchar(9)    not null,  /*对账日期*/
	code_type      varchar(6)    not null,  /*交易类型*/
	wt_date        varchar(9)    not null,  /*委托日期*/
	orderno		   number(9)     not null,	/*支付序号*/
	pt_date        number(9)     not null,  /*平台日期*/
	pt_trace_no    number(9)     not null,  /*平台流水*/
	or_br_no	   varchar(14)   not null,	/*发起行行号*/
	sndrcvflag	   varchar(5)    not null,	/*往来标示*/
	tx_amt         number(18,2)  not null,  /*交易金额*/
	con_marking    varchar(11)           ,	/*控制标识*/
	bor_lend       varchar(5)            ,  /*借贷标示*/
	main_acc_no	   varchar(32)           ,  /*主账号*/
	main_acc_nm	   varchar(60)           ,  /*主账号户名*/
	gd_acc_no	   varchar(32)           ,  /*过度账号*/
	snd_acc_no	   varchar(32)           ,  /*第二账号*/
	note_type      varchar(5)            ,  /*凭证种类*/
	note_no        varchar(17)           ,  /*凭证种类*/
	tx_br_no       varchar(6)            ,  /*交易机构*/
	teller         varchar(7)            ,  /*操作员号*/
	tx_date        number(9)             ,  /*核心日期*/
	trace_no       number(9)             ,  /*核心流水*/
	com_stat       varchar(2)            ,  /*对账标示 0-记账对账 1-冲正对账 2-补账 3-冲正*/
	batch_detail   varchar(2)            ,  /*记账方式：0 - 单笔 1 - 批量*/
	chrgflag       varchar(2)            ,  /*收费标识: 0 - 不收 1 - 收*/
	chrg_ind       varchar(2)            ,  /*手续费标识: 1 -现金  2 - 转账  3 - 不收*/
	chrg_amt       number(18,2)          ,  /*手续费金额*/
	chrgacno       varchar(32)           ,  /*手续费出处*/
	feeaccno       varchar(32)           ,  /*手续费收入科目*/
	com_pt_date    number(9)             ,  /*对账平台日期*/
	com_swno       number(9)             ,  /*对账平台流水*/
	gz_br_no       varchar(6)            ,  /*挂账机构*/
	gz_acct_no     varchar(32)           ,  /*挂账账号*/
	gz_reasen      varchar(3)            ,  /*挂账原因:*/
	com_flag       varchar(2)            ,  /*对账标识:0 - 对账平 1 - 对账错*/
	com_result     varchar(60)              /*对账结果描述*/
);
create  unique index chnl_compare_acct_1 on chnl_compare_acct(un_marking,syscode,com_date);



