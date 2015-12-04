drop index counter_tlog_1;
drop  table counter_tlog;
create table counter_tlog(
	platform_date number(9),   /**平台日期**/
	ptrace_no number(13),      /**平台流水**/
	Tx_date number(9),      /**核心日期**/
	Trace_no number(9),      /**核心流水**/
  Tx_amt number(16,2),       /**交易金额**/
	Main_ac_no varchar2(19),       /**主账户**/
  dc_ind varchar2(1),        /***借贷标识**/
	ct_ind varchar2(1),        /**现转标识**/
	App_ac_no varchar2(19)     /**第二账号**/
)tablespace users;
create	 unique index counter_tlog_1 on counter_tlog( platform_date,ptrace_no)tablespace indx;

