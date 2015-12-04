drop  table od_pay_pln;
create table od_pay_pln(
	ac_id number(9),
	ac_seqn number(6),
	od_amt number(16,2),
	wrk_date number(9),
	end_date number(9),
	off_ind varchar2(1) 
)tablespace users;
