drop index td_list_1;
drop  table td_list;
create table td_list(
	ac_id number(9),
	ac_seqn number(6),
	opn_br_no varchar2(5),
	tx_date	number(8),
	td_rate	number(8,5),
	dd_rate	number(8,5),
	flag varchar2(1),
	td_day   number(8),
	dd_day   number(8),
	td_lx	number(16,2),
	dd_lx	number(16,2),
	td_amt	number(16,2),
	dd_amt	number(16,2)
)tablespace users;
create	 unique index td_list_1 on td_list(ac_id,ac_seqn,flag)tablespace indx;
