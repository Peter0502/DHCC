drop  table mo_chg_bal;
create table mo_chg_bal(
	br_no varchar2(5),
	ac_id number(9),
	tx_date number(9),
	ac_seqn number(9),
	y_bal number(16,2),
	by_bal number(16,2),
	chg_bal number(16,2) 
)tablespace users;
