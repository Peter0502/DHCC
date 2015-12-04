drop index acno_hst_log_idx;
 drop index acno_hst_log_idx1;
drop  table acno_hst_log;
create table acno_hst_log(
	ac_type		varchar2(1),	/*帐户类型 1活期  2 贷款  3 定期***/
	ac_id 		number(9),	/*帐号标志号**/
	ac_seqn 	number(6),	/*帐号序号**/
	hst_cnt 	number(9),	/*帐号明细笔数**/
	tx_date		number(9),	/*交易日期**/
	br_no		varchar2(5),	/*开户机构**/
	note_type 	varchar2(3),	/*凭证种类**/
	note_no 	varchar2(16),	/*凭证号码**/
	tel 		varchar2(4),	/*操作柜员**/
	tx_amt 		number(16,2),	/*交易金额**/
	bal 		number(16,2),	/*帐户余额**/
	dc_ind		varchar2(1),	/*发生额借贷方**/
	print_flag	varchar2(1),	/*打印标志 0 未打印 1 打印 2 打印并且是已经打印的最后一条记录***/
	pagecnt		number(6),	/*该条记录打印在第几页**/
	brf 		varchar2(20),	/*摘要****/
	filler		varchar2(20),	/*备注1**/
	filler2		varchar2(40)	/*备注2**/
)tablespace users;
create	 index acno_hst_log_idx on acno_hst_log( ac_id,ac_seqn,hst_cnt) tablespace indx;
create   index acno_hst_log_idx1 on acno_hst_log(ac_id, print_flag) tablespace indx;
