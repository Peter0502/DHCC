drop index mo_stok_rebuy_idx;
drop  table mo_stok_rebuy_prot;
create table mo_stok_rebuy_prot(
	pact_no varchar2(20),
	br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	buy_type varchar2(1),
	tcht_type varchar2(20),
	par_value number(16,2),
	bal number(16,2),
	sign_date number(9),
	matr_date number(9),
	term number(6),
	pledge_rate number(8,5),
	rate number(8,5),
	over_rate number(8,5),
	opt_acno varchar2(24),
	opt_name varchar2(50),
	reg_date number(9),
	trace_no number(9),
	brf varchar2(40),
	sts varchar2(1),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
create	 unique index mo_stok_rebuy_idx on mo_stok_rebuy_prot( pact_no)tablespace indx;
