/******************************************************************************/
/*  table name   : mob_fee_mx   (短信扣费明细)                           */
/*  table index  : mob_fee_mx_idx                                          */
/*  create by    : 侯毅                                      */
/*  modified by wangxd@20101225					*/
/*  变更说明:新增扣费柜员,交易码,扣费说明,上次扣收日,交易码，签约账号及保留部分预留字段					*/
/******************************************************************************/

drop table mob_fee_mx;
drop index mob_fee_mx_idx;
create table mob_fee_mx(
	tx_date       number,          /*交易日期*/
	tel	      varchar2(6),       /*交易柜员*/
	trace_no      number,          /*系统流水*/
	tx_code	      varchar2(4),	/*交易码*/
	ct_ind	      varchar2(2),	/*现转标志 1--现金 2--转账*/
	br_no         varchar2(5),       /*网点号*/
	ac_no         varchar2(24),      /*账号*/
	name          varchar2(60),      /*户名*/
	sign_ac_no    varchar2(30),	/*签约帐号*/
	server_type   varchar2(4),       /*服务类型*/ 
	amt_should    number(16,2),    /*本期应收*/
	amt_fact      number(16,2),    /*实收*/
	cnt           number,          /*欠费期数*/
	lst_fee_date  number,		/*上次扣收日*/
	print_cnt     number,		/*打印次数*/
	memo	      varchar2(100),	/*扣费说明*/
	sts	      varchar2(2),	/*明细状态*/
	filler1	      varchar2(20),	/*备用字段1   冲正流水号*/
	filler2	      varchar2(30),	/*备用字段2*/
	filler3	      varchar2(50),	/*备用字段3*/
	filler4	      varchar2(100)	/*备用字段4*/
)tablespace users;
create  index mob_fee_mx_idx on mob_fee_mx ( tx_date,ac_no,trace_no ) ;


