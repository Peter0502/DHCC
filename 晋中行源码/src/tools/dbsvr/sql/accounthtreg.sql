drop index accounthtreg_idx;
drop  table accounthtreg;
create table accounthtreg(
	br_no varchar2(5),
	trace_no number(9),
	seqno number(9),
	change_no varchar2(4),
	br_ame varchar2(60),
	note_type varchar2(3),
	note_no varchar2(16),
	pay_ac_no varchar2(32),
	pay_name varchar2(60),
	amt number(16,2),
	cash_ac_no varchar2(32),
	cash_name varchar2(60),
	cash_opn_br_no varchar2(12),
	cash_chg_no varchar2(4),
	cash_opn_name varchar2(60),
	put_flag varchar2(1),
	tx_date number(9),
	tx_time number(9),
	stat varchar2(1),
	tel varchar2(4),
	chk varchar2(4),
	pwd varchar2(6) 
)tablespace users;
create	 unique index accounthtreg_idx on accounthtreg( br_no,seqno,put_flag,tx_date,trace_no)tablespace indx;
