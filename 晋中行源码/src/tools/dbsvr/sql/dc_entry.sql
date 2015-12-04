drop index dc_entry_1;
drop  table dc_entry;
create table dc_entry(
	entry_code varchar2(6),
	seqn number(6),
	cur_no varchar2(4),
	acc_hrt varchar2(5),
	dc_ind varchar2(4),
	use_br_ind varchar2(1),
	ct_type varchar2(1),
	ct_ind varchar2(4),
	amt_no varchar2(40),
	brf varchar2(20) 
)tablespace users;
create	 unique index dc_entry_1 on dc_entry( entry_code,seqn)tablespace indx;
