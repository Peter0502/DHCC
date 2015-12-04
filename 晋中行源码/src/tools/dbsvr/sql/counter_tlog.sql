drop index counter_tlog_1;
drop  table counter_tlog;
create table counter_tlog(
	platform_date number(9),   /**ƽ̨����**/
	ptrace_no number(13),      /**ƽ̨��ˮ**/
	Tx_date number(9),      /**��������**/
	Trace_no number(9),      /**������ˮ**/
  Tx_amt number(16,2),       /**���׽��**/
	Main_ac_no varchar2(19),       /**���˻�**/
  dc_ind varchar2(1),        /***�����ʶ**/
	ct_ind varchar2(1),        /**��ת��ʶ**/
	App_ac_no varchar2(19)     /**�ڶ��˺�**/
)tablespace users;
create	 unique index counter_tlog_1 on counter_tlog( platform_date,ptrace_no)tablespace indx;

