drop index com_eod_exec_idx;
drop  table com_eod_exec;
create table com_eod_exec(
	do_seq number(6),
	sub_tx_code varchar2(4),
	prog varchar2(30),
	prog_name varchar2(20),
	br_type varchar2(10),
	exec_pd varchar2(1),
	pd_month varchar2(2),
	exec_beg varchar2(2),
	exec_end varchar2(2),
	do_type varchar2(1),
	using varchar2(1),
	holiday varchar2(1),
	finish varchar2(1),
	eoy_num varchar2(1),
	eoy_finish varchar2(1) 
)tablespace users;
create	 unique index com_eod_exec_idx on com_eod_exec( do_type,do_seq)tablespace indx;
