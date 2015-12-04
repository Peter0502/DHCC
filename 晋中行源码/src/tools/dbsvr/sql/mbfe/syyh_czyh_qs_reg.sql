/* *********************************
* 商业银行与村镇银行资金清算表
*
*
**********************************/
drop  table syyh_czyh_qs_reg;
create table syyh_czyh_qs_reg(
	tx_date number(8),  /* 清算日期*/
	flag    varchar2(1), /* 1-大额 ，2-小额 */
	trace_no number(9),  /* 对账流水号 */
	tx_code varchar2(4),
	wz_amt  number(16,2), /* 村镇银行往帐金额 */
	lz_amt  number(16,2), /* 村镇银行来帐金额 */
	zc_amt  number(16,2), /* 清算金额,扎差金额*/
	tel     varchar2(7) , /* 清算操作员 */
	qs_trace_no number(9),/* 清算流水号 */
	qs_time     number(9),/* 清算时间 */
	qs_flag     varchar2(1), /* 清算标志 1-未清算，不可下班，2-已清算 */
	br_no     varchar2(5) /* 对方机构*/
)tablespace users;
create unique index syyh_czyh_qs_reg_1 on syyh_czyh_qs_reg( tx_date,flag)tablespace indx;
