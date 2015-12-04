drop index ln_verify_reg_1;
drop index ln_verify_reg_2;
drop table ln_verify_reg;
create table ln_verify_reg(
	ac_no		varchar2(21),
	ac_id		number(9),
	ac_seqn		number(9),
	name		varchar2(51),
	opn_brno 	varchar2(6),
	bal_amt		number(16,2),
	in_lo_amt 	number(16,2),
	out_lo_amt 	number(16,2),
	cmpd_lo_amtt	number(16,2),
	rate		number(8,5),
	over_rate	number(8,5),
	fine_rate	number(8,5),
	opn_date	number(9),
	mtr_date	number(9),
	verify_date	number(9),
	verify_trace_no	number(9),
	verify_brno	varchar2(6),
	verify_tel	varchar2(5)
)tablespace users;
create	unique index ln_verify_reg_1 on ln_verify_reg(ac_no) tablespace indx;
create	index	ln_verify_reg_2  on ln_verify_reg(ac_id,ac_seqn)tablespace indx;
